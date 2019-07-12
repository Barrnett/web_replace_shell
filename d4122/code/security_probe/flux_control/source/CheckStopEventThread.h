//////////////////////////////////////////////////////////////////////////
//CheckStopEventThread.h

#pragma once


#include <ace/Task.h>
#include "cpf/cpf.h"
#include "ace/OS_NS_Thread.h"
#include "cpf/my_event.h"

class CCheckStopEventThread : public ACE_Task_Base
{
public:
	CCheckStopEventThread(void);
	~CCheckStopEventThread(void);

public:
	void init(const char * stop_event_name);
	void start();
	void stop();

public:
	inline BOOL IsGetStopEvent() const
	{
		return m_bGetStopEvent;
	}

private:
	int	m_bstopping;

private:
	BOOL	m_bGetStopEvent;

private:
	virtual int svc(void);

private:
	my_event_t * Create_Stop_Event(const char *stop_event_name );

	int CheckStopEvent();

	my_event_t *		m_pStopEvent;




};
