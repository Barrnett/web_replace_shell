//////////////////////////////////////////////////////////////////////////
//MonitorTaskRW.h

#pragma once

#include "cpf/TinyXmlEx.h"
#include "MonitorTask.h"

class MONITOR_MANAGER_CLASS CMonitorTaskRW
{
public:
	CMonitorTaskRW(void);
	~CMonitorTaskRW(void);

public:
	static int GetMonitorTasks(CTinyXmlEx& xml_reader,
		TiXmlNode * monitored_task_list_parent,
		std::vector<MONITOR_TASK> & aMonitorTask);

	static bool GetOneMonitorTask(CTinyXmlEx& xml_reader,
		TiXmlNode * pNodeMonitoredTask,
		MONITOR_TASK& monitortask);

	static int SetMonitorTaskState(CTinyXmlEx& xml_writter,
		TiXmlNode * monitored_task_list_parent,
		const char * taskname,int state);
};



