#pragma once


#include "PacketIO/DataSendMgr.h"
// CCptDstFileDlg dialog

class CCptDstFileDlg : public CDialog
{
	DECLARE_DYNAMIC(CCptDstFileDlg)

public:
	CCptDstFileDlg(Cpt_Write_Param& cpt_write_param,CWnd* pParent = NULL);   // standard constructor
	virtual ~CCptDstFileDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CPT_DST_FILE };

public:
	Cpt_Write_Param& m_cpt_write_param;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	CString m_strSaveFilePath;
public:
	CString m_strSaveFileName;
public:
	UINT m_max_onefile_size;
public:
	UINT m_num_files;
public:
	UINT m_nSaveTime;
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedButtonBrowser();
	UINT m_nBufferSizeMB;
	afx_msg void OnBnClickedButtonDefalutName();
};
