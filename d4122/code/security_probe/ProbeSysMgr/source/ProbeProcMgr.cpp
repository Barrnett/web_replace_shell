/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProbeProcMgr.cpp
 *
 *  TITLE      :  Probe系统进程管理类实现
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责管理并对外提供所有进程的一般信息；
 *
 *****************************************************************************/

#include "MemInfo.h"
#include "ProbeProcMgr.h"

#ifdef OS_WINDOWS
	#include <tlhelp32.h>
#endif


CProbeProcMgr::MapProc CProbeProcMgr::m_mapAllProc;
/********************************************************************
Public Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::CProbeProcMgr()
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
CProbeProcMgr::CProbeProcMgr()
: m_pCfgInfo( NULL )
{

}

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::~CProbeProcMgr()
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
CProbeProcMgr::~CProbeProcMgr()
{
	VectorSingleProcMgr::const_iterator itr;
	for ( itr=m_vectSingleProc.begin(); itr!=m_vectSingleProc.end(); itr++ )
	{
		CSingleProcMgr * pMgr = *itr;

		if( pMgr )
		{
			delete pMgr;
		}
	}

	m_vectSingleProc.clear();

}

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::Init(IN const CMemInfo *pMemInfo,
 *								IN const SProcCfgInfo *pCfgInfo)
 *       
 * DESCRIPTION : 
 *       -- 初始化
 *       
 * PARAMETER   : 
 *       1: pMemInfo -- IN, 内存信息对象
 *       2: pCfgInfo -- IN, 进程配置信息对象
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CProbeProcMgr::Init(	IN const CMemInfo *pMemInfo, 
							IN const SProcCfgInfo *pCfgInfo )
{
	m_pCfgInfo = pCfgInfo;
		
	if ( NULL == m_pCfgInfo || m_pCfgInfo->vectComInfo.empty() )
	{
		return false;
	}
	
	CSingleProcMgr *pMgr = NULL;
	VectProcComInfo::const_iterator itr;
	for (	itr=m_pCfgInfo->vectComInfo.begin(); 
			itr!=m_pCfgInfo->vectComInfo.end(); 
			itr++ )
	{
		ACE_ASSERT( !itr->strExeFile.empty() );
		if ( itr->strExeFile.empty() )
		{
			ASSERT_INFO;
			return false;
		}
		
		pMgr = new CSingleProcMgr;
		ACE_ASSERT( pMgr );
		if ( NULL == pMgr )
		{
			ASSERT_INFO;
			return false;
		}

		if ( !pMgr->Init( itr->strExeFile.c_str(), pMemInfo, m_pCfgInfo ) )
		{
			//ACE_ASSERT( 0 );
			ACE_ERROR( (	LM_ERROR, 
							ACE_TEXT( "%I创建进程监控对象[%s]失败！！！\n" ), 
							ACE_TEXT( itr->strExeFile.c_str() ) ) );
			delete pMgr;
			pMgr = NULL;
			continue;
		}

		m_vectSingleProc.push_back( pMgr );
	}
			
    return true; 
}

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::Refresh(OUT VectorSingleProcMgr &vectFailedProc)
 *       
 * DESCRIPTION : 
 *       -- 更新所有进程状态
 *       
 * PARAMETER   : 
 *       1: vectFailedProc -- OUT, 更新失败的进程列表
 *       
 * RETURN     : 
 *       int -- 更新失败的进程个数
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
int CProbeProcMgr::Refresh( OUT VectorSingleProcMgr &vectFailedProc )
{
	LOCK_THREAD_GUARD_RETURN( -1 );

	// 获取所有进程的CPU利用率
	getAllProcCpuUsage();
	
	int nCount = 0;
	CSingleProcMgr *pMgr = NULL;
	VectorSingleProcMgr::const_iterator itr;
	for ( itr=m_vectSingleProc.begin(); itr!=m_vectSingleProc.end(); itr++ )
	{
		pMgr = *itr;
		ACE_ASSERT( pMgr );
		if ( NULL == pMgr )
		{
			ASSERT_INFO;
			return false;
		}

		if ( !pMgr->IsActiveClose() && !pMgr->Refresh() )
		{
			vectFailedProc.push_back( pMgr );
			nCount++;
		}
	}

	return nCount;
}

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::GetProcRunningCount() const
 *       
 * DESCRIPTION : 
 *       -- 获取被监控的正在运行的进程个数
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       int -- 正在运行的进程个数
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
int CProbeProcMgr::GetProcRunningCount() const
{
	LOCK_THREAD_GUARD_RETURN( -1 );
	
	int nCount = 0;
	CSingleProcMgr *pMgr = NULL;
	VectorSingleProcMgr::const_iterator itr;
	for ( itr=m_vectSingleProc.begin(); itr!=m_vectSingleProc.end(); itr++ )
	{
		pMgr = *itr;
		ACE_ASSERT( pMgr );
		if ( NULL == pMgr )
		{
			ASSERT_INFO;
			return -1;
		}

		if ( pMgr->IsActiveClose() || pMgr->IsRunning() )
		{
			nCount++;
		}
	}

	return nCount;
}

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::RestartAllTerminateProc(OUT VectorSingleProcMgr &vectStartFailedProc)
 *       
 * DESCRIPTION : 
 *       -- 重启所有终止的进程
 *       
 * PARAMETER   : 
 *       1: vectStartFailedProc -- OUT, 启动失败的进程列表
 *       
 * RETURN     : 
 *       int -- 启动失败的进程个数
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
int CProbeProcMgr::RestartAllTerminateProc( OUT VectorSingleProcMgr &vectStartFailedProc )
{
	LOCK_THREAD_GUARD_RETURN( -1 );

	int nCount = 0;
	CSingleProcMgr *pMgr = NULL;
	VectorSingleProcMgr::const_iterator itr;
	for ( itr=m_vectSingleProc.begin(); itr!=m_vectSingleProc.end(); itr++ )
	{
		pMgr = *itr;
		ACE_ASSERT( pMgr );
		if ( NULL == pMgr )
		{
			ASSERT_INFO;
			return -1;
		}

		const ACE_Time_Value& procFirstCreateTime = 
			pMgr->GetFirstCreateTime();

		if ( procFirstCreateTime != ACE_Time_Value::zero 
			&& (!pMgr->IsActiveClose()) )
		{
			// 记录日志，进程异常终止。
			ACE_DEBUG( ( LM_INFO, ACE_TEXT( "***********************************************************************************\n" ) ) );
			ACE_DEBUG( ( LM_INFO, ACE_TEXT( "* 进程[%s]异常终止，告警时间[%D]\n" ), ACE_TEXT( ((pMgr->GetProcInfo()).m_strExeName.c_str() )) ) );
			ACE_DEBUG( ( LM_INFO, ACE_TEXT( "***********************************************************************************\n\n" ) ) );
			
		}

		if ( !pMgr->Start() )
		{
			vectStartFailedProc.push_back( pMgr );
			nCount++;
		}
	}

	return nCount;
}

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::RestartAllProc(OUT VectorSingleProcMgr &vectRestartFailedProc)
 *       
 * DESCRIPTION : 
 *       -- 重启所有进程
 *       
 * PARAMETER   : 
 *       1: vectRestartFailedProc -- OUT, 重启失败的进程列表
 *       
 * RETURN     : 
 *       int -- 重启失败的进程个数
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
int CProbeProcMgr::RestartAllProc( OUT VectorSingleProcMgr &vectRestartFailedProc )
{
	LOCK_THREAD_GUARD_RETURN( -1 );

	int nCount = 0;
	VectorSingleProcMgr::const_iterator itr;
	for ( itr=m_vectSingleProc.begin(); itr!=m_vectSingleProc.end(); itr++ )
	{
		CSingleProcMgr * pMgr = *itr;
		ACE_ASSERT( pMgr );
		if ( NULL == pMgr )
		{
			ASSERT_INFO;
			return -1;
		}

		if ( !pMgr->Restart() )
		{
			vectRestartFailedProc.push_back( pMgr );
			nCount++;
		}
	}

	return nCount;
}

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::GetProcByNth(IN int nNth)
 *       
 * DESCRIPTION : 
 *       -- 按序号获取进程信息
 *       
 * PARAMETER   : 
 *       1: nNth -- IN, 序号
 *       
 * RETURN     : 
 *       CSingleProcMgr* -- 进程信息
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CSingleProcMgr* CProbeProcMgr::GetProcByNth( IN int nNth )
{
	LOCK_THREAD_GUARD_RETURN( NULL );

	if ( nNth >= (int)m_vectSingleProc.size() )
	{
		return NULL;
	}

	return m_vectSingleProc[ nNth ];
}

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::GetProcByPID(IN int nPID)
 *       
 * DESCRIPTION : 
 *       -- 按进程号获取进程信息
 *       
 * PARAMETER   : 
 *       1: nPID -- IN, 进程号
 *       
 * RETURN     : 
 *       CSingleProcMgr* -- 进程信息
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CSingleProcMgr* CProbeProcMgr::GetProcByPID( IN unsigned int uPID )
{
	LOCK_THREAD_GUARD_RETURN( NULL );

	VectorSingleProcMgr::iterator itr;
	for ( itr=m_vectSingleProc.begin(); itr!=m_vectSingleProc.end(); itr++ )
	{
		CSingleProcMgr *pMgr = *itr;
		ACE_ASSERT( pMgr );
		if ( NULL == pMgr )
		{
			ASSERT_INFO;
			return NULL;
		}

		if ( pMgr->IsMe( uPID ) )
		{
			return pMgr;
		}
	}

	return NULL;
}

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::GetProcByName(IN const char * szExeFileName)
 *       
 * DESCRIPTION : 
 *       -- 按进程的可执行文件名称获取进程信息，可以只有文件名，也可以为全路径名
 *       
 * PARAMETER   : 
 *       1: szExeFileName -- IN, 进程的可执行文件名称
 *       
 * RETURN     : 
 *       CSingleProcMgr* -- 进程信息
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CSingleProcMgr* CProbeProcMgr::GetProcByName( IN const char * szExeFileName )
{
	LOCK_THREAD_GUARD_RETURN( NULL );

	VectorSingleProcMgr::iterator itr;
	for ( itr=m_vectSingleProc.begin(); itr!=m_vectSingleProc.end(); itr++ )
	{
		CSingleProcMgr * pMgr = *itr;
		ACE_ASSERT( pMgr );
		if ( NULL == pMgr )
		{
			ASSERT_INFO;
			return NULL;
		}

		if ( pMgr->IsMe( szExeFileName ) )
		{		
			return pMgr;
		}
	}

	return NULL;
}

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::IsCpuUsePercentAlarm(OUT CSingleProcMgr* &pSingleMgr)
 *       
 * DESCRIPTION : 
 *       -- 是否有CPU利用率超门限告警
 *       
 * PARAMETER   : 
 *       1: pSingleMgr -- OUT, 进程信息
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CProbeProcMgr::IsCpuUsePercentAlarm( OUT CSingleProcMgr* &pSingleMgr )
{
	LOCK_THREAD_GUARD_RETURN( false );

	CSingleProcMgr *pMgr = NULL;
	VectorSingleProcMgr::iterator itr;
	for ( itr=m_vectSingleProc.begin(); itr!=m_vectSingleProc.end(); itr++ )
	{
		pMgr = *itr;
		ACE_ASSERT( pMgr );
		if ( NULL == pMgr )
		{
			ASSERT_INFO;
			return false;
		}

		if ( pMgr->IsCpuUsePercentAlarm() )
		{
			pSingleMgr = pMgr;
			return true;
		}
	}

	return false;
}

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::IsMemUsePercentAlarm(OUT CSingleProcMgr* &pSingleMgr)
 *       
 * DESCRIPTION : 
 *       -- 是否有内存利用率超门限告警
 *       
 * PARAMETER   : 
 *       1: pSingleMgr -- OUT, 进程信息
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CProbeProcMgr::IsMemUsePercentAlarm( OUT CSingleProcMgr* &pSingleMgr )
{
	LOCK_THREAD_GUARD_RETURN( false );

	CSingleProcMgr *pMgr = NULL;
	VectorSingleProcMgr::iterator itr;
	for ( itr=m_vectSingleProc.begin(); itr!=m_vectSingleProc.end(); itr++ )
	{
		pMgr = *itr;
		ACE_ASSERT( pMgr );
		if ( NULL == pMgr )
		{
			ASSERT_INFO;
			return false;
		}

		if ( pMgr->IsMemUsePercentAlarm() )
		{
			pSingleMgr = pMgr;
			return true;
		}
	}

	return false;
}

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::IsAbnormityEndAlarm(OUT CSingleProcMgr* &pSingleMgr)
 *       
 * DESCRIPTION : 
 *       -- 是否有进程异常终止告警
 *       
 * PARAMETER   : 
 *       1: pSingleMgr -- OUT, 进程信息
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CProbeProcMgr::IsAbnormityEndAlarm( OUT CSingleProcMgr* &pSingleMgr)
{
	LOCK_THREAD_GUARD_RETURN( false );
	
	CSingleProcMgr *pMgr = NULL;
	VectorSingleProcMgr::iterator itr;
	for ( itr=m_vectSingleProc.begin(); itr!=m_vectSingleProc.end(); itr++ )
	{
		pMgr = *itr;
		ACE_ASSERT( pMgr );
		if ( NULL == pMgr )
		{
			ASSERT_INFO;
			return false;
		}
		
		if ( pMgr->IsAbnormityEndAlarm() )
		{
			pSingleMgr = pMgr;
			return true;
		}
	}
	
	return false;
}

/********************************************************************
Private Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::getAllProcCpuUsage()
 *       
 * DESCRIPTION : 
 *       -- 获取所有进程的CPU利用率
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

bool CProbeProcMgr::getAllProcCpuUsage()
{
	/*

#ifdef OS_WINDOWS

	ACE_ASSERT( NtQuerySystemInformation );
	if ( NULL == NtQuerySystemInformation )
	{
		ASSERT_INFO;
		return false;
	}

	PPROCESSINFO pProcessInfo = ( PPROCESSINFO )m_aBuffer;	
	LONGLONG llTotalProcCPUDisp = 0;

	// 获取所有进程的当前状态信息
	LONG status = NtQuerySystemInformation( 5, ( PVOID )pProcessInfo, sizeof( m_aBuffer ), 0 );
	if ( status != NO_ERROR )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取所有进程信息失败！！！\n" ) ) );
        return false;
	}
    
	do
	{
		SProcCpuTime *psTmp = NULL;
		
		MapProc::iterator itr;
		itr = m_mapAllProc.find( pProcessInfo->dwProcessID );
		if ( itr != m_mapAllProc.end() )
		{
			psTmp = &( itr->second );
		}
		else
		{
			SProcCpuTime sTmp;
			sTmp.uPID = pProcessInfo->dwProcessID;
			sTmp.llLastCPUTime = 0;
			psTmp = &(m_mapAllProc[ sTmp.uPID ] = sTmp);
		}
		
		LONGLONG llCurrentTime = 
			( LONGLONG )pProcessInfo->KernelTime.QuadPart 
			+ ( LONGLONG )pProcessInfo->UserTime.QuadPart;
				
		// 设置被监控进程当前占用CPU状态
		CSingleProcMgr* pMgr = GetProcByPID( psTmp->uPID );
		if ( NULL != pMgr )
		{
			CProcInfo &rInfo = pMgr->GetProcInfo();
			rInfo.m_llCpuUseTime = llCurrentTime / 10000000;
			rInfo.m_llCurrDispTime = llCurrentTime - psTmp->llLastCPUTime;
			rInfo.m_bUpdateCpuInfo = true;
		}

		llTotalProcCPUDisp +=	llCurrentTime - psTmp->llLastCPUTime;

		psTmp->bUpdata = true;
		psTmp->llLastCPUTime = llCurrentTime;
		
		if ( 0 == pProcessInfo->dwOffset )
		{
			break;
		}

		pProcessInfo = ( PPROCESSINFO )( ( BYTE* )pProcessInfo + pProcessInfo->dwOffset );

	} while ( true );

	MapProc::iterator itr;
	for ( itr=m_mapAllProc.begin(); itr!= m_mapAllProc.end(); itr++ )
	{
		SProcCpuTime& sTmp = itr->second;

		if ( !sTmp.bUpdata )
		{
			itr = m_mapAllProc.erase( itr );
			continue;
		}

		sTmp.bUpdata = false;
	}
	
	// 设置被监控进程当前占用CPU利用率
	setCpuUsage( llTotalProcCPUDisp );
	
#endif
*/
	return true;
}


bool CProbeProcMgr::getTotalCpuUsage(LONGLONG& llKernalTime,LONGLONG& llUsrTime)
{
#ifdef OS_WINDOWS

	ACE_ASSERT( NtQuerySystemInformation );
	if ( NULL == NtQuerySystemInformation )
	{
		ASSERT_INFO;
		return false;
	}

	char aBuffer[0x20000];

	llKernalTime = llUsrTime = 0;

	PPROCESSINFO pProcessInfo = ( PPROCESSINFO )aBuffer;	

	// 获取所有进程的当前状态信息
	LONG status = NtQuerySystemInformation( 5, ( PVOID )pProcessInfo, sizeof( aBuffer ), 0 );
	if ( status != NO_ERROR )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取所有进程信息失败！！！\n" ) ) );
		return false;
	}

	do
	{
		SProcCpuTime *psTmp = NULL;

		MapProc::iterator itr;
		itr = m_mapAllProc.find( pProcessInfo->dwProcessID );
		if ( itr != m_mapAllProc.end() )
		{
			psTmp = &( itr->second );
		}
		else
		{
			SProcCpuTime sTmp;
			sTmp.uPID = pProcessInfo->dwProcessID;

			sTmp.llDiffKernalCPUTime = 0;
			sTmp.llDifftUsrCPUTime = 0;
			sTmp.llLastKernalCPUTime = 0;
			sTmp.llLastUsrCPUTime = 0;

			psTmp = &(m_mapAllProc[ sTmp.uPID ] = sTmp);
		}

		psTmp->llDiffKernalCPUTime = pProcessInfo->KernelTime.QuadPart-psTmp->llLastKernalCPUTime;
		psTmp->llDifftUsrCPUTime = pProcessInfo->UserTime.QuadPart-psTmp->llLastUsrCPUTime;

		llKernalTime += psTmp->llDiffKernalCPUTime;
		llUsrTime += psTmp->llDifftUsrCPUTime;

		psTmp->bUpdata = true;

		psTmp->llLastKernalCPUTime = pProcessInfo->KernelTime.QuadPart;
		psTmp->llLastUsrCPUTime = pProcessInfo->UserTime.QuadPart;

		if ( 0 == pProcessInfo->dwOffset )
		{
			break;
		}

		pProcessInfo = ( PPROCESSINFO )( ( BYTE* )pProcessInfo + pProcessInfo->dwOffset );

	} while ( true );

	 MapProc::iterator itr=m_mapAllProc.begin(); 
	 while( itr!= m_mapAllProc.end() )
	{
		SProcCpuTime& sTmp = itr->second;

		if ( !sTmp.bUpdata )
		{
			itr = m_mapAllProc.erase( itr );
			continue;
		}
		else
		{
			itr++;
		}

		sTmp.bUpdata = false;
	}


#endif

	return true;
}

/******************************************************************************
 * FUNCTION    : CProbeProcMgr::setCpuUsage(IN LONGLONG llTotalProcCPUDisp)
 *       
 * DESCRIPTION : 
 *       -- 设置被监控进程当前占用CPU利用率
 *       
 * PARAMETER   : 
 *       1: llTotalProcCPUDisp -- OUT, 所有进程当前占用CPU时间与上次之差的和
 *       
 * RETURN     : 
 *       NONE.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
void CProbeProcMgr::setCpuUsage( IN LONGLONG llTotalProcCPUDisp )
{
	if ( 0 == llTotalProcCPUDisp )
	{
		return;
	}

	VectorSingleProcMgr::iterator itr;
	for ( itr=m_vectSingleProc.begin(); itr!=m_vectSingleProc.end(); itr++ )
	{
		CSingleProcMgr *pMgr = *itr;
		ACE_ASSERT( pMgr );
		if ( NULL == pMgr )
		{
			ASSERT_INFO;
			return;
		}
		
		CProcInfo &rInfo = pMgr->GetProcInfo();
		if ( !rInfo.m_bUpdateCpuInfo )
		{
			rInfo.m_uCpuUsage = 0;
			rInfo.m_llCpuUseTime = 0;
			continue;
		}
		else
		{
			rInfo.m_bUpdateCpuInfo = false;
			rInfo.m_uCpuUsage = ( unsigned int )( rInfo.m_llCurrDispTime * 100 / llTotalProcCPUDisp );
		}
	}
}


