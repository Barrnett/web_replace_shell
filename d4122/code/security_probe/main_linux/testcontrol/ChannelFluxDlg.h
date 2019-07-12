#pragma once
//#include "afxcmn.h"


// CChannelFluxDlg dialog
#include "Flux_Control_Common_Struct.h"
#include "testcontrol.h"

class CBucketChannel;

class CChannelFluxDlg	// : public CPropertyPage
{
//	DECLARE_DYNAMIC(CChannelFluxDlg)

public:
	CChannelFluxDlg();   // standard constructor
	virtual ~CChannelFluxDlg();

// Dialog Data
//	enum { IDD = IDD_DIALOG_CHANNEL_FLUX };

private:
//	void InitListCtrl();


private:
	unsigned int GetChannelIdByIndex(int index);

	DWORD					m_dwLastTick;

	UINT_PTR				m_nTimer;

	void GetChannelBandInfo(const CBucketChannel * pChannel,
		unsigned int& min_band_up,unsigned int& max_band_up,
		unsigned int& min_band_down,unsigned int& max_band_down);

private:
	void CollectChannelInfo();

	CHANNEL_PERF_STAT_EX *	m_pOldStat;

	std::vector< std::pair<unsigned int,std::string> >	m_vt_channel_id;

//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

//	DECLARE_MESSAGE_MAP()
public:
	BOOL OnInitDialog();
//	CListCtrl m_listChannel;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	UINT m_nTimerInterval;
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonRefresh();
};
