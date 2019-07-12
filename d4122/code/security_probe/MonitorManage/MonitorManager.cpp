#include "stdafx.h"

#include "MonitorManager.h"
#include "ace/ACE.h"
#include "ace/Guard_T.h"
#include "ace/Process.h"
#include "ace/Log_Msg.h"



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CMonitorManager::CMonitorManager(BOOL bDebug,ACE_Log_Msg *pLog) : m_Mutex()
{
	m_bDebug = bDebug;

	m_pLogInstance = pLog;
}

CMonitorManager::~CMonitorManager()
{
	ReleaseAll();

	return;
}

CMonitorTask * CMonitorManager::FindMonitorTask(const char * name)
{
	std::vector<CMonitorTask *>::iterator iter;
	
	for(iter=m_vt_MonitorTask.begin();iter!=m_vt_MonitorTask.end();iter++)
	{	
		CMonitorTask * pMonitorTask= *iter;

		if( 0 == stricmp(pMonitorTask->GetTaskInfo().szProcessName,name) )
		{
			return pMonitorTask;
		}
	}

	return NULL;
}

int CMonitorManager::AddMonitorTask(CMonitorTask * pMonitorTask)
{	
	if( FindMonitorTask(pMonitorTask->GetTaskInfo().szProcessName) )
	{
		return -1;
	}

	m_vt_MonitorTask.push_back(pMonitorTask);
	
	return 0;
}

int CMonitorManager::AddMonitorTasks(const std::vector<MONITOR_TASK>& vTask)
{
	std::vector<MONITOR_TASK>::const_iterator iter=vTask.begin();

	for (; iter!=vTask.end(); iter++)
	{
		CMonitorTask * pMonitorTask = NULL;


		if( ACE_OS::strcmp(iter->sztask_type,"event") == 0 )
		{
			pMonitorTask=new CMonitorTask_Event();
		}
		else
		{
			pMonitorTask = new CMonitorTask_Common();
		}

		if(pMonitorTask)
		{
			pMonitorTask->init(m_bDebug,(*iter),m_pLogInstance);

			AddMonitorTask(pMonitorTask);
		}
	}

	return 0;
}

void CMonitorManager::GetMonitorTaskInfo(std::vector<MONITOR_TASK>& vTask)
{
	vTask.reserve(m_vt_MonitorTask.size());

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);

	std::vector<CMonitorTask *>::iterator iter=m_vt_MonitorTask.begin();

	while(iter!=m_vt_MonitorTask.end())
	{
		CMonitorTask * pMonitorTask=(*iter);

		vTask.push_back(pMonitorTask->GetTaskInfo());

		iter++;
	}

	return;
}

int CMonitorManager::DeleteMonitorTask(const char * name)
{
	std::vector<CMonitorTask *>::iterator iter;

	for(iter=m_vt_MonitorTask.begin();iter!=m_vt_MonitorTask.end();iter++)
	{	
		CMonitorTask * pMonitorTask= *iter;

		if( 0 == stricmp(pMonitorTask->GetTaskInfo().szProcessName,name) )
		{
			delete pMonitorTask;

			m_vt_MonitorTask.erase(iter);

			return 0;
		}
	}

	return -1;
}

int CMonitorManager::OnTimer(const ACE_Time_Value &current_time)
{
	int nRet;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);

	std::vector<CMonitorTask *>::iterator iter=m_vt_MonitorTask.begin();
	while(iter!=m_vt_MonitorTask.end())
	{
		nRet=(*iter)->OnTimer(current_time);

		if(nRet==-1) 
		{
			printf("The event of stop tantalos had signal,The CMonitorManager's OnTimer will return!\n");
			
			return -1;
		}

		iter++;
	}

	return 0;
}

int CMonitorManager::Restart(const char * name)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);

	CMonitorTask * pMonitorTask=FindMonitorTask(name);

	if(pMonitorTask==NULL) return -1;	

	if( pMonitorTask->ChangeConfigState(0,1) != 0 )
		return -1;

	ACE_OS::sleep(1);

	if( pMonitorTask->ChangeConfigState(1,1) != 0 )
		return -1;

	return 0;
}

int CMonitorManager::ChangeConfigState(const char * name,int nState,BOOL bTempStat)
{	
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);

	CMonitorTask * pMonitorTask=FindMonitorTask(name);
	if(pMonitorTask==NULL) return -1;	
	
	return pMonitorTask->ChangeConfigState(nState,bTempStat);
}

//-1表示失败
int CMonitorManager::GetConfigState(const char * name,int& nState,BOOL bTempStat)
{
//#ifdef OS_WINDOWS
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);
//#endif

	CMonitorTask * pMonitorTask=FindMonitorTask(name);
	if(pMonitorTask==NULL) return -1;	

	nState = pMonitorTask->GetConfigState(bTempStat);

	return 0;
}

//-1表示失败
int CMonitorManager::GetTaskRunState(const char * name,int& nState)
{
//#ifdef OS_WINDOWS
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);
//#endif
	CMonitorTask * pMonitorTask=FindMonitorTask(name);
	if(pMonitorTask==NULL) return -1;	

	nState = pMonitorTask->IsTaskExist();

	return 0;
}

void CMonitorManager::ReleaseAll()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);

	std::vector<CMonitorTask *>::iterator iter;
	
	//释放所有的内存空间
	for(iter=m_vt_MonitorTask.begin();iter!=m_vt_MonitorTask.end();iter++)
	{
		CMonitorTask * pMonitorTask=*iter;

		delete pMonitorTask;		
	}

	m_vt_MonitorTask.clear();
	
	return;
}

void CMonitorManager::StopAll(BOOL bTempStat)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);

	MY_ACE_DEBUG(m_pLogInstance,
		         (LM_DEBUG, ACE_TEXT( "[Info][%D]StopAll(%d)\n" ),bTempStat));

	std::vector<CMonitorTask *>::iterator iter;

	for(iter=m_vt_MonitorTask.begin();iter!=m_vt_MonitorTask.end();iter++)
	{
		CMonitorTask * pTask = (*iter);

		pTask->ChangeConfigState(0,bTempStat);
	}

	return;
}

void CMonitorManager::StartAll(BOOL bTempStat)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Mutex);

	MY_ACE_DEBUG(m_pLogInstance,
		(LM_DEBUG, ACE_TEXT( "[Info][%D]StartAll(%d)\n" ),bTempStat));

	std::vector<CMonitorTask *>::iterator iter;

	for(iter=m_vt_MonitorTask.begin();iter!=m_vt_MonitorTask.end();iter++)
	{
		CMonitorTask * pTask = (*iter);

		pTask->ChangeConfigState(1,bTempStat);
	}

	return;
}

