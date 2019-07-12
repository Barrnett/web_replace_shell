#ifndef _MONITOR_MANAGER_H
#define _MONITOR_MANAGER_H
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifdef OS_WINDOWS
#pragma warning(disable:4786)
#endif
#include <string>
#include <vector>
#include <map>
#include "MonitorMgrDllMacro.h"
#include "MonitorTask.h"
#include "ace/Recursive_Thread_Mutex.h"


class MONITOR_MANAGER_CLASS CMonitorManager
{
public:
	CMonitorManager(BOOL bDebug,ACE_Log_Msg *);
	~CMonitorManager();
	
public:
	int OnTimer(const ACE_Time_Value &current_time);

	//ǿ����������ֹͣ������m_taskInfo.nRunConfigState�ı䡣
	//bTempStat���Ϊ0����m_taskInfo.nPermanentConfigStateҲ�ı�
	//��������û��д��xml�ļ���д��XML�Ĺ����������������
	int ChangeConfigState(const char * name,int nState,BOOL bTempStat);

	//-1��ʾʧ��
	int GetConfigState(const char * name,int& nState,BOOL bTempStat);

	//-1��ʾʧ��
	int GetTaskRunState(const char * name,int& nState);
	
	int Restart(const char * name);

	void ReleaseAll();

	void StopAll(BOOL bTempStat);
	void StartAll(BOOL bTempStat);

	CMonitorTask * FindMonitorTask(const char * name);
	int AddMonitorTask(CMonitorTask * pMonitorTask);
	int AddMonitorTasks(const std::vector<MONITOR_TASK>& vTask);
	int DeleteMonitorTask(const char * name);

	void GetMonitorTaskInfo(std::vector<MONITOR_TASK>& vTask);

private:
	std::vector<CMonitorTask *>	m_vt_MonitorTask;

	ACE_Log_Msg *m_pLogInstance;

	ACE_Recursive_Thread_Mutex m_Mutex; 

	BOOL			m_bDebug;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif