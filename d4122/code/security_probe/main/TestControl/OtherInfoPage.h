#pragma once


// COtherInfoPage dialog
#include "cpf/ostypedef.h"
#include "cpf/ACE_Log_File_Msg.h"
#include "PacketIO/CommonMainCtrl.h"

class COtherInfoPage : public CPropertyPage
{
	DECLARE_DYNAMIC(COtherInfoPage)

public:
	COtherInfoPage(ACE_Log_File_Msg& log_file,
		CCommonMainCtrl& main_ctrl,
		int log_flux);

	virtual ~COtherInfoPage();

// Dialog Data
	enum { IDD = IDD_OTHER_INFO };

public:
	void start_test();
	void stop_test();

public:
	void StartTimer();
	void StopTimer();

private:
	DWORD				m_dwNowTick;
	DWORD				m_dwBegingTickcount;

private:
	ACE_UINT32			m_max_recv_buff;
	ACE_UINT32			m_max_send_buff;
	ACE_UINT32			m_max_pa_share_buff;

	int					m_nCalBuf_nums;
	ACE_UINT64			m_total_recv_buff;
	ACE_UINT64			m_total_send_buff;
	ACE_UINT64			m_total_pa_share_buff;

	ACE_UINT32			m_recv_alloc_buff;
	ACE_UINT32			m_send_alloc_buff;
	ACE_UINT32			m_pa_share_alloc_buff;

private:
	int					m_log_flux;

private:
	void ShowLinkInfo(DWORD dwtick);

	void ShowStat(DWORD dwtick);

	void GetFriendNumString(ACE_UINT64 num,CString& strnum);

	void LogBufInfo();

private:

	void	GetRunTimeInfo();
public:
	CString			m_strRecvBuffer;
	CString			m_strSendBuffer;

	CString			m_strMaxRecvBuffer;
	CString			m_strMaxSendBuffer;

	CString			m_strTimeSpendPrg;
	CString			m_strTimeSpendSys;

	CString m_strPAShareBuffer;
	CString	m_strMaxPAShareBuffer;

	UINT m_nTCPLinkTotal;
	UINT m_nTCPLinkCur;
	UINT m_nTCPLinkMax;

	UINT m_nUDPLinkTotal;
	UINT m_nUDPLinkCur;
	UINT m_nUDPLinkMax;

	UINT m_nTCPLinkTotal_dmz;
	UINT m_nTCPLinkCur_dmz;
	UINT m_nTCPLinkMax_dmz;

	UINT m_nUDPLinkTotal_dmz;
	UINT m_nUDPLinkCur_dmz;
	UINT m_nUDPLinkMax_dmz;


	CString m_strDroppedFCPkts;
	CString m_strDroppedPAPkts;

private:
	ACE_Log_File_Msg&	m_log_file;
	CCommonMainCtrl&	m_main_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBnClickedButtonClearPaRuninfo();

public:
	ULONGLONG m_ullReadLoops;//记录上一次循环次数
	ULONGLONG m_ullTimerPackets;//记录上一次定时包数
	ULONGLONG m_ullTotalPackets;

	BOOL	m_bEmptyTimerPkts;
};
