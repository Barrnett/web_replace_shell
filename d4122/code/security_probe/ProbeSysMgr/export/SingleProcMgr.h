/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  SingleProcMgr.h
 *
 *  TITLE      :  �������̹���������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  �������ĳһ���̣�
 *
 *****************************************************************************/

#ifndef __PROBE_SINGLE_PROC_MANAGER_H__
#define __PROBE_SINGLE_PROC_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProcInfo.h"

// ������Ϣ��
class PROBESYSMGR_CLASS CSingleProcMgr
{
public:
	CSingleProcMgr();
	~CSingleProcMgr();

	// ��ʼ��
	bool Init(	IN const char * szExeName,
				IN const CMemInfo *pMemInfo = NULL,
				IN const SProcCfgInfo *pCfgInfo = NULL );

	// ���½���״̬
	bool Refresh()
	{	return false;//return m_rInfo.Refresh();	
	}

	// �Ƿ��������ʳ����޸澯
	bool IsRunning()
	{	return m_rInfo.IsRunning();	}
	
	// �Ƿ�ʱ�����̿�ִ���ļ����ƻ�ȫ·����ִ���ļ�����
	bool IsMe( IN unsigned int uPID ) const
	{	return m_rInfo.m_uPID == uPID;	}

	// �Ƿ�ʱ�����̿�ִ���ļ����ƻ�ȫ·����ִ���ļ�����
	bool IsMe( IN const char * pszExeFileName ) const
	{	return m_rInfo.IsMe( pszExeFileName );	}

	// ��ȡ������Ϣ����������ȡ������Ϣ��Ӧ�ȵ���Refresh������
	CProcInfo& GetProcInfo()
	{	return m_rInfo;	}

	
	// �Ƿ��������ʳ����޸澯������ǰӦ�ȵ���Refresh����������������ݲ����Զ����£�
	bool IsCpuUsePercentAlarm();
	
	// �Ƿ����ڴ������ʳ����޸澯������ǰӦ�ȵ���Refresh����������������ݲ����Զ����£�
	bool IsMemUsePercentAlarm();

	// �Ƿ��н����쳣��ֹ�澯
	bool IsAbnormityEndAlarm()
	{ return (m_rInfo.m_rAbnormityEndTime != ACE_Time_Value::zero); }

	// �澯��ָ��쳣��ֹʱ��
	void ResetAbnormityEndTimeZero()
	{ m_rInfo.m_rAbnormityEndTime = ACE_Time_Value::zero; }

	// �������̣������̴�δ�������쳣��ֹʱ�ٴ��������д������ƣ�
	bool Start();

	// �����رս��̣�������������󣬱����̲�����ʹ��Start����������
	bool Close();

	bool IsActiveClose()
	{	return m_bActiveClose;	}

	
	// ��ֹ����
	bool Terminate()
	{	return m_rInfo.Terminate();	}
	
	// �������̣�����������û�д������ƣ�
	bool Restart();

	// ��ȡ����������Ϣ
	const SProcCfgInfo* GetCfgInfo()
	{	return m_pCfgInfo;	}

	const ACE_Time_Value& GetFirstCreateTime()
	{ return m_rFirstCreateTime; }

private:
	// �������Ƿ�Ϊ�����ر�
	bool			m_bActiveClose;

	// ������Ϣ�����
	CProcInfo		m_rInfo;
	
	// ����������Ϣ
	const SProcCfgInfo	*m_pCfgInfo;

	// ��һ��CPU�����ʸ澯ʱ��
	ACE_Time_Value	m_rLastCpuUasgeAlarmTime;

	// ��һ���ڴ������ʸ澯ʱ��
	ACE_Time_Value	m_rLastMemUasgeAlarmTime;

	// ��һ�ν�������ʱ��
	ACE_Time_Value	m_rFirstCreateTime;

	// ������������
	unsigned int	m_uRestartTimes;

	// �߳�ͬ�����
	MUTEX m_rMutex;

};

#endif // __PROBE_SINGLE_PROC_MANAGER_H__

