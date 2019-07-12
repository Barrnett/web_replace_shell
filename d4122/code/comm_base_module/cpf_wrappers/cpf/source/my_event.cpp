//////////////////////////////////////////////////////////////////////////
//my_event_t.h 

#include "cpf/my_event.h"

#ifndef OS_WINDOWS

my_event_t::my_event_t(void)
{
	m_memsize = 1024;
	m_mem_buffer = NULL;
	m_pAceShareMem = NULL;

	m_manual_reset = 0;
//	m_initial_state = 0,
	m_type = 0;
}

my_event_t::~my_event_t(void)
{
	event_destroy();
	return;
}

// 与ace的不同，initial_state < 0 表示不进行初始化
int my_event_t::event_init(
						  int manual_reset,
						  int initial_state,
						  int type,
						  const char *name)
{
	if (!name) return -1;

	m_manual_reset = manual_reset;
//	m_initial_state = initial_state;
	m_type = type,
	m_strShareName = "/dev/shm/";
	m_strShareName += name;

	int first_time = 0;

	if (NULL != m_pAceShareMem) return 1;

	m_pAceShareMem = new ACE_Shared_Memory_MM();
	if (-1 == m_pAceShareMem->open(m_strShareName.c_str(), -1, O_RDWR))
	{
		delete m_pAceShareMem;
		m_pAceShareMem = new ACE_Shared_Memory_MM(m_strShareName.c_str(), m_memsize);
		first_time = 1;
	}

	if (NULL != m_pAceShareMem)
	{
		m_mem_buffer = (char *)m_pAceShareMem->malloc();
		if(m_mem_buffer == NULL) // add by xumx, 2016-10-19
		{
			delete m_pAceShareMem;
			m_pAceShareMem = NULL;
			return(-2); 
		}
	}

//	if (first_time)
	if (initial_state >= 0)
	{
		*m_mem_buffer = initial_state ? 1 : 0;
	}
	return 0;
}

//相当于CloseDataStream+fini
int my_event_t::event_destroy()
{
	#if 1 // add by xumx, 2016-10-19
	if(m_pAceShareMem != NULL)
	{
		if(m_mem_buffer != NULL){ m_pAceShareMem->free(m_mem_buffer); }
		m_pAceShareMem->close();
		delete m_pAceShareMem;
	}
	#else
	if( m_pAceShareMem && m_mem_buffer )
	{	
		m_pAceShareMem->free(m_mem_buffer);
		m_pAceShareMem->close();
		delete m_pAceShareMem;
	}
	#endif

	m_pAceShareMem = NULL;
	m_mem_buffer = NULL;

	return 0;
}

int my_event_t::event_reset ()
{
	if (m_mem_buffer)
	{
		*m_mem_buffer = 0;
	}
	return 0;
}
int my_event_t::event_signal ()
{
	if (m_mem_buffer)
	{
		*m_mem_buffer = 1;
	}
	return 0;
}

int my_event_t::event_timedwait (ACE_Time_Value *timeout)
{
	if (NULL == m_mem_buffer)
		return -1;

	ACE_Time_Value tvSleep(0, 1000);
	ACE_Time_Value tvPassed(0, 0);

	// 循环等到标记为1
	while(*m_mem_buffer == 0) 
	{
		if (timeout)
		{
			if (tvPassed >= *timeout)
			return -2;
		}
		ACE_OS::sleep(tvSleep);
		tvPassed += tvSleep;
	}

	// 自动清除
	if (m_manual_reset)
	{
		*m_mem_buffer = 0;
	}
	return 0;
}

int my_event_t::event_wait ()
{
	ACE_Time_Value max_time = ACE_Time_Value::max_time;
	return event_timedwait(&max_time);

	/*
	if (NULL == m_mem_buffer)
		return -1;

	ACE_Time_Value tv;
	tv.usec(1000);

	// 循环等到标记为1
	while(*m_mem_buffer == 0) 
	{
		ACE_OS::sleep(tv);
	}

	// 自动清除
	if (m_manual_reset)
	{
		*m_mem_buffer = 0;
	}
	return 0;
	*/
}
#endif


//////////////////////////////////////////////////////////////////////////
namespace MY_EVENT 
{
#ifdef OS_WINDOWS
	int event_destroy (my_event_t *event)
	{	return ACE_OS::event_destroy(event);	}

	int event_init (my_event_t *event,
		int manual_reset,
		int initial_state,
		int type,
		const char *name)
	{	return ACE_OS::event_init(event, manual_reset, initial_state, type, name);	}

	int event_pulse (my_event_t *event)
	{	return ACE_OS::event_pulse(event);	}

	int event_reset (my_event_t *event)
	{	return ACE_OS::event_reset(event);	}

	int event_signal (my_event_t *event)
	{	return ACE_OS::event_signal(event);	}

	int event_timedwait (my_event_t *event,
		ACE_Time_Value *timeout,
		int use_absolute_time)
	{	return ACE_OS::event_timedwait(event, timeout, use_absolute_time);	}

	int event_wait (my_event_t *event)
	{	return ACE_OS::event_wait(event);	}

#else


	int event_destroy (my_event_t *event)
	{
		return event->event_destroy();
	}

	int event_init (my_event_t *event,
		int manual_reset,
		int initial_state,
		int type,
		const char *name)
	{
		return event->event_init(manual_reset, initial_state, type, name);
	}

	int event_pulse (my_event_t *event)
	{
		return -1;
	}

	int event_reset (my_event_t *event)
	{
		return event->event_reset();
	}

	int event_signal (my_event_t *event)
	{
		return event->event_signal();
	}

	int event_timedwait (my_event_t *event,
		ACE_Time_Value *timeout,
		int use_absolute_time)
	{
		return event->event_timedwait(timeout);
	}

	int event_wait (my_event_t *event)
	{
		return event->event_wait();
	}

#endif


	int event_init_for_server(my_event_t *event,
		int manual_reset,
		int initial_state,
		int type,
		const char *name)
	{
#ifdef OS_WINDOWS

		char full_name[MAX_PATH];

		sprintf(full_name,"Global\\%s",name);

		SECURITY_DESCRIPTOR SecurityDescriptor = { 0 };
		::InitializeSecurityDescriptor(&SecurityDescriptor, 1);
		::SetSecurityDescriptorDacl(&SecurityDescriptor, TRUE, NULL, FALSE);
		SECURITY_ATTRIBUTES SecurityAttribute = { 0 };
		SecurityAttribute.nLength               = sizeof(SecurityAttribute);
		SecurityAttribute.lpSecurityDescriptor = &SecurityDescriptor;
		SecurityAttribute.bInheritHandle        = TRUE;

		return ACE_OS::event_init(event,manual_reset,initial_state,type,full_name,NULL,&SecurityAttribute);
#else

		return MY_EVENT::event_init(event,manual_reset,initial_state,type,name);
#endif
	}

} /* namespace MY_EVENT */

