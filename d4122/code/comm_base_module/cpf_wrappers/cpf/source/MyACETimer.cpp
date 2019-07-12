//////////////////////////////////////////////////////////////////////////
//MyACETimer.cpp

#include "cpf/MyACETimer.h"

CMyACETimer::CMyACETimer(void)
{
}

CMyACETimer::~CMyACETimer(void)
{
}

void CMyACETimer::stop_timer_thread()
{
	deactivate();

	wait();

	return;
}


void CMyACETimer::stop_and_remove_all_timer()
{
	deactivate();

	wait();

	timer_queue()->cancel(this);

	return;
}
