//////////////////////////////////////////////////////////////////////////
//FCLiveDoCmd.cpp

#include "StdAfx.h"
#include "FCLiveDoCmd.h"
#include "SockManager.h"
#include "cpf/Data_Stream.h"
#include "SockOperationId.h"
#include "config_fc_server_xml.h"
#include "FCServerFacade.h"

CFCLiveDoCmd::CFCLiveDoCmd(void)
{
	m_pLogInstance = NULL;

	m_pFCSockManager = NULL;

	m_pFCServerFacade = NULL;

}

CFCLiveDoCmd::~CFCLiveDoCmd(void)
{
	//fini();

	return;
}



BOOL CFCLiveDoCmd::init(ACE_Log_File_Msg* pLogInstance,
						CFCSockManager* pFCSockManager,
						CFCServerFacade *pFCServerFacade)
{
	m_pLogInstance = pLogInstance;
	m_pFCSockManager = pFCSockManager;

	m_pFCServerFacade = pFCServerFacade;

	return true;
}


void CFCLiveDoCmd::fini()
{
	return;
}

int CFCLiveDoCmd::OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	int result = 0;
			
	switch(pMsgBlock->pPduHdr->nOperation)
	{	
		case OP_ID_GET_LAN_STATE:
			result = f_OnCmd_GetLanStatus(pRemoteAddr, pMsgBlock, pRetMsgBlock);
			break;

		case OP_ID_GET_E1_STATE:
			result = f_OnCmd_GetE1Status(pRemoteAddr, pMsgBlock, pRetMsgBlock);
			break;

		case OP_ID_GET_TUNNEL_STATE:
			result = f_OnCmd_GetTunnelStatus(pRemoteAddr, pMsgBlock, pRetMsgBlock);
			break;

		case OP_ID_GET_SERVICE_STATISTIC:
			result = f_OnCmd_GetFlowStatistic(pRemoteAddr, pMsgBlock, pRetMsgBlock);
			break;

		case OP_ID_GET_INFO:
			result = f_OnCmd_GetSysInfo(pRemoteAddr, pMsgBlock, pRetMsgBlock);
			break;

		case OP_ID_GET_VERSION:
			result = f_OnCmd_GetSoftVersion(pRemoteAddr, pMsgBlock, pRetMsgBlock);
			break;

		default:
			{
				char buf[64] = {0};
				pRemoteAddr->addr_to_string(buf,sizeof(buf));
				MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Err][%D]recv %u cmd from '%s' is not defined!!!\n"),pMsgBlock->pPduHdr->nOperation,buf));
				return -1;
			}
			break;
	}
	
	if (0 != result)
	{
		MY_ACE_DEBUG(m_pLogInstance, (LM_ERROR, ACE_TEXT("[Err][%D]fcserver operation %d failed, return %d.\r\n"), pMsgBlock->pPduHdr->nOperation, result));
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


/*BOOL CFCLiveDoCmd::OnCmd_TransToFCProbe_Live(MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	enum{SEND_TIMEOUT = 3};

	pRetMsgBlock = NULL;

	int nerror = m_pFCSockManager->TransRequestToFCProbe_Live(pMsgBlock,pRetMsgBlock,SEND_TIMEOUT);

	if( nerror != 0 )
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,nerror,RESPONSE_CODE_OTHER);
	}

	return true;
}*/

void CFCLiveDoCmd::f_get_LAN_card_statuse(UINT32 lan_count, UINT8 *lan_statuse)
{
	// 查询信息
	char info_query[128];
	//  返回信息
	char info_text[256];
	int ii=0;
	FILE *fp_query = NULL;

	for(ii=0; ii<lan_count; ii++)
	{
		lan_statuse[ii] = 0; // 默认值

		// 查询命令
		sprintf(info_query, "ethtool eth%d | grep Speed: ", ii);

		// 在管道中启动命令
		fp_query = popen(info_query, "r");
		if(fp_query == NULL){ continue; }

		// 分析返回信息
		memset(info_text, 0,  sizeof(info_text));
		fgets(info_text, sizeof(info_text)-1, fp_query);
		pclose(fp_query);  // 立即关闭句柄

		char *pt = strstr(info_text, "Speed: ");
		if(pt == NULL){ continue; } // 返回结果格式不对
		pt += 7; // strlen("Speed: ")
		int cur_speed = atoi(pt);

		switch(cur_speed)
		{
			case 0: lan_statuse[ii] = 0; break;
			case 10:  lan_statuse[ii] = 1; break;
			case 100:  lan_statuse[ii] = 2; break;
			case 1000:  lan_statuse[ii] = 3; break;
			default: lan_statuse[ii] = 3; break;
		}
	} // for(ii)
}
int CFCLiveDoCmd::f_OnCmd_GetLanStatus(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	// LAN口个数(4字节), eth0 ~ eth3
	UINT32 LAN_count = 4;
	// LAN口速率(单位M)
	UINT8 LAN_statue[4]={0}; // 0:断开,1:10M, 2:100M, 3:1000m

	// 获得4个网口的状态
	f_get_LAN_card_statuse(LAN_count, LAN_statue);

	// 返回信息
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, 128); // 开辟空间
	if (pRetMsgBlock)
	{
		char* pUsrDataHead = CMsgBlockManager::WritePtr(*pRetMsgBlock); // 用户数据区首地址
		char* pCurWrite = pUsrDataHead;
		CData_Stream_LE::PutUint32(pCurWrite, LAN_count); // 返回LAN口个数
		for(int ii=0; ii<LAN_count; ii++){ CData_Stream_LE::PutUint8(pCurWrite, LAN_statue[ii]); } // 各个网口状态
		pRetMsgBlock->nDataLen = (int)(pCurWrite - pUsrDataHead);

		return 0;
	}
	else
	{
		return -1;
	}
}

int CFCLiveDoCmd::f_OnCmd_GetE1Status(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
#if 0
	// 把请求直接传递给flux_control模块(即test_control 进程)
	return m_pFCSockManager->TransRequestToFCProbe_Live(pMsgBlock, pRetMsgBlock, 1);
#else
	// 返回信息
	UINT32 nE1Num = m_pFCServerFacade->m_pUnsStatShm->nE1Num;
	
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, sizeof(UINT32)+nE1Num*sizeof(UNS_E1_STAT_S)+16);
	if (pRetMsgBlock)
	{
		char* pUsrDataHead = CMsgBlockManager::WritePtr(*pRetMsgBlock); // 用户数据区首地址
		char* pCurWrite = pUsrDataHead;
		
		CData_Stream_LE::PutUint32(pCurWrite, nE1Num);
		for (int i=0; i<nE1Num; i++)
		{
			CData_Stream_LE::PutUint8(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagE1Stat[i].nE1Stat); 
		}
		pRetMsgBlock->nDataLen = (int)(pCurWrite - pUsrDataHead);

		return 0;
	}
	else
	{
		return -1;
	}

#endif
}

BOOL CFCLiveDoCmd::f_OnCmd_GetTunnelStatus(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
#if 0
	// 把请求直接传递给flux_control模块(即test_control 进程)
	return m_pFCSockManager->TransRequestToFCProbe_Live(pMsgBlock, pRetMsgBlock, 1);
#else
	// 返回信息
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, sizeof(UINT32)+5*sizeof(unsigned char)+16);
	if (pRetMsgBlock)
	{
		char* pUsrDataHead = CMsgBlockManager::WritePtr(*pRetMsgBlock); // 用户数据区首地址
		char* pCurWrite = pUsrDataHead;
		
		CData_Stream_LE::PutUint32(pCurWrite, 5);
		CData_Stream_LE::PutUint8(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagPeerStat.nMainStat); 
		CData_Stream_LE::PutUint8(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagPeerStat.nB1Stat); 
		CData_Stream_LE::PutUint8(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagPeerStat.nB2Stat); 
		CData_Stream_LE::PutUint8(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagPeerStat.nB3Stat); 
		CData_Stream_LE::PutUint8(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagPeerStat.nB4Stat); 
		
		pRetMsgBlock->nDataLen = (int)(pCurWrite - pUsrDataHead);

		return 0;
	}
	else
	{
		return -1;
	}
#endif
	return(TRUE);
}

int CFCLiveDoCmd::f_OnCmd_GetFlowStatistic(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
#if 0
	// 把请求直接传递给flux_control模块(即test_control 进程)
	return m_pFCSockManager->TransRequestToFCProbe_Live(pMsgBlock, pRetMsgBlock, 1);
#else
	ACE_UINT32 nFlowNum = m_pFCServerFacade->m_pUnsStatShm->nFlowNum;
	
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, (sizeof(UINT32)+nFlowNum*sizeof(UNS_FLOW_STAT_S)+16));
	if (pRetMsgBlock)
	{
		char* pUsrDataHead = CMsgBlockManager::WritePtr(*pRetMsgBlock); // 用户数据区首地址
		char* pCurWrite = pUsrDataHead;
		
		CData_Stream_LE::PutUint32(pCurWrite, nFlowNum);
		for(int i=0; i<nFlowNum; i++)
		{
			// 业务ID
			CData_Stream_LE::PutUint32(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagFlowStat[i].nFlowId);
			// 上行流量
			CData_Stream_LE::PutUint64(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagFlowStat[i].nUpBytes);
			// 上行以太网帧个数
			CData_Stream_LE::PutUint64(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagFlowStat[i].nUpPackets);
		} 
		pRetMsgBlock->nDataLen = (int)(pCurWrite - pUsrDataHead);

		return 0;
	}
	else
	{
		return -1;
	}

#endif
}

int CFCLiveDoCmd::f_OnCmd_GetSysInfo(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	// 返回信息
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, 128); // 开辟空间
	if (pRetMsgBlock)
	{
		char* pUsrDataHead = CMsgBlockManager::WritePtr(*pRetMsgBlock); // 用户数据区首地址
		char* pCurWrite = pUsrDataHead;

		// CUP使用率(百分比整数)
		CData_Stream_LE::PutUint8(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagSysInfo.nCpuRate);
		
		// 已使用内存(单位M)
		CData_Stream_LE::PutUint32(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagSysInfo.nMemUsed);
		
		// 总内存(单位M)
		CData_Stream_LE::PutUint32(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagSysInfo.nMemTotal);
		
		// 已使用硬盘(单位G)
		CData_Stream_LE::PutUint32(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagSysInfo.nHddUsed);

		// 总硬盘(单位G)
		CData_Stream_LE::PutUint32(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagSysInfo.nHddTotal);
		
		// 系统启动时间
		CData_Stream_LE::PutUint32(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagSysInfo.nSysStartTime);
		
		//运行时间
		CData_Stream_LE::PutUint32(pCurWrite, m_pFCServerFacade->m_pUnsStatShm->tagSysInfo.nSysRunTime);
		pRetMsgBlock->nDataLen = (int)(pCurWrite - pUsrDataHead);
		
		return 0;
	}
	else
	{
		return -1;
	}
}

int CFCLiveDoCmd::f_OnCmd_GetSoftVersion(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	// 返回信息
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, 64); // 开辟空间
	if (pRetMsgBlock)
	{
		char* pUsrDataHead = CMsgBlockManager::WritePtr(*pRetMsgBlock); // 用户数据区首地址
		char* pCurWrite = pUsrDataHead;

		CData_Stream_LE::PutUint8(pCurWrite, 0x01);//产品系列
		CData_Stream_LE::PutUint8(pCurWrite, 0x01);//产品型号
		
		CData_Stream_LE::PutString(pCurWrite, m_pFCServerFacade->m_strSoftVersion); // 字串
		pRetMsgBlock->nDataLen = (int)(pCurWrite - pUsrDataHead);
		
		return 0;
	}
	else
	{
		return -1;
	}
}

