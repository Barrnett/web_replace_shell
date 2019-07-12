/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  LogFileMgr.cpp
 *
 *  TITLE      :  Probe系统监控维护系统日志文件管理类实现
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  
 *
 *****************************************************************************/

#include "LogFileMgr.h"
#include "fstream"
#include "cpf/path_tools.h"

// 定义日志文件访问对象为单件模式
#if defined( ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION )
	template class ACE_Singleton< CLogFileMgr, ACE_Null_Mutex >;
	
#elif defined( ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA )
	#pragma ACE_Singleton< CLogFileMgr, ACE_Null_Mutex >
	
#elif defined( __GNUC__ ) && ( defined( _AIX ) || defined( __hpux ) )
	template ACE_Singleton< CLogFileMgr, ACE_Null_Mutex > *
	ACE_Singleton< CLogFileMgr, ACE_Null_Mutex >::singleton_;

#endif


/********************************************************************
Public Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CLogFileMgr::CLogFileMgr()
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
CLogFileMgr::CLogFileMgr()
 : m_pLogFile( NULL )
{
	
}

/******************************************************************************
 * FUNCTION    : CLogFileMgr::~CLogFileMgr()
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
CLogFileMgr::~CLogFileMgr()
{
	ACE_LOG_MSG->clr_flags( ACE_Log_Msg::OSTREAM );
	
	m_pLogFile->close();

	if ( NULL != m_pLogFile )
	{
		delete m_pLogFile;
		m_pLogFile = NULL;
	}
}

/******************************************************************************
 * FUNCTION    : CLogFileMgr::Init()
 *       
 * DESCRIPTION : 
 *       -- 初始化
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
bool CLogFileMgr::Init()
{	
	char strPath[MAX_PATH];
	if ( !CPF::GetModulePathName( strPath, sizeof( strPath ) ) )
	{
		ACE_ASSERT( 0 );
		ASSERT_INFO;
		return false;
	}

	std::string strLogFilePath( strPath );
	strLogFilePath += FILENAME_SEPERATOR_STR;
	
	m_strFilePath = strLogFilePath;
	m_strFileName = PROBESYSMONITOR_INIT_LOGFILE;

	m_bIsDirStdErr = true;
	ACE_LOG_MSG->set_flags( ACE_Log_Msg::STDERR );
	
	if ( !resetFile() )
	{
		return false;
	}
	
	ACE_DEBUG( ( LM_INFO, ACE_TEXT( "\n\n--------------------------------------------------------------------------\n" ) ) );
	ACE_DEBUG( ( LM_INFO, ACE_TEXT( "---- ProbeSysMonitor系统本次开始时间[%D] ----\n" ) ) );
	ACE_DEBUG( ( LM_INFO, ACE_TEXT( "--------------------------------------------------------------------------\n\n" ) ) );
	
	return true;
}

/******************************************************************************
 * FUNCTION    : CLogFileMgr::SetFilePath(IN const char *strLogFile, 
 *               IN bool bIsDirStdErr)
 *       
 * DESCRIPTION : 
 *       -- 
 *       
 * PARAMETER   : 
 *       1: strLogFile -- IN, 全路径日志文件名称
 *       2: bIsDirStdErr -- IN, 是否同时定向到标准输出
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
bool CLogFileMgr::SetFilePath(	IN const char *strLogFile, 
								IN bool bIsDirStdErr )
{
	std::string strLogFilePath;
	if ( NULL == strLogFile )
	{
		char strPath[MAX_PATH];
		if ( !CPF::GetModulePathName( strPath, sizeof( strPath ) ) )
		{
			ACE_ASSERT( 0 );
			return false;
		}
	
		strLogFilePath = strPath;
		strLogFilePath += FILENAME_SEPERATOR_STR;
	}
	else
	{
		strLogFilePath = strLogFile;
	}
	
	m_strFilePath = strLogFilePath;

	SetLogToStdErr( bIsDirStdErr );
	
	ACE_Time_Value rCurrentTime = ACE_OS::gettimeofday();
	return SetNewLogFileName( rCurrentTime );
}

/******************************************************************************
 * FUNCTION    : CLogFileMgr::SetNewLogFileName(IN const ACE_Time_Value &rCurrentTime)
 *       
 * DESCRIPTION : 
 *       -- 重新设置输出文件名称
 *       
 * PARAMETER   : 
 *       1: rCurrentTime -- IN, 当前时间
 *       
 * RETURN     : 
 *       bool -- true if success, false else.
 *       
 * SPECIAL    : 日志文件名统一为“ProbeSysMonitor_YYYYMMDDhh.log”
 *       
 *****************************************************************************/
bool CLogFileMgr::SetNewLogFileName( IN const ACE_Time_Value &rCurrentTime )
{
	time_t clock = rCurrentTime.sec();
	struct tm * psTime = ACE_OS::localtime( &clock );
	ACE_ASSERT( psTime );
	if ( NULL == psTime )
	{
		ASSERT_INFO;
		return false;
	}
	
	int nYear = psTime->tm_year + 1900;
	int nMon = psTime->tm_mon + 1;
	int nDay = psTime->tm_mday;
	int nHour = psTime->tm_hour;
	
	char buf[32];
	ACE_OS::sprintf( buf, DEFAULT_PROBESYS_LOGFILE_NAME, nYear, nMon, nDay, nHour );
	
	m_strFileName = buf;

	return resetFile();
}

/******************************************************************************
 * FUNCTION    : CLogFileMgr::SetLogToStdErr(IN bool bIsDirStdErr)
 *       
 * DESCRIPTION : 
 *       -- 设置是否同时定向到标准输出
 *       
 * PARAMETER   : 
 *       1: bIsDirStdErr -- IN, 是否同时定向到标准输出标志
 *       
 * RETURN     : 
 *       void
 *       
 * SPECIAL    : 
 *       
 *****************************************************************************/
void CLogFileMgr::SetLogToStdErr( IN bool bIsDirStdErr )
{
	if ( bIsDirStdErr == m_bIsDirStdErr )
	{
		return;
	}

	m_bIsDirStdErr = bIsDirStdErr;
	
	if ( m_bIsDirStdErr )
	{
		ACE_LOG_MSG->set_flags( ACE_Log_Msg::STDERR );
	}
	else
	{
		ACE_LOG_MSG->clr_flags( ACE_Log_Msg::STDERR );
	}	
}

/********************************************************************
Private Function
*********************************************************************/

/******************************************************************************
 * FUNCTION    : CLogFileMgr::resetFile()
 *       
 * DESCRIPTION : 
 *       -- 重新设置输出文件
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
bool CLogFileMgr::resetFile()
{
	if ( NULL == m_pLogFile )
	{
		m_pLogFile = new std::ofstream();
		ACE_ASSERT( NULL != m_pLogFile );
		if ( NULL == m_pLogFile )
		{
			ASSERT_INFO;
			return false;
		}
	}
	else
	{
		m_pLogFile->close();
	}

	std::string strFileName = m_strFilePath + m_strFileName;

	m_pLogFile->open( strFileName.c_str(), ios::out | ios::app );
	ACE_LOG_MSG->msg_ostream( m_pLogFile );
	
	if ( m_bIsDirStdErr )
	{
		ACE_LOG_MSG->clr_flags( ACE_Log_Msg::LOGGER );
	}
	else
	{
		ACE_LOG_MSG->clr_flags( ACE_Log_Msg::STDERR | ACE_Log_Msg::LOGGER );
	}
	
	ACE_LOG_MSG->set_flags( ACE_Log_Msg::OSTREAM );
	
	return true;
}

