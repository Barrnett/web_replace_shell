
// 20101123
// 修改为在linux下运行，不在具有dialog的状态，只是一个普通类，不使用OnTimer，完全有外部调用
// 将所有在界面呈现的东西，修改成printf输出和日志输出
//#include "stdafx.h"
//#include "TestControl.h"
#include "IfInfoPage.h"
#include "cpf/other_tools.h"
#include "flux_control.h"


// CIfInfoPage dialog

//IMPLEMENT_DYNAMIC(CIfInfoPage, CPropertyPage)

CIfInfoPage::CIfInfoPage(ACE_Log_File_Msg& log_file,int line_index,int log_flux)
	: m_log_file(log_file)
	, m_strInnerTotal(_T("0"))
	, m_strInnerTrans(_T("0"))
	, m_dInnerRate(0)
	, m_strOutterTotal(_T("0"))
	, m_strOutterTrans(_T("0"))
	, m_dOutterRate(0)
	, m_dTotalRate(0)
{
	m_nLineIndex = line_index;

	m_iSecondCount = 0;
	m_bStartTimer = false;
}

CIfInfoPage::~CIfInfoPage()
{
}



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
//	CDialog::OnInitDialog();

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
	
//		UpdateData(FALSE);
	}
	else if( 2 == nIDEvent )
	{
		if( m_log_flux )
		{
			ShowLogStat(GetTickCount());
		}
	}

//	CDialog::OnTimer(nIDEvent);

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

//	UpdateData(false);

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

	sprintf(buf,"LAN-Recv: pkts=%s;\tbps=%s;\tpps=%s.\t"
				"LAN-Send: pkts=%s;\tbps=%s;\tpps=%s.\t"
				"WAN-Recv: pkts=%s;\tbps=%s;\tpps=%s.\t"
				"WAN-Send: pkts=%s;\tbps=%s;\tpps=%s.",
		strInnerTotal_log.GetString(),strInnerTotalBPS_log.GetString(),strInnerTotalPPS_log.GetString(),
		strInnerTrans_log.GetString(),strInnerTransBPS_log.GetString(),strInnerTransPPS_log.GetString(),
		strOutterTotal_log.GetString(),strOutterTotalBPS_log.GetString(),strOutterTotalPPS_log.GetString(),
		strOutterTrans_log.GetString(),strOutterTransBPS_log.GetString(),strOutterTransPPS_log.GetString());

	MY_ACE_DEBUG(&m_log_file,
		(LM_ERROR,
		ACE_TEXT("[Info][%D]line_index=%d:%s\n"),m_nLineIndex,buf
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

//	SetTimer(1,2000,NULL);

//	SetTimer(2,1*60*1000,NULL);

	m_iSecondCount = 0;
	m_bStartTimer = true;
	return;
}

void CIfInfoPage::stop_test()
{
//	KillTimer(1);
//	KillTimer(2);
	m_bStartTimer = false;
	return;
}
void CIfInfoPage::OneSecondCall()
{
	if (!m_bStartTimer)
		return;

	m_iSecondCount ++;
/*
	if (m_iSecondCount % 2 == 0)	// 屏幕输出部分没用
	{
		OnTimer(1);
	}
*/

	if (m_iSecondCount % (5*60) == 0)
	{
		OnTimer(2);

		RefreshListCtrl();
	}
	return;
	//	SetTimer(1,2000,NULL);
	//	SetTimer(2,1*60*1000,NULL);
}


void CIfInfoPage::RefreshListCtrl()
{
	std::vector<STAT_TABLE_INFO> vt_tab_info;

	FLUX_CONTROL_GetStatTable(vt_tab_info);

	MY_ACE_DEBUG(&m_log_file, (LM_ERROR, ACE_TEXT("\n")));
	for(size_t i = 0; i < vt_tab_info.size(); ++i)
	{
		const STAT_TABLE_INFO& info = vt_tab_info[i];

		double daverage = 0;		
		if( info.row_nums > 0 )
			daverage = ((double)info.total_nums)/info.row_nums;

		MY_ACE_DEBUG(&m_log_file, (LM_ERROR, ACE_TEXT
			("[Info][%D]table name = %-30s\t len = %-3d\t sum nodes= %-3d\t Average = %.1f\n"),
			info.tab_name.c_str(), info.row_nums, info.total_nums, daverage));

	}
	MY_ACE_DEBUG(&m_log_file, (LM_ERROR, ACE_TEXT("\n")));

	return;
}