//////////////////////////////////////////////////////////////////////////
//TS_StatMgmt.cpp

#include "stdafx.h"
#include <stdio.h>
#include "cpf/ostypedef.h"
#include "cpf/adapter_info_query.h"
#include "FluxControlObserver.h"
#include "TS_StatMgmt.h"

CTS_StatMgmt::CTS_StatMgmt(void)
{
	m_pObserver = NULL;
	m_pUnsStatShm = NULL;

	m_nTaskRun = 0;
}

CTS_StatMgmt::~CTS_StatMgmt(void)
{
	m_iUnsStatShm.fin();
}

int CTS_StatMgmt::f_ModInit(CFluxControlObserver* pObserver)
{
	m_pObserver = pObserver;
	
	char strCommand[128] = {0};
	ACE_OS::sprintf(strCommand, "touch %s", UNS_STAT_SHM_NAME);
	ACE_OS::system(strCommand);
	
	int first_time = 0;
	m_pUnsStatShm = (UNS_STAT_SHM_S*)m_iUnsStatShm.init(UNS_STAT_SHM_NAME, sizeof(UNS_STAT_SHM_S), first_time);
	if ((NULL == m_pUnsStatShm) || ((UNS_STAT_SHM_S*)-1 == m_pUnsStatShm))
	{
		return -2;
	}
	else
	{
		if (first_time)
		{
			memset(m_pUnsStatShm, 0, sizeof(UNS_STAT_SHM_S));
		}
		m_pUnsStatShm->aLocalMgmtIf[3] = m_pObserver->m_piAccessMgmt->m_nIfIndex;
		unsigned int tmp;
		if (1 == adapter_query_ip(m_pObserver->m_piTsCfg->m_iLocalMgmtCfg.m_strCardName.c_str(), tmp))
		{
			m_pUnsStatShm->aLocalMgmtIp[3] = (tmp>>24)&0xFF;
			m_pUnsStatShm->aLocalMgmtIp[2] = (tmp>>16)&0xFF;
			m_pUnsStatShm->aLocalMgmtIp[1] = (tmp>>8)&0xFF;
			m_pUnsStatShm->aLocalMgmtIp[0] = (tmp)&0xFF;
		}
		m_pUnsStatShm->nLanNum = MAX_LAN_NUM;
		m_pUnsStatShm->nE1Num = MAX_E1_NUM;
		m_pUnsStatShm->nPeerNum = MAX_PEER_NUM;
		m_pUnsStatShm->tagPeerStat.nPeerId = MAX_PEER_NUM;
		m_pUnsStatShm->nFlowNum = m_pObserver->m_piTsCfg->m_vtFilterCfg.size();
		if (MAX_FLOW_NUM < m_pUnsStatShm->nFlowNum)
		{
			m_pUnsStatShm->nFlowNum = MAX_FLOW_NUM;
		}
		for (UINT32 i=0; i<m_pUnsStatShm->nFlowNum; i++)
		{
			m_pUnsStatShm->tagFlowStat[i].nFlowId = m_pObserver->m_piTsCfg->m_vtFilterCfg[i].id;
		}
	}

	return 0;
}

int CTS_StatMgmt::f_ModUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	unsigned int tmp;
	if (1 == adapter_query_ip(m_pObserver->m_piTsCfg->m_iLocalMgmtCfg.m_strCardName.c_str(), tmp))
	{
		m_pUnsStatShm->aLocalMgmtIp[3] = (tmp>>24)&0xFF;
		m_pUnsStatShm->aLocalMgmtIp[2] = (tmp>>16)&0xFF;
		m_pUnsStatShm->aLocalMgmtIp[1] = (tmp>>8)&0xFF;
		m_pUnsStatShm->aLocalMgmtIp[0] = (tmp)&0xFF;
	}
		
	m_pUnsStatShm->nFlowNum = pNewTsCfg->m_vtFilterCfg.size();
	if (MAX_FLOW_NUM < m_pUnsStatShm->nFlowNum)
	{
		m_pUnsStatShm->nFlowNum = MAX_FLOW_NUM;
	}
	for (UINT32 i=0; i<m_pUnsStatShm->nFlowNum; i++)
	{
		m_pUnsStatShm->tagFlowStat[i].nFlowId = pNewTsCfg->m_vtFilterCfg[i].id;
	}
	
	return 0;
}

int CTS_StatMgmt::Start()
{
	m_nTaskRun = 1;

	activate();

	return 0;
}

int CTS_StatMgmt::Stop()
{
	m_nTaskRun = 0;

	wait();

	return 0;
}

int CTS_StatMgmt::Pause()
{
	m_nTaskRun = 2;

	while(3 != m_nTaskRun)
	{
		usleep(100);
	}

	return 0;
}

int CTS_StatMgmt::Continue()
{
	m_nTaskRun = 1;
}

int CTS_StatMgmt::svc(void)
{
	int ret;
	bool bChange = false;
	unsigned int i;

 	while(0 != m_nTaskRun)
	{
		switch(m_nTaskRun)
		{
			case 1:
				//更新E1状态到共享内存
				ret = m_pObserver->m_piSglCard1->f_GetE1Stat();
				if (TRUE == ret)
				{
					for (i=0; i<8; i++)
					{
						m_pUnsStatShm->tagE1Stat[i].nE1Stat = m_pObserver->m_piSglCard1->m_vtE1Stat[i];
					}
				}

				ret = m_pObserver->m_piSglCard2->f_GetE1Stat();
				if (TRUE == ret)
				{
					for (i=0; i<4; i++)
					{
						m_pUnsStatShm->tagE1Stat[i+8].nE1Stat = m_pObserver->m_piSglCard2->m_vtE1Stat[i];
					}
				}

				//更新信道状态至共享内存
				if (ECHO_OK == m_pObserver->m_piMainNet->m_vtPeerDct[0]->m_eHostStat)
				{
					m_pUnsStatShm->tagPeerStat.nMainStat = 2;
				}
				else
				{
					m_pUnsStatShm->tagPeerStat.nMainStat = 0;
				}

				if (ECHO_OK == m_pObserver->m_piBackNet1->m_vtPeerDct[0]->m_eHostStat)
				{
					if (SYNC_MODE == m_pObserver->m_piTsCfg->m_iGlobalCfg.m_eForwardMode)
					{
						m_pUnsStatShm->tagPeerStat.nB1Stat = 2;
					}
					else if (BACK_NET1_RUN == m_pObserver->m_piPeerNode->m_vtPeerStat[0])
					{
						m_pUnsStatShm->tagPeerStat.nB1Stat = 2;
					}
					else
					{
						m_pUnsStatShm->tagPeerStat.nB1Stat = 1;
					}
				}
				else
				{
					m_pUnsStatShm->tagPeerStat.nB1Stat = 0;
				}
				
				if (ECHO_OK == m_pObserver->m_piBackNet2->m_vtPeerDct[0]->m_eHostStat)
				{
					if (SYNC_MODE == m_pObserver->m_piTsCfg->m_iGlobalCfg.m_eForwardMode)
					{
						m_pUnsStatShm->tagPeerStat.nB2Stat = 2;
					}
					else if (BACK_NET2_RUN == m_pObserver->m_piPeerNode->m_vtPeerStat[0])
					{
						m_pUnsStatShm->tagPeerStat.nB2Stat = 2;
					}
					else
					{
						m_pUnsStatShm->tagPeerStat.nB2Stat = 1;
					}
				}
				else
				{
					m_pUnsStatShm->tagPeerStat.nB2Stat = 0;
				}

				if (ECHO_OK == m_pObserver->m_piBackCom1->m_vtPeerDct[0]->m_eHostStat)
				{
					if (SYNC_MODE == m_pObserver->m_piTsCfg->m_iGlobalCfg.m_eForwardMode)
					{
						m_pUnsStatShm->tagPeerStat.nB3Stat = 2;
					}
					else if (BACK_COM1_RUN == m_pObserver->m_piPeerNode->m_vtPeerStat[0])
					{
						m_pUnsStatShm->tagPeerStat.nB3Stat = 2;
					}
					else
					{
						m_pUnsStatShm->tagPeerStat.nB3Stat = 1;
					}
				}
				else
				{
					m_pUnsStatShm->tagPeerStat.nB3Stat = 0;
				}

				if (ECHO_OK == m_pObserver->m_piBackCom2->m_vtPeerDct[0]->m_eHostStat)
				{
					if (SYNC_MODE == m_pObserver->m_piTsCfg->m_iGlobalCfg.m_eForwardMode)
					{
						m_pUnsStatShm->tagPeerStat.nB4Stat = 2;
					}
					else if (BACK_COM2_RUN == m_pObserver->m_piPeerNode->m_vtPeerStat[0])
					{
						m_pUnsStatShm->tagPeerStat.nB4Stat = 2;
					}
					else
					{
						m_pUnsStatShm->tagPeerStat.nB4Stat = 1;
					}
				}
				else
				{
					m_pUnsStatShm->tagPeerStat.nB4Stat = 0;
				}

				//更新流量统计至共享内存
				for (i=0; i<m_pUnsStatShm->nFlowNum; i++)
				{
					//m_pUnsStatShm->tagFlowStat[i].nFlowId = m_pObserver->m_piStatistic->m_vtFlowStatistic[i].nFlowId;
					m_pUnsStatShm->tagFlowStat[i].nUpBytes = m_pObserver->m_piStatistic->m_vtFlowStatistic[i].n64UpBytes;
					m_pUnsStatShm->tagFlowStat[i].nUpPackets = m_pObserver->m_piStatistic->m_vtFlowStatistic[i].n64UpPackets;
				}

				if (!bChange)	// 没有改变，sleep
				{
					ACE_OS::sleep(5);
				}
				break;

			case 2:
				m_nTaskRun = 3;
				break;

			case 3:
				ACE_OS::sleep(1);
				break;
		}
	}

	return 0;
}

