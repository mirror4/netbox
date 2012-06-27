
#pragma once

#include <atlbase.h>
#include <atlutil.h>
#include "ScriptHost.h"
#include "BoxLRUCache.h"
#include <BThread.h>

// CBoxScript command target

class CBoxScript : public CBoxSafeObject
{
	DECLARE_DYNCREATE(CBoxScript)
public:
	class CScriptError
	{
	public:
		CScriptError(void) :
			m_nLine(0),
			m_sCode(0)
		{}

		void Clear(void)
		{
			m_strSource.Empty();
			m_strFile.Empty();
			m_strDescription.Empty();
			m_sCode = 0;
			m_nLine = 0;
		}

		CString m_strSource;
		CString m_strFile;
		SCODE m_sCode;
		int m_nLine;
		CString m_strDescription;
	};

public:
	CBoxScript();
	virtual ~CBoxScript();

	void* GetBlock(int n)
	{
		if(m_pHost == NULL || n < 0 || n >= m_pHost->m_arrayBlock.GetCount())
			return NULL;

		return m_pHost->m_arrayBlock[n];
	}

	CScriptError& GetErrorMessage(void)
	{
		return m_error;
	}

	void SetLanguage(LPCTSTR pstrLanguage)
	{
		m_strLanguage = pstrLanguage;
	}

	void EnableDebug(BOOL bEnableDebug = TRUE)
	{
		m_bEnableDebug = bEnableDebug;
	}

	void StepDebug(BOOL bStepDebug = TRUE)
	{
		m_bStepDebug = bStepDebug;
	}

	void AddLineMap(int nIncludeFlagIndex, int nFileLine)
	{
		m_pHost->m_arrayLine.Add(m_nScriptLine);
		m_pHost->m_arrayLine.Add(nIncludeFlagIndex);
		m_pHost->m_arrayLine.Add(nFileLine);
	}

	void SetParserError(int nLineNo, LPCTSTR pstrFile, LPCTSTR pstrDescription)
	{
		m_error.m_strSource = _T("NetBox script parser error");
		m_error.m_strDescription = pstrDescription;
		m_error.m_strFile = pstrFile;
		m_error.m_nLine = nLineNo;
	}

	void Interrupt(LPCTSTR strError = NULL)
	{
		m_pHost->Interrupt(strError);
	}

	static void clearCache(void);

protected:
	virtual void InitScript(void);
	virtual int ParseScriptText(LPCSTR pstrText, int nCount, CStringA& strScriptText, int nIncludeFlagIndex);
	virtual int AfterParse(void);
	virtual void ClearScript(void);
	int LoadScriptFile(LPCTSTR pstrFile, CStringA& strScriptText, int nLineNo = 0);
	void GetScriptErrorInfo(void);

public:
	afx_msg long Load(LPCTSTR pstrFile);
	afx_msg long put_Source(LPCTSTR pstrScript);
	afx_msg void put_Timeout(long nTimeout);
	afx_msg long get_Timeout();
	afx_msg void AddNameItem(LPCTSTR pstrName, LPDISPATCH pUnk);
	afx_msg long Run(void);
	afx_msg void Close(void);
	afx_msg void SetGlobalObjects(void);
	afx_msg LPDISPATCH GetLastErrorDISP(void);

	CString m_strLanguage;
	UINT m_uiCodePage;

	DECLARE_DISPATCH_MAP()

protected:
	DWORD m_dwThreadId;

	CComPtr<CScriptHost> m_pHost;
	CScriptError m_error;

	BOOL m_bEnableDebug;
	int m_nTransaction;
	BOOL m_bStepDebug;
	CString m_strBasePath;
	CWordArray m_arrayIncludeFlags;
	long m_nDiskFileCount;
	long m_nCacheFileCount;
	int m_nScriptLine;
};

//extern CBoxLRUCache<CBoxScript::CScriptCache> g_cacheScript;
