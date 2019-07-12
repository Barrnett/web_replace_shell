#if  !defined(__TS_MAINNET__)
#define __TS_MAINNET__
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// �߼�����CK���ඨ��
#include "cpf/ostypedef.h"
#include "flux_control_common.h"
#include "tcpip_mgr_common_init.h"
#include "TS_Config.h"
#include "TS_Base.h"
#include "TS_OneWanLineStatusManager.h"
#include "TS_define.h"
//////////////////////////////////////////////////////////

// ·�����������(��), ��ÿ����ö�·�������һ��
#define TS_CHECK_R_PERIOD (2)
// ��·�������ʱ,����ⷴӦʱ��,���ӷ���ARP����,���յ���Ӧ���������ʱ��
#define TS_CHECK_R_TIMEOUT (2)

// Զ�������������(��)
#define TS_CHECK_DES_PERIOD (1)
// ���Ӧʱ��
#define TS_CHECK_DES_TIMEOUT (1)

// ping�Ż���,�����Ͷ�����
#define TS_CHECK_PING_COUNT (5)
#define TS_CHECK_PING_LOST (20)


// �߼�����CK���ඨ��
class CTS_MainNet
{
public:
	CTS_MainNet();
	virtual ~CTS_MainNet();

public:
	CTS_InterfaceCfg* m_pTsInterfaceCfg;
	CTS_CONFIG* m_pTsCfg;
	ACE_Log_File_Msg* m_pLogFile;
	CArpTableEx* m_pArpTable;

	ACE_UINT32 m_nIfIndex;

	ACE_UINT32 m_hlIp;
	ACE_UINT32 m_hlNetmask;

	ACE_UINT8 m_aMacAddr[6];
	ACE_INT16 m_nIfStatus;//-1:�˿ڱ�Ϊdown; 0:�˿�down�Ѵ���; 10/100/1000:�˿�����״̬
	
	// Զ�������̽��Ƶ��
	ACE_UINT32 m_nChkPeriod;
	ACE_UINT32 m_nChkQueue; // ping��ļ�¼����
	ACE_UINT32 m_nChkLostNum;
	ACE_UINT32 m_nChkReplyNum;
	ACE_UINT16 m_nChkEchoSeq;

	CTS_HOST_DETECT* m_piGatewayDct;

	std::vector<CTS_HOST_DETECT*> m_vtPeerDct;
	
public:
	virtual BOOL f_ModInit(CTS_InterfaceCfg * pTsInterfaceCfg, CTS_CONFIG * pTsConfig, ACE_Log_File_Msg * pLogFile, CArpTableEx * pArpTable) = 0;
	virtual BOOL f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg) = 0;
	virtual BOOL f_HandleArp(PACKET_INFO_EX & packet_info) = 0;
	virtual BOOL f_HandleIp(PACKET_INFO_EX & packet_info) = 0;
	virtual BOOL f_HandlePacket(PACKET_INFO_EX& packet_info) = 0;
	virtual void f_MainNetCheck(ACE_UINT32 now_time) = 0;

	virtual void f_SendArpReq(ACE_UINT32 dst_ip);
	virtual void f_SendEchoReq(ACE_UINT32 hlHostIp, ACE_UINT8 * dstMac);
	virtual int f_ForwardIp(PACKET_INFO_EX & packet_info);
	virtual int f_ForwardArp(PACKET_INFO_EX & packet_info);
	virtual int f_ForwardPacket(PACKET_INFO_EX & packet_info);

	virtual BOOL f_IsPeerTestIP(ACE_UINT32 hlIp, ACE_UINT32 & peer_index);
};

#endif

