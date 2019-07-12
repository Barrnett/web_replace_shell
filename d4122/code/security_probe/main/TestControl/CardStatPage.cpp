// CardStatPage.cpp : implementation file
//

#include "stdafx.h"
#include "TestControl.h"
#include "CardStatPage.h"
#include "cpf/other_tools.h"
#include "cpf/flux_tools.h"
#include "flux_control.h"

// CCardStatPage dialog

IMPLEMENT_DYNAMIC(CCardStatPage, CPropertyPage)

CCardStatPage::CCardStatPage()
	: CPropertyPage(CCardStatPage::IDD)
{

}

CCardStatPage::~CCardStatPage()
{
}

void CCardStatPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CARD, m_list_stat_table);

}


BEGIN_MESSAGE_MAP(CCardStatPage, CPropertyPage)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCardStatPage message handlers
BOOL CCardStatPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	InitListCtrl();

	return true;
}


void CCardStatPage::InitListCtrl()
{
	m_list_stat_table.SetExtendedStyle(
		(m_list_stat_table.GetExtendedStyle() 
		| LVS_EX_FULLROWSELECT 
		| LVS_EX_GRIDLINES));


	{
		int index = 0;
	
		m_list_stat_table.InsertColumn(index++,"卡号",LVCFMT_LEFT,60);

		m_list_stat_table.InsertColumn(index++,"名称",LVCFMT_LEFT,120);

		CRect rect;
		m_list_stat_table.GetClientRect(rect);

		int col_width = (rect.Width()-60-120-1)/4;

		m_list_stat_table.InsertColumn(index++,"接收bps",LVCFMT_LEFT,col_width);
		m_list_stat_table.InsertColumn(index++,"发送bps",LVCFMT_LEFT,col_width);

		m_list_stat_table.InsertColumn(index++,"接收pps",LVCFMT_LEFT,col_width);
		m_list_stat_table.InsertColumn(index++,"发送pps",LVCFMT_LEFT,col_width);
	}
}

void CCardStatPage::OnTimer(UINT_PTR nIDEvent)
{
	if( 1 == nIDEvent )
	{
		RefreshListCtrl();
	}
	else if( 2 == nIDEvent )
	{

	}

	CDialog::OnTimer(nIDEvent);

	return;
}

void CCardStatPage::start_test()
{

}

void CCardStatPage::stop_test()
{

}


void CCardStatPage::StartTimer()
{
	SetTimer(1,2000,NULL);

}

void CCardStatPage::StopTimer()
{
	KillTimer(1);

}

void CCardStatPage::RefreshListCtrl()
{
	int card_nums = FLUX_CONTROL_GetCardNums();
	
	m_list_stat_table.DeleteAllItems();

	for(int i = 0; i < card_nums; ++i)
	{
		std::string card_name;
		if( !FLUX_CONTROL_GetCardStat(i,m_diffStat[i],&card_name) )
		{
			memset(&m_diffStat[i],0x00,sizeof(m_diffStat[i]));
		}

		char index_buf[32] = {0};
		sprintf(index_buf,"%d",i+1);

		int nitem = m_list_stat_table.InsertItem(m_list_stat_table.GetItemCount(),index_buf);

		int col = 1;

		m_list_stat_table.SetItemText(nitem,col++,card_name.c_str());

		m_list_stat_table.SetItemText(nitem,col++,CPF::GetFluxInfo(m_diffStat[i].ddRecvByte*8/5,0).c_str());
		m_list_stat_table.SetItemText(nitem,col++,CPF::GetFluxInfo(m_diffStat[i].ddSendByte*8/5,0).c_str());

		m_list_stat_table.SetItemText(nitem,col++,CPF::GetFriendNumString(m_diffStat[i].ddRecvFrame/5).c_str());
		m_list_stat_table.SetItemText(nitem,col++,CPF::GetFriendNumString(m_diffStat[i].ddSendFrame/5).c_str());
	}

	return;
}


