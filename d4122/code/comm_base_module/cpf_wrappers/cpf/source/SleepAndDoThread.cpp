//////////////////////////////////////////////////////////////////////////
//SleepAndDoThread.cpp

#include "cpf/SleepAndDoThread.h"
#include "ace/OS_NS_unistd.h"

CSleepAndDoThread::CSleepAndDoThread(void)
{
}

CSleepAndDoThread::~CSleepAndDoThread(void)
{
}

void CSleepAndDoThread::sleep_and_do(int ms)
{
	m_ms = ms;

	activate();

	return;
}

void CSleepAndDoThread::Sleep_Wait()
{
	if( m_ms != 0 )
	{
		ACE_Time_Value ts;
		
		ts.msec(m_ms);
		
		ACE_OS::sleep(ts);
	}
}


int CSleepAndDoThread::svc(void)
{
	Sleep_Wait();

	DoFunc();

	return 0;

}



//////////////////////////////////////////////////////////////////////////
//CRebootTask

#include "cpf/os_syscall.h"

CRebootTask::CRebootTask(int type)
:m_type(type)
{
}

CRebootTask::~CRebootTask(void)
{
}

int CRebootTask::DoFunc()
{
	if( m_type == EXIT_PORCESS )
	{//退出整个程序

		exit(-1);
	}
	else if( m_type == REBOOT_COMPUTER)
	{//重新启动机器

		CPF::MySystemShutdown(0x02);
	}
	else if( m_type == POWEROFF_COMPUTER)
	{//关闭设备
		CPF::MySystemShutdown(0x08);
	}

	return 0;

}

