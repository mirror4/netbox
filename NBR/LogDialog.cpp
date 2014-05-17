// LogDialog.cpp : implementation file
//

#include "stdafx.h"
#include "NBR.h"
#include "LogDialog.h"
#include ".\logdialog.h"


// CLogDialog dialog

IMPLEMENT_DYNAMIC(CLogDialog, CDialog)
CLogDialog::CLogDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLogDialog::IDD, pParent)
{
	m_bInit = FALSE;
	m_bWantClose = FALSE;
}

CLogDialog::~CLogDialog()
{
}

void CLogDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG, m_wndOutput);
	DDX_Control(pDX, IDCANCEL, m_wndCancel);
}


BEGIN_MESSAGE_MAP(CLogDialog, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_MESSAGE(WM_WRITELOG, OnWriteLog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CLogDialog message handlers

void CLogDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CSize sizeNew(cx, cy);

	if(m_bInit)
	{
		CRect rcBtn;

		m_rcEdit.BottomRight() = sizeNew - m_ptEdit;
		m_wndOutput.MoveWindow(&m_rcEdit);

		rcBtn = m_rcButton + sizeNew;
		m_wndCancel.MoveWindow(&rcBtn);
	}
}

BOOL CLogDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rcDlg, rcBtn;

	GetClientRect(&rcDlg);

	m_wndOutput.SetLimitText(0);
	m_wndOutput.GetWindowRect(&m_rcEdit);
	ScreenToClient(&m_rcEdit);
	m_ptEdit = rcDlg.Size() - m_rcEdit.BottomRight();

	m_wndCancel.GetWindowRect(&rcBtn);
	ScreenToClient(&rcBtn);
	m_rcButton = rcBtn - rcDlg.Size();

	m_bInit = TRUE;

	SetDlgItemText(IDCANCEL, GetMessage(L"Cancel"));

	m_th.StartWorker(this);

	return TRUE;
}

BOOL CLogDialog::WriteLog(LPCTSTR szText)
{
	return (BOOL)SendMessage(WM_WRITELOG, 0, (LPARAM)szText);
}

LRESULT CLogDialog::OnWriteLog(WPARAM wParam, LPARAM lParam)
{
	if(*(LPCTSTR)lParam)
	{
		m_wndOutput.SetSel(IS_NT ? 0x7FFFFFFE : 0xFFFE, -1);
		m_wndOutput.ReplaceSel((LPCTSTR)lParam);
	}
	return !m_bWantClose;
}

void CLogDialog::OnBnClickedCancel()
{
	if(m_bWantClose)
	{
		if(!WaitForSingleObject(m_th.m_hThread, 0))
			CDialog::OnOK();
	}else
	{
		m_bWantClose = TRUE;
		m_wndCancel.SetWindowText(GetMessage(L"Close"));
		m_wndCancel.SetFocus();
	}
}

void CLogDialog::OnOK()
{
	if(m_bWantClose)
		WaitForSingleObject(m_th.m_hThread, INFINITE);
	else OnBnClickedCancel();
}


HBRUSH CLogDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if(nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetTextColor(RGB(0, 255, 0));
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(BLACK_BRUSH);

	}else return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}
