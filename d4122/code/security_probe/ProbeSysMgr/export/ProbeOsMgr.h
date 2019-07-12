/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  OsMgr.h
 *
 *  TITLE      :  Probeϵͳ����ϵͳ����������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  ������������ṩ����ϵͳ��һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_OS_MANAGER_H__
#define __PROBE_OS_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OsInfo.h"

// Probeϵͳ����ϵͳ��Ϣ������
class EXPORT_CLASS CProbeOsMgr
{
public:
	CProbeOsMgr();
	~CProbeOsMgr();

	// ��ʼ��
	bool Init();

	// ��ȡ����ϵͳ��Ϣ
	const COsInfo* GetOsInfo() const;
	
private:

private:
	// ����ϵͳ��Ϣ�����
	COsInfo	m_sInfo;

};

#endif // __PROBE_OS_MANAGER_H__

