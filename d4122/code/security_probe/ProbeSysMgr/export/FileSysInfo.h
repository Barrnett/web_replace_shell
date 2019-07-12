/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  FileSysInfo.h
 *
 *  TITLE      :  Probeϵͳ�ļ�ϵͳ��Ϣ������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  �����ṩĳһ�ļ�ϵͳ��һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_FILESYS_INFOMATION_H__
#define __PROBE_FILESYS_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// �ļ�ϵͳ��Ϣ��
class EXPORT_CLASS CFileSysInfo
{
public:
	CFileSysInfo();
	~CFileSysInfo();

	// ��ʼ��
	bool Init( IN const SFileSysCfgInfo *pCfgInfo = NULL );

	// �Ƿ��������ʳ����޸澯
	bool IsSysUsePercentAlarm();
	
private:


public:
	// ���
	int				m_nIndex;
	// ·��
	std::string		m_strPath;
	// ����������λ��M��
	int				m_nCapacity;
	// ������
	int 			m_nUsePercent;

private:
	// �ļ�ϵͳ������Ϣ
	SFileSysCfgInfo		*m_pCfgInfo;

};

#endif // __PROBE_FILESYS_INFOMATION_H__

