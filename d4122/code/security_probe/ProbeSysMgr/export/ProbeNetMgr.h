/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  NetMgr.h
 *
 *  TITLE      :  Probe系统网络管理类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责管理并对外提供网络的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_NET_MANAGER_H__
#define __PROBE_NET_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetInfo.h"

// Probe系统资源管理类
class EXPORT_CLASS CProbeNetMgr
{
public:
	CProbeNetMgr();
	~CProbeNetMgr();

	// 初始化
	bool Init( IN const SNetCfgInfo *pCfgInfo = NULL );

	// 获取被监控网络个数（目前无用）
	int GetCpuCount() const
	{	return m_vectInfo.size();	}

	// 按序号获取网络信息（目前仅能取得网络的一般信息，因此nNth参数目前无用）
	const CNetInfo* GetNetInfoByNth( IN int nNth = 0 );
	
	// 是否有利用率超门限告警
	bool IsNetUsePercentAlarm();
	
private:

private:
	// 网络配置信息
	SNetCfgInfo					*m_pCfgInfo;

	// 网络信息类对象链表
	std::vector< CNetInfo* >	m_vectInfo;

};

#endif // __PROBE_NET_MANAGER_H__

