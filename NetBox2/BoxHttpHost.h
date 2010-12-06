
#pragma once

#include "BoxApplication.h"
#include "BoxDictionary.h"
#include "BoxHttpServer.h"
#include "BoxLRUCache.h"
#include "BoxHttpSession.h"
#include <atlrx.h>

#include <openssl\rand.h>

// CBoxHttpHost command target

class CBoxHttpContext;

class CBoxHttpHost : public CBoxContents
{
	class CURLRECharTraits : public CAtlRECharTraits
	{
	public:
		static const TCHAR** GetAbbrevs()
		{
			static const TCHAR *s_szAbbrevs[] = 
			{
				_T("b([ \\t])"),		// white space (blank)
				_T("B([^ \\t])"),		// not white space (blank)
				_T("c([a-zA-Z])"),	// alpha
				_T("C(^[a-zA-Z])"),	// not alpha
				_T("d([0-9])"),		// digit
				_T("D(^[0-9])"),		// not digit
				_T("f(\f)"),		// form-feed
				_T("h([0-9a-fA-F])"),	// hex digit
				_T("n(\n)"),	// newline
				_T("q(\"[^\"]*\")|(\'[^\']*\')"),	// quoted string
				_T("r(\r))"),	// newline
				_T("s([ \f\n\r\t\v])"),	// whitespace character
				_T("S([^ \f\n\r\t\v])"),	// not whitespace character
				_T("t(\t)"),	// tab
				_T("v(\v)"),	// vertical tab
				_T("w([A-Za-z0-9_])"),	// word
				_T("W([^A-Za-z0-9_])"),	// word
				_T("z([0-9]+)"),		// integer
				NULL
			};

			return s_szAbbrevs;
		}
	};

	DECLARE_DYNAMIC(CBoxHttpHost)
public:
	CBoxHttpHost(void);
	~CBoxHttpHost(void)
	{
		int c = m_areUrl.GetCount();
		for (int i=0;i<c;i++)
		{
			delete m_areUrl[i];
		}
	}

	void InitHost(LPCTSTR pstrRoot)
	{
		BYTE randstr[21];

		m_strRoot = pstrRoot;

		RAND_seed(randstr, sizeof(randstr));
		RAND_seed((LPCTSTR)m_strRoot, m_strRoot.GetLength() * sizeof(TCHAR));

		RAND_bytes(randstr, sizeof(randstr) - 1);
		for(int i = 0; i < sizeof(randstr) - 1; i ++)
			randstr[i] = (randstr[i] % 26) + 'A';
		randstr[sizeof(randstr) - 1] = 0;

		m_strHostKey = randstr;
		m_pApplication->m_strHostKey = m_strHostKey;

		m_strEventSink = m_strRoot + _T("\\global.wsc");
	}

	BOOL GetSession(CString& strSessionCookie, CBoxObject<CBoxHttpSession>& pSession, CBoxHttpContext *pContext);
	void ClearSession(__int64 nNowTime);
	void OnStart(void);
	void OnEnd(void);
	BOOL URLRewrite(CString strURL, CString &strNewURL, CString &strNewQueryString);

public:
	CString m_strHostKey;
	CString m_strRoot;
	CString m_strLanguage;
	CString m_strBrowseHeader;
	CString m_strBrowseFooter;
	CStringA m_strErrorMessage;
	CStringArray m_astrDefault;
	BOOL m_bEnableScript;
	BOOL m_bEnableDebug;
	BOOL m_bEnableSession;
	BOOL m_bGlobalApplication;
	BOOL m_bEnableBrowse;
	BOOL m_bAutoWebDAV;
	BOOL m_bEnableWrite;
	long m_nScriptTimeout;
	CString m_strEventSink;
	CString m_strAddHeader;
	CString m_strSessionDomain;

	CBoxObject<CBoxApplication> m_pApplication;

	void AbandonSession(CBoxHttpSession* pItem);
	void ResetSession(CBoxHttpSession* pItem);

	void CallEvent(LPCWSTR strEvent, CBoxHttpContext *pContext, CBoxHttpSession *pSession, BOOL bAddHost = FALSE);

	BOOL m_bIsStart;

	CAtlArray<CAtlRegExp<CURLRECharTraits> *> m_areUrl;
	CAtlArray<CString> m_aUrl;

public:
	afx_msg long get_SessionTimeout(void);
	afx_msg void put_SessionTimeout(long nSessionTimeout);

	afx_msg VARIANT get_Application(VARIANT& varKey);
	afx_msg void put_Application(VARIANT& varKey, VARIANT& var);

	afx_msg BSTR get_EventSink(void);
	afx_msg void put_EventSink(LPCTSTR pstrName);

	afx_msg BSTR get_ApplicationID(void);
	afx_msg void put_ApplicationID(LPCTSTR pstrID);

	afx_msg void AddDefault(LPCTSTR pstrDefault);
	afx_msg void AddHeader(LPCTSTR pstrName, LPCTSTR pstrValue);

	afx_msg LPDISPATCH AddFolder(LPCTSTR pstrName, LPCTSTR pstrRoot);
	afx_msg void AttachFolder(LPCTSTR pstrName, LPDISPATCH pdispHost);

	afx_msg void AddURLRewriter(LPCTSTR pstrRE, LPCTSTR pstr);
	afx_msg BSTR URLRewriteTest(LPCTSTR pstr);

	DECLARE_DISPATCH_MAP()

private:
	long m_nSessionTimeout;
	CCriticalSection m_cs;

	CBoxLRUCache<CBoxHttpSession> m_cacheSessions;

	__int64 m_nNowTime;
};
