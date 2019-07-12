//////////////////////////////////////////////////////////////////////////
//FCSleepAndDoThread.h

#pragma once

#include "cpf/SleepAndDoThread.h"

class CFCSleepAndDoThread : public CSleepAndDoThread
{
public:
	CFCSleepAndDoThread(int type,int reason);
	virtual ~CFCSleepAndDoThread(void);

protected:
	virtual int DoFunc();

private:
	int m_type;
	int m_reason;

};
