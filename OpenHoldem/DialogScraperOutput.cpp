// DialogScraperOutput.cpp : implementation file
//

#include "stdafx.h"

#include "OpenHoldem.h"
#include "MainFrm.h"

#include "CScraper.h"
#include "CHeartbeatThread.h"
#include "CPreferences.h"
#include "..\CTransform\CTransform.h"

#include "DialogScraperOutput.h"

// CDlgScraperOutput dialog
CDlgScraperOutput	*m_ScraperOutputDlg = NULL;

#define ID_SCRAPEROUTPUT_SIZERBAR 5555

// Window map tells CWinMgr how to position dialog controls
BEGIN_WINDOW_MAP(ScraperOutputFormulaMap)
BEGINCOLS(WRCT_REST,0,RCMARGINS(4,4))

RCTOFIT(IDC_REGIONLIST)					// list box

RCFIXED(ID_SCRAPEROUTPUT_SIZERBAR,4)	// vertical sizer bar

BEGINROWS(WRCT_REST,0,0)
BEGINCOLS(WRCT_REST,0,0)
RCREST(IDC_SCRAPERBITMAP)			// bitmap control
ENDGROUP()

RCSPACE(8)

BEGINCOLS(WRCT_TOFIT,0,0)
BEGINROWS(WRCT_TOFIT,0,0)
RCSPACE(4)
RCTOFIT(IDC_STATIC_ZOOM)			// "Zoom:"
ENDGROUP()
RCSPACE(4)
RCTOFIT(IDC_ZOOM)					// zoom selection
RCSPACE(8)
BEGINROWS(WRCT_TOFIT,0,0)
RCSPACE(4)
RCTOFIT(IDC_STATIC_RESULT)		// "Result:"
ENDGROUP()
RCSPACE(4)
RCREST(IDC_SCRAPERRESULT)			// scraper result
ENDGROUP()
ENDGROUP()
END_WINDOW_MAP()

CDlgScraperOutput::CDlgScraperOutput(CWnd* pParent /*=NULL*/)
		: CDialog(CDlgScraperOutput::IDD, pParent), m_winMgr(ScraperOutputFormulaMap)
{
	__SEH_SET_EXCEPTION_HANDLER

	in_startup = true;
}

CDlgScraperOutput::~CDlgScraperOutput()
{
}

void CDlgScraperOutput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REGIONLIST, m_RegionList);
	DDX_Control(pDX, IDC_SCRAPERBITMAP, m_ScraperBitmap);
	DDX_Control(pDX, IDC_ZOOM, m_Zoom);
	DDX_Control(pDX, IDC_SCRAPERRESULT, m_ScraperResult);
}

BEGIN_MESSAGE_MAP(CDlgScraperOutput, CDialog)

	// WinMgr
	ON_REGISTERED_MESSAGE(WM_WINMGR, OnWinMgr)

	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_REGIONLIST, &CDlgScraperOutput::OnLbnSelchangeRegionlist)
	ON_CBN_SELCHANGE(IDC_ZOOM, &CDlgScraperOutput::OnCbnSelchangeZoom)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CDlgScraperOutput message handlers
BOOL CDlgScraperOutput::OnInitDialog()
{
	int			max_x = 0, max_y = 0;
	RECT		rect = {0};

	in_startup = true;

	CDialog::OnInitDialog();

	// Save tofit windows as current size
	m_winMgr.InitToFitSizeFromCurrent(this);		// make tofit = current size
	m_winMgr.CalcLayout(this);						  // recalc
	m_winMgr.SetWindowPositions(this);				// set positions

	// Create sizer bar window
	VERIFY(m_winMgrSizerBar.Create(WS_VISIBLE|WS_CHILD, this, m_winMgr, ID_SCRAPEROUTPUT_SIZERBAR));

	// Populate list box with all interesting regions
	AddListboxItems();

	m_Zoom.AddString("None");
	m_Zoom.AddString("2x");
	m_Zoom.AddString("4x");
	m_Zoom.AddString("8x");
	m_Zoom.AddString("16x");
	m_Zoom.SetCurSel(0);

	// Set dialog icon
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON1));
	this->SetIcon(hIcon, FALSE);

	// Restore window location and size, precision preference

	max_x = GetSystemMetrics(SM_CXSCREEN) - GetSystemMetrics(SM_CXICON);
	max_y = GetSystemMetrics(SM_CYSCREEN) - GetSystemMetrics(SM_CYICON);
	::SetWindowPos(m_hWnd, HWND_TOP, min(prefs.scraper_x(), max_x), min(prefs.scraper_y(), max_y),
				   prefs.scraper_dx(), prefs.scraper_dy(), SWP_NOCOPYBITS);
	m_Zoom.SetCurSel(prefs.scraper_zoom());
	m_Zoom.GetWindowRect(&rect);
	m_Zoom.SetWindowPos(NULL, 0, 0, rect.right-rect.left, 9999, SWP_NOMOVE);

	in_startup = false;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgScraperOutput::DestroyWindow()
{
	WINDOWPLACEMENT		wp;
	CMainFrame			*pMyMainWnd  = (CMainFrame *) (theApp.m_pMainWnd);

	// Save settings to registry
	GetWindowPlacement(&wp);
	prefs.set_scraper_x(wp.rcNormalPosition.left);
	prefs.set_scraper_y(wp.rcNormalPosition.top);
	prefs.set_scraper_dx(wp.rcNormalPosition.right - wp.rcNormalPosition.left);
	prefs.set_scraper_dy(wp.rcNormalPosition.bottom - wp.rcNormalPosition.top);
	prefs.set_scraper_zoom(m_Zoom.GetCurSel());

	// Uncheck scraper output button on main toolbar
	pMyMainWnd->m_MainToolBar.GetToolBarCtrl().CheckButton(ID_MAIN_TOOLBAR_SCRAPER_OUTPUT, false);

	return CDialog::DestroyWindow();
}

void CDlgScraperOutput::PostNcDestroy()
{
   delete m_ScraperOutputDlg;
	m_ScraperOutputDlg	=	NULL;

	CDialog::PostNcDestroy();
}

LRESULT CDlgScraperOutput::OnWinMgr(WPARAM wp, LPARAM lp) 
{
	NMWINMGR& nmw = *(NMWINMGR*)lp;
	RECT		rect = {0};

	if (nmw.code==NMWINMGR::GET_SIZEINFO) 
	{
		if (wp==(WORD)GetDlgCtrlID()) 
		{
			// Parent frame is requesting my size info. Report min size.
			m_winMgr.GetMinMaxInfo(this, nmw.sizeinfo);
			return true; // handled--important!
		}
	}

	else if (nmw.code==NMWINMGR::SIZEBAR_MOVED) 
	{
		// User moved a sizer bar: call WinMgr to do it!
		m_winMgr.MoveRect(wp, nmw.sizebar.ptMoved, this);
		m_winMgr.SetWindowPositions(this);
		if (!in_startup)
		{
			m_Zoom.GetWindowRect(&rect);
			m_Zoom.SetWindowPos(NULL, 0, 0, rect.right-rect.left, 9999, SWP_NOMOVE);
			UpdateDisplay();
		}
		return true;
	}

	return false; // not handled
}

void CDlgScraperOutput::OnSize(UINT nType, int cx, int cy)
{
	RECT	rect = {0};

	CDialog::OnSize(nType, cx, cy);

	m_winMgr.CalcLayout(0, 0, cx, cy, this);
	m_winMgr.SetWindowPositions(this);
	if (!in_startup)
	{
		m_Zoom.GetWindowRect(&rect);
		m_Zoom.SetWindowPos(NULL, 0, 0, rect.right-rect.left, 9999, SWP_NOMOVE);
		UpdateDisplay();
	}
}

void CDlgScraperOutput::AddListboxItems()
{
	int			i = 0;
	CString		s = "";

	m_RegionList.ResetContent();
	m_RegionList.SetCurSel(-1);

	for (i=0; i<p_tablemap->r$()->GetSize(); i++)
	{
		m_RegionList.AddString(p_tablemap->r$()->GetAt(i).name);
	}
}

void CDlgScraperOutput::OnLbnSelchangeRegionlist()
{
	UpdateDisplay();
}

void CDlgScraperOutput::OnCbnSelchangeZoom()
{
	UpdateDisplay();
}

void CDlgScraperOutput::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	UpdateDisplay();
}

void CDlgScraperOutput::UpdateDisplay()
{
	CString curtext = "";
	int		i = 0;
	bool	found_match = false;

	if (in_startup)  
		return;

	// Only do this if we are not in the middle of a scraper/symbol update
	if (TryEnterCriticalSection(&p_heartbeat_thread->cs_update_in_progress))
	{
		if (m_RegionList.GetCurSel() == -1)
		{
			DoBitblt(NULL, -1);  // Clear display
			LeaveCriticalSection(&p_heartbeat_thread->cs_update_in_progress);
			return;
		}

		m_RegionList.GetText(m_RegionList.GetCurSel(), curtext);

		found_match = false;
		for (i=0; i<p_tablemap->r$()->GetSize() && !found_match; i++)
		{
			if (p_tablemap->r$()->GetAt(i).name == curtext)
			{
				DoBitblt(p_tablemap->r$()->GetAt(i).last_bmp, i);
				found_match = true;
			}
		}

		if (!found_match)
		{
			DoBitblt(NULL, -1);  // Clear display
		}

		LeaveCriticalSection(&p_heartbeat_thread->cs_update_in_progress);
	}
}

void CDlgScraperOutput::DoBitblt(HBITMAP bitmap, int r$index)
{
	CDC			*pDC = m_ScraperBitmap.GetDC();
	HDC			hdcControl = *pDC;
	HDC			hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC			hdcCompat1 = CreateCompatibleDC(hdcScreen);
	HDC			hdcCompat2 = CreateCompatibleDC(hdcScreen);
	HBITMAP		hbm2 = NULL, old_bitmap1 = NULL, old_bitmap2 = NULL;
	int			w = 0, h = 0, zoom = 0;
	RECT		rect = {0};
	CBrush		gray_brush, *pTempBrush = NULL, oldbrush;
	CPen		null_pen, *pTempPen = NULL, oldpen;
	CString		res = "";
	CTransform	trans;

	if (in_startup) 	
	{
		DeleteDC(hdcCompat1);
		DeleteDC(hdcCompat2);
		DeleteDC(hdcScreen);
		ReleaseDC(pDC);
		return;
	}

	m_ScraperBitmap.GetWindowRect(&rect);

	// Erase control area
	gray_brush.CreateSolidBrush(COLOR_GRAY);
	pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
	oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush
	null_pen.CreatePen(PS_NULL, 0, COLOR_BLACK);
	pTempPen = (CPen*)pDC->SelectObject(&null_pen);
	oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
	pDC->Rectangle(1, 1, rect.right-rect.left, rect.bottom-rect.top);
	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldpen);

	// return if all we needed to do was erase display
	if (r$index == -1)
	{
		DeleteDC(hdcCompat1);
		DeleteDC(hdcCompat2);
		DeleteDC(hdcScreen);
		ReleaseDC(pDC);
		return;
	}

	// load bitmap into 1st DC and stretchblt to 2nd DC
	old_bitmap1 = (HBITMAP) SelectObject(hdcCompat1, bitmap);
	zoom = m_Zoom.GetCurSel()==0 ? 1 :
		   m_Zoom.GetCurSel()==1 ? 2 :
		   m_Zoom.GetCurSel()==2 ? 4 :
		   m_Zoom.GetCurSel()==3 ? 8 :
		   m_Zoom.GetCurSel()==4 ? 16 : 1;

	w = (p_tablemap->r$()->GetAt(r$index).right - p_tablemap->r$()->GetAt(r$index).left) * zoom;
	h = (p_tablemap->r$()->GetAt(r$index).bottom - p_tablemap->r$()->GetAt(r$index).top) * zoom;

	hbm2 = CreateCompatibleBitmap(hdcScreen, w, h);
	old_bitmap2 = (HBITMAP) SelectObject(hdcCompat2, hbm2);
	StretchBlt(	hdcCompat2, 0, 0, w, h,
				hdcCompat1, 0, 0,
				p_tablemap->r$()->GetAt(r$index).right - p_tablemap->r$()->GetAt(r$index).left,
				p_tablemap->r$()->GetAt(r$index).bottom - p_tablemap->r$()->GetAt(r$index).top,
				SRCCOPY );

	// Copy 2nd DC to control
	BitBlt( hdcControl, 1, 1, rect.right-rect.left-2, rect.bottom-rect.top-2,
			hdcCompat2, 0, 0, SRCCOPY );

	// Output result
	trans.DoTransform(&(p_tablemap->r$()->GetAt(r$index)), hdcCompat1, &res);
	m_ScraperResult.SetWindowText(res);

	// Clean up
	SelectObject(hdcCompat1, old_bitmap1);
	SelectObject(hdcCompat2, old_bitmap2);
	DeleteObject(hbm2);
	DeleteDC(hdcCompat1);
	DeleteDC(hdcCompat2);
	DeleteDC(hdcScreen);
	ReleaseDC(pDC);
}

void CDlgScraperOutput::OnCancel()
{
	CMainFrame			*pMyMainWnd  = (CMainFrame *) (theApp.m_pMainWnd);

	// Uncheck scraper output button on main toolbar
	pMyMainWnd->m_MainToolBar.GetToolBarCtrl().CheckButton(ID_MAIN_TOOLBAR_SCRAPER_OUTPUT, false);

	CDialog::OnCancel();
}
