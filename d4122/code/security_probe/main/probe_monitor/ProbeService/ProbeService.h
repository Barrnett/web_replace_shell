//////////////////////////////////////////////////////////////////////////
//ProbeService.h

#pragma once

#include "ace/Event_Handler.h"
#include "ace/NT_Service.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "cpf/MyNTService.h"

class CProbeService : public CMyNTService
{
public:
	CProbeService(void);
	virtual ~CProbeService(void);

public:
	int init(ACE_Log_Msg * pLogInstance);

public:
	virtual void handle_control (DWORD control_code);
	// We override <handle_control> because it handles stop requests
	// privately.

	virtual int  handle_exception (ACE_HANDLE h);
	// We override <handle_exception> so a 'stop' control code can pop
	// the reactor off of its wait.

	virtual int svc (void);
	// This is a virtual method inherited from ACE_NT_Service.

	virtual int handle_timeout (const ACE_Time_Value& tv,
		const void *arg = 0);
	// Where the real work is done:

private:

	BOOL ReadConfig();

	ACE_event_t * Create_Stop_Event(ACE_Log_File_Msg * loginstance,const char *stop_event_name );

	ACE_event_t *		m_pStopEvent;
	ACE_event_t *		m_pMonitorEvent;

	int CheckStopEvent();

private:
	typedef ACE_NT_Service inherited;
	
private:
	int					m_stopping;//准备停止
	int					m_stopped;//已经停止

	ACE_Log_Msg *		m_pLogInstance;


};
