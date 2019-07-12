//////////////////////////////////////////////////////////////////////////
//RestartTask.cpp

#include "stdafx.h"
#include "RestartTask.h"
#include "cpf/os_syscall.h"

CRestartTask::CRestartTask(void)
{
}

CRestartTask::~CRestartTask(void)
{
}

void CRestartTask::sleep_and_do(int ms,int type)
{
	m_type = type;
	m_ms = ms;
	activate();
}

int CRestartTask::svc(void)
{
	sleep_wait();

	if( m_type == 0 )
	{//�˳���������

		exit(-1);
	}
	else if( m_type == 1)
	{//������������

		CPF::MySystemShutdown(0x02);
	}
	else if( m_type == 2)
	{//�ر��豸
		CPF::MySystemShutdown(0x08);
	}

	return 0;

}

void CRestartTask::sleep_wait()
{
	ACE_Time_Value ts;

	ts.msec(m_ms);

	ACE_OS::sleep(ts);

	return;
}

