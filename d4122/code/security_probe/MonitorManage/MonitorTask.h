//////////////////////////////////////////////////////////////////////////
//MonitorTask.h

#pragma once

#include "ace/Synch.h"
#include "ace/Thread.h"
#include "ace/OS_NS_Thread.h"
#include "ace/Log_Msg.h"
#include "MonitorMgrDllMacro.h"


typedef struct _monitorTask
{
	_monitorTask()
	{
		szFileName[0] = 0;
		szDesc[0] = 0;
		szCmdLine[0] = 0;
		szFullCmdLine[0] = 0;
		szWorkingDirectory[0] = 0;
		szMonitorEventName[0] = 0;
		szStopEventName[0] = 0;
		sztask_type[0] = 0;
		szProcessName[0] = 0;

		szExitCommandLine[0] = 0;

		onoff = 1;
		
		nRunConfigState = 1;
		nPermanentConfigState = 1;

		nMonitorExistInterval = 3;
		nMonitorActiveInterval = 10;

		nStopInterval = 3;

		priority = 0;

	}


	//是否和event相关，0表示不相关，1表示相关，
	//如果sztask_type=="event"，szMonitorEventName和szStopEventName有效，
	//如果event_task==其他，szMonitorEventName和szStopEventName无效，

	int onoff;//是否有效，即是否要进行管理

	char sztask_type[64];

	char szMonitorEventName[64];
	char szStopEventName[64];

	char szFileName[256];
	char szDesc[256];
	char szCmdLine[256];
	char szFullCmdLine[256];
	char szProcessName[256];
	char szWorkingDirectory[256];

	char szExitCommandLine[256];

	int	priority;//0表示系统缺省的优先级，1表示最低优先级，2表示普通优先级，3,4等表示更高的优先级

	int	 nMonitorActiveInterval;				//被监测的程序需要在nMonitorInterval内至少将szMonitorEventName点亮一次
	int	 nMonitorExistInterval;

	int	 nStopInterval;					//被检测的程序需要在nStopInterval内检查时间是否被sigal
	int  nRunConfigState;					//配置的状态：0 stop ; 1 start
	int	 nPermanentConfigState;			//永久的状态，在文件中的配置状态，有时候nRunConfigState和nPermanentConfigState可能不一样

}MONITOR_TASK;

//////////////////////////////////////////////////////////////////////////
//CMonitorTask
//////////////////////////////////////////////////////////////////////////

class MONITOR_MANAGER_CLASS CMonitorTask
{
public:
	CMonitorTask();
	virtual ~CMonitorTask();

	void init(BOOL bDebug,const MONITOR_TASK& MonitorTask, ACE_Log_Msg* pLogFile);

	virtual int OnTimer(const ACE_Time_Value &current_time);

public:
	//强制停止进程，但是m_taskInfo.nRunConfigState没有改变。
	virtual int ForceStopTask() = 0;
	virtual int ForceStartTask() = 0;

protected:
	//停止任务，直到结束
	int TerminateProcesLoop(const char * szproc_name);

public:
	//进程是否存在
	virtual bool IsTaskExist() = 0;
	//进程是否活动.如果活动则肯定存在.
	virtual bool IsTaskActive() = 0;

	//强制启动或者停止，并且m_taskInfo.nConfigState改变。
	int ChangeConfigState(int nState,BOOL bTempStat);

	int GetConfigState(BOOL bTempStat);

private:
	//检查进程是否活动,返回true，表示进行了处理，返回了false，表示没有处理
	BOOL CheckToActive(const ACE_Time_Value &current_time);


	//检查进程是否存在,返回true，表示进行了处理，返回了false，表示没有处理
	BOOL CheckToExist(const ACE_Time_Value &current_time);

public:
	// 创建指定进程
	static int CreateProcS(BOOL bDebug,const char * szAppName, const char *szCmd=NULL, const char *szWorkingDirectory=NULL,int priority=1 );

	static int RunProcess(const char * szAppName, const char *szCmd, const char *szWorkingDirectory,int priority);
	static int RunProcessAsUser(const char * szAppName, const char *szCmd, const char *szWorkingDirectory,int priority);

	static BOOL GetTokenByName(HANDLE &hToken,const char * lpName);

	// 终止指定进程
	static bool TerminateProcS( IN unsigned int uPID );

	// 终止指定进程
	static bool TerminateProcS( IN const char * szProcessname );

	const MONITOR_TASK& GetTaskInfo() const
	{
		return m_taskInfo;
	}

	enum
	{
		CONFIG_STAT_STOP=0,
		CONFIG_STAT_START=1,
	};

	enum
	{
		RUNING_STAT_STOP=0,
		RUNING_STAT_RUNNING=1,
		RUNING_STAT_STARTING=2,
		RUNING_STAT_STOPPING=3,
	};
	int  m_nRunningState;//0 stop ; 1 running ;2 starting ;3 stopping

protected:
	static unsigned int FindProcessByName(const char * name);

protected:
	enum
	{
		///STOPPING_TIME_OUT=10,
		//RUNNING_TIME_OUT=30,
		//STARTING_TIME_OUT=60,

		STOPPING_TIME_OUT = 2,  //for debug
		RUNNING_TIME_OUT  = 2,
		STARTING_TIME_OUT = 2 
	};

	unsigned int	m_Pid;

	MONITOR_TASK	m_taskInfo;

	ACE_Log_Msg *	m_pLogInstance;
	
	ACE_Time_Value	m_lastActiveTime;//上次活动时间
	ACE_Time_Value	m_lastExistTime;//上次存在时间

protected:
	BOOL			m_bDebug;


};


//////////////////////////////////////////////////////////////////////////
//CMonitorTask_Event
//////////////////////////////////////////////////////////////////////////

class MONITOR_MANAGER_CLASS CMonitorTask_Event : public CMonitorTask
{
public:
	CMonitorTask_Event();
	virtual ~CMonitorTask_Event();

public:
	virtual int ForceStopTask();
	virtual int ForceStartTask();

public:
	//进程是否存在
	virtual bool IsTaskExist();
	//进程是否活动.如果活动则肯定存在.
	virtual bool IsTaskActive();


private:
	enum
	{	
		STARTING_TIME_OUT = 10
	};

};


//////////////////////////////////////////////////////////////////////////
//CMonitorTask_Common
//////////////////////////////////////////////////////////////////////////


class MONITOR_MANAGER_CLASS CMonitorTask_Common : public CMonitorTask
{
public:
	CMonitorTask_Common();
	virtual ~CMonitorTask_Common();

public:
	virtual int ForceStopTask();
	virtual int ForceStartTask();

public:
	//进程是否存在
	virtual bool IsTaskExist();
	//进程是否活动.如果活动则肯定存在.
	virtual bool IsTaskActive();
};