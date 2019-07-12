//////////////////////////////////////////////////////////////////////////
//ProbeServant.cpp
#include "stdafx.h"

#include "ProbeServant.h"
#include "MonitorManager.h"
#include "ProbeDoCmd.h"

#include "cpf/Data_Stream.h"

CProbeServant::CProbeServant()
{
	m_pLogInstance = NULL;

	m_pConfigInfo = NULL;

	m_probe_cmd = NULL;

	m_connect_nums = 0;
	m_disconnect_nums = 0;
}

CProbeServant::~CProbeServant(void)
{

	return;
}

void CProbeServant::init(ACE_Log_Msg* pLogInstance,
						 CProbeDoCmd * probe_cmd,
						 PROBESERVICE_CONFIG_PARAM * pConfigInfo)
{
	m_pLogInstance = pLogInstance;

	m_probe_cmd = probe_cmd;

	m_pConfigInfo = pConfigInfo;

	return;
}


enum{
	OP_ID_START_PROGRAM = 60001,
	OP_ID_STOP_PROGRAM = 60002,
	OP_ID_GET_PROGRAM_STATUS = 60003,
	OP_ID_RESTART_PROGRAM = 60004,

	OP_ID_GET_SERVER_SYS_INFO = 61001,//获取服务器的系统资源信息，cpu，内存和磁盘
	OP_ID_GET_PROBE_SYS_INFO = 61002,//获取probe的系统资源信息，cpu，内存和磁盘

	OP_ID_GET_SERVER_SYS_TIME = 61003,//获取服务器的系统时间
	OP_ID_GET_PROBE_SYS_TIME = 61004,//获取probe的系统时间

	OP_ID_SET_SERVER_SYS_TIME = 61005,//设置服务器的系统时间
	OP_ID_SET_PROBE_SYS_TIME = 61006,//设置probe的系统时间

	OP_ID_SET_REBOOT_TIME_INFO = 61007,//设置系统定期重新启动时间
	OP_ID_GET_REBOOT_TIME_INFO = 61008//获取系统定期重新启动时间

};

void CProbeServant::OnNotify(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock)
{
	switch((ACE_UINT16)pMsgBlock->pPduHdr->nOperation)
	{
	case OP_ID_RESTART_PROGRAM://先关闭后再启动某个程序
		{
			m_probe_cmd->OnCmd_RestartProgram(pRemoteAddr,pMsgBlock);				
		}
		break;

	default:
		ACE_ASSERT(FALSE);
		break;
	}
}

int CProbeServant::OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	switch((ACE_UINT16)pMsgBlock->pPduHdr->nOperation)
	{
	case OP_ID_START_PROGRAM:
		{
			m_probe_cmd->OnCmd_StartProgram(pRemoteAddr,pMsgBlock,pRetMsgBlock);
		}
		break;

	case OP_ID_STOP_PROGRAM:
		{
			m_probe_cmd->OnCmd_StopProgram(pRemoteAddr,pMsgBlock,pRetMsgBlock);	
		}
		break;

	case OP_ID_GET_PROGRAM_STATUS:
		{
			m_probe_cmd->OnCmd_GetProgramRunState(pRemoteAddr,pMsgBlock,pRetMsgBlock);				
		}
		break;


	case OP_ID_GET_SERVER_SYS_INFO:
	case OP_ID_GET_PROBE_SYS_INFO:
		{
			m_probe_cmd->OnCmd_GetSysState(pRemoteAddr,pMsgBlock,pRetMsgBlock);	
		}
		break;

	case OP_ID_GET_SERVER_SYS_TIME:
	case OP_ID_GET_PROBE_SYS_TIME:
		{
			m_probe_cmd->OnCmd_GetSysTime(pRemoteAddr,pMsgBlock,pRetMsgBlock);	
		}
		break;

	case OP_ID_SET_SERVER_SYS_TIME:
	case OP_ID_SET_PROBE_SYS_TIME:
		{
			m_probe_cmd->OnCmd_SetSysTime(pRemoteAddr,pMsgBlock,pRetMsgBlock);	
		}
		break;

	case OP_ID_SET_REBOOT_TIME_INFO:
		{
			m_probe_cmd->OnCmd_SetRebootTimeInfo(pRemoteAddr,pMsgBlock,pRetMsgBlock);	
		}
		break;

	case OP_ID_GET_REBOOT_TIME_INFO:
		{
			m_probe_cmd->OnCmd_GetRebootTimeInfo(pRemoteAddr,pMsgBlock,pRetMsgBlock);	
		}
		break;

	default:
		{
			char buf[64] = {0};

			pRemoteAddr->addr_to_string(buf,sizeof(buf));

			pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,-1,RESPONSE_CODE_NO_OPERATION);

			MY_ACE_DEBUG(m_pLogInstance,(LM_INFO,ACE_TEXT("[Info][%D]recv %u cmd from '%s' is not defined!!!\r\n"),pMsgBlock->pPduHdr->nOperation,buf));
		}
		break;
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


int CProbeServant::test_request()
{
#ifdef _DEBUG
	//if( m_pConfigInfo->nSupportRestoreModule & PROBESERVICE_CONFIG_PARAM::RESTORE_PROBE )
	//{
	//	m_probe_cmd->test_request(this);
	//}

	//if( m_pConfigInfo->nSupportRestoreModule & PROBESERVICE_CONFIG_PARAM::RESTORE_DB )
	//{
	//	m_db_cmd->test_request(this);
	//}
#endif//_DEBUG

	return 0;
}

