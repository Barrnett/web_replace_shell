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


	//�Ƿ��event��أ�0��ʾ����أ�1��ʾ��أ�
	//���sztask_type=="event"��szMonitorEventName��szStopEventName��Ч��
	//���event_task==������szMonitorEventName��szStopEventName��Ч��

	int onoff;//�Ƿ���Ч�����Ƿ�Ҫ���й���

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

	int	priority;//0��ʾϵͳȱʡ�����ȼ���1��ʾ������ȼ���2��ʾ��ͨ���ȼ���3,4�ȱ�ʾ���ߵ����ȼ�

	int	 nMonitorActiveInterval;				//�����ĳ�����Ҫ��nMonitorInterval�����ٽ�szMonitorEventName����һ��
	int	 nMonitorExistInterval;

	int	 nStopInterval;					//�����ĳ�����Ҫ��nStopInterval�ڼ��ʱ���Ƿ�sigal
	int  nRunConfigState;					//���õ�״̬��0 stop ; 1 start
	int	 nPermanentConfigState;			//���õ�״̬�����ļ��е�����״̬����ʱ��nRunConfigState��nPermanentConfigState���ܲ�һ��

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
	//ǿ��ֹͣ���̣�����m_taskInfo.nRunConfigStateû�иı䡣
	virtual int ForceStopTask() = 0;
	virtual int ForceStartTask() = 0;

protected:
	//ֹͣ����ֱ������
	int TerminateProcesLoop(const char * szproc_name);

public:
	//�����Ƿ����
	virtual bool IsTaskExist() = 0;
	//�����Ƿ�.������϶�����.
	virtual bool IsTaskActive() = 0;

	//ǿ����������ֹͣ������m_taskInfo.nConfigState�ı䡣
	int ChangeConfigState(int nState,BOOL bTempStat);

	int GetConfigState(BOOL bTempStat);

private:
	//�������Ƿ�,����true����ʾ�����˴���������false����ʾû�д���
	BOOL CheckToActive(const ACE_Time_Value &current_time);


	//�������Ƿ����,����true����ʾ�����˴���������false����ʾû�д���
	BOOL CheckToExist(const ACE_Time_Value &current_time);

public:
	// ����ָ������
	static int CreateProcS(BOOL bDebug,const char * szAppName, const char *szCmd=NULL, const char *szWorkingDirectory=NULL,int priority=1 );

	static int RunProcess(const char * szAppName, const char *szCmd, const char *szWorkingDirectory,int priority);
	static int RunProcessAsUser(const char * szAppName, const char *szCmd, const char *szWorkingDirectory,int priority);

	static BOOL GetTokenByName(HANDLE &hToken,const char * lpName);

	// ��ָֹ������
	static bool TerminateProcS( IN unsigned int uPID );

	// ��ָֹ������
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
	
	ACE_Time_Value	m_lastActiveTime;//�ϴλʱ��
	ACE_Time_Value	m_lastExistTime;//�ϴδ���ʱ��

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
	//�����Ƿ����
	virtual bool IsTaskExist();
	//�����Ƿ�.������϶�����.
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
	//�����Ƿ����
	virtual bool IsTaskExist();
	//�����Ƿ�.������϶�����.
	virtual bool IsTaskActive();
};