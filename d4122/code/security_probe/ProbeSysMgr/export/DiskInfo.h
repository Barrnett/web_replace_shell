/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  DiskInfo.h
 *
 *  TITLE      :  Probe系统磁盘信息类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责提供某一磁盘的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_DISK_INFOMATION_H__
#define __PROBE_DISK_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// 磁盘信息类
class PROBESYSMGR_CLASS CDiskInfo
{
public:
	CDiskInfo();
	~CDiskInfo();

	// 初始化
	bool Init();

	//重新获取磁盘信息
	bool Refresh();

public:
	typedef struct tag_DISK_INFO{

		tag_DISK_INFO()
		{
			nCapacity = 0;
			nUsed = 0;
		}

		std::string			name;

		// 总容量（单位：M）
		size_t				nCapacity;

		// 已使用磁盘（单位：M）
		size_t 				nUsed;
	}DISK_INFO;

	std::vector<DISK_INFO>	m_vtDisk;


};

#endif // __PROBE_DISK_INFOMATION_H__



