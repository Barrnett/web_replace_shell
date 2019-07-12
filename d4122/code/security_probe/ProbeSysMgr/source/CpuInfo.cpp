/******************************************************************************
 *  COPYRIGHTS :  
 *               
 *  FILE NAME  :  CpuInfo.cpp
 *               
 *  TITLE      :  Probe系统CPU信息类实现
 *               
 *  CREATE TIME:  2005-10-27 15:38:30
 *               
 *  AUTHOR     :  
 *               
 *  DESCRIPTION:  
 * 				
 *****************************************************************************/

#include "OsRegistTableMgr.h"
#include "CpuInfo.h"

#ifdef OS_WINDOWS

typedef enum _SYSTEM_INFORMATION_CLASS {  
	SystemBasicInformation,  //0
	SystemProcessorInformation,  //1
	SystemPerformanceInformation,  //2
	SystemTimeOfDayInformation,  //3
	SystemPathInformation,  //4
	SystemProcessInformation, //5  进程信息
	SystemCallCountInformation,  
	SystemDeviceInformation,  
	SystemProcessorPerformanceInformation,  
	SystemFlagsInformation,  
	SystemCallTimeInformation,  //10
	SystemModuleInformation,  //模块信息
	SystemLocksInformation,  
	SystemStackTraceInformation,  
	SystemPagedPoolInformation,  
	SystemNonPagedPoolInformation,  
	SystemHandleInformation,  
	SystemObjectInformation,  
	SystemPageFileInformation,  
	SystemVdmInstemulInformation,  
	SystemVdmBopInformation,  //20
	SystemFileCacheInformation,  
	SystemPoolTagInformation,  
	SystemInterruptInformation,  
	SystemDpcBehaviorInformation,  
	SystemFullMemoryInformation,  
	SystemLoadGdiDriverInformation,  
	SystemUnloadGdiDriverInformation,  
	SystemTimeAdjustmentInformation,  
	SystemSummaryMemoryInformation,  
	SystemNextEventIdInformation,  //30
	SystemEventIdsInformation,  
	SystemCrashDumpInformation,  
	SystemExceptionInformation,  
	SystemCrashDumpStateInformation,  
	SystemKernelDebuggerInformation,  
	SystemContextSwitchInformation,  
	SystemRegistryQuotaInformation,  
	SystemExtendServiceTableInformation,  
	SystemPrioritySeperation,  
	SystemPlugPlayBusInformation,  //40
	SystemDockInformation,  
	SystemPowerInformation2,  
	SystemProcessorSpeedInformation,  
	SystemCurrentTimeZoneInformation,  
	SystemLookasideInformation  
} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;  

	#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart)) 
	

    typedef struct
    {
        DWORD dwUnknown1;
        ULONG uKeMaximumIncrement;
        ULONG uPageSize;
        ULONG uMmNumberOfPhysicalPages;
        ULONG uMmLowestPhysicalPage;
        ULONG uMmHighestPhysicalPage;
        ULONG uAllocationGranularity;
        PVOID pLowestUserAddress;
        PVOID pMmHighestUserAddress;
        ULONG uKeActiveProcessors;
        BYTE bKeNumberProcessors;
        BYTE bUnknown2;
        WORD wUnknown3;
    } SYSTEM_BASIC_INFORMATION;

    typedef struct
    {
        LARGE_INTEGER liIdleTime;
        DWORD dwSpare[76];
    } SYSTEM_PERFORMANCE_INFORMATION;

    typedef struct
    {
        LARGE_INTEGER liKeBootTime;
        LARGE_INTEGER liKeSystemTime;
        LARGE_INTEGER liExpTimeZoneBias;
        ULONG uCurrentTimeZoneId;
        DWORD dwReserved;
    } SYSTEM_TIME_INFORMATION;

	typedef struct
		_SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
			LARGE_INTEGER IdleTime;
			LARGE_INTEGER KernelTime;
			LARGE_INTEGER UserTime;
			LARGE_INTEGER Reserved1[2];
			ULONG Reserved2;
	} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

#endif

/********************************************************************
Public Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CCpuInfo::CCpuInfo()
 *       
 * DESCRIPTION : 
 *       -- 构造函数
 *       
 * PARAMETER   :
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CCpuInfo::CCpuInfo()
: m_uCpuCount( 0 )// ,m_uFrequency( 0 )
{
    memset( &m_liOldIdleTime, 0, sizeof( m_liOldIdleTime ) );
	memset( &m_liOldSystemTime, 0, sizeof( m_liOldSystemTime ) );

	memset(m_uCurrentUsage,0x00,sizeof(m_uCurrentUsage));

}

/******************************************************************************
 * FUNCTION    : ProbeCpuInfo::~CCpuInfo(
 *       
 * DESCRIPTION : 析构函数
 *       
 *       
 * PARAMETER   : 
 *       
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CCpuInfo::~CCpuInfo()
{
	
}


/******************************************************************************
 * FUNCTION    : ProbeCpuInfo::Init(
 *       
 * DESCRIPTION : 初始化
 *       
 *       
 * PARAMETER   : 
 *       
 *       
 * RETURN     : 
 *       BOOL -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CCpuInfo::Init()
{	
#ifdef OS_WINDOWS

    SYSTEM_INFO sSysInfo;
	GetSystemInfo( &sSysInfo );
	
	m_uCpuCount = sSysInfo.dwNumberOfProcessors; 
	
	std::string strKey( "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0" );
	std::string strValueName( "ProcessorNameString" );

	if ( !COsRegistTableMgr::GetRegValue( strKey, strValueName, m_strType ) )
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "获取CPU类型失败！！！\n" ) ) );
		//return false;

		getCPUFrequence();

		ACE_TCHAR buf[128] = { 0 };
		ACE_OS::sprintf( buf, "CPU类型未知，CPU主频[%d]MHz", m_uFrequency );
		m_strType = buf;
	}

	CStr strTmp( m_strType.c_str() );
	strTmp.AllTrim();
	m_strType = strTmp.GetString();

	/*
	switch ( sSysInfo.wProcessorArchitecture )
	{
		case PROCESSOR_ARCHITECTURE_INTEL :
			m_strOEM = "INTEL";
			switch( sSysInfo.wProcessorLevel ) 
			{
				case 5:
					m_strType = "Pentium";
					break;
				case 6:
					m_strType = "Pentium2";
					break;
				case 7:
					m_strType = "Pentium3";
					break;
				case 15:
					m_strType = "Pentium4";
					break;
					
				default:
					m_strType = "未知";
					break;
			}
			break;

		default :
			m_strOEM = "未知";
			break;
	}
	
	getCPUFrequence();

*/

#else
	char strBuff[512];
	FILE *fpTemp;

//	if (NULL == (fpTemp = popen("cat /proc/cpuinfo | grep name | cut -f2 -d: | uniq -c", "r")))
	if (NULL == (fpTemp = popen("cat /proc/cpuinfo | grep name | wc", "r")))
	{
		printf("读CPU个数：popen错误, error=%d\n", errno);
		return -1;
	}
	fscanf(fpTemp, "%d", &m_uCpuCount);
	pclose(fpTemp);
//	printf("个数：%d\n", m_uCpuCount);


	if (NULL == (fpTemp = popen("cat /proc/cpuinfo | grep name | cut -f2 -d: ", "r")))
	{
		printf("读CPU信息：popen错误, error=%d\n", errno);
		return -1;
	}
	// Intel(R) Core(TM)2 Duo CPU     T7500  @ 2.20GHz
	fgets(strBuff, 512, fpTemp);
	pclose(fpTemp);
	char* p = strBuff;
	// 在@哪里截断
	while( (*p!='\0') && (*p != '@') )
	{ 
		p++;
	}
	*p = '\0';

	m_strType = strBuff;
//	printf("型号：%s\n", strBuff);


	if (NULL == (fpTemp = popen("cat /proc/cpuinfo | grep 'cpu MHz' | cut -f2 -d:", "r")))
	{
		printf("读CPU频率：popen错误\n");
		return -1;
	}
	fscanf(fpTemp, "%d", &m_uFrequency);
	pclose(fpTemp);
//	printf("频率：%d\n", m_uFrequency);

	GetIdleTime(m_liOldIdleTime, m_liOldSystemTime);

#endif

	memset( &m_liOldIdleTime, 0, sizeof( m_liOldIdleTime ) );
	memset( &m_liOldSystemTime, 0, sizeof( m_liOldSystemTime ) );

	memset(m_uCurrentUsage,0x00,sizeof(m_uCurrentUsage));
	
	return true;
}

/******************************************************************************
 * FUNCTION    : CCpuInfo::Refresh()
 *       
 * DESCRIPTION : 
 *       -- 更新当前CPU状态信息
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CCpuInfo::Refresh()
{
#ifdef OS_WINDOWS

	//CPU利用率的计算公式如下：

	//	100.0 * (IdleTime –PrevIdleTime) / (KernelTime + UserTime – PrevKernelTime – PrevUserTime)

	//	注1：实际内核时间（ActualKernelTime）= KernelTime– IdleTime，内核时间-空闲时间。

	//	注2：全部CPU时间（CpuTime）= UserTime +KernelTime，用户时间+内核时间。

	ACE_ASSERT( NtQuerySystemInformation );
	if ( NULL == NtQuerySystemInformation )
	{
		ASSERT_INFO;
		return false;
	}

	if( m_uCpuCount <= 0 )
	{
		return false;
	}
	
	SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION sppi[MAX_CPU_COUNT];

	if (NtQuerySystemInformation(SystemProcessorPerformanceInformation,
		sppi, m_uCpuCount*sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION), NULL) != 0) 
	{
		return false;
	}

	// computes system global times summingeach processor value

	for (DWORD i = 1; i <= m_uCpuCount&&i<MAX_CPU_COUNT-1; i++) 
	{
		ACE_UINT64 cur_total = sppi[i-1].UserTime.QuadPart+sppi[i-1].KernelTime.QuadPart;

		ACE_UINT64 cur_idle = sppi[i-1].IdleTime.QuadPart;

		if( m_liOldIdleTime[i] == 0 
			|| (cur_total-m_liOldSystemTime[i] == 0)
			)
		{
			m_uCurrentUsage[i] = 0;
		}
		else
		{
			unsigned int idle_rate = (unsigned int)((cur_idle-m_liOldIdleTime[i])*100/(cur_total-m_liOldSystemTime[i]));

			m_uCurrentUsage[i] = (unsigned int)(100-idle_rate);
		}

		m_liOldIdleTime[i] = cur_idle;
		m_liOldSystemTime[i] = cur_total;
	}

	if( m_uCpuCount > 0 )
	{
		unsigned int total_cpu_usage = 0;

		for(unsigned i = 1; i <= m_uCpuCount; ++i )
		{
			total_cpu_usage += m_uCurrentUsage[i];
		}

		m_uCurrentUsage[0] = total_cpu_usage/m_uCpuCount;
	}

#else

	ACE_UINT64	iIdleTime[MAX_CPU_COUNT] = {0};
	ACE_UINT64	iSystemTime[MAX_CPU_COUNT] = {0};

	GetIdleTime(iIdleTime, iSystemTime);

	for (DWORD i = 0; i <= m_uCpuCount&&i<MAX_CPU_COUNT; i++) 
	{
		if
		(
			(m_liOldIdleTime[i] > iIdleTime[i])
		 || (m_liOldSystemTime[i] >= iSystemTime[i])
		)
		{
			m_uCurrentUsage[i] = 0;
		}
		else
		{
			ACE_UINT64 tt = iSystemTime[i] - m_liOldSystemTime[i];
			ACE_UINT64 idl = iIdleTime[i] - m_liOldIdleTime[i];

			m_uCurrentUsage[i] = (unsigned int)(100 * (tt - idl) / tt);
		}

		m_liOldIdleTime[i]   = iIdleTime[i];
		m_liOldSystemTime[i] = iSystemTime[i];
	}
#endif

	return true;
}

/********************************************************************
Private Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CCpuInfo::getCPUFrequence()
 *       
 * DESCRIPTION : 
 *       -- 获取CPU主频
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *       None
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
#ifdef OS_WINDOWS
void CCpuInfo::getCPUFrequence()
{	
#ifdef _WIN64

	m_uFrequency = 0;

#else // _WIN64
	int nTime [ 2 ] = { 0, 0 };
	unsigned int uCPUClock = 0;
	_asm
	{
		rdtsc
		mov nTime[ 0 ], edx
		mov nTime[ 1 ], eax
	}
	Sleep( 1000 );
	_asm
	{
		rdtsc
		sub eax, nTime[ 1 ]
		sub edx, nTime[ 0 ]
		mov uCPUClock, eax
	}
	
	m_uFrequency = uCPUClock / 1000000;
	//ACE_DEBUG( ( LM_INFO, ACE_TEXT( "%ICPU主频:[%uMHz]\n" ), m_uFrequency ) );
#endif // _WIN64
}
#endif // OS_WINDOWS

#ifdef OS_LINUX
/*
cat /proc/stat 
1、  采样两个足够短的时间间隔的Cpu快照，分别记作t1,t2，其中t1、t2的结构均为： 
	(user、nice、system、idle、iowait、irq、softirq、stealstolen、guest)的9元组; 
2、  计算总的Cpu时间片totalCpuTime 
a)         把第一次的所有cpu使用情况求和，得到s1; 
b)         把第二次的所有cpu使用情况求和，得到s2; 
c)         s2 - s1得到这个时间间隔内的所有时间片，即totalCpuTime = s2 - s1 ; 
3、计算空闲时间idle 
	idle对应第四列的数据，用第二次的第四列 - 第一次的第四列即可 
	idle=第二次的第四列 - 第一次的第四列 
6、计算cpu使用率 
	pcpu =100* (total-idle)/total 
*/
void CCpuInfo::GetIdleTime(ACE_UINT64* iIdleTime, ACE_UINT64* iTotalTime)
{
	char strBuff[2048];
	int nCount;
	FILE *fpTemp;

	// 读cpu利用率
	if (NULL == (fpTemp = popen("cat /proc/stat | grep cpu", "r")) )
	{
		printf("读CPU频率：popen错误\n");
		return -1;
	}

	for(int i = 0; i < m_uCpuCount && i < MAX_CPU_COUNT-1; ++i)
	{
		fgets (strBuff, sizeof(strBuff), fpTemp); //从fd文件中读取长度为buff的字符串再存到起始地址为buff这个空间里

		int user_data = 0;
		int nice_data = 0;
		int system_data = 0;
		int idle_data = 0;

		sscanf (strBuff, "%*s %u %u %u %u", &user_data, &nice_data,&system_data, &idle_data);

		iTotalTime[i] = (user_data+nice_data+system_data+idle_data);
		iIdleTime[i] = idle_data;
	}

	fclose(fpTemp);     //关闭文件fd

	return;
}
#endif // OS_LINUX


