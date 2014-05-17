#pragma once
#include <afxcmn.h>
#include <afxwin.h>


// CCertView dialog

class CCertView : public CDialog
{
	DECLARE_DYNAMIC(CCertView)

public:
	CCertView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCertView();

// Dialog Data
	enum { IDD = IDD_CERTVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void ViewCert(void);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CButton m_wndUpdate;
	afx_msg void OnBnClickedUpdate();
	CStatic m_wndInfo;
};
