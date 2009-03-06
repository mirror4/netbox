// CertView.cpp : implementation file
//

#include "stdafx.h"
#include "NBR.h"
#include "CertView.h"
#include ".\certview.h"
#include ".\logon.h"


// CCertView dialog

IMPLEMENT_DYNAMIC(CCertView, CDialog)
CCertView::CCertView(CWnd* pParent /*=NULL*/)
	: CDialog(CCertView::IDD, pParent)
{
}

CCertView::~CCertView()
{
}

void CCertView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UPDATE, m_wndUpdate);
	DDX_Control(pDX, IDC_INFO, m_wndInfo);
}


BEGIN_MESSAGE_MAP(CCertView, CDialog)
	ON_BN_CLICKED(IDC_UPDATE, OnBnClickedUpdate)
END_MESSAGE_MAP()

void CCertView::ViewCert(void)
{
	theApp.LoadLic();

	if(theApp.m_certinfo.m_strError)
		SetDlgItemText(IDC_INFO, GetMessage(CBString(theApp.m_certinfo.m_strError)));

	m_wndUpdate.EnableWindow(!theApp.m_certinfo.m_bValid || theApp.m_certinfo.m_bUpdate);

	if(theApp.m_certinfo.m_nDevID)
	{
		SetDlgItemInt(IDC_DEVID, theApp.m_certinfo.m_nDevID);
		SetDlgItemText(IDC_DATE, theApp.m_certinfo.m_strStart + _T(" to ") + theApp.m_certinfo.m_strEnd);
	}else
	{
		SetDlgItemText(IDC_DEVID, _T("----------"));
		SetDlgItemText(IDC_DATE, _T("--------------"));
	}
}

BOOL CCertView::OnInitDialog()
{
	CDialog::OnInitDialog();

	ViewCert();

	SetWindowText(GetMessage(L"CertInfo"));
	SetDlgItemText(IDC_UPDATE, GetMessage(L"Update") + _T(" ..."));
	SetDlgItemText(IDOK, GetMessage(L"Close"));

	return TRUE;
}

void CCertView::OnBnClickedUpdate()
{
	CLogon dlg;
	dlg.DoModal();

	ViewCert();
}
