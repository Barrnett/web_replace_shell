/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProbeMemMgr.h
 *
 *  TITLE      :  Probe系统内存管理类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责管理并对外提供内存的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_MEMORY_MANAGER_H__
#define __PROBE_MEMORY_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

#include "MemInfo.h"

// Probe系统资源管理类
class PROBESYSMGR_CLASS CProbeMemMgr
{
public:
	CProbeMemMgr();
	~CProbeMemMgr();

	// 初始化
	bool Init( IN const SMemCfgInfo *pCfgInfo = NULL );

	// 更新内存状态信息
	bool Refresh()
	{	return m_rInfo.Refresh();	}
	
	// 获取内存状态信息
	const CMemInfo& GetMemInfo();

	// 是否有利用率超门限告警
	bool IsUsePercentAlarm();
	

	// 获取内存配置信息对象
	const SMemCfgInfo* GetCfgInfo() const
	{	return m_pCfgInfo;	}

	// 线程同步句柄
	MUTEX& GetMutex()
	{	return m_rMutex;	}


private:

private:
	// Probe系统内存信息对象
	CMemInfo			m_rInfo;
	
	// 内存配置信息
	const SMemCfgInfo	*m_pCfgInfo;

	// 上一次内存利用率告警时间
	ACE_Time_Value		m_rLastUasgeAlarmTime;

	// 线程同步句柄
	MUTEX m_rMutex;

};

#endif // __PROBE_MEMORY_MANAGER_H__

