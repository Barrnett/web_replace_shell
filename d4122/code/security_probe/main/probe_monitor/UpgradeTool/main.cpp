// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cpf/other_tools.h"
#include "ace/OS_main.h"
//#include "UpgradeTool.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "cpf/path_tools.h"
#include "cpf/TinyXmlEx.h"

int DoUpgrade(ACE_Log_File_Msg& log_file);


#ifdef OS_WINDOWS

BOOL EnableDebugPriv() 
{ 
	HANDLE hToken;
	LUID sedebugnameValue; 
	TOKEN_PRIVILEGES tkp; 

	if ( ! OpenProcessToken( GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) ) 
	{
		return FALSE; 
	}

	if ( ! LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &sedebugnameValue ) ) 
	{ 
		CloseHandle( hToken ); 
		return FALSE;
	} 

	tkp.PrivilegeCount = 1; 
	tkp.Privileges[0].Luid = sedebugnameValue; 
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	if ( ! AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL ) ) 
	{
		CloseHandle( hToken );
		return FALSE;
	}

	return true;
}

#endif

void init_log(ACE_Log_File_Msg& log_file)
{
	char log_filename[MAX_PATH];

	CPF::GetModuleFullFileName(log_filename,sizeof(log_filename),"upgrade_tool.log");


	log_file.init(log_filename,true);

	MY_ACE_DEBUG(&log_file,
		(LM_ERROR,
		ACE_TEXT("\n\n\n###############################################################\n")
		));

	MY_ACE_DEBUG(&log_file,
		(LM_ERROR,
		ACE_TEXT("\n%D:升级程序开始启动!!!!!!!!!!!!!!!!!!!\n")
		));

	MY_ACE_DEBUG(&log_file,
		(LM_ERROR,
		ACE_TEXT("\n###############################################################\n\n\n")
		));

	return;
}

void close_log(ACE_Log_File_Msg& log_file)
{
	log_file.fini();

	return;
}

void get_oem_info(std::string& cur_oemname,std::string& str_brand,std::string& str_desktop)
{
	cur_oemname = "unierm";

	char cur_oem_file_name[MAX_PATH];

	CPF::GetModuleFullFileName(cur_oem_file_name,sizeof(cur_oem_file_name),"oem.xml");

	CTinyXmlEx reader;

	if( 0 != reader.open_for_readonly(cur_oem_file_name) )
	{
		return;
	}

	reader.GetAttr(reader.GetRootNode(),"oem_name",cur_oemname);

	reader.GetValueLikeIni(reader.GetRootNode(),"brand",str_brand);
	reader.GetValueLikeIni(reader.GetRootNode(),"desktop_name",str_desktop);

	return;
}

int
ACE_TMAIN (int argc, ACE_TCHAR* argv[])
{
#ifdef OS_WINDOWS
	EnableDebugPriv();
#endif

	ACE::init();

	if( !CPF::TestProcMutex("Upgrade_tool_exe") )
	{
		printf("!!!!!!!!!!!!!!!程序已经启动!!!!!!!!!!\n");

		ACE_OS::sleep(2);

		ACE::fini();

		return -1;
	}

	ACE_Log_File_Msg log_file;
	init_log(log_file);

	time_t ltime_begin;
	time (&ltime_begin);

	struct tm *newtime = localtime(&ltime_begin);

	printf("#################################################\n");
	printf( "开始升级程序:%s....\n",asctime(newtime) );
	printf("#################################################\n");

	int nrtn = DoUpgrade(log_file);

	time_t ltime_end;
	time (&ltime_end);

	int diff = (int)(ltime_end - ltime_begin);

	if( 0 == nrtn )
	{
		printf("#################################################\n");
		printf("升级程序成功结束，spend time=%d(second)....\n",diff);
		printf("#################################################\n");

		MY_ACE_DEBUG(&log_file,
			(LM_ERROR,
			ACE_TEXT("\n\n\n###############################################################\n")
			));

		MY_ACE_DEBUG(&log_file,
			(LM_ERROR,
			ACE_TEXT("\n%D:升级程序成功结束，spend time=%d(second)....\n"),diff
			));

		MY_ACE_DEBUG(&log_file,
			(LM_ERROR,
			ACE_TEXT("\n###############################################################\n\n\n")
			));
	}
	else
	{
		printf("！！！！！！！！！！！！！！！！！！！！！！！！\n");
		printf("升级程序失败！！spend time=%d(second)....\n",diff);
		printf("！！！！！！！！！！！！！！！！！！！！！！！！\n");

		MY_ACE_DEBUG(&log_file,
			(LM_ERROR,
			ACE_TEXT("\n\n\n！！！！！！！！！！！！！！！！！！！！！！！！\n")
			));

		MY_ACE_DEBUG(&log_file,
			(LM_ERROR,
			ACE_TEXT("\n%D:升级程序失败，spend time=%d(second)....\n"),diff
			));

		MY_ACE_DEBUG(&log_file,
			(LM_ERROR,
			ACE_TEXT("\n！！！！！！！！！！！！！！！！！！！！！！！\n\n\n")
			));
	}


	close_log(log_file);

	ACE_OS::sleep(3);

	ACE::fini();

	return 0;
}

#include "../../../unierm_setup_dll/unierm_setup_dll.h"

int DoUpgrade(ACE_Log_File_Msg& log_file)
{
	Cunierm_setup_dll my_setup;

	char old_version[128] = {0};
	char new_version[128] = {0};

	BOOL bCanUpgrade = FALSE;
	int need_to_reboot = 0;

	printf("\n开始检查版本信息....\n");

	if( -1 == my_setup.CheckVersion(old_version,new_version,bCanUpgrade,need_to_reboot) )
	{
		return -1;
	}

	if( bCanUpgrade )
	{
		printf("\n版本信息检查结束，当前版本是：%s，新版本是：%s，可以升级....\n",old_version,new_version);

		MY_ACE_DEBUG(&log_file,
			(LM_ERROR,
			ACE_TEXT("\n版本信息检查结束，当前版本是：%s，新版本是：%s，可以升级....\n"),			
			old_version,new_version
			));
	}
	else
	{
		printf("\n版本信息检查结束，发现错误，当前版本是：%s，新版本是：%s，不能升级....\n",
			old_version,new_version);

		MY_ACE_DEBUG(&log_file,
			(LM_ERROR,
			ACE_TEXT("\n版本信息检查结束，发现错误，当前版本是：%s，新版本是：%s，不能升级....\n"),			
			old_version,new_version
			));

		return -1;
	}

	std::string cur_oemname;
	std::string str_brand;
	std::string str_desktop;

	get_oem_info(cur_oemname,str_brand,str_desktop);

	if( str_desktop.empty() )
	{
		str_desktop = str_brand;

		if( str_desktop.empty() )
		{
			str_desktop = cur_oemname;
		}
	}

	my_setup.init(false,cur_oemname.c_str(),str_brand.c_str(),str_desktop.c_str(),
		Cunierm_setup_dll::SETUP_TYPE_UPGRADE,NULL,NULL,NULL,NULL,0);

	my_setup.start_setup();

	int prev_stage = -2;

	while( !my_setup.IsFinished() )
	{	
		int stage = my_setup.GetStage();

		if( prev_stage != stage )
		{
			printf("\n正在升级，步骤：%d....\n",stage);
			prev_stage = stage;
		}

		ACE_OS::sleep(1);
	}

	printf("\n\n\n升级结束\n\n！！");

	if( my_setup.IsNeedReboot() )
	{
		printf("\n\n\n准备重新启动！！");

		my_setup.Reboot();
	}

	my_setup.close();

	return 0;
}

