/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProcInfo.cpp
 *
 *  TITLE      :  Probeϵͳ������Ϣ��ʵ��
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  �����ṩĳһ���̵�һ����Ϣ��
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
 *       -- ȷ���رս��̾������������
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
	// �رս���
	if ( NULL != m_hProcess )
	{
		if ( !CloseHandle( m_hProcess ) )
		{
			ACE_ASSERT( 0 );
			ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I�رս���ʧ�ܣ�����\n" ) ) );
		}
	}
}
#endif

/******************************************************************************
 * FUNCTION    : CProcInfo::CProcInfo()
 *       
 * DESCRIPTION : 
 *       -- ���캯��
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
 *       -- ��������
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
 *       -- ��ʼ��
 *       
 * PARAMETER   : 
 *       1: uPID -- IN, ���̱�ʶ
 *       2: strExeName -- IN, ���̿�ִ���ļ�ȫ·������
 *       3: pMemInfo -- IN, �ڴ���Ϣ����
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
 *       -- �Ƿ��Ǳ����̿�ִ���ļ����ƻ�ȫ·����ִ���ļ�����
 *       
 * PARAMETER   : 
 *       1: strExeFileName -- IN, ��ִ���ļ����ƻ�ȫ·����ִ���ļ�����
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
 *       -- �������̣����Ѿ�������ֱ�ӷ��أ�
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
	
	// ��������
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
 *       -- ��ֹ����
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
 *       -- ��������
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
 *       -- ���±�����״̬��Ϣ
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
	

	
	//���ض����̣��Բ�ѯ���������Ϣ
	hProcess = OpenProcess(	PROCESS_QUERY_INFORMATION,
							FALSE,
							m_uPID );
	if ( NULL == hProcess )
	{
		//int nErr = GetLastError();
		m_bIsRunning = false;

		// ���½����쳣��ֹʱ��Ϊ��ǰʱ��
		m_rAbnormityEndTime = ACE_OS::gettimeofday();
		m_bAbnormityEndTimeResetEnable = false;
			
		m_uPID = ACE_INVALID_PID;
		//ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I�򿪽���ʧ�ܣ�����\n" ) ) );
        return false; 
	}
	else
	{
		if (m_bAbnormityEndTimeResetEnable)
		{
			// ���½����쳣��ֹʱ��Ϊ0
			m_rAbnormityEndTime = ACE_Time_Value::zero;		
		}
	}

	// ȷ���رս��̾�������
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
 *       -- �Ƿ��������ʳ����޸澯
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
 *       -- ����ָ������
 *       
 * PARAMETER   : 
 *       1: szCmd -- IN, ָ�����̿�ִ���ļ�ȫ·������
 *       
 * RETURN     : 
 *       int -- �ɹ��򷵻ؽ���ID�����򷵻�-1
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
 *       -- ��ָֹ������
 *       
 * PARAMETER   : 
 *       1: uPID -- IN, ָ�����̱�ʶ
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
    // ��ȡϵͳ�����н��̵Ŀ��� 
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if ( INVALID_HANDLE_VALUE == hProcessSnap ) 
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ȡ�����б�ʧ�ܣ�����\n" ) ) );
        return false; 
	}
	
	// ȷ���رս��̾�������
	CCloseProcGuard rGuard( hProcessSnap );

    PROCESSENTRY32 pe32 = {0};
	// ��ʹ��PROCESSENTRY32����֮ǰ�����ʼ��dwSize��Ա
    pe32.dwSize = sizeof( PROCESSENTRY32 ); 
 
    // �������н��̣���ȡ������״̬
    if ( !Process32First( hProcessSnap, &pe32 ) ) 
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I���������б�ʧ�ܣ�����\n" ) ) );
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

	//��ȡ���̵�����Ȩ
	m_uPriority = GetPriorityClass( hProcess );
	if ( 0 == m_uPriority )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ȡ���̵�����Ȩʧ�ܣ�����\n" ) ) );
        return false; 
	}
	
	//��ȡ���̵�ʱ����Ϣ
	FILETIME CreateTime = { 0, 0 }, ExitTime = { 0, 0 }, KernelTime, UserTime;
	
	if ( !GetProcessTimes(	hProcess, 
							&CreateTime,
							&ExitTime,
							&KernelTime,
							&UserTime ) )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ȡ���̵�ʱ����Ϣʧ�ܣ�����\n" ) ) );
        return false; 
	}

	m_rCreateTime.set( CreateTime );
	
	// ��������ֹʱ�䲻Ϊ0�����жϽ����Ѿ���ֹ
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

	// ���̵Ĵ洢����Ϣ
	PROCESS_MEMORY_COUNTERS sPMCounter;
	memset(&sPMCounter,0x00,sizeof(sPMCounter));
	sPMCounter.cb = sizeof(sPMCounter);
	
	//��ȡ���̵Ĵ洢����Ϣ
	if ( !GetProcessMemoryInfo( hProcess, &sPMCounter, sizeof( sPMCounter ) ) )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I��ȡ���̵Ĵ洢����Ϣʧ�ܣ�����\n" ) ) );
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