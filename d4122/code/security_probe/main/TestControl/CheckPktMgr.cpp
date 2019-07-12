//////////////////////////////////////////////////////////////////////////
//CheckPktMgr.h


#include "StdAfx.h"
#include "CheckPktMgr.h"
#include "PacketIO/CommonMainCtrl.h"
#include "BypassManager/BypassTool.h"

CCheckPktMgr::CCheckPktMgr(void)
{
	m_bCheck = true;
	m_hdlg = NULL;
	m_pMainCtrl = NULL;
	m_pBypassTool = NULL;

	m_nTimesFailed = 0;

	m_no_pkt_in_secnums = 60;

	m_bCanPostMessage = false;
}

CCheckPktMgr::~CCheckPktMgr(void)
{
}

void CCheckPktMgr::init(BOOL bCheck,HWND hdlg,CCommonMainCtrl *pMainCtrl,
						CBypassTool * pBypassTool,
						int no_pkt_in_secnums )
{
	m_bCheck = bCheck;
	m_hdlg = hdlg;

	m_no_pkt_in_secnums = no_pkt_in_secnums;

	m_pMainCtrl = pMainCtrl;
	m_pBypassTool = pBypassTool;

	if( m_no_pkt_in_secnums > 0 )
	{
		m_ActiveTimer.schedule(this,(const void *)1,
			ACE_OS::gettimeofday(),
			ACE_Time_Value(1,0) );//1sec
	}

	return;

}

void CCheckPktMgr::fini()
{
	return;
}

void CCheckPktMgr::start_check()
{
	if( m_no_pkt_in_secnums > 0 )
	{
		m_ActiveTimer.activate();
	}

	return;

}

void CCheckPktMgr::stop_check()
{
	if( m_no_pkt_in_secnums > 0 )
	{
		m_ActiveTimer.timer_queue()->cancel(this);

		m_ActiveTimer.deactivate();

		m_ActiveTimer.wait();
	}

	return;
}


int CCheckPktMgr::handle_timeout(const ACE_Time_Value& tv,const void *arg)
{
	int int_param = (int)arg;

	if( int_param == 1 )
	{
		if( !m_bCheck )
		{
			return 0;
		}

		const CMyPlatRunCtrl::PlatRun_Info& run_info = m_pMainCtrl->GetRunInfo();

		if( (int)(run_info.ullTotalPackets - m_lastPkts) > 0 )
		{
			//comment by zhzhup -避免多线程调用ResetWDT
			//m_pBypassTool->ResetWDT(0);

			m_nTimesFailed = 0;

			m_lastPkts = run_info.ullTotalPackets;
		}
		else
		{
			m_lastPkts = run_info.ullTotalPackets;

			if( m_bCanPostMessage )
			{
				if( ++m_nTimesFailed >= m_no_pkt_in_secnums )
				{					
					m_bCanPostMessage = false;
					m_nTimesFailed = 0;
					m_lastPkts = 0;

					::PostMessage(m_hdlg,WM_RESTART_PROBE,0,0);
				}
			}
			
		}
	}

	return 0;

}
