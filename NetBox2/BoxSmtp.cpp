// BoxSmtp.cpp : implementation file
//

#include "stdafx.h"
#include "BoxSmtp.h"


// CBoxSmtp

IMPLEMENT_DYNCREATE(CBoxSmtp, CBoxSafeObject)


BEGIN_DISPATCH_MAP(CBoxSmtp, CBoxSafeObject)
	DISP_FUNCTION(CBoxSmtp, "Close", Close, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxSmtp, "Connect", Connect, VT_I4, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CBoxSmtp, "SSLConnect", SSLConnect, VT_I4, VTS_BSTR VTS_I4)

	DISP_FUNCTION(CBoxSmtp, "Hello", Hello, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CBoxSmtp, "mailFrom", mailFrom, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CBoxSmtp, "rcptTo", rcptTo, VT_I4, VTS_BSTR)

	DISP_FUNCTION(CBoxSmtp, "sendData", sendData, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CBoxSmtp, "BeginData", BeginData, VT_I4, VTS_NONE)
	DISP_FUNCTION(CBoxSmtp, "EndData", EndData, VT_I4, VTS_NONE)
	DISP_FUNCTION(CBoxSmtp, "WriteData", WriteData, VT_EMPTY, VTS_BSTR)

	DISP_FUNCTION(CBoxSmtp, "Reset", Reset, VT_I4, VTS_NONE)

	DISP_FUNCTION(CBoxSmtp, "sendMail", sendMail, VT_I4, VTS_BSTR VTS_BSTR VTS_BSTR)
END_DISPATCH_MAP()

// CBoxSmtp message handlers

void CBoxSmtp::Close(void)
{
	m_pSocket->Write(_T("DATA\r\n"));
	m_pSocket->Close();
}

long CBoxSmtp::Connect(LPCTSTR pstrHostAddress, UINT nHostPort)
{
	long l_ret;

	l_ret = m_pSocket->Connect(pstrHostAddress, nHostPort);
	if(l_ret)return l_ret;

	return waitResponse(220);
}

long CBoxSmtp::SSLConnect(LPCTSTR pstrHostAddress, UINT nHostPort)
{
	long l_ret;

	l_ret = m_pSocket->Connect(pstrHostAddress, nHostPort);
	if(l_ret)return l_ret;
	l_ret = m_pSocket->SSLConnect();
	if(l_ret)return l_ret;

	return waitResponse(220);
}

int CBoxSmtp::waitResponse(int ErrNo)
{
	CString str;
	int l_ret;

	do
	{
		if(m_pSocket->ReadString(str))return -1;
		if((l_ret = _tstoi(str)) != ErrNo)return l_ret;
	}while(str[3] != ' ');

	return 0;
}

long CBoxSmtp::Hello(LPCTSTR pstrDomain)
{
	m_pSocket->Write(_T("HELO "));
	m_pSocket->WriteLine(pstrDomain);
	return waitResponse(250);
}

long CBoxSmtp::mailFrom(LPCTSTR pstrMail)
{
	m_pSocket->Write(_T("MAIL FROM: <"));
	m_pSocket->Write(pstrMail);
	m_pSocket->Write(">\r\n");
	return waitResponse(250);
}

long CBoxSmtp::rcptTo(LPCTSTR pstrMail)
{
	m_pSocket->Write(_T("RCPT TO: <"));
	m_pSocket->Write(pstrMail);
	m_pSocket->Write(">\r\n");
	return waitResponse(250);
}

long CBoxSmtp::BeginData()
{
	m_pSocket->Write(_T("DATA\r\n"));
	return waitResponse(354);
}

long CBoxSmtp::EndData()
{
	m_pSocket->Write(_T("\r\n.\r\n"));
	return waitResponse(250);
}

void CBoxSmtp::WriteData(LPCTSTR pstrData)
{
	m_pSocket->Write(pstrData);
}

long CBoxSmtp::sendData(LPCTSTR pstrData)
{
	int l_ret;

	l_ret = BeginData();
	if(l_ret)return l_ret;

	WriteData(pstrData);

	return EndData();
}

long CBoxSmtp::Reset()
{
	m_pSocket->Write(_T("RSET\r\n"));
	return waitResponse(250);
}

long CBoxSmtp::sendMail(LPCTSTR pstrMailFrom, LPCTSTR pstrMailTo, LPCTSTR pstrData)
{
	int l_ret;

	l_ret = mailFrom(pstrMailFrom);
	if(l_ret)return l_ret;

	l_ret = rcptTo(pstrMailTo);
	if(l_ret)return l_ret;

	l_ret = sendData(pstrData);
	if(l_ret)return l_ret;

	return Reset();
}
