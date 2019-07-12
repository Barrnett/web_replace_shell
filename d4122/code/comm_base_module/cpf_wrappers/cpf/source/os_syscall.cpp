//////////////////////////////////////////////////////////////////////////
//os_syscall.cpp

#include "cpf/os_syscall.h"

#ifdef OS_LINUX
#	define	__USE_GNU
//#	define	_GNU_SOURCE             /* See feature_test_macros(7) */
#	include <sched.h>
#endif

#ifdef OS_WINDOWS
BOOL CPF::MySystemShutdown(UINT uFlags)
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 

	// Get a token for this process. 

	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return( FALSE ); 

	// Get the LUID for the shutdown privilege. 

	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
		&tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	// Get the shutdown privilege for this process. 

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES)NULL, 0); 

	if (GetLastError() != ERROR_SUCCESS) 
		return FALSE; 

	// Shut down the system and force all applications to close. 

	if (!ExitWindowsEx(uFlags | EWX_FORCE, 0) )
		return FALSE; 

	return TRUE;
}

#else
BOOL CPF::MySystemShutdown(UINT uFlags)
{
	// �ҵ���ǰ�û����������Ľ��̻�����Ҫ���̣�����kill��Ϣ

	// reboot ��������ϵͳ
	if (0x02 == uFlags)
	{
		system("sudo /sbin/reboot");
	}
	else if (0x08 == uFlags)
	{
		system("sudo /sbin/halt");
	}
	else
	{
		ACE_ASSERT(false);
	}

	return TRUE;
}

#endif




int CPF::GetCpuCount()
{
#ifdef OS_WINDOWS

	SYSTEM_INFO si;
	memset(&si,0x00,sizeof(si));

	typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
	PGNSI pfnGNSI = (PGNSI) GetProcAddress(GetModuleHandle("kernel32.dll"), "GetNativeSystemInfo");
	if(pfnGNSI)
	{
		pfnGNSI(&si);
	}
	else
	{
		GetSystemInfo(&si);
	}

	return (int)si.dwNumberOfProcessors;
#else

	int NUM_PROCS = sysconf(_SC_NPROCESSORS_CONF);
	return NUM_PROCS;

#endif

}

BOOL CPF::BindProcessCPU(int cpuID)
{
	static int CPU_MAX = CPF::GetCpuCount();
	if (CPU_MAX <= 1)
		return FALSE;

#ifdef OS_WINDOWS
	//������ָ��cpu��

	ACE_UINT32 cpu_mask = 0;
	if (-1 == cpuID)
	{
		for (int i=0; i<CPU_MAX; ++i)
		{
			cpu_mask |= (1 << i);
		}
	}
	else
	{
		cpu_mask = cpuID;
	}

	return SetProcessAffinityMask(GetCurrentProcess(), cpu_mask);

#elif defined(OS_LINUX)

	cpu_set_t cpu_mask;

	CPU_ZERO(&cpu_mask);

	if (-1 == cpuID)
	{
		for (int i=0; i<CPU_MAX; ++i)
		{
			CPU_SET(i, &cpu_mask);
		}
	}
	else
	{
		for(int i = 0; i < 32; ++i)
		{
			if( cpuID && (1<<i) )
			{
				CPU_SET(i, &cpu_mask);
			}
		}
	}

	if (sched_setaffinity(0, sizeof(cpu_mask), &cpu_mask) == -1)
		return FALSE;

	return TRUE;

#else
#	error Unknown operating system (CPF::BindProcessCPU).
#endif

}

BOOL CPF::BindThreadCPU(int cpuID)
{
	static int CPU_MAX = CPF::GetCpuCount();
	if (CPU_MAX <= 1)
		return FALSE;

#ifdef OS_WINDOWS
	//������ָ��cpu��

	ACE_UINT32 cpu_mask = 0;
	if (-1 == cpuID)
	{
		for (int i=0; i<CPU_MAX; ++i)
		{
			cpu_mask |= (1 << i);
		}
	}
	else
	{
		cpu_mask = cpuID;
	}

	return SetThreadAffinityMask(GetCurrentThread(), cpu_mask);

#elif defined(OS_LINUX)

	cpu_set_t cpu_mask;

	CPU_ZERO(&cpu_mask);

	if (-1 == cpuID)
	{
		for (int i=0; i<CPU_MAX; ++i)
		{
			CPU_SET(i, &cpu_mask);
		}
	}
	else
	{
		for(int i = 0; i < 32; ++i)
		{
			if( cpuID && (1<<i) )
			{
				CPU_SET(i, &cpu_mask);
			}
		}
	}

	if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_mask), &cpu_mask) == -1)
		return FALSE;

	return TRUE;

#else
#	error Unknown operating system (CPF::BindThreadCPU).
#endif

}

/*
Linux�������и��ؾ�����ƣ���һ�������ڲ�ͬCPU֮�����Ǩ�ơ���ˣ�һ�������������ĸ�CPU���ǲ�ȷ���ģ�����ʱ��Ϊ�˲������ݷ��㣬ȴ�����������󣬾��ǽ�ĳ�����̰󶨵�һ��CPU�����С�

��ô���أ��������Դ���룬��ô�ܼ򵥣��ڴ����������sched_setaffinity()�����Ϳ����ˣ������û���أ�

û��ϵ�������������������Ǿ���taskset���

��������򵥵��÷��ǣ�
taskset [mask] [command]
mask��CPU���룬��16��������ʾ���ӵ�λ��Ϊ1��λ��ʾ��CPU�Ͽ������иý��̡�command��Ҫ���е����
��taskset 0x00000033 /usr/bin/aaa��ʾ����/usr/bin/aaa������̣����Ұ���0��1��4��5��CPU�ϡ�


Ҳ����������벻ֱ�ۣ�û��ϵ������ʹ��-c������ָ��CPU��id�б�
taskset -c 0,1,4,5 /usr/bin/aaa

��ô�������һ���Ѿ������еĳ����أ�Ҳ�а취�����-p������
task -pc 0,1,4,5 `pidof /usr/bin/aaa`

*/