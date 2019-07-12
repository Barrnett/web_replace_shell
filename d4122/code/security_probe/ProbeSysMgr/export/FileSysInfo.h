/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  FileSysInfo.h
 *
 *  TITLE      :  Probe系统文件系统信息类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责提供某一文件系统的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_FILESYS_INFOMATION_H__
#define __PROBE_FILESYS_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// 文件系统信息类
class EXPORT_CLASS CFileSysInfo
{
public:
	CFileSysInfo();
	~CFileSysInfo();

	// 初始化
	bool Init( IN const SFileSysCfgInfo *pCfgInfo = NULL );

	// 是否有利用率超门限告警
	bool IsSysUsePercentAlarm();
	
private:


public:
	// 序号
	int				m_nIndex;
	// 路径
	std::string		m_strPath;
	// 总容量（单位：M）
	int				m_nCapacity;
	// 利用率
	int 			m_nUsePercent;

private:
	// 文件系统配置信息
	SFileSysCfgInfo		*m_pCfgInfo;

};

#endif // __PROBE_FILESYS_INFOMATION_H__

