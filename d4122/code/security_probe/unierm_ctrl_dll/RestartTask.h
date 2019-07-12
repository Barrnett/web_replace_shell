//////////////////////////////////////////////////////////////////////////
//RestartTask.h

#pragma once

#include <ace/Task.h>

class CRestartTask : public ACE_Task_Base
{
public:
	CRestartTask(void);
	virtual ~CRestartTask(void);

public:
	//type=0，退出当前程序
	//type=1，重新启动机器(reboot)
	//type=2，关闭设备（shutdown)
	void sleep_and_do(int ms,int type);

private:
	virtual int svc(void);

private:
	virtual void sleep_wait();

protected:
	int m_type;
	int	m_ms;

};
