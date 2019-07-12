#if !defined(__TS_STATISTIC__)
#define __TS_STATISTIC__

#include "cpf/ostypedef.h"
#include "tcpip_mgr_common_init.h"
#include "TS_Config.h"
#include "TS_define.h"


typedef struct TS_FLOW_STATISTIC
{
	// ҵ��ID
	UINT32 nFlowId;
	// ��������(�ֽ�)
	UINT64 n64UpBytes;
	// ������̫��֡����
	UINT64 n64UpPackets;
}TS_FLOW_STATISTIC_S;

class CTS_Statistic
{
public:
	CTS_Statistic();
	~CTS_Statistic();

public:
	CTS_CONFIG* m_pTsCfg;
	
	std::vector<TS_FLOW_STATISTIC_S> m_vtFlowStatistic;// ҵ������ͳ��

public:
	BOOL f_ModInit(CTS_CONFIG * pTsConfig);
	BOOL f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg);
	void f_FlowStatistic(PACKET_INFO_EX& packet_info);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

