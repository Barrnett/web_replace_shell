//////////////////////////////////////////////////////////////////////////
//CheckStopEventThread.cpp

#include "stdafx.h"
#include "CheckStopEventThread.h"
#include "cpf/my_event.h"

CCheckStopEventThread::CCheckStopEventThread(void)
{
	m_pStopEvent = NULL;

	m_bGetStopEvent = false;

}

CCheckStopEventThread::~CCheckStopEventThread(void)
{
	if( m_pStopEvent )
	{
		MY_EVENT::event_destroy(m_pStopEvent);
		delete m_pStopEvent;
		m_pStopEvent = NULL;
	}
}

void CCheckStopEventThread::init(const char * stop_event_name)
{
	m_pStopEvent = Create_Stop_Event(stop_event_name);	

	return;
}


void CCheckStopEventThread::start()
{
	m_bGetStopEvent = false;

	m_bstopping = FALSE;

	activate();

	return;
}


void CCheckStopEventThread::stop()
{
	m_bstopping = true;

	wait();

	return;
}

int CCheckStopEventThread::svc()
{
	int i = 0;

	while( !m_bstopping )
	{
		ACE_OS::sleep( ACE_Time_Value(1) );

		if( i% 2== 0 )
		{
			if( CheckStopEvent() == 0 )
			{
				m_bGetStopEvent = true;
				break;
			}
		}
	}

	return 0;
}

int CCheckStopEventThread::CheckStopEvent()
{
	if( !m_pStopEvent )
	{
		return -1;
	}

	ACE_Time_Value time_wait(0,0);

	int nRet = MY_EVENT::event_timedwait(m_pStopEvent, &time_wait);

	if( 0 == nRet )
	{
		MY_EVENT::event_destroy(m_pStopEvent);
		delete m_pStopEvent;
		m_pStopEvent = NULL;
	}

	return nRet;

}

my_event_t * CCheckStopEventThread::Create_Stop_Event(const char *stop_event_name )
{
	if( stop_event_name && strlen(stop_event_name) > 0 )
	{
		my_event_t * event = new my_event_t;

		if( -1 == MY_EVENT::event_init_for_server(event,true,0,0,stop_event_name) )
		{
			delete event;
			return NULL;
		}
		else
		{
			return event;
		}
	}
	else
	{
		return NULL;
	}

	return NULL;
}

