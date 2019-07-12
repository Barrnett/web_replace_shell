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
		printf("!!!!!!!!!!!!!!!�����Ѿ�����!!!!!!!!!!\n");

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

	//�ȹر���ǰ�ĳ���
	printf("\n��ʼ�رճ���....\n");

	restart_tool.StopOldProgram();

	printf("\n�����Ѿ��ر�....\n");

	//��������
	printf("\n��ʼ��������....\n");
	
	restart_tool.StartProgram();

	printf("\n�����������....\n");

	restart_tool.close();

	return 0;
}

