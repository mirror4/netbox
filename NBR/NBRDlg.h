// NBRDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "BuildBtn.h"
#include <bpath.h>

// CNBRDlg dialog
class CNBRDlg : public CDialog
{
// Construction
public:
	CNBRDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_NBR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
private:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CEdit m_wndOutput;
	CListCtrl m_wndSource;
	CComboBox m_wndSelect;
	CComboBox m_wndSrcFolder;
	CBuildBtn m_wndBuild;
	CStatic m_wndPBNB;
	CStatic m_wndName;
	CStatic m_wndLogo;
	CComboBox m_wndStartup;
	CComboBox m_wndIcon;
	CComboBox m_wndPRI;

	CEdit m_wndFileDescription;
	CEdit m_wndFileVersion;
	CEdit m_wndProductVersion;
	CEdit m_wndProductName;
	CEdit m_wndLegalCopyright;

	CBPath m_strSrcFolder;
	CString m_strStartup;
	CString m_strIcon;

	CMenu m_menuLang;

	CSize m_sizeMin;
	CSize m_sizeMain;

	CImageList m_imgs;
	CAtlArray<CString> m_arrayType;
	CRBMap<CString, int> m_typeIndex;

private:
	void GetSourceFolder(LPCTSTR szPath);
	void ChangeSelect(BOOL bCheck);
	void LoadFileArray(LPCTSTR pstrFile, CStringArray& strArray);
	void SaveFileArray(LPCTSTR pstrFile, CStringArray& strArray);

	void setLanguage(void);

public:
	afx_msg void OnBnClickedBrowser();
	afx_msg void OnBnClickedFolder();
	afx_msg void OnCbnSelchangeSrcfolder();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedApplication();
	afx_msg void OnBnClickedLibrary();
	afx_msg void OnBnClickedViewcert();
	afx_msg void OnBnClickedSysinfo();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLvnItemchangedSource(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedConsole();
};
