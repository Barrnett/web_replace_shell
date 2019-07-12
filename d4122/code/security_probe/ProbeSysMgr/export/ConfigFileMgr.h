/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ConfigFileMgr.h
 *
 *  TITLE      :  Probe系统监控维护系统配置文件管理类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:
 *
 *****************************************************************************/

#ifndef __CONFIG_FILE_MANAGER_H__
#define __CONFIG_FILE_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

class CIniFile;

// Probe系统监控维护系统配置文件管理类
class PROBESYSMGR_CLASS CConfigFileMgr
{
public:
	CConfigFileMgr();
	~CConfigFileMgr();

	// 初始化
	bool Init( IN const std::string &strCfgFileName );

	// 获取配置文件全路径名称
	const std::string& GetFileName() const
	{	return m_strFileName;	}


	// 获取SNMP配置信息
	const SSnmpCfgInfo* GetSnmpCfgInfo() const
	{	return m_pSnmpCfgInfo;	}

	// 获取控制配置信息
	const SCtlCfgInfo* GetCtlCfgInfo() const
	{	return m_pCtlCfgInfo;	}
	
	// 获取日志配置信息
	const SLogCfgInfo* GetLogCfgInfo() const
	{	return m_pLogCfgInfo;	}

	
	// 获取CPU配置信息
	const SCpuCfgInfo* GetCpuCfgInfo() const
	{	return m_pCpuCfgInfo;	}

	// 获取内存配置信息
	const SMemCfgInfo* GetMemCfgInfo() const
	{	return m_pMemCfgInfo;	}

	// 获取进程配置信息
	const SProcCfgInfo* GetProcCfgInfo() const
	{	return m_pProcCfgInfo;	}

	// 获取磁盘配置信息
	const SDiskCfgInfo* GetDiskCfgInfo() const
	{	return m_pDiskCfgInfo;	}

	// 获取文件系统配置信息
	const SFileSysCfgInfo* GetFileSysCfgInfo() const
	{	return m_pFileSysCfgInfo;	}

	// 获取板卡配置信息
	const SCardCfgInfo* GetCardCfgInfo() const
	{	return m_pCardCfgInfo;	}

	// 获取网络配置信息
	const SNetCfgInfo* GetNetCfgInfo() const
	{	return m_pNetCfgInfo;	}

private:
	// 获取SNMP配置信息
	bool getSnmpCfgInfo();

	// 获取控制配置信息
	bool getCtlCfgInfo();

	// 获取日志配置信息
	bool getLogCfgInfo();

	// 获取CPU配置信息
	bool getCpuCfgInfo();

	// 获取内存配置信息
	bool getMemCfgInfo();

	// 获取磁盘配置信息
	bool getDiskCfgInfo();

	// 获取文件系统配置信息
	bool getFileSysCfgInfo();

	// 获取板卡配置信息
	bool getCardCfgInfo();

	// 获取网络配置信息
	bool getNetCfgInfo();

	// 获取进程配置信息
	bool getProcCfgInfo();

private:
	// 配置文件名称
	std::string		m_strFileName;

	// 配置文件访问对象
	CIniFile		*m_pCfgFile;


	// SNMP配置信息
	SSnmpCfgInfo	*m_pSnmpCfgInfo;

	// 日志配置信息
	SLogCfgInfo		*m_pLogCfgInfo;

	// 控制配置信息
	SCtlCfgInfo		*m_pCtlCfgInfo;

	// CPU配置信息
	SCpuCfgInfo		*m_pCpuCfgInfo;

	// 内存配置信息
	SMemCfgInfo		*m_pMemCfgInfo;

	// 磁盘配置信息
	SDiskCfgInfo	*m_pDiskCfgInfo;

	// 文件系统配置信息
	SFileSysCfgInfo	*m_pFileSysCfgInfo;

	// 板卡配置信息
	SCardCfgInfo	*m_pCardCfgInfo;

	// 网络配置信息
	SNetCfgInfo		*m_pNetCfgInfo;

	// 进程配置信息
	SProcCfgInfo	*m_pProcCfgInfo;

};

#endif // __CONFIG_FILE_MANAGER_H__

