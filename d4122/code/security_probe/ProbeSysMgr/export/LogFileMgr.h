/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  LogFileMgr.h
 *
 *  TITLE      :  Probe系统监控维护系统日志文件管理类声明
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  
 *
 *****************************************************************************/

#ifndef __LOG_FILE_MANAGER_H__
#define __LOG_FILE_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

#include "ace/Singleton.h"

// Probe系统监控维护系统日志文件管理类
class PROBESYSMGR_CLASS CLogFileMgr
{
public:
	CLogFileMgr();
	~CLogFileMgr();

	// 初始化
	bool Init();

	// 重新设置输出文件路径
	bool SetFilePath(	IN const char *strLogFile = NULL,		// 日志文件路径
						IN bool bIsDirStdErr = false );			// 是否同时定向到标准输出

	// 重新设置输出文件名称
	bool SetNewLogFileName( IN const ACE_Time_Value &rCurrentTime );
	
	// 设置是否同时定向到标准输出
	void SetLogToStdErr( IN bool bIsDirStdErr = false );

	// 是否同时定向到标准输出
	bool IsLogToStdErr() const
	{	return m_bIsDirStdErr;	}

	// 获取日志文件全路径名称
	const std::string& GetFileName() const
	{	return m_strFileName;	}

private:
	// 重新设置输出文件
	bool resetFile();

private:
	// 日志文件路径
	std::string			m_strFilePath;

	// 当前日志文件名
	std::string			m_strFileName;

	// 是否同时定向到标准输出
	bool				m_bIsDirStdErr;

	// 日志文件流
	std::ofstream		*m_pLogFile;	
	
};

// 暂时向标准输出日志功能类
class PROBESYSMGR_CLASS CLogToStdErrGuard
{
public:
	CLogToStdErrGuard( IN CLogFileMgr *pLogFile )
		: m_pLogFile( pLogFile ), m_bIsDirStdErr( true )
	{
		if ( NULL != m_pLogFile )
		{
			m_bIsDirStdErr = m_pLogFile->IsLogToStdErr();
			if ( !m_bIsDirStdErr )
			{
				m_pLogFile->SetLogToStdErr( true );
			}
		}
	}

	~CLogToStdErrGuard()
	{
		if ( NULL != m_pLogFile )
		{
			if ( !m_bIsDirStdErr )
			{
				m_pLogFile->SetLogToStdErr( m_bIsDirStdErr );
			}
		}
	}

private:
	// Probe系统监控维护系统日志文件管理类对象
	CLogFileMgr *m_pLogFile;

	// 是否同时定向到标准输出
	bool		m_bIsDirStdErr;

};

// 定义日志文件访问对象为单件模式
typedef ACE_Singleton< CLogFileMgr, ACE_Null_Mutex > LogFileMgrSingleton;

// 定义日志文件访问单件对象为"LOG_FILE_MGR"，对日志文件的初始化只能通过这个对象进行，
// 记录日志则调用ACE_DEBUG
#define LOG_FILE_MGR LogFileMgrSingleton::instance()

#endif // __LOG_FILE_MANAGER_H__

