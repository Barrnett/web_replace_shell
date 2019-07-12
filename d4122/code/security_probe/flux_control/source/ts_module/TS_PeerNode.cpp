#include "TS_PeerNode.h"

CTS_PeerNode::CTS_PeerNode()
{
	m_vtPeerStat.clear();
}

CTS_PeerNode::~CTS_PeerNode()
{
	m_vtPeerStat.clear();
}

BOOL CTS_PeerNode::f_ModInit(CTS_CONFIG* pTsConfig, ACE_Log_File_Msg* pLogFile)
{
	m_pTsCfg = pTsConfig;
	m_pLogFile = pLogFile;

	size_t max = pTsConfig->m_vtPeerCfg.size();
	for (size_t i=0; i<max; i++)
	{
		m_vtPeerStat.push_back(DISCONNECT);
	}
	
	return TRUE;
}

BOOL CTS_PeerNode::f_ModUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	if (m_pTsCfg->m_vtPeerCfg != pNewTsCfg->m_vtPeerCfg)
	{
		size_t max = m_pTsCfg->m_vtPeerCfg.size();
		size_t i;

		std::vector<TS_PEER_STAT_E>::iterator iter;
		
		for (i=0, iter=m_vtPeerStat.begin(); i<max; i++)
		{
			CTS_PEER_CONFIG* pNewPeerCfg = pNewTsCfg->f_FindPeerCfg(m_pTsCfg->m_vtPeerCfg[i].m_nPeerId);
			if (NULL == pNewPeerCfg)
			{
				iter = m_vtPeerStat.erase(iter);
			}
			else
			{
				if (m_pTsCfg->m_vtPeerCfg[i] != *pNewPeerCfg)
				{
					*iter = DISCONNECT;
				}
				
				iter++;
			}
		}
		
		max = pNewTsCfg->m_vtPeerCfg.size();
		for (i=0; i<max; i++)
		{
			CTS_PEER_CONFIG* pOldPeerCfg = m_pTsCfg->f_FindPeerCfg(pNewTsCfg->m_vtPeerCfg[i].m_nPeerId);
			if (NULL == pOldPeerCfg)
			{
				m_vtPeerStat.push_back(DISCONNECT);
			}
		}
	}

	return TRUE;
}
