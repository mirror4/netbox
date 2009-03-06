#pragma once
#include <afxcmn.h>


// CSiDlg dialog

class CSiDlg : public CDialog
{
	DECLARE_DYNAMIC(CSiDlg)

public:
	CSiDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSiDlg();

// Dialog Data
	enum { IDD = IDD_SYSINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_wndSiList;
	virtual BOOL OnInitDialog();
};
