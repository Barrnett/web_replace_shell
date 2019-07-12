/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  CpuInfo.h
 *
 *  TITLE      :  Probe系统CPU信息类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责提供某一CPU的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_CPU_INFOMATION_H__
#define __PROBE_CPU_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// CPU信息类
class PROBESYSMGR_CLASS CCpuInfo
{
public:
	enum{MAX_CPU_COUNT=16};

public:
	CCpuInfo();
	~CCpuInfo();

	// 初始化
	bool Init();

	// 更新CPU状态信息
	bool Refresh();

private:
	// 获取当前CPU利用率
	void getCurrentUsage();

#ifdef OS_WINDOWS
	// 获取CPU主频
	void getCPUFrequence();
#else
	void GetIdleTime(ACE_UINT64* iIdleTime, ACE_UINT64* iTotalTime);
#endif

public:
	// 品牌
	//std::string		m_strOEM;
	// 型号
	std::string		m_strType;
	
	// CPU个数
	unsigned int	m_uCpuCount;
		
	// 主频（单位：M）
	unsigned int	m_uFrequency;

	// 当前利用率
	unsigned int	m_uCurrentUsage[MAX_CPU_COUNT];

private:

    ACE_UINT64		m_liOldIdleTime[MAX_CPU_COUNT];
    ACE_UINT64		m_liOldSystemTime[MAX_CPU_COUNT];

};

#endif // __PROBE_CPU_INFOMATION_H__

