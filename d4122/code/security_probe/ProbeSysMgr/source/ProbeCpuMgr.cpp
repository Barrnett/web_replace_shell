/******************************************************************************
 *  COPYRIGHTS :  
 *               
 *  FILE NAME  :  ProbeCpuMgr.cpp
 *               
 *  TITLE      :  ProbeϵͳCPU��Ϣ������ʵ��
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
CProbeCpuMgr::CProbeCpuMgr()
: m_pCfgInfo( NULL ), m_rLastUasgeAlarmTime( ACE_Time_Value::zero )
{
}

/******************************************************************************
 * FUNCTION    : ProbeCpuMgr::~CProbeCpuMgr(
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
CProbeCpuMgr::~CProbeCpuMgr()
{
}


/******************************************************************************
 * FUNCTION    : ProbeCpuMgr::Init(
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
bool CProbeCpuMgr::Init( IN const SCpuCfgInfo *pCfgInfo )
{
	m_pCfgInfo = pCfgInfo;

	return m_rInfo.Init();
}

/******************************************************************************
 * FUNCTION    : CProbeCpuMgr::GetCpuInfo()
 *       
 * DESCRIPTION : 
 *       -- ��ȡCPU״̬��Ϣ
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       const CCpuInfo& -- CPU״̬��Ϣ
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



