/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProbeSysCommon.h
 *
 *  TITLE      :  Probe系统数据结构及常量定义
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:
 *
 *****************************************************************************/

#ifndef __PROBE_SYS_COMMON_H__
#define __PROBE_SYS_COMMON_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(OS_WINDOWS)
	#define PROBE_SYSMGR_Export_Flag __declspec (dllexport)
	#define PROBE_SYSMGR_Import_Flag __declspec (dllimport)
#else
	#define PROBE_SYSMGR_Export_Flag
	#define PROBE_SYSMGR_Import_Flag
#endif

#ifdef __cplusplus
	#define PROBE_SYSMGR_API_Export_Flag	extern "C" PROBE_SYSMGR_Export_Flag
	#define PROBE_SYSMGR_API_Import_Flag	extern "C" PROBE_SYSMGR_Import_Flag
#else
	#define PROBE_SYSMGR_API_Export_Flag	PROBE_SYSMGR_Export_Flag
	#define PROBE_SYSMGR_API_Import_Flag	PROBE_SYSMGR_Import_Flag
#endif

#define PROBE_SYSMGR_CLASS_Export_Flag	PROBE_SYSMGR_Export_Flag
#define PROBE_SYSMGR_CLASS_Import_Flag	PROBE_SYSMGR_Import_Flag

#ifdef PROBESYSMGR_EXPORTS
	#define PROBESYSMGR_API PROBE_SYSMGR_API_Export_Flag
	#define PROBESYSMGR_CLASS PROBE_SYSMGR_CLASS_Export_Flag
#else
	#define PROBESYSMGR_API PROBE_SYSMGR_API_Import_Flag
	#define PROBESYSMGR_CLASS PROBE_SYSMGR_CLASS_Import_Flag
#endif

#ifdef PROBESYSMGR_EXPORTS
	#define PROBESYS_MONITOR_API PROBE_SYSMGR_API_Export_Flag
	#define PROBESYS_MONITOR_CLASS PROBE_SYSMGR_CLASS_Export_Flag
#else
	#define PROBESYS_MONITOR_API PROBE_SYSMGR_API_Import_Flag
	#define PROBESYS_MONITOR_CLASS PROBE_SYSMGR_CLASS_Import_Flag
#endif


#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include "ace/Atomic_Op_T.h"
#include "ace/Thread_Mutex.h"
#include "ace/Task.h"

#include "cpf/MyStr.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

#ifdef OS_WINDOWS
	#pragma warning( disable : 4786 )
#endif

#include "cpf/cpf.h"

/*****************************************************************************
// 常量
******************************************************************************/
// Probe告警定义
typedef enum
{
	E_ALARM_CPU_USEPECENT,			// Probe系统CPU利用率超过门限告警
	E_ALARM_CPU_TEMPERATURE,		// Probe系统CPU温度超过门限告警

	E_ALARM_MEM_USEPECENT,			// Probe系统内存利用率超过门限告警

	E_ALARM_DISK_USEPECENT,			// Probe系统磁盘利用率超过门限告警

	E_ALARM_FILESYS_USEPECENT,		// Probe系统文件系统利用率超过门限告警

	E_ALARM_NET_USEPECENT,			// Probe系统网络利用率超过门限告警

	E_ALARM_PROCESS_CPU_USEPECENT,	// 进程占用CPU利用率超过门限告警
	E_ALARM_PROCESS_MEM_USEPECENT	// 进程占用CPU利用率超过门限告警

} EProbeSysAlarmType;

// 告警处理方式定义
typedef enum
{
	E_HANDLE_EXEUTE_FILE,			// 执行配置文件中指定的程序

	E_HANDLE_RESTART_PROCESS,		// 重启进程
	E_HANDLE_TERMINATE_PROCESS,		// 关闭进程

	E_HANDLE_REBOOT_OS,				// 重启操作系统
	E_HANDLE_CLOSE_OS               // 关闭操作系统

} EProbeSysAlarmHandleType;

// 缺省定义--------------------------------------------------------

// 缺省整数无效值
#define DEFAULT_INVALID_VALUE					0

// 内存换算基数
#define MEM_SIZE_DIV							1024

// 文件名称-------------------------
// 配置文件
#define DEFAULT_PROBESYS_CFGFILE_NAME			"ProbeSysMonitor.ini"

// 日志文件
#define PROBESYSMONITOR_INIT_LOGFILE			"ProbeSysMonitorInit.log"				// 记录初始化错误
#define DEFAULT_PROBESYS_LOGFILE_NAME			"ProbeSysMonitor_%04d%02d%02d%02d.log"	// 记录运行时日志

// 控制-----------------------------

// 进程占用内存利用率超门限告警，是以物理内存还是虚拟内存计算缺省选项
#define DEFAULT_PROC_MEM_PHY_FLAG				1		// 物理内存
#define DEFAULT_PROC_MEM_VIR_FLAG				2		// 虚拟内存


// 配置文件项定义---------------------------------------------------

// 配置文件中由于操作系统差异造成的文件路径写法区分
#define OS_WINDOWS_ID							"WINDOWS_"
#define OS_LINUX_ID								"LINUX_"

// 控制-----------------------------
#define SECTION_CONTROL							"CONTROL"

// 控制台窗口标题，若不指定则缺省为"ProbeSysMonitor"
#define ITEM_CTL_CONSOLE_TITLE					"ConsoleTitle"
// 缺省控制台窗口标题
#define DEFAULT_CONSOLE_TITLE					"ProbeSysMonitor"

// 定时轮询间隔时间（单位：秒）
#define ITEM_CTL_TIMER_INTERVAL					"TimerInterval"
// 缺省定时轮询时间间隔（单位：秒）
#define DEFAULT_TIMER_INTERVAL					5


// 定时显示本系统运行状态，其间隔时间与TimerInterval的比数
#define ITEM_CTL_SHOW_TIMES						"ShowTimes"
// 缺省定时显示本系统运行状态间隔时间与TimerInterval的比数
#define DEFAULT_SHOW_TIMES						2


// 定时重启操作系统间隔时间（单位：小时），为0表示不定时重启
#define ITEM_CTL_REBOOT_OS_INTERVAL				"RebootOsInterval"
// 缺省定时重启操作系统间隔时间（单位：小时）
#define DEFAULT_REBOOT_OS_INTERVAL				0


// 定时重启操作系统间隔时间（单位：小时），为0表示不定时重启
#define ITEM_CTL_RESTART_RPOC_INTERVAL			"ReStartProcInterval"

// 是否自动注册到操作系统，以使操作系统重启后能够自动启动ProbeSysMonitor
#define ITEM_CTL_AUTO_REGIST					"AutoRegist"
// 缺省是否自动注册到操作系统
#define DEFAULT_AUTO_REGIST						true


// 是否在告警时主动发送Trap
#define ITEM_CTL_SEND_TRAP						"SendTrap"
// 缺省是否在告警时主动发送Trap
#define DEFAULT_SEND_TRAP						true



// CPU利用率超门限告警处理方法
#define ITEM_CTL_CPUUSEALARMHANDLE_DEFAULT		"CpuUseAlarmHandle_default"
#define ITEM_CTL_CPUUSEALARMHANDLE_CUSTOM		"CpuUseAlarmHandle_custom"

// 内存利用率超门限告警处理方法
#define ITEM_CTL_MEMUSEALARMHANDLE_DEFAULT		"MemUseAlarmHandle_default"
#define ITEM_CTL_MEMUSEALARMHANDLE_CUSTOM		"MemUseAlarmHandle_custom"

// 进程占用CPU利用率超门限告警处理方法
#define ITEM_CTL_PROCCPUUSEALARMHANDLE_DEFAULT	"ProcCpuUseAlarmHandle_default"
#define ITEM_CTL_PROCCPUUSEALARMHANDLE_CUSTOM	"ProcCpuUseAlarmHandle_custom"

// 进程占用内存利用率超门限告警处理方法
#define ITEM_CTL_PROCMEMUSEALARMHANDLE_DEFAULT	"ProcMemUseAlarmHandle_default"
#define ITEM_CTL_PROCMEMUSEALARMHANDLE_CUSTOM	"ProcMemUseAlarmHandle_custom"

// 日志-----------------------------
#define SECTION_LOG								"LOG"

// 定时日志时间间隔与TimerInterval的比数
#define ITEM_LOG_TIMES							"LogTimes"
// 缺省日志时间间隔与TimerInterval的比数
#define DEFAULT_LOG_TIMES						10

// 定时改变日志输出到新的文件的时间间隔
#define ITEM_CHANGE_LOGFILE_INTERVAL			"ChangeLogFileInterval"
// 缺省改变日志输出到新的文件的时间间隔（单位：小时）
#define DEFAULT_CHANGE_LOGFILE_INTERVAL			24


// 是否同时输出到控制台
#define ITEM_LOG_PUTOUT_TO_CONSOLE				"PutOutToConsole"
// 缺省是否同时输出到控制台
#define DEFAULT_LOG_PUTOUT_TO_CONSOLE			false

// 日志文件名称
#define ITEM_LOG_FILE_PATH						"LogFilePath"
// 缺省日志文件名称
#define DEFAULT_PROBESYS_LOGFILE_PATH			"LogDir"


// CPU-----------------------------
#define SECTION_CPU								"CPU"

// 利用率门限
#define ITEM_CPU_USEPERCENTLIMIT				"UsePercent"
// 持续超过利用率门限的时间门限
#define ITEM_CPU_USEALARMDURATION				"UseAlarmDuration"

// 温度门限
#define ITEM_CPU_TEMPERATURELIMIT				"TemperatureLimit"
// 持续超过温度门限的时间门限
#define ITEM_CPU_TEMPALARMDURATION				"TempAlarmDuration"

// 内存-----------------------------
#define SECTION_MEM								"MEM"

// 利用率门限
#define ITEM_MEM_USEPERCENTLIMIT				"UsePercent"
// 持续超过利用率门限的时间门限
#define ITEM_MEM_USEALARMDURATION				"UseAlarmDuration"

// 进程-----------------------------
#define SECTION_PROC							"PROC"

// CPU利用率门限
#define ITEM_PROC_CPU_USEPERCENTLIMIT			"CPUUsePercent"
// CPU持续超过利用率门限的时间门限
#define ITEM_PROC_CPU_USEALARMDURATION			"CPUUseAlarmDuration"

// 进程占用内存利用率超门限告警，是以物理内存还是虚拟内存计算选项
#define ITEM_PROC_MEM_MEMUSEFLAG				"MEMUseFlag"
// 内存利用率门限
#define ITEM_PROC_MEM_USEPERCENTLIMIT			"MEMUsePercent"
// 内存持续超过利用率门限的时间门限
#define ITEM_PROC_MEM_USEALARMDURATION			"MEMUseAlarmDuration"

// 进程两次重启时间最小间隔
#define ITEM_PROC_RESTARTINTERVAL				"RestartInterval"
// 进程最多重启次数
#define ITEM_PROC_MAXRESTARTTIMES				"MaxRestartTimes"

// 被监控的进程的全路径名称
#define ITEM_PROC_FULLPATH_EXEFILE				"ExeFullPath"


// SNMP-----------------------------
#define SECTION_SNMP							"SNMP"

// 是否开启SNMP服务，0表示不开启，1表示开启，若未指定则缺省为不开启
#define ITEM_SNMP_OPEN_SNMP						"OpenSnmpServer"
// 缺省是否开启SNMP服务
#define DEFAULT_OPEN_SNMP						false

// SNMP服务端口号
#define ITEM_SNMP_PORT							"SnmpPort"
// 缺省SNMP服务端口号
#define DEFAULT_SNMP_PORT						16100

// zhugf add 2005.12.6:
// 缺省的Mib库名称，默认为程序运行路径下的ProbeSysRmon
const char ITEM_SYSMONITOR_MIB_NAME[] = 	    "MibLib";
const char DEFAULT_SYSMONITOR_MIB_NAME[] =      "ProbeSysRmon";

// 缺省Trap source TrapSource
const char ITEM_SYS_TRAP_SOURCE[] = 	       	"TrapSource";
const char DEFAULT_SYS_TRAP_SOURCE[] =          "ProbeSysMonitor";

// 缺省Trap 目的地址 DefaultTrapAddr
const char ITEM_SYS_TRAP_DADDRESS[] = 	       	"DefaultTrapAddr";
const char DEFAULT_SYS_TRAP_DADDRESS[] =        "127.0.0.1/162";

// Trap 轮询时间间隔,单位为秒
const char ITEM_SYS_TRAP_INTERVAL[] = 			"TrapInterval";
const int DEFAULT_SYS_TRAP_INTERVAL =           1;

// 主线程循环休眠时间,单位为微妙
const long L_MAIN_TREAD_LOOP_SLEEP_US =         100000L;
/****************************************************************************
// 全局变量
*****************************************************************************/
// WINDOWS系统信息获取函数指针
#ifdef OS_WINDOWS
	//类型定义
	typedef LONG ( WINAPI *PROCNTQSI )( UINT, PVOID, ULONG, PULONG );
	extern PROCNTQSI NtQuerySystemInformation;
#endif

/****************************************************************************
// 数据结构
*****************************************************************************/
// 线程安全交互整数类型
typedef ACE_Atomic_Op< ACE_Thread_Mutex, int > SafeInt;

// 线程安全对象类型
typedef ACE_Thread_Mutex MUTEX;

#define LOCK_THREAD_GUARD ACE_GUARD( MUTEX, _MON_, ( MUTEX& )m_rMutex )

#define LOCK_THREAD_GUARD_RETURN( _RETURN_ ) ACE_GUARD_RETURN( MUTEX, _MON_, ( MUTEX& )m_rMutex, _RETURN_ )

//#define LOCK_THREAD_GUARD_2	(ACE_Guard< MUTEX > OBJ(m_rMutex); OBJ.locked ();)

//--------------------------------------------------------------------
// 控制
//--------------------------------------------------------------------
typedef struct _SCtlCfgInfo
{
	// 控制台窗口标题，若不指定则缺省为"ProbeSysMonitor"
	std::string strConsoleTitle;

	// 定时器轮询间隔时间（单位：秒）
	unsigned int uTimerInterval;

	// 定时显示本系统运行状态间隔时间（单位：秒）
	unsigned int uShowInterval;

	// 定时重启操作系统间隔时间（单位：秒）
	unsigned int uRebootOsInterval;

	// 定时重启进程间隔时间（单位：秒）
	unsigned int uRestartProcInterval;

	// 是否自动注册到操作系统，以使操作系统重启后能够自动启动ProbeSysMonitor
	bool bAutoRegist;

	// 是否在告警时主动发送Trap
	bool bSendTrap;
	
	//------------------------------------------------------------
	// 1: 重启进程；2：关闭进程；3：重启操作系统；4：关闭操作系统；
	//------------------------------------------------------------
	// CPU利用率超门限告警处理方法
	unsigned int uCpuUseAlarmHandle_default;
	std::string strCpuUseAlarmHandle_custom;

	// 内存利用率超门限告警处理方法
	unsigned int uMemUseAlarmHandle_default;
	std::string strMemUseAlarmHandle_custom;

	// 进程占用CPU利用率超门限告警处理方法
	unsigned int uProcCpuUseAlarmHandle_default;
	std::string strProcCpuUseAlarmHandle_custom;

	// 进程占用内存利用率超门限告警处理方法
	unsigned int uProcMemUseAlarmHandle_default;
	std::string strProcMemUseAlarmHandle_custom;

} SCtlCfgInfo;

//--------------------------------------------------------------------
// 日志
//--------------------------------------------------------------------
// 日志配置信息结构
typedef struct _SLogCfgInfo
{
	bool bIsLogToConsole;				// 日志是否输出到控制台
	unsigned int uLogInterval;			// 日志记录时间间隔（单位：秒）
	unsigned int uChangeLogFileInterval;// 定时改变日志输出到新的文件的时间间隔（单位：秒）
	std::string strFilePath;			// 日志文件存放路径

} SLogCfgInfo;

//--------------------------------------------------------------------
// CPU
//--------------------------------------------------------------------
// CPU配置信息结构
typedef struct _SCpuCfgInfo
{
	int nUsePercentLimit;			// 利用率门限，值：100*百分比，-1表示不监控该指标
	int nUseAlarmDuration;			// 持续超过利用率门限的时间门限，单位：秒，-1表示不监控该指标

} SCpuCfgInfo;

//--------------------------------------------------------------------
// 内存
//--------------------------------------------------------------------
// 内存配置信息结构
typedef struct _SMemCfgInfo
{
	int nUsePercentLimit;			// 利用率门限，值：100*百分比，-1表示不监控该指标
	int nUseAlarmDuration;			// 持续超过利用率门限的时间门限，单位：秒，-1表示不监控该指标

} SMemCfgInfo;

//--------------------------------------------------------------------
// 磁盘
//--------------------------------------------------------------------
// 磁盘配置信息结构
typedef struct _SDiskCfgInfo
{
	//下标为告警低级别.参看cpf::CAlarmLevelMgr2的头文件
	//某一级为-1表示该级别不用告警.
	int aUsePercentLimit_level[5*2];			// 5个级别-利用率门限，值：100*百分比，-1表示不监控该指标

} SDiskCfgInfo;

//--------------------------------------------------------------------
// 文件系统
//--------------------------------------------------------------------
// 被监控文件系统一般信息结构
typedef struct _SFileSysComInfo
{
	std::string strExeFile;			// 文件系统全路径

} SFileSysComInfo;

typedef std::vector< SFileSysComInfo > VectFileSysComInfo;

// 文件系统配置信息结构
typedef struct _SFileSysCfgInfo
{
	int nUsePercentLimit;			// 利用率门限，值：100*百分比，-1表示不监控该指标

	VectFileSysComInfo vectComInfo;	// 被监控进程一般信息列表

} SFileSysCfgInfo;

//--------------------------------------------------------------------
// 板卡
//--------------------------------------------------------------------
// 板卡配置信息结构
typedef struct _SCardCfgInfo
{

} SCardCfgInfo;

//--------------------------------------------------------------------
// 网络
//--------------------------------------------------------------------
// 网络配置信息结构
typedef struct _SNetCfgInfo
{
	int nUsePercentLimit;			// 利用率门限，值：100*百分比，-1表示不监控该指标
	int nUseAlarmDuration;			// 持续超过利用率门限的时间门限，单位：秒，-1表示不监控该指标

} SNetCfgInfo;

//--------------------------------------------------------------------
// 进程
//--------------------------------------------------------------------
// 被监控进程一般信息结构
typedef struct _SProcComInfo
{
	std::string strExeFile;			// 进程可执行文件全路径名称

} SProcComInfo;

typedef std::vector< SProcComInfo > VectProcComInfo;

// 进程配置信息结构
typedef struct _SProcCfgInfo
{
	int nCpuUsePercentLimit;		// CPU利用率门限，值：100*百分比，-1表示不监控该指标
	int nCpuUseAlarmDuration;		// CPU持续超过利用率门限的时间门限，单位：秒，-1表示不监控该指标

	int nMEMUseFlag;				// 进程占用内存利用率超门限告警，是以物理内存还是虚拟内存计算选项（1:物理内存，2:虚拟内存），缺省为物理内存
	int nMemUsePercentLimit;		// 内存利用率门限，值：100*百分比，-1表示不监控该指标
	int nMemUseAlarmDuration;		// 内存持续超过利用率门限的时间门限，单位：秒，-1表示不监控该指标

	int nRestartInterval;			// 进程两次重启时间最小间隔
	int nMaxRestartTimes;			// 进程最多重启次数

	VectProcComInfo vectComInfo;	// 被监控进程一般信息列表

} SProcCfgInfo;


//--------------------------------------------------------------------
// SNMP
//--------------------------------------------------------------------
// SNMP配置信息结构
typedef struct _SSnmpCfgInfo
{
	bool bOpenSvr;					// 是否开启SNMP服务
	unsigned short uSnmpPort;		// SNMP服务端口号
	
	CStr strMibName;				// Mib库名称
	CStr strTarpSource;				// Trap 来源标识
	CStr strTrapAddr;				// Trap 目的地址
	long lTrapInterval;             // Trap 轮询时间间隔,缺省为1秒.

} SSnmpCfgInfo;


//--------------------------------------------------------------------
// 全局函数
//--------------------------------------------------------------------
// 记载ASSERT错误发生的文件名和行号，在release版本中也起作用
#define ASSERT_INFO ACE_ERROR( ( LM_ERROR, "ASSERT错误[%N:%l]！！！\n" ) )

/******************************************************************************
 * FUNCTION    : GetStringTime(IN const ACE_Time_Value &rTime,
 *               ACE_TCHAR *strTime)
 *
 * DESCRIPTION :
 *       -- 将时间对象rTime格式化为一个字符串
 *
 * PARAMETER   :
 *       1: rTime -- IN, 时间对象
 *       2: strTime -- 字符串
 *
 * RETURN     :
 *       inline ACE_TCHAR* -- 字符串
 *
 * SPECIAL    :
 *
 *****************************************************************************/
inline ACE_TCHAR* GetStringTime( IN const ACE_Time_Value &rTime, OUT ACE_TCHAR *strTime )
{
	time_t nTime = rTime.sec();
	ACE_TCHAR *p = ACE_OS::ctime( &nTime );
	ACE_ASSERT( p );
	if ( NULL == p )
	{
		return NULL;
	}

	size_t strLen = ACE_OS::strlen( p );

	ACE_OS::strncpy( strTime, p, strLen );
	strTime[ strLen - 1 ] = 0;

	return strTime;
}

/******************************************************************************
 * FUNCTION    : GetCommandString(IN unsigned int uCmd)
 *       
 * DESCRIPTION : 
 *       -- 根据告警处理方式枚举返回相应文本
 *       
 * PARAMETER   : 
 *       1: uCmd -- IN, 告警处理方式枚举
 *       
 * RETURN     : 
 *       inline ACE_TCHAR* -- 相应文本
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
inline ACE_TCHAR* GetCommandString( IN unsigned int uCmd )
{
	switch ( uCmd )
	{
		case E_HANDLE_RESTART_PROCESS :			// 重启进程
			return ACE_TEXT( "重启进程" );
			break;

		case E_HANDLE_TERMINATE_PROCESS :		// 关闭进程
			return ACE_TEXT( "关闭进程" );
			break;

		case E_HANDLE_REBOOT_OS :				// 重启操作系统
			return ACE_TEXT( "重启操作系统" );
			break;

		case E_HANDLE_CLOSE_OS :				// 关闭操作系统
			return ACE_TEXT( "关闭操作系统" );
			break;

		case E_HANDLE_EXEUTE_FILE :				// 执行配置文件中指定的程序
		default :
			ASSERT_INFO;
			ACE_ASSERT( 0 );
			break;
	}

	return NULL;
}

/******************************************************************************
 * FUNCTION    : WaitToExit()
 *       
 * DESCRIPTION : 
 *       -- 在程序终止前等待用户确认
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
inline void WaitToExit()
{
#ifdef OS_WINDOWS
	printf( "press any key to exit !!" );
	HANDLE hConsol = GetStdHandle(STD_INPUT_HANDLE);

	if ( hConsol == NULL )
	{
		return;
	}
	
	FlushConsoleInputBuffer( hConsol );

 	char ch = getchar();
#endif
}


#endif // __PROBE_SYS_COMMON_H__

