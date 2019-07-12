//////////////////////////////////////////////////////////////////////////
//SleepAndDoThread.h

#pragma once

//启动一个线程，等待一段时间，然后执行一个命令

#include <ace/Task.h>
#include "cpf/cpf.h"

class CPF_CLASS CSleepAndDoThread : public ACE_Task_Base
{
public:
	CSleepAndDoThread(void);
	virtual ~CSleepAndDoThread(void);

public:
	//type=0，退出当前程序
	//type=1，重新启动机器(reboot)
	//type=2，关闭设备（shutdown)
	void sleep_and_do(int ms);

protected:
	//等待时间过去。
	virtual void Sleep_Wait();

	virtual int DoFunc() = 0;

private:
	virtual int svc(void);

private:
	int	m_ms;
};


class CPF_CLASS CRebootTask : public CSleepAndDoThread
{
public:
	//type=0，退出当前程序
	//type=1，重新启动机器(reboot)
	//type=2，关闭设备（shutdown)
	enum{
		EXIT_PORCESS=0,
		REBOOT_COMPUTER=1,
		POWEROFF_COMPUTER = 2,
	};
public:
	CRebootTask(int type);
	virtual ~CRebootTask(void);

public:
	virtual int DoFunc();

private:
	int m_type;

};