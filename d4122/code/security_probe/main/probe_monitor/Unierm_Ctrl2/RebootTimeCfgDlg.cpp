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

		AfxMessageBox("���ܳ���40��!",MB_OK|MB_ICONINFORMATION);

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
			AfxMessageBox("����ɹ�!");
		}
		else
		{
			AfxMessageBox("����ʧ��!");
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
	if( IDNO == AfxMessageBox("�ù���ֻ���ڳ��ҵ���ʹ�ã��ͻ��벻Ҫʹ��!\n\nϵͳ����������,�Ƿ����?",MB_YESNO|MB_ICONQUESTION) )
		return;

	if( IDNO == AfxMessageBox("ϵͳ����30�������,�Ƿ�ȷ��������",MB_YESNO|MB_ICONQUESTION) )
		return;

	m_pMonitorSvc->Reboot(30);

	return;
}
