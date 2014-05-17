// DlgLang.cpp : implementation file
//

#include "stdafx.h"
#include "NBR.h"
#include "DlgLang.h"
#include ".\dlglang.h"


// CDlgLang dialog

IMPLEMENT_DYNAMIC(CDlgLang, CDialog)
CDlgLang::CDlgLang(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLang::IDD, pParent)
{
}

CDlgLang::~CDlgLang()
{
}

void CDlgLang::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LANGUAGE, m_wndLang);
	DDX_Control(pDX, IDOK, m_wndOK);
}


BEGIN_MESSAGE_MAP(CDlgLang, CDialog)
	ON_LBN_SELCHANGE(IDC_LANGUAGE, OnLbnSelchangeLanguage)
	ON_LBN_DBLCLK(IDC_LANGUAGE, OnLbnDblclkLanguage)
END_MESSAGE_MAP()


// CDlgLang message handlers

BOOL CDlgLang::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(int i = 0; i < (int)theApp.m_arrayLang.GetCount(); i ++)
		m_wndLang.AddString(CString(theApp.m_arrayLang[i]));

	return TRUE;
}

void CDlgLang::OnCancel()
{
	if(theApp.m_nLanguage != -1)
		CDialog::OnCancel();
}

void CDlgLang::OnOK()
{
	if(theApp.m_nLanguage != -1)
		CDialog::OnOK();
}

void CDlgLang::OnLbnSelchangeLanguage()
{
	theApp.setLanguage(m_wndLang.GetCurSel());

	SetWindowText(GetMessage(L"SelLang"));
	SetDlgItemText(IDOK, GetMessage(L"OK"));

	m_wndOK.EnableWindow();
}

void CDlgLang::OnLbnDblclkLanguage()
{
	OnLbnSelchangeLanguage();
	OnOK();
}
