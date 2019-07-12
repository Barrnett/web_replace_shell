//////////////////////////////////////////////////////////////////////////
//RebootTimeCfgDlg.h

#pragma once


// CRebootTimeCfgDlg dialog
class MonitorSvc;

class CRebootTimeCfgDlg : public CDialog
{
	DECLARE_DYNAMIC(CRebootTimeCfgDlg)

public:
	CRebootTimeCfgDlg(MonitorSvc * psvc,CWnd* pParent = NULL);   // standard constructor
	virtual ~CRebootTimeCfgDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_REBOOT_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	MonitorSvc *	m_pMonitorSvc;


public:
	int m_nRebootPeriodDay;
	CTime m_reboot_time;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonTestReboot();
	int m_nReboot_Timeout_Param;
};
