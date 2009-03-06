#pragma once

#include "Worker.h"


#define WM_WRITELOG		(WM_USER + 1000)

// CLogDialog dialog

class CLogDialog : public CDialog
{
	DECLARE_DYNAMIC(CLogDialog)

public:
	CLogDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLogDialog();

// Dialog Data
	enum { IDD = IDD_LOG };

	BOOL WriteLog(LPCTSTR szText);

private:
	CEdit m_wndOutput;
	CButton m_wndCancel;

	CRect m_rcEdit, m_rcButton;
	CPoint m_ptEdit;
	BOOL m_bInit;
	BOOL m_bWantClose;

	CWorker m_th;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedCancel();
	afx_msg LRESULT OnWriteLog(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
