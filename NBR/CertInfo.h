#pragma once

#include <BDate.h>

class CCertInfo
{
public:
	CCertInfo(void);
	~CCertInfo(void);

	BOOL SetLic(void* pBuffer);
	void CheckDate(DATE d);
	void Clear(void);

	BYTE m_aHash[16];
	int m_nDevID;
	CString m_strStart;
	CString m_strEnd;

	CBDate m_dateEnd;
	CBDate m_dateTimeout;

	BOOL m_bUpdate;
	BOOL m_bValid;

	LPCTSTR m_strError;
};

