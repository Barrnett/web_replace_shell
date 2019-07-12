// CtrlParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestControl.h"
#include "CtrlParamDlg.h"

#include "flux_control.h"
#include "tcpip_mgr_common_init.h"

#include "TestControlParam.h"

// CCtrlParamDlg dialog

IMPLEMENT_DYNAMIC(CCtrlParamDlg, CDialog)

CCtrlParamDlg::CCtrlParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCtrlParamDlg::IDD, pParent)
	, m_b_save_fc_stat(FALSE)
	, m_b_save_in_dmz_stat(FALSE)
	, m_b_save_outter_dmz_stat(FALSE)
	, m_save_web_contravene_stat(0)
	, m_bResetWDTInUI(FALSE)
	, m_bResetWDTInLoop(FALSE)
	, m_live_log_level(0)
	, m_bSyncAddStat(FALSE)
	, m_bSyncDelStat(FALSE)
	, m_bTraceCode(false)
{

}

CCtrlParamDlg::~CCtrlParamDlg()
{
}

void CCtrlParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Check(pDX, IDC_CHECK_RESET_WDT_UI, m_bResetWDTInUI);
	DDX_Check(pDX, IDC_CHECK_RESET_WDT_LOOP, m_bResetWDTInLoop);
	DDX_Text(pDX, IDC_EDIT_LIVE_LOG_LEVEL, m_live_log_level);
	DDV_MinMaxInt(pDX, m_live_log_level, -1, 10000000);
	DDX_Check(pDX, IDC_CHECK_SYNC_ADD_STAT, m_bSyncAddStat);
	DDX_Check(pDX, IDC_CHECK_SYNC_DEL_STAT, m_bSyncDelStat);

	DDX_Check(pDX, IDC_CHECK_FC_OUTPUT, m_b_save_fc_stat);
	DDX_Check(pDX, IDC_CHECK_WEBCONTRAVENE_OUTPUT, m_save_web_contravene_stat);
	DDX_Check(pDX, IDC_CHECK_IN_DMZ_FC_OUTPUT, m_b_save_in_dmz_stat);
	DDX_Check(pDX, IDC_CHECK_OUT_DMZ_FC_OUTPUT, m_b_save_outter_dmz_stat);
	DDX_Check(pDX, IDC_CHECK_TRACE_CODE, m_bTraceCode);

}


BEGIN_MESSAGE_MAP(CCtrlParamDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCtrlParamDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCtrlParamDlg message handlers

BOOL CCtrlParamDlg::OnInitDialog()
{
	m_live_log_level = CCTestControlParam_Singleton::instance()->m_live_log_level;

	m_b_save_fc_stat = false;
	m_b_save_in_dmz_stat = false;
	m_b_save_outter_dmz_stat = false;

	m_bTraceCode = CCTestControlParam_Singleton::instance()->m_bTraceCode;

	m_bSyncAddStat = CCTestControlParam_Singleton::instance()->m_bSyncOnAddStat;
	m_bSyncDelStat = CCTestControlParam_Singleton::instance()->m_bSyncOnDelStat;

	m_bResetWDTInUI = CCTestControlParam_Singleton::instance()->m_bResetWDTInLoop;
	m_bResetWDTInLoop = CCTestControlParam_Singleton::instance()->m_bResetWDTInLoop;

	m_save_web_contravene_stat = FALSE;

	CDialog::OnInitDialog();


	return true;
}

void CCtrlParamDlg::OnBnClickedOk()
{
	UpdateData(true);

	CCTestControlParam_Singleton::instance()->m_bTraceCode = m_bTraceCode;
	CCTestControlParam_Singleton::instance()->m_bSyncOnAddStat = m_bSyncAddStat;
	CCTestControlParam_Singleton::instance()->m_bSyncOnDelStat = m_bSyncDelStat;
	CCTestControlParam_Singleton::instance()->m_live_log_level = g_live_log_level = m_live_log_level;

	CCTestControlParam_Singleton::instance()->m_bResetWDTInLoop = m_bResetWDTInUI;
	CCTestControlParam_Singleton::instance()->m_bResetWDTInLoop = m_bResetWDTInLoop;

	CCTestControlParam_Singleton::instance()->Write();

	OnOK();

	return;
}

