//////////////////////////////////////////////////////////////////////////
//SetSysTime_And_RestartTask.h

#pragma once

#include "cpf/ostypedef.h"
#include "RestartTask.h"

class CMonitorManager;

class CSetSysTime_And_RestartTask : public CRestartTask
{
public:
	CSetSysTime_And_RestartTask();
	virtual ~CSetSysTime_And_RestartTask();

public:
	void Init(CMonitorManager * pMonitorManager,const SYSTEMTIME& sys_time);

private:
	virtual void sleep_wait();

private:
	int SetProbeSysTime(const SYSTEMTIME& sys_time);


private:
	CMonitorManager * m_pMonitorManager;
	SYSTEMTIME			m_sys_time;

};

