//////////////////////////////////////////////////////////////////////////
//SleepAndDoThread.h

#pragma once

//����һ���̣߳��ȴ�һ��ʱ�䣬Ȼ��ִ��һ������

#include <ace/Task.h>
#include "cpf/cpf.h"

class CPF_CLASS CSleepAndDoThread : public ACE_Task_Base
{
public:
	CSleepAndDoThread(void);
	virtual ~CSleepAndDoThread(void);

public:
	//type=0���˳���ǰ����
	//type=1��������������(reboot)
	//type=2���ر��豸��shutdown)
	void sleep_and_do(int ms);

protected:
	//�ȴ�ʱ���ȥ��
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
	//type=0���˳���ǰ����
	//type=1��������������(reboot)
	//type=2���ر��豸��shutdown)
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