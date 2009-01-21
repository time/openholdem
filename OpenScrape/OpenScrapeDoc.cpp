// OpenScrapeDoc.cpp : implementation of the COpenScrapeDoc class
//

#include "stdafx.h"
#include "OpenScrape.h"
#include "MainFrm.h"
#include "OpenScrapeDoc.h"
#include "../CTransform/hash/lookup3.h"
#include "DialogTableMap.h"
#include "global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// COpenScrapeDoc
IMPLEMENT_DYNCREATE(COpenScrapeDoc, CDocument)

BEGIN_MESSAGE_MAP(COpenScrapeDoc, CDocument)
END_MESSAGE_MAP()


// COpenScrapeDoc construction/destruction

COpenScrapeDoc::COpenScrapeDoc()
{
    __SEH_SET_EXCEPTION_HANDLER

	p_tablemap->ClearTablemap();

	attached_hwnd = NULL;
	ZeroMemory(&attached_rect, sizeof(RECT));
	attached_bitmap = NULL;
	attached_pBits = NULL;
	blink_on = false;
	valid_open = false;
	is_dirty = false;
}

COpenScrapeDoc::~COpenScrapeDoc()
{
}

COpenScrapeDoc * COpenScrapeDoc::GetDocument() 
{
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);

	return (COpenScrapeDoc *) pFrame->GetActiveDocument();
}

BOOL COpenScrapeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	
	p_tablemap->ClearTablemap();

	if (theApp.m_TableMapDlg)  
		theApp.m_TableMapDlg->create_tree();

	return TRUE;
}

BOOL COpenScrapeDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	if (!valid_open)
	{
		p_tablemap->ClearTablemap();
		SetTitle("");
	}
	
	SetModifiedFlag(is_dirty);

	// Create tree on TableMap dialog
	if (theApp.m_TableMapDlg)  theApp.m_TableMapDlg->create_tree();

	// Force re-draw
	InvalidateRect(theApp.m_pMainWnd->GetSafeHwnd(), NULL, true);
	if (theApp.m_TableMapDlg)  theApp.m_TableMapDlg->Invalidate(true);

	return valid_open;
}

// COpenScrapeDoc serialization

void COpenScrapeDoc::Serialize(CArchive& ar)
{
	CString			loaded_version, s;
	CMainFrame		*pMyMainWnd  = (CMainFrame *) (theApp.m_pMainWnd);
	int				linenum, ret;

	if (ar.IsStoring())
	{
		p_tablemap->SaveTablemap(ar, VERSION_TEXT);
	}

	else
	{
		loaded_version = "";
		ret = p_tablemap->LoadTablemap((char *) ar.m_strFileName.GetString(), "", false, &linenum, &loaded_version);

		if (loaded_version == VER_WINSCRAPE && ret == SUCCESS)
		{
			if (MessageBox(pMyMainWnd->GetSafeHwnd(), "This is a WinScrape table 'profile'.  Would you like to convert to OpenScrape table map format?", 
						   "Convert table 'profile'?", MB_YESNO) == IDYES)
			{
				ret = p_tablemap->ConvertTablemap(pMyMainWnd->GetSafeHwnd(), _startup_path);

				if (ret != SUCCESS && ret != ERR_INCOMPLETEMASTER)
				{
					s.Format("Error %d converting table 'profile'.", ret);
					MessageBox(pMyMainWnd->GetSafeHwnd(), s.GetString(), "Table 'profile' conversion error.", MB_OK);
					is_dirty = false;
					valid_open = false;
				}
				else
				{
					is_dirty = true;
					valid_open = true;
				}
			}
			else
			{
				is_dirty = false;
				valid_open = false;
			}
		}

		else if (loaded_version == VER_WINSCRAPE && ret != SUCCESS)
		{
			MessageBox(pMyMainWnd->GetSafeHwnd(), "This is a WinScrape 'profile', but cannot be converted,\n"\
												  "as it does not have a version date of\n"\
												  "\"2006 Aug 7 13:11:37\" or\n"\
												  "\"2007 Oct 25 19:26:57\" or\n"\
												  "\"2007 Nov 1 08:32:55\" or\n"\
												  "\"2007 Nov 3 09:54:04\" or\n"\
												  "\"2007 Nov 6 11:52:24\" or\n",  "Table map load error", MB_OK);
			is_dirty = false;
			valid_open = false;
		}

		else if (ret != SUCCESS)
		{
			s.Format("Error %d loading table map.", ret);
			MessageBox(pMyMainWnd->GetSafeHwnd(), s.GetString(), "Table map load error", MB_OK);
			is_dirty = false;
			valid_open = false;
		}
		else 
		{
			is_dirty = false;
			valid_open = true;
		}
	}
}



// COpenScrapeDoc diagnostics

#ifdef _DEBUG
void COpenScrapeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COpenScrapeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// COpenScrapeDoc commands

