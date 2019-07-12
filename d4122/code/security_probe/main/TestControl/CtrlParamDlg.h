#pragma once

#include "flux_control.h"
// CCtrlParamDlg dialog

class CCtrlParamDlg : public CDialog
{
	DECLARE_DYNAMIC(CCtrlParamDlg)

public:
	CCtrlParamDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCtrlParamDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CTRL_PARAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	BOOL m_b_save_fc_stat;
	BOOL m_b_save_in_dmz_stat;
	BOOL m_b_save_outter_dmz_stat;
	BOOL m_save_web_contravene_stat;

	BOOL m_bTraceCode;

	BOOL m_bResetWDTInUI;
	BOOL m_bResetWDTInLoop;
	int m_live_log_level;
	BOOL m_bSyncAddStat;
	BOOL m_bSyncDelStat;
};
