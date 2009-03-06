
#pragma once
#include "BoxObject.h"
#include "BoxSocket.h"

// CBoxSmtp command target

class CBoxSmtp : public CBoxSafeObject
{
	DECLARE_DYNCREATE(CBoxSmtp)

public:
	CBoxSmtp(void)
	{
		m_pSocket.CreateInstance();
	}
	virtual ~CBoxSmtp(){Close();}

public:
	afx_msg void Close(void);
	afx_msg long Connect(LPCTSTR pstrHostAddress, UINT nHostPort);
	afx_msg long SSLConnect(LPCTSTR pstrHostAddress, UINT nHostPort);

	afx_msg long Hello(LPCTSTR pstrDomain);
	afx_msg long mailFrom(LPCTSTR pstrMail);
	afx_msg long rcptTo(LPCTSTR pstrMail);
	afx_msg long Reset();

	afx_msg long sendData(LPCTSTR pstrData);
	afx_msg long BeginData();
	afx_msg long EndData();
	afx_msg void WriteData(LPCTSTR pstrData);

	afx_msg long sendMail(LPCTSTR pstrMailFrom, LPCTSTR pstrMailTo, LPCTSTR pstrData);

	DECLARE_DISPATCH_MAP()

private:
	int waitResponse(int ErrNo);

	CBoxObject<CBoxSocket> m_pSocket;
};
