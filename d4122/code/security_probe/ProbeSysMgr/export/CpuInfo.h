/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  CpuInfo.h
 *
 *  TITLE      :  ProbeϵͳCPU��Ϣ������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  �����ṩĳһCPU��һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_CPU_INFOMATION_H__
#define __PROBE_CPU_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// CPU��Ϣ��
class PROBESYSMGR_CLASS CCpuInfo
{
public:
	enum{MAX_CPU_COUNT=16};

public:
	CCpuInfo();
	~CCpuInfo();

	// ��ʼ��
	bool Init();

	// ����CPU״̬��Ϣ
	bool Refresh();

private:
	// ��ȡ��ǰCPU������
	void getCurrentUsage();

#ifdef OS_WINDOWS
	// ��ȡCPU��Ƶ
	void getCPUFrequence();
#else
	void GetIdleTime(ACE_UINT64* iIdleTime, ACE_UINT64* iTotalTime);
#endif

public:
	// Ʒ��
	//std::string		m_strOEM;
	// �ͺ�
	std::string		m_strType;
	
	// CPU����
	unsigned int	m_uCpuCount;
		
	// ��Ƶ����λ��M��
	unsigned int	m_uFrequency;

	// ��ǰ������
	unsigned int	m_uCurrentUsage[MAX_CPU_COUNT];

private:

    ACE_UINT64		m_liOldIdleTime[MAX_CPU_COUNT];
    ACE_UINT64		m_liOldSystemTime[MAX_CPU_COUNT];

};

#endif // __PROBE_CPU_INFOMATION_H__

