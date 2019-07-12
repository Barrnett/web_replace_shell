/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  SingleProcMgr.h
 *
 *  TITLE      :  单个进程管理类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责管理某一进程；
 *
 *****************************************************************************/

#ifndef __PROBE_SINGLE_PROC_MANAGER_H__
#define __PROBE_SINGLE_PROC_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProcInfo.h"

// 进程信息类
class PROBESYSMGR_CLASS CSingleProcMgr
{
public:
	CSingleProcMgr();
	~CSingleProcMgr();

	// 初始化
	bool Init(	IN const char * szExeName,
				IN const CMemInfo *pMemInfo = NULL,
				IN const SProcCfgInfo *pCfgInfo = NULL );

	// 更新进程状态
	bool Refresh()
	{	return false;//return m_rInfo.Refresh();	
	}

	// 是否有利用率超门限告警
	bool IsRunning()
	{	return m_rInfo.IsRunning();	}
	
	// 是否时本进程可执行文件名称或全路径可执行文件名称
	bool IsMe( IN unsigned int uPID ) const
	{	return m_rInfo.m_uPID == uPID;	}

	// 是否时本进程可执行文件名称或全路径可执行文件名称
	bool IsMe( IN const char * pszExeFileName ) const
	{	return m_rInfo.IsMe( pszExeFileName );	}

	// 获取进程信息类对象（若想获取最新信息，应先调用Refresh函数）
	CProcInfo& GetProcInfo()
	{	return m_rInfo;	}

	
	// 是否有利用率超门限告警（调用前应先调用Refresh函数，否则计算数据不会自动更新）
	bool IsCpuUsePercentAlarm();
	
	// 是否有内存利用率超门限告警（调用前应先调用Refresh函数，否则计算数据不会自动更新）
	bool IsMemUsePercentAlarm();

	// 是否有进程异常终止告警
	bool IsAbnormityEndAlarm()
	{ return (m_rInfo.m_rAbnormityEndTime != ACE_Time_Value::zero); }

	// 告警后恢复异常终止时间
	void ResetAbnormityEndTimeZero()
	{ m_rInfo.m_rAbnormityEndTime = ACE_Time_Value::zero; }

	// 启动进程（当进程从未启动或异常终止时再次启动，有次数限制）
	bool Start();

	// 主动关闭进程（调用这个方法后，本进程不再能使用Start方法启动）
	bool Close();

	bool IsActiveClose()
	{	return m_bActiveClose;	}

	
	// 终止进程
	bool Terminate()
	{	return m_rInfo.Terminate();	}
	
	// 重启进程（主动重启，没有次数限制）
	bool Restart();

	// 获取进程配置信息
	const SProcCfgInfo* GetCfgInfo()
	{	return m_pCfgInfo;	}

	const ACE_Time_Value& GetFirstCreateTime()
	{ return m_rFirstCreateTime; }

private:
	// 本进程是否为主动关闭
	bool			m_bActiveClose;

	// 进程信息类对象
	CProcInfo		m_rInfo;
	
	// 进程配置信息
	const SProcCfgInfo	*m_pCfgInfo;

	// 上一次CPU利用率告警时间
	ACE_Time_Value	m_rLastCpuUasgeAlarmTime;

	// 上一次内存利用率告警时间
	ACE_Time_Value	m_rLastMemUasgeAlarmTime;

	// 第一次进程启动时间
	ACE_Time_Value	m_rFirstCreateTime;

	// 进程重启次数
	unsigned int	m_uRestartTimes;

	// 线程同步句柄
	MUTEX m_rMutex;

};

#endif // __PROBE_SINGLE_PROC_MANAGER_H__

