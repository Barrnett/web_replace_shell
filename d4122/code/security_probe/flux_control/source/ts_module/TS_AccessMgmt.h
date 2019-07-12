#if !defined(__TS_ACCESSMGMT__)
#define __TS_ACCESSMGMT__

#include "cpf/ostypedef.h"
#include "flux_control_common.h"
#include "tcpip_mgr_common_init.h"
#include "TS_Config.h"
#include "TS_Base.h"
#include "TS_OneWanLineStatusManager.h"
#include "TS_define.h"

class CTS_AccessMgmt
{
public:
	CTS_AccessMgmt();
	virtual ~CTS_AccessMgmt();

public:
	CTS_InterfaceCfg * m_pTsInterfaceCfg;
	CTS_CONFIG* m_pTsCfg;
	ACE_Log_File_Msg* m_pLogFile;
	
	ACE_UINT32 m_nIfIndex;

	ACE_UINT32 m_hlIp;

	ACE_UINT32 m_hlNetmask;

	ACE_UINT8 m_aMacAddr[6];

	ACE_UINT32 m_hlGateway;

	ACE_UINT32	m_nPeerMgmt;

public:
	BOOL f_ModInit(CTS_InterfaceCfg * pInterfaceCfg, CTS_CONFIG * pTsConfig, ACE_Log_File_Msg * pLogFile);
	BOOL f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg);
	void f_HandleMyIcmp(PACKET_INFO_EX& packet_info);
	BOOL f_HandleIp(PACKET_INFO_EX& packet_info);
	BOOL f_HandlePacket(PACKET_INFO_EX& packet_info);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

