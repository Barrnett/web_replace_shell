/******************************************************************************
 *  COPYRIGHTS :  
 *               
 *  FILE NAME  :  ProbeCpuMgr.cpp
 *               
 *  TITLE      :  Probe系统CPU信息管理类实现
 *               
 *  CREATE TIME:  2005-10-27 15:38:30
 *               
 *  AUTHOR     :  
 *               
 *  DESCRIPTION:  
 * 				
 *****************************************************************************/
#include "LogFileMgr.h"
#include "ConfigFileMgr.h"

#include "ProbeCpuMgr.h"


/********************************************************************
Public Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CProbeCpuMgr::CProbeCpuMgr()
 *       
 * DESCRIPTION : 
 *       -- 构造函数
 *       
 * PARAMETER   :
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CProbeCpuMgr::CProbeCpuMgr()
: m_pCfgInfo( NULL ), m_rLastUasgeAlarmTime( ACE_Time_Value::zero )
{
}

/******************************************************************************
 * FUNCTION    : ProbeCpuMgr::~CProbeCpuMgr(
 *       
 * DESCRIPTION : 析构函数
 *       
 *       
 * PARAMETER   : 
 *       
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CProbeCpuMgr::~CProbeCpuMgr()
{
}


/******************************************************************************
 * FUNCTION    : ProbeCpuMgr::Init(
 *       
 * DESCRIPTION : 初始化
 *       
 *       
 * PARAMETER   : 
 *       
 *       
 * RETURN     : 
 *       BOOL -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CProbeCpuMgr::Init( IN const SCpuCfgInfo *pCfgInfo )
{
	m_pCfgInfo = pCfgInfo;

	return m_rInfo.Init();
}

/******************************************************************************
 * FUNCTION    : CProbeCpuMgr::GetCpuInfo()
 *       
 * DESCRIPTION : 
 *       -- 获取CPU状态信息
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       const CCpuInfo& -- CPU状态信息
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
const CCpuInfo& CProbeCpuMgr::GetCpuInfo()
{
	//m_rInfo.Refresh();
	return m_rInfo;
}

/******************************************************************************
 * FUNCTION    : CProbeCpuMgr::IsUsePercentAlarm()
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
bool CProbeCpuMgr::IsUsePercentAlarm()
{
	LOCK_THREAD_GUARD_RETURN( false );
	
	ACE_ASSERT( m_pCfgInfo );
	if ( NULL == m_pCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}
	
	if ( m_pCfgInfo->nUsePercentLimit == DEFAULT_INVALID_VALUE 
		|| m_pCfgInfo->nUseAlarmDuration == DEFAULT_INVALID_VALUE )
	{
		return false;
	}
	
	if ( m_rInfo.m_uCurrentUsage[0] >= (unsigned int)m_pCfgInfo->nUsePercentLimit )
	{
		if ( ACE_Time_Value::zero == m_rLastUasgeAlarmTime )
		{
			m_rLastUasgeAlarmTime = ACE_OS::gettimeofday();
		}
		else
		{
			ACE_Time_Value elapsed_time = ACE_OS::gettimeofday() - m_rLastUasgeAlarmTime;
			if ( elapsed_time.sec() >= m_pCfgInfo->nUseAlarmDuration )
			{
				return true;
			}
		}
	}
	else
	{
		m_rLastUasgeAlarmTime = ACE_Time_Value::zero;
	}

	return false;
}

/********************************************************************
Private Function
*********************************************************************/



