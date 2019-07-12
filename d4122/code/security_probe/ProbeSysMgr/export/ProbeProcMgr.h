/******************************************************************************
 *  COPYRIGHTS :  
 *
 *  FILE NAME  :  ProbeProcMgr.h
 *
 *  TITLE      :  Probeϵͳ���̹���������
 *
 *  CREATE TIME:  2005-10-27 15:38:30
 *
 *  AUTHOR     :  
 *
 *  DESCRIPTION:  ������������ṩ���н��̵�һ����Ϣ��
 *
 *****************************************************************************/

#ifndef __PROBE_PROCESS_MANAGER_H__
#define __PROBE_PROCESS_MANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleProcMgr.h"

// �����б�
typedef std::vector< CSingleProcMgr* > VectorSingleProcMgr;

class CMemInfo;

// Probeϵͳ��Դ������
class PROBESYSMGR_CLASS CProbeProcMgr
{
public:
	CProbeProcMgr();
	~CProbeProcMgr();

	// ��ʼ��
	bool Init(	IN const CMemInfo *pMemInfo = NULL, 
				IN const SProcCfgInfo *pCfgInfo = NULL );
	

	// �������н���״̬
	int Refresh( OUT VectorSingleProcMgr &vectFailedProc );

	// ��ȡ����ؽ��̸���
	int GetProcCount() const
	{	return m_vectSingleProc.size();	}

	// ��ȡ����ص��������еĽ��̸���
	int GetProcRunningCount() const;

	// ����������ֹ�Ľ���
	int RestartAllTerminateProc( OUT VectorSingleProcMgr &vectRestartFailedProc );

	// �������н���
	int RestartAllProc( OUT VectorSingleProcMgr &vectRestartFailedProc );

	// ����Ż�ȡָ������
	CSingleProcMgr* GetProcByNth( IN int nNth );

	// �����̺Ż�ȡָ������
	CSingleProcMgr* GetProcByPID( IN unsigned int uPID );

	// �����̵Ŀ�ִ���ļ����ƻ�ȡָ�����̣�����ֻ���ļ�����Ҳ����Ϊȫ·����
	CSingleProcMgr* GetProcByName( IN const char * szExeFileName );


	// �Ƿ���CPU�����ʳ����޸澯
	bool IsCpuUsePercentAlarm( OUT CSingleProcMgr* &pSingleMgr );

	// �Ƿ����ڴ������ʳ����޸澯
	bool IsMemUsePercentAlarm( OUT CSingleProcMgr* &pSingleMgr );

	// �Ƿ��н����쳣��ֹ�澯
	bool IsAbnormityEndAlarm( OUT CSingleProcMgr* &pSingleMgr);
	
	// ��ȡ����������Ϣ����
	const SProcCfgInfo* GetCfgInfo() const
	{	return m_pCfgInfo;	}

	// �߳�ͬ�����
	MUTEX& GetMutex()
	{	return m_rMutex;	}



private:
	// ��ȡ���н��̵�CPU������
	bool getAllProcCpuUsage();

	// ���ñ���ؽ��̵�ǰռ��CPU������
	void setCpuUsage( IN LONGLONG llTotalProcCPUDisp );

private:
	// ����������Ϣ����
	const SProcCfgInfo	*m_pCfgInfo;
	
	// �������̹������������
	VectorSingleProcMgr	m_vectSingleProc;


	// �߳�ͬ�����
	MUTEX m_rMutex;

public:

//#ifdef OS_WINDOWS
	// ���ݻ���
	char m_aBuffer[0x20000];

	// ����ռ��CPU��Ϣ
	typedef struct _SProcCpuTime
	{
		// ���̱�ʶ
		unsigned int uPID;
		// �Ƿ����
		bool bUpdata;

		// ������ռ��CPUʱ���
		LONGLONG llDiffKernalCPUTime;
		LONGLONG llDifftUsrCPUTime;

		// �ϴα�����ռ��CPUʱ��
		LONGLONG llLastKernalCPUTime;
		LONGLONG llLastUsrCPUTime;

	} SProcCpuTime;
	
	// ���н��̼������㱾����CPU��������
	typedef std::map< unsigned int, SProcCpuTime > MapProc;
	static MapProc m_mapAllProc;

	static bool getTotalCpuUsage(LONGLONG& llKernalTime,LONGLONG& llUsrTime);

	static const SProcCpuTime * FindProcCpu(unsigned int pid)
	{
		MapProc::iterator itr;
		itr = m_mapAllProc.find( pid);

		if( itr != m_mapAllProc.end() )
		{
			return &itr->second;
		}

		return NULL;
	}

//#endif


};

#endif // __PROBE_PROCESS_MANAGER_H__

