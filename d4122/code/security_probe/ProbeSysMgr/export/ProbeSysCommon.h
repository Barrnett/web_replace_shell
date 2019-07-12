/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProbeSysCommon.h
 *
 *  TITLE      :  Probeϵͳ���ݽṹ����������
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
// ����
******************************************************************************/
// Probe�澯����
typedef enum
{
	E_ALARM_CPU_USEPECENT,			// ProbeϵͳCPU�����ʳ������޸澯
	E_ALARM_CPU_TEMPERATURE,		// ProbeϵͳCPU�¶ȳ������޸澯

	E_ALARM_MEM_USEPECENT,			// Probeϵͳ�ڴ������ʳ������޸澯

	E_ALARM_DISK_USEPECENT,			// Probeϵͳ���������ʳ������޸澯

	E_ALARM_FILESYS_USEPECENT,		// Probeϵͳ�ļ�ϵͳ�����ʳ������޸澯

	E_ALARM_NET_USEPECENT,			// Probeϵͳ���������ʳ������޸澯

	E_ALARM_PROCESS_CPU_USEPECENT,	// ����ռ��CPU�����ʳ������޸澯
	E_ALARM_PROCESS_MEM_USEPECENT	// ����ռ��CPU�����ʳ������޸澯

} EProbeSysAlarmType;

// �澯����ʽ����
typedef enum
{
	E_HANDLE_EXEUTE_FILE,			// ִ�������ļ���ָ���ĳ���

	E_HANDLE_RESTART_PROCESS,		// ��������
	E_HANDLE_TERMINATE_PROCESS,		// �رս���

	E_HANDLE_REBOOT_OS,				// ��������ϵͳ
	E_HANDLE_CLOSE_OS               // �رղ���ϵͳ

} EProbeSysAlarmHandleType;

// ȱʡ����--------------------------------------------------------

// ȱʡ������Чֵ
#define DEFAULT_INVALID_VALUE					0

// �ڴ滻�����
#define MEM_SIZE_DIV							1024

// �ļ�����-------------------------
// �����ļ�
#define DEFAULT_PROBESYS_CFGFILE_NAME			"ProbeSysMonitor.ini"

// ��־�ļ�
#define PROBESYSMONITOR_INIT_LOGFILE			"ProbeSysMonitorInit.log"				// ��¼��ʼ������
#define DEFAULT_PROBESYS_LOGFILE_NAME			"ProbeSysMonitor_%04d%02d%02d%02d.log"	// ��¼����ʱ��־

// ����-----------------------------

// ����ռ���ڴ������ʳ����޸澯�����������ڴ滹�������ڴ����ȱʡѡ��
#define DEFAULT_PROC_MEM_PHY_FLAG				1		// �����ڴ�
#define DEFAULT_PROC_MEM_VIR_FLAG				2		// �����ڴ�


// �����ļ����---------------------------------------------------

// �����ļ������ڲ���ϵͳ������ɵ��ļ�·��д������
#define OS_WINDOWS_ID							"WINDOWS_"
#define OS_LINUX_ID								"LINUX_"

// ����-----------------------------
#define SECTION_CONTROL							"CONTROL"

// ����̨���ڱ��⣬����ָ����ȱʡΪ"ProbeSysMonitor"
#define ITEM_CTL_CONSOLE_TITLE					"ConsoleTitle"
// ȱʡ����̨���ڱ���
#define DEFAULT_CONSOLE_TITLE					"ProbeSysMonitor"

// ��ʱ��ѯ���ʱ�䣨��λ���룩
#define ITEM_CTL_TIMER_INTERVAL					"TimerInterval"
// ȱʡ��ʱ��ѯʱ��������λ���룩
#define DEFAULT_TIMER_INTERVAL					5


// ��ʱ��ʾ��ϵͳ����״̬������ʱ����TimerInterval�ı���
#define ITEM_CTL_SHOW_TIMES						"ShowTimes"
// ȱʡ��ʱ��ʾ��ϵͳ����״̬���ʱ����TimerInterval�ı���
#define DEFAULT_SHOW_TIMES						2


// ��ʱ��������ϵͳ���ʱ�䣨��λ��Сʱ����Ϊ0��ʾ����ʱ����
#define ITEM_CTL_REBOOT_OS_INTERVAL				"RebootOsInterval"
// ȱʡ��ʱ��������ϵͳ���ʱ�䣨��λ��Сʱ��
#define DEFAULT_REBOOT_OS_INTERVAL				0


// ��ʱ��������ϵͳ���ʱ�䣨��λ��Сʱ����Ϊ0��ʾ����ʱ����
#define ITEM_CTL_RESTART_RPOC_INTERVAL			"ReStartProcInterval"

// �Ƿ��Զ�ע�ᵽ����ϵͳ����ʹ����ϵͳ�������ܹ��Զ�����ProbeSysMonitor
#define ITEM_CTL_AUTO_REGIST					"AutoRegist"
// ȱʡ�Ƿ��Զ�ע�ᵽ����ϵͳ
#define DEFAULT_AUTO_REGIST						true


// �Ƿ��ڸ澯ʱ��������Trap
#define ITEM_CTL_SEND_TRAP						"SendTrap"
// ȱʡ�Ƿ��ڸ澯ʱ��������Trap
#define DEFAULT_SEND_TRAP						true



// CPU�����ʳ����޸澯������
#define ITEM_CTL_CPUUSEALARMHANDLE_DEFAULT		"CpuUseAlarmHandle_default"
#define ITEM_CTL_CPUUSEALARMHANDLE_CUSTOM		"CpuUseAlarmHandle_custom"

// �ڴ������ʳ����޸澯������
#define ITEM_CTL_MEMUSEALARMHANDLE_DEFAULT		"MemUseAlarmHandle_default"
#define ITEM_CTL_MEMUSEALARMHANDLE_CUSTOM		"MemUseAlarmHandle_custom"

// ����ռ��CPU�����ʳ����޸澯������
#define ITEM_CTL_PROCCPUUSEALARMHANDLE_DEFAULT	"ProcCpuUseAlarmHandle_default"
#define ITEM_CTL_PROCCPUUSEALARMHANDLE_CUSTOM	"ProcCpuUseAlarmHandle_custom"

// ����ռ���ڴ������ʳ����޸澯������
#define ITEM_CTL_PROCMEMUSEALARMHANDLE_DEFAULT	"ProcMemUseAlarmHandle_default"
#define ITEM_CTL_PROCMEMUSEALARMHANDLE_CUSTOM	"ProcMemUseAlarmHandle_custom"

// ��־-----------------------------
#define SECTION_LOG								"LOG"

// ��ʱ��־ʱ������TimerInterval�ı���
#define ITEM_LOG_TIMES							"LogTimes"
// ȱʡ��־ʱ������TimerInterval�ı���
#define DEFAULT_LOG_TIMES						10

// ��ʱ�ı���־������µ��ļ���ʱ����
#define ITEM_CHANGE_LOGFILE_INTERVAL			"ChangeLogFileInterval"
// ȱʡ�ı���־������µ��ļ���ʱ��������λ��Сʱ��
#define DEFAULT_CHANGE_LOGFILE_INTERVAL			24


// �Ƿ�ͬʱ���������̨
#define ITEM_LOG_PUTOUT_TO_CONSOLE				"PutOutToConsole"
// ȱʡ�Ƿ�ͬʱ���������̨
#define DEFAULT_LOG_PUTOUT_TO_CONSOLE			false

// ��־�ļ�����
#define ITEM_LOG_FILE_PATH						"LogFilePath"
// ȱʡ��־�ļ�����
#define DEFAULT_PROBESYS_LOGFILE_PATH			"LogDir"


// CPU-----------------------------
#define SECTION_CPU								"CPU"

// ����������
#define ITEM_CPU_USEPERCENTLIMIT				"UsePercent"
// �����������������޵�ʱ������
#define ITEM_CPU_USEALARMDURATION				"UseAlarmDuration"

// �¶�����
#define ITEM_CPU_TEMPERATURELIMIT				"TemperatureLimit"
// ���������¶����޵�ʱ������
#define ITEM_CPU_TEMPALARMDURATION				"TempAlarmDuration"

// �ڴ�-----------------------------
#define SECTION_MEM								"MEM"

// ����������
#define ITEM_MEM_USEPERCENTLIMIT				"UsePercent"
// �����������������޵�ʱ������
#define ITEM_MEM_USEALARMDURATION				"UseAlarmDuration"

// ����-----------------------------
#define SECTION_PROC							"PROC"

// CPU����������
#define ITEM_PROC_CPU_USEPERCENTLIMIT			"CPUUsePercent"
// CPU�����������������޵�ʱ������
#define ITEM_PROC_CPU_USEALARMDURATION			"CPUUseAlarmDuration"

// ����ռ���ڴ������ʳ����޸澯�����������ڴ滹�������ڴ����ѡ��
#define ITEM_PROC_MEM_MEMUSEFLAG				"MEMUseFlag"
// �ڴ�����������
#define ITEM_PROC_MEM_USEPERCENTLIMIT			"MEMUsePercent"
// �ڴ�����������������޵�ʱ������
#define ITEM_PROC_MEM_USEALARMDURATION			"MEMUseAlarmDuration"

// ������������ʱ����С���
#define ITEM_PROC_RESTARTINTERVAL				"RestartInterval"
// ���������������
#define ITEM_PROC_MAXRESTARTTIMES				"MaxRestartTimes"

// ����صĽ��̵�ȫ·������
#define ITEM_PROC_FULLPATH_EXEFILE				"ExeFullPath"


// SNMP-----------------------------
#define SECTION_SNMP							"SNMP"

// �Ƿ���SNMP����0��ʾ��������1��ʾ��������δָ����ȱʡΪ������
#define ITEM_SNMP_OPEN_SNMP						"OpenSnmpServer"
// ȱʡ�Ƿ���SNMP����
#define DEFAULT_OPEN_SNMP						false

// SNMP����˿ں�
#define ITEM_SNMP_PORT							"SnmpPort"
// ȱʡSNMP����˿ں�
#define DEFAULT_SNMP_PORT						16100

// zhugf add 2005.12.6:
// ȱʡ��Mib�����ƣ�Ĭ��Ϊ��������·���µ�ProbeSysRmon
const char ITEM_SYSMONITOR_MIB_NAME[] = 	    "MibLib";
const char DEFAULT_SYSMONITOR_MIB_NAME[] =      "ProbeSysRmon";

// ȱʡTrap source TrapSource
const char ITEM_SYS_TRAP_SOURCE[] = 	       	"TrapSource";
const char DEFAULT_SYS_TRAP_SOURCE[] =          "ProbeSysMonitor";

// ȱʡTrap Ŀ�ĵ�ַ DefaultTrapAddr
const char ITEM_SYS_TRAP_DADDRESS[] = 	       	"DefaultTrapAddr";
const char DEFAULT_SYS_TRAP_DADDRESS[] =        "127.0.0.1/162";

// Trap ��ѯʱ����,��λΪ��
const char ITEM_SYS_TRAP_INTERVAL[] = 			"TrapInterval";
const int DEFAULT_SYS_TRAP_INTERVAL =           1;

// ���߳�ѭ������ʱ��,��λΪ΢��
const long L_MAIN_TREAD_LOOP_SLEEP_US =         100000L;
/****************************************************************************
// ȫ�ֱ���
*****************************************************************************/
// WINDOWSϵͳ��Ϣ��ȡ����ָ��
#ifdef OS_WINDOWS
	//���Ͷ���
	typedef LONG ( WINAPI *PROCNTQSI )( UINT, PVOID, ULONG, PULONG );
	extern PROCNTQSI NtQuerySystemInformation;
#endif

/****************************************************************************
// ���ݽṹ
*****************************************************************************/
// �̰߳�ȫ������������
typedef ACE_Atomic_Op< ACE_Thread_Mutex, int > SafeInt;

// �̰߳�ȫ��������
typedef ACE_Thread_Mutex MUTEX;

#define LOCK_THREAD_GUARD ACE_GUARD( MUTEX, _MON_, ( MUTEX& )m_rMutex )

#define LOCK_THREAD_GUARD_RETURN( _RETURN_ ) ACE_GUARD_RETURN( MUTEX, _MON_, ( MUTEX& )m_rMutex, _RETURN_ )

//#define LOCK_THREAD_GUARD_2	(ACE_Guard< MUTEX > OBJ(m_rMutex); OBJ.locked ();)

//--------------------------------------------------------------------
// ����
//--------------------------------------------------------------------
typedef struct _SCtlCfgInfo
{
	// ����̨���ڱ��⣬����ָ����ȱʡΪ"ProbeSysMonitor"
	std::string strConsoleTitle;

	// ��ʱ����ѯ���ʱ�䣨��λ���룩
	unsigned int uTimerInterval;

	// ��ʱ��ʾ��ϵͳ����״̬���ʱ�䣨��λ���룩
	unsigned int uShowInterval;

	// ��ʱ��������ϵͳ���ʱ�䣨��λ���룩
	unsigned int uRebootOsInterval;

	// ��ʱ�������̼��ʱ�䣨��λ���룩
	unsigned int uRestartProcInterval;

	// �Ƿ��Զ�ע�ᵽ����ϵͳ����ʹ����ϵͳ�������ܹ��Զ�����ProbeSysMonitor
	bool bAutoRegist;

	// �Ƿ��ڸ澯ʱ��������Trap
	bool bSendTrap;
	
	//------------------------------------------------------------
	// 1: �������̣�2���رս��̣�3����������ϵͳ��4���رղ���ϵͳ��
	//------------------------------------------------------------
	// CPU�����ʳ����޸澯������
	unsigned int uCpuUseAlarmHandle_default;
	std::string strCpuUseAlarmHandle_custom;

	// �ڴ������ʳ����޸澯������
	unsigned int uMemUseAlarmHandle_default;
	std::string strMemUseAlarmHandle_custom;

	// ����ռ��CPU�����ʳ����޸澯������
	unsigned int uProcCpuUseAlarmHandle_default;
	std::string strProcCpuUseAlarmHandle_custom;

	// ����ռ���ڴ������ʳ����޸澯������
	unsigned int uProcMemUseAlarmHandle_default;
	std::string strProcMemUseAlarmHandle_custom;

} SCtlCfgInfo;

//--------------------------------------------------------------------
// ��־
//--------------------------------------------------------------------
// ��־������Ϣ�ṹ
typedef struct _SLogCfgInfo
{
	bool bIsLogToConsole;				// ��־�Ƿ����������̨
	unsigned int uLogInterval;			// ��־��¼ʱ��������λ���룩
	unsigned int uChangeLogFileInterval;// ��ʱ�ı���־������µ��ļ���ʱ��������λ���룩
	std::string strFilePath;			// ��־�ļ����·��

} SLogCfgInfo;

//--------------------------------------------------------------------
// CPU
//--------------------------------------------------------------------
// CPU������Ϣ�ṹ
typedef struct _SCpuCfgInfo
{
	int nUsePercentLimit;			// ���������ޣ�ֵ��100*�ٷֱȣ�-1��ʾ����ظ�ָ��
	int nUseAlarmDuration;			// �����������������޵�ʱ�����ޣ���λ���룬-1��ʾ����ظ�ָ��

} SCpuCfgInfo;

//--------------------------------------------------------------------
// �ڴ�
//--------------------------------------------------------------------
// �ڴ�������Ϣ�ṹ
typedef struct _SMemCfgInfo
{
	int nUsePercentLimit;			// ���������ޣ�ֵ��100*�ٷֱȣ�-1��ʾ����ظ�ָ��
	int nUseAlarmDuration;			// �����������������޵�ʱ�����ޣ���λ���룬-1��ʾ����ظ�ָ��

} SMemCfgInfo;

//--------------------------------------------------------------------
// ����
//--------------------------------------------------------------------
// ����������Ϣ�ṹ
typedef struct _SDiskCfgInfo
{
	//�±�Ϊ�澯�ͼ���.�ο�cpf::CAlarmLevelMgr2��ͷ�ļ�
	//ĳһ��Ϊ-1��ʾ�ü����ø澯.
	int aUsePercentLimit_level[5*2];			// 5������-���������ޣ�ֵ��100*�ٷֱȣ�-1��ʾ����ظ�ָ��

} SDiskCfgInfo;

//--------------------------------------------------------------------
// �ļ�ϵͳ
//--------------------------------------------------------------------
// ������ļ�ϵͳһ����Ϣ�ṹ
typedef struct _SFileSysComInfo
{
	std::string strExeFile;			// �ļ�ϵͳȫ·��

} SFileSysComInfo;

typedef std::vector< SFileSysComInfo > VectFileSysComInfo;

// �ļ�ϵͳ������Ϣ�ṹ
typedef struct _SFileSysCfgInfo
{
	int nUsePercentLimit;			// ���������ޣ�ֵ��100*�ٷֱȣ�-1��ʾ����ظ�ָ��

	VectFileSysComInfo vectComInfo;	// ����ؽ���һ����Ϣ�б�

} SFileSysCfgInfo;

//--------------------------------------------------------------------
// �忨
//--------------------------------------------------------------------
// �忨������Ϣ�ṹ
typedef struct _SCardCfgInfo
{

} SCardCfgInfo;

//--------------------------------------------------------------------
// ����
//--------------------------------------------------------------------
// ����������Ϣ�ṹ
typedef struct _SNetCfgInfo
{
	int nUsePercentLimit;			// ���������ޣ�ֵ��100*�ٷֱȣ�-1��ʾ����ظ�ָ��
	int nUseAlarmDuration;			// �����������������޵�ʱ�����ޣ���λ���룬-1��ʾ����ظ�ָ��

} SNetCfgInfo;

//--------------------------------------------------------------------
// ����
//--------------------------------------------------------------------
// ����ؽ���һ����Ϣ�ṹ
typedef struct _SProcComInfo
{
	std::string strExeFile;			// ���̿�ִ���ļ�ȫ·������

} SProcComInfo;

typedef std::vector< SProcComInfo > VectProcComInfo;

// ����������Ϣ�ṹ
typedef struct _SProcCfgInfo
{
	int nCpuUsePercentLimit;		// CPU���������ޣ�ֵ��100*�ٷֱȣ�-1��ʾ����ظ�ָ��
	int nCpuUseAlarmDuration;		// CPU�����������������޵�ʱ�����ޣ���λ���룬-1��ʾ����ظ�ָ��

	int nMEMUseFlag;				// ����ռ���ڴ������ʳ����޸澯�����������ڴ滹�������ڴ����ѡ�1:�����ڴ棬2:�����ڴ棩��ȱʡΪ�����ڴ�
	int nMemUsePercentLimit;		// �ڴ����������ޣ�ֵ��100*�ٷֱȣ�-1��ʾ����ظ�ָ��
	int nMemUseAlarmDuration;		// �ڴ�����������������޵�ʱ�����ޣ���λ���룬-1��ʾ����ظ�ָ��

	int nRestartInterval;			// ������������ʱ����С���
	int nMaxRestartTimes;			// ���������������

	VectProcComInfo vectComInfo;	// ����ؽ���һ����Ϣ�б�

} SProcCfgInfo;


//--------------------------------------------------------------------
// SNMP
//--------------------------------------------------------------------
// SNMP������Ϣ�ṹ
typedef struct _SSnmpCfgInfo
{
	bool bOpenSvr;					// �Ƿ���SNMP����
	unsigned short uSnmpPort;		// SNMP����˿ں�
	
	CStr strMibName;				// Mib������
	CStr strTarpSource;				// Trap ��Դ��ʶ
	CStr strTrapAddr;				// Trap Ŀ�ĵ�ַ
	long lTrapInterval;             // Trap ��ѯʱ����,ȱʡΪ1��.

} SSnmpCfgInfo;


//--------------------------------------------------------------------
// ȫ�ֺ���
//--------------------------------------------------------------------
// ����ASSERT���������ļ������кţ���release�汾��Ҳ������
#define ASSERT_INFO ACE_ERROR( ( LM_ERROR, "ASSERT����[%N:%l]������\n" ) )

/******************************************************************************
 * FUNCTION    : GetStringTime(IN const ACE_Time_Value &rTime,
 *               ACE_TCHAR *strTime)
 *
 * DESCRIPTION :
 *       -- ��ʱ�����rTime��ʽ��Ϊһ���ַ���
 *
 * PARAMETER   :
 *       1: rTime -- IN, ʱ�����
 *       2: strTime -- �ַ���
 *
 * RETURN     :
 *       inline ACE_TCHAR* -- �ַ���
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
 *       -- ���ݸ澯����ʽö�ٷ�����Ӧ�ı�
 *       
 * PARAMETER   : 
 *       1: uCmd -- IN, �澯����ʽö��
 *       
 * RETURN     : 
 *       inline ACE_TCHAR* -- ��Ӧ�ı�
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
inline ACE_TCHAR* GetCommandString( IN unsigned int uCmd )
{
	switch ( uCmd )
	{
		case E_HANDLE_RESTART_PROCESS :			// ��������
			return ACE_TEXT( "��������" );
			break;

		case E_HANDLE_TERMINATE_PROCESS :		// �رս���
			return ACE_TEXT( "�رս���" );
			break;

		case E_HANDLE_REBOOT_OS :				// ��������ϵͳ
			return ACE_TEXT( "��������ϵͳ" );
			break;

		case E_HANDLE_CLOSE_OS :				// �رղ���ϵͳ
			return ACE_TEXT( "�رղ���ϵͳ" );
			break;

		case E_HANDLE_EXEUTE_FILE :				// ִ�������ļ���ָ���ĳ���
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
 *       -- �ڳ�����ֹǰ�ȴ��û�ȷ��
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

