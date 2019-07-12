 //////////////////////////////////////////////////////////////////////////
//FCServerFacade.cpp

#include "StdAfx.h"
#include "FCServerFacade.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/path_tools.h"
#include "ProbeServant.h"
#include "CConfigAndCtrlDoCmd.h"
#include "cpf/NetcomBaseServant.h"
#include "tcpip_mgr_common_init.h"
#include "SockOperationId.h"
#include "SysMaintenanceDoCmd.h"
#include "config_fc_server_xml.h"
#include "config_center.h"
#include "cpf/other_tools.h"
#include "ace/High_Res_Timer.h"
#include "cpf/Common_Func_Macor.h"
#include "cpf/adapter_info_query.h"

// 线程处理函数
void *LMB_KEY_thread_proc(void *arg)
{
	LMB_SCREEN_SERIAL *	lmb_handle = (LMB_SCREEN_SERIAL *)arg;
	
	if(lmb_handle == NULL){ return(NULL); }
	if(lmb_handle->Is_serial_successed() < 0){ return(NULL); }

	while (FALSE == lmb_handle->f_MapShareMem())
	{
		sleep(1);
	}
	
	// 循环读取key
	int read_size=0;
	unsigned char read_key_code[2];
	while(1)
	{
		read_size = lmb_handle->ace_read_dev.recv(read_key_code, 1);
		if(read_size == 1)
		{
			lmb_handle->API_LMB_OPERATE(SERIAL_OPERATE_BACKGROUND_OPEN, NULL, 0);
			lmb_handle->f_ScreenTimeout(10);
			
			if ((LMB_KEYCODE_UP == read_key_code[0]) || (LMB_KEYCODE_LEFT == read_key_code[0]))
			{

				lmb_handle->f_ScreenMoveUp();
				lmb_handle->f_ScreenPrintf();
			}
			else if ((LMB_KEYCODE_DOWN == read_key_code[0]) || (LMB_KEYCODE_RIGHT == read_key_code[0]))
			{
				lmb_handle->f_ScreenMoveDown();
				lmb_handle->f_ScreenPrintf();
			}
		}
		else
		{
			sleep(1);
		}
	}

	return(NULL); 
}

CFCServerFacade::CFCServerFacade(void)
{
	m_pLogInstance = NULL;
	m_pProbeNetCom = NULL;

	//default-port
	m_nListenPort = 83;

	m_pMonitorEvent = NULL;
	m_monitor_event_interval = 0;

	// add by xumx {{{
	m_pLMB_SCREEN_SERIAL = NULL; // add by xumx 2016-10-24
	m_pProbeSysInfo = NULL;// add by xumx 2016-10-25
	m_LMB_thread_Id = 0;
	m_LMB_thread_Handle = NULL;
	// add by xumx }}}

	memset(m_strSoftVersion, 0, 64);

	m_pUnsStatShm = NULL;
}

CFCServerFacade::~CFCServerFacade(void)
{
// add by xumx {{{
	// add by xumx 2016-10-24
	if(m_pLMB_SCREEN_SERIAL != NULL)
	{
		m_pLMB_SCREEN_SERIAL->Close_serial();
		delete(m_pLMB_SCREEN_SERIAL);
		m_pLMB_SCREEN_SERIAL = NULL;
	}

	if( m_pProbeSysInfo ) // add by xumx 2016-10-25
	{
		delete m_pProbeSysInfo;
		m_pProbeSysInfo = NULL;
	}

	if ((m_LMB_thread_Id > 0) && (m_LMB_thread_Handle != NULL))
	{
		ACE_Thread::cancel(m_LMB_thread_Id); // 停止线程
	}
// add by xumx }}}
}

BOOL CFCServerFacade::init(
						   ACE_Log_File_Msg * pLogInstance,
						   const char * monitor_event_name,
						   int monitor_event_interval)
{
	m_prg_start_time = GetTickCount();

	m_pLogInstance = pLogInstance;

	char* pTemp = f_GetSoftVersion();
	if (NULL == pTemp)
	{
		MY_ACE_DEBUG(m_pLogInstance, (LM_ERROR, ACE_TEXT("[Err][%D]CFCServerFacade::f_GetSoftVersion failed\n")));
		return FALSE;
	}
	
	{
		m_monitor_event_interval = monitor_event_interval/3;

		if( m_monitor_event_interval <= 0 )
			m_monitor_event_interval = 3;

		if( monitor_event_name && strlen(monitor_event_name) > 0 )
		{
			m_pMonitorEvent = new my_event_t;
			if( -1 == MY_EVENT::event_init_for_server(m_pMonitorEvent,0,1,0,monitor_event_name) )
			{
				MY_ACE_DEBUG(m_pLogInstance,
					(LM_ERROR,ACE_TEXT("[Err][%D]create event for monitor_event:%s failed.\n"),
					monitor_event_name));
				delete m_pMonitorEvent;
				m_pMonitorEvent = NULL;
			}
			else
			{
				MY_ACE_DEBUG(m_pLogInstance,
					(LM_ERROR,ACE_TEXT("[Info][%D]create event for monitor_event:%s success!\n"),
					monitor_event_name));
			}
		}
		else
		{
			MY_ACE_DEBUG(m_pLogInstance,
				(LM_ERROR,ACE_TEXT("[Err][%D]monitor_event_name is empty,can not create monitor event!!\n")));
		}
	}
	
	if( m_pMonitorEvent )
	{
		MY_EVENT::event_signal(m_pMonitorEvent);
	}


	LoadProbeNetcom();

	if( m_pMonitorEvent )
	{
		MY_EVENT::event_signal(m_pMonitorEvent);
	}

	int ret = f_UiInit();
	if (0 > ret)
	{
		MY_ACE_DEBUG(m_pLogInstance, (LM_ERROR, ACE_TEXT("[Err][%D]f_LMB_init failed, return:%d\n"), ret));
		return FALSE;
	}

	return TRUE;

}
void CFCServerFacade::fini()
{
	UnLoadProbeNetcom();

	return;
}

void CFCServerFacade::OnFCReconnectedToServer(BOOL bConnectOK,CConnectToServerMgr * sock_mgr)
{
	if( bConnectOK && m_pProbeNetCom && m_pProbeNetCom->GetConfigAndCtrlDoCmd() )
	{
		//m_pProbeNetCom->GetConfigAndCtrlDoCmd()->Syn_AllConfig_To_FCProbe();

	}
}

BOOL CFCServerFacade::Start()
{
	schedule_timer();

	m_ActiveTimer.timer_queue()->gettimeofday(ACE_High_Res_Timer::gettimeofday_hr);
	m_ActiveTimer.activate();

	return true;
}

void CFCServerFacade::Stop()
{
	m_ActiveTimer.stop_and_remove_all_timer();

	return;
}


void CFCServerFacade::schedule_timer()
{
	enum{
		INTERVAL_Test_ConnectToServer = 60,
		INTERVAL_SHOW_LMB = 5,
	};

	/*delete by zhongjh, see handle_timeout
	m_ActiveTimer.schedule(this,(const void *)TIMER_Test_ConnectToServer,
		ACE_High_Res_Timer::gettimeofday_hr()+ACE_Time_Value(INTERVAL_Test_ConnectToServer*2),
		ACE_Time_Value(INTERVAL_Test_ConnectToServer) );*/

	if( m_pMonitorEvent )
	{
		m_ActiveTimer.schedule(this,(const void *)TIMER_SET_EVENT,
			ACE_High_Res_Timer::gettimeofday_hr(),
			ACE_Time_Value(m_monitor_event_interval) );
	}

	// 液晶屏的显示的时钟
	// add by xumx 2016-10-24 
	m_ActiveTimer.schedule
	(
		this,
		(const void *)TIMER_LMB_SHOW,
		ACE_High_Res_Timer::gettimeofday_hr(),
		ACE_Time_Value(INTERVAL_SHOW_LMB)
	);

	return;
}

int CFCServerFacade::handle_timeout(const ACE_Time_Value& in_tv,const void *arg)
{
	//ACE_Time_Value cur_time = ACE_OS::gettimeofday();

	int int_param = (int)arg;

	switch(int_param)
	{
		case TIMER_SET_EVENT:
			if (m_pMonitorEvent)
			{
				MY_EVENT::event_signal(m_pMonitorEvent);
			}
			break;

		/*delete by zhongjh, test tcp server should be moved to unierm_ctrl process rather than in self
		case TIMER_Test_ConnectToServer:
			{
				TestConnectToServer();
			}
			break;*/

		// add by xumx 2016-10-24 
		case TIMER_LMB_SHOW:
			handle_LBM_show();
			break;

		default:
			break;
	}

	return 0;
}

BOOL CFCServerFacade::LoadProbeNetcom()
{
	ACE_UINT32 t1 = GetTickCount();

	if (m_pMonitorEvent)
	{
		MY_EVENT::event_signal(m_pMonitorEvent);
	}

	m_FCSockManager.init(this);

	ACE_UINT32 t2 = GetTickCount();

	MY_ACE_DEBUG(m_pLogInstance, (LM_INFO, ACE_TEXT("[Info][%D]m_FCSockManager.init spend %d ms!!!\r\n"), (t2 - t1)));

	if (m_pMonitorEvent)
	{
		MY_EVENT::event_signal(m_pMonitorEvent);
	}

	int multi_thread = FCServerConfig_Obj()->multi_thread;
	int max_client_nums = FCServerConfig_Obj()->max_client_nums;
	
	m_pProbeNetCom = new CProbeNetCom;

	if( !m_pProbeNetCom )
	{
		ACE_ASSERT(FALSE);
		return false;
	}

	ACE_Time_Value recv_timeout(FCServerConfig_Obj()->recv_timeout_ms/1000,(FCServerConfig_Obj()->recv_timeout_ms%1000)*1000);
	ACE_Time_Value send_timeout(FCServerConfig_Obj()->send_timeout_ms/1000,(FCServerConfig_Obj()->send_timeout_ms%1000)*1000);

	if( !m_pProbeNetCom->init(m_nListenPort
		,CMD_SERVANT_ID
		, LIVE_SERVANT_ID
		,SYS_MAINTANCE_SERVANT_ID
		,multi_thread
		,max_client_nums
		,&recv_timeout
		,&send_timeout
		,m_pLogInstance
		,this
		,&m_FCSockManager) )
	{
		MY_ACE_DEBUG(m_pLogInstance,
			(LM_INFO,ACE_TEXT("[Err][%D]m_pProbeNetCom->init failed. please check port %d is available.\n"),
			m_nListenPort));

		return false;
	}

	ACE_UINT32 t3 = GetTickCount();

	MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]m_pProbeNetCom->init spend %d ms!!!\r\n"),(t3-t2)));

	if( m_pMonitorEvent )
	{
		MY_EVENT::event_signal(m_pMonitorEvent);
	}

	m_pProbeNetCom->start();

	return true;
}


void CFCServerFacade::UnLoadProbeNetcom()
{
	if( m_pProbeNetCom )
	{
		m_pProbeNetCom->stop();
		m_pProbeNetCom->close();
		delete m_pProbeNetCom;
		m_pProbeNetCom = NULL;
	}

	m_FCSockManager.fini();

	return;
}

int CFCServerFacade::SendEchoToServer(int& out_total_fail_nums,int& out_connect_fail_nums)
{
	static int nTotalFaliedNums = 0;
	static int nConnectFailedNums = 0;

	CNetClientWithOneProxy<CProxyImp>	net_client;

	if( !net_client.init("127.0.0.1",m_nListenPort,NULL) )
	{
		++nTotalFaliedNums;
		++nConnectFailedNums;

		out_total_fail_nums = nTotalFaliedNums;
		out_connect_fail_nums = nConnectFailedNums;

		if( nTotalFaliedNums >= 3 )
			return -1;
		else
			return 0;		
	}
	else
	{
		nConnectFailedNums = 0;
	}

	//{
	//	char buf[100] = {0};

	//	ACE_INET_Addr local_addr;

	//	net_client.get_local_addr(local_addr);

	//	local_addr.addr_to_string(buf,sizeof(buf));

	//	MY_ACE_DEBUG(m_pLogInstance,
	//		(LM_INFO,ACE_TEXT("\n\nbegin send echo_test,from %s\n"),buf));
	//}

	net_client.start();

/*
	MSG_BLOCK * pResponseMsgBlock = NULL;

	unsigned int test_count_id = -1;

	int nrt = net_client.GetProxy()->SendRequest(
		CMD_SERVANT_ID,OP_ID_ACCOUNT_ECHO,
		&test_count_id,
		sizeof(test_count_id),
		pResponseMsgBlock,3);

	if( pResponseMsgBlock )
	{
		CMsgBlockManager::Free(pResponseMsgBlock);
	}

	//MY_ACE_DEBUG(m_pLogInstance,
	//	(LM_INFO,ACE_TEXT("%D:echo test result=(%d)\n"),nrt));

	if( nrt == 0 )
	{		
		nTotalFaliedNums = 0;
	}
	else
	{
		++nTotalFaliedNums;
	}

	out_total_fail_nums = nTotalFaliedNums;
	out_connect_fail_nums = nConnectFailedNums;

	net_client.stop();

	if( nTotalFaliedNums >= 3 )
		return -1;

	if( nTotalFaliedNums == 0 )
		return 1;
	*/
	return 0;
}


void CFCServerFacade::TestConnectToServer()
{
//#ifndef _DEBUG

	int out_total_fail_nums = 0;
	int out_connect_fail_nums = 0;

	int nRtn = SendEchoToServer(out_total_fail_nums,out_connect_fail_nums);

	if( nRtn == -1 )
	{
	//	MY_ACE_DEBUG(m_pLogInstance,
	//		(LM_INFO,ACE_TEXT("****************************************\n")));

		MY_ACE_DEBUG(m_pLogInstance,
			(LM_INFO,ACE_TEXT("[Err][%D]not connect to server=(%d,%d)，may be socket  blockrestart program!!\n"),out_total_fail_nums,out_connect_fail_nums));

	//	MY_ACE_DEBUG(m_pLogInstance,
	//		(LM_INFO,ACE_TEXT("****************************************\n")));

#ifndef _DEBUG
		exit(-2);
#endif
	}
	else if( nRtn == 0 )
	{
	//	MY_ACE_DEBUG(m_pLogInstance,
		//	(LM_INFO,ACE_TEXT("****************************************\n")));

		MY_ACE_DEBUG(m_pLogInstance,
			(LM_INFO,ACE_TEXT("[Err][%D]not connect to server =(%d,%d)，may be socket block,   wait next retry!!\n"),out_total_fail_nums,out_connect_fail_nums));

	//	MY_ACE_DEBUG(m_pLogInstance,
	//		(LM_INFO,ACE_TEXT("****************************************\n")));
	}
	else
	{
	//	MY_ACE_DEBUG(m_pLogInstance,
	//		(LM_INFO,ACE_TEXT("[Info][%D]connect to server OK\n")));

	}

//#endif

	return;	

}

BOOL CFCServerFacade::BackupSysCfgFile(const char * sys_cfg_file)
{
	return CSysMaintenanceDoCmd::BackupSysCfgFile(sys_cfg_file);

}

BOOL CFCServerFacade::LoadSysCfgFile(const char * sys_cfg_file, int incude_local_machine)
{
	return CSysMaintenanceDoCmd::LoadSysCfgFile(sys_cfg_file, incude_local_machine);

}

BOOL CFCServerFacade::RestoreOrig()
{
	return CSysMaintenanceDoCmd::RestoreOrig();

}


ACE_UINT32	CFCServerFacade::GetPgrRunTime_Second() const
{
	return (GetTickCount()-m_prg_start_time)/1000;
}

char *CFCServerFacade::f_GetSoftVersion(void)
{
	CTinyXmlEx xml_doc;

	// 打开文件
	BOOL result = xml_doc.open_for_readonly("/home/ts/version/version.xml");
	if(result < 0){ return(NULL); }

	// XML文件的根节点
	TiXmlNode *xml_root_node = xml_doc.GetRootNode();

	// 版本信息节点
	TiXmlNode *version_root = xml_doc.FirstChildElement(xml_root_node, "SERVICE");
	if(version_root == NULL)
	{
		xml_doc.close();
		return(NULL);
	}

	std::string version;
	xml_doc.GetValueLikeIni(version_root, "VERSION", version);
	if((version.length() == 0) || (version.length() > 3))
	{ 
		xml_doc.close(); 
		return(NULL); 
	}

	std::string release;
	xml_doc.GetValueLikeIni(version_root, "RELEASE", release);
	if((release.length() == 0) || (release.length() > 3))
	{ 
		xml_doc.close(); 
		return(NULL); 
	}
	
	std::string build;
	xml_doc.GetValueLikeIni(version_root, "BUILD", build);
	if((build.length() == 0) || (build.length() > 3))
	{ 
		xml_doc.close();
		return(NULL); 
	}

	sprintf(m_strSoftVersion, "%s.%s.%s", version.c_str(), release.c_str(), build.c_str());

	xml_doc.close();

	return(m_strSoftVersion);
}

int CFCServerFacade::f_UiInit()
{
	//
	do{
		usleep(5*1000);
		m_pUnsStatShm = (UNS_STAT_SHM_S*)m_iUnsStatShm.map(UNS_STAT_SHM_NAME);
	}while((NULL == m_pUnsStatShm) || ((UNS_STAT_SHM_S*)-1 == m_pUnsStatShm));

	//更新系统常量信息
	m_pProbeSysInfo = new CProbeSysInfo;
	if (NULL == m_pProbeSysInfo)
	{
		MY_ACE_DEBUG(m_pLogInstance, (LM_ERROR, ACE_TEXT("[Err][%D]m_pProbeSysInfo create failed.\n")));
			
		return -1;
	}
	m_pProbeSysInfo->init(NULL);

	do{
		m_pProbeSysInfo->m_memInfo.Refresh();		
		m_pUnsStatShm->tagSysInfo.nMemTotal = m_pProbeSysInfo->m_memInfo.m_uPhysicalMem/1000;
	}while(0 == m_pUnsStatShm->tagSysInfo.nMemTotal);
	MY_ACE_DEBUG(m_pLogInstance, (LM_INFO, ACE_TEXT("[Info][%D]m_pProbeSysInfo->m_memInfo.Refresh() OK.\n")));
	
	do{
		m_pProbeSysInfo->m_diskInfo.Refresh();
		UINT32 nDiskNum = m_pProbeSysInfo->m_diskInfo.m_vtDisk.size();
		UINT32 disk_size_totoal = 0;
		for(UINT32 i=0; i<nDiskNum; i++)
		{
			disk_size_totoal += m_pProbeSysInfo->m_diskInfo.m_vtDisk[i].nCapacity;  // 总空间(单位M)
		}
		m_pUnsStatShm->tagSysInfo.nHddTotal = disk_size_totoal/1000;
	}while(0 == m_pUnsStatShm->tagSysInfo.nHddTotal);
	MY_ACE_DEBUG(m_pLogInstance, (LM_INFO, ACE_TEXT("[Info][%D]m_pProbeSysInfo->m_diskInfo.Refresh() OK.\n")));

	UINT32 now_time = time(NULL);
	UINT32 system_run_time = GetTickCount_s();
	m_pUnsStatShm->tagSysInfo.nSysStartTime = now_time - system_run_time;

	// 液晶屏
	m_pLMB_SCREEN_SERIAL = new LMB_SCREEN_SERIAL;
	if (NULL == m_pLMB_SCREEN_SERIAL)
	{
		MY_ACE_DEBUG(m_pLogInstance, (LM_ERROR, ACE_TEXT("[Err][%D]m_pLMB_SCREEN_SERIAL create failed.\n")));
		return -2;
	}
	
	m_pLMB_SCREEN_SERIAL->serial_open("/dev/ttyS1");
	if(m_pLMB_SCREEN_SERIAL->Is_serial_successed() < 0)
	{
		delete m_pLMB_SCREEN_SERIAL;
		m_pLMB_SCREEN_SERIAL=NULL;
		
		MY_ACE_DEBUG(m_pLogInstance, (LM_ERROR, ACE_TEXT("[Err][%D]m_pLMB_SCREEN_SERIAL->serial_open failed.\n")));
		return -3;
	}
	
	m_pLMB_SCREEN_SERIAL->f_CreateShareMem();
	for (int i=0; i<21; i++)
	{
		m_pLMB_SCREEN_SERIAL->f_AddOneLine(NULL);
		f_RefreshLmbData(i);
	}

	m_pLMB_SCREEN_SERIAL->API_LMB_OPERATE(SERIAL_OPERATE_CLEAR_SCREEN, NULL, 0);
	m_pLMB_SCREEN_SERIAL->API_LMB_OPERATE(SERIAL_OPERATE_HIDE_CURSOR, NULL, 0);
	m_pLMB_SCREEN_SERIAL->API_LMB_OPERATE(SERIAL_OPERATE_BACKGROUND_OPEN, NULL, 0);
	m_pLMB_SCREEN_SERIAL->f_ScreenTimeout(5);

	// 启动一个线程,读取液晶屏的key,  add by xumx 2016-10-25
	ACE_Thread::spawn
	(
		(ACE_THR_FUNC)LMB_KEY_thread_proc,   //线程执行函数
		(void *)m_pLMB_SCREEN_SERIAL,  //执行函数参数
		0,   // 线程属性
		&m_LMB_thread_Id,
		&m_LMB_thread_Handle
	);

	return 0;
}

void CFCServerFacade::f_RefreshLmbData(unsigned int nCurLine)
{
	char text[16] = {0};
	int nE1Index;
	int bFirst = TRUE;
	
	switch(nCurLine)
	{
		case 0:
			sprintf(text, "ADMIN:LAN%d", m_pUnsStatShm->aLocalMgmtIf[3]+1);
			break;
		case 1:
			sprintf(text, "%d.%d.%d.%d", m_pUnsStatShm->aLocalMgmtIp[0],m_pUnsStatShm->aLocalMgmtIp[1],m_pUnsStatShm->aLocalMgmtIp[2],m_pUnsStatShm->aLocalMgmtIp[3]);
			break;
		case 2:
			sprintf(text, "CPU:%d%%", m_pUnsStatShm->tagSysInfo.nCpuRate);
			break;
		case 3:
			sprintf(text, "MEM:%.2f/%.2fG", (double)(m_pUnsStatShm->tagSysInfo.nMemUsed)/1000, (double)(m_pUnsStatShm->tagSysInfo.nMemTotal)/1000);
			break;

		case 4:
			sprintf(text, "MAIN NET:%s", (0==m_pUnsStatShm->tagPeerStat.nMainStat)?"OFF":"ON");
			break;
		case 5:
			sprintf(text, "B1:%s", (0==m_pUnsStatShm->tagPeerStat.nB1Stat)?"OFF":"ON");
			break;
		case 6:
			sprintf(text, "B2:%s", (0==m_pUnsStatShm->tagPeerStat.nB2Stat)?"OFF":"ON");
			break;
		case 7:
			sprintf(text, "B3:%s", (0==m_pUnsStatShm->tagPeerStat.nB3Stat)?"OFF":"ON");
			break;
		case 8:
			sprintf(text, "B4:%s", (0==m_pUnsStatShm->tagPeerStat.nB4Stat)?"OFF":"ON");
			break;

		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
			nE1Index = nCurLine-9;
			sprintf(text, "E1-%d:", nE1Index+1);
			if (0 == m_pUnsStatShm->tagE1Stat[nE1Index].nE1Stat)
			{
				strcat(text, "NORMAL");
			}
			else
			{
				if (m_pUnsStatShm->tagE1Stat[nE1Index].nE1Stat & 0x01){strcat(text, "LOS");bFirst=FALSE;}
				if (m_pUnsStatShm->tagE1Stat[nE1Index].nE1Stat & 0x02){strcat(text, bFirst?"AIS":";AIS");bFirst=FALSE;}
				if (m_pUnsStatShm->tagE1Stat[nE1Index].nE1Stat & 0x04){strcat(text, bFirst?"LOOP":";LOOP");bFirst=FALSE;}
				if (m_pUnsStatShm->tagE1Stat[nE1Index].nE1Stat & 0x08){strcat(text, bFirst?"MLOF":";MLOF");bFirst=FALSE;}
				if (m_pUnsStatShm->tagE1Stat[nE1Index].nE1Stat & 0x10){strcat(text, bFirst?"RDI":";RDI");bFirst=FALSE;}
				if (m_pUnsStatShm->tagE1Stat[nE1Index].nE1Stat & 0x20){strcat(text, bFirst?"CRC":";CRC");}
			}
			break;

		default:
			break;
	}
	
	m_pLMB_SCREEN_SERIAL->f_WriteOneLine(text, nCurLine);
}

void CFCServerFacade::handle_LBM_show()
{
	//更新系统信息
	m_pProbeSysInfo->m_cpuInfo.Refresh();
	m_pUnsStatShm->tagSysInfo.nCpuRate = m_pProbeSysInfo->m_cpuInfo.m_uCurrentUsage[0];

	m_pProbeSysInfo->m_memInfo.Refresh();
	m_pUnsStatShm->tagSysInfo.nMemUsed = (m_pProbeSysInfo->m_memInfo.m_uPhysicalMem - m_pProbeSysInfo->m_memInfo.m_uFreePhysicalMem)/1000;

	m_pProbeSysInfo->m_diskInfo.Refresh();
	UINT32 nDiskNum = m_pProbeSysInfo->m_diskInfo.m_vtDisk.size();
	UINT32 disk_size_used = 0;
	for(UINT32 i=0; i<nDiskNum; i++)
	{
		disk_size_used += m_pProbeSysInfo->m_diskInfo.m_vtDisk[i].nUsed; // 已使用空间(单位M)
	}
	m_pUnsStatShm->tagSysInfo.nHddUsed = disk_size_used/1000;

	m_pUnsStatShm->tagSysInfo.nSysRunTime = GetTickCount_s();

	//
	f_RefreshLmbData(m_pLMB_SCREEN_SERIAL->f_GetCurLine());
	f_RefreshLmbData(m_pLMB_SCREEN_SERIAL->f_GetCurLine()+1);
	m_pLMB_SCREEN_SERIAL->f_ScreenPrintf();

	if (m_pLMB_SCREEN_SERIAL->f_ScreenTimeout(0))
	{
		m_pLMB_SCREEN_SERIAL->API_LMB_OPERATE(SERIAL_OPERATE_BACKGROUND_OFF, NULL, 0);
	}
}

