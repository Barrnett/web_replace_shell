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


	if (!runHelper.IsService())	// 如果是前台运行，输出提示信息
	{
		printf("\n\n\n");
		printf("==============================================================================\n");
		printf("\n                      Welcome to use %s Debug ! \n", runHelper.GetOpt()->pszSelfName);
		printf("\n");
	}


	char chTemp;
	bool bStopEvent = false;
	while (runHelper.GetRunFlag() && !bStopEvent)	//  等待信号退出
	{
/*		if (!runHelper.IsService())	// 前台运行，则显示运行状态
		{
			printf("------------------------------------------------------------------------------.\n\n");

			// 等待输入退出消息
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

	// 到这里是退出部分
	dlg.OnBnClickedButtonStopAndExit();

	return 0;
}




int main(int argc, char** argv)
{
	// 确定运行模式
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


