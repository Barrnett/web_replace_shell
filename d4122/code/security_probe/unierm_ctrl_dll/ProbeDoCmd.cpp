//////////////////////////////////////////////////////////////////////////
//ProbeDoCmd.cpp
#include "stdafx.h"

#include "ProbeDoCmd.h"
#include "MonitorTask.h"
#include "cpf/path_tools.h"
#include "cpf/IniFile.h"
#include "cpf/Data_Stream.h"
#include "MonitorManager.h"
#include "ProbeServant.h"
//#include "ProbeSysInfo.h"
#include "RestartTask.h"
#include "SetSysTime_And_RestartTask.h"
#include "MonitorSvc.h"
#include "ace/Date_Time.h"

CProbeDoCmd::CProbeDoCmd(void)
{
	m_pMonitorMgr = NULL;
	//m_pProbeSysInfo = NULL;
	m_pMonitorSvc = NULL;

	m_probe_server_port = 0;
	m_probe_servant_id = 0;

}

CProbeDoCmd::~CProbeDoCmd(void)
{
}

void CProbeDoCmd::init_ex(ACE_Log_Msg* pLogInstance,
						  CMonitorManager* pMonitorMgr,
						  MonitorSvc * pMonitorSvc,
						  PROBESERVICE_CONFIG_PARAM * pConfigInfo,
						  CProbeSysInfo *	pProbeSysInfo)
{
	CBaseDoCmd::init(pLogInstance,pConfigInfo);

	m_pMonitorMgr = pMonitorMgr;
	m_pMonitorSvc = pMonitorSvc;
	//m_pProbeSysInfo = pProbeSysInfo;

	{
		CIniFile inifile;

		const char * filename = CPF::GetModuleOtherFileName("ini","tcpip_mgr.ini");

		if( inifile.Open(filename) )
		{
			inifile.GetValue("net_com","listen_port",m_probe_server_port);
			inifile.GetValue("net_com","servant_id",m_probe_servant_id);
		}
	}

	return;
}

void CProbeDoCmd::OnCmd_StartProgram(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	CBaseDoCmdTool::log_request(m_pLogInstance,pRemoteAddr,"StartProgram");

	const char * pdata = CMsgBlockManager::ReadPtr(*pMsgBlock,0,4);

	if( !pdata )
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,(short)-100,RESPONSE_CODE_SUCCESS);

		return;
	}

	const int temp_type = (int)CData_Stream_LE::GetUint32(pdata);

	std::string prg_name = CData_Stream_LE::GetString(pdata);
	
	ACE_UINT32 start_time = GetTickCount();

	int nrtn = start_probe(prg_name.c_str(),temp_type);

	pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,(short)nrtn,RESPONSE_CODE_SUCCESS);

	//花费的时间
	int sec_spend = (int)(GetTickCount() - start_time)/1000;

	MY_ACE_DEBUG(m_pLogInstance,
		(LM_INFO,ACE_TEXT("[Info][%D]send rsp for StartProgram %s .time spend=%d(s)\n")
		,prg_name.c_str(),sec_spend));


	return;
}

void CProbeDoCmd::OnCmd_StopProgram(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	CBaseDoCmdTool::log_request(m_pLogInstance,pRemoteAddr,"OnCmd_StopProgram");

	const char * pdata = CMsgBlockManager::ReadPtr(*pMsgBlock,0,4);

	if( !pdata )
	{
		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,(short)-100,RESPONSE_CODE_SUCCESS);

		return;
	}

	const int temp_type = (int)CData_Stream_LE::GetUint32(pdata);

	std::string prg_name = CData_Stream_LE::GetString(pdata);

	ACE_UINT32 start_time = GetTickCount();

	int nrtn = stop_probe(prg_name.c_str(),temp_type);

	pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,(short)nrtn,RESPONSE_CODE_SUCCESS);

	//花费的时间
	int sec_spend = (int)(GetTickCount() - start_time)/1000;

	MY_ACE_DEBUG(m_pLogInstance,
		(LM_INFO,ACE_TEXT("[Info][%D]send rsp for StopProgram %s.time spend=%d(s)\n"),
		prg_name.c_str(),sec_spend));

	return;
}

void CProbeDoCmd::OnCmd_RestartProgram(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock)
{
	CBaseDoCmdTool::log_request(m_pLogInstance,pRemoteAddr,"RestartProgram");

	const char * pdata = CMsgBlockManager::ReadPtr(*pMsgBlock,0,4);

	if( !pdata )
	{
		return;
	}

	const int temp_type = (int)CData_Stream_LE::GetUint32(pdata);

	std::string prg_name = CData_Stream_LE::GetString(pdata);

	ACE_UINT32 start_time = GetTickCount();

	int nrtn = stop_probe(prg_name.c_str(),temp_type);

	if( nrtn == 0 )
	{
		nrtn = start_probe(prg_name.c_str(),temp_type);
	}

	//花费的时间
	int sec_spend = (int)(GetTickCount() - start_time)/1000;

	MY_ACE_DEBUG(m_pLogInstance,
		(LM_INFO,ACE_TEXT("[Info][%D]send rsp for RestartProgram %s.time spend=%d(s)\n"),
		prg_name.c_str(),sec_spend));

	return;
}



void CProbeDoCmd::OnCmd_GetProgramRunState(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	//probe的运行状态
	int nstate = -1;

	const char * pdata = CMsgBlockHelper_Stream::ReadPtr(*pMsgBlock,0,8);

	if( !pdata )
	{
		ACE_ASSERT(FALSE);

		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,(short)nstate,RESPONSE_CODE_SUCCESS);

		return;
	}

	int temp_type = (int)CData_Stream_LE::GetUint32(pdata);
	const char * prg_name = CData_Stream_LE::GetString(pdata);

	if( m_pMonitorMgr )
	{
		if( m_pMonitorMgr->GetConfigState(prg_name,nstate,temp_type) != 0 )
		{
			nstate = -1;
		}
	}

	pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,(short)nstate,RESPONSE_CODE_SUCCESS);

	return;

}

void CProbeDoCmd::OnCmd_GetSysState(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	if( m_pConfigInfo->bLogState )
	{
		CBaseDoCmdTool::log_request(m_pLogInstance,pRemoteAddr,"GetProbeSysState");
	}

	const size_t buf_alloc = 4096;

	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock,buf_alloc,RESPONSE_CODE_SUCCESS);

	if( pRetMsgBlock )
	{
		size_t buf_used = GetProbeSysInfo(pRetMsgBlock);

		pRetMsgBlock->nDataLen=(int)buf_used;
	}

	return;

}

void CProbeDoCmd::OnCmd_SetSysTime(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	CBaseDoCmdTool::log_request(m_pLogInstance,pRemoteAddr,"SetSystemTime");

	SYSTEMTIME sys_time;

	int nrtn = ExtractSystemTime(pMsgBlock,sys_time);

	pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,(short)nrtn,RESPONSE_CODE_SUCCESS);

	CSetSysTime_And_RestartTask * restart_task = new CSetSysTime_And_RestartTask;

	restart_task->Init(m_pMonitorMgr,sys_time);

	restart_task->sleep_and_do(20*1000,1);

	return;
}

void CProbeDoCmd::OnCmd_GetSysTime(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	if( m_pConfigInfo->bLogState )
	{
		CBaseDoCmdTool::log_request(m_pLogInstance,pRemoteAddr,"GetSystemTime");
	}

	const size_t buf_alloc = 4096;

	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock,buf_alloc,RESPONSE_CODE_SUCCESS);

	if( pRetMsgBlock )
	{
		size_t buf_used = GetProbeSysTime(pRetMsgBlock);

		pRetMsgBlock->nDataLen=(int)buf_used;
	}

	return;

}

size_t CProbeDoCmd::GetProbeSysInfo(MSG_BLOCK * pResponMsgBlock)
{
#if 0
	char * pdata = CMsgBlockHelper_Stream::WritePtr(*pResponMsgBlock,0,pResponMsgBlock->nBufferSize);

	char * pbegin = pdata;

	//当前系统cpu利用率
	CData_Stream_LE::PutUint8(pdata,(char)m_pProbeSysInfo->m_cpuInfo.m_uCpuCount+1);

	for(int i = 0; i < m_pProbeSysInfo->m_cpuInfo.m_uCpuCount+1; ++i)
	{
		CData_Stream_LE::PutUint8(pdata,m_pProbeSysInfo->m_cpuInfo.m_uCurrentUsage[i]);
	}

	#ifdef OS_WINDOWS
	//系统总mem
	CData_Stream_LE::PutUint32(pdata,m_pProbeSysInfo->m_memInfo.m_uPageFile/1000);	//1024);
	//系统mem使用情况
	unsigned int mem_used = (m_pProbeSysInfo->m_memInfo.m_uPageFile - m_pProbeSysInfo->m_memInfo.m_uFreePageFile)/1000;	//1024;
	CData_Stream_LE::PutUint32(pdata,mem_used);
	#else
	CData_Stream_LE::PutUint32(pdata,m_pProbeSysInfo->m_memInfo.m_uPhysicalMem/1000);
	unsigned int mem_used = (m_pProbeSysInfo->m_memInfo.m_uPhysicalMem - m_pProbeSysInfo->m_memInfo.m_uFreePhysicalMem)/1000;
	CData_Stream_LE::PutUint32(pdata,mem_used);
	#endif	// OS_WINDOWS


	return pdata - pbegin;
#else
	return(0);
#endif
}

size_t CProbeDoCmd::GetProbeSysTime(MSG_BLOCK * pResponMsgBlock)
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

	return pdata - pbegin;
}

int CProbeDoCmd::ExtractSystemTime(MSG_BLOCK * pReqMsgBlock,SYSTEMTIME& sys_time)
{
	memset(&sys_time,0x00,sizeof(sys_time));

	const char * curtime = CMsgBlockManager::ReadPtr(*pReqMsgBlock,0,sizeof(SYSTEMTIME));

	if( !curtime )
		return -1;

	memcpy(&sys_time,curtime,sizeof(sys_time));

	return 0;
}

/*
int CProbeDoCmd::SetProbeSysTime(MSG_BLOCK * pReqMsgBlock)
{
	SYSTEMTIME systime;

	ExtractSystemTime(pReqMsgBlock,systime);

#ifdef OS_WINDOWS

	BOOL bOK = SetLocalTime(&systime);

	if( bOK )
		return 0;
	else
		return -1;
#else
	struct tm locddtime;
	locddtime.tm_year	= systime.wYear - 1900;
	locddtime.tm_mon	= systime.wMonth -1;
	locddtime.tm_wday	= systime.wDayOfWeek;
	locddtime.tm_mday	= systime.wDay;
	locddtime.tm_hour	= systime.wHour;
	locddtime.tm_min	= systime.wMinute;
	locddtime.tm_sec	= systime.wSecond;

	timeval newTimeval = {0, 0};
	newTimeval.tv_sec = mktime(&locddtime);
	return settimeofday(&newTimeval, NULL);
#endif

}


*/

int CProbeDoCmd::start_probe(const char * prg_name,BOOL bTempState)
{
	int nrtn = 0;

	if( m_pMonitorMgr )
	{
		nrtn = ChangeConfigState(prg_name,1,bTempState);
	}

	return nrtn;
}

int CProbeDoCmd::stop_probe(const char * prg_name,BOOL bTempState)
{
	int nrtn = 0;
	if( m_pMonitorMgr )
	{
		nrtn = ChangeConfigState(prg_name,0,bTempState);
	}

	return nrtn;
}

int CProbeDoCmd::ChangeConfigState(const char * prg_name,int state,BOOL bTempState)
{
	int nrtn = 0;

	if( m_pMonitorMgr )
	{	
		nrtn = m_pMonitorMgr->ChangeConfigState(prg_name,state,bTempState);

		if( nrtn == 0 && !bTempState )
		{
			ACE_TCHAR conf_filename[MAX_PATH];
			CPF::GetModuleOtherFileName(conf_filename, MAX_PATH,
				"ini","ProbeService.xml");//hard coded

			CMonitorManagerConfig config;
			if( config.open(conf_filename) == 0 )
			{
				config.SetMonitorTaskState(prg_name,state);
			}
		}
		
	}

	return nrtn;
}

int CProbeDoCmd::test_request(CProbeServant * pProbeServant)
{
#ifdef _DEBUG

	//test_reqeust_SetFuncState(pProbeServant);

	//test_simple_request(8,pProbeServant);//GetFuncState();

	//test_simple_request(3,pProbeServant);//GetProbeSysState();

	//test_simple_request(10,pProbeServant);//GetConfigVersion();

	//test_simple_request(11,pProbeServant);//RestoreOrgConfig

	//test_request_config_renew(pProbeServant);

	//test_simple_request(12,pProbeServant);//GetProbeVersion();

	//test_simple_request(17,pProbeServant);//check_probe_license();

	//test_simple_request(1000,pProbeServant);//测试一个不存在的命令

	//test_request_remove_device(pProbeServant);
	//test_request_add_device(pProbeServant);

#endif

	return 0;
}

int CProbeDoCmd::test_request_config_renew(CProbeServant * pProbeServant)
{
	if( !pProbeServant )
		return -1;

	MSG_BLOCK * pRequstMsgBlock = CMsgBlockManager::Malloc(10240);

	pRequstMsgBlock->pPduHdr->nOperation = 5;

	char * pdata = CMsgBlockHelper_Stream::WritePtr(*pRequstMsgBlock,0,10240);

	CData_Stream_LE::PutUint16(pdata,3);

	const char cfg_file_type[] = "cfg_file_type.xml";
	const char cfg_organization[] = "cfg_organization.xml";
	const char cfg_policy[] = "cfg_policy.xml";

	CData_Stream_LE::PutUint16(pdata,sizeof(cfg_file_type));
	CData_Stream_LE::PutFixString(pdata,sizeof(cfg_file_type),cfg_file_type);

	CData_Stream_LE::PutUint16(pdata,sizeof(cfg_organization));
	CData_Stream_LE::PutFixString(pdata,sizeof(cfg_organization),cfg_organization);

	CData_Stream_LE::PutUint16(pdata,sizeof(cfg_policy));
	CData_Stream_LE::PutFixString(pdata,sizeof(cfg_policy),cfg_policy);

	MSG_BLOCK * pResponseMsgBlock = NULL;

	ACE_INET_Addr addr(8001,"127.0.0.1");

	pProbeServant->OnRequest(&addr,pRequstMsgBlock,pResponseMsgBlock);

	if( pResponseMsgBlock )
	{
		/*const char * pdata_rsp = */CMsgBlockHelper_Stream::ReadPtr(*pResponseMsgBlock,0,pResponseMsgBlock->nDataLen);

	}

	CMsgBlockManager::Free(pRequstMsgBlock);
	CMsgBlockManager::Free(pResponseMsgBlock);

	return 0;
}

void CProbeDoCmd::OnCmd_SetRebootTimeInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	const char * pdata = CMsgBlockHelper_Stream::ReadPtr(*pMsgBlock,0,4+3*4+4);

	if( !pdata )
	{
		CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,-1,RESPONSE_CODE_SUCCESS);
		return;
	}

	{
		m_pMonitorSvc->m_enable_reboot = (int)CData_Stream_LE::GetUint32(pdata);

		m_pMonitorSvc->m_nRebootPeriodDay= (int)CData_Stream_LE::GetUint32(pdata);

		m_pMonitorSvc->m_nRebootHour = (int)CData_Stream_LE::GetUint32(pdata);
		m_pMonitorSvc->m_nRebootMin = (int)CData_Stream_LE::GetUint32(pdata);
		m_pMonitorSvc->m_nRebootSec = (int)CData_Stream_LE::GetUint32(pdata);

		m_pMonitorSvc->m_nRebootTimeout_Param = (int)CData_Stream_LE::GetUint32(pdata);
	}

	{
		m_pMonitorSvc->m_enable_poweroff = (int)CData_Stream_LE::GetUint32(pdata);

		m_pMonitorSvc->m_nPoweroffHour = (int)CData_Stream_LE::GetUint32(pdata);
		m_pMonitorSvc->m_nPoweroffMin = (int)CData_Stream_LE::GetUint32(pdata);
		m_pMonitorSvc->m_nPoweroffSec = (int)CData_Stream_LE::GetUint32(pdata);
	}

	m_pMonitorSvc->WriteRebootInfo();

	pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,0,RESPONSE_CODE_SUCCESS);

	return;

}

void CProbeDoCmd::OnCmd_GetRebootTimeInfo(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock,512);

	char * pdata = CMsgBlockManager::WritePtr(*pRetMsgBlock);
	char * begin = pdata;

	CData_Stream_LE::PutUint16(pdata,0);

	{
		CData_Stream_LE::PutUint32(pdata,m_pMonitorSvc->m_enable_reboot);
		CData_Stream_LE::PutUint32(pdata,m_pMonitorSvc->m_nRebootPeriodDay);

		CData_Stream_LE::PutUint32(pdata,m_pMonitorSvc->m_nRebootHour);
		CData_Stream_LE::PutUint32(pdata,m_pMonitorSvc->m_nRebootMin);
		CData_Stream_LE::PutUint32(pdata,m_pMonitorSvc->m_nRebootSec);

		CData_Stream_LE::PutUint32(pdata,m_pMonitorSvc->m_nRebootTimeout_Param);
	}

	{
		CData_Stream_LE::PutUint32(pdata,m_pMonitorSvc->m_enable_poweroff);

		CData_Stream_LE::PutUint32(pdata,m_pMonitorSvc->m_nPoweroffHour);
		CData_Stream_LE::PutUint32(pdata,m_pMonitorSvc->m_nPoweroffMin);
		CData_Stream_LE::PutUint32(pdata,m_pMonitorSvc->m_nPoweroffSec);
	}

	pRetMsgBlock->nDataLen = (int)(pdata-begin);

	return;

}
