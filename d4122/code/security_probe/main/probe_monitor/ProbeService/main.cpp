//////////////////////////////////////////////////////////////////////////
//main.cpp

#include "ace/streams.h"
#include "ace/OS_NS_errno.h"
#include "ace/Singleton.h"

#include "cpf/MyNTService.h"
#include "cpf/NTServiceCommonProcess.h"
#include "cpf/other_tools.h"
#include "ProbeService.h"

#include "cpf/ACE_Log_File_Msg.h"

#include "MonitorSvc.h"

//
//step1:
//



#define SERVER_NAME         ACE_TEXT("Probe_Monitor")
#define DESC_NAME			ACE_TEXT("Probe_Monitor")

//
//step2
//
NT_SERVICE_DEFINE(CMyNTService,DESC_NAME)



//
//step3
//

void log_and_print(ACE_Log_File_Msg& log_file,const char * info);

int
ACE_TMAIN (int argc, ACE_TCHAR* argv[])
{
	ACE::init();

	if( !CPF::TestProcMutex("CUnierm_CtrlApp.exe") )
	{
		printf("!!!!!!!!!!!!!!!�����Ѿ�����!!!!!!!!!!\n");

		ACE_OS::sleep(2);

		ACE::fini();

		return -1;
	}

	const char MPLAT_LOG_FILE_DIR[] = "log";

	{//����logĿ¼
		const char * pLogPath = CPF::GetModuleFullFileName(MPLAT_LOG_FILE_DIR);
		CPF::CreateFullDirecory(pLogPath);
	}

	ACE_Log_File_Msg	log_file;

	{//����log_instance
		ACE_TCHAR log_filename[MAX_PATH];
		CPF::GetModuleOtherFileName(log_filename, MAX_PATH,
			MPLAT_LOG_FILE_DIR,"probe_monitor.log");//hard coded

		log_file.init(log_filename,false,0);
	}
	

	PROCESS::instance()->parse_args(argc, argv);

	if( PROCESS::instance()->opt_start
		|| PROCESS::instance()->opt_debug)
	{
		log_and_print(log_file,"��س�������");
	}
	else if( PROCESS::instance()->opt_kill )
	{
		log_and_print(log_file,"ֹͣ��س���");
	}
	else if( PROCESS::instance()->opt_install )
	{
		log_and_print(log_file,"��װ������");
	}
	else if( PROCESS::instance()->opt_remove )
	{
		log_and_print(log_file,"ж�ط�����");
	}

	MonitorSvc* psvc = new MonitorSvc(PROCESS::instance()->opt_debug);

	char name[] = "MonitorService";

	CProbeService * pMyNtService = new CProbeService;
	pMyNtService->init(&log_file);
	pMyNtService->insert_subsvc(name, (CSubNTServiceBase*)psvc);

	PROCESS::instance()->init(&log_file,pMyNtService,SERVER_NAME,DESC_NAME);

    int nrtn = PROCESS::instance()->run (argc, argv);

	if( pMyNtService )
	{
		delete pMyNtService;
		pMyNtService = NULL;
	}
	if (psvc)
	{
		delete  psvc;
		psvc = NULL;
	}

	if( PROCESS::instance()->opt_start )
	{
		log_and_print(log_file,"��ط�����������ɹ�");

	}
	else if( PROCESS::instance()->opt_debug)
	{
		log_and_print(log_file,"��ط�������˳�");
	}
	else if( PROCESS::instance()->opt_kill )
	{
		log_and_print(log_file,"ֹͣ��س���---OK");
	}
	else if( PROCESS::instance()->opt_install )
	{
		log_and_print(log_file,"��װ������---OK");
	}
	else if( PROCESS::instance()->opt_remove )
	{
		log_and_print(log_file,"ж�ط�����---OK");
	}


	ACE::fini();

	ACE_OS::sleep(3);

	return nrtn;
}

void log_and_print(ACE_Log_File_Msg& log_file,const char * info)
{
	MY_ACE_DEBUG(&log_file,
		(LM_INFO,ACE_TEXT("\n\n\n#####################################################\n")));
	MY_ACE_DEBUG(&log_file,
		(LM_INFO,ACE_TEXT("%D:%s!!!!!!!!!!!!!!!!!!\n"),info));
	MY_ACE_DEBUG(&log_file,
		(LM_INFO,ACE_TEXT("\n#####################################################\n\n")));

	printf("\n\n\n#####################################################\n");
	printf("\n%s!!!!!!!!!!!!!!!!!!\n",info);
	printf("\n#####################################################\n\n");

	return;
}


