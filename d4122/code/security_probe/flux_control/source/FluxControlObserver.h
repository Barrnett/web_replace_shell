//////////////////////////////////////////////////////////////////////////
//FluxControlObserver.h

#pragma once

//#include "flux_control.h"
#include "PacketIO/MyBaseObserver.h"
#include "flux_control.h"
#include "InterfaceCfg.h"
#include "InterfaceFactory.h"
#include "SerialInterface.h"
#include "cpf/PrivateNet.h"
#include "cpf/IniFile.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "MyCapDataMgr.h"

#include "cpf/ComImp.h"
#include "cpf/CPF_ShareMem.h"

#include "MirrorOutConfig.h"
#include "MirrorOutDataMgr.h"

#include "TestControlParam.h"

#include "flux_control_base.h"
#include "TS_define.h"
#include "UNS_define.h"
#include "ts_module/TS_Config.h"
#include "ts_module/TS_StatMgmt.h"
#include "ts_module/TS_MainNet.h"
#include "ts_module/TS_MainNetL2.h"
#include "ts_module/TS_MainNetL3.h"
#include "ts_module/TS_MainNetLan.h"
#include "ts_module/TS_BackNet.h"
#include "ts_module/TS_BackNetL2.h"
#include "ts_module/TS_BackNetL3.h"
#include "ts_module/TS_BackNetLan.h"
#include "ts_module/TS_BackCom.h"
#include "ts_module/TS_BackComL2.h"
#include "ts_module/TS_BackComL3.h"
#include "ts_module/TS_BackComLan.h"
#include "ts_module/TS_Mirror.h"
#include "ts_module/TS_MirrorL2.h"
#include "ts_module/TS_MirrorL3.h"
#include "ts_module/TS_MirrorLan.h"
#include "ts_module/TS_AccessMgmt.h"
#include "ts_module/TS_PeerNode.h"
#include "ts_module/TS_Statistic.h"
#include "ts_module/TS_SglCard.h"

class CProbeNetCom;
class CEtherioWriteDataStream;
class CSaveBeforeSendPacketHandle;
class CArpTableEx;

class CFluxControlObserver
{
public:
	CFluxControlObserver(void);
	virtual ~CFluxControlObserver(void);

public:
	BOOL init(ACE_Log_File_Msg * plog_file,const char * drv_name);
	void fin();

public:

	BOOL OnStart();
	BOOL OnStop(int type);
	
	BOOL PushPacket(PACKET_LEN_TYPE type,RECV_PACKET& packet);

public:
	int OnMirrorOutConfig(const CMirrorOutConfig& new_config);

public:
	CMirrorOutDataMgr	m_mirrorout_mgr;

	BOOL StartCapDataForClient(BOOL bUsdFlt,
		std::vector< std::string > vt_from_card_name,
		int ncapsize_mb,int ncaptime_sec,
		std::string& str_url);

	void StopCapData(BOOL bDeleteFile);
	int GetCapStatus(int& pos);

	BOOL StartCapDataForTest(DRIVER_TYPE drv_type,
		BOOL bUsdFlt,
		const char * flt_file_name,
		std::vector< std::string > vt_from_card_name,
		int ncapsize_mb,int file_nums,
		int ncaptime_sec,
		const char * file_path,
		const char * file_name);

	BOOL DoOutPut();

public:
	inline void SetRebootHandler(IFCRebootHandler * pFCRebootHandler)
	{
		m_pFCRebootHandler = pFCRebootHandler;
	}

public:
	int RebootHandler(int type,int reason)
	{
		if( m_pFCRebootHandler )
			return m_pFCRebootHandler->Do(type,reason);
//		else
//			ACE_ASSERT(FALSE);

		return -1;
	}

public:
	CInterfaceFactory *		m_pInterfaceCfgFactory;

	inline CInterfaceCfg * GetInterfaceCfg()
	{
		if( m_pInterfaceCfgFactory )
			return m_pInterfaceCfgFactory->GetInterfaceCfg();

		return NULL;
	}

	inline const CInterfaceCfg * GetInterfaceCfg() const
	{
		if( m_pInterfaceCfgFactory )
			return m_pInterfaceCfgFactory->GetInterfaceCfg();

		return NULL;
	}

public:
	CSerialInterface*		m_pSerialInterface;

private:
	CSaveBeforeSendPacketHandle *	m_pSaveBeforeSendPacketHandle;
	
	PACKET_INFO_EX m_tagPacketInfo;

public:
	//output_card_index对于路由模式，outputcard_index已经计算好了，直接给流控模块
	int HandlePacket(PACKET_INFO_EX & packet_info);
	
	void HandleTimer(RECV_PACKET & packet);

	void HandleFirstPacket(const CTimeStamp32& cur_time);

public:
	ACE_Log_File_Msg*	m_pLogFile;
	
	// TS 的配置 
	CTS_CONFIG* m_piTsCfg;

	ACE_UINT32 m_nUpdateCfg;//修改配置升级进度(0:启动配置升级；100:正确完成；101:配置文件出错)

	CTS_MainNet* m_piMainNet;

	CTS_Mirror* m_piMirror;

	CTS_BackNet* m_piBackNet1;

	CTS_BackNet* m_piBackNet2;

	CTS_BackCom* m_piBackCom1;

	CTS_BackCom* m_piBackCom2;

	CTS_AccessMgmt* m_piAccessMgmt;//本地管理口模块

	CTS_Statistic* m_piStatistic;
	
	CTS_SglCard* m_piSglCard1;

	CTS_SglCard* m_piSglCard2;

	CTS_PeerNode* m_piPeerNode;

	CTS_StatMgmt* m_piTsStatMgmt;

	CArpTableEx*			m_pArpTable;

public:
	CMirrorOutConfig		m_mirror_out_config;
	CMyCapDataMgrEx		m_cap_data_mgr_ex;

public:
	int TS_init(CTS_InterfaceCfg * pInterfaceCfg, CArpTableEx * pArpTable);
	int TS_UpdateConfig(void);
	int TS_ForwardIpOut(PACKET_INFO_EX & packet_info);
	int TS_ForwardArpOut(PACKET_INFO_EX & packet_info);
	int TS_ForwardIpIn(PACKET_INFO_EX & packet_info);
	int TS_ForwardArpIn(PACKET_INFO_EX & packet_info);
	int TS_HandlePacket(PACKET_INFO_EX & packet_info);
	int TS_HandleSerialPacket(RECV_PACKET & packet);
	void TS_HandleTimer(RECV_PACKET & packet);	

public:
	//将数据发送到当前收数据的网卡
	int SendPacketToCurInputCard(DIR_IO dir_phy,const char * packet,int packet_len,int send_nums);

	int SendPacketToCard(int card_index,const char * packet,int packet_len,int send_nums);

public:
	BOOL GetProbeIfDynInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	BOOL GetProbeAllIfCardInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	BOOL GetAllIPCardInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	
	BOOL GetNetIfConfigRunStatus(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	BOOL OnCmd_GetProbeAllRunCardInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	//BOOL GetProbeDebugInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	

private:
	void SendPktToCard(IWriteDataStream * pWriteDataStream,BOOL bIsInternetVM,int output_card_index,const PACKET_INFO_EX& packet_info);

private:
	BOOL LoadProbeNetcom();
	void UnLoadProbeNetcom();

	BOOL GetProbeConfigPath();
	BOOL GetPACopyDataInfo();

public:
	ACE_UINT32		m_mgr_hostorder_ip;//管理口ip地址
	std::string		m_gr_host_name;//本身设备的主机名

public:
	int OnIPChanged();

private:
	BOOL GetManagerPortIP();

private:
	CProbeNetCom *			m_pProbeNetCom;

private:
	IFCRebootHandler *					m_pFCRebootHandler;

private:
	std::string							m_str_drv_name;

private:
	ACE_Recursive_Thread_Mutex	m_tm;

public:
	friend class CChannleCtrlMgr_LOCKER_ATOM;

};

extern CFluxControlObserver *			g_pFluxControlObserver;


//用于界面下发配置策略和处理数据线程之间的同步
//处理线程必须在完整处理完一个数据包之后，才能修改配置

class CChannleCtrlMgr_LOCKER_ATOM
{
public:
	CChannleCtrlMgr_LOCKER_ATOM()
	{
		g_pFluxControlObserver->m_tm.acquire();
	}

	~CChannleCtrlMgr_LOCKER_ATOM()
	{
		g_pFluxControlObserver->m_tm.release();
	}
};

