/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  MemInfo.h
 *
 *  TITLE      :  Probe系统内存信息类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责提供某一内存的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_MEMORY_INFOMATION_H__
#define __PROBE_MEMORY_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// 内存信息类
class PROBESYSMGR_CLASS CMemInfo
{
public:
	CMemInfo();
	~CMemInfo();

	// 初始化
	bool Init();

	// 更新内存状态信息
	bool Refresh();
	
private:


public:
	// 物理内存总容量（单位：KB）
	unsigned int	m_uPhysicalMem;
	// 物理内存剩余容量（单位：KB）
	unsigned int	m_uFreePhysicalMem;

	// 交换文件总容量（单位：KB）
	unsigned int	m_uPageFile;
	// 交换文件剩余容量（单位：KB）
	unsigned int	m_uFreePageFile;

	// 虚拟内存总容量（单位：KB）
	unsigned int	m_uVirtualMem;
	// 虚拟内存剩余容量（单位：KB）
	unsigned int	m_uFreeVirtualMem;

	// 当前利用率
	unsigned int	m_uCurrentUsage;

};

#endif // __PROBE_MEMORY_INFOMATION_H__

