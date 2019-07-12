/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  FileSysMgr.h
 *
 *  TITLE      :  Probe系统文件系统管理类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责管理并对外提供所有文件系统的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_FILESYS_MANAGER_H__
#define __PROBE_FILESYS_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileSysInfo.h"

// Probe系统资源管理类
class EXPORT_CLASS CProbeFileSysMgr
{
public:
	CProbeFileSysMgr();
	~CProbeFileSysMgr();

	// 初始化
	bool Init( IN const SFileSysCfgInfo *pCfgInfo = NULL );

	// 获取被监控文件系统个数
	int GetFileSysCount() const
	{	return m_vectInfo.size();	}

	// 按序号获取文件系统信息
	const CFileSysInfo* GetFileSysInfoByNth( IN int nNth );
	
	
	// 是否有利用率超门限告警
	bool IsFileSysUsePercentAlarm( OUT CFileSysInfo* &pFileSysInfo );
	
private:

private:
	// 文件系统配置信息
	SFileSysCfgInfo					*m_pCfgInfo;

	// 文件系统信息类对象链表
	std::vector< CFileSysInfo* >	m_vectInfo;

};

#endif // __PROBE_FILESYS_MANAGER_H__

