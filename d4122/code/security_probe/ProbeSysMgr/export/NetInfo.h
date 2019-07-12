/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  NetInfo.h
 *
 *  TITLE      :  Probeϵͳ������Ϣ������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  �����ṩĳһ�����һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_NET_INFOMATION_H__
#define __PROBE_NET_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// ������Ϣ��
class EXPORT_CLASS CNetInfo
{
public:
	CNetInfo();
	~CNetInfo();

	// ��ʼ��
	bool Init( IN const SNetCfgInfo *pCfgInfo = NULL );

	// �Ƿ��������ʳ����޸澯
	bool IsUsePercentAlarm();
	
private:


public:
	// ���
	int				m_nIndex;
	// ����
	std::string		m_strDescribe;
	// ������ʣ���λ��M/�룩
	int				m_nMaxSpeed;
	// ��ǰ���ʣ���λ��k/�룩
	int				m_nCurrentSpeed;

private:
	// ����������Ϣ
	SNetCfgInfo		*m_pCfgInfo;

};

#endif // __PROBE_NET_INFOMATION_H__

