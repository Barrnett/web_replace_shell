/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProbeMemMgr.h
 *
 *  TITLE      :  Probeϵͳ�ڴ����������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  ������������ṩ�ڴ��һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_MEMORY_MANAGER_H__
#define __PROBE_MEMORY_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

#include "MemInfo.h"

// Probeϵͳ��Դ������
class PROBESYSMGR_CLASS CProbeMemMgr
{
public:
	CProbeMemMgr();
	~CProbeMemMgr();

	// ��ʼ��
	bool Init( IN const SMemCfgInfo *pCfgInfo = NULL );

	// �����ڴ�״̬��Ϣ
	bool Refresh()
	{	return m_rInfo.Refresh();	}
	
	// ��ȡ�ڴ�״̬��Ϣ
	const CMemInfo& GetMemInfo();

	// �Ƿ��������ʳ����޸澯
	bool IsUsePercentAlarm();
	

	// ��ȡ�ڴ�������Ϣ����
	const SMemCfgInfo* GetCfgInfo() const
	{	return m_pCfgInfo;	}

	// �߳�ͬ�����
	MUTEX& GetMutex()
	{	return m_rMutex;	}


private:

private:
	// Probeϵͳ�ڴ���Ϣ����
	CMemInfo			m_rInfo;
	
	// �ڴ�������Ϣ
	const SMemCfgInfo	*m_pCfgInfo;

	// ��һ���ڴ������ʸ澯ʱ��
	ACE_Time_Value		m_rLastUasgeAlarmTime;

	// �߳�ͬ�����
	MUTEX m_rMutex;

};

#endif // __PROBE_MEMORY_MANAGER_H__

