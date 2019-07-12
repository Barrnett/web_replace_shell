// RebootTimeCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Unierm_Ctrl.h"
#include "RebootTimeCfgDlg.h"
#include "MonitorSvc.h"


// CRebootTimeCfgDlg dialog

IMPLEMENT_DYNAMIC(CRebootTimeCfgDlg, CDialog)

CRebootTimeCfgDlg::CRebootTimeCfgDlg(MonitorSvc * psvc,CWnd* pParent /*=NULL*/)
: CDialog(CRebootTimeCfgDlg::IDD, pParent)
, m_nRebootPeriodDay(0)
, m_reboot_time(0)
, m_pMonitorSvc(psvc)
, m_nReboot_Timeout_Param(0)
{
}

CRebootTimeCfgDlg::~CRebootTimeCfgDlg()
{
}

void CRebootTimeCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PERIOD_DAY, m_nRebootPeriodDay);
	DDX_DateTimeCtrl(pDX, IDC_TIME, m_reboot_time);
	DDX_Text(pDX, IDC_EDIT_REBOOT_TIMEOUT_PARAM, m_nReboot_Timeout_Param);
}


BEGIN_MESSAGE_MAP(CRebootTimeCfgDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CRebootTimeCfgDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRebootTimeCfgDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEST_REBOOT, &CRebootTimeCfgDlg::OnBnClickedButtonTestReboot)
END_MESSAGE_MAP()


// CRebootTimeCfgDlg message handlers
BOOL CRebootTimeCfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ACE_ASSERT(m_pMonitorSvc);

	m_nRebootPeriodDay = m_pMonitorSvc->m_nRebootPeriodDay;
	m_nReboot_Timeout_Param = m_pMonitorSvc->m_nRebootTimeout_Param;

	CTime reboottime(2009,11,3,
		m_pMonitorSvc->m_nRebootHour,
		m_pMonitorSvc->m_nRebootMin,
		m_pMonitorSvc->m_nRebootSec);

	m_reboot_time = reboottime;

	UpdateData(false);

	return true;
}


void CRebootTimeCfgDlg::OnBnClickedOk()
{
	if( !UpdateData(TRUE) )
	{
		return;
	}

	if( m_nRebootPeriodDay > 40 )
	{
		m_nRebootPeriodDay = 40;

		AfxMessageBox("不能超过40天!",MB_OK|MB_ICONINFORMATION);

		UpdateData(false);

		return;
	}

	if( m_pMonitorSvc )
	{
		m_pMonitorSvc->m_nRebootPeriodDay = m_nRebootPeriodDay;

		m_pMonitorSvc->m_nRebootHour = m_reboot_time.GetHour();
		m_pMonitorSvc->m_nRebootMin = m_reboot_time.GetMinute();
		m_pMonitorSvc->m_nRebootSec = m_reboot_time.GetSecond();

		m_pMonitorSvc->m_nRebootTimeout_Param = m_nReboot_Timeout_Param;

		if( m_pMonitorSvc->WriteRebootInfo() )
		{
			AfxMessageBox("保存成功!");
		}
		else
		{
			AfxMessageBox("保存失败!");
		}
	}

	OnOK();

	return;

}

void CRebootTimeCfgDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CRebootTimeCfgDlg::OnBnClickedButtonTestReboot()
{
	if( IDNO == AfxMessageBox("该功能只用于厂家调试使用，客户请不要使用!\n\n系统将立即启动,是否继续?",MB_YESNO|MB_ICONQUESTION) )
		return;

	if( IDNO == AfxMessageBox("系统将在30秒后启动,是否确定继续？",MB_YESNO|MB_ICONQUESTION) )
		return;

	m_pMonitorSvc->Reboot(30);

	return;
}
