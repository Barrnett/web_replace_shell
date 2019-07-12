#include <stdio.h>
#include "TestControlDlg.h"
#include "cpf/processrunhelper.h"
#include "ChannelFluxDlg.h"
#include "config_center.h"
#include "TestControlParam.h"


int main_proc(CProcessRunHelper& runHelper)
{
	// ��ʼ��+����
	CTestControlDlg	dlg(runHelper.m_strCmdLine.c_str());
	if (FALSE == dlg.OnInitDialog() )
	{
		printf("dlg.OnInitDialog error\n");
		CPF::fini();
		exit (0);
	}

	dlg.OnBnClickedButtonStart();


	// ����
	if (!runHelper.IsService())	// �����ǰ̨���У������ʾ��Ϣ
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
	while (runHelper.GetRunFlag() && !bStopEvent)	//  �ȴ��ź��˳�
	{
/*
		if (!runHelper.IsService())	// ǰ̨���У�����ʾ����״̬
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

	// ����
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
	// ȷ������ģʽ
	CProcessRunHelper runHelper;
	if (runHelper.ParseCommand(argc, argv) <= 0)
		return 0;

	if(0 != runHelper.TryAcquire())
	{
		printf("%s is Running! [%d]\n", runHelper.GetOpt()->pszSelfName, errno);
		return 0;
	}

	// ɾ���ɹ����ڴ����Դ
	//����ط����ܵ��ã���Ϊseprobe������ܻ�����
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
