//////////////////////////////////////////////////////////////////////////
//TimerTask.h

#pragma once

#include "ace/Task.h"
#include "ace/ACE.h"
#include "cpf/cpf.h"
#include "cpf/my_event.h"


/**
*关于对TIME_VALUE结果填充的约定:
*  根据配置文件要对这个结构进行填充,我这里期望的填充行为如下:
*    (1)tm结构中的tm_year, tm_mon, tm_mday, tm_hour, tm_sec, tm_min
*       根据其循环类型被赋与正确的值,其他可以忽略，如：在以天为循环
*       单位的情况下，只需要将tm_hour, tm_sec, tm_min填对即可
*    (2)配置文件如果指定的是周的时间概念,应该转化为(1)中的tm结构成员
*       然后完成填充
*/



class CPF_CLASS TimerTaskBase : public ACE_Task_Base 
{
public:
	TimerTaskBase()
	{
	}

	virtual ~TimerTaskBase()
	{
	}

public:
	virtual int startJob() = 0;
	virtual int stopJob() = 0;
	
	const char * getName() const
	{
        return name_.c_str();
	}

	void setName(ACE_TCHAR * name)
	{
		name_.assign(name);
	}

private:
	std::string	name_;//task name 
};

//////////////////////////////////////////////////////////////////////////
//CThreadTimerTask
//////////////////////////////////////////////////////////////////////////

class CPF_CLASS CThreadTimerTask : public TimerTaskBase 
{
public:
	CThreadTimerTask()
	{
	}

	virtual ~CThreadTimerTask()
	{
	}
	virtual int startJob()
	{
		this->open();
		this->activate();
		
		return 0;
	}

	virtual int stopJob()
	{
		this->close();
        this->wait();//

		return 0;
	}

};

//////////////////////////////////////////////////////////////////////////
//CCommonTimerTask
//////////////////////////////////////////////////////////////////////////

class CPF_CLASS CCommonTimerTask : public TimerTaskBase 
{
public:
	CCommonTimerTask()
	{
	}

	virtual ~CCommonTimerTask()
	{
	}
	virtual int startJob()
	{
		//init
		this->open();

		//real work
		this->svc();

		return 0;
	}

	virtual int stopJob()
	{
		return this->close();
	}

};


//////////////////////////////////////////////////////////////////////////
//CTimerRestartProcTask
//////////////////////////////////////////////////////////////////////////
//根据时间定期重新启动进程的任务
class CPF_CLASS CTimerRestartProcTask : public CCommonTimerTask
{
public:
	CTimerRestartProcTask(ACE_Log_Msg * pLogInstance);
	virtual ~CTimerRestartProcTask();

	int init(const char * proc_name,
		bool brestart=true,//是否要重新启动
		const char * cmd_line=NULL,
		const char * working_dir=NULL,
		const char *stop_event_name=NULL,
		//stop_interval单位为秒
		unsigned int stop_interval=0);

protected:
	virtual int svc();

private:
	std::string		m_strProcName;
	std::string		m_strCmdline;
	std::string		m_strStopEventName;
	std::string		m_strWorkingDir;
	ACE_Time_Value	m_stopInterval;
	bool			m_brestart;

private:
	pid_t				m_Pid;
	ACE_Log_Msg *		m_pLogInstance;

protected:
	static unsigned int FindProcessByName(const char * name);
	static bool TerminateProcS( IN unsigned int uPID );
	static int CreateProcS( const char * szAppName, const char *szCmd,const char *szWorkingDirectory);

	my_event_t *		m_pStopEvent;
	my_event_t * Create_Stop_Event(const char *stop_event_name);

};

//////////////////////////////////////////////////////////////////////////
//CTimerRebootTask:定期重新启动机器
//////////////////////////////////////////////////////////////////////////
class CPF_CLASS CTimerRebootTask : public CCommonTimerTask
{
public:
	CTimerRebootTask(ACE_Log_Msg * pLogInstance);
	virtual ~CTimerRebootTask();

protected:
	virtual int svc();

private:
	ACE_Log_Msg *		m_pLogInstance;

};

