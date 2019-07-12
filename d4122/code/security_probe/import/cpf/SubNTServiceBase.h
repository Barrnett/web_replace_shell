//////////////////////////////////////////////////////////////////////////
//SubNTServiceBase.h

#pragma once

#include "ace/Task.h"
#include "cpf/cpf.h"

#ifdef OS_WINDWOS
#	include "winsvc.h"
#else
#define SERVICE_CONTROL_PAUSE 0x00000002
#define SERVICE_CONTROL_CONTINUE 0x00000003
#endif

class CPF_CLASS CSubNTServiceBase : public ACE_Task<ACE_MT_SYNCH>
{
public:
	CSubNTServiceBase(void){};
	virtual ~CSubNTServiceBase(void){};

public:
	virtual int handle_control (DWORD control_code)
	{
		switch (control_code)
		{
		case SERVICE_CONTROL_PAUSE:
			this->pause_svc();
			break;

		case SERVICE_CONTROL_CONTINUE:
			this->continue_svc();
			break;

		default:
			break;
		}

		return 0;
	}
public:

	virtual int start_svc()
	{
		return 0;
	}

	virtual int stop_svc()
	{
		return 0;
	}
	
	virtual int pause_svc()
	{
		return 0;
	}

	virtual int continue_svc()
	{
		return 0;
	}
};
