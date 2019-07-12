/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProbeSysMgr.h
 *
 *  TITLE      :  Probe系统资源管理类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责管理所有系统资源，提供对系统资源进行查询和操作的接口
 *
 *****************************************************************************/

#ifndef __PROBE_SYSTEM_MANAGER_H__
#define __PROBE_SYSTEM_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LogFileMgr.h"
#include "ConfigFileMgr.h"

#include "ProbeCpuMgr.h"
#include "ProbeMemMgr.h"
#include "ProbeProcMgr.h"
//#include "ProbeOSMgr.h"
//#include "ProbeDiskMgr.h"
//#include "ProbeFileSysMgr.h"
//#include "ProbeCardMgr.h"
//#include "ProbeNetMgr.h"


// Probe系统资源管理类
class PROBESYSMGR_CLASS CProbeSysMgr
{
public:
	CProbeSysMgr();
	~CProbeSysMgr();

	// 初始化
	bool Init( IN const CConfigFileMgr *pCfgFile );

	// 更新所有被监控Probe系统资源状态
	void Refresh();

	// 关闭操作系统
	void CloseOS();

	// 重启操作系统
	void RebootOS();
	
	
	// 配置文件对象
	const CConfigFileMgr* GetCfgFileMgr() const
	{	return m_pCfgFile;	}

	// CPU管理类对象
	CProbeCpuMgr& GetProbeCpuMgr() const
	{	return *m_pCpuMgr;	}
	
	// 内存管理类对象
	CProbeMemMgr& GetProbeMemMgr() const
	{	return *m_pMemMgr;	}
	
	// 进程管理类对象，可能为空
	CProbeProcMgr* GetProbeProcessMgr() const
	{	return m_pProcMgr;  }
/*	
	// 操作系统信息管理类对象
	CProbeOSMgr& GetProbeOSMgr() const
	{	return *m_pOSMgr; 		}
	
	// 磁盘管理类对象
	CProbeDiskMgr& GetProbeDiskMgr() const
	{	return *m_pDiskMgr; 	}
	
	// 文件系统信息管理类对象
	CProbeFileSysMgr& GetProbeFileSysMgr() const
	{	return *m_pFileSysMgr;  }
	
	// 板卡管理类对象
	CProbeCardMgr& GetProbeCardMgr() const
	{	return *m_pCardMgr; 	}
	
	// 网络状态信息管理类对象
	CProbeNetMgr& GetProbeNetMgr() const
	{	return *m_pNetMgr; 	    }
*/

	// 线程同步句柄
	MUTEX& GetMutex()
	{	return m_rMutex;	}

private:
	typedef enum
	{
		E_OS_REBOOT = 1,	// 重启操作系统
		E_OS_CLOSE			// 关闭操作系统
	} ESetOsCmd;

	// 关闭或重启操作系统
	void setOs( IN ESetOsCmd eCmd = E_OS_REBOOT );

private:
	// 配置文件对象
	const CConfigFileMgr	*m_pCfgFile;

	CProbeCpuMgr 			*m_pCpuMgr; 	// CPU管理类对象，负责管理并对外提供所有CPU的一般信息；
	CProbeMemMgr			*m_pMemMgr; 	// 内存管理类对象，负责获取内存信息并对外提供访问接口；
	CProbeProcMgr 			*m_pProcMgr;	// 进程管理类对象，负责管理并对外提供所有所有进程的控制接口和一般信息；
/*	CProbeOSMgr 			*m_pOSMgr; 		// 操作系统信息管理类对象，负责获取操作系统信息并对外提供访问接口；
	CProbeDiskMgr 			*m_pDiskMgr; 	// 磁盘管理类对象，负责管理并对外提供所有磁盘的一般信息；
	CProbeFileSysMgr 		*m_pFileSysMgr; // 文件系统信息管理类对象，负责管理并对外提供所有文件系统的一般信息；
	CProbeCardMgr 			*m_pCardMgr; 	// 板卡管理类对象，负责管理并对外提供所有板卡的一般信息；
	CProbeNetMgr 			*m_pNetMgr; 	// 网络状态信息管理类对象，负责获取网络状态信息并对外提供访问接口；
*/
	
	// 线程同步句柄
	MUTEX m_rMutex;

};

#endif // __PROBE_SYSTEM_MANAGER_H__

