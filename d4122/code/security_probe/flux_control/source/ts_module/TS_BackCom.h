#if !defined(__TS_BACKCOM__)
#define __TS_BACKCOM__

#include "cpf/ostypedef.h"
#include "flux_control_common.h"
#include "tcpip_mgr_common_init.h"
#include "TS_Config.h"
#include "TS_Base.h"
#include "TS_define.h"
#include "SerialInterface.h"

class CTS_BackCom
{
public:
	CTS_BackCom();
	virtual ~CTS_BackCom();

public:
	CSerialInterface*		m_pTsSerialInterface;
	CTS_CONFIG*			m_pTsCfg;
	ACE_Log_File_Msg*	m_pLogFile;
	CTS_BACKCOM_CONFIG*	m_pBackComCfg;
		
	//´®¿Ú±àºÅ
	ACE_UINT8 m_nComIndex;

	ACE_UINT32 m_nChkPeriod;
	
	std::vector<CTS_HOST_DETECT*> m_vtPeerDct;

	CTS_PACKET_FILTER* m_piPeerPktFilter;

public:
	virtual BOOL f_ModInit(CSerialInterface * pTsSerialInterface, CTS_CONFIG * pTsConfig, CTS_BACKCOM_CONFIG * pBackComConfig, ACE_Log_File_Msg * pLogFile);
	virtual BOOL f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg, CTS_BACKCOM_CONFIG * pNewBackComConfig);
	virtual BOOL f_PeerInit(CTS_CONFIG * pTsConfig);
	virtual BOOL f_FilterInit(CTS_CONFIG * pTsConfig, CTS_BACKCOM_CONFIG * pBackComConfig);
	virtual BOOL f_FilterUpdateCfg(CTS_CONFIG * pNewTsCfg, CTS_BACKCOM_CONFIG * pNewBackComConfig);

	virtual void f_SendEchoReq();
	virtual BOOL f_ForwardIp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index) = 0;
	virtual BOOL f_MulticastIp(PACKET_INFO_EX & packet_info) = 0;
	virtual BOOL f_ForwardArp(PACKET_INFO_EX & packet_info, ACE_UINT32 des_index) = 0;
	void f_BackComCheck(ACE_UINT32 now_time);
};

#endif

