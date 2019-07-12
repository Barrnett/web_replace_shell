#include <stdio.h>
#include "TestControlDlg.h"
#include "cpf/processrunhelper.h"
#include "ChannelFluxDlg.h"
#include "config_center.h"
#include "TestControlParam.h"


int main_proc(CProcessRunHelper& runHelper)
{
	// 初始化+运行
	CTestControlDlg	dlg(runHelper.m_strCmdLine.c_str());
	if (FALSE == dlg.OnInitDialog() )
	{
		printf("dlg.OnInitDialog error\n");
		CPF::fini();
		exit (0);
	}

	dlg.OnBnClickedButtonStart();


	// 控制
	if (!runHelper.IsService())	// 如果是前台运行，输出提示信息
	{
		printf("\n\n\n");
		printf("==============================================================================\n");
		printf("\n                      Welcome to use %s Debug ! \n", runHelper.GetOpt()->pszSelfName);
		printf("\n");
	}

int i = 0;
//CChannelFluxDlg dlg1;
//dlg1.OnInitDialog();
	char chTemp;
	bool bStopEvent = false;
// ACE_UINT64 last_time = 0, cur_time = 0;
// ACE_OS::gettimeofday().to_usec(last_time);
	while (runHelper.GetRunFlag() && !bStopEvent)	//  等待信号退出
	{
/*
		if (!runHelper.IsService())	// 前台运行，则显示运行状态
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
		else
*/
		{
			sleep(1);
			dlg.OneSecondCall();

// ACE_OS::gettimeofday().to_usec(cur_time);
// printf("%18lld, %18lld, %18lld\n", last_time, cur_time, (cur_time-last_time) );
// last_time = cur_time;

			bStopEvent = dlg.m_bStopEvent;

		}
	}

	// 结束
	dlg.OnBnClickedButtonStopAndExit();

    return 0;
}

void delete_share_mem_file()
{
	const char* pszIPCS = "ipcs -m | grep root | awk '{print $2}' | xargs ipcrm shm";
	system(pszIPCS);

	return;
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

	// 删除旧共享内存的资源
	//这个地方不能调用，因为seprobe程序可能还存在
	//delete_share_mem_file();

	CPF::init();

	ConfigCenter_Initialize();

	CCTestControlParam_Singleton::instance()->Read(
		ConfigCenter_Obj()->GetProbeFullConfigName("test_control_param.xml").c_str());

	main_proc(runHelper);

	ConfigCenter_UnInitialize();

	CPF::fini();

	return 0;
}
