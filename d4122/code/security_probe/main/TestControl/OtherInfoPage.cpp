// OtherInfoPage.cpp : implementation file
//

#include "stdafx.h"
#include "TestControl.h"
#include "OtherInfoPage.h"

#include "cpf/other_tools.h"
#include "PacketIO/MyPlatRunCtrl.h"
#include "flux_control.h"

// COtherInfoPage dialog

IMPLEMENT_DYNAMIC(COtherInfoPage, CPropertyPage)

COtherInfoPage::COtherInfoPage(ACE_Log_File_Msg& log_file,
							   CCommonMainCtrl& main_ctrl,
							   int log_flux)
	: CPropertyPage(COtherInfoPage::IDD)
	, m_log_file(log_file)
	, m_main_ctrl(main_ctrl)
	, m_strTimeSpendPrg(_T(""))
	, m_strTimeSpendSys(_T(""))
	, m_strDroppedFCPkts(_T("0(0.00%)"))
	, m_strDroppedPAPkts(_T("0(0.00%)"))	
	, m_strPAShareBuffer(_T(""))
	
	, m_nTCPLinkTotal(0)
	, m_nTCPLinkCur(0)
	, m_nTCPLinkMax(0)
	, m_nUDPLinkTotal(0)
	, m_nUDPLinkCur(0)
	, m_nUDPLinkMax(0)

	, m_nTCPLinkTotal_dmz(0)
	, m_nTCPLinkCur_dmz(0)
	, m_nTCPLinkMax_dmz(0)
	, m_nUDPLinkTotal_dmz(0)
	, m_nUDPLinkCur_dmz(0)
	, m_nUDPLinkMax_dmz(0)

	, m_ullReadLoops(0)
	, m_ullTimerPackets(0)
	, m_ullTotalPackets(0)
	, m_log_flux(log_flux)
{
	m_dwBegingTickcount = 0;

	m_max_recv_buff = 0;
	m_max_send_buff = 0;
	m_max_pa_share_buff = 0;

	m_nCalBuf_nums = 0;

	m_total_recv_buff = 0;
	m_total_send_buff = 0;
	m_total_pa_share_buff = 0;

	m_recv_alloc_buff = 0;
	m_send_alloc_buff = 0;
	m_pa_share_alloc_buff = 0;

	m_dwNowTick = 0;
	m_dwBegingTickcount = GetTickCount();

	m_bEmptyTimerPkts = false;
}

COtherInfoPage::~COtherInfoPage()
{
}

void COtherInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_RECV_BUFFER, m_strRecvBuffer);
	DDX_Text(pDX, IDC_EDIT_SEND_BUFFER, m_strSendBuffer);

	DDX_Text(pDX, IDC_EDIT_MAX_RECV_BUFFER, m_strMaxRecvBuffer);
	DDX_Text(pDX, IDC_EDIT_MAX_SEND_BUFFER, m_strMaxSendBuffer);

	DDX_Text(pDX, IDC_EDIT_TIMESPEND, m_strTimeSpendPrg);
	DDX_Text(pDX, IDC_EDIT_TIMESPEND_SYS, m_strTimeSpendSys);
	
	DDX_Text(pDX, IDC_EDIT_DROP_PKTS, m_strDroppedFCPkts);
	DDX_Text(pDX, IDC_EDIT_DROP_PA_PKTS, m_strDroppedPAPkts);

	DDX_Text(pDX, IDC_EDIT_PA_BUFFER, m_strPAShareBuffer);
	DDX_Text(pDX, IDC_EDIT_MAX_PA_BUFFER, m_strMaxPAShareBuffer);

	DDX_Text(pDX, IDC_EDIT_TCP_TOTAL, m_nTCPLinkTotal);
	DDX_Text(pDX, IDC_EDIT_TCP_CUR, m_nTCPLinkCur);
	DDX_Text(pDX, IDC_EDIT_TCP_MAX, m_nTCPLinkMax);

	DDX_Text(pDX, IDC_EDIT_UDP_TOTAL, m_nUDPLinkTotal);
	DDX_Text(pDX, IDC_EDIT_UDP_CUR, m_nUDPLinkCur);
	DDX_Text(pDX, IDC_EDIT_UDP_MAX, m_nUDPLinkMax);

	DDX_Text(pDX, IDC_EDIT_READ_LOOP, m_ullReadLoops);
	DDX_Text(pDX, IDC_EDIT_TIMER_PKTS, m_ullTimerPackets);

	DDX_Text(pDX, IDC_EDIT_TCP_TOTAL2, m_nTCPLinkTotal_dmz);
	DDX_Text(pDX, IDC_EDIT_TCP_CUR2, m_nTCPLinkCur_dmz);
	DDX_Text(pDX, IDC_EDIT_TCP_MAX2, m_nTCPLinkMax_dmz);

	DDX_Text(pDX, IDC_EDIT_UDP_TOTAL2, m_nUDPLinkTotal_dmz);
	DDX_Text(pDX, IDC_EDIT_UDP_CUR2, m_nUDPLinkCur_dmz);
	DDX_Text(pDX, IDC_EDIT_UDP_MAX2, m_nUDPLinkMax_dmz);
}


BEGIN_MESSAGE_MAP(COtherInfoPage, CPropertyPage)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_PA_RUNINFO, &COtherInfoPage::OnBnClickedButtonClearPaRuninfo)

END_MESSAGE_MAP()

void COtherInfoPage::GetFriendNumString(ACE_UINT64 num,CString& strnum)
{
	strnum = CPF::GetFriendNumString(num).c_str();

	return;
}


// COtherInfoPage message handlers
BOOL COtherInfoPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return true;
}

void COtherInfoPage::OnTimer(UINT_PTR nIDEvent)
{
	if( 1 == nIDEvent )
	{
		ShowStat(GetTickCount());

		ShowLinkInfo(GetTickCount());

		GetRunTimeInfo();

		UpdateData(FALSE);
	}
	else if( 2 == nIDEvent )
	{
		if( m_log_flux )
		{
			LogBufInfo();
		}
	}

	CDialog::OnTimer(nIDEvent);

	return;
}

void COtherInfoPage::LogBufInfo()
{
	if( m_nCalBuf_nums == 0 )
	{
		m_nCalBuf_nums = 1;
	}

	//输出buf信息
	char buf[1024]={0};

	sprintf(buf,"收缓冲最大=%u(%.2f),平均=%u(%.2f);发缓冲最大=%u(%.2f),平均=%u(%.2f);pa缓冲最大=%u(%.2f),平均=%u(%.2f).",
		m_max_recv_buff,
		m_recv_alloc_buff?(double)m_max_recv_buff*100/m_recv_alloc_buff:0,
		(unsigned int)(m_total_recv_buff/m_nCalBuf_nums),
		m_recv_alloc_buff?(double)(m_total_recv_buff/m_nCalBuf_nums)*100/m_recv_alloc_buff:0,

		m_max_send_buff,
		m_send_alloc_buff?(double)m_max_send_buff*100/m_send_alloc_buff:0,
		(unsigned int)(m_total_send_buff/m_nCalBuf_nums),
		m_send_alloc_buff?(double)(m_total_send_buff/m_nCalBuf_nums)*100/m_send_alloc_buff:0,

		m_max_pa_share_buff,
		m_pa_share_alloc_buff?(double)m_max_pa_share_buff*100/m_pa_share_alloc_buff:0,
		(unsigned int)(m_total_pa_share_buff/m_nCalBuf_nums),
		m_pa_share_alloc_buff?(double)(m_total_pa_share_buff/m_nCalBuf_nums)*100/m_pa_share_alloc_buff:0
		);


	MY_ACE_DEBUG(&m_log_file,
		(LM_ERROR,
		ACE_TEXT("\n%D:%s\n"),buf
		));

	m_max_recv_buff = 0;
	m_max_send_buff = 0;
	m_max_pa_share_buff = 0;

	m_nCalBuf_nums = 0;

	m_total_recv_buff = 0;
	m_total_send_buff = 0;
	m_total_pa_share_buff = 0;

	return;
}

void COtherInfoPage::GetRunTimeInfo()
{
	DWORD dwDiffSec = (GetTickCount()-m_dwBegingTickcount)/1000;

	m_strTimeSpendPrg = CPF::GetTimeLastInfo(dwDiffSec).c_str();

	m_strTimeSpendSys = CPF::GetTimeLastInfo(GetTickCount()/1000).c_str();

}

void COtherInfoPage::ShowStat(DWORD nowtick)
{
	if( 0 == m_dwNowTick )
	{
		m_dwNowTick = nowtick;
		return;
	}


	{
		ACE_UINT32 dwUsed=0;

		dwUsed = FLUX_CONTROL_GetRecvBufferUsed(&m_recv_alloc_buff);

		GetFriendNumString(dwUsed,m_strRecvBuffer);	

		if( dwUsed > m_max_recv_buff )
			m_max_recv_buff = dwUsed;

		GetFriendNumString(m_max_recv_buff,m_strMaxRecvBuffer);	

		m_total_recv_buff += dwUsed;

	}

	{
		ACE_UINT32 dwUsed=0;

		dwUsed = FLUX_CONTROL_GetSendBufferUsed(&m_send_alloc_buff);

		GetFriendNumString(dwUsed,m_strSendBuffer);

		if( dwUsed > m_max_send_buff )
			m_max_send_buff = dwUsed;

		GetFriendNumString(m_max_send_buff,m_strMaxSendBuffer);	

		m_total_send_buff += dwUsed;
	}

	{
		/*
		ACE_UINT32 dwUsed=0;

		dwUsed = FLUX_CONTROL_GetPAShareBufferUsed(&m_pa_share_alloc_buff);

		GetFriendNumString(dwUsed,m_strPAShareBuffer);

		if( dwUsed > m_max_pa_share_buff )
			m_max_pa_share_buff = dwUsed;

		GetFriendNumString(m_max_pa_share_buff,m_strMaxPAShareBuffer);	

		m_total_pa_share_buff += dwUsed;
		*/
	}

	{
		/*
		ACE_UINT64 dropped_kts=0;
		ACE_UINT64 total_kts=0;

		FLUX_CONTROL_GetPARunInfo(dropped_kts,total_kts);

		double rate = 0;
		if( total_kts!= 0 )
			rate = (double)(dropped_kts*100)/total_kts;

		CString strFailedPkts_temp;
		GetFriendNumString(dropped_kts,strFailedPkts_temp);

		m_strDroppedPAPkts.Format("%s(%0.2f)",strFailedPkts_temp.GetString(),rate);
		*/
	}

	++m_nCalBuf_nums;

	{
		const CMyPlatRunCtrl::PlatRun_Info& runinfo = m_main_ctrl.GetRunInfo();

		double rate = 0;
		if( runinfo.ullTotalPackets != 0 )
			rate = (double)(runinfo.ullDroppedPkts*100)/runinfo.ullTotalPackets;

		CString strSendFailedPkts_temp;
		GetFriendNumString(runinfo.ullDroppedPkts,strSendFailedPkts_temp);

		m_strDroppedFCPkts.Format("%s(%0.2f)",strSendFailedPkts_temp.GetString(),rate);

		if( m_ullReadLoops == runinfo.ullReadLoops )
		{
			ACE_UINT32 dwRecvUsed=0;
			ACE_UINT32 dwRecvAllocBuffer = 0;

			dwRecvUsed = FLUX_CONTROL_GetRecvBufferUsed(&dwRecvAllocBuffer);

			ACE_UINT32 dwSendUsed=0;
			ACE_UINT32 dwSendAllocBuffer = 0;

			dwSendUsed = FLUX_CONTROL_GetSendBufferUsed(&dwSendAllocBuffer);
			
		}

		if( m_ullTimerPackets == runinfo.ullTimerPackets )
		{
			ACE_UINT32 dwRecvUsed=0;
			ACE_UINT32 dwRecvAllocBuffer = 0;

			dwRecvUsed = FLUX_CONTROL_GetRecvBufferUsed(&dwRecvAllocBuffer);

			ACE_UINT32 dwSendUsed=0;
			ACE_UINT32 dwSendAllocBuffer = 0;

			dwSendUsed = FLUX_CONTROL_GetSendBufferUsed(&dwSendAllocBuffer);
					

			m_bEmptyTimerPkts = true;
		}
		else
		{
			if( m_bEmptyTimerPkts )
			{
				MY_ACE_DEBUG(&m_log_file,
					(LM_ERROR,
					ACE_TEXT("\n%D:no warning2: have timer pkts in 2 secs,timer_pkts=%Q,pkts=%Q\n"),
					runinfo.ullTimerPackets,runinfo.ullTotalPackets-m_ullTotalPackets
					));

				m_bEmptyTimerPkts = false;
			}
		}

		m_ullReadLoops = runinfo.ullReadLoops;
		m_ullTimerPackets = runinfo.ullTimerPackets;
		m_ullTotalPackets = runinfo.ullTotalPackets;

	}

	UpdateData(false);

	m_dwNowTick = nowtick;

	return;

}

void COtherInfoPage::ShowLinkInfo(DWORD dwtick)
{
	{
		{
			FLUX_CONTROL_GetLinkNums(0,1,m_nTCPLinkTotal,m_nTCPLinkCur);

			if( m_nTCPLinkCur > m_nTCPLinkMax )
				m_nTCPLinkMax = m_nTCPLinkCur;
		}

		{
			FLUX_CONTROL_GetLinkNums(0,2,m_nUDPLinkTotal,m_nUDPLinkCur);

			if( m_nUDPLinkCur > m_nUDPLinkMax )
				m_nUDPLinkMax = m_nUDPLinkCur;
		}
	}


	{
		{
			FLUX_CONTROL_GetLinkNums(1,1,m_nTCPLinkTotal_dmz,m_nTCPLinkCur_dmz);

			if( m_nTCPLinkCur_dmz > m_nTCPLinkMax_dmz )
				m_nTCPLinkMax_dmz = m_nTCPLinkCur_dmz;
		}

		{
			FLUX_CONTROL_GetLinkNums(1,2,m_nUDPLinkTotal_dmz,m_nUDPLinkCur_dmz);

			if( m_nUDPLinkCur_dmz > m_nUDPLinkMax_dmz )
				m_nUDPLinkMax_dmz = m_nUDPLinkCur_dmz;
		}
	}

	return;
}

void COtherInfoPage::OnBnClickedButtonClearPaRuninfo()
{
	return;
}

void COtherInfoPage::start_test()
{
	m_dwNowTick = 0;
	m_dwBegingTickcount = GetTickCount();

	m_max_recv_buff = 0;
	m_max_send_buff = 0;
	m_max_pa_share_buff = 0;

	m_nCalBuf_nums = 0;

	m_total_recv_buff = 0;
	m_total_send_buff = 0;
	m_total_pa_share_buff = 0;

	m_ullReadLoops = 0;
	m_ullTimerPackets = 0;
	m_ullTotalPackets = 0;


	m_bEmptyTimerPkts = false;

	return;
}

void COtherInfoPage::stop_test()
{
	return;
}

void COtherInfoPage::StartTimer()
{
	start_test();

	SetTimer(1,2000,NULL);

	SetTimer(2,1*60*1000,NULL);

}

void COtherInfoPage::StopTimer()
{
	KillTimer(1);
	KillTimer(2);


}