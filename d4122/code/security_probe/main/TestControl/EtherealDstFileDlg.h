#pragma once

#include "PacketIO/DataSendMgr.h"
// CEtherealDstFileDlg dialog

class CEtherealDstFileDlg : public CDialog
{
	DECLARE_DYNAMIC(CEtherealDstFileDlg)

public:
	CEtherealDstFileDlg(Ethereal_Write_Param& ethreal_write_param,CWnd* pParent = NULL);   // standard constructor
	virtual ~CEtherealDstFileDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ETHEREAL_DST_FILE };

private:
	Ethereal_Write_Param& m_ethreal_write_param;

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
	afx_msg void OnBnClickedButtonDefalutName();
};
