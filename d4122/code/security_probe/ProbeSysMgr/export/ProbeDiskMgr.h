/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProbeDiskMgr.h
 *
 *  TITLE      :  Probe系统磁盘管理类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责管理并对外提供所有磁盘的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_DISK_MANAGER_H__
#define __PROBE_DISK_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DiskInfo.h"

// Probe系统资源管理类
class PROBESYSMGR_CLASS CProbeDiskMgr
{
public:
	CProbeDiskMgr();
	~CProbeDiskMgr();

	// 初始化
	bool Init(IN const CDiskInfo * pDiskInfo,IN const SDiskCfgInfo *pCfgInfo);

	// 获取被监控磁盘个数
	int GetDiskCount() const;

	// 按序号获取磁盘信息
	const CDiskInfo::DISK_INFO* GetDiskInfoByNth( IN int nNth );
	int	GetDiskAlarmLevelByNth( IN int nNth );

	void genrate_disk_alarm();


private:
	// 磁盘配置信息
	const SDiskCfgInfo		*m_pCfgInfo;

	// 磁盘信息类对象链表
	const CDiskInfo			*m_pDiskInfo;

	int *					m_pCurAlaramLevel;//每个网卡的告警级别
	size_t					m_nInitDiskNums;//初始化的时候磁盘的个数

};

#endif // __PROBE_DISK_MANAGER_H__

