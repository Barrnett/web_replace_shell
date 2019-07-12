/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  OsInfo.h
 *
 *  TITLE      :  Probe系统操作系统信息类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责提供某一操作系统的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_OS_INFOMATION_H__
#define __PROBE_OS_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// 操作系统信息类
class PROBESYSMGR_CLASS COsInfo
{
public:
	COsInfo();
	~COsInfo();

	// 初始化
	bool Init();

private:


public:
	// 品牌
	std::string		m_strTrademark;
	// 版本
	std::string		m_strVersion;
	// 发行公司
	std::string		m_strCompany;

	std::string		m_strHostName;

private:
	
};

#endif // __PROBE_OS_INFOMATION_H__

