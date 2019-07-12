/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  SingleProcMgr.cpp
 *
 *  TITLE      :  单个进程管理类实现
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责管理某一进程；
 *
 *****************************************************************************/

#include "LogFileMgr.h"
#include "ConfigFileMgr.h"
#include "MemInfo.h"

#include "SingleProcMgr.h"

/********************************************************************
Public Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CSingleProcMgr::CSingleProcMgr()
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
CSingleProcMgr::CSingleProcMgr()
:	m_pCfgInfo( NULL ), m_bActiveClose( false ),
	m_rLastCpuUasgeAlarmTime( ACE_Time_Value::zero ), 
	m_rLastMemUasgeAlarmTime( ACE_Time_Value::zero ),
	m_rFirstCreateTime( ACE_Time_Value::zero ), m_uRestartTimes( 0 )
{

}

/******************************************************************************
 * FUNCTION    : CSingleProcMgr::~CSingleProcMgr()
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
CSingleProcMgr::~CSingleProcMgr()
{

}

/******************************************************************************
 * FUNCTION    : CSingleProcMgr::Init(IN unsigned int uPID, 
 *				 IN const char * szExeName, 
 *               IN const CMemInfo *pMemInfo, 
 *               IN const SProcCfgInfo *pCfgInfo)
 *       
 * DESCRIPTION : 
 *       -- 初始化
 *       
 * PARAMETER   : 
 *       1: uPID -- IN, 进程标识
 *       2: szExeName -- IN, 进程可执行文件全路径名称
 *       3: pMemInfo -- IN, 内存信息对象
 *       4: pCfgInfo -- IN, 进程配置信息
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CSingleProcMgr::Init(	IN const char * szExeName,
							IN const CMemInfo *pMemInfo, 
							IN const SProcCfgInfo *pCfgInfo )
{
	if( !szExeName || strlen(szExeName) == 0 )
	{
		ASSERT_INFO;
		return false;
	}	
	
	m_pCfgInfo = pCfgInfo;

	if ( !m_rInfo.Init( szExeName, pMemInfo ) )
	{
		return false;
	}

	if ( m_rInfo.m_bIsRunning )
	{
		m_rFirstCreateTime = m_rInfo.m_rCreateTime;
	}

	return true;
}

/******************************************************************************
 * FUNCTION    : CSingleProcMgr::IsCpuUsePercentAlarm()
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
 * SPECIAL    : 调用前应先调用Refresh函数，否则计算数据不会自动更新
 *       
 *****************************************************************************/
bool CSingleProcMgr::IsCpuUsePercentAlarm()
{
	LOCK_THREAD_GUARD_RETURN( false );
	
	ACE_ASSERT( m_pCfgInfo );
	if ( NULL == m_pCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}

	if ( m_pCfgInfo->nCpuUsePercentLimit == DEFAULT_INVALID_VALUE 
		|| m_pCfgInfo->nCpuUseAlarmDuration == DEFAULT_INVALID_VALUE )
	{
		return false;
	}
	
	/*
	if ( !m_rInfo.Refresh() )
	{
		ACE_ERROR( (	LM_ERROR, 
						ACE_TEXT( "%I更新进程[%s]状态失败！！！\n" ),
						m_rInfo.m_strExeName.c_str() ) );
		return false;
	}*/
	
	if ( m_rInfo.m_uCpuUsage >= (unsigned int)m_pCfgInfo->nCpuUsePercentLimit )
	{
		if ( ACE_Time_Value::zero == m_rLastCpuUasgeAlarmTime )
		{
			m_rLastCpuUasgeAlarmTime = ACE_OS::gettimeofday();
		}
		else
		{
			ACE_Time_Value elapsed_time = ACE_OS::gettimeofday() - m_rLastCpuUasgeAlarmTime;
			if ( elapsed_time.sec() >= m_pCfgInfo->nCpuUseAlarmDuration )
			{
				return true;
			}
		}
	}
	else
	{
		m_rLastCpuUasgeAlarmTime = ACE_Time_Value::zero;
	}
	
	return false;
}

/******************************************************************************
 * FUNCTION    : CSingleProcMgr::IsMemUsePercentAlarm()
 *       
 * DESCRIPTION : 
 *       -- 是否有内存利用率超门限告警
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 调用前应先调用Refresh函数，否则计算数据不会自动更新
 *       
 *****************************************************************************/
bool CSingleProcMgr::IsMemUsePercentAlarm()
{
	LOCK_THREAD_GUARD_RETURN( false );
	
	ACE_ASSERT( m_pCfgInfo );
	if ( NULL == m_pCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}

	if ( m_pCfgInfo->nMemUsePercentLimit == DEFAULT_INVALID_VALUE 
		|| m_pCfgInfo->nMemUseAlarmDuration == DEFAULT_INVALID_VALUE )
	{
		return false;
	}
	
	/*
	if ( !m_rInfo.Refresh() )
	{
		ACE_ERROR( (	LM_ERROR, 
						ACE_TEXT( "%I更新进程[%s]状态失败！！！\n" ),
						m_rInfo.m_strExeName.c_str() ) );
		return false;
	}*/
	
	unsigned int uMemUsage = 0;
	if ( DEFAULT_PROC_MEM_PHY_FLAG == m_pCfgInfo->nMEMUseFlag )
	{
		uMemUsage = m_rInfo.m_uPhysicalMemUsage;
	}
	else
	{
		uMemUsage = m_rInfo.m_uVirtualMemUsage;
	}

	if ( uMemUsage >= (unsigned int)m_pCfgInfo->nMemUsePercentLimit )
	{
		if ( ACE_Time_Value::zero == m_rLastMemUasgeAlarmTime )
		{
			m_rLastMemUasgeAlarmTime = ACE_OS::gettimeofday();
		}
		else
		{
			ACE_Time_Value elapsed_time = ACE_OS::gettimeofday() - m_rLastMemUasgeAlarmTime;
			if ( elapsed_time.sec() >= m_pCfgInfo->nMemUseAlarmDuration )
			{
				return true;
			}
		}
	}
	else
	{
		m_rLastMemUasgeAlarmTime = ACE_Time_Value::zero;
	}
	
	return false;
}

/******************************************************************************
 * FUNCTION    : CSingleProcMgr::Start()
 *       
 * DESCRIPTION : 
 *       -- 启动进程
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 当进程从未启动或异常终止时再次启动，有次数限制
 *       
 *****************************************************************************/
bool CSingleProcMgr::Start()
{
	LOCK_THREAD_GUARD_RETURN( false );
	
	if ( m_bActiveClose || m_rInfo.m_bIsRunning )
	{
		return true;
	}
	
	bool bResetStartTime = false;
	ACE_ASSERT( m_pCfgInfo );
	if ( NULL == m_pCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}

	if ( m_pCfgInfo->nRestartInterval != DEFAULT_INVALID_VALUE 
		&& m_pCfgInfo->nMaxRestartTimes != DEFAULT_INVALID_VALUE )
	{			
		ACE_Time_Value rCurrTime = ACE_OS::gettimeofday();

		if ( ACE_Time_Value::zero == m_rFirstCreateTime )
		{
			m_rFirstCreateTime = rCurrTime;
					
			return m_rInfo.Start();
		}
		else
		{
			ACE_Time_Value rTime = rCurrTime - m_rFirstCreateTime;
			if ( rTime.sec() <= m_pCfgInfo->nRestartInterval )
			{	
				if ( m_uRestartTimes >= (unsigned int)m_pCfgInfo->nMaxRestartTimes )
				{
					ACE_DEBUG( (	LM_INFO, 
									ACE_TEXT( "%I进程[%s]在%d小时%d分钟内超过最大重启次数[%d]！！！\n" ), 
									ACE_TEXT( m_rInfo.m_strExeName.c_str() ),
									rTime.sec() / 3600,
									( rTime.sec() % 3600 ) / 60,
									m_pCfgInfo->nMaxRestartTimes ) );

					m_bActiveClose = true;

					return false;
				}
			}
			else
			{
				m_rFirstCreateTime = rCurrTime;
				m_uRestartTimes = 0;
				return m_rInfo.Start();
			}
		}
	}
	
	if ( m_rInfo.Start() )
	{
		m_uRestartTimes++;
		return true;
	}

	return false;
}

/******************************************************************************
 * FUNCTION    : CSingleProcMgr::Close()
 *       
 * DESCRIPTION : 
 *       -- 主动关闭进程
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 调用这个方法后，本进程不再能使用Start方法启动
 *       
 *****************************************************************************/
bool CSingleProcMgr::Close()
{
	LOCK_THREAD_GUARD_RETURN( false );
	
	m_bActiveClose = true;

	return Terminate();
}

/******************************************************************************
 * FUNCTION    : CSingleProcMgr::Restart()
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
 * SPECIAL    : 主动重启，没有次数限制
 *       
 *****************************************************************************/
bool CSingleProcMgr::Restart()
{
	LOCK_THREAD_GUARD_RETURN( false );
	
	m_rFirstCreateTime = ACE_OS::gettimeofday();
	m_uRestartTimes = 0;
	
	if ( m_rInfo.Restart() )
	{
		return true;
	}

	return false;
}

/********************************************************************
Private Function
*********************************************************************/


