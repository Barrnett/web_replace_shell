//////////////////////////////////////////////////////////////////////////
//CConfigAndCtrlDoCmd.h


#include "StdAfx.h"
#include "CConfigAndCtrlDoCmd.h"
#include <sstream>
#include "cpf/Data_Stream.h"
#include "cpf/path_tools.h"
#include "SockManager.h"
#include "SockOperationId.h"
#include <string>
#include "FCServerFacade.h"
#include "cpf/XMLFileSock.h"
#include "cpf/os_syscall.h"
#include "ace/Date_Time.h"
#include "SysMaintenanceDoCmd.h"
#include "config_fc_server_xml.h"
#include "EZInclude.h" // add by xumx 2016-10-27

enum{SEND_TIMEOUT = 8};
enum{TRANS_TO_FC=0X01};

CCConfigAndCtrlDoCmd::CCConfigAndCtrlDoCmd(void)
{
	m_pLogInstance = NULL;

	m_pFCSockManager = NULL;

	m_pFCServerFacade = NULL;

}

CCConfigAndCtrlDoCmd::~CCConfigAndCtrlDoCmd(void)
{
}

void CCConfigAndCtrlDoCmd::init(ACE_Log_File_Msg* pLogInstance,
							CFCSockManager* pFCSockManager,
							CFCServerFacade * pFCServerFacade)
{
	m_pLogInstance = pLogInstance;

	m_pFCSockManager = pFCSockManager;

	m_pFCServerFacade = pFCServerFacade;

	return;
}


void CCConfigAndCtrlDoCmd::fini()
{
	return;
}


int CCConfigAndCtrlDoCmd::OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	switch(pMsgBlock->pPduHdr->nOperation)
	{
		case OP_ID_SET_CONFIG_FILE:
			OnCmd_SetConfigFile(pRemoteAddr, pMsgBlock, pRetMsgBlock);
			break;

		case OP_ID_GET_CONFIG_FILE:
			OnCmd_GetConfigFile(pRemoteAddr, pMsgBlock, pRetMsgBlock);
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
			char buf[64] = { 0 };
			pRemoteAddr->addr_to_string(buf, sizeof(buf));
			MY_ACE_DEBUG(m_pLogInstance, (LM_INFO, ACE_TEXT("%D:recv %u cmd from '%s' is not defined!!!\r\n"), pMsgBlock->pPduHdr->nOperation, buf));
			return -1;
		}
		break;
	}//end switch

	if( pRetMsgBlock )
	{
		return pRetMsgBlock->pPduHdr->nResponseCode;
	}

	return -1;
}

/*
template<typename CONFIG_TYPE>
BOOL OnCmd_Send_Config_To_Probe(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock,
	const char * log_string, const char * xml_file_name, const char * db_log_string)
{
	//确保文件正确的写下去
	std::string temp_fcserver_config_file_name = ConfigCenter_Obj()->GetProbeFullConfigName(xml_file_name);

	BOOL brtn = CPF::OnCmd_Safe_SetOneXmlFileConfig(
		temp_fcserver_config_file_name.c_str(), pRemoteAddr, pMsgBlock, pRetMsgBlock);

	if (!brtn)
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, -1, RESPONSE_CODE_SUCCESS);
		return false;
	}

	CONFIG_TYPE temp_policy;

	if (!temp_policy.LoadFromFile(temp_fcserver_config_file_name.c_str()))
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, -2, RESPONSE_CODE_SUCCESS);
		return false;
	}

	{
		std::string probe_config_file_name = ConfigCenter_Obj()->GetProbeFullConfigName(file_name);

		CopyFile(temp_fcserver_config_file_name.c_str(), probe_config_file_name.c_str(), FALSE);
	}

	m_pFCSockManager->SendNewVersionCfg_ToFCProbe(file_name);

	pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, 0, RESPONSE_CODE_SUCCESS);

	return true;
}
*/

BOOL CCConfigAndCtrlDoCmd::MyTransRequestToFCProbe(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	pRetMsgBlock = NULL;

	int nerror = m_pFCSockManager->TransRequestToFCProbe_Cmd(pMsgBlock,pRetMsgBlock,SEND_TIMEOUT);
	if( nerror != 0 )
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,nerror,RESPONSE_CODE_OTHER);
	}

	return (nerror==0);
}

size_t CCConfigAndCtrlDoCmd::GetProbeSysTime(MSG_BLOCK * pResponMsgBlock)
{
	SYSTEMTIME the_time;

#ifdef OS_WINDOWS
	GetLocalTime(&the_time);
#else

/*
	time_t curtime; 
	struct tm *locddtime = NULL;

	curtime = time(NULL);
	locddtime = localtime (&curtime);

	if (NULL != locddtime)
	{
		the_time.wYear		= locddtime->tm_year+1900;
		the_time.wMonth		= locddtime->tm_mon+1;
		the_time.wDayOfWeek = locddtime->tm_wday;
		the_time.wDay		= locddtime->tm_mday;
		the_time.wHour		= locddtime->tm_hour;
		the_time.wMinute	= locddtime->tm_min;
		the_time.wSecond	= locddtime->tm_sec;
		the_time.wMilliseconds = 0;
	}
*/
	ACE_Date_Time tvTime;	//(ACE_OS::gettimeofday());   
	the_time.wYear		= tvTime.year();
	the_time.wMonth		= tvTime.month();
	the_time.wDayOfWeek = tvTime.weekday();
	the_time.wDay		= tvTime.day();
	the_time.wHour		= tvTime.hour();
	the_time.wMinute	= tvTime.minute();
	the_time.wSecond	= tvTime.second();
	the_time.wMilliseconds = tvTime.microsec();  
#endif

	char * pdata = CMsgBlockHelper_Stream::WritePtr(*pResponMsgBlock,0,pResponMsgBlock->nBufferSize);

	char * pbegin = pdata;

	CData_Stream_LE::PutFixString(pdata,sizeof(the_time),(char *)&the_time);

	return (int)(pdata - pbegin);
}

BOOL CCConfigAndCtrlDoCmd::SendOneXMLFileToFCProbe(const char * filename,int operation)
{
	char * buf = NULL;

	int length = CTinyXmlEx::ReadOneXmlFileForSend(filename,buf);

	MSG_BLOCK * pResponseMsgBlock = NULL;

	int nrtn = -1;

	if( length == -1 )
	{
		char empty_data[8] = {0};

		nrtn = m_pFCSockManager->m_fc_probe_sock.SendSyncRequest(
			m_pFCSockManager->m_fc_probe_cmd_servantid,
			operation,empty_data,sizeof(empty_data),
			pResponseMsgBlock,SEND_TIMEOUT);

	}
	else
	{
		nrtn = m_pFCSockManager->m_fc_probe_sock.SendSyncRequest(
			m_pFCSockManager->m_fc_probe_cmd_servantid,
			operation,buf,length,pResponseMsgBlock,SEND_TIMEOUT);

		delete [] buf;
	}

	if( pResponseMsgBlock )
	{
		CMsgBlockHelper_Stream::Free(pResponseMsgBlock);
	}

	return 0==nrtn;
}

BOOL CCConfigAndCtrlDoCmd::OnCmd_SetOneXMLFileConfig_ErrorByProbe(int trans_to_probe,ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock,
							   const char * log_string,const char * xml_file_name,const char * db_log_string)
{
	CBaseDoCmdTool::log_request(m_pLogInstance,pRemoteAddr,log_string);

	BOOL bLogOK = true;

	//先转发给probe
	pRetMsgBlock = NULL;

	int nerror = 0;

	if( trans_to_probe & 0x01 )
	{
		m_pFCSockManager->TransRequestToFCProbe_Cmd(pMsgBlock,pRetMsgBlock,SEND_TIMEOUT);
	}

	if( nerror != 0 )
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,nerror,RESPONSE_CODE_OTHER);

		bLogOK = false;
	}
	else
	{
		if( trans_to_probe == 0 )
		{
			pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,0,RESPONSE_CODE_SUCCESS);
		}	

		bLogOK = true;
	}

	return true;
}

BOOL CCConfigAndCtrlDoCmd::OnCmd_SetOneXMLFileConfig_ErrorByFCServer(int trans_to_probe,ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock,
											   const char * log_string,const char * xml_file_name,const char * db_log_string)
{
	CBaseDoCmdTool::log_request(m_pLogInstance,pRemoteAddr,log_string);

	BOOL bLogOK = true;

	//先转发给probe
	if( trans_to_probe & 0x01 )
	{
		MSG_BLOCK *  pRetMsgBlock1 = NULL;
		int nerror1 = m_pFCSockManager->TransRequestToFCProbe_Cmd(pMsgBlock,pRetMsgBlock1,SEND_TIMEOUT);
		if(pRetMsgBlock1)
		{
			CMsgBlockManager::Free(pRetMsgBlock1);
		}
	}

	pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,0,RESPONSE_CODE_SUCCESS);

	return true;
}

BOOL CCConfigAndCtrlDoCmd::OnCmd_SetConfigFile(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	const char * read_data = CMsgBlockManager::ReadPtr(*pMsgBlock, 0, 5+5+4);

	if (read_data == NULL)
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, -1);
		return true;
	}

	const char * pfile_name = CData_Stream_LE::GetSafeString(read_data);

	if (pfile_name == NULL)
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, -2);
		return true;
	}

	const char * full_file_name = CData_Stream_LE::GetSafeString(read_data);

	if (full_file_name == NULL)
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, -3);
		return true;
	}

	int in_config_param = CData_Stream_LE::GetUint32(read_data);
	int out_config_param = 0;

	int rtn = 0;

	//if (stricmp(pfile_name, "uns_sys_config.uct") == 0)
	if (ACE_OS::strstr(pfile_name, ".uct"))//add by zhongjh
	{
		rtn = CSysMaintenanceDoCmd::LoadSysCfgFile(full_file_name, in_config_param);
	}
	else if (stricmp(pfile_name, "TS_config.xml") == 0
		|| stricmp(pfile_name, "local_config.xml") == 0
		|| stricmp(pfile_name, "filter_config.xml") == 0)
	{
		MyTransRequestToFCProbe(pRemoteAddr, pMsgBlock, pRetMsgBlock);
		return true;
	}	
	else
	{
		rtn = 100;
	}

	//LOG
	char buf_info[1024] = {0};
	sprintf(buf_info,"SetConfigFile=%s(return:%d)", pfile_name, rtn);
	CBaseDoCmdTool::log_request(m_pLogInstance, pRemoteAddr, buf_info);

	//
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, 6);
	if (pRetMsgBlock)
	{
		char * write_data = CMsgBlockManager::WritePtr(*pRetMsgBlock);

		CData_Stream_LE::PutUint16(write_data, (ACE_UINT16)rtn);//Code
		CData_Stream_LE::PutUint32(write_data, out_config_param);//output_config_param
	}

	return true;
}



BOOL CCConfigAndCtrlDoCmd::OnCmd_GetConfigFile(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
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

	int nrtn = 0;
	std::string out_file_name;

	if (stricmp(pfile_name, "uns_sys_config.uct") == 0)
	{
		out_file_name = ConfigCenter_Obj()->GetProbeFullConfigName("uns_sys_config.uct");

		nrtn = CSysMaintenanceDoCmd::BackupSysCfgFile(out_file_name.c_str());
	}
	else
	{
		nrtn = 100;
	}

	//LOG
	char buf_info[1024] = { 0 };
	sprintf(buf_info, "GetConfigFile=%s(return:%d)", pfile_name, nrtn);
	CBaseDoCmdTool::log_request(m_pLogInstance, pRemoteAddr, buf_info);

	//
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock, 2048);
	if (pRetMsgBlock)
	{
		char * write_data = CMsgBlockManager::WritePtr(*pRetMsgBlock);
		char * begin_pos = write_data;

		CData_Stream_LE::PutUint16(write_data, nrtn);//Code
		CData_Stream_LE::PutString(write_data, out_file_name.c_str());//
		CData_Stream_LE::PutUint32(write_data, out_config_param);//output_config_param
		pRetMsgBlock->nDataLen = (int)(write_data - begin_pos);
	}

	return true;
}
///////////////////////////////////

BOOL CCConfigAndCtrlDoCmd::OnCmd_clear_service_statistic(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	// 把请求直接传递给flux_control模块(即test_control 进程)
	int nerror = m_pFCSockManager->TransRequestToFCProbe_Cmd(pMsgBlock, pRetMsgBlock, 1);

	if( nerror != 0 )
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, nerror, RESPONSE_CODE_OTHER);
	}

	return(TRUE);
}

BOOL CCConfigAndCtrlDoCmd::OnCmd_EnableMgmtTunnel(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	// 把请求直接传递给flux_control模块(即test_control 进程)
	int nerror = m_pFCSockManager->TransRequestToFCProbe_Cmd(pMsgBlock, pRetMsgBlock, 1);
	if( nerror != 0 )
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, nerror, RESPONSE_CODE_OTHER);
	}

	return TRUE;
}

BOOL CCConfigAndCtrlDoCmd::OnCmd_DisableMgmtTunnel(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	// 把请求直接传递给flux_control模块(即test_control 进程)
	int nerror = m_pFCSockManager->TransRequestToFCProbe_Cmd(pMsgBlock, pRetMsgBlock, 1);
	if( nerror != 0 )
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, nerror, RESPONSE_CODE_OTHER);
	}

	return TRUE;
}

BOOL CCConfigAndCtrlDoCmd::OnCmd_ApplyUpdateCfg(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	// 把请求直接传递给flux_control模块(即test_control 进程)
	int nerror = m_pFCSockManager->TransRequestToFCProbe_Cmd(pMsgBlock, pRetMsgBlock, 1);
	if( nerror != 0 )
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, nerror, RESPONSE_CODE_OTHER);
	}

	return TRUE;
}

BOOL CCConfigAndCtrlDoCmd::OnCmd_CheckUpdateCfg(ACE_INET_Addr * pRemoteAddr, MSG_BLOCK * pMsgBlock, MSG_BLOCK * & pRetMsgBlock)
{
	// 把请求直接传递给flux_control模块(即test_control 进程)
	int nerror = m_pFCSockManager->TransRequestToFCProbe_Cmd(pMsgBlock, pRetMsgBlock, 1);
	if( nerror != 0 )
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock, nerror, RESPONSE_CODE_OTHER);
	}

	return TRUE;
}

