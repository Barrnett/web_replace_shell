/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  OsInfo.h
 *
 *  TITLE      :  Probeϵͳ����ϵͳ��Ϣ������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  �����ṩĳһ����ϵͳ��һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_OS_INFOMATION_H__
#define __PROBE_OS_INFOMATION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbeSysCommon.h"

// ����ϵͳ��Ϣ��
class PROBESYSMGR_CLASS COsInfo
{
public:
	COsInfo();
	~COsInfo();

	// ��ʼ��
	bool Init();

private:


public:
	// Ʒ��
	std::string		m_strTrademark;
	// �汾
	std::string		m_strVersion;
	// ���й�˾
	std::string		m_strCompany;

	std::string		m_strHostName;

private:
	
};

#endif // __PROBE_OS_INFOMATION_H__

