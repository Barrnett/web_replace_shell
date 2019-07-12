//////////////////////////////////////////////////////////////////////////
//FCLiveCmd.cpp

#include "stdafx.h"
#include "FCLiveDoCmd.h"
#include "SockOperationId.h"

#include "cpf/Data_Stream.h"
#include "FluxControlObserver.h"

CFCLiveDoCmd::CFCLiveDoCmd(void)
{
	m_pLogInstance = NULL;
}

CFCLiveDoCmd::~CFCLiveDoCmd(void)
{
}


void CFCLiveDoCmd::init(ACE_Log_File_Msg* pLogInstance)
{
	m_pLogInstance = pLogInstance;	

	return;
}

//#define SAFE_DELETE_OBJ(x)	if(x){delete x;x=NULL;}

void CFCLiveDoCmd::close()
{
	return;
}

int CFCLiveDoCmd::OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	int result = 0;
	
	switch(pMsgBlock->pPduHdr->nOperation)
	{
		case OP_ID_GET_E1_STATE:
			result = f_OnCmd_GetE1Stat(pRemoteAddr, pMsgBlock, pRetMsgBlock);
			break;
			
		case OP_ID_GET_TUNNEL_STATE:
			result = f_OnCmd_GetTunnelStatus(pRemoteAddr, pMsgBlock, pRetMsgBlock);
			break;

		case OP_ID_GET_SERVICE_STATISTIC:
			result = f_OnCmd_GetFlowStatistic(pRemoteAddr, pMsgBlock, pRetMsgBlock);
			break;


		default:
			{
				char buf[64] = {0};

				pRemoteAddr->addr_to_string(buf,sizeof(buf));

				MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Err][%D]recv %u cmd from '%s' is not defined!!!\n"),pMsgBlock->pPduHdr->nOperation,buf));

				//ACE_ASSERT(FALSE);

				return -1;
			}
			break;
	}
	
	if (0 != result)
	{
		MY_ACE_DEBUG(m_pLogInstance, (LM_ERROR, ACE_TEXT("[Err][%D]:testcontrol operation %d failed, return %d.\r\n"), pMsgBlock->pPduHdr->nOperation, result));
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, result, RESPONSE_CODE_OTHER);
	}
			
	if( pRetMsgBlock )
	{
		return pRetMsgBlock->pPduHdr->nResponseCode;
	}
	else
	{
		return -1;
	}
}

int CFCLiveDoCmd::f_OnCmd_GetE1Stat(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	int ret = g_pFluxControlObserver->m_piSglCard1->f_GetE1Stat();
	if (FALSE == ret)
	{
		return -2;
	}

	ret = g_pFluxControlObserver->m_piSglCard2->f_GetE1Stat();
	if (FALSE ==ret)
	{
		return -3;
	}
	
	// 返回信息
	ACE_UINT32 nE1Num1 = g_pFluxControlObserver->m_piTsCfg->m_iSglCard1.m_E1Max;
	ACE_UINT32 nE1Num2 = g_pFluxControlObserver->m_piTsCfg->m_iSglCard2.m_E1Max;
	ACE_UINT32 nE1Num = nE1Num1 + nE1Num2;
	
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, sizeof(UINT32)+nE1Num*sizeof(unsigned char)+16); // 开辟空间
	if (pRetMsgBlock)
	{
		char* pUsrDataHead = CMsgBlockManager::WritePtr(*pRetMsgBlock); // 用户数据区首地址
		char* pCurWrite = pUsrDataHead;


		CData_Stream_LE::PutUint32(pCurWrite, nE1Num); // E1个数
		for (ACE_UINT32 i=0; i<nE1Num1; i++)
		{
			CData_Stream_LE::PutUint8(pCurWrite, g_pFluxControlObserver->m_piSglCard1->m_vtE1Stat[i]); 
		}
		for (ACE_UINT32 i=0; i<nE1Num2; i++)
		{
			CData_Stream_LE::PutUint8(pCurWrite, g_pFluxControlObserver->m_piSglCard2->m_vtE1Stat[i]); 
		}

		pRetMsgBlock->nDataLen = (int)(pCurWrite - pUsrDataHead);
		
		return 0;
	}
	else
	{
		return -1;
	}
}

int CFCLiveDoCmd::f_OnCmd_GetTunnelStatus(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	// 返回信息
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, 128); // 开辟空间
	if (pRetMsgBlock)
	{
		char* pUsrDataHead = CMsgBlockManager::WritePtr(*pRetMsgBlock); // 用户数据区首地址
		char* pCurWrite = pUsrDataHead;

		CData_Stream_LE::PutUint32(pCurWrite, 5); // 信道个数
		if (ECHO_OK == g_pFluxControlObserver->m_piMainNet->m_vtPeerDct[0]->m_eHostStat)
		{ 
			CData_Stream_LE::PutUint8(pCurWrite, 2); 
		}
		else
		{ 
			CData_Stream_LE::PutUint8(pCurWrite, 0); 
		}
		if (ECHO_OK == g_pFluxControlObserver->m_piBackNet1->m_vtPeerDct[0]->m_eHostStat)
		{
			if (SYNC_MODE == g_pFluxControlObserver->m_piTsCfg->m_iGlobalCfg.m_eForwardMode)
			{
				CData_Stream_LE::PutUint8(pCurWrite, 2); 
			}
			else if (BACK_NET1_RUN == g_pFluxControlObserver->m_piPeerNode->m_vtPeerStat[0])
			{
				CData_Stream_LE::PutUint8(pCurWrite, 2); 
			}
			else
			{
				CData_Stream_LE::PutUint8(pCurWrite, 1); 
			}
		}
		else
		{ 
			CData_Stream_LE::PutUint8(pCurWrite, 0); 
		}
		if (ECHO_OK == g_pFluxControlObserver->m_piBackNet2->m_vtPeerDct[0]->m_eHostStat)
		{ 
			if (SYNC_MODE == g_pFluxControlObserver->m_piTsCfg->m_iGlobalCfg.m_eForwardMode)
			{
				CData_Stream_LE::PutUint8(pCurWrite, 2); 
			}
			else if (BACK_NET2_RUN == g_pFluxControlObserver->m_piPeerNode->m_vtPeerStat[0])
			{
				CData_Stream_LE::PutUint8(pCurWrite, 2); 
			}
			else
			{
				CData_Stream_LE::PutUint8(pCurWrite, 1); 
			}
		}
		else
		{ 
			CData_Stream_LE::PutUint8(pCurWrite, 0); 
		}
		if (ECHO_OK == g_pFluxControlObserver->m_piBackCom1->m_vtPeerDct[0]->m_eHostStat)
		{ 
			if (SYNC_MODE == g_pFluxControlObserver->m_piTsCfg->m_iGlobalCfg.m_eForwardMode)
			{
				CData_Stream_LE::PutUint8(pCurWrite, 2); 
			}
			else if (BACK_COM1_RUN == g_pFluxControlObserver->m_piPeerNode->m_vtPeerStat[0])
			{
				CData_Stream_LE::PutUint8(pCurWrite, 2); 
			}
			else
			{
				CData_Stream_LE::PutUint8(pCurWrite, 1); 
			}
		}
		else
		{ 
			CData_Stream_LE::PutUint8(pCurWrite, 0); 
		}
		if (ECHO_OK == g_pFluxControlObserver->m_piBackCom2->m_vtPeerDct[0]->m_eHostStat)
		{ 
			if (SYNC_MODE == g_pFluxControlObserver->m_piTsCfg->m_iGlobalCfg.m_eForwardMode)
			{
				CData_Stream_LE::PutUint8(pCurWrite, 2); 
			}
			else if (BACK_COM2_RUN == g_pFluxControlObserver->m_piPeerNode->m_vtPeerStat[0])
			{
				CData_Stream_LE::PutUint8(pCurWrite, 2); 
			}
			else
			{
				CData_Stream_LE::PutUint8(pCurWrite, 1); 
			}
		}
		else
		{ 
			CData_Stream_LE::PutUint8(pCurWrite, 0); 
		}

		pRetMsgBlock->nDataLen = (int)(pCurWrite - pUsrDataHead);
		
		return 0;
	}
	else
	{
		return -1;
	}
}

int CFCLiveDoCmd::f_OnCmd_GetFlowStatistic(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	ACE_UINT32 nFlowNum = g_pFluxControlObserver->m_piStatistic->m_vtFlowStatistic.size();
	
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, (sizeof(UINT32)+nFlowNum*sizeof(TS_FLOW_STATISTIC_S)+16));
	if (pRetMsgBlock)
	{
		char* pUsrDataHead = CMsgBlockManager::WritePtr(*pRetMsgBlock); // 用户数据区首地址
		char* pCurWrite = pUsrDataHead;
		
		CData_Stream_LE::PutUint32(pCurWrite, nFlowNum);
		for(int i=0; i<nFlowNum; i++)
		{
			// 业务ID
			CData_Stream_LE::PutUint32(pCurWrite, g_pFluxControlObserver->m_piStatistic->m_vtFlowStatistic[i].nFlowId);
			// 上行流量
			CData_Stream_LE::PutUint64(pCurWrite, g_pFluxControlObserver->m_piStatistic->m_vtFlowStatistic[i].n64UpBytes);
			// 上行以太网帧个数
			CData_Stream_LE::PutUint64(pCurWrite, g_pFluxControlObserver->m_piStatistic->m_vtFlowStatistic[i].n64UpPackets);
		} 
		pRetMsgBlock->nDataLen = (int)(pCurWrite - pUsrDataHead);

		return 0;
	}
	else
	{
		return -1;
	}
}

