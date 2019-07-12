//////////////////////////////////////////////////////////////////////////
//FluxControlObserver.cpp

#include "stdafx.h"
#include "FluxControlObserver.h"
#include "FluxControlFrameObserver.h"
#include "cpf/path_tools.h"
#include "cpf/IniFile.h"
#include "cpf/CommonMacTCPIPDecode.h"
#include "cpf/CommonEtherDecode.h"
#include "ProbeServant.h"

#include"tcpip_mgr_common_init.h"
#include "cpf/addr_tools.h"
#include "cpf/crc_tools.h"
#include "PacketIO/MyPlatRunCtrl.h"
#include "cpf/ConstructPacket.h"

#include "PacketIO/EtherioSendPacket.h"

#include "TestControlParam.h"

#include "config_center.h"
#include "flux_control_global.h"
#include "SaveBeforeSendPacketHandle.h"
#include "ConfigFacade.h"
#include "TS_interfaceCfg.h"
#include "ArpTableEx.h"

#include "ace/Shared_Memory_MM.h"
#include "ace/Shared_Memory_Pool.h"
#include "ace/Malloc.h"
#include "ace/Malloc_T.h"
///////////////////////////////////////////

int SignalMonitorEvent_Func(void * pSignalParam)
{
	g_pFluxControlFrameObserver->GetPlayRunCtrl()->SignalMonitorEvent();

	return 0;
}

CFluxControlObserver::CFluxControlObserver(void)
{
	m_pInterfaceCfgFactory = NULL;

	m_pProbeNetCom = NULL;

	m_mgr_hostorder_ip = 0;

	m_pFCRebootHandler = NULL;

	m_pLogFile = NULL;

	m_str_drv_name = "etherio.sys";

	m_pSaveBeforeSendPacketHandle = NULL;

	m_pArpTable = NULL;

	m_nUpdateCfg = 100;

	m_piTsCfg = NULL;

	m_piBackNet1 = NULL;
	m_piBackNet2 = NULL;

	return;
}

CFluxControlObserver::~CFluxControlObserver(void)
{
	delete m_piTsCfg;
	delete m_piMainNet;
	delete m_piMirror;
	delete m_piBackNet1;
	delete m_piBackNet2;
	delete m_piBackCom1;
	delete m_piBackCom2;
	delete m_piAccessMgmt;//本地管理口模块
	delete m_piStatistic;
	delete m_piSglCard1;
	delete m_piSglCard2;
	delete m_piPeerNode;
	delete m_piTsStatMgmt;
	delete m_pArpTable;

	return;
}

BOOL CFluxControlObserver::init(ACE_Log_File_Msg * plog_file,const char * drv_name)
{
	m_pLogFile = plog_file;
	m_str_drv_name = drv_name;

	GetProbeConfigPath();

	tcpip_mgr_common_init(
		CCTestControlParam_Singleton::instance()->m_live_log_size*1024,
		CCTestControlParam_Singleton::instance()->m_live_log_level);

	g_pFluxControlFrameObserver->GetPlayRunCtrl()->SignalMonitorEvent();

	/*unsigned int max_hosts = 100;
	unsigned int max_links = 10000;
	unsigned int max_server_nums = 256;
	
	{
		m_mirror_out_config.LoadFromFile(ConfigCenter_Obj()->GetProbeFullConfigName("mirror_out_config.xml").c_str());

		GetPACopyDataInfo();
	}*/

	CConfigFacade::Initialize();

	// TS 的配置
	m_piTsCfg = new CTS_CONFIG;
	if (NULL == m_piTsCfg)
	{
		return FALSE;
	}
	m_piTsCfg->f_Init(m_pLogFile);

	int ret = m_piTsCfg->f_ReadConfigFile();
	if (0 > ret)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]read config failed. return:%d\n"), ret));
		return FALSE;
	}

	ACE_Time_Value begin_time = ACE_OS::gettimeofday();

	MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]begin init eth...\n")));

	m_pInterfaceCfgFactory = new CInterfaceFactory;
	if (NULL == m_pInterfaceCfgFactory)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]m_pInterfaceCfgFactory malloc failed.\n")));
		return FALSE;
	}

	g_pFluxControlFrameObserver->GetPlayRunCtrl()->SignalMonitorEvent();

	m_pSaveBeforeSendPacketHandle = new CSaveBeforeSendPacketHandle(this);

	std::string card_info;
	if (FALSE == m_piTsCfg->f_FindAllCardName(card_info))// 列举出用到的网卡名字
	{
		return FALSE;
	} 

	int error_code = 0;
	if( !m_pInterfaceCfgFactory->init(ConfigCenter_Obj()->m_str_probe_config_path.c_str(),
		CCTestControlParam_Singleton::instance()->m_buffer_pkt_thread,
		error_code,card_info,m_str_drv_name.c_str(),
		SignalMonitorEvent_Func,g_pFluxControlFrameObserver,m_pSaveBeforeSendPacketHandle) )
	{
		MY_ACE_DEBUG(m_pLogFile,
			(LM_ERROR, ACE_TEXT("[Err][%D]m_pInterfaceCfgFactory->init(%s)failed! errcode=%d,card_info=%s,drv_name=%s.\n"),
			ConfigCenter_Obj()->m_str_probe_config_path.c_str(),error_code,card_info.c_str(),m_str_drv_name.c_str()));
		return(FALSE);
	}
	MY_ACE_DEBUG(m_pLogFile,
		(LM_ERROR, ACE_TEXT("[Info][%D]init eth success! card_info=%s,total_time=%d(ms).\n"),
		card_info.c_str(), (ACE_OS::gettimeofday()-begin_time).get_msec()));

	g_pFluxControlFrameObserver->GetPlayRunCtrl()->SignalMonitorEvent();

	m_pSerialInterface = new CSerialInterface;
	if (NULL == m_pSerialInterface)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]m_pSerialInterface malloc failed.\n")));
		return FALSE;
	}
	ret = m_pSerialInterface->f_Init();
	if (0 > ret)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]m_pSerialInterface init failed. return:%d\n"), ret));
		return FALSE;
	}

	{
		if( m_mirror_out_config.m_mirror_status && 
			m_mirror_out_config.m_vt_mirror_from_card.size() > 0 )
		{
			if( !m_pInterfaceCfgFactory->FindCard(m_mirror_out_config.m_strMirrorToCard.c_str()) )
			{		
				char mirror_config_file_name[MAX_PATH]={0};

				CPF::JoinPathToPath(mirror_config_file_name,sizeof(mirror_config_file_name),
					ConfigCenter_Obj()->m_str_probe_config_path.c_str(),"mirror_out_flt.flt");

				m_mirrorout_mgr.start_mirror(m_pInterfaceCfgFactory,
					m_mirror_out_config,mirror_config_file_name);
			}
			else
			{
				MY_ACE_DEBUG(m_pLogFile,
					(LM_ERROR,
					ACE_TEXT("\n镜像输出口 %s 被占用！！\n"),
					m_mirror_out_config.m_strMirrorToCard.c_str()
					));
			}
		}
	}
	
	if (GetInterfaceCfg() == NULL)
	{ 
		return FALSE; 
	}

	//ARP表
	m_pArpTable = new CArpTableEx((CTS_InterfaceCfg *)GetInterfaceCfg());
	if (NULL == m_pArpTable)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]m_pArpTable malloc failed.\n")));
		return FALSE;
	}
	else
	{
		m_pArpTable->init(1000, 60);
	}

	//
	ret = TS_init((CTS_InterfaceCfg *)GetInterfaceCfg(), m_pArpTable);
	if (0 > ret)
	{ 
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]TS_init failed. return:%d\n"), ret));
		return FALSE; 
	}
	MY_ACE_DEBUG(m_pLogFile, (LM_INFO, ACE_TEXT("[Info][%D]TS_init success.\n")));

	//
	LoadProbeNetcom();

	//
	m_pSerialInterface->m_piSerialDataSend->f_StartSend();
	
	return true;
}

void CFluxControlObserver::fin()
{
	UnLoadProbeNetcom();

	if( m_pInterfaceCfgFactory )
	{
		m_pInterfaceCfgFactory->fin();
		delete m_pInterfaceCfgFactory;
		m_pInterfaceCfgFactory = NULL;
	}

	if( m_pSaveBeforeSendPacketHandle )
	{
		delete m_pSaveBeforeSendPacketHandle;
		m_pSaveBeforeSendPacketHandle = NULL;
	}

	tcpip_mgr_common_fini();

	m_mirrorout_mgr.stop_mirror();

	CConfigFacade::UnInitialize();

	return;
}

BOOL CFluxControlObserver::OnStart()
{
	if( GetInterfaceCfg() && GetInterfaceCfg()->GetWriteDataStream() )
	{
		GetInterfaceCfg()->GetWriteDataStream()->StartToWrite();
	}

	m_mirrorout_mgr.Start_To_Write();

	return true;
}

BOOL CFluxControlObserver::OnStop(int type)
{
	if( GetInterfaceCfg()
		&& GetInterfaceCfg()->GetWriteDataStream() )
	{
		GetInterfaceCfg()->GetWriteDataStream()->StopWriting();
	}

	m_mirrorout_mgr.Stop_Writting();

	return true;

}

BOOL CFluxControlObserver::PushPacket(PACKET_LEN_TYPE type, RECV_PACKET& packet)
{	
	int ret;
	
	if (PACKET_OK == type)
	{
		//PACKET_INFO_EX packet_info;

		m_tagPacketInfo.reset();
		//m_tagPacketInfo.pkt_len_type = type;
		m_tagPacketInfo.packet = &packet;
		//m_tagPacketInfo.tsCurPkt = packet.pHeaderinfo->stamp;
		//m_tagPacketInfo.vm_id = 0;

		CCommonMacTCPIPDecode::MacDecodeToTransport
		(
			(BYTE *)packet.pPacket,
			packet.nPktlen,
			m_tagPacketInfo.tcpipContext,
			true,
			false
		);
		
		if (TRUE == CCommonEtherDecode::MAC_DECODE_INFO::IsARP(m_tagPacketInfo.tcpipContext.dwProtoOverMac))
		{		
			CCommonEtherDecode::ARPProtocolDecode
			(
				m_tagPacketInfo.tcpipContext.pIPHeader,
				m_tagPacketInfo.tcpipContext.dwIPPacketLen,
				m_tagPacketInfo.m_arp_decode_info
			);
		}

		TS_HandlePacket(m_tagPacketInfo);
	}
	else if (PACKET_TIMER == type)
	{
		switch (m_nUpdateCfg)
		{
			case 0:
				m_nUpdateCfg++;
				break;

			case 1:
				ret = TS_UpdateConfig();
				if (0 > ret)
				{
					m_nUpdateCfg = 101;
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Err][%D]update config failed. return:%d\n"), ret));
				}
				else
				{
					m_nUpdateCfg = 98;
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]update config success.\n")));
				}
				break;

			case 98:
				m_nUpdateCfg++;
				break;
				
			case 99:
				m_nUpdateCfg++;
				break;

			case 100:
				TS_HandleTimer(packet);
				break;

			case 101:
				break;

			default:
				break;
		}
	}
	else if (PACKET_SERIAL_OK == type)
	{
		TS_HandleSerialPacket(packet);
	}

	return TRUE;
}

int CFluxControlObserver::TS_init(CTS_InterfaceCfg * pInterfaceCfg, CArpTableEx * pArpTable)
{
	ACE_OS::system("echo 1 >/proc/sys/net/ipv4/icmp_echo_ignore_all");
	ACE_OS::system("echo 1 >/proc/sys/net/ipv4/conf/all/arp_ignore");
	ACE_OS::system("echo 2 >/proc/sys/net/ipv4/conf/all/arp_announce");

	int ret = 0;
	
	if (L2_MODE == m_piTsCfg->m_iGlobalCfg.m_eNetworkMode)
	{		
		m_piMainNet = new CTS_MainNetL2;
		ret = m_piMainNet->f_ModInit(pInterfaceCfg, m_piTsCfg, m_pLogFile, pArpTable);
		if (FALSE == ret)
		{ 
			return -1; 
		}
		
		m_piMirror = new CTS_MirrorL2;
		ret = m_piMirror->f_ModInit(m_piTsCfg, m_pLogFile);
		if (FALSE == ret)
		{ 
			return -2; 
		}

		m_piBackNet1 = new CTS_BackNetL2EoSgl1;
		ret = m_piBackNet1->f_ModInit(pInterfaceCfg, m_piTsCfg, m_pLogFile);
		if (FALSE == ret)
		{ 
			return -3; 
		}

		if ("eth4" == m_piTsCfg->m_iBackNet2Cfg.m_strCardName)
		{
			m_piBackNet2 = new CTS_BackNetL2EoSgl2;
		}
		else
		{
			if (0 == m_piTsCfg->m_iBackNet2Cfg.m_hlGateway)
			{
				m_piBackNet2 = new CTS_BackNetL2EoL2;
			}
			else
			{
				m_piBackNet2 = new CTS_BackNetL2EoL3;
			}
		}
		ret = m_piBackNet2->f_ModInit(pInterfaceCfg, m_piTsCfg, m_pLogFile);
		if (FALSE == ret)
		{ 
			return -5; 
		}

		m_piBackCom1 = new CTS_BackComL2;
		ret = m_piBackCom1->f_ModInit(m_pSerialInterface, m_piTsCfg, &(m_piTsCfg->m_iBackCom1Cfg), m_pLogFile);
		if (FALSE == ret)
		{
			return -6;
		}

		m_piBackCom2 = new CTS_BackComL2;
		ret = m_piBackCom2->f_ModInit(m_pSerialInterface, m_piTsCfg, &(m_piTsCfg->m_iBackCom2Cfg), m_pLogFile);
		if (FALSE == ret)
		{
			return -7;
		}
	}
	else if (L3_MODE == m_piTsCfg->m_iGlobalCfg.m_eNetworkMode)
	{
		m_piMainNet = new CTS_MainNetL3;
		ret = m_piMainNet->f_ModInit(pInterfaceCfg, m_piTsCfg, m_pLogFile, pArpTable);
		if (FALSE == ret)
		{ 
			return -1; 
		}
		
		m_piMirror = new CTS_MirrorL3;
		ret = m_piMirror->f_ModInit(m_piTsCfg, m_pLogFile);
		if (FALSE == ret)
		{ 
			return -2; 
		}

		m_piBackNet1 = new CTS_BackNetL3EoSgl1;
		ret = m_piBackNet1->f_ModInit(pInterfaceCfg, m_piTsCfg, m_pLogFile);
		if (FALSE == ret)
		{ 
			return -3; 
		}

		if ("eth4" == m_piTsCfg->m_iBackNet2Cfg.m_strCardName)
		{
			m_piBackNet2 = new CTS_BackNetL3EoSgl2;
		}
		else
		{
			if (0 == m_piTsCfg->m_iBackNet2Cfg.m_hlGateway)
			{
				m_piBackNet2 = new CTS_BackNetL3EoL2;
			}
			else
			{
				m_piBackNet2 = new CTS_BackNetL3EoL3;
			}
		}
		ret = m_piBackNet2->f_ModInit(pInterfaceCfg, m_piTsCfg, m_pLogFile);
		if (FALSE == ret)
		{ 
			return -5; 
		}

		m_piBackCom1 = new CTS_BackComL3;
		ret = m_piBackCom1->f_ModInit(m_pSerialInterface, m_piTsCfg, &(m_piTsCfg->m_iBackCom1Cfg), m_pLogFile);
		if (FALSE == ret)
		{
			return -6;
		}

		m_piBackCom2 = new CTS_BackComL3;
		ret = m_piBackCom2->f_ModInit(m_pSerialInterface, m_piTsCfg, &(m_piTsCfg->m_iBackCom2Cfg), m_pLogFile);
		if (FALSE == ret)
		{
			return -7;
		}
	}
	else if (LAN_MODE == m_piTsCfg->m_iGlobalCfg.m_eNetworkMode)
	{
		//LAN模式下强制为主断备传
		m_piTsCfg->m_iGlobalCfg.m_eForwardMode = ASYNC_MODE;
		
		m_piMainNet = new CTS_MainNetLan;
		ret = m_piMainNet->f_ModInit(pInterfaceCfg, m_piTsCfg, m_pLogFile, pArpTable);
		if (FALSE == ret)
		{ 
			return -1; 
		}
		
		m_piMirror = new CTS_MirrorLan;
		ret = m_piMirror->f_ModInit(m_piTsCfg, m_pLogFile);
		if (FALSE == ret)
		{ 
			return -2; 
		}

		m_piBackNet1 = new CTS_BackNetLanEoSgl1;
		ret = m_piBackNet1->f_ModInit(pInterfaceCfg, m_piTsCfg, m_pLogFile);
		if (FALSE == ret)
		{ 
			return -3; 
		}

		if ("eth4" == m_piTsCfg->m_iBackNet2Cfg.m_strCardName)
		{
			m_piBackNet2 = new CTS_BackNetLanEoSgl2;
		}
		else
		{
			m_piBackNet2 = new CTS_BackNetLanEoL2;
		}
		ret = m_piBackNet2->f_ModInit(pInterfaceCfg, m_piTsCfg, m_pLogFile);
		if (FALSE == ret)
		{ 
			return -5; 
		}

		m_piBackCom1 = new CTS_BackComLan;
		ret = m_piBackCom1->f_ModInit(m_pSerialInterface, m_piTsCfg, &(m_piTsCfg->m_iBackCom1Cfg), m_pLogFile);
		if (FALSE == ret)
		{
			return -6;
		}

		m_piBackCom2 = new CTS_BackComLan;
		ret = m_piBackCom2->f_ModInit(m_pSerialInterface, m_piTsCfg, &(m_piTsCfg->m_iBackCom2Cfg), m_pLogFile);
		if (FALSE == ret)
		{
			return -7;
		}
	}

	//流量统计
	m_piStatistic = new CTS_Statistic;
	ret = m_piStatistic->f_ModInit(m_piTsCfg);
	if (FALSE == ret)
	{ 
		return -11; 
	}

	//E1板管理
	m_piSglCard1 = new CTS_SglCard;
	ret = m_piSglCard1->f_ModInit(m_piTsCfg, &(m_piTsCfg->m_iSglCard1), m_pLogFile);
	if (FALSE == ret)
	{
		return -12;
	}

	//E1板管理
	m_piSglCard2 = new CTS_SglCard;
	ret = m_piSglCard2->f_ModInit(m_piTsCfg, &(m_piTsCfg->m_iSglCard2), m_pLogFile);
	if (FALSE ==ret)
	{
		return -13;
	}

	m_piPeerNode = new CTS_PeerNode;
	ret = m_piPeerNode->f_ModInit(m_piTsCfg, m_pLogFile);
	if (FALSE == ret)
	{
		return -14;
	}

	//本地访问管理
	m_piAccessMgmt = new CTS_AccessMgmt;
	ret = m_piAccessMgmt->f_ModInit(pInterfaceCfg, m_piTsCfg, m_pLogFile);
	if (FALSE == ret)
	{ 
		return -15; 
	}
	
	//状态管理
	m_piTsStatMgmt = new CTS_StatMgmt;
	if (NULL == m_piTsStatMgmt)
	{
		return -16;
	}
	else
	{
		ret = m_piTsStatMgmt->f_ModInit(this);
		if (0 > ret)
		{
			return -17;
		}
		ret = m_piTsStatMgmt->Start();
		if (0 > ret)
		{
			return -18;
		}
	}
	
	return 0;
}

int CFluxControlObserver::TS_UpdateConfig(void)
{
	CTS_CONFIG* piNewTsCfg = new CTS_CONFIG;
	if (NULL == piNewTsCfg)
	{
		return -1;
	}

	piNewTsCfg->f_Init(m_pLogFile);

	int ret = piNewTsCfg->f_ReadConfigFile();
	if (0 > ret)
	{
		delete piNewTsCfg;
		
		return -2;
	}

	if (m_piTsCfg->m_iGlobalCfg != piNewTsCfg->m_iGlobalCfg)
	{
		//reboot(RB_AUTOBOOT);
		if (m_piTsCfg->m_iGlobalCfg.m_eNetworkMode != piNewTsCfg->m_iGlobalCfg.m_eNetworkMode)
		{
			MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]system reboot: network mode changed.\n")));
		}

		if (m_piTsCfg->m_iGlobalCfg.m_eForwardMode != piNewTsCfg->m_iGlobalCfg.m_eForwardMode)
		{
			MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]system reboot: forward mode changed.\n")));
		}

		if (m_piTsCfg->m_iGlobalCfg.m_nQueueSize != piNewTsCfg->m_iGlobalCfg.m_nQueueSize)
		{
			MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]system reboot: queue size changed.\n")));
		}
		
		ACE_OS::system("reboot -p");
	}

	m_piTsStatMgmt->Pause();
	
	m_piMirror->f_ModUpdateCfg(piNewTsCfg);

	m_piMainNet->f_ModUpdateCfg(piNewTsCfg);

	m_piBackNet1->f_ModUpdateCfg(piNewTsCfg);
	m_piBackNet2->f_ModUpdateCfg(piNewTsCfg);
	m_piBackCom1->f_ModUpdateCfg(piNewTsCfg, &(piNewTsCfg->m_iBackCom1Cfg));
	m_piBackCom2->f_ModUpdateCfg(piNewTsCfg, &(piNewTsCfg->m_iBackCom2Cfg));
	
	m_piStatistic->f_ModUpdateCfg(piNewTsCfg);
	
	m_piSglCard1->f_ModUpdateCfg(piNewTsCfg, &(piNewTsCfg->m_iSglCard1));
	m_piSglCard2->f_ModUpdateCfg(piNewTsCfg, &(piNewTsCfg->m_iSglCard2));

	m_piPeerNode->f_ModUpdateCfg(piNewTsCfg);

	m_piAccessMgmt->f_ModUpdateCfg(piNewTsCfg);

	m_piTsStatMgmt->f_ModUpdateCfg(piNewTsCfg);
	m_piTsStatMgmt->Continue();
	
	delete m_piTsCfg;
	m_piTsCfg = piNewTsCfg;
	
	return 0;
}

int CFluxControlObserver::TS_ForwardIpOut(PACKET_INFO_EX& packet_info)
{
	switch (packet_info.tcpipContext.dwIndexProtocl)
	{
		case CCommonIPv4Decode::INDEX_PID_IPv4_ICMP:
		case CCommonIPv4Decode::INDEX_PID_IPv4_IPIGMP:
			break;

		case CCommonIPv4Decode::INDEX_PID_IPv4_TCP:
		case CCommonIPv4Decode::INDEX_PID_IPv4_UDP:
			//路由协议报文不转发
			if ((CCommonUDPDecode::PID_UDPPORT_BGP == packet_info.tcpipContext.conn_addr.server.wdPort)
				|| (CCommonUDPDecode::PID_UDPPORT_RIP == packet_info.tcpipContext.conn_addr.server.wdPort))
			{
				return FALSE;
			}
			break;

		default://非TCP/UDP(比如INDEX_PID_IPv4_IPOSPF)不转发
			return FALSE;
			break;
	}
	
	ACE_UINT32 peer_index = 0;
	if (TRUE != m_piTsCfg->f_IsPeerHostIp(packet_info.tcpipContext.conn_addr.server.dwIP, peer_index))// 目的IP必须是认识的远程IP
	{
		return FALSE;
	}
	if (TRUE != m_piTsCfg->f_IsLocalHostIp(packet_info.tcpipContext.conn_addr.client.dwIP))// 源IP必须是内网IP
	{
		return FALSE;
	}

	BOOL result;
	if (peer_index)//单播
	{
		peer_index--;
		switch (m_piTsCfg->m_iGlobalCfg.m_eForwardMode)
		{
			case ASYNC_MODE:
				if (BACK_NET1_RUN == m_piPeerNode->m_vtPeerStat[peer_index])
				{
					m_piBackNet1->f_ForwardIp(packet_info, peer_index);
					m_piBackNet2->f_PushPktQueue(packet_info, peer_index);
				}
				else if (BACK_NET2_RUN == m_piPeerNode->m_vtPeerStat[peer_index])
				{
					m_piBackNet2->f_ForwardIp(packet_info, peer_index);
				}
				else if (BACK_COM1_RUN == m_piPeerNode->m_vtPeerStat[peer_index])
				{
					m_piBackCom1->f_ForwardIp(packet_info, peer_index);
				}
				else if (BACK_COM2_RUN == m_piPeerNode->m_vtPeerStat[peer_index])
				{
					m_piBackCom2->f_ForwardIp(packet_info, peer_index);
				}
				else if (MAIN_NET_RUN == m_piPeerNode->m_vtPeerStat[peer_index])
				{
					m_piBackNet1->f_PushPktQueue(packet_info, peer_index);
					m_piBackNet2->f_PushPktQueue(packet_info, peer_index);
				}
				else
				{
					m_piBackNet1->f_ForwardIp(packet_info, peer_index);
					m_piBackNet2->f_ForwardIp(packet_info, peer_index);
					//m_piBackCom1->f_ForwardIp(packet_info, peer_index);
					//m_piBackCom2->f_ForwardIp(packet_info, peer_index);
				}
				break;

			case SYNC_MODE:
				result = m_piBackNet1->f_ForwardIp(packet_info, peer_index);
				/*if (TRUE == result)
				{
					break;
				}*/
				result = m_piBackNet2->f_ForwardIp(packet_info, peer_index);
				/*if (TRUE == result)
				{
					break;
				}*/
				result = m_piBackCom1->f_ForwardIp(packet_info, peer_index);
				/*if (TRUE == result)
				{
					break;
				}*/
				result = m_piBackCom2->f_ForwardIp(packet_info, peer_index);
				/*if (TRUE == result)
				{
					break;
				}*/
				break;

			default:
				break;
		}
		
		m_piStatistic->f_FlowStatistic(packet_info);
	}
	else//组播
	{
		size_t max = m_piTsCfg->m_vtPeerCfg.size();
		switch (m_piTsCfg->m_iGlobalCfg.m_eForwardMode)
		{
			case ASYNC_MODE:
				for (size_t i=0; i<max; i++)
				{
					if (BACK_NET1_RUN == m_piPeerNode->m_vtPeerStat[peer_index])
					{
						m_piBackNet1->f_MulticastIp(packet_info);
					}
					else if (BACK_NET2_RUN == m_piPeerNode->m_vtPeerStat[peer_index])
					{
						m_piBackNet2->f_MulticastIp(packet_info);
					}
					else if (BACK_COM1_RUN == m_piPeerNode->m_vtPeerStat[peer_index])
					{
						m_piBackCom1->f_MulticastIp(packet_info);
					}
					else if (BACK_COM2_RUN == m_piPeerNode->m_vtPeerStat[peer_index])
					{
						m_piBackCom2->f_MulticastIp(packet_info);
					}
				}
				break;

			default:
				break;
		}
	}
	
	return TRUE;
}

int CFluxControlObserver::TS_ForwardArpOut(PACKET_INFO_EX& packet_info)
{
	ACE_UINT32 peer_index = 0;
	if (TRUE != m_piTsCfg->f_IsPeerHostIp(ACE_NTOHL(packet_info.m_arp_decode_info.destNetOrderIPAddr), peer_index))// 目的IP必须是认识的远程IP
	{
		return FALSE;
	}
	if (TRUE != m_piTsCfg->f_IsLocalHostIp(ACE_NTOHL(packet_info.m_arp_decode_info.senderNetOrderIPAddr)))// 源IP必须是内网IP
	{
		return FALSE;
	}

	BOOL result;
	if (peer_index)
	{
		peer_index--;
		switch (m_piTsCfg->m_iGlobalCfg.m_eForwardMode)
		{
			case ASYNC_MODE:
				if (ECHO_CHECKING == m_piMainNet->m_vtPeerDct[peer_index]->m_eHostStat)
				{
					break;
				}
				else if (BACK_NET1_RUN == m_piPeerNode->m_vtPeerStat[peer_index])
				{
					m_piBackNet1->f_ForwardArp(packet_info, peer_index);
					m_piBackNet2->f_PushPktQueue(packet_info, peer_index);
				}
				else if (BACK_NET2_RUN == m_piPeerNode->m_vtPeerStat[peer_index])
				{
					m_piBackNet2->f_ForwardArp(packet_info, peer_index);
				}
				else if (BACK_COM1_RUN == m_piPeerNode->m_vtPeerStat[peer_index])
				{
					m_piBackCom1->f_ForwardArp(packet_info, peer_index);
				}
				else if (BACK_COM2_RUN == m_piPeerNode->m_vtPeerStat[peer_index])
				{
					m_piBackCom2->f_ForwardArp(packet_info, peer_index);
				}
				else
				{
					m_piBackNet1->f_PushPktQueue(packet_info, peer_index);
					m_piBackNet2->f_PushPktQueue(packet_info, peer_index);
				}
				break;

			/*case SYNC_MODE:
				result = m_piBackNet1->f_ForwardArp(packet_info, peer_index);
				if (FALSE == result)
				{
					break;
				}
				result = m_piBackNet2->f_ForwardArp(packet_info, peer_index);
				if (FALSE == result)
				{
					break;
				}
				result = m_piBackCom1->f_ForwardArp(packet_info, peer_index);
				if (FALSE == result)
				{
					break;
				}
				result = m_piBackCom2->f_ForwardArp(packet_info, peer_index);
				if (FALSE == result)
				{
					break;
				}
				break;*/

			default:
				break;
		}
	}
	
	return TRUE;
}

int CFluxControlObserver::TS_ForwardIpIn(PACKET_INFO_EX& packet_info)
{
	switch (packet_info.tcpipContext.dwIndexProtocl)
	{
		case CCommonIPv4Decode::INDEX_PID_IPv4_ICMP:
		case CCommonIPv4Decode::INDEX_PID_IPv4_IPIGMP:
			break;

		case CCommonIPv4Decode::INDEX_PID_IPv4_TCP:
		case CCommonIPv4Decode::INDEX_PID_IPv4_UDP:
			//路由协议报文不转发
			if ((CCommonUDPDecode::PID_UDPPORT_BGP == packet_info.tcpipContext.conn_addr.server.wdPort)
				|| (CCommonUDPDecode::PID_UDPPORT_RIP == packet_info.tcpipContext.conn_addr.server.wdPort))
			{
				return FALSE;
			}
			break;

		default://非TCP/UDP(比如INDEX_PID_IPv4_IPOSPF)不转发
			return FALSE;
			break;
	}
	
	if (TRUE != m_piTsCfg->f_IsLocalHostIp(packet_info.tcpipContext.conn_addr.server.dwIP))//目的
	{
		return FALSE;
	}
	ACE_UINT32 peer_index = 0;
	if (TRUE != m_piTsCfg->f_IsPeerHostIp(packet_info.tcpipContext.conn_addr.client.dwIP, peer_index))//源
	{
		return FALSE;
	}
	
	m_piMainNet->f_ForwardIp(packet_info);
	
	return TRUE;
}

int CFluxControlObserver::TS_ForwardArpIn(PACKET_INFO_EX& packet_info)
{
	if (TRUE != m_piTsCfg->f_IsLocalHostIp(ACE_NTOHL(packet_info.m_arp_decode_info.destNetOrderIPAddr)))//目的
	{
		return FALSE;
	}
	ACE_UINT32 peer_index = 0;
	if (TRUE != m_piTsCfg->f_IsPeerHostIp(ACE_NTOHL(packet_info.m_arp_decode_info.senderNetOrderIPAddr), peer_index))//源
	{
		return FALSE;
	}

	m_piMainNet->f_ForwardArp(packet_info);
	
	return TRUE;
}

int CFluxControlObserver::TS_HandlePacket(PACKET_INFO_EX& packet_info)
{
	int ret;

	if (m_piMainNet->m_nIfIndex == packet_info.packet->pHeaderinfo->btCardNo)
	{				
		if (CCommonEtherDecode::MAC_DECODE_INFO::IsIP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
		{
			ret = m_piMainNet->f_HandleIp(packet_info);
			if (TRUE == ret)
			{
				return TRUE;
			}
			
			ret = TS_ForwardIpOut(packet_info);
			if (TRUE == ret)
			{
				return TRUE;
			}
		}
		else if (CCommonEtherDecode::MAC_DECODE_INFO::IsARP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
		{
			ret = m_piMainNet->f_HandleArp(packet_info);
			if (TRUE == ret)
			{
				return TRUE;
			}

			ret = TS_ForwardArpOut(packet_info);
			if (TRUE == ret)
			{
				return TRUE;
			}
		}
	}
	else if (m_piMirror->m_nIfIndex == packet_info.packet->pHeaderinfo->btCardNo)
	{
		if (CCommonEtherDecode::MAC_DECODE_INFO::IsIP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
		{
			ret = m_piMirror->f_HandleIp(packet_info);
			if (TRUE == ret)
			{
				return TRUE;
			}
			
			ret = TS_ForwardIpOut(packet_info);
			if (TRUE == ret)
			{
				return TRUE;
			}
		}
		else if (CCommonEtherDecode::MAC_DECODE_INFO::IsARP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
		{
			ret = m_piMirror->f_HandleArp(packet_info);
			if (TRUE == ret)
			{
				return TRUE;
			}

			ret = TS_ForwardArpOut(packet_info);
			if (TRUE == ret)
			{
				return TRUE;
			}
		}
	}
	else if (m_piBackNet1->m_nIfIndex == packet_info.packet->pHeaderinfo->btCardNo)
	{		
		if (CCommonEtherDecode::MAC_DECODE_INFO::IsIP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
		{
			ret = m_piBackNet1->f_HandleIp(packet_info);
			if (TRUE == ret)
			{
				if (TRUE == m_piAccessMgmt->m_nPeerMgmt)
				{
					(CTS_InterfaceCfg *)GetInterfaceCfg()->GetWriteDataStream()->WritePacket
					(
						PACKET_OK,
						(const char *)(packet_info.packet->pPacket),
						packet_info.packet->nPktlen,
						(1<<m_piAccessMgmt->m_nIfIndex),
						1
					);
				}
				return TRUE;
			}
			
			ret = TS_ForwardIpIn(packet_info);
			if (TRUE == ret)
			{
				return TRUE;
			}
		}
		else if (CCommonEtherDecode::MAC_DECODE_INFO::IsARP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
		{
			ret = m_piBackNet1->f_HandleArp(packet_info);
			if (TRUE == ret)
			{
				if (TRUE == m_piAccessMgmt->m_nPeerMgmt)
				{
					(CTS_InterfaceCfg *)GetInterfaceCfg()->GetWriteDataStream()->WritePacket
					(
						PACKET_OK,
						(const char *)(packet_info.packet->pPacket),
						packet_info.packet->nPktlen,
						(1<<m_piAccessMgmt->m_nIfIndex),
						1
					);
				}
				return TRUE;
			}

			ret = TS_ForwardArpIn(packet_info);
			if (TRUE == ret)
			{
				return TRUE;
			}
		}
	}
	else if (m_piBackNet2->m_nIfIndex == packet_info.packet->pHeaderinfo->btCardNo)
	{
		if (CCommonEtherDecode::MAC_DECODE_INFO::IsIP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
		{
			ret = m_piBackNet2->f_HandleIp(packet_info);
			if (TRUE == ret)
			{
				if (TRUE == m_piAccessMgmt->m_nPeerMgmt)
				{
					(CTS_InterfaceCfg *)GetInterfaceCfg()->GetWriteDataStream()->WritePacket
					(
						PACKET_OK,
						(const char *)(packet_info.packet->pPacket),
						packet_info.packet->nPktlen,
						(1<<m_piAccessMgmt->m_nIfIndex),
						1
					);
				}
				return TRUE;
			}
			
			ret = TS_ForwardIpIn(packet_info);
			if (TRUE == ret)
			{
				return TRUE;
			}
		}
		else if (CCommonEtherDecode::MAC_DECODE_INFO::IsARP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
		{
			ret = m_piBackNet2->f_HandleArp(packet_info);
			if (TRUE == ret)
			{
				if (TRUE == m_piAccessMgmt->m_nPeerMgmt)
				{
					(CTS_InterfaceCfg *)GetInterfaceCfg()->GetWriteDataStream()->WritePacket
					(
						PACKET_OK,
						(const char *)(packet_info.packet->pPacket),
						packet_info.packet->nPktlen,
						(1<<m_piAccessMgmt->m_nIfIndex),
						1
					);
				}
				return TRUE;
			}

			ret = TS_ForwardArpIn(packet_info);
			if (TRUE == ret)
			{
				return TRUE;
			}
		}
	}
	else if (m_piAccessMgmt->m_nIfIndex == packet_info.packet->pHeaderinfo->btCardNo)//本地管理模块
	{
		if (CCommonEtherDecode::MAC_DECODE_INFO::IsIP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
		{
			ret = m_piAccessMgmt->f_HandleIp(packet_info);
			if (TRUE == ret)
			{
				return TRUE;
			}

			if (TRUE == m_piAccessMgmt->m_nPeerMgmt)
			{
				ACE_UINT32 peer_index = 0;
				if (TRUE == m_piBackNet1->f_IsPeerTestIP(packet_info.tcpipContext.conn_addr.server.dwIP, peer_index))// 目的IP必须是认识的远程IP
				{
					(CTS_InterfaceCfg *)GetInterfaceCfg()->GetWriteDataStream()->WritePacket
					(
						PACKET_OK,
						(const char *)(packet_info.packet->pPacket),
						packet_info.packet->nPktlen,
						(1<<m_piBackNet1->m_nIfIndex),
						1
					);
					return TRUE;
				}
				else if (TRUE == m_piBackNet2->f_IsPeerTestIP(packet_info.tcpipContext.conn_addr.server.dwIP, peer_index))
				{
					(CTS_InterfaceCfg *)GetInterfaceCfg()->GetWriteDataStream()->WritePacket
					(
						PACKET_OK,
						(const char *)(packet_info.packet->pPacket),
						packet_info.packet->nPktlen,
						(1<<m_piBackNet2->m_nIfIndex),
						1
					);
					return TRUE;
				}
			}
		}
		else if (CCommonEtherDecode::MAC_DECODE_INFO::IsARP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
		{
			//m_piAccessMgmt->f_HandleArp(packet_info);
			
			if (TRUE == m_piAccessMgmt->m_nPeerMgmt)
			{
				if (TRUE == m_piBackNet1->f_IsSubNetIp(ACE_NTOHL(packet_info.m_arp_decode_info.destNetOrderIPAddr)))
				{
					(CTS_InterfaceCfg *)GetInterfaceCfg()->GetWriteDataStream()->WritePacket
					(
						PACKET_OK,
						(const char *)(packet_info.packet->pPacket),
						packet_info.packet->nPktlen,
						(1<<m_piBackNet1->m_nIfIndex),
						1
					);
					return TRUE;
				}
				else if (TRUE == m_piBackNet2->f_IsSubNetIp(ACE_NTOHL(packet_info.m_arp_decode_info.destNetOrderIPAddr)))
				{
					(CTS_InterfaceCfg *)GetInterfaceCfg()->GetWriteDataStream()->WritePacket
					(
						PACKET_OK,
						(const char *)(packet_info.packet->pPacket),
						packet_info.packet->nPktlen,
						(1<<m_piBackNet2->m_nIfIndex),
						1
					);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

int CFluxControlObserver::TS_HandleSerialPacket(RECV_PACKET& packet)
{
	CTS_BackCom* pTsBackCom;
	int ret;

	if (m_piBackCom1->m_nComIndex == packet.pHeaderinfo->btCardNo)
	{
		pTsBackCom = m_piBackCom1;
	}
	else if (m_piBackCom2->m_nComIndex == packet.pHeaderinfo->btCardNo)
	{
		pTsBackCom = m_piBackCom2;
	}
	else
	{
		return -1;
	}

	if (60 < packet.nPktlen)
	{
		m_tagPacketInfo.reset();
		m_tagPacketInfo.packet = &packet;

		CCommonMacTCPIPDecode::MacDecodeToTransport
		(
			(BYTE *)packet.pPacket,
			packet.nPktlen,
			m_tagPacketInfo.tcpipContext,
			true,
			false
		);
		
		if (TRUE == CCommonEtherDecode::MAC_DECODE_INFO::IsIP(m_tagPacketInfo.tcpipContext.dwProtoOverMac))
		{
			ret = TS_ForwardIpIn(m_tagPacketInfo);
			if (TRUE == ret)
			{
				return TRUE;
			}
		}
		else if (TRUE == CCommonEtherDecode::MAC_DECODE_INFO::IsARP(m_tagPacketInfo.tcpipContext.dwProtoOverMac))
		{		
			CCommonEtherDecode::ARPProtocolDecode
			(
				m_tagPacketInfo.tcpipContext.pIPHeader,
				m_tagPacketInfo.tcpipContext.dwIPPacketLen,
				m_tagPacketInfo.m_arp_decode_info
			);
			
			ret = TS_ForwardArpIn(m_tagPacketInfo);
			if (TRUE == ret)
			{
				return TRUE;
			}
		}
	}
	else
	{
		//
		pTsBackCom->m_vtPeerDct[0]->m_iHostTest.OnGetOneRsp(1, packet.pHeaderinfo->stamp.sec);
	}

	return TRUE;
}

void CFluxControlObserver::TS_HandleTimer(RECV_PACKET& packet)
{
	ACE_UINT32 now_time = packet.pHeaderinfo->stamp.sec; // time(NULL);
	
	m_piMainNet->f_MainNetCheck(now_time);
	m_piBackNet1->f_BackNetCheck(now_time);
	m_piBackNet2->f_BackNetCheck(now_time);
	m_piBackCom1->f_BackComCheck(now_time);
	m_piBackCom2->f_BackComCheck(now_time);

	size_t max = m_piPeerNode->m_vtPeerStat.size();
	for (size_t i=0; i<max; i++)
	{
		switch (m_piPeerNode->m_vtPeerStat[i])
		{
			case DISCONNECT:
				if (ECHO_OK == m_piMainNet->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = MAIN_NET_RUN;
				}
				else if (ECHO_OK == m_piBackNet1->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = BACK_NET1_RUN;
				}
				else if (ECHO_OK == m_piBackNet2->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = BACK_NET2_RUN;
				}
				else if (ECHO_OK == m_piBackCom1->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = BACK_COM1_RUN;
				}
				else if (ECHO_OK == m_piBackCom2->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = BACK_COM2_RUN;
				}
				break;
				
			case MAIN_NET_RUN:
				if (ECHO_OK != m_piMainNet->m_vtPeerDct[i]->m_eHostStat)
				{
					if (ECHO_OK == m_piBackNet1->m_vtPeerDct[i]->m_eHostStat)
					{
						m_piBackNet1->f_FlushPktQueue(i);
						
						m_piPeerNode->m_vtPeerStat[i] = BACK_NET1_RUN;
					}
					else if (ECHO_OK == m_piBackNet2->m_vtPeerDct[i]->m_eHostStat)
					{
						m_piBackNet2->f_FlushPktQueue(i);
						
						m_piPeerNode->m_vtPeerStat[i] = BACK_NET2_RUN;
					}
					else if (ECHO_OK == m_piBackCom1->m_vtPeerDct[i]->m_eHostStat)
					{
						m_piPeerNode->m_vtPeerStat[i] = BACK_COM1_RUN;
					}
					else if (ECHO_OK == m_piBackCom2->m_vtPeerDct[i]->m_eHostStat)
					{
						m_piPeerNode->m_vtPeerStat[i] = BACK_COM2_RUN;
					}
					else
					{
						m_piPeerNode->m_vtPeerStat[i] = DISCONNECT;
					}
				}
				break;

			case BACK_NET1_RUN:
				if (ECHO_OK == m_piMainNet->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = MAIN_NET_RUN;
				}
				else if (ECHO_OK != m_piBackNet1->m_vtPeerDct[i]->m_eHostStat)
				{
					if (ECHO_OK == m_piBackNet2->m_vtPeerDct[i]->m_eHostStat)
					{
						m_piBackNet2->f_FlushPktQueue(i);
						
						m_piPeerNode->m_vtPeerStat[i] = BACK_NET2_RUN;
					}
					else if (ECHO_OK == m_piBackCom1->m_vtPeerDct[i]->m_eHostStat)
					{
						m_piPeerNode->m_vtPeerStat[i] = BACK_COM1_RUN;
					}
					else if (ECHO_OK == m_piBackCom2->m_vtPeerDct[i]->m_eHostStat)
					{
						m_piPeerNode->m_vtPeerStat[i] = BACK_COM2_RUN;
					}
					else
					{
						m_piPeerNode->m_vtPeerStat[i] = DISCONNECT;
					}
				}
				break;

			case BACK_NET2_RUN:
				if (ECHO_OK == m_piMainNet->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = MAIN_NET_RUN;
				}
				else if (ECHO_OK == m_piBackNet1->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = BACK_NET1_RUN;
				}
				else if (ECHO_OK != m_piBackNet2->m_vtPeerDct[i]->m_eHostStat)
				{
					if (ECHO_OK == m_piBackCom1->m_vtPeerDct[i]->m_eHostStat)
					{
						m_piPeerNode->m_vtPeerStat[i] = BACK_COM1_RUN;
					}
					else if (ECHO_OK == m_piBackCom2->m_vtPeerDct[i]->m_eHostStat)
					{
						m_piPeerNode->m_vtPeerStat[i] = BACK_COM2_RUN;
					}
					else
					{
						m_piPeerNode->m_vtPeerStat[i] = DISCONNECT;
					}
				}
				break;

			case BACK_COM1_RUN:	
				if (ECHO_OK == m_piMainNet->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = MAIN_NET_RUN;
				}
				else if (ECHO_OK == m_piBackNet1->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = BACK_NET1_RUN;
				}
				else if (ECHO_OK == m_piBackNet2->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = BACK_NET2_RUN;
				}
				else if (ECHO_OK != m_piBackCom1->m_vtPeerDct[i]->m_eHostStat)
				{
					if (ECHO_OK == m_piBackCom2->m_vtPeerDct[i]->m_eHostStat)
					{
						m_piPeerNode->m_vtPeerStat[i] = BACK_COM2_RUN;
					}
					else
					{
						m_piPeerNode->m_vtPeerStat[i] = DISCONNECT;
					}
				}
				break;

			case BACK_COM2_RUN:
				if (ECHO_OK == m_piMainNet->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = MAIN_NET_RUN;
				}
				else if (ECHO_OK == m_piBackNet1->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = BACK_NET1_RUN;
				}
				else if (ECHO_OK == m_piBackNet2->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = BACK_NET2_RUN;
				}
				else if (ECHO_OK == m_piBackCom1->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = BACK_COM1_RUN;
				}
				else if (ECHO_OK != m_piBackCom2->m_vtPeerDct[i]->m_eHostStat)
				{
					m_piPeerNode->m_vtPeerStat[i] = DISCONNECT;
				}
				break;

			default:
				break;
		}
	}

	return;
}
////////////////////////////////

int CmpTime1(const struct tm * ptm,
			 int nRebootHour,
			 int nRebootMin,
			 int nRebootSec )
{
	if( ptm->tm_hour < nRebootHour )
		return -1;

	if( ptm->tm_hour > nRebootHour )
		return 1;

	if( ptm->tm_min < nRebootMin )
		return -1;

	if( ptm->tm_min > nRebootMin )
		return 1;

	if( ptm->tm_sec < nRebootSec )
		return -1;

	if( ptm->tm_sec > nRebootSec )
		return 1;

	return 0;
}

BOOL CmpTime2(const struct tm * ptm,
			  int nRebootHour,
			  int nRebootMin,
			  int nRebootSec,
			  int timeout_sec)
{
	int ncmp_value = CmpTime1(ptm,nRebootHour,nRebootMin,nRebootSec);

	if( ncmp_value == 0 )
		return true;

	if( ncmp_value < 0 )
		return false;

	int next_hour = nRebootHour;
	int next_min = nRebootMin;
	int next_secode = nRebootSec + timeout_sec;

	if( next_secode >= 60 )
	{
		next_min += next_secode/60;
		next_secode = next_secode%60;

		if( next_min >= 60 )
		{
			next_hour += next_min/60;
			next_min = next_min%60;
		}
	}

	if( CmpTime1(ptm,next_hour,next_min,next_secode) <= 0 )
	{
		return true;
	}

	return false;
}

void CFluxControlObserver::HandleFirstPacket(const CTimeStamp32& cur_time_stamp)
{
	return;
}

void CFluxControlObserver::SendPktToCard(IWriteDataStream * pWriteDataStream,BOOL bIsInternetVM,int output_card_index,const PACKET_INFO_EX& packet_info)
{
	{
		if( !packet_info.tcpipContext.IsIP() )
		{
			pWriteDataStream->WritePacket(
				packet_info.pkt_len_type,
				(const char *)packet_info.packet->pPacket,
				packet_info.packet->nPktlen,
				(1<<output_card_index),3);
		}
		else
		{
			int mtu_layer_2 = 1514;

			if( packet_info.tcpipContext.vlanId != -1 )
			{
				mtu_layer_2 += 4;
			}

			const int mtu_ip = mtu_layer_2-(int)(packet_info.tcpipContext.pIPHeader-packet_info.tcpipContext.pMACHeader);

			if( packet_info.tcpipContext.dwIPPacketLen > (unsigned int)mtu_ip )
			{
				g_pCConfigFacade->SplitIPPacket_By_MaxIPLen(packet_info.tcpipContext,pWriteDataStream,output_card_index,mtu_ip);
			}
			else
			{
				pWriteDataStream->WritePacket(
					packet_info.pkt_len_type,
					(const char *)packet_info.packet->pPacket,
					packet_info.packet->nPktlen,
					(1<<output_card_index),3);
			}
		}
	}

	return;
}

BOOL CFluxControlObserver::LoadProbeNetcom()
{
	ACE_TCHAR conf_filename[MAX_PATH];
	CPF::GetModuleOtherFileName(conf_filename, MAX_PATH, "ini","test_control.xml");//hard coded

	CTinyXmlEx reader;

	if( 0 != reader.open(conf_filename) ){ return false; }

	TiXmlNode * net_com_node = reader.GetRootNode()->FirstChildElement("net_com");

	if( !net_com_node )
	{
		ACE_ASSERT(FALSE);
		return false;
	}

	int listen_port = 61001;
	int cmd_servant_id = 1;
	int live_servant_id = 2;
	int multi_thread = 0;
	int max_client_nums = 256;

	reader.GetValueLikeIni(net_com_node,"listen_port",listen_port);
	reader.GetValueLikeIni(net_com_node,"cmd_servant_id",cmd_servant_id);
	reader.GetValueLikeIni(net_com_node,"live_servant_id",live_servant_id);
	reader.GetValueLikeIni(net_com_node,"multi_thread",multi_thread);
	reader.GetValueLikeIni(net_com_node,"max_client_nums",max_client_nums);

	m_pProbeNetCom = new CProbeNetCom;

	ACE_Time_Value recv_timeout(3);
	ACE_Time_Value send_timeout(3);

	if( !m_pProbeNetCom )
		return false;

	if( !m_pProbeNetCom->init(listen_port
		,cmd_servant_id
		,live_servant_id
		,multi_thread
		,max_client_nums
		,&recv_timeout,&send_timeout
		,this,m_pLogFile) )
	{
		MY_ACE_DEBUG(m_pLogFile,
			(LM_INFO,ACE_TEXT("[Err][%D]The communication module failed to start, and the communication interface could not be monitored! \n")
			));

		return false;
	}

	m_pProbeNetCom->start();

	return true;
}

void CFluxControlObserver::UnLoadProbeNetcom()
{
	if( m_pProbeNetCom )
	{
		m_pProbeNetCom->stop();
		m_pProbeNetCom->close();
		delete m_pProbeNetCom;
		m_pProbeNetCom = NULL;
	}

	return;
}

int CFluxControlObserver::OnMirrorOutConfig(const CMirrorOutConfig& new_config)
{
	if( new_config == m_mirror_out_config )
	{
		return 0;
	}

	if( new_config.m_mirror_status == m_mirror_out_config.m_mirror_status
		&& new_config.m_mirror_status == 0 )
	{
		m_mirror_out_config = new_config;

		return 0;
	}

	m_mirrorout_mgr.stop_mirror();

	if( new_config.m_mirror_status == 0 )
	{
		m_mirror_out_config = new_config;

		return 0;
	}

	char mirror_config_file_name[MAX_PATH]={0};

	CPF::JoinPathToPath(mirror_config_file_name,sizeof(mirror_config_file_name),
		ConfigCenter_Obj()->m_str_probe_config_path.c_str(),"mirror_out_flt.flt");

	if( !m_mirrorout_mgr.start_mirror(m_pInterfaceCfgFactory,
		new_config,mirror_config_file_name) )
	{
		return -1;
	}

	m_mirror_out_config = new_config;

	return 0;
	
}
BOOL CFluxControlObserver::GetPACopyDataInfo()
{
	return true;
}

BOOL CFluxControlObserver::StartCapDataForClient(BOOL bUsdFlt,
										std::vector< std::string > vt_from_card_name,
										int ncapsize_mb,
										int ncaptime_sec,
										std::string& str_url)
{	
	CChannleCtrlMgr_LOCKER_ATOM lock_;

	if( m_cap_data_mgr_ex.IsCapping() )
	{
		m_cap_data_mgr_ex.stop_cap(true);
	}

	m_cap_data_mgr_ex.init(m_pInterfaceCfgFactory,vt_from_card_name);

	std::string strSaveFileName = CPF::FormatTime(5,ACE_OS::gettimeofday().sec());
	strSaveFileName += ".cap";

	str_url = CPF::JoinPathToPath("udata_cap/cap_data/",strSaveFileName.c_str());

	char flt_file_name[MAX_PATH];

	CPF::JoinPathToPath(flt_file_name,sizeof(flt_file_name),
		ConfigCenter_Obj()->m_str_probe_config_path.c_str(),
		"cap_flt.flt");

	BOOL bOK = m_cap_data_mgr_ex.start_cap(
		DRIVER_TYPE_FILE_ETHEREAL,
		bUsdFlt,flt_file_name,
		ncapsize_mb,1,ncaptime_sec,
		ConfigCenter_Obj()->m_prg_base_path.c_str(),
		strSaveFileName.c_str());

	return bOK;

}

BOOL CFluxControlObserver::StartCapDataForTest(DRIVER_TYPE drv_type,
											   BOOL bUsdFlt,
											   const char * flt_file_name,
											   std::vector< std::string > vt_from_card_name,
											   int ncapsize_mb,
											   int file_nums,
											   int ncaptime_sec,
											   const char * file_path,
											   const char * file_name)
{	
	CChannleCtrlMgr_LOCKER_ATOM lock_;

	if( m_cap_data_mgr_ex.IsCapping() )
	{
		return false;
	}

	m_cap_data_mgr_ex.init(m_pInterfaceCfgFactory,vt_from_card_name);

	BOOL bOK = m_cap_data_mgr_ex.start_cap(
		drv_type,
		bUsdFlt,flt_file_name,
		ncapsize_mb,file_nums,
		ncaptime_sec,
		file_path,
		file_name);

	return bOK;

}

void CFluxControlObserver::StopCapData(BOOL bDeleteFile)
{
	CChannleCtrlMgr_LOCKER_ATOM lock_;

	m_cap_data_mgr_ex.stop_cap(bDeleteFile);

	return;
}

int CFluxControlObserver::GetCapStatus(int& pos)
{
	return m_cap_data_mgr_ex.get_cap_status(pos);
}

BOOL CFluxControlObserver::GetProbeConfigPath()
{
	char xml_file_name[MAX_PATH] = {0};

	CPF::GetModuleOtherFileName(xml_file_name,sizeof(xml_file_name),"ini","test_control.xml");

	CTinyXmlEx xml_reader;

	if( 0 != xml_reader.open(xml_file_name) )
	{
		return FALSE;
	}

	if( !xml_reader.GetRootNode() )
	{
		return false;
	}

	TiXmlNode * common_node =  xml_reader.GetRootNode()->FirstChildElement("common");

	if( common_node )
	{
		xml_reader.GetValueLikeIni(common_node,ACE_TEXT("self_host_name"),m_gr_host_name);
	}	

	return true;
}

int CFluxControlObserver::SendPacketToCard(int card_index,const char * packet,int packet_len,int send_nums)
{
	if( card_index == -1 )
		return -1;

	CInterfaceCfg * pInterfaceCfg = GetInterfaceCfg();

	if( !pInterfaceCfg )
		return -1;

	IWriteDataStream * pWriteDataSteam = pInterfaceCfg->GetWriteDataStream();

	if( !pWriteDataSteam )
		return -1;

	for(int i = 0; i < send_nums; ++i)
	{
		pWriteDataSteam->WritePacket(
			PACKET_OK,(const char *)packet,packet_len,(1<<card_index),3);
	}

	return 0;
}

int CFluxControlObserver::SendPacketToCurInputCard(DIR_IO dir_phy,const char * packet,int packet_len,int send_nums)
{
#if 0 // xumx delete 2016-6-28
	{
		if( m_cur_input_card_index != -1 )
		{
			CInterfaceCfg * pInterfaceCfg = GetInterfaceCfg();

			if( pInterfaceCfg )
			{			
				if( dir_phy == DIR_I2O )
					return SendPacketToCard(m_cur_input_card_index,packet,packet_len,send_nums);
				else
					return 0;
			}
		}
	}
#endif
	return 0;
}


BOOL CFluxControlObserver::GetManagerPortIP()
{
	m_mgr_hostorder_ip = 0;

	std::vector<CInterfaceCfg::IP_CARD_INFO>	vt_ip_card_info;

	CInterfaceCfg::GetAllIPCardInfo(vt_ip_card_info);

	MY_ACE_DEBUG(m_pLogFile,
		(LM_ERROR,
		ACE_TEXT("[Info][%D]GetManagerPortIP.GetAllIPCardInfo=%d\n"),(int)vt_ip_card_info.size()
		));

	const char port_name[][8] = {"MNT","MIR","MIR1","LAN1","LAN2","LAN","WAN1","WAN2","WAN"};

	for(int j = 0; j < sizeof(port_name)/sizeof(port_name[0]); ++j)
	{
		for(size_t i = 0; i < vt_ip_card_info.size(); ++i)
		{
			if( stricmp(vt_ip_card_info[i].friend_name,port_name[j]) == 0 )
			{
				m_mgr_hostorder_ip = vt_ip_card_info[i].host_ip;
				
				break;
			}			
		}

		if( m_mgr_hostorder_ip != 0 )
		{
			break;
		}
	}

	if( m_mgr_hostorder_ip == 0 && vt_ip_card_info.size() > 0 )
	{
		m_mgr_hostorder_ip = vt_ip_card_info[0].host_ip;
	}

	MY_ACE_DEBUG(m_pLogFile,
		(LM_ERROR,
		ACE_TEXT("[Info][%D]GetManagerPortIP.m_mgr_hostorder_ip=%s\n"),CPF::hip_to_string(m_mgr_hostorder_ip)
		));

	return true;

}

int CFluxControlObserver::OnIPChanged()
{
	if( !m_pInterfaceCfgFactory )
		return 0;

	GetManagerPortIP();


	return 0;
}


BOOL CFluxControlObserver::DoOutPut()
{

	return true;
}

BOOL CFluxControlObserver::GetProbeIfDynInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{

	return true;

}

BOOL CFluxControlObserver::GetNetIfConfigRunStatus(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	BOOL bRunOK = (m_pInterfaceCfgFactory->GetInterfaceCfg() != NULL);

	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock,2);

	char * pdata = CMsgBlockManager::WritePtr(*pRetMsgBlock,0,0);
	char * orig = pdata;

	CData_Stream_LE::PutUint16(pdata,(ACE_UINT16)(bRunOK?0:-1));

	pRetMsgBlock->nDataLen = (int)(pdata-orig);

	return true;
}


BOOL CFluxControlObserver::GetProbeAllIfCardInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	BOOL bDrvOK = true;

	std::vector<std::string> vt_cardinfo;

	m_pInterfaceCfgFactory->GetAllNetCardInfo(bDrvOK,vt_cardinfo);

	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock,2048);

	char * pdata = CMsgBlockManager::WritePtr(*pRetMsgBlock,0,0);
	char * orig = pdata;

	CData_Stream_LE::PutUint32(pdata,bDrvOK);
	CData_Stream_LE::PutVectorString(pdata,vt_cardinfo);

	pRetMsgBlock->nDataLen = (int)(pdata-orig);

	return true;

}

BOOL CFluxControlObserver::OnCmd_GetProbeAllRunCardInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	std::vector<ACE_UINT64> vt_cardinfo;

	m_pInterfaceCfgFactory->GetProbeAllRunCardInfo(vt_cardinfo);

	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock,2048);

	char * pdata = CMsgBlockManager::WritePtr(*pRetMsgBlock,0,0);
	char * orig = pdata;

	CData_Stream_LE::PutVectorInt64(pdata,vt_cardinfo);

	pRetMsgBlock->nDataLen = (int)(pdata-orig);

	return true;

}

BOOL CFluxControlObserver::GetAllIPCardInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	std::vector<CInterfaceCfg::IP_CARD_INFO> vt_ip_cardinfo;

	CInterfaceCfg::GetAllIPCardInfo(vt_ip_cardinfo);

	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock,4+vt_ip_cardinfo.size()*sizeof(CInterfaceCfg::IP_CARD_INFO));

	char * pdata = CMsgBlockManager::WritePtr(*pRetMsgBlock,0,0);
	char * orig = pdata;

	CData_Stream_LE::PutUint32(pdata,(ACE_UINT32)vt_ip_cardinfo.size());

	for(size_t i = 0; i < vt_ip_cardinfo.size(); ++i)
	{
		CData_Stream_LE::PutUint32(pdata,vt_ip_cardinfo[i].host_ip);
		CData_Stream_LE::PutUint32(pdata,vt_ip_cardinfo[i].host_mask);
		CData_Stream_LE::PutUint32(pdata,vt_ip_cardinfo[i].host_gateway);

		CData_Stream_LE::PutUint32(pdata,vt_ip_cardinfo[i].dns1);
		CData_Stream_LE::PutUint32(pdata,vt_ip_cardinfo[i].dns2);

		CData_Stream_LE::PutFixString(pdata,6,(const char *)vt_ip_cardinfo[i].mac_addr);

		CData_Stream_LE::PutString(pdata,vt_ip_cardinfo[i].friend_name);
	}

	pRetMsgBlock->nDataLen = (int)(pdata-orig);

	return true;
}

