/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  LogFileMgr.h
 *
 *  TITLE      :  Probeϵͳ���ά��ϵͳ��־�ļ�����������
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

// Probeϵͳ���ά��ϵͳ��־�ļ�������
class PROBESYSMGR_CLASS CLogFileMgr
{
public:
	CLogFileMgr();
	~CLogFileMgr();

	// ��ʼ��
	bool Init();

	// ������������ļ�·��
	bool SetFilePath(	IN const char *strLogFile = NULL,		// ��־�ļ�·��
						IN bool bIsDirStdErr = false );			// �Ƿ�ͬʱ���򵽱�׼���

	// ������������ļ�����
	bool SetNewLogFileName( IN const ACE_Time_Value &rCurrentTime );
	
	// �����Ƿ�ͬʱ���򵽱�׼���
	void SetLogToStdErr( IN bool bIsDirStdErr = false );

	// �Ƿ�ͬʱ���򵽱�׼���
	bool IsLogToStdErr() const
	{	return m_bIsDirStdErr;	}

	// ��ȡ��־�ļ�ȫ·������
	const std::string& GetFileName() const
	{	return m_strFileName;	}

private:
	// ������������ļ�
	bool resetFile();

private:
	// ��־�ļ�·��
	std::string			m_strFilePath;

	// ��ǰ��־�ļ���
	std::string			m_strFileName;

	// �Ƿ�ͬʱ���򵽱�׼���
	bool				m_bIsDirStdErr;

	// ��־�ļ���
	std::ofstream		*m_pLogFile;	
	
};

// ��ʱ���׼�����־������
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
	// Probeϵͳ���ά��ϵͳ��־�ļ����������
	CLogFileMgr *m_pLogFile;

	// �Ƿ�ͬʱ���򵽱�׼���
	bool		m_bIsDirStdErr;

};

// ������־�ļ����ʶ���Ϊ����ģʽ
typedef ACE_Singleton< CLogFileMgr, ACE_Null_Mutex > LogFileMgrSingleton;

// ������־�ļ����ʵ�������Ϊ"LOG_FILE_MGR"������־�ļ��ĳ�ʼ��ֻ��ͨ�����������У�
// ��¼��־�����ACE_DEBUG
#define LOG_FILE_MGR LogFileMgrSingleton::instance()

#endif // __LOG_FILE_MANAGER_H__

