// restart_all_program.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "restart_all_program.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/path_tools.h"
#include "MonitorTaskRW.h"
#include "MonitorManager.h"
#ifdef OS_WINDOWS
#include "ace/Process.h"
#include "shellapi.h"
#else
#include <unistd.h>
#endif
#include "cpf/os_syscall.h"


Crestart_all_program::Crestart_all_program()
{
	m_pCurUpdateNode = NULL;
}

Crestart_all_program::~Crestart_all_program()
{

}

BOOL Crestart_all_program::init()
{
	const char xml_file_name[] = "restart_all_program.xml";

	if( 0 != m_xml_reader.open_for_readonly(CPF::GetModuleFullFileName(xml_file_name)) )
	{
		return false;
	}

	m_pCurUpdateNode = m_xml_reader.GetRootNode();

	return true;
}

void Crestart_all_program::close()
{
	m_xml_reader.close();

	return;
}


int Crestart_all_program::StopOldProgram()
{
	//读取需要停止程序的配置

	TiXmlNode * service_node = 
		m_pCurUpdateNode->FirstChildElement("MonitorService");

	if( !service_node )
	{
		return -1;
	}

	std::vector<MONITOR_TASK> aMonitorTask;

	CMonitorTaskRW::GetMonitorTasks(m_xml_reader,service_node,aMonitorTask);

	if( aMonitorTask.size() == 0 )
	{
		return 0;
	}

	//停止程序
	CMonitorManager monitor_mgr(true,NULL);

	monitor_mgr.AddMonitorTasks(aMonitorTask);

	monitor_mgr.StopAll();

	return 0;
}


int Crestart_all_program::StartProgram()
{
	TiXmlNode * start_proram_node = 
		m_pCurUpdateNode->FirstChildElement("start_proram");

	if( !start_proram_node )
	{
		return -1;
	}


	for(TiXmlNode * program_node = start_proram_node->FirstChildElement("program");
		program_node;
		program_node = program_node->NextSiblingElement("program"))
	{
		int onoff = 1;

		m_xml_reader.GetAttr(program_node,"onoff",onoff);

		if( onoff )
		{
			std::string str_program;

			m_xml_reader.GetValue(program_node,str_program);

			if( str_program.empty() )
				continue;

			//启动程序
			//...
			StartOneProgram(str_program.c_str());
		}
	}

	int reboot = 0;
	m_xml_reader.GetAttr(start_proram_node,"reboot",reboot);

	if( reboot == 1 )
	{
		//重新启动机器
		CPF::MySystemShutdown(0x02);
	}

	return 0;
}

int Crestart_all_program::StartOneProgram(const char * prg_name)
{
#ifdef OS_LINUX
//	printf("\n Crestart_all_program::StartOneProgram = %s\n", prg_name);
	char strPath[512];
	char strFile[128];
	char strOldPath[128];
	if (NULL == prg_name)
		return -1;
	strcpy(strPath, prg_name);

	ACE_OS::getcwd((char*)strOldPath, 128);
	// 先进入目录，在启动程序
	char* pPos = strrchr(strPath, '/');
	if (NULL != pPos)
	{
		*pPos = '\0';
		int iRtn = chdir(strPath);
//		printf("chdir = %s, Rtn = %d\n", strPath, iRtn);

		// 将名字copy过去
		pPos ++;
		sprintf(strFile, "./%s", pPos);
	}
	else
	{
		sprintf(strFile, "./%s", strPath);
	}

//	printf("Crestart_all_program::StartOneProgram = %s\n", strFile);

	/*return*/ system(strFile); // 这个返回值并不是pid

	chdir(strOldPath);	// 返回开始的目录
	return 0;
#else
	std::string str_suf;

	CPF::GetSufFromFileName(prg_name,str_suf);

	if(stricmp(str_suf.c_str(),"bat") == 0 )
	{
		RunBat(prg_name,NULL,NULL);
	}
	else
	{
		int process_id = RunProcess(prg_name,NULL,NULL);

		if( process_id == -1 )
			return -1;
	}
	return 0;
#endif
}

#ifdef OS_WINDOWS
int Crestart_all_program::RunProcess(const char * szAppName, const char *szCmd, const char *szWorkingDirectory)
{
	if( !szAppName && !szCmd )
		return -1;

	ACE_Process rProcess;
	ACE_Process_Options rOpt;

	rOpt.handle_inheritance(false);

	if( szWorkingDirectory )
	{
		rOpt.working_directory(szWorkingDirectory);
	}

#ifdef OS_WINDOWS
	rOpt.creation_flags( CREATE_NEW_CONSOLE );
#endif

	if( szAppName && szCmd )
		rOpt.command_line("%s %s",szAppName,szCmd);
	else if( szAppName )
		rOpt.command_line("%s",szAppName);
	else if( szCmd )
		rOpt.command_line("%s",szCmd);

	return rProcess.spawn( rOpt );
}

int Crestart_all_program::RunBat(const char * szAppName, const char *szCmd, const char *szWorkingDirectory)
{
	if( !szAppName && !szCmd )
		return -1;

	if( CPF::IsFullPathName(szAppName) )
	{
		const char * path_name = CPF::GetPathNameFromFullName(szAppName);

		ShellExecute(NULL,NULL,szAppName,NULL,path_name,SW_SHOWNORMAL);
	}
	else
	{
		ShellExecute(NULL,NULL,szAppName,NULL,NULL,SW_SHOWNORMAL);
	}

	return 0;
}

#endif //OS_WINDOWS