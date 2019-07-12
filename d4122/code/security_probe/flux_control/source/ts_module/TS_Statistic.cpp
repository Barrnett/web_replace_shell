#include "TS_Statistic.h"
#include "TS_Base.h"

CTS_Statistic::CTS_Statistic()
{
	m_vtFlowStatistic.clear();
}

CTS_Statistic::~CTS_Statistic()
{
	m_vtFlowStatistic.clear();
}

BOOL CTS_Statistic::f_ModInit(CTS_CONFIG* pTsConfig)
{
	m_pTsCfg = pTsConfig;
	
	size_t max = pTsConfig->m_vtFilterCfg.size();
	for (size_t i=0; i<max; i++)
	{
		TS_FLOW_STATISTIC_S tagFlowStatistic;
		
		tagFlowStatistic.nFlowId = pTsConfig->m_vtFilterCfg[i].id;
		tagFlowStatistic.n64UpBytes = 0;
		tagFlowStatistic.n64UpPackets = 0;
		
		m_vtFlowStatistic.push_back(tagFlowStatistic);
	}

	return TRUE;
}

BOOL CTS_Statistic::f_ModUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	if (m_pTsCfg->m_vtFilterCfg != pNewTsCfg->m_vtFilterCfg)
	{
		size_t max = m_pTsCfg->m_vtFilterCfg.size();
		size_t i;

		std::vector<TS_FLOW_STATISTIC_S>::iterator iter;

		for (i=0, iter=m_vtFlowStatistic.begin(); i<max; i++)
		{
			CTS_FILTER_CONFIG* pNewFilterCfg = pNewTsCfg->f_FindFilter(m_pTsCfg->m_vtFilterCfg[i].id);
			if (NULL == pNewFilterCfg)
			{
				iter = m_vtFlowStatistic.erase(iter);
			}
			else
			{
				if (m_pTsCfg->m_vtFilterCfg[i] != *pNewFilterCfg)
				{
					//*iter.nFlowId = pNewTsCfg->m_vtFilterCfg[i].id;
					(*iter).n64UpBytes = 0;
					(*iter).n64UpPackets = 0;
				}
				
				iter++;
			}
		}

		max = pNewTsCfg->m_vtFilterCfg.size();
		for (i=0; i<max; i++)
		{
			CTS_FILTER_CONFIG* pOldFilterCfg = m_pTsCfg->f_FindFilter(pNewTsCfg->m_vtFilterCfg[i].id);
			if (NULL == pOldFilterCfg)
			{
				TS_FLOW_STATISTIC_S tagFlowStatistic;
				
				tagFlowStatistic.nFlowId = pNewTsCfg->m_vtFilterCfg[i].id;
				tagFlowStatistic.n64UpBytes = 0;
				tagFlowStatistic.n64UpPackets = 0;
				
				m_vtFlowStatistic.push_back(tagFlowStatistic);
			}
		}
	}

	m_pTsCfg = pNewTsCfg;
	
	return TRUE;
}

void CTS_Statistic::f_FlowStatistic(PACKET_INFO_EX& packet_info)
{
	size_t max = m_pTsCfg->m_vtFilterCfg.size();
	for (size_t i=0; i<max; i++)
	{
		if (TRUE == f_cmp_pkt_with_filter(packet_info, &(m_pTsCfg->m_vtFilterCfg[i]))) // ÒµÎñÆ¥Åä
		{
			m_vtFlowStatistic[i].n64UpBytes += packet_info.packet->nPktlen;
			m_vtFlowStatistic[i].n64UpPackets++;
			return;
		}
	}
}
