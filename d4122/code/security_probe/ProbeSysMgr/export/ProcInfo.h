/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProcInfo.h
 *
 *  TITLE      :  Probe系统进程信息类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  负责提供某一进程的一般信息；
 *
 *****************************************************************************/

#ifndef __PROBE_PROCESS_INFOMATION_H__
#define __PROBE_PROCESS_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

#ifdef OS_WINDOWS	
	#include <windows.h>
	#include <tlhelp32.h>
	#include "WinSDK_Psapi.h"

	// 进程数据结构---------------------------------------
	typedef struct _THREAD_INFO
	{
		LARGE_INTEGER CreateTime;
		DWORD dwUnknown1;
		DWORD dwStartAddress;
		DWORD StartEIP;
		DWORD dwOwnerPID;
		DWORD dwThreadId;
		DWORD dwCurrentPriority;
		DWORD dwBasePriority;
		DWORD dwContextSwitches;
		DWORD Unknown;
		DWORD WaitReason;

	}THREADINFO, *PTHREADINFO;

	typedef struct _UNICODE_STRING
	{
		USHORT Length;
		USHORT MaxLength;
		PWSTR Buffer;
	} UNICODE_STRING;

	typedef struct _PROCESS_INFO
	{
		DWORD dwOffset;
		DWORD dwThreadsCount;
		DWORD dwUnused1[6];
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER KernelTime;
		UNICODE_STRING ProcessName;

		DWORD dwBasePriority;
		DWORD dwProcessID;
		DWORD dwParentProcessId;
		DWORD dwHandleCount;
		DWORD dwUnused3[2];

		DWORD dwVirtualBytesPeak;
		DWORD dwVirtualBytes;
		ULONG dwPageFaults;
		DWORD dwWorkingSetPeak;
		DWORD dwWorkingSet;
		DWORD dwQuotaPeakPagedPoolUsage;
		DWORD dwQuotaPagedPoolUsage;
		DWORD dwQuotaPeakNonPagedPoolUsage;
		DWORD dwQuotaNonPagedPoolUsage;
		DWORD dwPageFileUsage;
		DWORD dwPageFileUsagePeak;

		DWORD dCommitCharge;
		THREADINFO ThreadSysInfo[1];

	} PROCESSINFO, *PPROCESSINFO;

class CCloseProcGuard
{
public:
	CCloseProcGuard( HANDLE hProcess )
		: m_hProcess( hProcess )
	{}

	~CCloseProcGuard();

private:
	HANDLE m_hProcess;
};

#else

	typedef void* HANDLE;

#endif

class CMemInfo;
class CSingleProcMgr;

// 进程信息类
class PROBESYSMGR_CLASS CProcInfo
{
public:
	CProcInfo();
	~CProcInfo();

	// 初始化
	bool Init(	IN const char * pszExeName,
				IN const CMemInfo *pMemInfo = NULL );

	// 更新本进程状态信息
	bool Refresh(const LONGLONG& llKernalTime,const LONGLONG& llUsrTime);
	
	// 是否有利用率超门限告警
	bool IsRunning();
	
	// 是否时本进程可执行文件名称或全路径可执行文件名称
	bool IsMe( IN const char* strExeFileName ) const;

	
	// 启动进程（若已经启动则直接返回）
	bool Start();

	// 终止本进程
	bool Terminate();
	
	// 重启本进程
	bool Restart();


	// 创建指定进程
	static int CreateProcS( IN const char* strCmd );

	// 终止指定进程
	static bool TerminateProcS( IN unsigned int uPID );

#ifdef OS_WINDOWS
private:
	// 获取本进程状态
	bool getProcStatus();

	// 获取进程的基本信息
	bool getBaseInfo( IN HANDLE hProcess );

	// 获取当前内存利用率
	bool getCurrMemUsage( IN HANDLE hProcess );
	bool getCurrCPUUsage( IN unsigned int uPID,
		const LONGLONG& llKernalTime,
		const LONGLONG& llUsrTime);
#endif

public:
	// 进程可执行文件全路径名称
	std::string		m_strExeName;

	// 进程标识
	unsigned int	m_uPID;

	// 进程优先级
	unsigned int	m_uPriority;
	
	// 进程创建时间
	ACE_Time_Value	m_rCreateTime;

	// 进程终止时间
	ACE_Time_Value	m_rExitTime;

	// CPU总计占用时间（单位：秒）
	LONGLONG		m_llCpuUseTime;

	// 当前CPU利用率
	unsigned int	m_uCpuUsage;
	
	// 当前占用物理内存（单位：KB）
	unsigned int	m_uPhysicalMem;

	// 当前占用物理内存利用率
	unsigned int	m_uPhysicalMemUsage;

	// 当前占用虚拟内存（单位：KB）
	unsigned int	m_uVirtualMem;

	// 当前占用虚拟内存利用率
	unsigned int	m_uVirtualMemUsage;

	//当前pagefile的大小（单位：KB）
	unsigned int	m_uPagefileMem;

	//当前pagefile的利用率
	unsigned int	m_uPagefileUsage;
	
	// 异常终止时间
	ACE_Time_Value	m_rAbnormityEndTime;

	// 异常终止时间置位许可
	bool          	m_bAbnormityEndTimeResetEnable;


	// 本次本进程占用CPU时间与上次的差值
	LONGLONG		m_llCurrDispTime;

	// 是否已经更新本次本进程占用CPU信息
	bool			m_bUpdateCpuInfo;

private:	
	// 是否正在运行
	bool			m_bIsRunning;
	
	// 内存信息对象
	const CMemInfo	*m_pMemInfo;

	friend class CSingleProcMgr;
};

#endif // __PROBE_PROCESS_INFOMATION_H__

