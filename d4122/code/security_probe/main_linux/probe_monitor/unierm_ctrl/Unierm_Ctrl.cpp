/*
#include <stdio.h>
#include <stdlib.h>
#include "cpf/cpf.h"
#include "cpf/MyStr.h"
#include "cpf/path_tools.h"
#include "cpf/other_tools.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/VersionCompCheck.h"
#include "PacketIO/CommonMainCtrl.h"
#include "flux_control.h"
#include "ByPassManager/BypassTool.h"
#include "ace/Process_Mutex.h"
#include "ChannelFluxDlg.h"
*/

#include "cpf/processrunhelper.h"
#include "Unierm_CtrlDlg.h"



int main_proc(CProcessRunHelper& runHelper)
{
	// 初始化+运行
	CUnierm_CtrlDlg	dlg(runHelper.m_strCmdLine.c_str());
	if (FALSE == dlg.OnInitDialog() )
	{
		printf("OnInitDialog error\n");
		CPF::fini();
		exit (0);
	}

//	dlg.OnBnClickedButtonStart();


	// 控制
	if (!runHelper.IsService())	// 如果是前台运行，输出提示信息
	{
		printf("\n\n\n");
		printf("==============================================================================\n");
		printf("\n                      Welcome to use %s Debug ! \n", runHelper.GetOpt()->pszSelfName);
		printf("\n");
	}

	bool bStopEvent = false;
	while (runHelper.GetRunFlag() && !bStopEvent)	//  等待信号退出
	{
		{
			sleep(1);
			dlg.OneSecondCall();

			bStopEvent = dlg.m_bStopEvent;
		}
	}


	// 结束
	dlg.OnBnClickedCancel();

	return 0;
}



int main(int argc, char* argv[])
{
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

