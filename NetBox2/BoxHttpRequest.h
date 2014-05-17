
#pragma once

#include "BoxStream.h"
#include "BoxHttpAccept.h"
#include <BBrowserCaps.h>

// CBoxHttpRequest command target

class CBoxHttpRequest : public CBoxStream
{
	DECLARE_DYNAMIC(CBoxHttpRequest)
public:
	void Bind(CBoxHttpAccept* pAccept)
	{
		m_pAccept = pAccept;
	}

public:
	virtual HRESULT StreamRead(void *pv, ULONG cb, ULONG *pcbRead);

public:
	afx_msg LPDISPATCH get_ClientCertificate(void);
	afx_msg LPDISPATCH get_Cookies(void);
	afx_msg LPDISPATCH get_Form(void);
	afx_msg LPDISPATCH get_QueryString(void);
	afx_msg LPDISPATCH get_ServerVariables(void);
	afx_msg LPDISPATCH get_Browser(void);

	afx_msg long get_TotalBytes(void);
	afx_msg BSTR get_ContentType(void);
	afx_msg VARIANT BinaryRead(long count);

	afx_msg VARIANT get_Item(VARIANT& varName);

	DECLARE_DISPATCH_MAP()

private:
	void CheckClientCertificate(void);
	void CheckCookies(void);
	void CheckForm(void);
	void CheckQueryString(void);
	void CheckServerVariables(void);

private:
	CBoxObject<CBoxHttpAccept> m_pAccept;

	CBComPtr<CBRequestDictionary> m_pClientCertificate;
	CBComPtr<CBRequestDictionary> m_pCookies;
	CBComPtr<CBRequestDictionary> m_pForm;
	CBComPtr<CBRequestDictionary> m_pQueryString;
	CBComPtr<CBRequestDictionary> m_pServerVariables;
	CBComPtr<CBBrowserCaps> m_pBrowserCaps;
};
