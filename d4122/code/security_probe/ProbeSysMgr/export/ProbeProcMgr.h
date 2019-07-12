/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProbeProcMgr.h
 *
 *  TITLE      :  Probe系统进程管理类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责管理并对外提供所有进程的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_PROCESS_MANAGER_H__
#define __PROBE_PROCESS_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleProcMgr.h"

// 进程列表
typedef std::vector< CSingleProcMgr* > VectorSingleProcMgr;

class CMemInfo;

// Probe系统资源管理类
class PROBESYSMGR_CLASS CProbeProcMgr
{
public:
	CProbeProcMgr();
	~CProbeProcMgr();

	// 初始化
	bool Init(	IN const CMemInfo *pMemInfo = NULL, 
				IN const SProcCfgInfo *pCfgInfo = NULL );
	

	// 更新所有进程状态
	int Refresh( OUT VectorSingleProcMgr &vectFailedProc );

	// 获取被监控进程个数
	int GetProcCount() const
	{	return m_vectSingleProc.size();	}

	// 获取被监控的正在运行的进程个数
	int GetProcRunningCount() const;

	// 重启所有终止的进程
	int RestartAllTerminateProc( OUT VectorSingleProcMgr &vectRestartFailedProc );

	// 重启所有进程
	int RestartAllProc( OUT VectorSingleProcMgr &vectRestartFailedProc );

	// 按序号获取指定进程
	CSingleProcMgr* GetProcByNth( IN int nNth );

	// 按进程号获取指定进程
	CSingleProcMgr* GetProcByPID( IN unsigned int uPID );

	// 按进程的可执行文件名称获取指定进程，可以只有文件名，也可以为全路径名
	CSingleProcMgr* GetProcByName( IN const char * szExeFileName );


	// 是否有CPU利用率超门限告警
	bool IsCpuUsePercentAlarm( OUT CSingleProcMgr* &pSingleMgr );

	// 是否有内存利用率超门限告警
	bool IsMemUsePercentAlarm( OUT CSingleProcMgr* &pSingleMgr );

	// 是否有进程异常终止告警
	bool IsAbnormityEndAlarm( OUT CSingleProcMgr* &pSingleMgr);
	
	// 获取进程配置信息对象
	const SProcCfgInfo* GetCfgInfo() const
	{	return m_pCfgInfo;	}

	// 线程同步句柄
	MUTEX& GetMutex()
	{	return m_rMutex;	}



private:
	// 获取所有进程的CPU利用率
	bool getAllProcCpuUsage();

	// 设置被监控进程当前占用CPU利用率
	void setCpuUsage( IN LONGLONG llTotalProcCPUDisp );

private:
	// 进程配置信息对象
	const SProcCfgInfo	*m_pCfgInfo;
	
	// 单个进程管理类对象链表
	VectorSingleProcMgr	m_vectSingleProc;


	// 线程同步句柄
	MUTEX m_rMutex;

public:

//#ifdef OS_WINDOWS
	// 数据缓存
	char m_aBuffer[0x20000];

	// 进程占用CPU信息
	typedef struct _SProcCpuTime
	{
		// 进程标识
		unsigned int uPID;
		// 是否更新
		bool bUpdata;

		// 本进程占用CPU时间差
		LONGLONG llDiffKernalCPUTime;
		LONGLONG llDifftUsrCPUTime;

		// 上次本进程占用CPU时间
		LONGLONG llLastKernalCPUTime;
		LONGLONG llLastUsrCPUTime;

	} SProcCpuTime;
	
	// 所有进程集，计算本进程CPU利用率用
	typedef std::map< unsigned int, SProcCpuTime > MapProc;
	static MapProc m_mapAllProc;

	static bool getTotalCpuUsage(LONGLONG& llKernalTime,LONGLONG& llUsrTime);

	static const SProcCpuTime * FindProcCpu(unsigned int pid)
	{
		MapProc::iterator itr;
		itr = m_mapAllProc.find( pid);

		if( itr != m_mapAllProc.end() )
		{
			return &itr->second;
		}

		return NULL;
	}

//#endif


};

#endif // __PROBE_PROCESS_MANAGER_H__

