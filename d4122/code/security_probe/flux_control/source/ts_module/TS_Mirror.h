#if !defined(__TS_MIRROR__)
#define __TS_MIRROR__

#include "cpf/ostypedef.h"
#include "cpf/CommonQueueManager.h"
#include "flux_control_common.h"
#include "tcpip_mgr_common_init.h"
#include "TS_Config.h"
#include "TS_Base.h"
#include "TS_define.h"

class CTS_Mirror
{
public:
	CTS_Mirror();
	virtual ~CTS_Mirror();

public:
	CTS_CONFIG* m_pTsCfg;
	ACE_Log_File_Msg *m_pLogFile;
	
	// M网口使用的物理网口编号
	ACE_UINT32 m_nIfIndex;

	ACE_UINT8 m_aMacAddr[6];
	
public:
	virtual BOOL f_ModInit(CTS_CONFIG * pTsConfig, ACE_Log_File_Msg * pLogFile);

	virtual BOOL f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg);

	virtual BOOL f_HandleArp(PACKET_INFO_EX & packet_info);
	virtual BOOL f_HandleIp(PACKET_INFO_EX & packet_info);
	virtual BOOL f_HandlePacket(PACKET_INFO_EX & packet_info);
};

#endif

