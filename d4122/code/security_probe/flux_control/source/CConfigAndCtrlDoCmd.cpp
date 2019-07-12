//////////////////////////////////////////////////////////////////////////
//CConfigAndCtrlDoCmd.cpp

#include "stdafx.h"
#include "CConfigAndCtrlDoCmd.h"
#include "FluxControlObserver.h"
#include "cpf/path_tools.h"
#include "cpf/Data_Stream.h"
#include "SockOperationId.h"
#include "cpf/XMLFileSock.h"
#include "FluxControlFrameObserver.h"
#include "config_center.h"
#include "FCSleepAndDoThread.h"

CCConfigAndCtrlDoCmd::CCConfigAndCtrlDoCmd(void)
{
	m_pLogInstance = NULL;
}

CCConfigAndCtrlDoCmd::~CCConfigAndCtrlDoCmd(void)
{
}


void CCConfigAndCtrlDoCmd::init(ACE_Log_File_Msg * pLogInstance,
								CFluxControlObserver *	pFluxControlObserver)
{
	m_pLogInstance = pLogInstance;

	return;
}

void CCConfigAndCtrlDoCmd::close()
{
	return;
}

int CCConfigAndCtrlDoCmd::OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	switch(pMsgBlock->pPduHdr->nOperation)
	{
	case OP_ID_SET_CONFIG_FILE:
		OnCmd_SetCfgFile(pRemoteAddr, pMsgBlock, pRetMsgBlock);
		break;

	case OP_ID_GET_CONFIG_FILE:
		OnCmd_GetCfgFile(pRemoteAddr, pMsgBlock, pRetMsgBlock);
		break;

	case OP_ID_CLEAR_SERVICE_STATISTIC:
		OnCmd_clear_service_statistic(pRemoteAddr, pMsgBlock, pRetMsgBlock);
		break;

	case OP_ID_ENABLE_MGMT_TUNNEL:
		OnCmd_EnableMgmtTunnel(pRemoteAddr, pMsgBlock, pRetMsgBlock);
		break;

	case OP_ID_DISABLE_MGMT_TUNNEL:
		OnCmd_DisableMgmtTunnel(pRemoteAddr, pMsgBlock, pRetMsgBlock);
		break;

	case OP_ID_APPLY_UPDATE_CONFIG:
		OnCmd_ApplyUpdateCfg(pRemoteAddr, pMsgBlock, pRetMsgBlock);
		break;

	case OP_ID_CHECK_UPDATE_CONFIG:
		OnCmd_CheckUpdateCfg(pRemoteAddr, pMsgBlock, pRetMsgBlock);
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

	return 0;

}


BOOL CCConfigAndCtrlDoCmd::OnCmd_SetCfgFile(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	const char * read_data = CMsgBlockManager::ReadPtr(*pMsgBlock, 0, 5+5+4);

	if (!read_data)
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, -1);
		return true;
	}

	const char * pfile_name = CData_Stream_LE::GetSafeString(read_data);

	if (!pfile_name)
	{
		//ACE_ASSERT(FALSE);
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, -2);
		return true;
	}

	const char * pfull_name = CData_Stream_LE::GetSafeString(read_data);
	
	if (!pfull_name)
	{
		//ACE_ASSERT(FALSE);
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, -2);
		return true;
	}

	int in_config_param = CData_Stream_LE::GetUint32(read_data);

	int out_config_param = 0;

	if (stricmp(pfile_name, "TS_config.xml") == 0
		|| stricmp(pfile_name, "local_config.xml") == 0
		|| stricmp(pfile_name, "filter_config.xml") == 0)
	{
		CFCSleepAndDoThread * reboot_task = new CFCSleepAndDoThread(CRebootTask::EXIT_PORCESS, 0);

		reboot_task->sleep_and_do(5 * 1000);
	}
	else
	{
		//ACE_ASSERT(FALSE);
		return true;
	}

	char buf_info[1024] = {0};

	sprintf(buf_info,"TestNextVersionCfg cfg_file_name=%s",pfile_name);

	CBaseDoCmdTool::log_request(m_pLogInstance, pRemoteAddr, buf_info);

	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, 6);

	char * write_data = CMsgBlockManager::WritePtr(*pRetMsgBlock);

	CData_Stream_LE::PutUint16(write_data, 0);
	CData_Stream_LE::PutUint32(write_data, out_config_param);

	return true;
}

BOOL CCConfigAndCtrlDoCmd::OnCmd_GetCfgFile(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	const char * read_data = CMsgBlockManager::ReadPtr(*pMsgBlock, 0, 5+4);

	if (!read_data)
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, -1);
		return true;
	}

	const char * pfile_name = CData_Stream_LE::GetSafeString(read_data);

	if (!pfile_name)
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, -2);
		return true;
	}

	int in_config_param = CData_Stream_LE::GetUint32(read_data);

	int out_config_param = 0;

	std::string out_file_name;

	if (stricmp(pfile_name, "tb_org_and_employee.xml") == 0)
	{
		//OnCmd_GetOrgAndEmployeeConfig(pRemoteAddr, pMsgBlock, pRetMsgBlock, 0, &out_file_name);
	}
	else
	{
		//ACE_ASSERT(FALSE);
	}

	char buf_info[2048] = { 0 };

	sprintf(buf_info, "want to get cfg_file_name=%s, out file name=%s", pfile_name, out_file_name.c_str());

	CBaseDoCmdTool::log_request(m_pLogInstance, pRemoteAddr, buf_info);

	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, 2048);

	char * write_data = CMsgBlockManager::WritePtr(*pRetMsgBlock);
	char * begin_pos = write_data;

	CData_Stream_LE::PutUint16(write_data, 0);
	CData_Stream_LE::PutString(write_data, out_file_name.c_str());
	CData_Stream_LE::PutUint32(write_data, out_config_param);

	pRetMsgBlock->nDataLen = (int)(write_data - begin_pos);

	return true;
}

BOOL CCConfigAndCtrlDoCmd::OnCmd_clear_service_statistic(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	// 清零
	ACE_UINT32 business_count = g_pFluxControlObserver->m_piStatistic->m_vtFlowStatistic.size();
	for(int ii=0; ii<business_count; ii++)
	{
		// 上行流量
		g_pFluxControlObserver->m_piStatistic->m_vtFlowStatistic[ii].n64UpBytes = 0;
		// 上行以太网帧个数
		g_pFluxControlObserver->m_piStatistic->m_vtFlowStatistic[ii].n64UpPackets = 0;
	}

	// 返回状态
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, 32);
	char * write_data = CMsgBlockManager::WritePtr(*pRetMsgBlock);
	char * begin_pos = write_data;
	CData_Stream_LE::PutUint16(write_data, 0);
	pRetMsgBlock->nDataLen = (int)(write_data - begin_pos);

	return(TRUE);
}

BOOL CCConfigAndCtrlDoCmd::OnCmd_EnableMgmtTunnel(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	g_pFluxControlObserver->m_piAccessMgmt->m_nPeerMgmt = TRUE;

	// 返回状态
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, 32);
	if (pRetMsgBlock)
	{
		char * pUsrDataHead = CMsgBlockManager::WritePtr(*pRetMsgBlock);
		char * pCurWrite = pUsrDataHead;
		
		CData_Stream_LE::PutUint16(pCurWrite, 0);
		pRetMsgBlock->nDataLen = (int)(pCurWrite - pUsrDataHead);
	}

	return(TRUE);
}

BOOL CCConfigAndCtrlDoCmd::OnCmd_DisableMgmtTunnel(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	g_pFluxControlObserver->m_piAccessMgmt->m_nPeerMgmt = FALSE;

	// 返回状态
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, 32);
	if (pRetMsgBlock)
	{
		char * pUsrDataHead = CMsgBlockManager::WritePtr(*pRetMsgBlock);
		char * pCurWrite = pUsrDataHead;
		
		CData_Stream_LE::PutUint16(pCurWrite, 0);
		pRetMsgBlock->nDataLen = (int)(pCurWrite - pUsrDataHead);
	}

	return(TRUE);
}

BOOL CCConfigAndCtrlDoCmd::OnCmd_ApplyUpdateCfg(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	if (100 <= g_pFluxControlObserver->m_nUpdateCfg)
	{
		g_pFluxControlObserver->m_nUpdateCfg = 0;
	}

	// 返回状态
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, 32);
	if (pRetMsgBlock)
	{
		char * pUsrDataHead = CMsgBlockManager::WritePtr(*pRetMsgBlock);
		char * pCurWrite = pUsrDataHead;
		
		CData_Stream_LE::PutUint16(pCurWrite, 0);
		pRetMsgBlock->nDataLen = (int)(pCurWrite - pUsrDataHead);
	}
	
	return TRUE;
}

BOOL CCConfigAndCtrlDoCmd::OnCmd_CheckUpdateCfg(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	// 返回状态
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, 32);
	if (pRetMsgBlock)
	{
		char * pUsrDataHead = CMsgBlockManager::WritePtr(*pRetMsgBlock);
		char * pCurWrite = pUsrDataHead;
		
		CData_Stream_LE::PutUint16(pCurWrite, 0);
		CData_Stream_LE::PutUint32(pCurWrite, g_pFluxControlObserver->m_nUpdateCfg);
		pRetMsgBlock->nDataLen = (int)(pCurWrite - pUsrDataHead);
	}
	
	return TRUE;
}

