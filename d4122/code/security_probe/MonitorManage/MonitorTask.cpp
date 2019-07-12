//////////////////////////////////////////////////////////////////////////
//MonitorTask.cpp

#include "MonitorTask.h"
#include "ace/Process.h"
#include "cpf/path_tools.h"
#include "ace/Process_Mutex.h"
#include "cpf/my_event.h"
#include "cpf/other_tools.h"

#ifndef ERROR_ALREADY_EXISTS
#	define ERROR_ALREADY_EXISTS        183  /* see also 80 ? */
#endif
#ifndef WAIT_OBJECT_0
#	define WAIT_OBJECT_0	0
#endif

#ifdef OS_WINDOWS	
#include <windows.h>
#include <tlhelp32.h>
//#include "WinSDK_Psapi.h"

// 进程数据结构---------------------------------------
typedef struct _THREAD_INFO
{
	LARGE_INTEGER CreateTime;
	DWORD dwUnknown1;
	DWORD dwStartAddress;
	DWORD StartEIP;
	DWORD dwOwnerPID;
	DWORD dwThreadId;
	DWORD dwCurrentPriority;
	DWORD dwBasePriority;
	DWORD dwContextSwitches;
	DWORD Unknown;
	DWORD WaitReason;

}THREADINFO, *PTHREADINFO;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaxLength;
	PWSTR Buffer;
} UNICODE_STRING;

typedef struct _PROCESS_INFO
{
	DWORD dwOffset;
	DWORD dwThreadsCount;
	DWORD dwUnused1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;

	DWORD dwBasePriority;
	DWORD dwProcessID;
	DWORD dwParentProcessId;
	DWORD dwHandleCount;
	DWORD dwUnused3[2];

	DWORD dwVirtualBytesPeak;
	DWORD dwVirtualBytes;
	ULONG dwPageFaults;
	DWORD dwWorkingSetPeak;
	DWORD dwWorkingSet;
	DWORD dwQuotaPeakPagedPoolUsage;
	DWORD dwQuotaPagedPoolUsage;
	DWORD dwQuotaPeakNonPagedPoolUsage;
	DWORD dwQuotaNonPagedPoolUsage;
	DWORD dwPageFileUsage;
	DWORD dwPageFileUsagePeak;

	DWORD dCommitCharge;
	THREADINFO ThreadSysInfo[1];

} PROCESSINFO, *PPROCESSINFO;

#endif

CMonitorTask::CMonitorTask()
{
	m_pLogInstance = NULL;

	m_nRunningState=RUNING_STAT_STOP;

	m_Pid = ACE_INVALID_PID;

	m_bDebug = false;
}

CMonitorTask::~CMonitorTask()
{

}

void CMonitorTask::init(BOOL bDebug,const MONITOR_TASK& MonitorTask, ACE_Log_Msg* pLogFile)
{
	m_bDebug = bDebug;

	memcpy(&m_taskInfo,&MonitorTask,sizeof(m_taskInfo));

	//文件名中是否带有路径
	bool bpath = (ACE_OS::strchr(m_taskInfo.szFileName,FILENAME_SEPERATOR) != NULL);

#ifdef OS_WINDOWS
	if( !bpath )
	{
		bpath = (ACE_OS::strchr(m_taskInfo.szFileName,LINUX_FILENAME_SEPERATOR) != NULL);
	}
#endif

	if( !bpath )
	{
		ACE_OS::strcpy(m_taskInfo.szFileName,CPF::GetModuleFullFileName(m_taskInfo.szFileName));
	}

	const char * szProcessName = CPF::GetFileNameFromFullName(m_taskInfo.szFileName);

	if( szProcessName )
	{
		strcpy(m_taskInfo.szProcessName,szProcessName);
	}

	m_pLogInstance = pLogFile;

	if( m_pLogInstance == NULL )
	{
		m_pLogInstance = ACE_Log_Msg::instance();
	}

	m_nRunningState=RUNING_STAT_STOP;

	m_Pid = ACE_INVALID_PID;

	if( IsTaskActive() & IsTaskExist())
	{
		m_nRunningState = RUNING_STAT_RUNNING;
	}

	m_lastExistTime = m_lastActiveTime = ACE_OS::gettimeofday();
}



/******************************************************************************
* FUNCTION    : CProcInfo::CreateProcS(IN const char * szAppName,IN const char *szCmd, IN const char *szWorkingDirectory)
*       
* DESCRIPTION : 
*       -- 创建指定进程
*       
* PARAMETER   : 
*       1: strCmd -- IN, 指定进程可执行文件全路径名称
*       
* RETURN     : 
*       int -- 成功则返回进程ID，否则返回-1
*       
* SPECIAL    : 
*       
*****************************************************************************/
int CMonitorTask::CreateProcS( BOOL bDebug,const char * szAppName, const char *szCmd, const char *szWorkingDirectory,int priority)
{
	return RunProcess(szAppName,szCmd,szWorkingDirectory,priority);

	/*
	if( bDebug )
	{
		return RunProcess(szAppName,szCmd,szWorkingDirectory);
	}
	else
	{
#ifdef OS_WINDOWS
		return RunProcessAsUser(szAppName,szCmd,szWorkingDirectory);

#else
		return RunProcess(szAppName,szCmd,szWorkingDirectory);
#endif
	}
	*/
}



int CMonitorTask::RunProcess(const char * szAppName, const char *szCmd, const char *szWorkingDirectory,int priority)
{
#ifdef OS_LINUX
//	printf("\nCMonitorTask::RunProcess = %s %s\n", szAppName, szCmd);
	char strPath[512];
	char strFile[512];
	char strFind[512];
	if (NULL==szAppName && NULL==szCmd)
		return -1;

	if (NULL != szAppName)
	{
		strcpy(strPath, szAppName);

		// 先进入目录，在启动程序
		char* pPos = strrchr(strPath, '/');
		if (NULL != pPos)
		{
			*pPos = '\0';
			int iRtn = chdir(strPath);
	//		printf("chdir = %s, Rtn = %d\n", strPath, iRtn);

			// 将名字copy过去
			pPos ++;
			sprintf(strFile, "%s/%s", strPath, pPos);
			strcpy(strFind, pPos);
		}
		else
		{
			sprintf(strFile, "./%s", strPath);
			strcpy(strFind, strPath);
		}


		if (NULL != szCmd)
		{
			strcat(strFile, " ");
			strcat(strFile, szCmd);
		}
	}
	else
	{
		strcpy(strFile, szCmd);
	}

//	printf("CMonitorTask::RunProcess = %s\n", strFile);

	/*return*/ system(strFile); // 这个返回值并不是pid
	ACE_OS::sleep(2);
	return FindProcessByName(strFind);
#endif



	if( !szAppName && !szCmd )
			return -1;

	ACE_Process rProcess;
	ACE_Process_Options rOpt;

	ULONG priority_class = 0;

#ifdef OS_WINDOWS

	if( priority == 1 )
		priority_class = IDLE_PRIORITY_CLASS;
	else if( priority == 2 )
		priority_class = NORMAL_PRIORITY_CLASS;
	else if( priority == 3 )
		priority_class = HIGH_PRIORITY_CLASS;
	else if( priority == 4 )
		priority_class = REALTIME_PRIORITY_CLASS;

#endif

	rOpt.handle_inheritance(false);

	if( szWorkingDirectory )
	{
		rOpt.working_directory(szWorkingDirectory);
	}

#ifdef OS_WINDOWS
	rOpt.creation_flags( priority_class|CREATE_NEW_CONSOLE );
#endif

	if( szAppName && szCmd )
		rOpt.command_line("%s %s",szAppName,szCmd);
	else if( szAppName )
		rOpt.command_line("%s",szAppName);
	else if( szCmd )
		rOpt.command_line("%s",szCmd);
	return rProcess.spawn( rOpt );
}


/*
BOOL CMonitorTask::GetTokenByName(HANDLE &hToken,const char * lpName)
{
	if(!lpName)
	{
		return FALSE;
	}
	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet      = FALSE; 
	PROCESSENTRY32 pe32      = {0}; 

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return (FALSE); 

	pe32.dwSize = sizeof(PROCESSENTRY32); 

	if (Process32First(hProcessSnap, &pe32)) 
	{
		do 
		{
			if ( 0 == ACE_OS::strcasecmp(lpName,pe32.szExeFile) )
			{
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
					FALSE,pe32.th32ProcessID);
				bRet = OpenProcessToken(hProcess,TOKEN_ALL_ACCESS,&hToken);
				CloseHandle (hProcessSnap); 
				return (bRet);
			}
		} 
		while (Process32Next(hProcessSnap, &pe32)); 
		bRet = TRUE; 
	} 
	else 
		bRet = FALSE;

	CloseHandle (hProcessSnap); 
	return (bRet);
}



BOOL CMonitorTask::RunProcessAsUser(const char * szAppName, const char *szCmd, const char *szWorkingDirectory,int priority)   
{   
	HANDLE hToken;   

	char explorer_prg[] = "explorer.exe";

	if(!GetTokenByName(hToken,explorer_prg))   
	{   
		return -1;   
	}   

	STARTUPINFO si;   

	PROCESS_INFORMATION pi;   

	ZeroMemory(&si, sizeof(STARTUPINFO));   

	si.cb= sizeof(STARTUPINFO);   

	si.lpDesktop ="winsta0\\default";

	BOOL bResult = CreateProcessAsUser(
		hToken,szAppName,(LPSTR)szCmd,
		NULL,NULL,FALSE,
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, 
		NULL,
		szWorkingDirectory,&si,&pi);   

	CloseHandle(hToken); 

	if( bResult )
		return 0;

	DWORD dwError = GetLastError();

	return -1;

}  

*/
/******************************************************************************
* FUNCTION    : CProcInfo::TerminateProcS(IN unsigned int uPID)
*       
* DESCRIPTION : 
*       -- 终止指定进程
*       
* PARAMETER   : 
*       1: uPID -- IN, 指定进程标识
*       
* RETURN     : 
*       bool -- true if success, false else.
*       
* SPECIAL    : 
*       
*****************************************************************************/
bool CMonitorTask::TerminateProcS( IN unsigned int uPID )
{
	int nRet = 0;

#ifdef OS_WINDOWS	
	nRet = ACE::terminate_process( uPID );
#else
	char cmd_buf[512]={0};
	sprintf(cmd_buf,"kill -9 %u",uPID);
	system(cmd_buf);
#endif

	if ( -1 == nRet )
	{
		return false; 
	}

	return true;
}

// 终止指定进程
bool CMonitorTask::TerminateProcS( IN const char * szProcessname )
{
	unsigned int uPID = FindProcessByName(szProcessname);

	if( uPID == -1 )
		return false;

	return TerminateProcS(uPID);
}

int CMonitorTask::TerminateProcesLoop(const char * szproc_name)
{
	int count = 0;

	m_Pid = ACE_INVALID_PID;

	while( IsTaskExist() )
	{
		ACE_OS::sleep(1);

		TerminateProcS(m_Pid);

		MY_ACE_DEBUG(m_pLogInstance,
			(LM_DEBUG, ACE_TEXT("[Info][%D]TerminateProcesLoop '%s',count = %d,pid=%u\n"), szproc_name,count,m_Pid));

		m_Pid = ACE_INVALID_PID;

		++count;


	}

	return count;
}

int CMonitorTask::ChangeConfigState(int nState,BOOL bTempState)
{
	if( CONFIG_STAT_START == nState 
		&& bTempState 
		&& CONFIG_STAT_STOP == m_taskInfo.nPermanentConfigState)
	{
		return 0;
	}

	if( CONFIG_STAT_START == nState )
	{
		ForceStartTask();
	}
	else if( CONFIG_STAT_STOP == nState )
	{
		ForceStopTask();
	}
	else
	{
		ACE_ASSERT(FALSE);
		return -1;
	}

	m_taskInfo.nRunConfigState=nState;

	if( !bTempState )
	{
		m_taskInfo.nPermanentConfigState = nState;
	}

	return 0;
}

int CMonitorTask::GetConfigState(BOOL bTempStat)
{
	if( bTempStat )
		return m_taskInfo.nRunConfigState;
	else
		return m_taskInfo.nPermanentConfigState;
}

int CMonitorTask::OnTimer(const ACE_Time_Value &in_current_time)
{
	ACE_Time_Value current_time = ACE_OS::gettimeofday();

	if(m_taskInfo.nRunConfigState==CONFIG_STAT_STOP)
	{
		ForceStopTask();
		return 0;
	}
	else
	{
		switch(m_nRunningState) 
		{
		case RUNING_STAT_STOP:
			ForceStartTask();
			break;

		case RUNING_STAT_RUNNING:
			if( !CheckToExist(current_time) )
			{
				if( stricmp(m_taskInfo.sztask_type,"event") == 0 )
				{
					if( CheckToActive(current_time) )
					{
						
					}
				}
			}			
			break;

		case RUNING_STAT_STARTING:
			ForceStartTask();
			break;

		case RUNING_STAT_STOPPING:
			ACE_ASSERT(0);
			break;
		default:
			ACE_ASSERT(0);
			break;
		}
	}

	return 0;
}

//检查进程是否活动
BOOL CMonitorTask::CheckToActive(const ACE_Time_Value &current_time)
{
	if( (current_time-m_lastActiveTime).sec() < m_taskInfo.nMonitorActiveInterval )
	{
		return false;
	}
	
	if( IsTaskActive() )
	{
		m_lastExistTime = m_lastActiveTime = current_time;		

		return false;
	}
	
	if (ACE_INVALID_PID != m_Pid )
	{
		TerminateProcS(m_Pid);

		if( IsTaskExist() )
		{
			MY_ACE_DEBUG(m_pLogInstance,
				(LM_DEBUG, ACE_TEXT("[Info][%D]program has blocked: %s,we Terminate it!\n"), m_taskInfo.szFileName));
		}
		else
		{
			MY_ACE_DEBUG(m_pLogInstance,
				(LM_DEBUG, ACE_TEXT("[Info][%D]program had exist:%s,it maybe clashd or closed by user!\n"), m_taskInfo.szFileName));
		}
	}

	m_Pid = ACE_INVALID_PID;
	m_nRunningState=RUNING_STAT_STOP;

	return true;
		
}



//检查进程是否存在
BOOL CMonitorTask::CheckToExist(const ACE_Time_Value &current_time)
{
	if( (current_time-m_lastExistTime).sec() < m_taskInfo.nMonitorExistInterval )
	{
		return false;
	}

	if( IsTaskExist() )
	{
		m_lastExistTime = current_time;

		return false;
	}

	if (ACE_INVALID_PID != m_Pid )
	{
		TerminateProcS(m_Pid);
	}

	m_Pid = ACE_INVALID_PID;
	m_nRunningState=RUNING_STAT_STOP;

	return true;
}

unsigned int CMonitorTask::FindProcessByName(const char * name)
{
#ifdef OS_WINDOWS

	// 获取系统中所有进程的快照 
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if ( INVALID_HANDLE_VALUE == hProcessSnap ) 
	{
		ACE_ASSERT( 0 );
		//MY_ACE_DEBUG( m_pLogInstance,( LM_ERROR, ACE_TEXT( "%I获取进程列表失败！！！\n" ) ) );
		return false; 
	}

	PROCESSENTRY32 pe32 = {0};
	// 在使用PROCESSENTRY32对象之前必须初始化dwSize成员
	pe32.dwSize = sizeof( PROCESSENTRY32 ); 

	// 遍历所有进程，获取本进程状态
	if ( !Process32First( hProcessSnap, &pe32 ) ) 
	{
		CloseHandle(hProcessSnap);
		ACE_ASSERT( 0 );
		//MY_ACE_DEBUG(m_pLogInstance, ( LM_ERROR, ACE_TEXT( "%I遍历进程列表失败！！！\n" ) ) );
		return false; 
	}

	bool bFound = false;
	do 
	{
		if ( 0 == ACE_OS::strcasecmp(name,pe32.szExeFile) )
		{
			bFound = true;
			break;
		}
	} while ( Process32Next( hProcessSnap, &pe32 ) ); 

	CloseHandle(hProcessSnap);

	if ( bFound )
	{
		return (unsigned int)pe32.th32ProcessID;
	}

#else

	char szTmp[128];
	sprintf(szTmp, "ps -ef|grep %s |grep -v grep", name);
	FILE* fp = popen(szTmp, "r");
	if (fp == NULL)
	{
		// 找不到，换种方式在找
		sprintf(szTmp, "ps aux |grep %s |grep -v grep", name);
		fp = popen(szTmp, "r");
		if (fp == NULL)
		{
			//printf("CMonitorTask::FindProcessByName, error, cmd=%s\n", szTmp);
			return -1;
		}
	}
	int iProcID = -1;
	fscanf(fp, "%*s %d", &iProcID);
	pclose(fp);

	return iProcID;

#endif

	return -1;
}


//////////////////////////////////////////////////////////////////////////
//CMonitorTask_Event
//////////////////////////////////////////////////////////////////////////


CMonitorTask_Event::CMonitorTask_Event()
{
}

CMonitorTask_Event::~CMonitorTask_Event()
{
}

int CMonitorTask_Event::ForceStopTask()
{
	if( !IsTaskExist() )//进程根本没有启动
	{
		//MY_ACE_DEBUG(m_pLogInstance,
		//	(LM_INFO,ACE_TEXT("%D:the program named '%s' is not running!\n"),m_taskInfo.szProcessName));

		return 0;
	}

	my_event_t hStopEvent;

	ACE_OS::last_error(0);
	MY_EVENT::event_init_for_server (&hStopEvent, 
		TRUE, 
		TRUE, 
		0, 
		m_taskInfo.szStopEventName);

	MY_EVENT::event_signal(&hStopEvent);

#ifdef OS_WINDOWS
	if (ACE_OS::last_error() == ERROR_ALREADY_EXISTS)
#endif
	{
		int nRet = -1;

		for( int i = 0; i < m_taskInfo.nStopInterval/2; ++i)
		{
			ACE_OS::sleep(2);
			
			nRet=ACE::process_active(m_Pid);

			if( nRet == 0 )
			{
				break;
			}
		}
	
		if (nRet == 0) 
		{
			MY_ACE_DEBUG(m_pLogInstance,
				(LM_INFO,ACE_TEXT("[Info][%D]the program named '%s' had stoped!\n"),m_taskInfo.szProcessName));
		}
		else
		{			
			TerminateProcS(m_Pid);

			MY_ACE_DEBUG(m_pLogInstance,
				(LM_INFO,ACE_TEXT("[Info][%D]the program named '%s' force to Terminate,pid=%u!\n"),m_taskInfo.szProcessName,m_Pid));
		}

		TerminateProcesLoop(m_taskInfo.szProcessName);
	}

#ifdef OS_WINDOWS
	else
	{
		TerminateProcS(m_Pid);

		MY_ACE_DEBUG(m_pLogInstance,
			(LM_INFO,ACE_TEXT("[Info][%D]the program named '%s' force to Terminate,pid=%u!\n"),m_taskInfo.szProcessName,m_Pid));

		TerminateProcesLoop(m_taskInfo.szProcessName);
	}
#endif

	MY_EVENT::event_destroy(&hStopEvent);

	m_nRunningState = RUNING_STAT_STOP;

	return 0;
}

int CMonitorTask_Event::ForceStartTask()
{
	switch(m_nRunningState)
	{
	case RUNING_STAT_STOP:
	case RUNING_STAT_RUNNING:
	if( IsTaskExist() && IsTaskActive() )
		{
			m_nRunningState=RUNING_STAT_RUNNING;
			m_lastExistTime = m_lastActiveTime = ACE_OS::gettimeofday();

			MY_ACE_DEBUG(m_pLogInstance,
				(LM_DEBUG, ACE_TEXT("[Info][%D]Start Program: '%s' already running!\n"), m_taskInfo.szFileName));

			return 0;
		}
		else
		{
			if( m_taskInfo.szFullCmdLine[0] )
				m_Pid = CreateProcS(m_bDebug,NULL,m_taskInfo.szFullCmdLine,m_taskInfo.szWorkingDirectory,m_taskInfo.priority);
			else

				m_Pid = CreateProcS(m_bDebug,m_taskInfo.szFileName,m_taskInfo.szCmdLine,m_taskInfo.szWorkingDirectory,m_taskInfo.priority);

			if(m_Pid != ACE_INVALID_PID)
			{
				for(int iLoop=0;iLoop<STARTING_TIME_OUT;iLoop++)
				{
					if( IsTaskActive() )
					{
						m_nRunningState=RUNING_STAT_RUNNING;
						m_lastExistTime = m_lastActiveTime = ACE_OS::gettimeofday();

						MY_ACE_DEBUG(m_pLogInstance,
							(LM_DEBUG, ACE_TEXT("[Info][%D]Start Program: '%s' Success!\n"), m_taskInfo.szFileName));

						return 0;
					}

					ACE_OS::sleep(1);
				}
				MY_ACE_DEBUG(m_pLogInstance,
					(LM_DEBUG, ACE_TEXT("[Err][%D]Start Program: '%s' failed!\n"), m_taskInfo.szFileName));

				TerminateProcS(m_Pid);
				m_Pid = ACE_INVALID_PID;
				return -1;
			}
			else
			{
				MY_ACE_DEBUG(m_pLogInstance,
					(LM_DEBUG, ACE_TEXT("[Err][%D]Start Program: '%s' failed!\n"), m_taskInfo.szFileName));
			}
		}
		return -1;
		break;


	case RUNING_STAT_STARTING:
		for(int iLoop=0;iLoop<STARTING_TIME_OUT;iLoop++)
		{
			if( IsTaskActive() )
			{
				m_nRunningState=RUNING_STAT_RUNNING;

				m_lastExistTime = m_lastActiveTime = ACE_OS::gettimeofday();
				
				MY_ACE_DEBUG(m_pLogInstance,
					(LM_DEBUG, ACE_TEXT("[Info][%D]Start Program: '%s' Success!\n"), m_taskInfo.szFileName));

				return 0;
			}

			ACE_OS::sleep(1);
		}

		MY_ACE_DEBUG(m_pLogInstance,
			(LM_DEBUG, ACE_TEXT("[Err][%D]Start Program: '%s' failed!\n"), m_taskInfo.szFileName));

		if(m_Pid != ACE_INVALID_PID)
		{
			TerminateProcS(m_Pid);
			m_Pid = ACE_INVALID_PID;
			return -1;
		}
		break;

	case RUNING_STAT_STOPPING:
		ACE_ASSERT(0);
		break;

	default:
		ACE_ASSERT(0);
		break;
	}	
	return -1;
}




//进程是否存在
bool CMonitorTask_Event::IsTaskExist()
{
	bool bexist = false;

#ifdef OS_WINDOWS
	my_event_t hMonitorEvent;

	ACE_OS::last_error(0);

	MY_EVENT::event_init_for_server (&hMonitorEvent, 
		FALSE, 
		FALSE, 
		0, 
		m_taskInfo.szMonitorEventName);

	int last_error = ACE_OS::last_error();

	if (last_error== ERROR_ALREADY_EXISTS)
	{
		bexist = true;
	}

	MY_EVENT::event_destroy(&hMonitorEvent);

#else
	char szName[128];
	sprintf(szName, "mutex_%s", m_taskInfo.szProcessName);
	ACE_Process_Mutex mutexCreate(szName);
	if (0 == mutexCreate.tryacquire())
	{
		mutexCreate.release();	// 能创建，说明进程不在，
		bexist = false;
	}
	else
		bexist = true;
#endif

	if( bexist && m_Pid == ACE_INVALID_PID )
	{
		m_Pid = FindProcessByName(m_taskInfo.szProcessName);
	}

	return bexist;
}

//进程是否活动.如果活动则肯定存在.
// 这里最好为连续几次无事件在确定	----
bool CMonitorTask_Event::IsTaskActive()
{
	ACE_OS::last_error(0);

	my_event_t hMonitorEvent;

	int init_result = // add by xumx, 2016-10-19
	MY_EVENT::event_init_for_server
	(
		&hMonitorEvent, 
		1,	// auto clean
		#ifdef OS_WINDOWS
			0,  // init value
		#else
			-1,	// -1表示不进行初始化
		#endif
		0, 
		m_taskInfo.szMonitorEventName
	);

	if(init_result < 0){ return(TRUE); }// add by xumx, 2016-10-19

	#ifdef OS_WINDOWS
		if (ACE_OS::last_error() != ERROR_ALREADY_EXISTS)
		{
			MY_EVENT::event_destroy(&hMonitorEvent);
			return false;
		}
	#endif

	ACE_Time_Value wait_time(0,0);

	int nRet = MY_EVENT::event_timedwait(&hMonitorEvent, &wait_time);
	MY_EVENT::event_destroy(&hMonitorEvent);

	if( m_Pid == ACE_INVALID_PID )
	{
		m_Pid = FindProcessByName(m_taskInfo.szProcessName);
	}

	if (nRet == WAIT_OBJECT_0)
	{
		return true;
	}

	return false;

//	return IsTaskExist();
}

//////////////////////////////////////////////////////////////////////////
//CMonitorTask_Common
//////////////////////////////////////////////////////////////////////////


CMonitorTask_Common::CMonitorTask_Common()
{

}

CMonitorTask_Common::~CMonitorTask_Common()
{
}

int CMonitorTask_Common::ForceStopTask()
{
	if(m_nRunningState==RUNING_STAT_STOP)
	{
		//MY_ACE_DEBUG(m_pLogInstance,
		//	(LM_INFO,ACE_TEXT("%D:the program named '%s' is not running!\n"),m_taskInfo.szProcessName));

		return 0;//进程根本没有启动
	}

	if (ACE_INVALID_PID != m_Pid )
	{
		if( m_taskInfo.szExitCommandLine[0] != 0 )
		{//执行关闭命令

			int pid = CreateProcS(false,NULL,m_taskInfo.szExitCommandLine);

			if( pid != ACE_INVALID_PID )
			{

				MY_ACE_DEBUG(m_pLogInstance,
					(LM_INFO,ACE_TEXT("[Info][%D]the program named '%s' is stop by cmd_line %s, the cmd can run success,and wait to stop!\n"),
					m_taskInfo.szProcessName,m_taskInfo.szExitCommandLine));

				int num = 0;
				const int max_num = 5;

				while( ACE::process_active(m_Pid) && num++ < max_num)
				{
					ACE_OS::sleep(3);
				}
			}
			else
			{
				MY_ACE_DEBUG(m_pLogInstance,
					(LM_INFO,ACE_TEXT("[Info][%D]the program named '%s' is stop by cmd_line %s, the cmd can not run success!\n"),
					m_taskInfo.szProcessName,m_taskInfo.szExitCommandLine));

			}
		}

		if( ACE::process_active(m_Pid) )
		{
			MY_ACE_DEBUG(m_pLogInstance,
				(LM_INFO,ACE_TEXT("[Err][%D]the cmd_line can not stop the program %s,so we Terminate pid=%u!\n"),
				m_taskInfo.szProcessName,m_Pid));

			TerminateProcS(m_Pid);
		}
		else
		{
			MY_ACE_DEBUG(m_pLogInstance,
				(LM_INFO,ACE_TEXT("[Info][%D]the cmd_line stop the program %s ok\n"),
				m_taskInfo.szProcessName));
		}

		TerminateProcesLoop(m_taskInfo.szProcessName);
	}

	m_nRunningState = RUNING_STAT_STOP;

	MY_ACE_DEBUG(m_pLogInstance,
		(LM_INFO,ACE_TEXT("[Info][%D]the program named '%s' force to stoped!\n"),m_taskInfo.szProcessName));

	return 0;
}

int CMonitorTask_Common::ForceStartTask()
{
	switch(m_nRunningState)
	{
	case RUNING_STAT_STOP:
	case RUNING_STAT_RUNNING:
		if( IsTaskExist() )
		{
			m_nRunningState=RUNING_STAT_RUNNING;
			m_lastExistTime = m_lastActiveTime = ACE_OS::gettimeofday();

			MY_ACE_DEBUG(m_pLogInstance,
				(LM_DEBUG, ACE_TEXT("[Info][%D]Start Program: '%s' already running!\n"), m_taskInfo.szFileName));

			return 0;
		}
		else
		{
			if( m_taskInfo.szFullCmdLine[0] )
				m_Pid = CreateProcS(m_bDebug,NULL,m_taskInfo.szFullCmdLine,m_taskInfo.szWorkingDirectory,m_taskInfo.priority);
			else
				m_Pid = CreateProcS(m_bDebug,m_taskInfo.szFileName,m_taskInfo.szCmdLine,m_taskInfo.szWorkingDirectory,m_taskInfo.priority);

			if(m_Pid != ACE_INVALID_PID)
			{
				m_nRunningState=RUNING_STAT_RUNNING;
				m_lastExistTime = m_lastActiveTime = ACE_OS::gettimeofday();

				MY_ACE_DEBUG(m_pLogInstance,
					(LM_DEBUG, ACE_TEXT("[Info][%D]Force Start Program: '%s' Success!\n"), m_taskInfo.szFileName));

				return 0;			
			}
			else
			{
				MY_ACE_DEBUG(m_pLogInstance,
					(LM_DEBUG, ACE_TEXT("[Err][%D]Force Start Program: '%s' failed!\n"), m_taskInfo.szFileName));
			}
		}

		return -1;
		break;

	case RUNING_STAT_STARTING:
		return 0;

	case RUNING_STAT_STOPPING:
		ACE_ASSERT(0);
		break;

	default:
		ACE_ASSERT(0);
		break;
	}	
	return -1;
}

//进程是否存在
bool CMonitorTask_Common::IsTaskExist()
{
	if (ACE_INVALID_PID != m_Pid )
	{
		return ACE::process_active(m_Pid);
	}
	else
	{		
		m_Pid = FindProcessByName(m_taskInfo.szProcessName);

		return m_Pid != ACE_INVALID_PID;
	}
	
}

//进程是否活动.如果活动则肯定存在.
bool CMonitorTask_Common::IsTaskActive()
{
	return IsTaskExist();
}
