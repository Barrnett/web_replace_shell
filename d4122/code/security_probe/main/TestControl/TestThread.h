#pragma once
#include <ace/Task.h>
#include "ace/OS_NS_Thread.h"

class CTestThread :
	public ACE_Task_Base
{
public:
	CTestThread(void);
	virtual ~CTestThread(void);

public:
	int svc(void);

public:
	ACE_event_t 	m_MonitorEvent;
};
