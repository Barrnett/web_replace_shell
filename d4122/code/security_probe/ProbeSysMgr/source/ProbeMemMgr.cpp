/******************************************************************************
 *  COPYRIGHTS :  
 *               
 *  FILE NAME  :  ProbeMemMgr.cpp
 *               
 *  TITLE      :  Probeϵͳ�ڴ���Ϣ������ʵ��
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
 *       -- ���캯��
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
 * DESCRIPTION : ��������
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
 * DESCRIPTION : ��ʼ��
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
 *       -- ��ȡ�ڴ�״̬��Ϣ
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       const CMemInfo& -- �ڴ�״̬��Ϣ
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
	
	// ������������
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



