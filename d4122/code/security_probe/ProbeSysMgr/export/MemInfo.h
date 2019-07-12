/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  MemInfo.h
 *
 *  TITLE      :  Probeϵͳ�ڴ���Ϣ������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  �����ṩĳһ�ڴ��һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_MEMORY_INFOMATION_H__
#define __PROBE_MEMORY_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// �ڴ���Ϣ��
class PROBESYSMGR_CLASS CMemInfo
{
public:
	CMemInfo();
	~CMemInfo();

	// ��ʼ��
	bool Init();

	// �����ڴ�״̬��Ϣ
	bool Refresh();
	
private:


public:
	// �����ڴ�����������λ��KB��
	unsigned int	m_uPhysicalMem;
	// �����ڴ�ʣ����������λ��KB��
	unsigned int	m_uFreePhysicalMem;

	// �����ļ�����������λ��KB��
	unsigned int	m_uPageFile;
	// �����ļ�ʣ����������λ��KB��
	unsigned int	m_uFreePageFile;

	// �����ڴ�����������λ��KB��
	unsigned int	m_uVirtualMem;
	// �����ڴ�ʣ����������λ��KB��
	unsigned int	m_uFreeVirtualMem;

	// ��ǰ������
	unsigned int	m_uCurrentUsage;

};

#endif // __PROBE_MEMORY_INFOMATION_H__

