// 使用时注意：
//   CTabWndEx内的子窗口，即用AddTab()加入的窗口，其父窗口必须为CTabWndEx

#if !defined(AFX_TABWNDEX_H__10E15955_436E_11D2_8BEB_00A0C9B05590__INCLUDED_)
#define AFX_TABWNDEX_H__10E15955_436E_11D2_8BEB_00A0C9B05590__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabWndEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabWndEx window


class CTabInfo;
typedef CArray<CTabInfo*, CTabInfo*&>	CTabInfoArray;

class CPrevAndNextWndMgr;

class CTabWndEx : public CWnd
{

public:
	static void init_clolr();

// Construction
public:
	CTabWndEx(BOOL bSupportLink = false);

	enum Style
	{
		STYLE_3D = 0,						// VC中的WorkSpace风格
		STYLE_FLAT = 1,						// VC中的OutPut风格，不带滚动条
		STYLE_FLAT_SHARED_HORZ_SCROLL = 2	// VC中的OutPut风格，带水平滚动条
											// 消息转发到tab中活动的窗口，自己不处理
	};

	enum Location							// 标签的位置
	{
		LOCATION_BOTTOM = 0,
		LOCATION_TOP = 1
	};

// Operations
public:
	// Create methods:
	BOOL Create (Style style, const RECT& rect, CWnd* pParentWnd, UINT nID,
				Location location = LOCATION_BOTTOM);

	//是否打开邮件关闭tab的菜单
	void EnableCloseMenu(BOOL bEnableCloseMenu)
	{
		m_bEnableCloseMenu = bEnableCloseMenu;
	}

	BOOL SetImageList (UINT uiID, int cx = 15, 
		COLORREF clrTransp = RGB (255, 0, 255));

	void RecalcLayout ();

	// Tab access methods:
	// 只有STYLE_3D风格才能使用uiImageId，否则= -1
	void AddTab (CWnd* pTabWnd, LPCTSTR lpszTabLabel, UINT uiImageId);
	void AddTab (CWnd* pTabWnd, UINT uiResTabLabel, UINT uiImageId);

	int GetTabsNum () const
	{
		return m_iTabsNum;
	}

	BOOL RemoveThisTab (int iTab);
	void RemoveAllTabs ();
	BOOL RemoveOtherTabs (int iTab);
	
	void ActivateNextTab();
	void ActivatePrevTab();
	void CloseOldestTab();

	// Tab properties:
	CWnd* GetTabWnd (int iTab) const;
	BOOL GetTabRect (int iTab, CRect& rect) const;
	BOOL GetTabLabel (int iTab, CString& strLabel) const;
	UINT GetTabIcon (int iTab) const;

	// Tab activation:
	int GetActiveTab () const
	{
		return m_iActiveTab;
	}

	int GetTabIndex(CWnd * pWnd) const;

	CWnd* GetActiveWnd () const;
	BOOL SetActiveTab (int iTab);

	BOOL EnsureVisible (int iTab);

	// Active tab color operations:
	void SetActiveTabColor (COLORREF clr);
	void SetActiveTabTextColor (COLORREF clr);

	COLORREF GetActiveTabColor () const
	{
		return m_clrActiveTabBk == (COLORREF) -1 ?
			::GetSysColor (COLOR_WINDOW) : m_clrActiveTabBk;
	}

	COLORREF GetActiveTabTextColor () const
	{
		return m_clrActiveTabFg == (COLORREF) -1 ?
			::GetSysColor (COLOR_WINDOWTEXT) : m_clrActiveTabFg;
	}

	// Scrolling methods:
	BOOL SynchronizeScrollBar (SCROLLINFO* pScrollInfo = NULL);
	CScrollBar* GetScrollBar ()
	{
		return m_bSharedScroll ? &m_wndScrollWnd : NULL;
	}

	void HideActiveWindowHorzScrollBar ();

protected:
	void TruncateText (CDC* pDC, CString& strText, int nMaxWidth);
	CWnd* FindTargetWnd (const CPoint& pt);

	int FindTableWnd (const CPoint& pt);

	void AdjustTabs ();
	void AdjustTabsScroll ();
	void AdjustWndScroll ();

//	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void SetTabsHeight ();

private:
	CPrevAndNextWndMgr *	m_pPrevAndNextWndMgr;

// Attributes
protected:
	BOOL			m_bEnableCloseMenu;
	BOOL			m_bFlat;			// Is flat (Excell-like) mode
	BOOL			m_bSharedScroll;	// Have a scrollbar shared with active window
	Location		m_location;			// Tabs location

	CTabInfoArray	m_arTabs;			// Array of CTabInfo objects
	int				m_iTabsNum;			// m_arTabs size
	int				m_iActiveTab;		// Active tab number

	int				m_nScrollBarRight;	// Scrollbar right side
	CRect			m_rectTabsArea;		// Tabs area
	CRect			m_rectWndArea;		// Child window area

	int				m_nTabsHeight;		// Tabs area height

	CImageList		m_Images;			// Tab images (for 3d tabs only)
	CSize			m_sizeImage;		// Tab image size

	CToolTipCtrl	m_ToolTip;			// Tab tooltip (for 3d tabs only)
	CScrollBar		m_wndScrollTabs;	// Tabs <-left, ritgh-> arrows
	CScrollBar		m_wndScrollWnd;		// Active window horizontal scroll bar

	int				m_nTabsHorzOffset;
	int				m_nTabsHorzOffsetMax;
	int				m_nTabsTotalWidth;

	int				m_nHorzScrollWidth;	// Shared scrollbar width
	CRect			m_rectTabSplitter;	// Splitter between tabs and scrollbar
	BOOL			m_bTrackSplitter;	// Is splitter tracked?

	COLORREF		m_clrActiveTabBk;	// Active tab backgound color
	COLORREF		m_clrActiveTabFg;	// Active tab foreground color

	CBrush			m_brActiveTab;		// Active tab background brush

	CFont			m_fntTabs;			// Tab font (flat tabs only)
	CFont			m_fntTabsBold;		// Active tab font (flat tabs only)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalendarToolBarWnd)
	public:
	BOOL PreTranslateMessage(MSG* pMsg);
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL
	virtual BOOL OnNotify(WPARAM, LPARAM lParam, LRESULT* pResult);

	virtual void Draw3DTab (CDC* pDC, CTabInfo* pTab, BOOL bActive);
	virtual void DrawFlatTab (CDC* pDC, CTabInfo* pTab, BOOL bActive);

// Implementation
public:
	virtual ~CTabWndEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTabWndEx)
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTabCloseOther();
	afx_msg void OnTabCloseAll();
	afx_msg void OnTabCloseThis();
};

//////////////////////////////////////////////////////////////////////////////
// CTabWndEx notification messages:

extern UINT TABWND_CHANGE_ACTIVE_TAB;
extern UINT TABWND_ON_HSCROLL;
extern UINT TABWND_CLOSE_TAB;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABWNDEX_H__10E15955_436E_11D2_8BEB_00A0C9B05590__INCLUDED_)
