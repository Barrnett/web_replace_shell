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
	// 找到当前用户所以启动的进程或者重要进程，发送kill消息

	// reboot 重新启动系统
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
	//进程与指定cpu绑定

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
	//进程与指定cpu绑定

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
Linux调度器有负载均衡机制，将一个进程在不同CPU之间进行迁移。因此，一个进程运行在哪个CPU上是不确定的，但有时候为了测试数据方便，却有这样的需求，就是将某个进程绑定到一个CPU上运行。

怎么办呢？如果你有源代码，那么很简单，在代码里面加上sched_setaffinity()函数就可以了，但如果没有呢？

没关系，我们有秘密武器，那就是taskset命令。

该命令最简单的用法是：
taskset [mask] [command]
mask是CPU掩码，用16进制数表示，从低位起，为1的位表示该CPU上可以运行该进程。command是要运行的命令。
如taskset 0x00000033 /usr/bin/aaa表示运行/usr/bin/aaa这个进程，并且绑定在0，1，4，5号CPU上。


也许你觉得掩码不直观，没关系，可以使用-c参数，指定CPU的id列表：
taskset -c 0,1,4,5 /usr/bin/aaa

那么，如果是一个已经在运行的程序呢？也有办法，请出-p参数：
task -pc 0,1,4,5 `pidof /usr/bin/aaa`

*/