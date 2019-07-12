/******************************************************************************
 *  COPYRIGHTS :  
 *               
 *  FILE NAME  :  ProbeMemMgr.cpp
 *               
 *  TITLE      :  Probe系统内存信息管理类实现
 *               
 *  CREATE TIME:  2005-10-27 15:38:30
 *               
 *  AUTHOR     :  
 *               
 *  DESCRIPTION:  
 * 				
 *****************************************************************************/

#include "LogFileMgr.h"
#include "ProbeSysCommon.h"
#include "ConfigFileMgr.h"

#include "ProbeMemMgr.h"

/********************************************************************
Public Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CProbeMemMgr::CProbeMemMgr()
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
CProbeMemMgr::CProbeMemMgr()
:	m_pCfgInfo( NULL ), m_rLastUasgeAlarmTime( ACE_Time_Value::zero )
{
}

/******************************************************************************
 * FUNCTION    : ProbeMemMgr::~CProbeMemMgr(
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
CProbeMemMgr::~CProbeMemMgr()
{
}


/******************************************************************************
 * FUNCTION    : ProbeMemMgr::Init(
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
bool CProbeMemMgr::Init( IN const SMemCfgInfo *pCfgInfo )
{
	m_pCfgInfo = pCfgInfo;	
	
	return m_rInfo.Init();
}

/******************************************************************************
 * FUNCTION    : CProbeMemMgr::GetMemInfo()
 *       
 * DESCRIPTION : 
 *       -- 获取内存状态信息
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       const CMemInfo& -- 内存状态信息
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
const CMemInfo& CProbeMemMgr::GetMemInfo()
{
	//m_rInfo.Refresh();
	return m_rInfo;
}

/******************************************************************************
 * FUNCTION    : CProbeMemMgr::IsUsePercentAlarm()
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
bool CProbeMemMgr::IsUsePercentAlarm()
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
	
	// 总容量利用率
	if ( m_rInfo.m_uCurrentUsage >= (unsigned int)m_pCfgInfo->nUsePercentLimit )
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



