//////////////////////////////////////////////////////////////////////////
//CheckPktMgr.h

#pragma once

#include "ace/Event_Handler.h"
#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"

class CCommonMainCtrl;
class CBypassTool;

#define WM_RESTART_PROBE	(WM_USER+100)

class CCheckPktMgr : public ACE_Event_Handler
{
public:
	CCheckPktMgr(void);
	~CCheckPktMgr(void);

	void init(BOOL bCheck,HWND hdlg,CCommonMainCtrl *pMainCtrl,
		CBypassTool * pBypassTool,int no_pkt_in_secnums);

	void fini();

	void start_check();
	void stop_check();

	void SetCanPostMessage()
	{
		m_nTimesFailed = 0;
		m_bCanPostMessage = true;
	}

	void EnableCheck(BOOL bCheck)
	{
		m_bCheck = bCheck;
	}

private:
	BOOL			m_bCheck;
	HWND			m_hdlg;
	CCommonMainCtrl *m_pMainCtrl;
	CBypassTool *	m_pBypassTool;

	ACE_UINT64		m_lastPkts;
	int				m_nTimesFailed;


	//<!--多少秒钟没有数据，认为系统出错，系统重新启动probe程序,缺省60秒-->
	int				m_no_pkt_in_secnums;//

	BOOL			m_bCanPostMessage;

private:
	typedef ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap>	ACTIVE_TIMER;

	ACTIVE_TIMER 				m_ActiveTimer;

	virtual int handle_timeout(const ACE_Time_Value& tv,const void *arg);
};
