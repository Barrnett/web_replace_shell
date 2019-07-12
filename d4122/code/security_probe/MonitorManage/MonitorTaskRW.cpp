//////////////////////////////////////////////////////////////////////////
//MonitorTaskRW.cpp

#include "MonitorTaskRW.h"
#include "cpf/path_tools.h"
#include "cpf/strtools.h"

CMonitorTaskRW::CMonitorTaskRW(void)
{
}

CMonitorTaskRW::~CMonitorTaskRW(void)
{
}


int CMonitorTaskRW::GetMonitorTasks(CTinyXmlEx& xml_reader,
									   TiXmlNode * monitored_task_list_parent,
									   std::vector<MONITOR_TASK> & aMonitorTask)
{

	const char MONITORED_TASK[] = "monitored_task";

	TiXmlNode * pNodeMonitoredTask = NULL;

	for(pNodeMonitoredTask = monitored_task_list_parent->FirstChildElement(MONITORED_TASK);
		pNodeMonitoredTask != NULL;
		pNodeMonitoredTask=pNodeMonitoredTask->NextSiblingElement(MONITORED_TASK))
	{	
		MONITOR_TASK monitortask;

		if( GetOneMonitorTask(xml_reader,pNodeMonitoredTask,monitortask) )
		{
			if( monitortask.onoff )
			{
				aMonitorTask.push_back(monitortask);
			}
		}		
	}

	return 0;
}

bool CMonitorTaskRW::GetOneMonitorTask(CTinyXmlEx& xml_reader,
										  TiXmlNode * pNodeMonitoredTask,
										  MONITOR_TASK& monitortask)
{
	std::string strTemp;

	if( !xml_reader.GetAttr(pNodeMonitoredTask,"type",strTemp) )
	{
		strTemp = "event";
	}

	xml_reader.GetAttr(pNodeMonitoredTask,"onoff",monitortask.onoff);

	strncpy(monitortask.sztask_type,strTemp.c_str(),63);
	monitortask.sztask_type[63]=0;

	CPF::str_lwr(monitortask.sztask_type);

	xml_reader.GetValueLikeIni(pNodeMonitoredTask,"ConfigState",monitortask.nPermanentConfigState);
	monitortask.nRunConfigState = monitortask.nPermanentConfigState;

	xml_reader.GetValueLikeIni(pNodeMonitoredTask,"MonitorActiveInterval",monitortask.nMonitorActiveInterval);
	xml_reader.GetValueLikeIni(pNodeMonitoredTask,"MonitorExistInterval",monitortask.nMonitorExistInterval);

	xml_reader.GetValueLikeIni(pNodeMonitoredTask,"StopEventInterval",monitortask.nStopInterval);
	xml_reader.GetValueLikeIni(pNodeMonitoredTask,"priority",monitortask.priority);

	if( !xml_reader.GetValueLikeIni(pNodeMonitoredTask,"FileName",
		monitortask.szFileName,sizeof(monitortask.szFileName)) )
	{
		return false;
	}

	strncpy(monitortask.szProcessName,CPF::GetFileNameFromFullName(monitortask.szFileName),255);
	monitortask.szProcessName[255]=0;

	xml_reader.GetValueLikeIni(pNodeMonitoredTask,"desc",
		monitortask.szDesc,sizeof(monitortask.szDesc));

	xml_reader.GetValueLikeIni(pNodeMonitoredTask,"FullCommandLine",
		monitortask.szFullCmdLine,sizeof(monitortask.szFullCmdLine));

	xml_reader.GetValueLikeIni(pNodeMonitoredTask,"CommandLine",
		monitortask.szCmdLine,sizeof(monitortask.szCmdLine));

	xml_reader.GetValueLikeIni(pNodeMonitoredTask,"WorkingDirectory",
		monitortask.szWorkingDirectory,sizeof(monitortask.szWorkingDirectory));

	xml_reader.GetValueLikeIni(pNodeMonitoredTask,"ExitCommandLine",
		monitortask.szExitCommandLine,sizeof(monitortask.szExitCommandLine));

	if( ACE_OS::strcmp(monitortask.sztask_type,"event") == 0 )
	{
		if( !xml_reader.GetValueLikeIni(pNodeMonitoredTask,"MonitorEventName",
			monitortask.szMonitorEventName,sizeof(monitortask.szMonitorEventName)) )
		{
			return false;
		}

		if( !xml_reader.GetValueLikeIni(pNodeMonitoredTask,"StopEventName",
			monitortask.szStopEventName,sizeof(monitortask.szStopEventName)) )
		{
			return false;
		}
	}

	return true;
}

int CMonitorTaskRW::SetMonitorTaskState(CTinyXmlEx& xml_writter,
										TiXmlNode * monitored_task_list_parent,
										const char * taskname,int state)
{
	const char MONITORED_TASK[] = "monitored_task";

	TiXmlNode * pNodeMonitoredTask = NULL;

	for(pNodeMonitoredTask = monitored_task_list_parent->FirstChildElement(MONITORED_TASK);
		pNodeMonitoredTask != NULL;
		pNodeMonitoredTask=monitored_task_list_parent->NextSibling(MONITORED_TASK))
	{	
		MONITOR_TASK monitortask;

		if( GetOneMonitorTask(xml_writter,pNodeMonitoredTask,monitortask) )
		{
			if( stricmp(monitortask.szFileName,taskname) != 0 )
				continue;

			xml_writter.SetValueLikeIni(pNodeMonitoredTask,"ConfigState",state);

			xml_writter.savefile();

			return 0;
		}		
	}

	return -1;
}
