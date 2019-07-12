/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  OsMgr.h
 *
 *  TITLE      :  Probe系统操作系统管理类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责管理并对外提供操作系统的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_OS_MANAGER_H__
#define __PROBE_OS_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OsInfo.h"

// Probe系统操作系统信息管理类
class EXPORT_CLASS CProbeOsMgr
{
public:
	CProbeOsMgr();
	~CProbeOsMgr();

	// 初始化
	bool Init();

	// 获取操作系统信息
	const COsInfo* GetOsInfo() const;
	
private:

private:
	// 操作系统信息类对象
	COsInfo	m_sInfo;

};

#endif // __PROBE_OS_MANAGER_H__

