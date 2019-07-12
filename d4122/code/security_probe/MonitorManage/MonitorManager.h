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

	//强制启动或者停止，并且m_taskInfo.nRunConfigState改变。
	//bTempStat如果为0，则m_taskInfo.nPermanentConfigState也改变
	//但是配置没有写入xml文件。写入XML的工作，由外面来完成
	int ChangeConfigState(const char * name,int nState,BOOL bTempStat);

	//-1表示失败
	int GetConfigState(const char * name,int& nState,BOOL bTempStat);

	//-1表示失败
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