/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  NetInfo.h
 *
 *  TITLE      :  Probe系统网络信息类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责提供某一网络的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_NET_INFOMATION_H__
#define __PROBE_NET_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// 网络信息类
class EXPORT_CLASS CNetInfo
{
public:
	CNetInfo();
	~CNetInfo();

	// 初始化
	bool Init( IN const SNetCfgInfo *pCfgInfo = NULL );

	// 是否有利用率超门限告警
	bool IsUsePercentAlarm();
	
private:


public:
	// 序号
	int				m_nIndex;
	// 描述
	std::string		m_strDescribe;
	// 最大速率（单位：M/秒）
	int				m_nMaxSpeed;
	// 当前速率（单位：k/秒）
	int				m_nCurrentSpeed;

private:
	// 网络配置信息
	SNetCfgInfo		*m_pCfgInfo;

};

#endif // __PROBE_NET_INFOMATION_H__

