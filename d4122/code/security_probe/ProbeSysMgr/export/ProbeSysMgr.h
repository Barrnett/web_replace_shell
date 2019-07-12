/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProbeSysMgr.h
 *
 *  TITLE      :  Probeϵͳ��Դ����������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  �����������ϵͳ��Դ���ṩ��ϵͳ��Դ���в�ѯ�Ͳ����Ľӿ�
 *
 *****************************************************************************/

#ifndef __PROBE_SYSTEM_MANAGER_H__
#define __PROBE_SYSTEM_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LogFileMgr.h"
#include "ConfigFileMgr.h"

#include "ProbeCpuMgr.h"
#include "ProbeMemMgr.h"
#include "ProbeProcMgr.h"
//#include "ProbeOSMgr.h"
//#include "ProbeDiskMgr.h"
//#include "ProbeFileSysMgr.h"
//#include "ProbeCardMgr.h"
//#include "ProbeNetMgr.h"


// Probeϵͳ��Դ������
class PROBESYSMGR_CLASS CProbeSysMgr
{
public:
	CProbeSysMgr();
	~CProbeSysMgr();

	// ��ʼ��
	bool Init( IN const CConfigFileMgr *pCfgFile );

	// �������б����Probeϵͳ��Դ״̬
	void Refresh();

	// �رղ���ϵͳ
	void CloseOS();

	// ��������ϵͳ
	void RebootOS();
	
	
	// �����ļ�����
	const CConfigFileMgr* GetCfgFileMgr() const
	{	return m_pCfgFile;	}

	// CPU���������
	CProbeCpuMgr& GetProbeCpuMgr() const
	{	return *m_pCpuMgr;	}
	
	// �ڴ���������
	CProbeMemMgr& GetProbeMemMgr() const
	{	return *m_pMemMgr;	}
	
	// ���̹�������󣬿���Ϊ��
	CProbeProcMgr* GetProbeProcessMgr() const
	{	return m_pProcMgr;  }
/*	
	// ����ϵͳ��Ϣ���������
	CProbeOSMgr& GetProbeOSMgr() const
	{	return *m_pOSMgr; 		}
	
	// ���̹��������
	CProbeDiskMgr& GetProbeDiskMgr() const
	{	return *m_pDiskMgr; 	}
	
	// �ļ�ϵͳ��Ϣ���������
	CProbeFileSysMgr& GetProbeFileSysMgr() const
	{	return *m_pFileSysMgr;  }
	
	// �忨���������
	CProbeCardMgr& GetProbeCardMgr() const
	{	return *m_pCardMgr; 	}
	
	// ����״̬��Ϣ���������
	CProbeNetMgr& GetProbeNetMgr() const
	{	return *m_pNetMgr; 	    }
*/

	// �߳�ͬ�����
	MUTEX& GetMutex()
	{	return m_rMutex;	}

private:
	typedef enum
	{
		E_OS_REBOOT = 1,	// ��������ϵͳ
		E_OS_CLOSE			// �رղ���ϵͳ
	} ESetOsCmd;

	// �رջ���������ϵͳ
	void setOs( IN ESetOsCmd eCmd = E_OS_REBOOT );

private:
	// �����ļ�����
	const CConfigFileMgr	*m_pCfgFile;

	CProbeCpuMgr 			*m_pCpuMgr; 	// CPU��������󣬸�����������ṩ����CPU��һ����Ϣ��
	CProbeMemMgr			*m_pMemMgr; 	// �ڴ��������󣬸����ȡ�ڴ���Ϣ�������ṩ���ʽӿڣ�
	CProbeProcMgr 			*m_pProcMgr;	// ���̹�������󣬸�����������ṩ�������н��̵Ŀ��ƽӿں�һ����Ϣ��
/*	CProbeOSMgr 			*m_pOSMgr; 		// ����ϵͳ��Ϣ��������󣬸����ȡ����ϵͳ��Ϣ�������ṩ���ʽӿڣ�
	CProbeDiskMgr 			*m_pDiskMgr; 	// ���̹�������󣬸�����������ṩ���д��̵�һ����Ϣ��
	CProbeFileSysMgr 		*m_pFileSysMgr; // �ļ�ϵͳ��Ϣ��������󣬸�����������ṩ�����ļ�ϵͳ��һ����Ϣ��
	CProbeCardMgr 			*m_pCardMgr; 	// �忨��������󣬸�����������ṩ���а忨��һ����Ϣ��
	CProbeNetMgr 			*m_pNetMgr; 	// ����״̬��Ϣ��������󣬸����ȡ����״̬��Ϣ�������ṩ���ʽӿڣ�
*/
	
	// �߳�ͬ�����
	MUTEX m_rMutex;

};

#endif // __PROBE_SYSTEM_MANAGER_H__

