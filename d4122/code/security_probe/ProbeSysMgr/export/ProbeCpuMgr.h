/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProbeCpuMgr.h
 *
 *  TITLE      :  Probe系统CPU信息管理类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责管理并对外提供所有CPU的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_CPU_MANAGER_H__
#define __PROBE_CPU_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

#include "CpuInfo.h"

// Probe系统CPU信息管理类
class PROBESYSMGR_CLASS CProbeCpuMgr
{
public:
	CProbeCpuMgr();
	~CProbeCpuMgr();

	// 初始化
	bool Init( IN const SCpuCfgInfo *pCfgInfo = NULL );

	// 更新CPU状态信息
	bool Refresh()
	{	return m_rInfo.Refresh();	}
	
	// 是否有利用率超门限告警
	bool IsUsePercentAlarm();
	
	// 获取CPU状态信息
	const CCpuInfo& GetCpuInfo();


	// 获取CPU配置信息对象
	const SCpuCfgInfo* GetCfgInfo() const
	{	return m_pCfgInfo;	}

	// 线程同步句柄
	MUTEX& GetMutex()
	{	return m_rMutex;	}

private:
	// Probe系统CPU信息对象
	CCpuInfo			m_rInfo;

	// CPU配置信息
	const SCpuCfgInfo	*m_pCfgInfo;

	// 上一次CPU利用率告警时间
	ACE_Time_Value		m_rLastUasgeAlarmTime;

	// 线程同步句柄
	MUTEX m_rMutex;

};

#endif // __PROBE_CPU_MANAGER_H__

