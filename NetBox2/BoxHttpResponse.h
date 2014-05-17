
#pragma once

#include "BoxStream.h"
#include "BoxHttpAccept.h"
#include "BoxScript.h"

// CBoxHttpResponse command target

class CBoxHttpResponse : public CBoxStream
{
	DECLARE_DYNAMIC(CBoxHttpResponse)
public:
	CBoxHttpResponse(void) :
		m_bEnd(FALSE)
	{}

	void Bind(CBoxHttpAccept* pAccept, CBoxScript* pScript)
	{
		m_pAccept = pAccept;
		m_pScript = pScript;
	}

	virtual HRESULT StreamWrite(const void *pv, ULONG cb, ULONG *pcbWritten);

	CBoxScript* m_pScript;
	BOOL m_bEnd;

public:
	afx_msg LPDISPATCH get_Cookies(void);

	afx_msg BOOL get_Buffer(void);
	afx_msg void put_Buffer(BOOL bBuffer);

	afx_msg void put_CacheControl(LPCTSTR pstrCacheControl);
	afx_msg void put_ContentType(LPCTSTR pstrContentType);
	afx_msg void put_ContentEncoding(LPCTSTR pstrContentEncoding);
	afx_msg void put_Expires(long nTime);
	afx_msg void put_ExpiresAbsolute(DATE dateExpires);
	afx_msg void put_Status(LPCTSTR pstrStatus);

	afx_msg void AddHeader(LPCTSTR pstrName, LPCTSTR pstrValue);
	afx_msg void AppendToLog(LPCTSTR pstrLog);
	afx_msg void BinaryWrite(VARIANT& var);
	afx_msg void Clear(void);
	afx_msg void End(void);
	afx_msg void Flush(void);
	afx_msg void PICS(LPCTSTR pstrPICSLabel);
	afx_msg void Redirect(LPCTSTR pstrUrl);
	afx_msg void MapToFile(LPCTSTR pstrFile);
	afx_msg void Write(VARIANT& var);
	afx_msg void WriteBlock(long id);

	afx_msg BOOL IsClientConnected(void);

	afx_msg LPDISPATCH get_Socket(void);

	DECLARE_DISPATCH_MAP()

private:
	CBoxObject<CBoxHttpAccept> m_pAccept;
	CBComPtr<CBWriteCookieDictionary> m_pCookies;
};
