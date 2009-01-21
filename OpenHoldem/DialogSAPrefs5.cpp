// DialogSAPrefs5.cpp : implementation file
//

#include "stdafx.h"

#include "SAPrefsSubDlg.h"
#include "DialogSAPrefs5.h"

#include "CPreferences.h"

// CDlgSAPrefs5 dialog

IMPLEMENT_DYNAMIC(CDlgSAPrefs5, CSAPrefsSubDlg)

CDlgSAPrefs5::CDlgSAPrefs5(CWnd* pParent /*=NULL*/)
		: CSAPrefsSubDlg(CDlgSAPrefs5::IDD, pParent)
{
	__SEH_SET_EXCEPTION_HANDLER
}

CDlgSAPrefs5::~CDlgSAPrefs5()
{
}

void CDlgSAPrefs5::DoDataExchange(CDataExchange* pDX)
{
	CSAPrefsSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HANDRANKVALUE, m_HandrankValue);
	DDX_Control(pDX, IDC_AVTIME, m_AvTime);
	DDX_Control(pDX, IDC_DISABLE_CACHING, m_DisableCaching);
}

BEGIN_MESSAGE_MAP(CDlgSAPrefs5, CSAPrefsSubDlg)
END_MESSAGE_MAP()

// CDlgSAPrefs5 message handlers
BOOL CDlgSAPrefs5::OnInitDialog()
{
	CString		text = "";

	CSAPrefsSubDlg::OnInitDialog();

	m_HandrankValue.AddString("169");
	m_HandrankValue.AddString("1000");
	m_HandrankValue.AddString("1326");
	m_HandrankValue.AddString("2652");
	m_HandrankValue.AddString("p");
	m_HandrankValue.SelectString(0, prefs.sym_handrank_value());

	text.Format("%.2f", prefs.sym_av_time());
	m_AvTime.SetWindowText(text);

	m_DisableCaching.SetCheck(prefs.sym_disable_caching() ? BST_CHECKED : BST_UNCHECKED);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSAPrefs5::OnOK()
{
	CString			text = "";

	m_HandrankValue.GetWindowText(text);
	prefs.set_sym_handrank_value(text);

	m_AvTime.GetWindowText(text);
	prefs.set_sym_av_time(atof(text.GetString()));

	prefs.set_sym_disable_caching(m_DisableCaching.GetCheck()==BST_CHECKED ? true : false);

	CSAPrefsSubDlg::OnOK();
}
