#pragma once
#include <afxwin.h>


// CDlgLang dialog

class CDlgLang : public CDialog
{
	DECLARE_DYNAMIC(CDlgLang)

public:
	CDlgLang(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLang();

// Dialog Data
	enum { IDD = IDD_LANGUAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_wndLang;
	virtual BOOL OnInitDialog();

protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnLbnSelchangeLanguage();
	CButton m_wndOK;
	afx_msg void OnLbnDblclkLanguage();
};
