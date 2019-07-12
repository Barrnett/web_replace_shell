//////////////////////////////////////////////////////////////////////////
//ProbeService.cpp

#include "ProbeService.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "ace/Reactor.h"
#include "cpf/path_tools.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/my_event.h"

CProbeService::CProbeService(void)
{
	m_stopping = 0;
	m_stopped = 0;

	m_pStopEvent = NULL;
	m_pMonitorEvent = NULL;

	reactor (ACE_Reactor::instance ());
}

CProbeService::~CProbeService(void)
{
	ACE_Reactor::instance ()->cancel_timer(this);

	return;

}

int CProbeService::init(ACE_Log_Msg * pLogInstance)
{
	m_pLogInstance = pLogInstance;

	MY_ACE_DEBUG(m_pLogInstance,
			(LM_ERROR,
			ACE_TEXT("\n\n\n###############################################################\n")
			));

	MY_ACE_DEBUG(m_pLogInstance,
		(LM_ERROR,
		ACE_TEXT("\n%D:Unierm_Ctrl服务程序开始启动!!!!!!!!!!!!!!!!!!!\n")
		));

	MY_ACE_DEBUG(m_pLogInstance,
		(LM_ERROR,
		ACE_TEXT("\n###############################################################\n\n\n")
		));


	ReadConfig();

	return 0;
}

BOOL CProbeService::ReadConfig()
{
	CTinyXmlEx xml_reader;

	{
		char xml_file_name[MAX_PATH] = {0};

		CPF::GetModuleOtherFileName(xml_file_name,sizeof(xml_file_name),"ini","ProbeService.xml");

		if( 0 != xml_reader.open(xml_file_name) )
		{
			ACE_ASSERT(FALSE);
			return FALSE;
		}
	}

	TiXmlNode * pcommon_node = xml_reader.GetRootNode()->FirstChildElement("common");

	if( !pcommon_node )
	{
		ACE_ASSERT(FALSE);
		return FALSE;
	}

	std::string strCmdLine;

	xml_reader.GetValueLikeIni(pcommon_node,"cmd_line",strCmdLine);

	{
		char monitor_event_name[256] = {0};
		char stop_event_name[256] = {0};
		int  monitor_event_interval = 0;
		int  stop_event_interval = 0;

		int nrtn = sscanf(strCmdLine.c_str(),"%s %s %d %d",
			monitor_event_name,stop_event_name,
			&monitor_event_interval,&stop_event_interval );

		m_pStopEvent = Create_Stop_Event(NULL,stop_event_name);

		{
			monitor_event_interval = monitor_event_interval/3;

			if( monitor_event_interval <= 0 )
				monitor_event_interval = 3;

			if( monitor_event_name && strlen(monitor_event_name) > 0 )
			{
				m_pMonitorEvent = new ACE_event_t;

				if( -1 == MY_EVENT::event_init_for_server(m_pMonitorEvent,0,1,0,monitor_event_name) )
				{
					delete m_pMonitorEvent;
					m_pMonitorEvent = NULL;
				}			
			}

		}
	}

	return TRUE;

}

void
CProbeService::handle_control (DWORD control_code)
{
	if (control_code == SERVICE_CONTROL_SHUTDOWN
		|| control_code == SERVICE_CONTROL_STOP)
	{
		report_status (SERVICE_STOP_PENDING);

		MY_ACE_DEBUG (m_pLogInstance,
			(LM_INFO,
			ACE_TEXT ("Service control stop requested\n")));
		
		m_stopping = 1;

		reactor ()->notify (this,
			ACE_Event_Handler::EXCEPT_MASK);

		//等待线程结束
		while( !m_stopped )
		{
			ACE_ASSERT(m_stopping);

			ACE_OS::sleep(1);
		}		
	}
	else
	{
		inherited::handle_control (control_code);
	}
}

// This is just here to be the target of the notify from above... it
// doesn't do anything except aid on popping the reactor off its wait
// and causing a drop out of handle_events.

int
CProbeService::handle_exception (ACE_HANDLE)
{
	return 0;
}

// Beep every two seconds.  This is what this NT service does...

int
CProbeService::handle_timeout (const ACE_Time_Value &tv,
						 const void *)
{
	CheckStopEvent();

	if( m_pMonitorEvent )
	{
		ACE_OS::event_signal(m_pMonitorEvent);
	}

	return 0;
}


// This is the main processing function for the Service.  It sets up
// the initial configuration and runs the event loop until a shutdown
// request is received.

int
CProbeService::svc (void)
{
	MY_ACE_DEBUG (m_pLogInstance,(LM_DEBUG,
		ACE_TEXT ("%D:CProbeService::svc CProbeService begin running\n")));

	//// As an NT service, we come in here in a different thread than the
	//// one which created the reactor.  So in order to do anything, we
	//// need to own the reactor. If we are not a service, report_status
	//// will return -1.
	if (report_status (SERVICE_RUNNING) == 0)
		reactor ()->owner (ACE_Thread::self ());

	m_stopping = 0;
	m_stopped = 0;

	{

		//// Schedule a timer every two seconds.
		ACE_Time_Value tv (1, 0);
		ACE_Reactor::instance ()->schedule_timer (this, 0, tv, tv);

		while (!this->m_stopping)
		{
			reactor ()->handle_events ();
		}	
	}

	// Cleanly terminate connections, terminate threads.
	MY_ACE_DEBUG (m_pLogInstance,(LM_DEBUG,
		ACE_TEXT ("Shutting down\n")));

	stop_all_sub_service();

	reactor ()->cancel_timer (this);
	
	m_stopped = 1;

	if( m_pStopEvent )
	{
		ACE_OS::event_destroy(m_pStopEvent);
		delete m_pStopEvent;
		m_pStopEvent = NULL;
	}

	if( m_pMonitorEvent )
	{
		ACE_OS::event_destroy(m_pMonitorEvent);
		delete m_pMonitorEvent;
		m_pMonitorEvent = NULL;
	}

	return 0;
}


ACE_event_t * CProbeService::Create_Stop_Event(ACE_Log_File_Msg * loginstance,const char *stop_event_name )
{
	if( stop_event_name && strlen(stop_event_name) > 0 )
	{
		ACE_event_t * event = new ACE_event_t;

		if( -1 == MY_EVENT::event_init_for_server(event,true,0,0,stop_event_name) )
		{
			MY_ACE_DEBUG(loginstance,
				(LM_INFO,
				ACE_TEXT("stop_event 创建失败!\n")
				));

			printf("stop_event 创建失败\n");
			delete event;
			return NULL;
		}
		else
		{
			MY_ACE_DEBUG(loginstance,
				(LM_INFO,
				ACE_TEXT("创建stop_event : '%s' 成功!\n"),stop_event_name
				));

			printf("创建stop_event : '%s' 成功!\n\n",stop_event_name);

			return event;
		}
	}
	else
	{
		MY_ACE_DEBUG(loginstance,
			(LM_INFO,
			ACE_TEXT("stop_event的名称为空，没有创建stop_event!\n")
			));

		printf("stop_event的名称为空，没有创建stop_event!\n\n");

		return NULL;
	}

	return NULL;
}

int CProbeService::CheckStopEvent()
{
	if( !m_pStopEvent )
	{
		return -1;
	}

	ACE_Time_Value time_wait(0,0);

	int nRet = ACE_OS::event_timedwait(m_pStopEvent, &time_wait);

	if( 0 == nRet )
	{
		if( m_pStopEvent )
		{
			ACE_OS::event_destroy(m_pStopEvent);
			delete m_pStopEvent;
			m_pStopEvent = NULL;
		}

		if( m_pMonitorEvent )
		{
			ACE_OS::event_destroy(m_pMonitorEvent);
			delete m_pMonitorEvent;
			m_pMonitorEvent = NULL;
		}

		m_stopping = 1;

		return 0;
	}

	return -1;
}
