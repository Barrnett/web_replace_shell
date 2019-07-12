//////////////////////////////////////////////////////////////////////////
//SetSysTime_And_RestartTask.cpp

#include "stdafx.h"
#include "SetSysTime_And_RestartTask.h"
#include "MonitorManager.h"

CSetSysTime_And_RestartTask::CSetSysTime_And_RestartTask(void)
{
	m_pMonitorManager = NULL;
}

CSetSysTime_And_RestartTask::~CSetSysTime_And_RestartTask(void)
{
}

void CSetSysTime_And_RestartTask::Init(CMonitorManager * pMonitorManager
										   ,const SYSTEMTIME& sys_time)
{
	m_pMonitorManager = pMonitorManager;

	memcpy(&m_sys_time,&sys_time,sizeof(sys_time));

	return;
}


void CSetSysTime_And_RestartTask::sleep_wait()
{
	ACE_UINT32 begin_time = GetTickCount();

	//Í£Ö¹³ÌÐò
	m_pMonitorManager->StopAll(1);

	SetProbeSysTime(m_sys_time);

	ACE_UINT32 end_time = GetTickCount();

	ACE_UINT32 spend_time = (end_time-begin_time);

	if( spend_time < (ACE_UINT32)m_ms )
	{
		ACE_Time_Value ts;

		ts.msec((long)(m_ms-spend_time));

		ACE_OS::sleep(ts);
	}

	return;

}

int CSetSysTime_And_RestartTask::SetProbeSysTime(const SYSTEMTIME& sys_time)
{
#ifdef OS_WINDOWS

	BOOL bOK = SetLocalTime(&m_sys_time);

	if( bOK )
		return 0;
	else
		return -1;

#else
	char szTime[MAX_PATH];
	sprintf(szTime, "date --set \"%02d/%02d/%02d %02d:%02d:%02d\"", 
		sys_time.wMonth, sys_time.wDay, sys_time.wYear,
		sys_time.wHour, sys_time.wMinute, sys_time.wSecond);
//	printf("%s\n", szTime);
	system(szTime);
	system("hwclock -w");
//	ACE_ASSERT(false);
	return 0;
#endif

}
