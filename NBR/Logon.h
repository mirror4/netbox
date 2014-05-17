#pragma once
#include <afxwin.h>


// CLogon dialog

class CLogon : public CDialog
{
	DECLARE_DYNAMIC(CLogon)

public:
	CLogon(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLogon();

// Dialog Data
	enum { IDD = IDD_LOGON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_wndUser;
	CEdit m_wndPassword;
	afx_msg void OnBnClickedRegister();
protected:
	virtual void OnOK();
};
