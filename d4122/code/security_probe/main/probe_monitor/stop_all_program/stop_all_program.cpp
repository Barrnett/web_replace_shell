// stop_all_program.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stop_all_program.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/path_tools.h"
#include "MonitorTaskRW.h"
#include "MonitorManager.h"
#include "ace/Process.h"
#include "shellapi.h"
#include "cpf/os_syscall.h"


CStop_all_program::CStop_all_program()
{
	m_pCurUpdateNode = NULL;
}

CStop_all_program::~CStop_all_program()
{

}

BOOL CStop_all_program::init()
{
	const char xml_file_name[] = "stop_all_program.xml";

	if( 0 != m_xml_reader.open_for_readonly(CPF::GetModuleFullFileName(xml_file_name)) )
	{
		return false;
	}

	m_pCurUpdateNode = m_xml_reader.GetRootNode();

	return true;
}

void CStop_all_program::close()
{
	m_xml_reader.close();

	return;
}


int CStop_all_program::StopOldProgram()
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

	monitor_mgr.StopAll(1);

	return 0;
}

