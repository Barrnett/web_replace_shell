/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  FileSysMgr.h
 *
 *  TITLE      :  Probeϵͳ�ļ�ϵͳ����������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  ������������ṩ�����ļ�ϵͳ��һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_FILESYS_MANAGER_H__
#define __PROBE_FILESYS_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileSysInfo.h"

// Probeϵͳ��Դ������
class EXPORT_CLASS CProbeFileSysMgr
{
public:
	CProbeFileSysMgr();
	~CProbeFileSysMgr();

	// ��ʼ��
	bool Init( IN const SFileSysCfgInfo *pCfgInfo = NULL );

	// ��ȡ������ļ�ϵͳ����
	int GetFileSysCount() const
	{	return m_vectInfo.size();	}

	// ����Ż�ȡ�ļ�ϵͳ��Ϣ
	const CFileSysInfo* GetFileSysInfoByNth( IN int nNth );
	
	
	// �Ƿ��������ʳ����޸澯
	bool IsFileSysUsePercentAlarm( OUT CFileSysInfo* &pFileSysInfo );
	
private:

private:
	// �ļ�ϵͳ������Ϣ
	SFileSysCfgInfo					*m_pCfgInfo;

	// �ļ�ϵͳ��Ϣ���������
	std::vector< CFileSysInfo* >	m_vectInfo;

};

#endif // __PROBE_FILESYS_MANAGER_H__

