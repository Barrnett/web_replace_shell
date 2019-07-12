// TestControlDlg.h : header file
//

#pragma once

#include "PacketIO/CommonMainCtrl.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "flux_control.h"
#include "PacketIO/EmptyDataSource.h"

//class CTabWndEx;
class CIfInfoPage;
//class COtherInfoPage;
//class CStatTablePage;


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



// CTestControlDlg dialog
class CTestControlDlg		// : public CDialog
{
// Construction
public:
	CTestControlDlg(const char * pCommandLine = "");	// standard constructor
	virtual ~CTestControlDlg();

// Dialog Data
//	enum { IDD = IDD_TESTCONTROL_DIALOG };

//	protected:
//	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	int RebootHandler(int type,int reason);

private:
	CCommonMainCtrl		m_main_ctrl;

private:
	std::string			m_strCmdLine;

public:
	int CheckStopEvent();

private:
	BOOL CreateEmpyeDataSource();
	void DestroyEmpyeDataSource();

// Implementation
protected:
//	HICON m_hIcon;

	// Generated message map functions
public:
	BOOL OnInitDialog();
	void OnBnClickedOk();
	void OnBnClickedCancel();
	void OnBnClickedButtonStopAndExit();
	void OnBnClickedButtonStart();
	void OnBnClickedButtonStop();
	void OnTimer(unsigned int  nIDEvent);
	int OnRebootHandlerMsg(int type, int reason);

private:
	CEmptyDataSource * m_pEmptyDataSouce;

private:
// 	CTabWndEx *	m_pWndTabs;
// 
// 	COtherInfoPage *	m_pOtherInfoPage;
// 	CStatTablePage *	m_pStatTablePage;

 	std::vector<CIfInfoPage *>	m_vt_IfInfo_page;

 	void CreateIfInfoPage();
 	void DestroyIfInfoPage();

	// add by xumx, 2016-12-12
	char *f_get_system_version();
	char m_version_string[64];

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
	ACE_Log_File_Msg	m_log_file;
	void OnClose();
/*
	void OnBnClickedButtonChannelFlux();
	void OnBnClickedButtonCtrlParam();
	void OnBnClickedButtonCapData();

	void OnBnClickedButtonSaveAutoamp();
	void OnBnClickedButtonClearAutoamp();
*/

	// 原OnTimer调用，修改到这里
	void OneSecondCall();
	int		m_iSecondCount;
	bool	m_bStartTimer;

	// 特殊的，需要将原来的StopEvent增加标记
	bool	m_bStopEvent;
};
