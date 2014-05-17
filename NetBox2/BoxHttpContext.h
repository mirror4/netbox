
#pragma once

#include "BoxHttpAccept.h"
#include "BoxHttpRequest.h"
#include "BoxHttpResponse.h"

// CBoxHttpContext command target

class CBoxHttpContext : public CBoxSafeObject
{
	DECLARE_DYNAMIC(CBoxHttpContext)

public:
	CBoxHttpContext(void)
	{
		m_pRequest.CreateInstance();
		m_pResponse.CreateInstance();
	}

	void Bind(CBoxHttpAccept* pAccept, CBoxScript* pScript);

public:
	afx_msg void put_ScriptTimeout(long tm);
	afx_msg long get_ScriptTimeout(void);

	afx_msg LPDISPATCH CreateObject(LPCTSTR pstrClass);
	afx_msg void Execute(LPCTSTR pstrPath);
	afx_msg LPDISPATCH GetLastErrorDISP(void);
	afx_msg BSTR MapPath(LPCTSTR pstrPath);
	afx_msg void Transfer(LPCTSTR pstrPath);

	afx_msg BSTR HtmlEncode(LPCTSTR pstrText);
	afx_msg BSTR UrlEncode(LPCTSTR pstrText);

	DECLARE_DISPATCH_MAP()

public:
	CBoxObject<CBoxHttpAccept> m_pAccept;
	CBoxObject<CBoxHttpRequest> m_pRequest;
	CBoxObject<CBoxHttpResponse> m_pResponse;

	CString m_strBasePath;
};
