#if !defined(__TS_BACKNET__)
#define __TS_BACKNET__

#include "cpf/ostypedef.h"
#include "flux_control_common.h"
#include "tcpip_mgr_common_init.h"
#include "TS_Config.h"
#include "TS_Base.h"
#include "TS_OneWanLineStatusManager.h"
#include "TS_define.h"

class CTS_BackNet
{
public:
	CTS_BackNet();
	virtual ~CTS_BackNet();

public:
	CTS_InterfaceCfg*		m_pTsInterfaceCfg;
	CTS_CONFIG*			m_pTsCfg;
	ACE_Log_File_Msg*	m_pLogFile;
		
	// 备用网口使用的物理网口编号
	ACE_UINT32 m_nIfIndex;

	ACE_UINT32 m_hlIp;
	ACE_UINT32 m_hlNetmask;
	ACE_UINT32 m_hlSubnet;

	ACE_UINT8 m_aMacAddr[6];
	ACE_INT16 m_nIfStatus;//-1:端口变为down; 0:端口down已处理; 10/100/1000:端口速率状态

	// 远程网络的探测频率
	ACE_UINT32 m_nChkPeriod;
	ACE_UINT32 m_nChkQueue; // ping表的记录个数
	ACE_UINT32 m_nChkLostNum;
	ACE_UINT32 m_nChkReplyNum;
	ACE_UINT16 m_nChkEchoSeq;

	CTS_HOST_DETECT* m_piGatewayDct;
	
	std::vector<CTS_HOST_DETECT*> m_vtPeerDct;

	CTS_PACKET_FILTER* m_piPeerPktFilter;

	std::vector<CTS_PACKET_QUEUE*> m_vtPeerPktQueue;

public:
	virtual BOOL f_ModInit(CTS_InterfaceCfg * pTsInterfaceCfg, CTS_CONFIG * pTsConfig, ACE_Log_File_Msg * pLogFile) = 0;
	virtual BOOL f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg) = 0;
	virtual void f_BackNetCheck(ACE_UINT32 now_time) = 0;
	

	virtual void f_HandleMyIcmp(PACKET_INFO_EX & packet_info);
	virtual BOOL f_HandleArp(PACKET_INFO_EX & packet_info);
	virtual BOOL f_HandleIp(PACKET_INFO_EX & packet_info);
	virtual BOOL f_HandlePacket(PACKET_INFO_EX & packet_info);

	virtual void f_SendEchoReq(ACE_UINT32 hlHostIp, ACE_UINT8 * dstMac);
	virtual void f_SendArpReq(ACE_UINT32 dst_ip);


	virtual BOOL f_ForwardIp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index) = 0;
	virtual BOOL f_MulticastIp(PACKET_INFO_EX & packet_info) = 0;
	virtual BOOL f_ForwardArp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index) = 0;
	virtual BOOL f_PushPktQueue(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index) = 0;
	virtual BOOL f_FlushPktQueue(ACE_UINT32 des_index) = 0;

	virtual BOOL f_IsPeerTestIP(ACE_UINT32 hlIp, ACE_UINT32 & peer_index);
	virtual BOOL f_IsSubNetIp(ACE_UINT32 hlIp);
};

class CTS_BackNetEoSgl1 : virtual public CTS_BackNet
{
public:
	CTS_BackNetEoSgl1();
	virtual ~CTS_BackNetEoSgl1();

public:
	BOOL f_ModInit(CTS_InterfaceCfg * pTsInterfaceCfg, CTS_CONFIG * pTsConfig, ACE_Log_File_Msg * pLogFile);
	BOOL f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg);
	BOOL f_PeerInit(CTS_CONFIG * pTsConfig);
	BOOL f_PeerUpdateCfg(CTS_CONFIG * pNewTsCfg);
	BOOL f_FilterInit(CTS_CONFIG * pTsConfig);
	BOOL f_FilterUpdateCfg(CTS_CONFIG * pNewTsCfg);
	BOOL f_QueueInit(CTS_CONFIG * pTsConfig);
	BOOL f_QueueUpdateCfg(CTS_CONFIG * pNewTsCfg);
	void f_BackNetCheck(ACE_UINT32 now_time);
};

class CTS_BackNetEoSgl2 : virtual public CTS_BackNet
{
public:
	CTS_BackNetEoSgl2();
	virtual ~CTS_BackNetEoSgl2();

public:
	BOOL f_ModInit(CTS_InterfaceCfg * pTsInterfaceCfg, CTS_CONFIG * pTsConfig, ACE_Log_File_Msg * pLogFile);
	BOOL f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg);
	BOOL f_PeerInit(CTS_CONFIG * pTsConfig);
	BOOL f_PeerUpdateCfg(CTS_CONFIG * pNewTsCfg);
	BOOL f_FilterInit(CTS_CONFIG * pTsConfig);
	BOOL f_FilterUpdateCfg(CTS_CONFIG * pNewTsCfg);
	BOOL f_QueueInit(CTS_CONFIG * pTsConfig);
	BOOL f_QueueUpdateCfg(CTS_CONFIG * pNewTsCfg);
	void f_BackNetCheck(ACE_UINT32 now_time);
};

class CTS_BackNetEoL2 : virtual public CTS_BackNet
{
public:
	CTS_BackNetEoL2();
	virtual ~CTS_BackNetEoL2();

public:
	BOOL f_ModInit(CTS_InterfaceCfg * pTsInterfaceCfg, CTS_CONFIG * pTsConfig, ACE_Log_File_Msg * pLogFile);
	BOOL f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg);
	BOOL f_PeerInit(CTS_CONFIG * pTsConfig);
	BOOL f_PeerUpdateCfg(CTS_CONFIG * pNewTsCfg);
	BOOL f_FilterInit(CTS_CONFIG * pTsConfig);
	BOOL f_FilterUpdateCfg(CTS_CONFIG * pNewTsCfg);
	BOOL f_QueueInit(CTS_CONFIG * pTsConfig);
	BOOL f_QueueUpdateCfg(CTS_CONFIG * pNewTsCfg);
	void f_BackNetCheck(ACE_UINT32 now_time);
};

class CTS_BackNetEoL3 : virtual public CTS_BackNet
{
public:
	CTS_BackNetEoL3();
	virtual ~CTS_BackNetEoL3();

public:
	BOOL f_ModInit(CTS_InterfaceCfg * pTsInterfaceCfg, CTS_CONFIG * pTsConfig, ACE_Log_File_Msg * pLogFile);
	BOOL f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg);
	BOOL f_PeerInit(CTS_CONFIG * pTsConfig);
	BOOL f_PeerUpdateCfg(CTS_CONFIG * pNewTsCfg);
	BOOL f_FilterInit(CTS_CONFIG * pTsConfig);
	BOOL f_FilterUpdateCfg(CTS_CONFIG * pNewTsCfg);
	BOOL f_QueueInit(CTS_CONFIG * pTsConfig);
	BOOL f_QueueUpdateCfg(CTS_CONFIG * pNewTsCfg);
	BOOL f_HandleArp(PACKET_INFO_EX & packet_info);
	BOOL f_HandleIp(PACKET_INFO_EX & packet_info);
	void f_PeerDetect(ACE_UINT32 now_time);
	void f_BackNetCheck(ACE_UINT32 now_time);
};

#endif

