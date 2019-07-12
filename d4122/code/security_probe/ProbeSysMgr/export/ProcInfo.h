/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProcInfo.h
 *
 *  TITLE      :  Probeϵͳ������Ϣ������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  �����ṩĳһ���̵�һ����Ϣ��
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

	// �������ݽṹ---------------------------------------
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

// ������Ϣ��
class PROBESYSMGR_CLASS CProcInfo
{
public:
	CProcInfo();
	~CProcInfo();

	// ��ʼ��
	bool Init(	IN const char * pszExeName,
				IN const CMemInfo *pMemInfo = NULL );

	// ���±�����״̬��Ϣ
	bool Refresh(const LONGLONG& llKernalTime,const LONGLONG& llUsrTime);
	
	// �Ƿ��������ʳ����޸澯
	bool IsRunning();
	
	// �Ƿ�ʱ�����̿�ִ���ļ����ƻ�ȫ·����ִ���ļ�����
	bool IsMe( IN const char* strExeFileName ) const;

	
	// �������̣����Ѿ�������ֱ�ӷ��أ�
	bool Start();

	// ��ֹ������
	bool Terminate();
	
	// ����������
	bool Restart();


	// ����ָ������
	static int CreateProcS( IN const char* strCmd );

	// ��ָֹ������
	static bool TerminateProcS( IN unsigned int uPID );

#ifdef OS_WINDOWS
private:
	// ��ȡ������״̬
	bool getProcStatus();

	// ��ȡ���̵Ļ�����Ϣ
	bool getBaseInfo( IN HANDLE hProcess );

	// ��ȡ��ǰ�ڴ�������
	bool getCurrMemUsage( IN HANDLE hProcess );
	bool getCurrCPUUsage( IN unsigned int uPID,
		const LONGLONG& llKernalTime,
		const LONGLONG& llUsrTime);
#endif

public:
	// ���̿�ִ���ļ�ȫ·������
	std::string		m_strExeName;

	// ���̱�ʶ
	unsigned int	m_uPID;

	// �������ȼ�
	unsigned int	m_uPriority;
	
	// ���̴���ʱ��
	ACE_Time_Value	m_rCreateTime;

	// ������ֹʱ��
	ACE_Time_Value	m_rExitTime;

	// CPU�ܼ�ռ��ʱ�䣨��λ���룩
	LONGLONG		m_llCpuUseTime;

	// ��ǰCPU������
	unsigned int	m_uCpuUsage;
	
	// ��ǰռ�������ڴ棨��λ��KB��
	unsigned int	m_uPhysicalMem;

	// ��ǰռ�������ڴ�������
	unsigned int	m_uPhysicalMemUsage;

	// ��ǰռ�������ڴ棨��λ��KB��
	unsigned int	m_uVirtualMem;

	// ��ǰռ�������ڴ�������
	unsigned int	m_uVirtualMemUsage;

	//��ǰpagefile�Ĵ�С����λ��KB��
	unsigned int	m_uPagefileMem;

	//��ǰpagefile��������
	unsigned int	m_uPagefileUsage;
	
	// �쳣��ֹʱ��
	ACE_Time_Value	m_rAbnormityEndTime;

	// �쳣��ֹʱ����λ���
	bool          	m_bAbnormityEndTimeResetEnable;


	// ���α�����ռ��CPUʱ�����ϴεĲ�ֵ
	LONGLONG		m_llCurrDispTime;

	// �Ƿ��Ѿ����±��α�����ռ��CPU��Ϣ
	bool			m_bUpdateCpuInfo;

private:	
	// �Ƿ���������
	bool			m_bIsRunning;
	
	// �ڴ���Ϣ����
	const CMemInfo	*m_pMemInfo;

	friend class CSingleProcMgr;
};

#endif // __PROBE_PROCESS_INFOMATION_H__

