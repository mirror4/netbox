// SiDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NBR.h"
#include "sidlg.h"


// CSiDlg dialog

IMPLEMENT_DYNAMIC(CSiDlg, CDialog)
CSiDlg::CSiDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSiDlg::IDD, pParent)
{
}

CSiDlg::~CSiDlg()
{
}

void CSiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SILIST, m_wndSiList);
}


BEGIN_MESSAGE_MAP(CSiDlg, CDialog)
END_MESSAGE_MAP()


// CSiDlg message handlers

BOOL CSiDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndSiList.ModifyStyle(0, LVS_REPORT);
	m_wndSiList.InsertColumn(0, GetMessage(L"Item"), LVCFMT_LEFT, 100);
	m_wndSiList.InsertColumn(1, GetMessage(L"Value"), LVCFMT_LEFT, 340);
	m_wndSiList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	long nCount, i;
	CString k, v;

	theApp.m_pSysInfo->get_Count(&nCount);

	for(i = 0; i < nCount; i ++)
	{
		theApp.m_pSysInfo->GetAssoc(i, k, v);
		m_wndSiList.InsertItem(i, k);
		m_wndSiList.SetItemText(i, 1, v);
	}

	SetWindowText(GetMessage(L"SysInfo"));
	SetDlgItemText(IDOK, GetMessage(L"Close"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
