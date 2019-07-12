 
#pragma once

#include "ace/Shared_Memory_MM.h"
#include "ace/Time_Value.h"
#include "cpf/cpf.h"
#include "ace/OS_NS_Thread.h"

#ifdef OS_WINDOWS
#define my_event_t	ACE_event_t
#else
class CPF_CLASS my_event_t
{
public:
	my_event_t(void);
	virtual ~my_event_t(void);


	int event_init(
			int manual_reset,
			int initial_state,
			int type,
			const char *name);

	int event_destroy();
	int event_reset ();
	int event_signal ();

	int event_timedwait (ACE_Time_Value *timeout);
	int event_wait ();

public:
	int				m_manual_reset;
//	int				m_initial_state;
	int				m_type;
	std::string		m_strShareName;


	size_t			m_memsize;
	ACE_Shared_Memory_MM* m_pAceShareMem;
	char *			m_mem_buffer;

};
#endif



namespace MY_EVENT 
{
/*
#ifdef OS_WINDOWS
	CPF_CLASS int event_destroy (ACE_event_t *event);

	CPF_CLASS int event_init (ACE_event_t *event,
		int manual_reset,
		int initial_state,
		int type,
		const char *name);

	CPF_CLASS int event_pulse (ACE_event_t *event);

	CPF_CLASS int event_reset (ACE_event_t *event);

	CPF_CLASS int event_signal (ACE_event_t *event);

	CPF_CLASS int event_timedwait (ACE_event_t *event,
		ACE_Time_Value *timeout,
		int use_absolute_time = 1);

	CPF_CLASS int event_wait (ACE_event_t *event);
#else*/

	CPF_CLASS int event_destroy (my_event_t *event);

	CPF_CLASS int event_init (my_event_t *event,
								 int manual_reset,
								 int initial_state,
								 int type,
								 const char *name);

	CPF_CLASS int event_pulse (my_event_t *event);

	CPF_CLASS int event_reset (my_event_t *event);

	CPF_CLASS int event_signal (my_event_t *event);

	CPF_CLASS int event_timedwait (my_event_t *event,
									  ACE_Time_Value *timeout,
									  int use_absolute_time = 1);

	CPF_CLASS int event_wait (my_event_t *event);

	//在服务中创建事件,对于linux（实际上是直接调用event_init）
	CPF_CLASS int event_init_for_server(
		my_event_t *event,
		int manual_reset,
		int initial_state,
		int type,
		const char *name);
//#endif
} /* namespace MY_EVENT */
