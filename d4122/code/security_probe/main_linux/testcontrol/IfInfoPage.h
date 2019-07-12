#pragma once

// CStatTablePage的功能太简单了，也放在这里实现吧

// CIfInfoPage dialog
#include "testcontrol.h"
#include "cpf/flux_tools.h"
#include "cpf/ACE_Log_File_Msg.h"

class CIfInfoPage	// : public CPropertyPage
{
//	DECLARE_DYNAMIC(CIfInfoPage)

public:
	CIfInfoPage(ACE_Log_File_Msg& log_file,int line_index,int log_flux);
	virtual ~CIfInfoPage();

// Dialog Data
//	enum { IDD = IDD_IF_INFO };

public:
	void start_test();
	void stop_test();

	// 原OnTimer调用，修改到这里
	void OneSecondCall();
	int		m_iSecondCount;
	bool	m_bStartTimer;
	void RefreshListCtrl();
public:
	//入口
	CString m_strInnerTotal;
	CString m_strInnerTrans;
	double m_dInnerRate;

	CString m_strInnerTotalBPS;
	CString m_strInnerTotalPPS;
	CString m_strInnerTransBPS;
	CString m_strInnerTransPPS;

	//出口
	CString m_strOutterTotal;
	CString m_strOutterTrans;
	double m_dOutterRate;

	CString m_strOutterTotalBPS;
	CString m_strOutterTotalPPS;
	CString m_strOutterTransBPS;
	CString m_strOutterTransPPS;

	//总数
	CString m_strTotalTotal;
	CString m_strTotalTrans;
	double m_dTotalRate;

	CString m_strTotalTotalBPS;
	CString m_strTotalTotalPPS;
	CString m_strTotalTransBPS;
	CString m_strTotalTransPPS;

private:
	void ShowStat(DWORD dwtick);
	void ShowLogStat(DWORD dwtick);

	void GetFriendNumString(ACE_UINT64 num,CString& strnum);

	void ShowFlux(DWORD dwtick,
		const STAT_SI_ITEM& newflux,const STAT_SI_ITEM& oldflux,
		CString * total_pkts,CString * diff_pkts,CString& bps,CString& pps);

	static double GetRate(const STAT_SI_ITEM& old_orign,const STAT_SI_ITEM& new_orign,
		const STAT_SI_ITEM& old_trans,const STAT_SI_ITEM& new_trans);

public:
	ACE_Log_File_Msg&	m_log_file;

private:
	DWORD	m_dwNowTick;
	DWORD	m_dwNowLogTick;

	int		m_nLineIndex;

private:
	STAT_SI_ITEM	m_InnerIfFlux;
	STAT_SI_ITEM	m_InnerTransFlux;

	STAT_SI_ITEM	m_OutterIfFlux;
	STAT_SI_ITEM	m_OutterTransFlux;

	STAT_SI_ITEM	m_TotalIfFlux;
	STAT_SI_ITEM	m_TotalTransFlux;

private:
	//log的上一次的流量变量
	STAT_SI_ITEM	m_InnerIfFlux_log;
	STAT_SI_ITEM	m_InnerTransFlux_log;

	STAT_SI_ITEM	m_OutterIfFlux_log;
	STAT_SI_ITEM	m_OutterTransFlux_log;

//protected:
public:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

//	DECLARE_MESSAGE_MAP()

	afx_msg void OnTimer(UINT_PTR nIDEvent);

};

typedef CIfInfoPage * LPIfInfoPage;