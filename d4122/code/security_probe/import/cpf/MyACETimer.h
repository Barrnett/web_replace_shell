//////////////////////////////////////////////////////////////////////////
//MyACETimer.h

#pragma once

#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"
#include "cpf/cpf.h"

class CPF_CLASS CMyACETimer : public ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap>
{
public:
	CMyACETimer(void);
	virtual ~CMyACETimer(void);

public:
	void stop_timer_thread();

	void stop_and_remove_all_timer();

};
