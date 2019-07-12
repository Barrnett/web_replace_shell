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
	//type=0���˳���ǰ����
	//type=1��������������(reboot)
	//type=2���ر��豸��shutdown)
	void sleep_and_do(int ms,int type);

private:
	virtual int svc(void);

private:
	virtual void sleep_wait();

protected:
	int m_type;
	int	m_ms;

};
