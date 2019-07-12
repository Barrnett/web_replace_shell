//////////////////////////////////////////////////////////////////////////
//TimerTask.cpp

#include "cpf/TimerTask.h"
#include "ace/Process.h"
#include "cpf/path_tools.h"

#ifndef OS_WINDOWS
#include "cpf/my_event.h"
#endif //OS_WINDOWS

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


CTimerRestartProcTask::CTimerRestartProcTask(ACE_Log_Msg * pLogInstance)
{
	m_brestart = true;
	m_pStopEvent = NULL;
	m_Pid = ACE_INVALID_PID;

	m_pLogInstance = pLogInstance;

	if( !m_pLogInstance )
	{
		m_pLogInstance = ACE_Log_Msg::instance ();
	}
}

CTimerRestartProcTask::~CTimerRestartProcTask()
{
	if( m_pStopEvent )
	{
		MY_EVENT::event_destroy(m_pStopEvent);
		delete m_pStopEvent;
		m_pStopEvent = NULL;
	}
}

int CTimerRestartProcTask::init(const char * proc_name,
								bool brestart,
								const char * cmd_line, 
								const char * working_dir,
								const char *stop_event_name,
								unsigned int stop_interval)
{

	m_strProcName = proc_name;
	m_brestart = brestart;
	m_strCmdline = cmd_line;
	m_strWorkingDir = working_dir;
	m_strStopEventName = stop_event_name;

	m_stopInterval.set(stop_interval,0);

	m_pStopEvent = Create_Stop_Event(stop_event_name);

	return 0;
}

int CTimerRestartProcTask::svc()
{
	MY_ACE_DEBUG(m_pLogInstance,
		(LM_DEBUG, ACE_TEXT("[Info][%D]Program: '%s' ready to restart\n"), m_strProcName.c_str()));

	if( ACE_INVALID_PID == m_Pid)
	{
		m_Pid = FindProcessByName(CPF::GetFileNameFromFullName(m_strProcName.c_str()));
	}

	if( m_pStopEvent )
	{
		MY_EVENT::event_signal(m_pStopEvent);
		ACE_OS::sleep(m_stopInterval);
	}

	if (ACE_INVALID_PID != m_Pid &&  ACE::process_active(m_Pid))
	{
		TerminateProcS(m_Pid);
	}

	if( m_brestart )
	{
		m_Pid = CreateProcS(m_strProcName.c_str(),m_strCmdline.c_str(),m_strWorkingDir.c_str());		

		if(m_Pid != ACE_INVALID_PID)
		{
			MY_ACE_DEBUG(m_pLogInstance,
				(LM_DEBUG, ACE_TEXT("[Info][%D]Start Program: '%s' Success!\n"), m_strProcName.c_str()));

			return 0;
		}
		else
		{
			MY_ACE_DEBUG(m_pLogInstance,
				(LM_DEBUG, ACE_TEXT("[Err][%D]Start Program: '%s' failed!\n"), m_strProcName.c_str()));

			return -1;
		}
	}

	return 0;
}

my_event_t * CTimerRestartProcTask::Create_Stop_Event(const char *stop_event_name)
{
	if( stop_event_name && strlen(stop_event_name) > 0 )
	{
		my_event_t * event = new my_event_t;
		if( -1 == MY_EVENT::event_init(event,0,0,0,stop_event_name) )
		{
			delete event;
			return NULL;
		}
		else
		{
			return event;
		}
	}
	else
	{
		return NULL;
	}

	return NULL;
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
int CTimerRestartProcTask::CreateProcS( const char * szAppName, const char *szCmd,const char *szWorkingDirectory)
{
	ACE_Process rProcess;
	ACE_Process_Options rOpt;

	if( szWorkingDirectory )
	{
		rOpt.working_directory(szWorkingDirectory);
	}

	rOpt.handle_inheritance(false);

#ifdef OS_WINDOWS
	rOpt.creation_flags( CREATE_NEW_CONSOLE );
#endif

	rOpt.command_line("%s %s",szAppName,szCmd);


	return rProcess.spawn( rOpt );
}

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
bool CTimerRestartProcTask::TerminateProcS( IN unsigned int uPID )
{
	int nRet = ACE::terminate_process( uPID );
	if ( -1 == nRet )
	{
		return false; 
	}

	return true;
}

unsigned int CTimerRestartProcTask::FindProcessByName(const char * name)
{
#ifdef OS_WINDOWS

	// 获取系统中所有进程的快照 
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if ( INVALID_HANDLE_VALUE == hProcessSnap ) 
	{
		ACE_ASSERT( 0 );
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

#endif

	return -1;
}

//////////////////////////////////////////////////////////////////////////
//CTimerRebootTask
//////////////////////////////////////////////////////////////////////////

CTimerRebootTask::CTimerRebootTask(ACE_Log_Msg * pLogInstance)
{
	m_pLogInstance = pLogInstance;
}

CTimerRebootTask::~CTimerRebootTask()
{

}

int CTimerRebootTask::svc()
{
	MY_ACE_DEBUG(m_pLogInstance,(LM_DEBUG,(ACE_TEXT("[Info][%D]begin to timer_reboot\n"))));

#ifdef OS_WINDOWS
	HANDLE hToken;    
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES, &hToken))    
	{	
		TOKEN_PRIVILEGES tkp; 	
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 	
		tkp.PrivilegeCount = 1;	
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;   	
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0);
		CloseHandle(hToken);    
	}		
	ExitWindowsEx(EWX_REBOOT|EWX_FORCE,0);
#endif

	return 0;

}


