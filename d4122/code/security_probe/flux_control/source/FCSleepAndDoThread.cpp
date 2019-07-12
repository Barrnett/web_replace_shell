//////////////////////////////////////////////////////////////////////////
//FCSleepAndDoThread.cpp

#include "stdafx.h"
#include "FCSleepAndDoThread.h"
#include "FluxControlObserver.h"

CFCSleepAndDoThread::CFCSleepAndDoThread(int type,int reason)
:m_type(type),m_reason(reason)
{
}

CFCSleepAndDoThread::~CFCSleepAndDoThread(void)
{
}

int CFCSleepAndDoThread::DoFunc()
{
	g_pFluxControlObserver->RebootHandler(m_type,m_reason);

	return 0;
}

