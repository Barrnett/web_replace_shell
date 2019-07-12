// IfInfoPage.cpp : implementation file
//

#include "stdafx.h"
#include "TestControl.h"
#include "IfInfoPage.h"
#include "cpf/other_tools.h"
#include "flux_control.h"


// CIfInfoPage dialog

IMPLEMENT_DYNAMIC(CIfInfoPage, CPropertyPage)

CIfInfoPage::CIfInfoPage(ACE_Log_File_Msg& log_file,int line_index,int log_flux)
	: CPropertyPage(CIfInfoPage::IDD)
	, m_log_file(log_file)
	, m_strInnerTotal(_T("0"))
	, m_strInnerTrans(_T("0"))
	, m_dInnerRate(0)
	, m_strOutterTotal(_T("0"))
	, m_strOutterTrans(_T("0"))
	, m_dOutterRate(0)
	, m_dTotalRate(0)
	, m_log_flux(log_flux)
{
	m_nLineIndex = line_index;
}

CIfInfoPage::~CIfInfoPage()
{
}


void CIfInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_INNER_TOTAL, m_strInnerTotal);
	DDX_Text(pDX, IDC_EDIT_INNER_TRANS, m_strInnerTrans);
	DDX_Text(pDX, IDC_EDIT_INNER_RATE, m_dInnerRate);
	DDX_Text(pDX, IDC_EDIT_INNER_TOTAL_BPS, m_strInnerTotalBPS);
	DDX_Text(pDX, IDC_EDIT_INNER_TOTAL_PPS, m_strInnerTotalPPS);
	DDX_Text(pDX, IDC_EDIT_INNER_TRANS_BPS, m_strInnerTransBPS);
	DDX_Text(pDX, IDC_EDIT_INNER_TRANS_PPS, m_strInnerTransPPS);

	DDX_Text(pDX, IDC_EDIT_OUTTER_TOTAL, m_strOutterTotal);
	DDX_Text(pDX, IDC_EDIT_OUTTER_TRANS, m_strOutterTrans);
	DDX_Text(pDX, IDC_EDIT_OUTTER_RATE, m_dOutterRate);
	DDX_Text(pDX, IDC_EDIT_OUTTER_TOTAL_BPS, m_strOutterTotalBPS);
	DDX_Text(pDX, IDC_EDIT_OUTTER_TOTAL_PPS, m_strOutterTotalPPS);
	DDX_Text(pDX, IDC_EDIT_OUTTER_TRANS_BPS, m_strOutterTransBPS);
	DDX_Text(pDX, IDC_EDIT_OUTTER_TRANS_PPS, m_strOutterTransPPS);

	DDX_Text(pDX, IDC_EDIT_TOTAL_TOTAL, m_strTotalTotal);
	DDX_Text(pDX, IDC_EDIT_TOTAL_TRANS, m_strTotalTrans);
	DDX_Text(pDX, IDC_EDIT_TOTAL_RATE, m_dTotalRate);
	DDX_Text(pDX, IDC_EDIT_TOTAL_TOTAL_BPS, m_strTotalTotalBPS);
	DDX_Text(pDX, IDC_EDIT_TOTAL_TOTAL_PPS, m_strTotalTotalPPS);
	DDX_Text(pDX, IDC_EDIT_TOTAL_TRANS_BPS, m_strTotalTransBPS);
	DDX_Text(pDX, IDC_EDIT_TOTAL_TRANS_PPS, m_strTotalTransPPS);
}


BEGIN_MESSAGE_MAP(CIfInfoPage, CPropertyPage)
	ON_WM_TIMER()
END_MESSAGE_MAP()


void CIfInfoPage::GetFriendNumString(ACE_UINT64 num,CString& strnum)
{
	strnum = CPF::GetFriendNumString(num).c_str();

	return;
}

void CIfInfoPage::ShowFlux(DWORD dwtick,
						   const STAT_SI_ITEM& newflux,const STAT_SI_ITEM& oldflux,
						   CString * total_pkts,CString * diff_pkts,CString& bps,CString& pps)
{
	if( total_pkts )
	{
		CIfInfoPage::GetFriendNumString(newflux.ddFrame,*total_pkts);
	}

	if( diff_pkts )
	{
		CIfInfoPage::GetFriendNumString(newflux.ddFrame-oldflux.ddFrame,*diff_pkts);
	}
	
	std::string flux_info;

	CPF::GetFluxInfo(newflux.ddByte,oldflux.ddByte,dwtick,1,&flux_info);

	bps = flux_info.c_str();

	CPF::GetPktSpeed(newflux.ddFrame,oldflux.ddFrame,dwtick,1,&flux_info);

	pps = flux_info.c_str();

	return;

}


// CIfInfoPage message handlers
BOOL CIfInfoPage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_dwNowTick = 0;
	m_dwNowLogTick = 0;

	m_InnerIfFlux.ddFrame = m_InnerIfFlux.ddByte = 0;
	m_InnerTransFlux.ddFrame = m_InnerTransFlux.ddByte = 0;

	m_OutterIfFlux.ddFrame = m_OutterIfFlux.ddByte = 0;
	m_OutterTransFlux.ddFrame = m_OutterTransFlux.ddByte = 0;

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIfInfoPage::OnTimer(UINT_PTR nIDEvent)
{
	if( 1 == nIDEvent )
	{
		ShowStat(GetTickCount());	
	
		UpdateData(FALSE);
	}
	else if( 2 == nIDEvent )
	{
		if( m_log_flux )
		{
			ShowLogStat(GetTickCount());
		}
	}

	CDialog::OnTimer(nIDEvent);

	return;
}


void CIfInfoPage::ShowStat(DWORD nowtick)
{
	if( 0 == m_dwNowTick )
	{
		m_dwNowTick = nowtick;
		return;
	}

	STAT_SI_ITEM	TotalIfFlux;
	STAT_SI_ITEM	TotalTransFlux;

	{
		STAT_SI_ITEM item_if;

		FLUX_CONTROL_GetDevIfFlux(item_if,m_nLineIndex,DIR_I2O);

		ShowFlux(nowtick-m_dwNowTick,item_if,m_InnerIfFlux,&m_strInnerTotal,NULL,m_strInnerTotalBPS,m_strInnerTotalPPS);


		STAT_SI_ITEM item_trans;

		FLUX_CONTROL_GetDevTransFlux(item_trans,m_nLineIndex,DIR_I2O);

		ShowFlux(nowtick-m_dwNowTick,item_trans,m_InnerTransFlux,&m_strInnerTrans,NULL,m_strInnerTransBPS,m_strInnerTransPPS);

		m_dInnerRate = GetRate(m_InnerIfFlux,item_if,m_InnerTransFlux,item_trans);

		m_InnerIfFlux = item_if;

		TotalIfFlux = TotalIfFlux + item_if;

		m_InnerTransFlux = item_trans;

		TotalTransFlux = TotalTransFlux + item_trans;
	}


	{
		STAT_SI_ITEM item_if;

		FLUX_CONTROL_GetDevIfFlux(item_if,m_nLineIndex,DIR_O2I);

		ShowFlux(nowtick-m_dwNowTick,item_if,m_OutterIfFlux,&m_strOutterTotal,NULL,m_strOutterTotalBPS,m_strOutterTotalPPS);


		STAT_SI_ITEM item_trans;

		FLUX_CONTROL_GetDevTransFlux(item_trans,m_nLineIndex,DIR_O2I);

		ShowFlux(nowtick-m_dwNowTick,item_trans,m_OutterTransFlux,&m_strOutterTrans,NULL,m_strOutterTransBPS,m_strOutterTransPPS);

		m_dOutterRate = GetRate(m_OutterIfFlux,item_if,m_OutterTransFlux,item_trans);

		m_OutterIfFlux = item_if;

		TotalIfFlux = TotalIfFlux + item_if;

		m_OutterTransFlux = item_trans;

		TotalTransFlux = TotalTransFlux + item_trans;
	}

	{
		ShowFlux(nowtick-m_dwNowTick,TotalIfFlux,m_TotalIfFlux,&m_strTotalTotal,NULL,m_strTotalTotalBPS,m_strTotalTotalPPS);

		ShowFlux(nowtick-m_dwNowTick,TotalTransFlux,m_TotalTransFlux,&m_strTotalTrans,NULL,m_strTotalTransBPS,m_strTotalTransPPS);

		m_dTotalRate = GetRate(m_TotalIfFlux,TotalIfFlux,m_TotalTransFlux,TotalTransFlux);

		m_TotalIfFlux = TotalIfFlux;
		m_TotalTransFlux = TotalTransFlux;

	}

	UpdateData(false);

	m_dwNowTick = nowtick;


}

void CIfInfoPage::ShowLogStat(DWORD nowtick)
{
	if( 0 == m_dwNowLogTick )
	{
		m_dwNowLogTick = nowtick;
		return;
	}

	CString strInnerTotal_log;
	CString strInnerTotalBPS_log;
	CString strInnerTotalPPS_log;

	CString strInnerTrans_log;
	CString strInnerTransBPS_log;
	CString strInnerTransPPS_log;

	{
		STAT_SI_ITEM item_if;

		FLUX_CONTROL_GetDevIfFlux(item_if,m_nLineIndex,DIR_I2O);

		ShowFlux(nowtick-m_dwNowLogTick,item_if,m_InnerIfFlux_log,NULL,&strInnerTotal_log,strInnerTotalBPS_log,strInnerTotalPPS_log);

		STAT_SI_ITEM item_trans;

		FLUX_CONTROL_GetDevTransFlux(item_trans,m_nLineIndex,DIR_I2O);

		ShowFlux(nowtick-m_dwNowLogTick,item_trans,m_InnerTransFlux_log,NULL,&strInnerTrans_log,strInnerTransBPS_log,strInnerTransPPS_log);

		m_InnerIfFlux_log = item_if;

		m_InnerTransFlux_log = item_trans;
	}

	CString strOutterTotal_log;
	CString strOutterTotalBPS_log;
	CString strOutterTotalPPS_log;

	CString strOutterTrans_log;
	CString strOutterTransBPS_log;
	CString strOutterTransPPS_log;

	{
		STAT_SI_ITEM item_if;

		FLUX_CONTROL_GetDevIfFlux(item_if,m_nLineIndex,DIR_O2I);

		ShowFlux(nowtick-m_dwNowLogTick,item_if,m_OutterIfFlux_log,NULL,&strOutterTotal_log,strOutterTotalBPS_log,strOutterTotalPPS_log);

		STAT_SI_ITEM item_trans;

		FLUX_CONTROL_GetDevTransFlux(item_trans,m_nLineIndex,DIR_O2I);

		ShowFlux(nowtick-m_dwNowLogTick,item_trans,m_OutterTransFlux_log,NULL,&strOutterTrans_log,strOutterTransBPS_log,strOutterTransPPS_log);

		m_OutterIfFlux_log = item_if;

		m_OutterTransFlux_log = item_trans;
	}

	char buf[1024];

	sprintf(buf,"lan-接收:pkts=%s;bps=%s;pps=%s.\nlan-转发:pkts=%s;bps=%s;pps=%s.\nwan-接收:pkts=%s;bps=%s;pps=%s.\nwan-转发:pkts=%s;bps=%s;pps=%s.\n",
		strInnerTotal_log.GetString(),strInnerTotalBPS_log.GetString(),strInnerTotalPPS_log.GetString(),
		strInnerTrans_log.GetString(),strInnerTransBPS_log.GetString(),strInnerTransPPS_log.GetString(),
		strOutterTotal_log.GetString(),strOutterTotalBPS_log.GetString(),strOutterTotalPPS_log.GetString(),
		strOutterTrans_log.GetString(),strOutterTransBPS_log.GetString(),strOutterTransPPS_log.GetString());

	MY_ACE_DEBUG(&m_log_file,
		(LM_ERROR,
		ACE_TEXT("\n%D:line_index=%d:\n%s"),m_nLineIndex,buf
		));

	m_dwNowLogTick = nowtick;

	return;

}
double CIfInfoPage::GetRate(const STAT_SI_ITEM& old_orign,const STAT_SI_ITEM& new_orign,
							const STAT_SI_ITEM& old_trans,const STAT_SI_ITEM& new_trans)
{
	double drate = 0;

	if( new_orign.ddFrame - old_orign.ddFrame == 0 )
	{
		drate = 0;
	}
	else
	{
		drate = (double)(new_trans.ddFrame - old_trans.ddFrame) *100/(new_orign.ddFrame - old_orign.ddFrame);
	}

	return drate;
}

void CIfInfoPage::start_test()
{
	m_dwNowTick = 0;
	m_dwNowLogTick = 0;

	m_InnerIfFlux.ddFrame = m_InnerIfFlux.ddByte = 0;
	m_InnerTransFlux.ddFrame = m_InnerTransFlux.ddByte = 0;

	m_OutterIfFlux.ddFrame = m_OutterIfFlux.ddByte = 0;
	m_OutterTransFlux.ddFrame = m_OutterTransFlux.ddByte = 0;

	m_InnerIfFlux_log.ddFrame = m_InnerIfFlux_log.ddByte = 0;
	m_InnerTransFlux_log.ddFrame = m_InnerTransFlux_log.ddByte = 0;

	m_OutterIfFlux_log.ddFrame = m_OutterIfFlux_log.ddByte = 0;
	m_OutterTransFlux_log.ddFrame = m_OutterTransFlux_log.ddByte = 0;

	return;
}

void CIfInfoPage::stop_test()
{
	return;
}

void CIfInfoPage::StartTimer()
{
	start_test();

	SetTimer(1,2000,NULL);

	SetTimer(2,1*60*1000,NULL);

}

void CIfInfoPage::StopTimer()
{
	KillTimer(1);
	KillTimer(2);

}