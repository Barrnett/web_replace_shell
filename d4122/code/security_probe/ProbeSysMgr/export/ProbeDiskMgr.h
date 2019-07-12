/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProbeDiskMgr.h
 *
 *  TITLE      :  Probeϵͳ���̹���������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  ������������ṩ���д��̵�һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_DISK_MANAGER_H__
#define __PROBE_DISK_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DiskInfo.h"

// Probeϵͳ��Դ������
class PROBESYSMGR_CLASS CProbeDiskMgr
{
public:
	CProbeDiskMgr();
	~CProbeDiskMgr();

	// ��ʼ��
	bool Init(IN const CDiskInfo * pDiskInfo,IN const SDiskCfgInfo *pCfgInfo);

	// ��ȡ����ش��̸���
	int GetDiskCount() const;

	// ����Ż�ȡ������Ϣ
	const CDiskInfo::DISK_INFO* GetDiskInfoByNth( IN int nNth );
	int	GetDiskAlarmLevelByNth( IN int nNth );

	void genrate_disk_alarm();


private:
	// ����������Ϣ
	const SDiskCfgInfo		*m_pCfgInfo;

	// ������Ϣ���������
	const CDiskInfo			*m_pDiskInfo;

	int *					m_pCurAlaramLevel;//ÿ�������ĸ澯����
	size_t					m_nInitDiskNums;//��ʼ����ʱ����̵ĸ���

};

#endif // __PROBE_DISK_MANAGER_H__

