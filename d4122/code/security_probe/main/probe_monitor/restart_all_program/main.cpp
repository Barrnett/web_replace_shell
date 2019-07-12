// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cpf/other_tools.h"
#include "ace/OS_main.h"
#include "restart_all_program.h"

int DoRestart();

int
ACE_TMAIN (int argc, ACE_TCHAR* argv[])
{
	ACE::init();

	if( !CPF::TestProcMutex("restart_all_program") )
	{
		printf("!!!!!!!!!!!!!!!程序已经启动!!!!!!!!!!\n");

		ACE_OS::sleep(2);

		ACE::fini();

		return -1;
	}

	DoRestart();

	ACE_OS::sleep(3);

	ACE::fini();


	return 0;
}

int DoRestart()
{
	Crestart_all_program restart_tool;

	if( !restart_tool.init() )
		return -1;

	//先关闭以前的程序
	printf("\n开始关闭程序....\n");

	restart_tool.StopOldProgram();

	printf("\n程序已经关闭....\n");

	//启动程序
	printf("\n开始启动程序....\n");
	
	restart_tool.StartProgram();

	printf("\n启动程序完成....\n");

	restart_tool.close();

	return 0;
}

