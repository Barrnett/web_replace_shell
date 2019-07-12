/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProbeCpuMgr.h
 *
 *  TITLE      :  ProbeϵͳCPU��Ϣ����������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  ������������ṩ����CPU��һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_CPU_MANAGER_H__
#define __PROBE_CPU_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

#include "CpuInfo.h"

// ProbeϵͳCPU��Ϣ������
class PROBESYSMGR_CLASS CProbeCpuMgr
{
public:
	CProbeCpuMgr();
	~CProbeCpuMgr();

	// ��ʼ��
	bool Init( IN const SCpuCfgInfo *pCfgInfo = NULL );

	// ����CPU״̬��Ϣ
	bool Refresh()
	{	return m_rInfo.Refresh();	}
	
	// �Ƿ��������ʳ����޸澯
	bool IsUsePercentAlarm();
	
	// ��ȡCPU״̬��Ϣ
	const CCpuInfo& GetCpuInfo();


	// ��ȡCPU������Ϣ����
	const SCpuCfgInfo* GetCfgInfo() const
	{	return m_pCfgInfo;	}

	// �߳�ͬ�����
	MUTEX& GetMutex()
	{	return m_rMutex;	}

private:
	// ProbeϵͳCPU��Ϣ����
	CCpuInfo			m_rInfo;

	// CPU������Ϣ
	const SCpuCfgInfo	*m_pCfgInfo;

	// ��һ��CPU�����ʸ澯ʱ��
	ACE_Time_Value		m_rLastUasgeAlarmTime;

	// �߳�ͬ�����
	MUTEX m_rMutex;

};

#endif // __PROBE_CPU_MANAGER_H__

