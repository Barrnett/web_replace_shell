// TestControlDlg.h : header file
//

#pragma once

#include "PacketIO/CommonMainCtrl.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "afxwin.h"
#include "flux_control.h"
#include "PacketIO/EmptyDataSource.h"

class CTabWndEx;
class CCardStatPage;
class COtherInfoPage;


class CTestControlDlg;

class CTestControlRebootHandler : public IFCRebootHandler
{
public:
	CTestControlRebootHandler(CTestControlDlg * pDlg)
	{
		m_pMainDlg = pDlg;
	}

	virtual ~CTestControlRebootHandler(){}

public:
	virtual int Do(int type,int reason);

private:
	CTestControlDlg *	m_pMainDlg;
};


class CVersionCompInfo;

// CTestControlDlg dialog
class CTestControlDlg : public CDialog
{
// Construction
public:
	CTestControlDlg(const char * pCommandLine,CWnd* pParent = NULL);	// standard constructor
	virtual ~CTestControlDlg();

// Dialog Data
	enum { IDD = IDD_TESTCONTROL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	int RebootHandler(int type,int reason);

private:
	CCommonMainCtrl		m_main_ctrl;


private:
	std::string			m_strCmdLine;

	int CheckStopEvent();

private:
	BOOL				m_bStartTimer;

private:
	BOOL CreateEmpyeDataSource();
	void DestroyEmpyeDataSource();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg LRESULT OnRebootHandlerMsg(WPARAM wparam,LPARAM lparam);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonStopAndExit();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	CEmptyDataSource * m_pEmptyDataSouce;

private:
	CTabWndEx *	m_pWndTabs;

	CCardStatPage *		m_pCardStatPage;

	COtherInfoPage *	m_pOtherInfoPage;

	void CreateIfInfoPage();
	void DestroyIfInfoPage();

private:
	void stop_test();
	int start_test();

	enum{
		IIMER_CHECK_STOP_EVENT = 2,
		TIMER_EMPTY_DATA_SOURCE = 3
	};

private:
	CTestControlRebootHandler m_TestControlRebootHandler;

public:
	ACE_Log_File_Msg*	m_p_log_file;

	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonChannelFlux();
	afx_msg void OnBnClickedButtonCtrlParam();
	afx_msg void OnBnClickedButtonCapData();

	afx_msg void OnBnClickedButtonSaveAutoamp();
	afx_msg void OnBnClickedButtonClearAutoamp();
	afx_msg void OnBnClickedButtonOutPut();
	afx_msg void OnBnClickedButtonStartTimer();
	afx_msg void OnBnClickedButtonStopTimer();
};
