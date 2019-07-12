#pragma once
#include "afxwin.h"


// CCapConfigDlg dialog
#include "PacketIO/DataSend_Param_Info.h"
#include "Flux_Control_Common_Struct.h"

class CCapConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CCapConfigDlg)

public:
	CCapConfigDlg(const std::vector<std::string>& vt_cardinfo,CWnd* pParent = NULL);   // standard constructor
	virtual ~CCapConfigDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CAP_CONFIG };

public:
	Cpt_Write_Param			m_cpt_write_param;
	Ethereal_Write_Param	m_ethreal_write_param;

	BOOL			m_bCapping;

public:
	CString			m_str_save_path;

private:
	DWORD			m_begin_cap_time;

private:
	std::vector<std::string>	m_vt_cardinfo;

private:
	void EnableCtrls();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonConfigFlt();
	afx_msg void OnBnClickedButtonFileConfig();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonStopCap();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL m_bFilter;
	afx_msg void OnBnClickedRadioCpt();
	afx_msg void OnBnClickedRadioEthreal();
	int m_cap_file_type;//0表示cpt，1表示ethreal

	CCheckListBox	m_listbox_card;
	
};
