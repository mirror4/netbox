// NBR.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <BSysInfo.h>
//#include "CertInfo.h"
#include <BDate.h>
#include <BMStream.h>
#include <BMessageManager.h>

// CNBRApp:
// See NBR.cpp for the implementation of this class
//

class CNBRApp : public CWinApp
{
public:
	CNBRApp();

// Overrides
	public:
	virtual BOOL InitInstance();

	CBComPtr<CBSysInfo> m_pSysInfo;
	CBComPtr<CBMessageManager> m_pMsgMan;
	CBComPtr<CBMessageManager::CColl> m_pMsgColl;
	CAtlArray<CBString> m_arrayLang;
	int m_nLanguage;

	void setLanguage(int l)
	{
		m_nLanguage = l;
		m_pMsgColl.Release();
		m_pMsgMan->GetLanguage(m_arrayLang[m_nLanguage], &m_pMsgColl);
	}

	CBBlockStream m_mStream;
	BYTE m_aHash[16];
//	CCertInfo m_certinfo;

	void LoadLic(void);

	CString m_strLogon;
	CString m_strBasePath;

	CString m_strStartPath;
	BOOL m_bBuild;

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CNBRApp theApp;

CString inline GetMessage(LPCWSTR pName)
{
	return CString(theApp.m_pMsgColl->GetMessage(pName));
}
