// TabWndEx.cpp : implementation file
//

#include "stdafx.h"
#include "TabWndEx.h"

#include "resource.h"       // main symbols

#include "PrevAndNextWndMgr.h"
#include "cpf/ostypedef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT TABWND_CHANGE_ACTIVE_TAB = (WM_USER+1001);
UINT TABWND_ON_HSCROLL		= (WM_USER+1002);
UINT TABWND_CLOSE_TAB = (WM_USER+1003);

/////////////////////////////////////////////////////////////////////////////
// CTabInfo

class CTabInfo : public CObject
{
	friend class CTabWndEx;

	CTabInfo(const CString&	strText, const UINT	uiIcon, CWnd* pWnd) 
		: m_pWnd (pWnd), m_uiIcon (uiIcon)
	{
		m_strText = strText;
		m_rect.SetRectEmpty ();
		m_nFullWidth = 0;
	}

	CString		m_strText;
	const UINT	m_uiIcon;
	CRect		m_rect;
	CWnd*		m_pWnd;
	int			m_nFullWidth;
};

static const CString strOfficeFontName	= _T("Tahoma");
static const CString strDefaultFontName = _T("MS Sans Serif");
static const CString strVertFontName	= _T("Arial");

struct GLOBAL_DATA
{
	GLOBAL_DATA();
	~GLOBAL_DATA();

	void init();

	// solid brushes with convenient gray colors and system colors
	HBRUSH hbrBtnHilite, hbrBtnShadow;

	// color values of system colors used for CToolBar
	COLORREF clrBtnFace, clrBtnShadow, clrBtnHilite;
	COLORREF clrBtnText, clrWindowFrame;
	COLORREF clrBtnDkShadow, clrBtnLight;
	COLORREF clrGrayedText;
	COLORREF clrHilite;
	COLORREF clrTextHilite;

	CBrush	brBtnFace;
	CBrush	brHilite;
	CBrush	brLight;
	CBrush	brBlack;


	// Toolbar and menu fonts:
	CFont	fontRegular;
	CFont	fontBold;
	CFont	fontUnderline;
	CFont	fontVert;
	CFont	fontVertCaption;

	int		m_nTextHeightHorz;
	int		m_nTextHeightVert;


	// Implementation

	void UpdateSysColors();
	void UpdateTextMetrics ();


};

COLORREF MyGetSysColor(int index)
{
	return ::GetSysColor(index);

	//return skinppGetSkinSysColor(index);
}

static int CALLBACK FontFamalyProcFonts (const LOGFONT FAR* lplf,
										 const TEXTMETRIC FAR* /*lptm*/,
										 ULONG /*ulFontType*/,
										 LPARAM /*lParam*/)
{
	ASSERT (lplf != NULL);

	CString strFont = lplf->lfFaceName;
	return strFont.CollateNoCase (strOfficeFontName) == 0 ? 0 : 1;
}



GLOBAL_DATA::GLOBAL_DATA()
{

}

void GLOBAL_DATA::init()
{
	//---------------------------------------------------------
	// Cached system values (updated in CWnd::OnSysColorChange)
	//---------------------------------------------------------
	hbrBtnShadow = NULL;
	hbrBtnHilite = NULL;
	UpdateSysColors();

	//m_hcurStretch = NULL;
	//m_hcurStretchVert = NULL;
	//m_hcurHand = NULL;
	//m_hiconTool = NULL;

	//------------------
	// Initialize fonts:
	//------------------
	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	::SystemParametersInfo (SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);

	LOGFONT lf;
	memset (&lf, 0, sizeof (LOGFONT));

	lf.lfCharSet = DEFAULT_CHARSET; // Add by Yuhu.Wang on 99/10/25
	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;	// By Sven Ritter
	lf.lfItalic = info.lfMenuFont.lfItalic;

	//////////////////////////////////////////////
	// Modify by Yuhu.Wang on 99/11/02
	// Check if we should use system font
	//--------------------------------------------
	_tcscpy (lf.lfFaceName, info.lfMenuFont.lfFaceName);

	BOOL fUseSystemFont = (info.lfMenuFont.lfCharSet > SYMBOL_CHARSET);
	if (!fUseSystemFont)
	{
		//----------------------------------
		// Check for "Tahoma" font existance:
		//----------------------------------
		CWindowDC dc (NULL);
		if (::EnumFontFamilies (dc.GetSafeHdc (), NULL, FontFamalyProcFonts, 0) == 0)
		{
			//---------------------------
			// Found! Use MS Office font!
			//---------------------------
			_tcscpy (lf.lfFaceName, strOfficeFontName);
		}
		else
		{
			//-----------------------------
			// Not found. Use default font:
			//-----------------------------
			_tcscpy (lf.lfFaceName, strDefaultFontName);
		}
	}
	//--------------------------------------------
	//////////////////////////////////////////////

	fontRegular.CreateFontIndirect (&lf);

	lf.lfUnderline = TRUE;
	fontUnderline.CreateFontIndirect (&lf);
	lf.lfUnderline = FALSE;

	//------------------
	// Create bold font:
	//------------------
	lf.lfWeight = FW_BOLD;
	fontBold.CreateFontIndirect (&lf);

	//----------------------
	// Create vertical font:
	//----------------------
	CFont font;
	if (font.CreateStockObject (DEFAULT_GUI_FONT))
	{
		if (font.GetLogFont (&lf) != 0)
		{
			lf.lfOrientation = 900;
			lf.lfEscapement = 2700;	// By Andy

			lf.lfHeight = info.lfMenuFont.lfHeight;	// By Sven Ritter
			lf.lfWeight = info.lfMenuFont.lfWeight;
			lf.lfItalic = info.lfMenuFont.lfItalic;

			//			if(!fUseSystemFont)
			{
				_tcscpy (lf.lfFaceName, strVertFontName);
			}
			//--------------------------------------------
			//////////////////////////////////////////////

			fontVert.CreateFontIndirect (&lf);

			lf.lfEscapement = 900;
			fontVertCaption.CreateFontIndirect (&lf);
		}
	}

	UpdateTextMetrics ();

	//-----------------------
	// Detect the kind of OS:
	//-----------------------
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	//::GetVersionEx (&osvi);
	//bIsWindowsNT4 = ((osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
	//	(osvi.dwMajorVersion < 5));
}
//*******************************************************************************************
GLOBAL_DATA::~GLOBAL_DATA()
{
	// cleanup standard brushes
	if (hbrBtnShadow != NULL)
	{
		::DeleteObject (hbrBtnShadow);
	}

	if (hbrBtnHilite != NULL)
	{
		::DeleteObject (hbrBtnHilite);
	}

	//// cleanup standard cursors
	//if (m_hcurStretch != NULL)
	//{
	//	::DeleteObject (m_hcurStretch);
	//}

	//if (m_hcurStretchVert != NULL)
	//{
	//	::DeleteObject (m_hcurStretchVert);
	//}

	//if (m_hcurHand != NULL)
	//{
	//	::DeleteObject (m_hcurHand);
	//}

	//if (m_hiconTool != NULL)
	//{
	//	::DeleteObject (m_hiconTool);
	//}
}


void GLOBAL_DATA::UpdateTextMetrics ()
{
	CWindowDC dc (NULL);

	CFont* pOldFont = dc.SelectObject (&fontRegular);
	ASSERT (pOldFont != NULL);

	TEXTMETRIC tm;
	dc.GetTextMetrics (&tm);

	m_nTextHeightHorz = tm.tmHeight + 2;

	dc.SelectObject (&fontVert);
	dc.GetTextMetrics (&tm);

	m_nTextHeightVert = tm.tmHeight + 2;

	dc.SelectObject (pOldFont);
}

void GLOBAL_DATA::UpdateSysColors()
{
	clrBtnFace = MyGetSysColor(COLOR_BTNFACE);
	clrBtnShadow = MyGetSysColor(COLOR_BTNSHADOW);
	clrBtnDkShadow = MyGetSysColor(COLOR_3DDKSHADOW);
	clrBtnLight = MyGetSysColor(COLOR_3DLIGHT);
	clrBtnHilite = MyGetSysColor(COLOR_BTNHIGHLIGHT);
	clrBtnText = MyGetSysColor(COLOR_BTNTEXT);
	clrGrayedText = MyGetSysColor (COLOR_GRAYTEXT);
	clrWindowFrame = MyGetSysColor(COLOR_WINDOWFRAME);

	clrHilite = MyGetSysColor(COLOR_HIGHLIGHT);
	clrTextHilite = MyGetSysColor(COLOR_HIGHLIGHTTEXT);

	if (hbrBtnShadow != NULL)
	{
		::DeleteObject (hbrBtnShadow);
	}

	if (hbrBtnHilite != NULL)
	{
		::DeleteObject (hbrBtnHilite);
	}

	hbrBtnShadow = ::CreateSolidBrush(clrBtnShadow);
	ASSERT(hbrBtnShadow != NULL);
	hbrBtnHilite = ::CreateSolidBrush(clrBtnHilite);
	ASSERT(hbrBtnHilite != NULL);

	brBtnFace.DeleteObject ();
	brBtnFace.CreateSolidBrush (clrBtnFace);

	brHilite.DeleteObject ();
	brHilite.CreateSolidBrush (clrHilite);

	brBlack.DeleteObject ();
	brBlack.CreateSolidBrush (clrBtnDkShadow);

	CWindowDC dc (NULL);
//	m_nBitsPerPixel = dc.GetDeviceCaps (BITSPIXEL);
	brLight.DeleteObject ();

//	if (m_nBitsPerPixel > 8)
	{
		//-------------------
		// By Maarten Hoeben:
		//-------------------
	/*	COLORREF clrLight = 
			RGB (
			(GetRValue(clrBtnFace) + ((GetRValue(clrBtnHilite) -
			GetRValue(clrBtnFace)) / 2 )),

			(GetGValue(clrBtnFace) + ((GetGValue(clrBtnHilite) -
			GetGValue(clrBtnFace)) / 2)),

			(GetBValue(clrBtnFace) + ((GetBValue(clrBtnHilite) -
			GetBValue(clrBtnFace)) / 2)),
			);*/

		COLORREF clrLight = 
			RGB (
			(GetRValue(clrBtnFace) + ((GetRValue(clrBtnHilite) -
			GetRValue(clrBtnFace)) / 2 )),

			(GetGValue(clrBtnFace) + ((GetGValue(clrBtnHilite) -
			GetGValue(clrBtnFace)) / 2)),

			(GetBValue(clrBtnFace) + ((GetBValue(clrBtnHilite) -
			GetBValue(clrBtnFace)) / 2))
			);
		brLight.CreateSolidBrush (clrLight);
	}
	//else
	{
		/*HBITMAP hbmGray = CreateDitherBitmap (dc.GetSafeHdc ());
		ASSERT (hbmGray != NULL);

		CBitmap bmp;
		bmp.Attach (hbmGray);

		brLight.CreatePatternBrush (&bmp);*/
	}

}

/////////////////////////////////////////////////////////////////////////////
// CTabWndEx

GLOBAL_DATA globalData;

#define TAB_BORDER_SIZE	3
#define TEXT_MARGIN		4
#define IMAGE_MARGIN	4
#define MIN_SROLL_WIDTH (::GetSystemMetrics (SM_CXHSCROLL) * 2)
#define SPLITTER_WIDTH	5
#define TABS_FONT		_T("Arial")

void CTabWndEx::init_clolr()
{
	globalData.init();
}

CTabWndEx::CTabWndEx(BOOL bSupportLink)
{
	m_bEnableCloseMenu = false;
	m_iTabsNum = 0;
	m_iActiveTab = -1;
	m_sizeImage = CSize (0, 0);
	m_bFlat = FALSE;
	m_bSharedScroll = FALSE;
	m_rectTabsArea.SetRectEmpty ();
	m_rectWndArea.SetRectEmpty ();
	m_nTabsHorzOffset = 0;
	m_nTabsHorzOffsetMax = 0;
	m_nTabsTotalWidth = 0;
	m_nHorzScrollWidth = 0;
	m_nScrollBarRight = 0;
	m_rectTabSplitter.SetRectEmpty ();
	m_bTrackSplitter = FALSE;
	m_location = LOCATION_BOTTOM;

	m_clrActiveTabBk = (COLORREF) -1;
	m_clrActiveTabFg = (COLORREF) -1;
	
	m_nTabsHeight = 0;

	if( bSupportLink )
	{
		m_pPrevAndNextWndMgr = new CPrevAndNextWndMgr(true);
	}
	else
	{
		m_pPrevAndNextWndMgr = NULL;
	}

}
//***************************************************************************************
CTabWndEx::~CTabWndEx()
{
	if( m_pPrevAndNextWndMgr )
	{
		delete m_pPrevAndNextWndMgr;
		m_pPrevAndNextWndMgr = NULL;
	}

	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CTabInfo* pTab = (CTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		delete pTab;
	}

	m_arTabs.RemoveAll ();

	return;
}

BEGIN_MESSAGE_MAP(CTabWndEx, CWnd)
	//{{AFX_MSG_MAP(CTabWndEx)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_HSCROLL()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SETTINGCHANGE()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTabWndEx::Create (Style style, const RECT& rect, CWnd* pParentWnd, 
						 UINT nID, Location location /* = LOCATION_BOTTOM*/)
{
	m_bFlat = (style == STYLE_FLAT) || (style == STYLE_FLAT_SHARED_HORZ_SCROLL);
	m_bSharedScroll = style == STYLE_FLAT_SHARED_HORZ_SCROLL;
	m_location = location;

	if (m_bFlat && m_location != LOCATION_BOTTOM)
	{
		ASSERT (FALSE);
		m_location = LOCATION_BOTTOM;
	}

	if (!m_bFlat && m_bSharedScroll)
	{
		//--------------------------------------
		// Only flat tab has a shared scrollbar!
		//--------------------------------------
		ASSERT (FALSE);
		m_bSharedScroll = FALSE;
	}

	return CWnd::Create (NULL, _T(""), WS_CHILD | WS_VISIBLE, rect,
		pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
// CTabWndEx message handlers

void CTabWndEx::OnDestroy() 
{
	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CTabInfo* pTab = (CTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		pTab->m_pWnd->DestroyWindow ();
		delete pTab;
	}

	m_arTabs.RemoveAll ();

	m_iTabsNum = 0;
	m_iActiveTab = -1;

	if( m_pPrevAndNextWndMgr )
	{
		m_pPrevAndNextWndMgr->close();
	}

	CWnd::OnDestroy();
}
//***************************************************************************************
void CTabWndEx::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rectClient;
	GetClientRect (&rectClient);

	CBrush* pOldBrush = dc.SelectObject(&globalData.brBtnFace);
	ASSERT (pOldBrush != NULL);

	CPen penDrak (PS_SOLID, 1, globalData.clrBtnDkShadow);
	CPen* pOldPen = (CPen*) dc.SelectObject (&penDrak);
	ASSERT(pOldPen != NULL);

	CRect rectTabs = rectClient;

	if (m_location == LOCATION_BOTTOM)
	{
		rectTabs.top = m_rectTabsArea.top - 2 * TAB_BORDER_SIZE;
	}
	else
	{
		rectTabs.bottom = m_rectTabsArea.bottom + TAB_BORDER_SIZE;
	}

	dc.FillRect (rectTabs, &globalData.brBtnFace);

	CRect rectFrame = rectClient;

	if (m_bFlat)
	{
		//---------------------------
		// Draw line bellow the tabs:
		//---------------------------
		dc.MoveTo (rectFrame.left, m_rectTabsArea.top);
		dc.LineTo (rectFrame.right, m_rectTabsArea.top);
	}
	else
	{
		if (m_location == LOCATION_BOTTOM)
		{
			rectFrame.bottom = m_rectTabsArea.top;
		}
		else
		{
			rectFrame.top = m_rectTabsArea.bottom;
		}
	}

	//-----------------------------------------------------
	// Draw wide 3-dimensional frame around the Tabs area:
	//-----------------------------------------------------
	dc.Draw3dRect (&rectFrame, globalData.clrBtnHilite, globalData.clrBtnDkShadow);
	
	rectFrame.DeflateRect (1, 1);
	dc.Draw3dRect (&rectFrame,
				globalData.clrBtnLight, globalData.clrBtnShadow);

	rectFrame.DeflateRect (1, 1);

	if (rectFrame.Width () > 0 && rectFrame.Height () > 0)
	{
		dc.PatBlt (rectFrame.left, rectFrame.top, TAB_BORDER_SIZE, rectFrame.Height (), PATCOPY);
		dc.PatBlt (rectFrame.left, rectFrame.top, rectFrame.Width (), TAB_BORDER_SIZE, PATCOPY);
		dc.PatBlt (rectFrame.right - TAB_BORDER_SIZE, rectFrame.top, TAB_BORDER_SIZE, rectFrame.Height (), PATCOPY);
		dc.PatBlt (rectFrame.left, rectFrame.bottom - TAB_BORDER_SIZE, rectFrame.Width (), TAB_BORDER_SIZE, PATCOPY);

		rectFrame.DeflateRect (TAB_BORDER_SIZE - 2, TAB_BORDER_SIZE - 2);

		if (rectFrame.Width () > 0 && rectFrame.Height () > 0)
		{
			dc.Draw3dRect (&rectFrame,
						globalData.clrBtnDkShadow, globalData.clrBtnHilite);
		}
	}

	CFont* pOldFont = dc.SelectObject (m_bFlat ?	&m_fntTabs : 
													&globalData.fontRegular);
	ASSERT(pOldFont != NULL);

	dc.SetBkMode (TRANSPARENT);
	dc.SetTextColor (globalData.clrBtnText);

	if (m_rectTabsArea.Width () > 2 * TAB_BORDER_SIZE + 1 &&
		m_rectTabsArea.Height () > 2 * TAB_BORDER_SIZE + 1)
	{
		//-----------
		// Draw tabs:
		//-----------
		if (m_bFlat)
		{
			CRgn rgn;
			rgn.CreateRectRgnIndirect (m_rectTabsArea);

			dc.SelectClipRgn (&rgn);
		}

		for (int i = 0; i < m_iTabsNum; i ++)
		{
			CTabInfo* pTab = (CTabInfo*) m_arTabs [i];
			ASSERT_VALID (pTab);

			if (m_bFlat)
			{
				if (i != m_iActiveTab)	// Draw active button last
				{
					DrawFlatTab (&dc, pTab, FALSE);
				}
			}
			else
			{
				Draw3DTab (&dc, pTab, i == m_iActiveTab);
			}
		}

		if (m_bFlat && m_iActiveTab >= 0)
		{
			//-----------------
			// Draw active tab:
			//-----------------
			CTabInfo* pTabActive = (CTabInfo*) m_arTabs [m_iActiveTab];
			ASSERT_VALID (pTabActive);

			dc.SelectObject (&m_brActiveTab);
			dc.SelectObject (&m_fntTabsBold);
			dc.SetTextColor (GetActiveTabTextColor ());

			DrawFlatTab (&dc, pTabActive, TRUE);

			//---------------------------------
			// Draw line bellow the active tab:
			//---------------------------------
			CPen penLight (PS_SOLID, 1, GetActiveTabColor ());
			dc.SelectObject (&penLight);

			dc.MoveTo (pTabActive->m_rect.left + 1, pTabActive->m_rect.top);
			dc.LineTo (pTabActive->m_rect.right, pTabActive->m_rect.top);
		}

		if (m_bFlat)
		{
			dc.SelectClipRgn (NULL);
		}
	}

	if (!m_rectTabSplitter.IsRectEmpty ())
	{
		dc.FillRect (m_rectTabSplitter, &globalData.brBtnFace);

		CRect rectTabSplitter = m_rectTabSplitter;

		dc.Draw3dRect (rectTabSplitter,
			globalData.clrBtnDkShadow, globalData.clrBtnShadow);
		rectTabSplitter.DeflateRect (1, 1);
		dc.Draw3dRect (rectTabSplitter,
			globalData.clrBtnHilite, globalData.clrBtnShadow);
	}
	
	dc.SelectObject (pOldFont);
	dc.SelectObject (pOldBrush);
	dc.SelectObject (pOldPen);
}
//***************************************************************************************
void CTabWndEx::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	int nTabsAreaWidth = cx - 4 * ::GetSystemMetrics (SM_CXVSCROLL) 
							- 2 * TAB_BORDER_SIZE;

	if (nTabsAreaWidth <= MIN_SROLL_WIDTH)
	{
		m_nHorzScrollWidth = 0;
	}
	else if (nTabsAreaWidth / 2 > MIN_SROLL_WIDTH)
	{
		m_nHorzScrollWidth = nTabsAreaWidth / 2;
	}
	else
	{
		m_nHorzScrollWidth = nTabsAreaWidth; 
	}

	RecalcLayout ();
	SynchronizeScrollBar ();
}
//***************************************************************************************
void CTabWndEx::AddTab (CWnd* pNewWnd, LPCTSTR lpszName, UINT uiImageId)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pNewWnd);
	ASSERT (pNewWnd->GetParent()->GetSafeHwnd () == GetSafeHwnd ());
	ASSERT (lpszName != NULL);

	if (m_bFlat)
	{
		ASSERT (uiImageId == -1);
		uiImageId = (UINT) -1;
	}
	else if (m_sizeImage != CSize (0, 0))
	{
		ASSERT (uiImageId != (UINT) -1);
	}

	CTabInfo* pTab = new CTabInfo (lpszName, uiImageId, pNewWnd);
	ASSERT (pTab != NULL);
	m_arTabs.SetAtGrow (m_iTabsNum ++, pTab);

	if (!m_bFlat)
	{
		CRect rectEmpty (0, 0, 0, 0);
		m_ToolTip.AddTool (this, lpszName, &rectEmpty, m_iTabsNum);
	}

	RecalcLayout ();

	if (m_iTabsNum == 1)
	{
		//----------------------------------------
		// First tab automatically becames active:
		//----------------------------------------
		SetActiveTab (0);
	}

	if (GetActiveWnd () != pNewWnd)
	{
		pNewWnd->ShowWindow (SW_HIDE);
	}

	if( m_pPrevAndNextWndMgr )
	{
		m_pPrevAndNextWndMgr->InsertOneNode(pNewWnd);
	}
}
//***************************************************************************************
void CTabWndEx::AddTab (CWnd* pTabWnd, UINT uiResTabLabel, UINT uiImageId)
{
	CString strLabel;
	strLabel.LoadString (uiResTabLabel);

	AddTab (pTabWnd, strLabel, uiImageId);
}
//***************************************************************************************
BOOL CTabWndEx::RemoveThisTab (int iTab)
{
	if (iTab < 0 || iTab >= m_iTabsNum)
	{
		TRACE(_T("RemoveTab: illegal tab number %d\n"), iTab);
		return FALSE;
	}

	//if (m_iTabsNum == 1)
	//{
	//	RemoveAllTabs ();
	//	return TRUE;
	//}

	CTabInfo* pTab = (CTabInfo*) m_arTabs [iTab];
	ASSERT_VALID (pTab);

	GetParent ()->SendMessage (TABWND_CLOSE_TAB, iTab,-1);

	//----------------------------
	// Detach tab from collection:
	//----------------------------
	m_arTabs.RemoveAt (iTab);
	m_iTabsNum --;


	int iNextTab = -1;

	if( m_pPrevAndNextWndMgr )
	{
		m_pPrevAndNextWndMgr->DeleteOneNode(pTab->m_pWnd);

		CWnd * pNextWnd = m_pPrevAndNextWndMgr->GetCur();

		if( pNextWnd )
		{
			iNextTab = GetTabIndex(pNextWnd);
		}

		m_iActiveTab = -1;
	}
	else
	{
		if (m_iActiveTab >= iTab)
		{
			iNextTab = mymax (0, mymin (m_iTabsNum, m_iActiveTab - 1));
			m_iActiveTab = -1;
		}
	}

	//-----------------------------------
	// Destroy tab window and delete tab:
	//-----------------------------------
	ASSERT_VALID (pTab->m_pWnd);
	pTab->m_pWnd->DestroyWindow ();
	delete pTab;

	RecalcLayout ();

	if( m_iTabsNum > 0 && iNextTab != -1 )
	{
		SetActiveTab (iNextTab);
		GetParent ()->SendMessage (TABWND_CHANGE_ACTIVE_TAB, m_iActiveTab,-1);				
	}
	else
	{
		m_iActiveTab = -1;
	}

	return TRUE;
}

BOOL CTabWndEx::RemoveOtherTabs (int iTab)
{
	CTabInfo* pTab = (CTabInfo*) m_arTabs [iTab];
	ASSERT_VALID (pTab);

	CWnd * pWnd = pTab->m_pWnd;

	while( m_iTabsNum != 1 )
	{
		for(int i = 0; i < m_arTabs.GetSize(); ++i)
		{
			if( ((CTabInfo*)m_arTabs[i])->m_pWnd != pWnd )
			{
				RemoveThisTab(i);
				break;
			}
		}
	}

	return true;

}

//***************************************************************************************
void CTabWndEx::RemoveAllTabs ()
{
	for(int i = m_arTabs.GetSize()-1; i >= 0; --i)
	{
		RemoveThisTab(i);
	}

	return;

}
//***************************************************************************************
BOOL CTabWndEx::SetActiveTab (int iTab)
{
	if (iTab < 0 || iTab >= m_iTabsNum)
	{
		TRACE(_T("SetActiveTab: illegal tab number %d\n"), iTab);
		return FALSE;
	}

	if (m_iActiveTab == iTab)	// Already active, do nothing
	{
		return TRUE;
	}

	if (m_iActiveTab != -1)
	{
		//--------------------
		// Hide active window:
		//--------------------
		GetActiveWnd()->ShowWindow (SW_HIDE);
	}

	m_iActiveTab = iTab;

	//------------------------
	// Show new active window:
	//------------------------
	HideActiveWindowHorzScrollBar ();

	CWnd* pWndActive = GetActiveWnd ();
	ASSERT_VALID (pWndActive);

	pWndActive->ShowWindow (SW_SHOW);

	//----------------------------------------------------------------------
	// Small trick: to adjust active window scroll sizes, I should change an
	// active window size twice (+1 pixel and -1 pixel):
	//----------------------------------------------------------------------
	pWndActive->SetWindowPos (NULL,
			-1, -1,
			m_rectWndArea.Width (), m_rectWndArea.Height (),
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	pWndActive->SetWindowPos (NULL,
			-1, -1,
			m_rectWndArea.Width () - 1, m_rectWndArea.Height (),
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

	EnsureVisible (m_iActiveTab);

	if (m_bFlat)
	{
		SynchronizeScrollBar ();
	}

	//-------------
	// Redraw tabs:
	//-------------
	Invalidate ();
	UpdateWindow ();

	///// By Uladzimir Liashkevich
	CView* pActiveView = DYNAMIC_DOWNCAST (CView, pWndActive);
	if (pActiveView != NULL)
	{
		CFrameWnd* pFrame = pActiveView->GetParentFrame ();
		ASSERT_VALID (pFrame);

		pFrame->SetActiveView (pActiveView);
	}
	else
	{
		pWndActive->SetFocus ();
	}
	////

	return TRUE;
}
//***************************************************************************************
void CTabWndEx::AdjustTabs ()
{
	m_nTabsTotalWidth = 0;

	if (m_iTabsNum == 0)
	{
		return;
	}

	//-------------------------
	// Define tab's full width:
	//-------------------------
	CClientDC dc (this);

	CFont* pOldFont = dc.SelectObject (m_bFlat ? 
		&m_fntTabsBold : &globalData.fontRegular);
	ASSERT(pOldFont != NULL);

	CRect rectClient;
	GetClientRect (&rectClient);

	//----------------------------------------------
	// First, try set all tabs in its original size:
	//----------------------------------------------
	int x = m_rectTabsArea.left - m_nTabsHorzOffset;
	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CTabInfo* pTab = (CTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		pTab->m_nFullWidth = m_sizeImage.cx + IMAGE_MARGIN +
				dc.GetTextExtent (pTab->m_strText).cx + 2 * TEXT_MARGIN;

		pTab->m_rect = CRect (CPoint (x, m_rectTabsArea.top),
						CSize (pTab->m_nFullWidth, m_rectTabsArea.Height () - 2));

		if (m_location == LOCATION_TOP)
		{
			pTab->m_rect.OffsetRect (0, 2);
		}

		if (!m_bFlat)
		{
			m_ToolTip.SetToolRect (this, i + 1, CRect (0, 0, 0, 0));
		}

		x += pTab->m_rect.Width () + 1;
		m_nTabsTotalWidth += pTab->m_rect.Width () + 1;

		if (m_bFlat)
		{
			//--------------------------------------------
			// In the flat mode tab is overlapped by next:
			//--------------------------------------------
			pTab->m_rect.right += m_nTabsHeight / 2;
		}

//		pTab->m_strDisplayedText = pTab->m_strText;
	}

	if (m_bFlat || x < m_rectTabsArea.right)
	{
		m_nTabsTotalWidth += m_nTabsHeight / 2;
		dc.SelectObject(pOldFont);
		return;
	}

	//-----------------------------------------
	// Not enouth space to show the whole text.
	//-----------------------------------------
	int nTabWidth = m_rectTabsArea.Width () / m_iTabsNum;

	//------------------------------------
	// May be it's too wide for some tabs?
	//------------------------------------
	int nRest = 0;
	int nCuttedTabsNum = m_iTabsNum;

	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CTabInfo* pTab = (CTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		// syc, 2002.5.23
		// increase 2, it may be too strict.
		if (pTab->m_nFullWidth + 2 < nTabWidth)
		{
			nRest += nTabWidth - pTab->m_nFullWidth;
			nCuttedTabsNum --;
		}
	}

	if (nCuttedTabsNum <= 0)
	{
		ASSERT (FALSE);	// Somethin wrong
		dc.SelectObject(pOldFont);
		return;
	}

	nTabWidth += nRest / nCuttedTabsNum;

	//----------------------------------
	// Last pass: set actual rectangles:
	//----------------------------------
	x = m_rectTabsArea.left;
	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CTabInfo* pTab = (CTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		BOOL bIsTrucncated = pTab->m_nFullWidth > nTabWidth;
		int nCurrTabWidth = (bIsTrucncated) ? nTabWidth : pTab->m_nFullWidth;

		pTab->m_rect = CRect (CPoint (x, m_rectTabsArea.top),
						CSize (nCurrTabWidth, m_rectTabsArea.Height ()));

		if (!m_bFlat)
		{
			pTab->m_rect.bottom -= 2;
			m_ToolTip.SetToolRect (this, i + 1, 
				bIsTrucncated ? pTab->m_rect : CRect (0, 0, 0, 0));
		}

		x += nCurrTabWidth + 1;
	}

	dc.SelectObject(pOldFont);
}
//***************************************************************************************
void CTabWndEx::Draw3DTab (CDC* pDC, CTabInfo* pTab, BOOL bActive)
{
	ASSERT_VALID (pTab);
	ASSERT_VALID (pDC);

	const int iVertOffset = 2;
	const int iHorzOffset = 2;

	CRect rectTab = pTab->m_rect;
	
	if (!bActive)
	{
		if (m_location == LOCATION_BOTTOM)
		{
			rectTab.bottom -= iVertOffset;
		}
		else
		{
			rectTab.top += iVertOffset;
		}
	}

	CPen penLight (PS_SOLID, 1, MyGetSysColor (COLOR_3DHILIGHT));
	CPen penDkGray (PS_SOLID, 1, MyGetSysColor (COLOR_3DSHADOW));

	CPen* pOldPen = NULL;

	if (m_location == LOCATION_BOTTOM)
	{
		pOldPen = (CPen*) pDC->SelectObject (&penLight);
		ASSERT(pOldPen != NULL);

		pDC->MoveTo (rectTab.left, rectTab.top);
		pDC->LineTo (rectTab.left, rectTab.bottom - iVertOffset);

		pDC->SelectStockObject (BLACK_PEN);

		pDC->LineTo (rectTab.left + iHorzOffset, rectTab.bottom);
		pDC->LineTo (rectTab.right - iHorzOffset, rectTab.bottom);
		pDC->LineTo (rectTab.right, rectTab.bottom - iVertOffset);
		pDC->LineTo (rectTab.right, rectTab.top - 1);

		pDC->SelectObject(&penDkGray);

		pDC->MoveTo (rectTab.left + iHorzOffset + 1, rectTab.bottom - 1);
		pDC->LineTo (rectTab.right - iHorzOffset, rectTab.bottom - 1);
		pDC->LineTo (rectTab.right - 1, rectTab.bottom - iVertOffset);
		pDC->LineTo (rectTab.right - 1, rectTab.top - 1);
	}
	else
	{
		// By Brian Palmer:
		pOldPen = (CPen*) pDC->SelectStockObject (BLACK_PEN);
		ASSERT(pOldPen != NULL);
		
		pDC->MoveTo (rectTab.right, bActive ? rectTab.bottom : rectTab.bottom - 1);
		pDC->LineTo (rectTab.right, rectTab.top + iVertOffset);
		pDC->LineTo (rectTab.right - iHorzOffset, rectTab.top);
		
		pDC->SelectObject (&penLight);
		
		pDC->LineTo (rectTab.left + iHorzOffset, rectTab.top);
		
		pDC->LineTo (rectTab.left, rectTab.top + iVertOffset);
		pDC->LineTo (rectTab.left, rectTab.bottom);
		
		pDC->SelectObject (&penDkGray);
		
		pDC->MoveTo (rectTab.right - 1, bActive ? rectTab.bottom : rectTab.bottom - 1);
		pDC->LineTo (rectTab.right - 1, rectTab.top + iVertOffset - 1);
 	}

	if (bActive)
	{
		const int iBarHeight = 3;
		const int y = (m_location == LOCATION_BOTTOM) ? 
			(rectTab.top - iBarHeight) : (rectTab.bottom);

		CBrush active_brush(RGB(195,0,0));

		pDC->FillRect (CRect (CPoint (rectTab.left, y), 
							CSize (rectTab.Width () - 1, iBarHeight)), &active_brush);
	}

	if (m_sizeImage.cx + IMAGE_MARGIN <= rectTab.Width ())
	{
		if (m_Images.GetSafeHandle () != NULL)
		{
			//----------------------
			// Draw the tab's image:
			//----------------------
			CRect rectImage = rectTab;

			rectImage.top += (rectTab.Height () - m_sizeImage.cy) / 2;
			rectImage.bottom = rectImage.top + m_sizeImage.cy;

			rectImage.left += IMAGE_MARGIN;
			rectImage.right = rectImage.left + m_sizeImage.cx;

			m_Images.Draw (pDC, pTab->m_uiIcon, rectImage.TopLeft (), ILD_TRANSPARENT);
		}

		//------------------------------
		// Finally, draw the tab's text:
		//------------------------------
		CRect rcText = rectTab;
		rcText.left += m_sizeImage.cx + 2 * TEXT_MARGIN;

		pDC->DrawText (pTab->m_strText, rcText,
							DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS);
	}

	pDC->SelectObject (pOldPen);
}
//***************************************************************************************
void CTabWndEx::DrawFlatTab (CDC* pDC, CTabInfo* pTab, BOOL /*bActive*/)
{
	ASSERT_VALID (pTab);
	ASSERT_VALID (pDC);

	CRect rectTab = pTab->m_rect;
	rectTab.bottom --;

	//----------------
	// Draw tab edges:
	//----------------
	enum{ POINTS_NUM=4};

	POINT pts [POINTS_NUM];

	pts [0].x = rectTab.left;
	pts [0].y = rectTab.top;

	pts [1].x = rectTab.left + m_nTabsHeight / 2;
	pts [1].y = rectTab.bottom;

	pts [2].x = rectTab.right - m_nTabsHeight / 2;
	pts [2].y = rectTab.bottom;

	pts [3].x = rectTab.right;
	pts [3].y = rectTab.top;

	pDC->Polygon (pts, POINTS_NUM);

	//---------------
	// Draw tab text:
	//---------------
	pDC->DrawText (pTab->m_strText, rectTab,
						DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS);
}
//***************************************************************************************
void CTabWndEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);

	if (m_rectTabSplitter.PtInRect (point))
	{
		m_bTrackSplitter = TRUE;
		SetCapture ();
		return;
	}

	CWnd* pWndParent = GetParent ();
	ASSERT_VALID (pWndParent);

	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CTabInfo* pTab = (CTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		if (pTab->m_rect.PtInRect (point))
		{
			if (i != m_iActiveTab && m_rectTabsArea.PtInRect (point))
			{
				int old_active_index = m_iActiveTab;
				SetActiveTab (i);
				pWndParent->SendMessage (TABWND_CHANGE_ACTIVE_TAB, m_iActiveTab,old_active_index);

				if( m_pPrevAndNextWndMgr )
				{
					m_pPrevAndNextWndMgr->ShowOneNode(GetTabWnd(m_iActiveTab));

				}
			}

			return;
		}
	}

	CWnd* pWndTarget = FindTargetWnd (point);
	if (pWndTarget == NULL)
	{
		return;
	}

	ASSERT_VALID (pWndTarget);

	MapWindowPoints (pWndTarget, &point, 1);
	pWndTarget->SendMessage (WM_LBUTTONDOWN, nFlags, 
							MAKELPARAM (point.x, point.y));
}
//***************************************************************************************
void CTabWndEx::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDblClk(nFlags, point);

	int iTab = FindTableWnd(point);

	if( iTab != -1 )
	{
		if( m_bEnableCloseMenu )
		{
			this->RemoveThisTab(iTab);
		}
	}
	else
	{
		CWnd* pWndTarget = FindTargetWnd (point);
		if (pWndTarget == NULL)
		{
			return;
		}

		ASSERT_VALID (pWndTarget);

		MapWindowPoints (pWndTarget, &point, 1);
		pWndTarget->SendMessage (WM_LBUTTONDBLCLK, nFlags, 
			MAKELPARAM (point.x, point.y));
	}


}
//****************************************************************************************
int CTabWndEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_bFlat)
	{
		//-------------------
		// Create scrollbars:
		//-------------------
		CRect rectDummy (0, 0, 0, 0);
		m_wndScrollTabs.Create (WS_CHILD | WS_VISIBLE | SBS_HORZ, rectDummy,
			this, (UINT) -1);

		if (m_bSharedScroll)
		{
			m_wndScrollWnd.Create (WS_CHILD | WS_VISIBLE | SBS_HORZ, rectDummy,
				this, (UINT) -1);
		}

		//---------------------
		// Create active brush:
		//---------------------
		m_brActiveTab.CreateSolidBrush (GetActiveTabColor ());
	}
	else
	{
		//---------------------------------------
		// Text may be truncated. Create tooltip.
		//---------------------------------------
		m_ToolTip.Create (this, TTS_ALWAYSTIP);
		m_ToolTip.Activate (TRUE);
		m_ToolTip.BringWindowToTop ();
	}

	SetTabsHeight ();
	return 0;
}
//***************************************************************************************
BOOL CTabWndEx::SetImageList (UINT uiID, int cx, COLORREF clrTransp)
{
	if (m_bFlat)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	if (!m_Images.Create (uiID, cx, 0, clrTransp))
	{
		return FALSE;
	}

	IMAGEINFO info;
	m_Images.GetImageInfo (0, &info);

	CRect rectImage = info.rcImage;
	m_sizeImage = rectImage.Size ();

	SetTabsHeight ();
	return TRUE;
}
//***************************************************************************************
BOOL CTabWndEx::OnEraseBkgnd(CDC* pDC)
{
	if (m_iTabsNum == 0)
	{
		CRect rectClient;
		GetClientRect (rectClient);
		pDC->FillRect (rectClient, &globalData.brBtnFace);
	}

	return TRUE;
}
//****************************************************************************************
BOOL CTabWndEx::PreTranslateMessage(MSG* pMsg) 
{
   	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (m_iActiveTab != -1 &&
			::GetAsyncKeyState (VK_CONTROL) & 0x8000)	// Ctrl is pressed
		{
			switch (pMsg->wParam)
			{
			case VK_NEXT:
				{
					int old_active_index = m_iActiveTab;

					SetActiveTab (m_iActiveTab == m_iTabsNum - 1 ? 0 : m_iActiveTab + 1);
					GetActiveWnd ()->SetFocus ();
					GetParent ()->SendMessage (TABWND_CHANGE_ACTIVE_TAB, m_iActiveTab,old_active_index);

					if( m_pPrevAndNextWndMgr )
					{
						m_pPrevAndNextWndMgr->ShowOneNode(GetTabWnd(m_iActiveTab));

					}
				}
				break;

			case VK_PRIOR:
				{
					int old_active_index = m_iActiveTab;

					SetActiveTab (m_iActiveTab == 0 ? m_iTabsNum - 1 : m_iActiveTab - 1);
					GetActiveWnd ()->SetFocus ();
					GetParent ()->SendMessage (TABWND_CHANGE_ACTIVE_TAB, m_iActiveTab,old_active_index);

					if( m_pPrevAndNextWndMgr )
					{
						m_pPrevAndNextWndMgr->ShowOneNode(GetTabWnd(m_iActiveTab));

					}
				}
				break;
			}
		}
		// Continue....

	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_NCLBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONUP:
	case WM_MOUSEMOVE:
		if (!m_bFlat)
		{
			m_ToolTip.RelayEvent(pMsg);
		}
		break;
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CTabWndEx::CloseOldestTab()
{
	CWnd * pOldest = m_pPrevAndNextWndMgr->GetOldest();

	if( pOldest )
	{
		int index = GetTabIndex(pOldest);

		RemoveThisTab(index);
	}

	return;
}

void CTabWndEx::ActivateNextTab()
{
	int next_tab = -1;
	int old_tab = -1;

	if( m_pPrevAndNextWndMgr )
	{
		CWnd * pCurWnd = m_pPrevAndNextWndMgr->GetCur();

		if( pCurWnd )
			old_tab = GetTabIndex(pCurWnd);

		CWnd * pNextWnd = NULL;

		if( m_pPrevAndNextWndMgr->MoveToNext(pNextWnd) )
		{
			next_tab = GetTabIndex(pNextWnd);
		}
	}
	else
	{
		next_tab == (m_iActiveTab==m_iTabsNum - 1) ? 0 : m_iActiveTab + 1;

		old_tab = m_iActiveTab;
	}

	if( next_tab != -1 )
	{
		SetActiveTab (next_tab);

		GetParent ()->SendMessage (TABWND_CHANGE_ACTIVE_TAB, next_tab,old_tab);

	}

	return;
}

void CTabWndEx::ActivatePrevTab()
{
	int prev_tab = -1;
	int old_tab = -1;

	if( m_pPrevAndNextWndMgr )
	{
		CWnd * pCurWnd = m_pPrevAndNextWndMgr->GetCur();

		if( pCurWnd )
			old_tab = GetTabIndex(pCurWnd);

		CWnd * pPrevWnd = NULL;

		if( m_pPrevAndNextWndMgr->MoveToPrev(pPrevWnd) )
		{
			prev_tab = GetTabIndex(pPrevWnd);
		}
	}
	else
	{
		prev_tab = ((m_iActiveTab == 0) ? m_iTabsNum - 1 : m_iActiveTab - 1);

		old_tab = m_iActiveTab;
	}

	if( prev_tab != -1 )
	{
		SetActiveTab (prev_tab);

		GetParent ()->SendMessage (TABWND_CHANGE_ACTIVE_TAB, prev_tab,old_tab);

	}

	return;
}


//****************************************************************************************
CWnd* CTabWndEx::GetTabWnd (int iTab) const
{
	if (iTab >= 0 && iTab < m_iTabsNum)
	{
		CTabInfo* pTab = (CTabInfo*) m_arTabs [iTab];
		ASSERT_VALID (pTab);

		return pTab->m_pWnd;
	}
	else
	{
		return NULL;
	}
}

int CTabWndEx::GetTabIndex(CWnd * pWnd) const
{
	for(int i = 0; i < m_arTabs.GetCount(); ++i)
	{
		CTabInfo* pTab = (CTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		if( pTab->m_pWnd == pWnd  )
			return i;
	}

	return -1;
}

BOOL CTabWndEx::GetTabLabel (int iTab, CString& strLabel) const
{
	if (iTab >= 0 && iTab < m_iTabsNum)
	{
		CTabInfo* pTab = (CTabInfo*) m_arTabs [iTab];
		ASSERT_VALID (pTab);

		strLabel = pTab->m_strText;

		return true;
	}
	else
	{
		return false;
	}
}

//******************************************************************************************
CWnd* CTabWndEx::GetActiveWnd () const
{
	return m_iActiveTab == -1 ? 
		NULL : 
		((CTabInfo*) m_arTabs [m_iActiveTab])->m_pWnd;
}
//******************************************************************************************
void CTabWndEx::TruncateText (CDC* pDC, CString& strText, int nMaxWidth)
//--------------
// By Alan Fritz
//--------------
{
	ASSERT_VALID (pDC);

	if (strText.IsEmpty ())
	{
		return;
	}

	CString strFirstChar = strText.Left (1);
	if (pDC->GetTextExtent (strFirstChar).cx > nMaxWidth)
	{
		// Even first char can't be displayed!
		strText.Empty ();
		return;
	}

	const CString strEllipses (_T("..."));
	CString strNewText (strText);

	while (strText.GetLength () > 1 &&
		pDC->GetTextExtent (strNewText).cx > nMaxWidth)
	{
		strText = strText.Left (strText.GetLength () - 1);
		strNewText = strText + strEllipses;
	}

	if (strText.GetLength () == 1)
	{
		// Start remove dots from ellipses...
		while (!strNewText.IsEmpty () &&
			pDC->GetTextExtent (strNewText).cx > nMaxWidth)
		{
			strNewText = strNewText.Left (strNewText.GetLength () - 1);
		}
	}

	strText = strNewText;
}
//******************************************************************************************
void CTabWndEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (!m_bFlat)
	{
		CWnd::OnHScroll (nSBCode, nPos, pScrollBar);
		return;
	}

	if (pScrollBar->GetSafeHwnd () == m_wndScrollWnd.GetSafeHwnd ())
	{
		static BOOL bInsideScroll = FALSE;

		if (m_iActiveTab != -1 && !bInsideScroll)
		{
			CWnd* pWndActive = GetActiveWnd ();
			ASSERT_VALID (pWndActive);

			WPARAM wParam = MAKEWPARAM (nSBCode, nPos);

			//----------------------------------
			// Pass scroll to the active window:
			//----------------------------------
			bInsideScroll = TRUE;
			pWndActive->SendMessage (WM_HSCROLL, wParam, 0);
			bInsideScroll = FALSE;

			m_wndScrollWnd.SetScrollPos (pWndActive->GetScrollPos (SB_HORZ));

			HideActiveWindowHorzScrollBar ();
			GetParent ()->SendMessage (TABWND_ON_HSCROLL, wParam);
		}

		return;
	}

	if (pScrollBar->GetSafeHwnd () != m_wndScrollTabs.GetSafeHwnd ())
	{
		CWnd::OnHScroll (nSBCode, nPos, pScrollBar);
		return;
	}

	int nPrevOffset = m_nTabsHorzOffset;
	const int nScrollOffset = 20;

	switch (nSBCode)
	{
	case SB_LINELEFT:
		m_nTabsHorzOffset -= nScrollOffset;
		break;

	case SB_LINERIGHT:
		m_nTabsHorzOffset += nScrollOffset;
		break;

	default:
		AdjustTabsScroll ();
		return;
	}

	m_nTabsHorzOffset = mymin (mymax (0, m_nTabsHorzOffset), m_nTabsHorzOffsetMax);
	if (nPrevOffset != m_nTabsHorzOffset)
	{
		AdjustTabs ();
		InvalidateRect (m_rectTabsArea);
		UpdateWindow ();

		if (m_nTabsHorzOffset == 0)
		{
			m_wndScrollTabs.EnableScrollBar ();
			m_wndScrollTabs.EnableScrollBar (ESB_DISABLE_LTUP);
		}
		else if (m_nTabsHorzOffset == m_nTabsHorzOffsetMax)
		{
			m_wndScrollTabs.EnableScrollBar ();
			m_wndScrollTabs.EnableScrollBar (ESB_DISABLE_RTDN);
		}
	}
}
//******************************************************************************************
CWnd* CTabWndEx::FindTargetWnd (const CPoint& point)
{
	if (point.y < m_nTabsHeight)
	{
		return NULL;
	}

	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CTabInfo* pTab = (CTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		if (pTab->m_rect.PtInRect (point))
		{
			return NULL;
		}
	}

	CWnd* pWndParent = GetParent ();
	ASSERT_VALID (pWndParent);

	return pWndParent;
}

int CTabWndEx::FindTableWnd (const CPoint& point)
{
	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CTabInfo* pTab = (CTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		if (pTab->m_rect.PtInRect (point))
		{
			return i;
		}
	}

	return -1;
}


//************************************************************************************
void CTabWndEx::AdjustTabsScroll ()
{
	ASSERT_VALID (this);

	if (!m_bFlat)
	{
		m_nTabsHorzOffset = 0;
		return;
	}

	if (m_iTabsNum == 0)
	{
		m_wndScrollTabs.EnableScrollBar (ESB_DISABLE_BOTH);
		m_nTabsHorzOffsetMax = 0;
		m_nTabsHorzOffset = 0;
		return;
	}

	int nPrevHorzOffset = m_nTabsHorzOffset;

	m_nTabsHorzOffsetMax = mymax (0, m_nTabsTotalWidth - m_rectTabsArea.Width ());
	m_nTabsHorzOffset = mymax (0, mymin (m_nTabsHorzOffset, m_nTabsHorzOffsetMax));

	m_wndScrollTabs.EnableScrollBar ();
	if (m_nTabsHorzOffset <= 0)
	{
		m_nTabsHorzOffset = 0;
		m_wndScrollTabs.EnableScrollBar (ESB_DISABLE_LTUP);
	}

	if (m_nTabsHorzOffset >= m_nTabsHorzOffsetMax)
	{
		m_nTabsHorzOffset = m_nTabsHorzOffsetMax;
		m_wndScrollTabs.EnableScrollBar (ESB_DISABLE_RTDN);
	}

	if (nPrevHorzOffset != m_nTabsHorzOffset)
	{
		AdjustTabs ();
		InvalidateRect (m_rectTabsArea);
		UpdateWindow ();
	}
}
//*************************************************************************************
void CTabWndEx::RecalcLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	ASSERT_VALID (this);

	CRect rectClient;
	GetClientRect (rectClient);

	m_rectTabsArea = rectClient;
	m_rectTabsArea.DeflateRect (TAB_BORDER_SIZE, 0);

	if (m_bFlat)
	{
		m_rectTabsArea.bottom -= TAB_BORDER_SIZE - 1;
		m_rectTabsArea.top = m_rectTabsArea.bottom - m_nTabsHeight;

		int nScrollBarWidth = ::GetSystemMetrics (SM_CXHSCROLL) * 2;
		int nScrollBarHeight = m_rectTabsArea.Height () - 3;

		m_rectTabsArea.left += nScrollBarWidth + 1;
		m_rectTabsArea.right -= TAB_BORDER_SIZE;

		if (m_rectTabsArea.right < m_rectTabsArea.left)
		{
			nScrollBarWidth = 0;
			m_rectTabsArea.left = rectClient.left + TAB_BORDER_SIZE + 1;
			m_rectTabsArea.right = rectClient.right - TAB_BORDER_SIZE - 1;
		}

		if (m_rectTabsArea.Height () + TAB_BORDER_SIZE > rectClient.Height ())
		{
			nScrollBarHeight = 0;
			m_rectTabsArea.left = 0;
			m_rectTabsArea.right = 0;
		}

		ASSERT (m_wndScrollTabs.GetSafeHwnd () != NULL);

		m_wndScrollTabs.SetWindowPos (NULL,
			rectClient.left + TAB_BORDER_SIZE + 1, m_rectTabsArea.top + 1,
			nScrollBarWidth, nScrollBarHeight,
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
	}
	else
	{
		if (m_location == LOCATION_BOTTOM)
		{
			m_rectTabsArea.top = m_rectTabsArea.bottom - m_nTabsHeight;
		}
		else
		{
			m_rectTabsArea.bottom = m_rectTabsArea.top + m_nTabsHeight;
		}

		m_rectTabsArea.right -= 2 * TAB_BORDER_SIZE;
	}

	m_nScrollBarRight = m_rectTabsArea.right - ::GetSystemMetrics (SM_CXVSCROLL);

	m_rectWndArea = rectClient;
	m_rectWndArea.DeflateRect (TAB_BORDER_SIZE + 1, TAB_BORDER_SIZE + 1);
	
	if (m_bFlat)
	{
		m_rectWndArea.bottom = m_rectTabsArea.top;
	}
	else
	{
		if (m_location == LOCATION_BOTTOM)
		{
			m_rectWndArea.bottom = m_rectTabsArea.top - 2 * TAB_BORDER_SIZE + 1;
		}
		else
		{
			m_rectWndArea.top = m_rectTabsArea.bottom + 2 * TAB_BORDER_SIZE - 1;
		}
	}

	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CTabInfo* pTab = (CTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		pTab->m_pWnd->SetWindowPos (NULL,
			m_rectWndArea.left, m_rectWndArea.top,
			m_rectWndArea.Width () - 1,
			m_rectWndArea.Height (),
			SWP_NOACTIVATE | SWP_NOZORDER);
	}

	AdjustWndScroll ();
	AdjustTabs ();
	AdjustTabsScroll ();

	Invalidate ();
	UpdateWindow ();
}
//*************************************************************************************
void CTabWndEx::AdjustWndScroll ()
{
	if (!m_bSharedScroll)
	{
		return;
	}

	ASSERT_VALID (this);

	CRect rectScroll = m_rectTabsArea;

	if (m_nHorzScrollWidth >= MIN_SROLL_WIDTH)
	{
		rectScroll.right = m_nScrollBarRight;
		rectScroll.left = rectScroll.right - m_nHorzScrollWidth;
		rectScroll.bottom -= 2;

		m_rectTabSplitter = rectScroll;
		m_rectTabSplitter.top ++;
		m_rectTabSplitter.right = rectScroll.left;
		m_rectTabSplitter.left = m_rectTabSplitter.right - SPLITTER_WIDTH;

		m_rectTabsArea.right = m_rectTabSplitter.left;
		
		ASSERT (!m_rectTabSplitter.IsRectEmpty ());
	}
	else
	{
		rectScroll.SetRectEmpty ();
		m_rectTabSplitter.SetRectEmpty ();
	}

	m_wndScrollWnd.SetWindowPos (NULL,
		rectScroll.left, rectScroll.top,
		rectScroll.Width (), rectScroll.Height (),
		SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOCOPYBITS);
}
//***************************************************************************************
BOOL CTabWndEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_bFlat && !m_rectTabSplitter.IsRectEmpty ())
	{
		CPoint ptCursor;
		::GetCursorPos (&ptCursor);
		ScreenToClient (&ptCursor);

		if (m_rectTabSplitter.PtInRect (ptCursor))
		{
			::SetCursor (NULL/*globalData.m_hcurStretch*/);
			return TRUE;
		}
	}
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
//***************************************************************************************
void CTabWndEx::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bTrackSplitter)
	{
		m_bTrackSplitter = FALSE;
		ReleaseCapture ();
	}
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CTabWndEx::OnRButtonUp(UINT nFlags, CPoint point)
{


}

//***************************************************************************************
void CTabWndEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bTrackSplitter)
	{
		int nSplitterLeftPrev = m_rectTabSplitter.left;

		m_nHorzScrollWidth = mymin (
			m_nScrollBarRight - m_rectTabsArea.left - SPLITTER_WIDTH, 
			m_nScrollBarRight - point.x);

		m_nHorzScrollWidth = mymax (MIN_SROLL_WIDTH, m_nHorzScrollWidth);
		AdjustWndScroll ();

		if (m_rectTabSplitter.left > nSplitterLeftPrev)
		{
			CRect rect = m_rectTabSplitter;
			rect.left = nSplitterLeftPrev - 20;
			rect.right = m_rectTabSplitter.left;
			rect.InflateRect (0, TAB_BORDER_SIZE);

			InvalidateRect (rect);
		}

		CRect rectTabSplitter = m_rectTabSplitter;
		rectTabSplitter.InflateRect (0, TAB_BORDER_SIZE);

		InvalidateRect (rectTabSplitter);
		UpdateWindow ();
		AdjustTabsScroll ();
	}
	
	CWnd::OnMouseMove(nFlags, point);
}
//***************************************************************************************
void CTabWndEx::OnCancelMode() 
{
	CWnd::OnCancelMode();
	
	if (m_bTrackSplitter)
	{
		m_bTrackSplitter = FALSE;
		ReleaseCapture ();
	}
}
//***********************************************************************************
void CTabWndEx::SetActiveTabTextColor (COLORREF clr)
{
	ASSERT (m_bFlat);
	m_clrActiveTabFg = clr;
}
//***********************************************************************************
void CTabWndEx::SetActiveTabColor (COLORREF clr)
{
	ASSERT (m_bFlat);

	m_clrActiveTabBk = clr;

	if (m_brActiveTab.GetSafeHandle () != NULL)
	{
		m_brActiveTab.DeleteObject ();
	}

	m_brActiveTab.CreateSolidBrush (GetActiveTabColor ());
}
//**********************************************************************************
void CTabWndEx::OnSysColorChange() 
{
	CWnd::OnSysColorChange();

	if (m_bFlat && m_clrActiveTabFg == (COLORREF) -1)
	{
		if (m_brActiveTab.GetSafeHandle () != NULL)
		{
			m_brActiveTab.DeleteObject ();
		}

		m_brActiveTab.CreateSolidBrush (GetActiveTabColor ());

		Invalidate ();
		UpdateWindow ();
	}
}
//***********************************************************************************
BOOL CTabWndEx::SynchronizeScrollBar (SCROLLINFO* pScrollInfo/* = NULL*/)
{
	if (!m_bSharedScroll)
	{
		return FALSE;
	}

	ASSERT_VALID (this);

	SCROLLINFO scrollInfo;
	memset (&scrollInfo, 0, sizeof (SCROLLINFO));

	scrollInfo.cbSize = sizeof (SCROLLINFO);
	scrollInfo.fMask = SIF_ALL;

	CWnd* pWndActive = GetActiveWnd ();

	if (pScrollInfo != NULL)
	{
		scrollInfo = *pScrollInfo;
	}
	else if (pWndActive != NULL)
	{
		if (!pWndActive->GetScrollInfo (SB_HORZ, &scrollInfo) ||
			scrollInfo.nMin + (int) scrollInfo.nPage >= scrollInfo.nMax)
		{
			m_wndScrollWnd.EnableScrollBar (ESB_DISABLE_BOTH);
			return TRUE;
		}
	}

	m_wndScrollWnd.EnableScrollBar (ESB_ENABLE_BOTH);
	m_wndScrollWnd.SetScrollInfo (&scrollInfo);

	HideActiveWindowHorzScrollBar ();
	return TRUE;
}
//*************************************************************************************
void CTabWndEx::HideActiveWindowHorzScrollBar ()
{
	CWnd* pWnd = GetActiveWnd ();
	if (pWnd == NULL)
	{
		return;
	}

	ASSERT_VALID (pWnd);

	pWnd->ShowScrollBar (SB_HORZ, FALSE);
	pWnd->ModifyStyle (WS_HSCROLL, 0, SWP_DRAWFRAME);
}
//************************************************************************************
void CTabWndEx::SetTabsHeight ()
{
	if (m_bFlat)
	{
		m_nTabsHeight = ::GetSystemMetrics (SM_CYHSCROLL) + TEXT_MARGIN / 2;

		CFont* pDefaultFont = 
			CFont::FromHandle ((HFONT) ::GetStockObject (DEFAULT_GUI_FONT));
		ASSERT_VALID (pDefaultFont);

		if (pDefaultFont != NULL)	// Just to be relaxed....
		{
			LOGFONT lfDefault;
			pDefaultFont->GetLogFont (&lfDefault);

			LOGFONT lf;
			memset (&lf, 0, sizeof (LOGFONT));

			lf.lfCharSet = lfDefault.lfCharSet;
			lf.lfHeight = lfDefault.lfHeight;
			_tcscpy (lf.lfFaceName, TABS_FONT);

			CClientDC dc (this);

			TEXTMETRIC tm;

			do
			{
				m_fntTabs.DeleteObject ();
				m_fntTabs.CreateFontIndirect (&lf);

				CFont* pFont = dc.SelectObject (&m_fntTabs);
				ASSERT (pFont != NULL);

				dc.GetTextMetrics (&tm);
				dc.SelectObject (pFont);

				if (tm.tmHeight + TEXT_MARGIN / 2 <= m_nTabsHeight)
				{
					break;
				}

				//------------------
				// Try smaller font:
				//------------------
				if (lf.lfHeight < 0)
				{
					lf.lfHeight ++;
				}
				else
				{
					lf.lfHeight --;
				}
			}
			while (lf.lfHeight != 0);

			//------------------
			// Create bold font:
			//------------------
			lf.lfWeight = FW_BOLD;
			m_fntTabsBold.DeleteObject ();
			m_fntTabsBold.CreateFontIndirect (&lf);
		}
	}
	else
	{
		m_nTabsHeight = (mymax (m_sizeImage.cy + 2 * IMAGE_MARGIN,
			globalData.m_nTextHeightHorz + TEXT_MARGIN / 2));
	}
}
//*************************************************************************************
void CTabWndEx::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CWnd::OnSettingChange(uFlags, lpszSection);
	
	//-----------------------------------------------------------------
	// In the flat modetabs height should be same as scroll bar height
	//-----------------------------------------------------------------
	if (m_bFlat)
	{
		SetTabsHeight ();
		RecalcLayout ();
		SynchronizeScrollBar ();
	}
}
//*************************************************************************************
BOOL CTabWndEx::EnsureVisible (int iTab)
{
	if (iTab < 0 || iTab >= m_iTabsNum)
	{
		TRACE(_T("EnsureVisible: illegal tab number %d\n"), iTab);
		return FALSE;
	}

	//---------------------------------------------------------
	// Be sure, that active tab is visible (not out of scroll):
	//---------------------------------------------------------
	if (m_bFlat && m_rectTabsArea.Width () > 0)
	{
		CRect rectActiveTab = ((CTabInfo*) m_arTabs [m_iActiveTab])->m_rect;
		BOOL bAdjustTabs = FALSE;

		if (rectActiveTab.left < m_rectTabsArea.left)
		{
			m_nTabsHorzOffset -= (m_rectTabsArea.left - rectActiveTab.left);
			bAdjustTabs = TRUE;
		}
		else if (rectActiveTab.right > m_rectTabsArea.right)
		{
			m_nTabsHorzOffset += (rectActiveTab.right - m_rectTabsArea.right);
			bAdjustTabs = TRUE;
		}

		if (bAdjustTabs)
		{
			AdjustTabs ();
			AdjustTabsScroll ();
		}
	}

	return TRUE;
}
//**********************************************************************************
BOOL CTabWndEx::OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CWnd::OnNotify (wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT (pNMHDR != NULL);

	if (pNMHDR->code == TTN_SHOW && !m_bFlat)
	{
		m_ToolTip.SetWindowPos (&wndTop, -1, -1, -1, -1,
			SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}
//*********************************************************************************
BOOL CTabWndEx::GetTabRect (int iTab, CRect& rect) const
{
	if (iTab < 0 || iTab >= m_iTabsNum)
	{
		return FALSE;
	}

	CTabInfo* pTab = (CTabInfo*) m_arTabs [iTab];
	ASSERT_VALID (pTab);

	rect = pTab->m_rect;
	return TRUE;
}
//********************************************************************************
UINT CTabWndEx::GetTabIcon (int iTab) const
{
	if (iTab < 0 || iTab >= m_iTabsNum)
	{
		return (UINT) -1;
	}

	CTabInfo* pTab = (CTabInfo*) m_arTabs [iTab];
	ASSERT_VALID (pTab);

	return pTab->m_uiIcon;
}

BOOL CTabWndEx::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	for (int i = 0; i < m_iTabsNum; i++)
	{
		CWnd* pWnd = GetTabWnd(i);
		ASSERT(::IsWindow(pWnd->GetSafeHwnd()));
		if (pWnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	}
	
	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CTabWndEx::OnTabCloseOther()
{
	RemoveOtherTabs(GetActiveTab());
}

void CTabWndEx::OnTabCloseThis()
{
	RemoveThisTab(GetActiveTab());

}

void CTabWndEx::OnTabCloseAll()
{
	RemoveAllTabs();
}

