#pragma once


// CCardStatPage dialog

#include "cpf/flux_tools.h"

class CCardStatPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CCardStatPage)

public:
	CCardStatPage();
	virtual ~CCardStatPage();

// Dialog Data
	enum { IDD = IDD_CARD_INFO };


private:
	CListCtrl m_list_stat_table;

public:
	void start_test();
	void stop_test();


	void StartTimer();
	void StopTimer();

private:
	void RefreshListCtrl();
	void InitListCtrl();

private:
	STAT_BI_ITEM	m_diffStat[32];


protected:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
