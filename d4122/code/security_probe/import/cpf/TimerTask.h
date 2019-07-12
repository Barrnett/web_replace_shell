//////////////////////////////////////////////////////////////////////////
//TimerTask.h

#pragma once

#include "ace/Task.h"
#include "ace/ACE.h"
#include "cpf/cpf.h"
#include "cpf/my_event.h"


/**
*���ڶ�TIME_VALUE�������Լ��:
*  ���������ļ�Ҫ������ṹ�������,�����������������Ϊ����:
*    (1)tm�ṹ�е�tm_year, tm_mon, tm_mday, tm_hour, tm_sec, tm_min
*       ������ѭ�����ͱ�������ȷ��ֵ,�������Ժ��ԣ��磺������Ϊѭ��
*       ��λ������£�ֻ��Ҫ��tm_hour, tm_sec, tm_min��Լ���
*    (2)�����ļ����ָ�������ܵ�ʱ�����,Ӧ��ת��Ϊ(1)�е�tm�ṹ��Ա
*       Ȼ��������
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
//����ʱ�䶨�������������̵�����
class CPF_CLASS CTimerRestartProcTask : public CCommonTimerTask
{
public:
	CTimerRestartProcTask(ACE_Log_Msg * pLogInstance);
	virtual ~CTimerRestartProcTask();

	int init(const char * proc_name,
		bool brestart=true,//�Ƿ�Ҫ��������
		const char * cmd_line=NULL,
		const char * working_dir=NULL,
		const char *stop_event_name=NULL,
		//stop_interval��λΪ��
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
//CTimerRebootTask:����������������
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

