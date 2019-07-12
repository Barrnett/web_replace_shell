#include "StdAfx.h"
#include "TestThread.h"
#include "ace/OS_NS_unistd.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "cpf/path_tools.h"

CTestThread::CTestThread(void)
{
	ACE_OS::event_init(&m_MonitorEvent,0,0,0,"myp_lat_run_ctrl");
	
}

CTestThread::~CTestThread(void)
{
	ACE_OS::event_destroy(&m_MonitorEvent);
}


int CTestThread::svc(void)
{
	/*while( 1 )
	{
		double d = 0.2;

		for(int i = 0; i < 1000; ++i)
		{
			d = d*(double)i;
		}

		ACE_OS::event_signal(&m_MonitorEvent);
	}*/

	static int index = 0;

	ACE_Log_File_Msg log_msg;

	char buf[10];

	sprintf(buf,"%d.log",++index);

	log_msg.init(CPF::GetModuleFullFileName(buf),true,0);

	ACE_Time_Value stime(0,1*1000);

	const int nums = 100;


	while( 1 )
	{
		DWORD dwtotal = 0;
		for( int i = 0; i < nums; ++i)
		{
			ACE_Time_Value t1 = ACE_OS::gettimeofday();

			//ACE_OS::event_timedwait(&m_thread.m_MonitorEvent,&stime,0);
			//ACE_OS::event_reset(&m_thread.m_MonitorEvent);
			ACE_OS::sleep(stime);

			dwtotal += (DWORD)(ACE_OS::gettimeofday() - t1).msec();
		}

		MY_ACE_DEBUG(&log_msg,
			(LM_INFO,ACE_TEXT("empty time=%d(ms),empty nums = %d,av = %.2f\n"),dwtotal,nums,(double)dwtotal/nums));

	}

	int k = 0;

	return 0;
}
