/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProcInfo.cpp
 *
 *  TITLE      :  Probe系统进程信息类实现
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责提供某一进程的一般信息；
 *
 *****************************************************************************/

#include "LogFileMgr.h"
#include "MemInfo.h"
#include "ProcInfo.h"
#include "SingleProcMgr.h"
#include "ace/Process.h"
#include "ProbeProcMgr.h"
#include "cpf/path_tools.h"

/********************************************************************
Public Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CCloseProcGuard::~CCloseProcGuard()
 *       
 * DESCRIPTION : 
 *       -- 确保关闭进程句柄类析构函数
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
#ifdef OS_WINDOWS
CCloseProcGuard::~CCloseProcGuard()
{
	// 关闭进程
	if ( NULL != m_hProcess )
	{
		if ( !CloseHandle( m_hProcess ) )
		{
			ACE_ASSERT( 0 );
			ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I关闭进程失败！！！\n" ) ) );
		}
	}
}
#endif

/******************************************************************************
 * FUNCTION    : CProcInfo::CProcInfo()
 *       
 * DESCRIPTION : 
 *       -- 构造函数
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CProcInfo::CProcInfo()
:	m_pMemInfo( NULL ), m_uPID( ACE_INVALID_PID ), 
	m_uPriority( 0 ), m_bIsRunning( false ),
	m_rCreateTime( ACE_Time_Value::zero ), m_rExitTime( ACE_Time_Value::zero ), 
	m_uCpuUsage( 0 ),  m_llCpuUseTime( 0 ), m_llCurrDispTime( 0 ),
	m_uPhysicalMem( 0 ), m_uPhysicalMemUsage( 0 ), 
	m_uPagefileMem(0),m_uPagefileUsage(0),
	m_rAbnormityEndTime( ACE_Time_Value::zero ), 
	m_bAbnormityEndTimeResetEnable( false ),
	m_uVirtualMem( 0 ), m_uVirtualMemUsage( 0 )
{

}

/******************************************************************************
 * FUNCTION    : CProcInfo::~CProcInfo()
 *       
 * DESCRIPTION : 
 *       -- 析构函数
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CProcInfo::~CProcInfo()
{

}

/******************************************************************************
 * FUNCTION    : CProcInfo::Init(IN unsigned int uPID, IN const std::string &strExeName, 
 *               IN const CMemInfo *pMemInfo)
 *       
 * DESCRIPTION : 
 *       -- 初始化
 *       
 * PARAMETER   : 
 *       1: uPID -- IN, 进程标识
 *       2: strExeName -- IN, 进程可执行文件全路径名称
 *       3: pMemInfo -- IN, 内存信息对象
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CProcInfo::Init(	IN const char * pszExeName,
						IN const CMemInfo *pMemInfo )
{
	if( pszExeName == NULL || strlen(pszExeName) == 0 )
	{
		ASSERT_INFO;
		return false;
	}
		
	m_pMemInfo = pMemInfo;
	m_strExeName = pszExeName;	

	return Refresh(0,0);
}

/******************************************************************************
 * FUNCTION    : CProcInfo::IsMe(IN constchar * szExeFileName)
 *       
 * DESCRIPTION : 
 *       -- 是否是本进程可执行文件名称或全路径可执行文件名称
 *       
 * PARAMETER   : 
 *       1: strExeFileName -- IN, 可执行文件名称或全路径可执行文件名称
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CProcInfo::IsMe( IN const char * pszExeFileName ) const
{
	if( m_strExeName.compare(pszExeFileName) == 0 )
		return true;

	if( strcmp(CPF::GetFileNameFromFullName(m_strExeName.c_str()),pszExeFileName) == 0 )
		return true;

	return false;
	/*
	std::string strExeFileName(pszExeFileName);

	int nRet = strExeFileName.compare( m_strExeName );
	if ( nRet > 0 )
	{
		return false;
	}

	if ( nRet == 0 )
	{
		return false;
	}
	
	int nLen1 = m_strExeName.length();
	int nLen2 = strExeFileName.length();

	nRet = m_strExeName.compare( m_strExeName.substr( nLen1 - nLen2 ) );
	if ( nRet != 0 )
	{
		return false;
	}
*/
	return true;
}

/******************************************************************************
 * FUNCTION    : CProcInfo::Start()
 *       
 * DESCRIPTION : 
 *       -- 启动进程（若已经启动则直接返回）
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CProcInfo::Start()
{
	if ( m_bIsRunning )
	{
		return true;
	}
	
	// 启动进程
	m_uPID = CreateProcS( m_strExeName.c_str() );
	if ( ACE_INVALID_PID == m_uPID )
	{
		m_bIsRunning = false;
        return false; 
	}
	
	m_bIsRunning = true;
	m_rExitTime = ACE_Time_Value::zero;	
	
	return Refresh(0,0);
}

/******************************************************************************
 * FUNCTION    : CProcInfo::Terminate()
 *       
 * DESCRIPTION : 
 *       -- 终止进程
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CProcInfo::Terminate()
{
	if ( !m_bIsRunning )
	{
		return true;
	}

	if ( !TerminateProcS( m_uPID ) )
	{
        return false; 
	}
	
	m_bIsRunning = false;
	m_uPID = ACE_INVALID_PID;
	m_rExitTime = ACE_OS::gettimeofday();

	return true;
}

/******************************************************************************
 * FUNCTION    : CProcInfo::Restart()
 *       
 * DESCRIPTION : 
 *       -- 重启进程
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CProcInfo::Restart()
{
	Terminate();
	return Start();
}

/******************************************************************************
 * FUNCTION    : CProcInfo::Refresh()
 *       
 * DESCRIPTION : 
 *       -- 更新本进程状态信息
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       None
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
#ifdef OS_WINDOWS

bool CProcInfo::Refresh(const LONGLONG& llKernalTime,const LONGLONG& llUsrTime)
{
	m_uPhysicalMem = 0;
	m_uPhysicalMemUsage = 0;	
	
	m_uVirtualMem = 0;
	m_uVirtualMemUsage = 0;

	m_uCpuUsage = 0;

	if ( ACE_INVALID_PID == m_uPID && !getProcStatus() )
	{
		return true;
	}
	
	HANDLE hProcess = NULL;
	

	
	//打开特定进程，以查询进程相关信息
	hProcess = OpenProcess(	PROCESS_QUERY_INFORMATION,
							FALSE,
							m_uPID );
	if ( NULL == hProcess )
	{
		//int nErr = GetLastError();
		m_bIsRunning = false;

		// 更新进程异常终止时间为当前时间
		m_rAbnormityEndTime = ACE_OS::gettimeofday();
		m_bAbnormityEndTimeResetEnable = false;
			
		m_uPID = ACE_INVALID_PID;
		//ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I打开进程失败！！！\n" ) ) );
        return false; 
	}
	else
	{
		if (m_bAbnormityEndTimeResetEnable)
		{
			// 更新进程异常终止时间为0
			m_rAbnormityEndTime = ACE_Time_Value::zero;		
		}
	}

	// 确保关闭进程句柄类对象
	CCloseProcGuard rGuard( hProcess );

	if ( !getBaseInfo( hProcess ) )
	{
		return false;
	}

	getCurrMemUsage( hProcess );
	getCurrCPUUsage(m_uPID,llKernalTime,llUsrTime);


	return true;	
}
#else

bool CProcInfo::Refresh(const LONGLONG& llKernalTime,const LONGLONG& llUsrTime)
{
#ifndef TMP_NO_PROC
	12312312312
#endif
}
#endif

/******************************************************************************
 * FUNCTION    : CProcInfo::IsRunning()
 *       
 * DESCRIPTION : 
 *       -- 是否有利用率超门限告警
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CProcInfo::IsRunning()
{
	Refresh(0,0);
	
	return m_bIsRunning;
}

/******************************************************************************
 * FUNCTION    : CProcInfo::CreateProcS(IN const char * szCmd)
 *       
 * DESCRIPTION : 
 *       -- 创建指定进程
 *       
 * PARAMETER   : 
 *       1: szCmd -- IN, 指定进程可执行文件全路径名称
 *       
 * RETURN     : 
 *       int -- 成功则返回进程ID，否则返回-1
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
int CProcInfo::CreateProcS( IN const char * szCmd )
{
	ACE_Process rProcess;
	ACE_Process_Options rOpt;
	rOpt.command_line( szCmd );
#ifdef OS_WINDOWS
	rOpt.creation_flags( CREATE_NEW_CONSOLE );
#endif

	rOpt.handle_inheritance(false);

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
bool CProcInfo::TerminateProcS( IN unsigned int uPID )
{
	int nRet = ACE::terminate_process( uPID );
	if ( -1 == nRet )
	{
        return false; 
	}
	
	return true;
}

#ifdef OS_WINDOWS
bool CProcInfo::getProcStatus()
{	
    // 获取系统中所有进程的快照 
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if ( INVALID_HANDLE_VALUE == hProcessSnap ) 
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取进程列表失败！！！\n" ) ) );
        return false; 
	}
	
	// 确保关闭进程句柄类对象
	CCloseProcGuard rGuard( hProcessSnap );

    PROCESSENTRY32 pe32 = {0};
	// 在使用PROCESSENTRY32对象之前必须初始化dwSize成员
    pe32.dwSize = sizeof( PROCESSENTRY32 ); 
 
    // 遍历所有进程，获取本进程状态
    if ( !Process32First( hProcessSnap, &pe32 ) ) 
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I遍历进程列表失败！！！\n" ) ) );
        return false; 
	}

	std::string strExeName = CPF::GetFileNameFromFullName(m_strExeName.c_str());

	bool bFound = false;
    do 
    {
		//ACE_DEBUG( ( LM_INFO, ACE_TEXT( "%Ipe32.szExeFile[%s]\n" ), ACE_TEXT( pe32.szExeFile ) ) );
		if ( 0 == strExeName.compare( pe32.szExeFile ) )
		{
			bFound = true;
			break;
		}
    } while ( Process32Next( hProcessSnap, &pe32 ) ); 

	if ( bFound )
	{
		m_bIsRunning = true;
		m_uPID = pe32.th32ProcessID;
	}
	else
	{
		m_uPhysicalMem = 0;
		m_uPhysicalMemUsage = 0;	
		
		m_uVirtualMem = 0;
		m_uVirtualMemUsage = 0;

		m_bIsRunning = false;
		return false;
	}

	return true;
}

bool CProcInfo::getBaseInfo( IN HANDLE hProcess )
{
	ACE_ASSERT( hProcess );
	if ( NULL == hProcess )
	{
		ASSERT_INFO;
		return false;
	}

	m_rCreateTime.set( 0 );
	m_rExitTime.set( 0 );

	//获取进程的优先权
	m_uPriority = GetPriorityClass( hProcess );
	if ( 0 == m_uPriority )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取进程的优先权失败！！！\n" ) ) );
        return false; 
	}
	
	//获取进程的时间信息
	FILETIME CreateTime = { 0, 0 }, ExitTime = { 0, 0 }, KernelTime, UserTime;
	
	if ( !GetProcessTimes(	hProcess, 
							&CreateTime,
							&ExitTime,
							&KernelTime,
							&UserTime ) )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取进程的时间信息失败！！！\n" ) ) );
        return false; 
	}

	m_rCreateTime.set( CreateTime );
	
	// 若进程终止时间不为0，则判断进程已经终止
	if ( 0 == ExitTime.dwHighDateTime && 0 == ExitTime.dwLowDateTime )
	{
		m_bIsRunning = true;
	}
	else
	{
		m_rExitTime.set( ExitTime );
		m_bIsRunning = false;
	}

	return true;
}

bool CProcInfo::getCurrMemUsage( IN HANDLE hProcess )
{
#ifdef _WIN64
	m_uPhysicalMem = 0;
	m_uPhysicalMemUsage = 0;

	m_uPagefileMem = 0;
	m_uPagefileUsage = 0;

	m_uVirtualMem = 0;
	m_uVirtualMemUsage = 0;

	return true;
#else

	ACE_ASSERT( NULL != hProcess && NULL != m_pMemInfo );
	if ( NULL == hProcess || NULL == m_pMemInfo )
	{
		ASSERT_INFO;
		return false;
	}

	if ( !m_bIsRunning )
	{
		return false;
	}

	// 进程的存储区信息
	PROCESS_MEMORY_COUNTERS sPMCounter;
	memset(&sPMCounter,0x00,sizeof(sPMCounter));
	sPMCounter.cb = sizeof(sPMCounter);
	
	//获取进程的存储区信息
	if ( !GetProcessMemoryInfo( hProcess, &sPMCounter, sizeof( sPMCounter ) ) )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取进程的存储区信息失败！！！\n" ) ) );
        return false; 
	}

	m_uPhysicalMem = sPMCounter.WorkingSetSize / MEM_SIZE_DIV;
	m_uPhysicalMemUsage = m_uPhysicalMem * 100 / m_pMemInfo->m_uPhysicalMem;

	m_uPagefileMem = (sPMCounter.PagefileUsage + sPMCounter.WorkingSetSize) / MEM_SIZE_DIV;
	m_uPagefileUsage = m_uPagefileMem*100 / m_pMemInfo->m_uPageFile;

	m_uVirtualMem = sPMCounter.PagefileUsage / MEM_SIZE_DIV;
	m_uVirtualMemUsage = m_uVirtualMem * 100 / m_pMemInfo->m_uVirtualMem;
	
	return true;
#endif
}


bool CProcInfo::getCurrCPUUsage( IN unsigned int uPID,
								const LONGLONG& llKernalTime,
								const LONGLONG& llUsrTime )
{
	const CProbeProcMgr::SProcCpuTime * pcputime = CProbeProcMgr::FindProcCpu(uPID);

	if( !pcputime )
		return false;

	if( llKernalTime + llUsrTime != 0 )
	{
		m_uCpuUsage = (unsigned int)
			((pcputime->llDiffKernalCPUTime+pcputime->llDifftUsrCPUTime)*100/(llKernalTime+llUsrTime));
	}
	else
	{
		m_uCpuUsage = 0;
	}

	return true;
}
#endif // OS_WINDOWS