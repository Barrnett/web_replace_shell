/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  DiskInfo.h
 *
 *  TITLE      :  Probeϵͳ������Ϣ������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  �����ṩĳһ���̵�һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_DISK_INFOMATION_H__
#define __PROBE_DISK_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// ������Ϣ��
class PROBESYSMGR_CLASS CDiskInfo
{
public:
	CDiskInfo();
	~CDiskInfo();

	// ��ʼ��
	bool Init();

	//���»�ȡ������Ϣ
	bool Refresh();

public:
	typedef struct tag_DISK_INFO{

		tag_DISK_INFO()
		{
			nCapacity = 0;
			nUsed = 0;
		}

		std::string			name;

		// ����������λ��M��
		size_t				nCapacity;

		// ��ʹ�ô��̣���λ��M��
		size_t 				nUsed;
	}DISK_INFO;

	std::vector<DISK_INFO>	m_vtDisk;


};

#endif // __PROBE_DISK_INFOMATION_H__



