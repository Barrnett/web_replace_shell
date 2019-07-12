/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ConfigFileMgr.cpp
 *
 *  TITLE      :  Probe系统监控维护系统配置文件管理类实现
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:
 *
 *****************************************************************************/

#include "LogFileMgr.h"
#include "cpf/cpf.h"
#include "cpf/IniFile.h"
#include "cpf/path_tools.h"

#include "ProbeSysCommon.h"
#include "FileDirInfo.h"

#include "ConfigFileMgr.h"



/********************************************************************
Public Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::CConfigFileMgr()
 *       
 * DESCRIPTION : 
 *       -- 构造函数
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CConfigFileMgr::CConfigFileMgr()
 : 	m_pSnmpCfgInfo( NULL ), m_pCfgFile( NULL ), m_pLogCfgInfo( NULL ), 
	m_pCpuCfgInfo( NULL ), m_pMemCfgInfo( NULL ), m_pCtlCfgInfo( NULL ),
	m_pDiskCfgInfo( NULL ), m_pFileSysCfgInfo( NULL ), m_pCardCfgInfo( NULL ),
	m_pNetCfgInfo( NULL ), m_pProcCfgInfo( NULL )
{

}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::~CConfigFileMgr()
 *       
 * DESCRIPTION : 
 *       -- 析构函数
 *       
 * PARAMETER   : 
 *       None
 *       
 * RETURN     : 
 *        -- 
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
CConfigFileMgr::~CConfigFileMgr()
{
	// 配置文件访问对象
	if ( NULL != m_pCfgFile )
	{
		delete m_pCfgFile;
		m_pCfgFile = NULL;
	}

	// 销毁CPU配置信息对象
	if ( NULL != m_pCpuCfgInfo )
	{
		delete m_pCpuCfgInfo;
		m_pCpuCfgInfo = NULL;
	}

	// 销毁内存配置信息对象
	if ( NULL != m_pMemCfgInfo )
	{
		delete m_pMemCfgInfo;
		m_pMemCfgInfo = NULL;
	}

	// 销毁磁盘配置信息对象
	if ( NULL != m_pDiskCfgInfo )
	{
		delete m_pDiskCfgInfo;
		m_pDiskCfgInfo = NULL;
	}

	// 销毁文件系统配置信息对象
	if ( NULL != m_pFileSysCfgInfo )
	{
		delete m_pFileSysCfgInfo;
		m_pFileSysCfgInfo = NULL;
	}

	// 销毁板卡配置信息对象
	if ( NULL != m_pCardCfgInfo )
	{
		delete m_pCardCfgInfo;
		m_pCardCfgInfo = NULL;
	}

	// 销毁网络配置信息对象
	if ( NULL != m_pNetCfgInfo )
	{
		delete m_pNetCfgInfo;
		m_pNetCfgInfo = NULL;
	}

	// 销毁进程配置信息对象
	if ( NULL != m_pProcCfgInfo )
	{
		delete m_pProcCfgInfo;
		m_pProcCfgInfo = NULL;
	}

	if ( NULL != m_pSnmpCfgInfo )
	{
		delete m_pSnmpCfgInfo;
		m_pSnmpCfgInfo = NULL;
	}

	if ( NULL != m_pCtlCfgInfo )
	{
		delete m_pCtlCfgInfo;
		m_pCtlCfgInfo = NULL;
	}

	
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::Init(IN const std::string &strCfgFileName)
 *       
 * DESCRIPTION : 
 *       -- 初始化
 *       
 * PARAMETER   : 
 *       1: strCfgFileName -- IN, 配置文件全路径名称
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CConfigFileMgr::Init( IN const std::string &strCfgFileName )
{
	// 配置文件访问对象
	char strPath[MAX_PATH];
	if ( !CPF::GetModulePathName( strPath, sizeof( strPath ) ) )
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取配置文件路径失败！！！\n" ) ) );
		ACE_ASSERT( 0 );
		return false;
	}
	
	std::string strFullPath( strPath );
	strFullPath += FILENAME_SEPERATOR_STR;
	strFullPath += "ini";
	if ( !CFileDirInfo::IsFileExist( strFullPath, strCfgFileName ) )
	{
		ACE_ERROR( (	LM_ERROR, 
						ACE_TEXT( "\n文件路径[%s]下\n不存在配置文件[%s]，\n无法获取配置信息，将全部使用缺省配置！\n\n" ), 
						ACE_TEXT( strFullPath.c_str() ),
						ACE_TEXT( strCfgFileName.c_str() ) ) );
	}
	else
	{
		strFullPath += FILENAME_SEPERATOR_STR;
		strFullPath += strCfgFileName;
	}

	m_strFileName = strFullPath;
	m_pCfgFile = new CIniFile;
	ACE_ASSERT( m_pCfgFile );
	if ( NULL == m_pCfgFile )
	{
		ASSERT_INFO;
		return false;
	}
	m_pCfgFile->Open(m_strFileName.c_str());

	//ACE_DEBUG( ( LM_INFO, ACE_TEXT( "%I配置文件名称[%s]\n" ), ACE_TEXT( m_pCfgFile->GetFileName() ) ) );
	
	
	// 获取SNMP配置信息
	if ( !getSnmpCfgInfo() )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取SNMP配置信息失败！！！\n" ) ) );
	}

	// 获取控制配置信息
	if ( !getCtlCfgInfo() )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取控制配置信息失败！！！\n" ) ) );
	}

	// 获取日志文件名称
	if ( !getLogCfgInfo() )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取日志配置信息失败！！！\n" ) ) );
	}


	// 获取CPU配置信息
	if ( !getCpuCfgInfo() )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取CPU配置信息失败！！！\n" ) ) );
	}
	
	// 获取内存配置信息
	if ( !getMemCfgInfo() )
	{
		ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取内存配置信息失败！！！\n" ) ) );
	}

	// 获取进程配置信息
	if ( !getProcCfgInfo() )
	{
		//ACE_ASSERT( 0 );
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "%I获取进程配置信息失败！！！\n" ) ) );
	}

	return true;
}


/********************************************************************
Private Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getSnmpCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- 获取SNMP配置信息
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
bool CConfigFileMgr::getSnmpCfgInfo()
{
	m_pSnmpCfgInfo = new SSnmpCfgInfo;
	ACE_ASSERT( m_pSnmpCfgInfo );
	if ( NULL == m_pSnmpCfgInfo )
	{
		return false;
	}
	
	// 是否开启SNMP服务
	unsigned int nOpenSvr = 0;
	if ( !m_pCfgFile->GetValue( SECTION_SNMP, 
								ITEM_SNMP_OPEN_SNMP, 
								nOpenSvr ) )
	{
		m_pSnmpCfgInfo->bOpenSvr = DEFAULT_OPEN_SNMP;
	}
	else
	{
		m_pSnmpCfgInfo->bOpenSvr = ( 0 == nOpenSvr ) ? false : true;
	}

	// 定时轮询间隔时间（单位：秒）
	unsigned int uPort = 0;
	if ( !m_pCfgFile->GetValue( SECTION_SNMP, 
								ITEM_SNMP_PORT, 
								uPort )
		|| uPort < 1 )
	{
		uPort = DEFAULT_SNMP_PORT;
	}
	m_pSnmpCfgInfo->uSnmpPort = ( unsigned short )uPort;
	
	// Mib库名称
	{
		CStr strKey(ITEM_SYSMONITOR_MIB_NAME);
		
		if ( !m_pCfgFile->GetValue( SECTION_SNMP, 
									strKey, 
									m_pSnmpCfgInfo->strMibName )
			|| m_pSnmpCfgInfo->strMibName.IsEmpty() )
		{
			m_pSnmpCfgInfo->strMibName.AddString( DEFAULT_SYSMONITOR_MIB_NAME );
		}
	}
	
	// Trap 来源标识
	{
		CStr strKey(ITEM_SYS_TRAP_SOURCE);
		
		if ( !m_pCfgFile->GetValue( SECTION_SNMP, 
			strKey, 
			m_pSnmpCfgInfo->strTarpSource )
			|| m_pSnmpCfgInfo->strTarpSource.IsEmpty() )
		{
			m_pSnmpCfgInfo->strTarpSource.AddString( DEFAULT_SYS_TRAP_SOURCE );
		}
		
	}
	// Trap 目的地址
	{
		CStr strKey(ITEM_SYS_TRAP_DADDRESS);
		
		if ( !m_pCfgFile->GetValue( SECTION_SNMP, 
			strKey, 
			m_pSnmpCfgInfo->strTrapAddr )
			|| m_pSnmpCfgInfo->strTrapAddr.IsEmpty() )
		{
			m_pSnmpCfgInfo->strTrapAddr.AddString( DEFAULT_SYS_TRAP_DADDRESS );
		}
	}

	// Trap轮询间隔时间（单位：秒）
	{
		int lTrapInterval = 0;
		if ( !m_pCfgFile->GetValue( SECTION_SNMP, 
			ITEM_SYS_TRAP_INTERVAL, 
			lTrapInterval )
			|| lTrapInterval < DEFAULT_SYS_TRAP_INTERVAL )
		{
			lTrapInterval = DEFAULT_SYS_TRAP_INTERVAL;
		}
		m_pSnmpCfgInfo->lTrapInterval = lTrapInterval;
	}
	
	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getCtlCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- 获取控制配置信息
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
bool CConfigFileMgr::getCtlCfgInfo()
{
	m_pCtlCfgInfo = new SCtlCfgInfo;
	ACE_ASSERT( m_pCtlCfgInfo );
	if ( NULL == m_pCtlCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}

	CStr strConsoleTitle;
	// 控制台窗口标题，若不指定则缺省为"ProbeSysMonitor"
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_CONSOLE_TITLE, 
								strConsoleTitle )
		|| strConsoleTitle.IsEmpty() )
	{
		m_pCtlCfgInfo->strConsoleTitle = DEFAULT_CONSOLE_TITLE;
	}
	else
	{
		m_pCtlCfgInfo->strConsoleTitle = strConsoleTitle.GetString();
	}
	
	// 定时轮询间隔时间（单位：秒）
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_TIMER_INTERVAL, 
								m_pCtlCfgInfo->uTimerInterval )
		|| m_pCtlCfgInfo->uTimerInterval < 1 )
	{
		m_pCtlCfgInfo->uTimerInterval = DEFAULT_TIMER_INTERVAL;
	}
	
	// 定时轮询间隔时间（单位：秒）
	unsigned int uShowTimes = 0;
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_SHOW_TIMES, 
								uShowTimes )
		|| uShowTimes < 1 )
	{
		uShowTimes = DEFAULT_SHOW_TIMES;
	}
	m_pCtlCfgInfo->uShowInterval = m_pCtlCfgInfo->uTimerInterval * uShowTimes;
	
	// 定时重启操作系统间隔时间（单位：秒），为0表示不定时重启
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_REBOOT_OS_INTERVAL, 
								m_pCtlCfgInfo->uRebootOsInterval ) )
	{
		m_pCtlCfgInfo->uRebootOsInterval = DEFAULT_REBOOT_OS_INTERVAL;
	}
	m_pCtlCfgInfo->uRebootOsInterval *= 3600;
	
	// 定时重启进程间隔时间（单位：秒），为0表示不定时重启
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_RESTART_RPOC_INTERVAL, 
								m_pCtlCfgInfo->uRestartProcInterval ) )
	{
		m_pCtlCfgInfo->uRestartProcInterval = DEFAULT_REBOOT_OS_INTERVAL;
	}
	m_pCtlCfgInfo->uRestartProcInterval *= 3600;
	
	// 是否自动注册到操作系统
	unsigned int nAutoRegist = 0;
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_AUTO_REGIST, 
								nAutoRegist ) )
	{
		m_pCtlCfgInfo->bAutoRegist = DEFAULT_AUTO_REGIST;
	}
	else
	{
		m_pCtlCfgInfo->bAutoRegist = ( 0 == nAutoRegist ) ? false : true;
	}

	// 是否在告警时主动发送Trap
	unsigned int nSendTrap = 0;
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_SEND_TRAP, 
								nSendTrap ) )
	{
		m_pCtlCfgInfo->bSendTrap = DEFAULT_SEND_TRAP;
	}
	else
	{
		m_pCtlCfgInfo->bSendTrap = ( 0 == nSendTrap ) ? false : true;
	}

	
#ifdef OS_WINDOWS
	std::string strPrefixItem( OS_WINDOWS_ID );
#else
	std::string strPrefixItem( OS_LINUX_ID );
#endif
	std::string strItem;

	CStr strTmp;

	// CPU利用率超门限告警处理方法
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_CPUUSEALARMHANDLE_DEFAULT, 
								m_pCtlCfgInfo->uCpuUseAlarmHandle_default )
		|| E_HANDLE_REBOOT_OS > m_pCtlCfgInfo->uCpuUseAlarmHandle_default
		|| E_HANDLE_CLOSE_OS < m_pCtlCfgInfo->uCpuUseAlarmHandle_default )
	{
		m_pCtlCfgInfo->uCpuUseAlarmHandle_default = E_HANDLE_REBOOT_OS;
	}

	strItem = strPrefixItem + ITEM_CTL_CPUUSEALARMHANDLE_CUSTOM;
	if ( m_pCfgFile->GetValue(	SECTION_CONTROL, 
								strItem.c_str(), 
								strTmp ) )
	{
		strTmp.AllTrim();
		m_pCtlCfgInfo->strCpuUseAlarmHandle_custom = strTmp.GetString();
	}

	// 内存利用率超门限告警处理方法
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_MEMUSEALARMHANDLE_DEFAULT, 
								m_pCtlCfgInfo->uMemUseAlarmHandle_default )
		|| E_HANDLE_REBOOT_OS > m_pCtlCfgInfo->uMemUseAlarmHandle_default
		|| E_HANDLE_CLOSE_OS < m_pCtlCfgInfo->uMemUseAlarmHandle_default )
	{
		m_pCtlCfgInfo->uMemUseAlarmHandle_default = E_HANDLE_REBOOT_OS;
	}

	strItem = strPrefixItem + ITEM_CTL_MEMUSEALARMHANDLE_CUSTOM;
	if ( m_pCfgFile->GetValue(	SECTION_CONTROL, 
								strItem.c_str(), 
								strTmp ) )
	{
		strTmp.AllTrim();
		m_pCtlCfgInfo->strMemUseAlarmHandle_custom = strTmp.GetString();
	}

	// 进程-----------------------------------------------------------------------
	// 进程占用CPU利用率超门限告警处理方法
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_PROCCPUUSEALARMHANDLE_DEFAULT, 
								m_pCtlCfgInfo->uProcCpuUseAlarmHandle_default )
		|| E_HANDLE_RESTART_PROCESS > m_pCtlCfgInfo->uProcCpuUseAlarmHandle_default
		|| E_HANDLE_CLOSE_OS < m_pCtlCfgInfo->uProcCpuUseAlarmHandle_default )
	{
		m_pCtlCfgInfo->uProcCpuUseAlarmHandle_default = E_HANDLE_RESTART_PROCESS;
	}

	strItem = strPrefixItem + ITEM_CTL_PROCCPUUSEALARMHANDLE_CUSTOM;
	if ( m_pCfgFile->GetValue(	SECTION_CONTROL, 
								strItem.c_str(), 
								strTmp ) )
	{
		strTmp.AllTrim();
		m_pCtlCfgInfo->strProcCpuUseAlarmHandle_custom = strTmp.GetString();
	}

	// 进程占用内存利用率超门限告警处理方法
	if ( !m_pCfgFile->GetValue( SECTION_CONTROL, 
								ITEM_CTL_PROCMEMUSEALARMHANDLE_DEFAULT, 
								m_pCtlCfgInfo->uProcMemUseAlarmHandle_default )
		|| E_HANDLE_RESTART_PROCESS > m_pCtlCfgInfo->uProcMemUseAlarmHandle_default
		|| E_HANDLE_CLOSE_OS < m_pCtlCfgInfo->uProcMemUseAlarmHandle_default )
	{
		m_pCtlCfgInfo->uProcMemUseAlarmHandle_default = E_HANDLE_RESTART_PROCESS;
	}

	strItem = strPrefixItem + ITEM_CTL_PROCMEMUSEALARMHANDLE_CUSTOM;
	if ( m_pCfgFile->GetValue(	SECTION_CONTROL, 
								strItem.c_str(), 
								strTmp ) )
	{
		strTmp.AllTrim();
		m_pCtlCfgInfo->strProcMemUseAlarmHandle_custom = strTmp.GetString();
	}

	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getLogCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- 获取日志配置信息
 *       
 * PARAMETER   : 
 *       NONE
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CConfigFileMgr::getLogCfgInfo()
{
#ifdef OS_WINDOWS
	std::string strItem( OS_WINDOWS_ID );
#else
	std::string strItem( OS_LINUX_ID );
#endif
	strItem.append( ITEM_LOG_FILE_PATH );

	m_pLogCfgInfo = new SLogCfgInfo;
	ACE_ASSERT( m_pLogCfgInfo );
	if ( NULL == m_pLogCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}

	// 是否将日志信息同时输出到控制台窗口
	int nBool = 0;
	if ( !m_pCfgFile->GetValue( SECTION_LOG, ITEM_LOG_PUTOUT_TO_CONSOLE, nBool ) )
	{
		m_pLogCfgInfo->bIsLogToConsole = DEFAULT_LOG_PUTOUT_TO_CONSOLE;
	}
	else
	{
		if ( 0 == nBool )
		{
			m_pLogCfgInfo->bIsLogToConsole = false;
		}
		else
		{
			m_pLogCfgInfo->bIsLogToConsole = true;
		}
	}
	
	// 定时日志时间间隔
	unsigned int uTimes = 0;
	if ( m_pCfgFile->GetValue( SECTION_LOG, ITEM_LOG_TIMES, uTimes ) )
	{
		m_pLogCfgInfo->uLogInterval = uTimes * m_pCtlCfgInfo->uTimerInterval;
	}
	else
	{
		m_pLogCfgInfo->uLogInterval = DEFAULT_LOG_TIMES * m_pCtlCfgInfo->uTimerInterval;
	}
	
	// 定时改变日志输出到新的文件的时间间隔
	unsigned int uInterval = 0;
	if ( m_pCfgFile->GetValue( SECTION_LOG, ITEM_CHANGE_LOGFILE_INTERVAL, uInterval ) )
	{
		m_pLogCfgInfo->uChangeLogFileInterval = uInterval * 3600;
	}
	else
	{
		m_pLogCfgInfo->uChangeLogFileInterval = DEFAULT_CHANGE_LOGFILE_INTERVAL * 3600;
	}
	
	// 日志文件路径
	CStr strFile;
	if ( m_pCfgFile->GetValue( SECTION_LOG, strItem.c_str(), strFile ) )
	{
		m_pLogCfgInfo->strFilePath = strFile.GetString();
	}
	else
	{
		char strPath[MAX_PATH];
		CPF::GetModulePathName( strPath, MAX_PATH );
		m_pLogCfgInfo->strFilePath = strPath;
	}
	
	int nEndPos = m_pLogCfgInfo->strFilePath.size() - 1;
	if ( FILENAME_SEPERATOR != m_pLogCfgInfo->strFilePath[ nEndPos ] )
	{
		m_pLogCfgInfo->strFilePath += FILENAME_SEPERATOR;
	}
	
	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getCpuCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- 获取CPU配置信息
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
bool CConfigFileMgr::getCpuCfgInfo()
{
	// 利用率门限，值：100*百分比，0表示不监控该指标
	int nUsePercent = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_CPU, ITEM_CPU_USEPERCENTLIMIT, nUsePercent );

	// 持续超过利用率门限的时间门限，单位：秒，0表示不监控该指标
	int nUseAlarmDuration = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_CPU, ITEM_CPU_USEALARMDURATION, nUseAlarmDuration );

	m_pCpuCfgInfo = new SCpuCfgInfo;
	ACE_ASSERT( m_pCpuCfgInfo );
	if ( NULL == m_pCpuCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}

	m_pCpuCfgInfo->nUsePercentLimit = nUsePercent;
	m_pCpuCfgInfo->nUseAlarmDuration = nUseAlarmDuration;

	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getMemCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- 获取内存配置信息
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
bool CConfigFileMgr::getMemCfgInfo()
{
	// 利用率门限，值：100*百分比，0表示不监控该指标
	int nUsePercent = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_MEM, ITEM_MEM_USEPERCENTLIMIT, nUsePercent );

	// 持续超过利用率门限的时间门限，单位：秒，0表示不监控该指标
	int nUseAlarmDuration = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_MEM, ITEM_MEM_USEALARMDURATION, nUseAlarmDuration );

	m_pMemCfgInfo = new SMemCfgInfo;
	ACE_ASSERT( m_pMemCfgInfo );
	if ( NULL == m_pMemCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}

	m_pMemCfgInfo->nUsePercentLimit = nUsePercent;
	m_pMemCfgInfo->nUseAlarmDuration = nUseAlarmDuration;

	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getProcessCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- 获取进程配置信息
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
bool CConfigFileMgr::getProcCfgInfo()
{
	// CPU利用率门限，值：100*百分比，0表示不监控该指标
	int nCpuUsePercent = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_CPU_USEPERCENTLIMIT, nCpuUsePercent );

	// CPU持续超过利用率门限的时间门限，单位：秒，0表示不监控该指标
	int nCpuUseAlarmDuration = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_CPU_USEALARMDURATION, nCpuUseAlarmDuration );

	// 进程占用内存利用率超门限告警，是以物理内存还是虚拟内存计算选项（1:物理内存，2:虚拟内存），缺省为物理内存
	int nMEMUseFlag = DEFAULT_PROC_MEM_PHY_FLAG;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_MEM_MEMUSEFLAG, nMEMUseFlag );

	// 内存利用率门限，值：100*百分比，0表示不监控该指标
	int nMemUsePercent = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_MEM_USEPERCENTLIMIT, nMemUsePercent );

	// 内存持续超过利用率门限的时间门限，单位：秒，0表示不监控该指标
	int nMemUseAlarmDuration = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_MEM_USEALARMDURATION, nMemUseAlarmDuration );

	// 进程重启时间间隔限制（单位：小时），0表示没有进程重启限制
	int nRestartInterval = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_RESTARTINTERVAL, nRestartInterval );
	//nRestartInterval *= 3600;

	// 进程在RestartInterval时间内最多重启次数，0表示没有进程重启限制
	int nMaxRestartTimes = DEFAULT_INVALID_VALUE;
	m_pCfgFile->GetValue( SECTION_PROC, ITEM_PROC_MAXRESTARTTIMES, nMaxRestartTimes );

	// 被监控进程一般信息列表
#ifdef OS_WINDOWS
	std::string strItem( OS_WINDOWS_ID );
#else
	std::string strItem( OS_LINUX_ID );
#endif
	strItem.append( ITEM_PROC_FULLPATH_EXEFILE );

	std::vector<std::string> arrExeFile;
	if ( !m_pCfgFile->GetValue( SECTION_PROC, strItem.c_str(), arrExeFile )
		|| arrExeFile.size() == 0 )
	{
		ACE_ERROR( ( LM_ERROR, ACE_TEXT( "\n无法获取被监控进程信息！！！\n\n" ) ) );
		return false;
	}

	m_pProcCfgInfo = new SProcCfgInfo;
	ACE_ASSERT( m_pProcCfgInfo );
	if ( NULL == m_pProcCfgInfo )
	{
		ASSERT_INFO;
		return false;
	}

	m_pProcCfgInfo->nCpuUsePercentLimit = nCpuUsePercent;	
	m_pProcCfgInfo->nCpuUseAlarmDuration = nCpuUseAlarmDuration;
	m_pProcCfgInfo->nMEMUseFlag = nMEMUseFlag;
	m_pProcCfgInfo->nMemUsePercentLimit = nMemUsePercent;
	m_pProcCfgInfo->nMemUseAlarmDuration = nMemUseAlarmDuration;
	m_pProcCfgInfo->nRestartInterval = nRestartInterval * 3600; // 转为秒
	m_pProcCfgInfo->nMaxRestartTimes = nMaxRestartTimes;

	SProcComInfo sTmp;
	int nSize = arrExeFile.size(), i = 0;
	for ( i=0; i<nSize; i++ )
	{
		std::string &strFile = arrExeFile[i];
		if ( strFile.empty() )
		{
			continue;
		}
		
		//strFile.AllTrim();
		sTmp.strExeFile = strFile;
		m_pProcCfgInfo->vectComInfo.push_back( sTmp );
	}

	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getDiskCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- 获取磁盘配置信息
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
bool CConfigFileMgr::getDiskCfgInfo()
{
//	m_pDiskCfgInfo = new SDiskCfgInfo;
//	ACE_ASSERT( m_pDiskCfgInfo );

	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getFileSysCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- 获取文件系统配置信息
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
bool CConfigFileMgr::getFileSysCfgInfo()
{
//	m_pFileSysCfgInfo = new SFileSysCfgInfo;
//	ACE_ASSERT( m_pFileSysCfgInfo );

	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getCardCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- 获取板卡配置信息
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
bool CConfigFileMgr::getCardCfgInfo()
{
//	m_pCardCfgInfo = new SCardCfgInfo;
//	ACE_ASSERT( m_pCardCfgInfo );
	
	return true;
}

/******************************************************************************
 * FUNCTION    : CConfigFileMgr::getNetCfgInfo()
 *       
 * DESCRIPTION : 
 *       -- 获取网络配置信息
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
bool CConfigFileMgr::getNetCfgInfo()
{
//	m_pNetCfgInfo = new SNetCfgInfo;
//	ACE_ASSERT( m_pNetCfgInfo );

	return true;
}


