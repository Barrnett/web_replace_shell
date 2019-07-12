// FCServerDlg.h : header file
//

#pragma once

#include "FCServerFacadeEx.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "fcserver.h"
#include "cpf/my_event.h"

//class CProbeNetCom;


// CFCServerDlg dialog
class CFCServerDlg
{
// Construction
public:
	CFCServerDlg(BOOL b_restore_orig_prg, const char * pCommandLine);


private:
	CFCServerFacadeEx		m_fcServer;

	ACE_Log_File_Msg		m_log_file;

private:
	std::string			m_strCmdLine;
	BOOL				m_b_restore_orig_prg;

	my_event_t * Create_Stop_Event(ACE_Log_File_Msg * loginstance,const char *stop_event_name );

	int CheckStopEvent();

	my_event_t *		m_pStopEvent;


private:
	void	GetRunTimeInfo();

private:
	DWORD	m_dwBegingTickcount;

	CStr		m_strTitle_Version;


	void SetWindowText(const char* pstrText)
	{
		MY_ACE_DEBUG(&m_log_file, (LM_INFO, pstrText) );
	}

	// add by xumx, 2016-12-12
	char *f_get_system_version();
	char m_version_string[64];

	// Generated message map functions
public:
	BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CString m_strTimeSpend;
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonStopAndExit();

	// 原OnTimer调用，修改到这里
	void OneSecondCall();
	int		m_iSecondCount;
	bool	m_bStartTimer;

	// 特殊的，需要将原来的StopEvent增加标记
	bool	m_bStopEvent;

};
