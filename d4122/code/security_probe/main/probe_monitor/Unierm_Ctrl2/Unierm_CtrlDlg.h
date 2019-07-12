
// Unierm_CtrlDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include "MonitorTask.h"
#include "ace/OS_NS_Thread.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "afxdtctl.h"

class MonitorSvc;

class CUnierm_CtrlDlg : public CDialog
{
// 构造
public:
	CUnierm_CtrlDlg(const char * pCommandLine,CWnd* pParent = NULL);	// 标准构造函数
	~CUnierm_CtrlDlg();

// 对话框数据
	enum { IDD = IDD_UNIERM_CTRL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	MonitorSvc *	m_pMonitorSvc;

	std::vector<MONITOR_TASK>	m_vt_task;

protected:
	void InitListCtrl();
	void RefreshListCtrl();
	void RefreshState();

private:
	CString GetStatString(int state);

private:
	std::string			m_strCmdLine;

	ACE_event_t * Create_Stop_Event(ACE_Log_File_Msg * loginstance,const char *stop_event_name );

	ACE_event_t *		m_pStopEvent;
	ACE_event_t *		m_pMonitorEvent;

	int CheckStopEvent();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listPrg;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonConfig();
	afx_msg void OnBnClickedButtonStopAll();
	afx_msg void OnBnClickedButtonStopAllAndThis();
};
