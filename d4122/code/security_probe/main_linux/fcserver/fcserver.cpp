// fcserver.cpp 

#include <stdio.h>
#include <iostream>
#include "cpf/processrunhelper.h"
#include "FCServerDlg.h"



int main_proc(CProcessRunHelper& runHelper)
{
	BOOL b_restore_orig_prg = FALSE;
	{
		const char key_for_restore_orig_prg[] = "restore_orig_prg";
		const char* pSelfName = runHelper.GetOpt()->pszSelfName;
		const char* pCmdLine = runHelper.m_strCmdLine.c_str();

		if( strstr(pSelfName, key_for_restore_orig_prg)
			|| strstr(pCmdLine, key_for_restore_orig_prg) 
			)
		{
			b_restore_orig_prg = true;
		}
	}


	// do ur self
	CFCServerDlg	dlg(b_restore_orig_prg, runHelper.m_strCmdLine.c_str());
	if (FALSE == dlg.OnInitDialog() )
	{
		printf("OnInitDialog error\n");
		CPF::fini();
		exit (0);
	}
	dlg.OnBnClickedButtonStart();


	if (!runHelper.IsService())	// �����ǰ̨���У������ʾ��Ϣ
	{
		printf("\n\n\n");
		printf("==============================================================================\n");
		printf("\n                      Welcome to use %s Debug ! \n", runHelper.GetOpt()->pszSelfName);
		printf("\n");
	}


	char chTemp;
	bool bStopEvent = false;
	while (runHelper.GetRunFlag() && !bStopEvent)	//  �ȴ��ź��˳�
	{
/*		if (!runHelper.IsService())	// ǰ̨���У�����ʾ����״̬
		{
			printf("------------------------------------------------------------------------------.\n\n");

			// �ȴ������˳���Ϣ
			printf("exit: x or y or q\n");
			std::cin>>chTemp;
			if (chTemp == 'y' || chTemp == 'x' || chTemp == 'q')
			{
				break;
			}
		}
*/
		sleep(1);

		dlg.OneSecondCall();
		bStopEvent = dlg.m_bStopEvent;
	}

	// ���������˳�����
	dlg.OnBnClickedButtonStopAndExit();

	return 0;
}




int main(int argc, char** argv)
{
	// ȷ������ģʽ
	CProcessRunHelper runHelper;
	if (runHelper.ParseCommand(argc, argv) <= 0)
		return 0;

	if(0 != runHelper.TryAcquire())
	{
		printf("%s is Running! [%d]\n", runHelper.GetOpt()->pszSelfName, errno);
		return 0;
	}

	CPF::init();

	main_proc(runHelper);
	
	CPF::fini();
	
	return 0;
}


