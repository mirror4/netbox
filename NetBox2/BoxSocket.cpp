// BoxSocket.cpp : implementation file
//

#include "stdafx.h"
#include "NetBox2.h"
#include "BoxSocket.h"
#include "BoxObject.h"
#include "BoxBinPtr.h"

// CBoxSocket

IMPLEMENT_DYNCREATE(CBoxSocket, CBoxSafeObject)

CBoxSocket::CBoxSocket() :
	m_nPos(0),
	m_nSize(0),
	m_nLastLineChar(0),
	m_bEcho(FALSE),
	m_nConnectLimit(0),
	m_dwAcceptIP(0),
	m_hSocket(INVALID_SOCKET),
	m_ulBindIP(INADDR_NONE),
	m_usBindPort(0)
{
	ZeroMemory(&m_varCookie, sizeof(m_varCookie));
	ZeroMemory(&m_pAsyn, sizeof(m_pAsyn));
}

CBoxSocket::~CBoxSocket()
{
	Close();
}

BEGIN_DISPATCH_MAP(CBoxSocket, CBoxSafeObject)
	DISP_FUNCTION(CBoxSocket, "Close", Close, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxSocket, "Bind", Bind, VT_I4, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CBoxSocket, "Connect", Connect, VT_I4, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CBoxSocket, "Listen", Listen, VT_I4, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CBoxSocket, "Accept", AcceptDispatch, VT_DISPATCH, VTS_NONE)

	DISP_FUNCTION(CBoxSocket, "SSLAccept", SSLAcceptDispatch, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CBoxSocket, "SSLConnect", SSLConnect, VT_I4, VTS_NONE)
	DISP_FUNCTION(CBoxSocket, "SSLClose", SSLClose, VT_EMPTY, VTS_NONE)

	DISP_FUNCTION(CBoxSocket, "BinaryRead", BinaryRead, VT_VARIANT, VTS_I4)
	DISP_FUNCTION(CBoxSocket, "ReadLine", ReadLine, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CBoxSocket, "ReadPassword", ReadPassword, VT_BSTR, VTS_NONE)

	DISP_FUNCTION(CBoxSocket, "BinaryWrite", BinaryWrite, VT_EMPTY, VTS_VARIANT)
	DISP_FUNCTION(CBoxSocket, "Write", Write, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CBoxSocket, "WriteLine", WriteLine, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CBoxSocket, "WriteBlankLines", WriteBlankLines, VT_EMPTY, VTS_I4)

	DISP_FUNCTION(CBoxSocket, "Resolve", Resolve, VT_BSTR, VTS_BSTR)

	DISP_PROPERTY_EX(CBoxSocket, "Cookie", get_Cookie, put_Cookie, VT_VARIANT)

	DISP_PROPERTY_EX(CBoxSocket, "LocalAddress", get_LocalAddress, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CBoxSocket, "LocalPort", get_LocalPort, SetNotSupported, VT_I4)

	DISP_PROPERTY_EX(CBoxSocket, "Handle", get_Handle, SetNotSupported, VT_I4)

	DISP_PROPERTY_EX(CBoxSocket, "RemoteAddress", get_RemoteAddress, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CBoxSocket, "RemotePort", get_RemotePort, SetNotSupported, VT_I4)

	DISP_PROPERTY_EX(CBoxSocket, "dataAvailable", get_dataAvailable, SetNotSupported, VT_I4)

	DISP_PROPERTY(CBoxSocket, "Echo", m_bEcho, VT_BOOL)
	DISP_PROPERTY_EX(CBoxSocket, "Eof", get_Eof, SetNotSupported, VT_BOOL)
	DISP_PROPERTY_EX(CBoxSocket, "Timeout", GetNotSupported, put_Timeout, VT_I4)
END_DISPATCH_MAP()

// CBoxSocket message handlers

HRESULT CBoxSocket::StreamRead(void *pv, ULONG cb, ULONG *pcbRead)
{
	if(m_hSocket == INVALID_SOCKET)
		return OLECMDERR_E_DISABLED;

	int lRead = ReadBuffer((char*)pv, cb);
	if(lRead == SOCKET_ERROR)
		return TYPE_E_IOERROR;
	if(pcbRead != NULL)
		*pcbRead = lRead;
	return S_OK;
}

HRESULT CBoxSocket::StreamWrite(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	if(m_hSocket == INVALID_SOCKET)
		return OLECMDERR_E_DISABLED;

	int lWrite = WriteBlock((char*)pv, cb);
	if(lWrite != cb)
		return TYPE_E_IOERROR;
	if(pcbWritten != NULL)
		*pcbWritten = cb;
	return S_OK;
}

long CBoxSocket::get_Handle(void)
{
	return (long)m_hSocket;
}

BSTR CBoxSocket::get_LocalAddress(void)
{
	SOCKADDR_IN sockAddr;
	int SockLen;

	SockLen = sizeof(SOCKADDR);
	getsockname(m_hSocket, (SOCKADDR*)&sockAddr, &SockLen);

	CString str;

	str.Format(_T("%d.%d.%d.%d"),
		sockAddr.sin_addr.S_un.S_un_b.s_b1,
		sockAddr.sin_addr.S_un.S_un_b.s_b2,
		sockAddr.sin_addr.S_un.S_un_b.s_b3,
		sockAddr.sin_addr.S_un.S_un_b.s_b4);

	return str.AllocSysString();
}

long CBoxSocket::get_LocalPort(void)
{
	SOCKADDR_IN sockAddr;
	int SockLen;

	SockLen = sizeof(SOCKADDR);
	getsockname(m_hSocket, (SOCKADDR*)&sockAddr, &SockLen);

	return ntohs(sockAddr.sin_port);
}

BSTR CBoxSocket::get_RemoteAddress(void)
{
	SOCKADDR_IN sockAddr;
	int SockLen;

	SockLen = sizeof(SOCKADDR);
	getpeername(m_hSocket, (SOCKADDR*)&sockAddr, &SockLen);

	CString str;

	str.Format(_T("%d.%d.%d.%d"),
		sockAddr.sin_addr.S_un.S_un_b.s_b1,
		sockAddr.sin_addr.S_un.S_un_b.s_b2,
		sockAddr.sin_addr.S_un.S_un_b.s_b3,
		sockAddr.sin_addr.S_un.S_un_b.s_b4);

	return str.AllocSysString();
}

long CBoxSocket::get_RemotePort(void)
{
	SOCKADDR_IN sockAddr;
	int SockLen;

	SockLen = sizeof(SOCKADDR);
	getpeername(m_hSocket, (SOCKADDR*)&sockAddr, &SockLen);

	return ntohs(sockAddr.sin_port);
}

VARIANT CBoxSocket::get_Cookie(void)
{
	VARIANT var;

	VariantInit(&var);

	m_csSocket.Lock();
	VariantCopyInd(&var, &m_varCookie);
	m_csSocket.Unlock();

	return var;
}

void CBoxSocket::put_Cookie(VARIANT *pVal)
{
	m_csSocket.Lock();
	VariantCopyInd(&m_varCookie, pVal);
	m_csSocket.Unlock();
}

long CBoxSocket::get_dataAvailable(void)
{
	return m_nSize - m_nPos;
}

BSTR CBoxSocket::Resolve(LPCTSTR pstrHostAddress)
{
	CStringA str(pstrHostAddress);
	str.Trim();

	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(str);

	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(str);
		if (lphost == NULL)
			return NULL;

		sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
	}

	str.Format(_T("%d.%d.%d.%d"),
		sockAddr.sin_addr.S_un.S_un_b.s_b1,
		sockAddr.sin_addr.S_un.S_un_b.s_b2,
		sockAddr.sin_addr.S_un.S_un_b.s_b3,
		sockAddr.sin_addr.S_un.S_un_b.s_b4);

	return str.AllocSysString();
}

long CBoxSocket::Bind(LPCTSTR pstrSocketAddress, UINT nSocketPort)
{
	CStringA str(pstrSocketAddress);
	str.Trim();

	if (!str.IsEmpty())
	{
		m_ulBindIP = inet_addr(str);
		if (m_ulBindIP == INADDR_NONE)
			return SOCKET_ERROR;
	}

	m_usBindPort = (u_short)nSocketPort;
	return 0;
}

long CBoxSocket::Connect(LPCTSTR pstrHostAddress, UINT nHostPort)
{
	Close();

	m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(m_hSocket == INVALID_SOCKET)
		return SOCKET_ERROR;

	BOOL bKeepAlive = TRUE;
	setsockopt(m_hSocket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&bKeepAlive, sizeof(bKeepAlive));

	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;

	if (m_ulBindIP != INADDR_NONE)
	{
		sockAddr.sin_addr.s_addr = m_ulBindIP;
		sockAddr.sin_port = htons((u_short)m_usBindPort);

		if(bind(m_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)))
			return SOCKET_ERROR;
	}

	CStringA str(pstrHostAddress);
	str.Trim();

	sockAddr.sin_addr.s_addr = inet_addr(str);

	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(str);
		if (lphost == NULL)
			return SOCKET_ERROR;

		sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
	}

	sockAddr.sin_port = htons((u_short)nHostPort);

	return connect(m_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
}

long CBoxSocket::Listen(LPCTSTR pstrSocketAddress, UINT nSocketPort)
{
	Close();

	m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(m_hSocket == INVALID_SOCKET)
		return SOCKET_ERROR;

	BOOL bKeepAlive = TRUE;
	setsockopt(m_hSocket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&bKeepAlive, sizeof(bKeepAlive));

	CStringA str(pstrSocketAddress);
	str.Trim();

	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;

	if (!str.IsEmpty())
	{
		DWORD lResult = inet_addr(str);
		if (lResult == INADDR_NONE)
			return SOCKET_ERROR;

		sockAddr.sin_addr.s_addr = lResult;
	}

	sockAddr.sin_port = htons((u_short)nSocketPort);

	if(bind(m_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)))
		return SOCKET_ERROR;

	m_mapAccept.InitHashTable(6151);

	return listen(m_hSocket, SOMAXCONN);
}

BOOL CBoxSocket::Accept(CBoxObject<CBoxSocket>& pSocket)
{
	SOCKET s;
	SOCKADDR_IN sockAddr;
	int SockLen;

	SockLen = sizeof(SOCKADDR);
	pSocket.CreateInstance();

	while(TRUE)
	{
		do
		{
			s = accept(m_hSocket, (SOCKADDR*)&sockAddr, &SockLen);
			if(s == INVALID_SOCKET)
			{
				DWORD dwError;

				dwError = WSAGetLastError();
				if(WSAEINTR == dwError || WSAENOTSOCK == dwError || WSAEINVAL == dwError)
					return FALSE;
			}
		}while(s == INVALID_SOCKET);

		BOOL bKeepAlive = TRUE;
		setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (const char*)&bKeepAlive, sizeof(bKeepAlive));

		if(m_nConnectLimit <= 0)
		{
			pSocket->Attach(s);
			pSocket->m_pListen = this;

			m_csSocket.Lock();
			m_linkAccept.AddHead(pSocket);
			m_csSocket.Unlock();
			break;
		}else
		{
			m_csSocket.Lock();

			WORD& wCount = m_mapAccept[sockAddr.sin_addr.s_addr];
			if(wCount < m_nConnectLimit)
			{
				wCount ++;

				pSocket->Attach(s);
				pSocket->m_pListen = this;
				pSocket->m_dwAcceptIP = sockAddr.sin_addr.s_addr;

				m_linkAccept.AddHead(pSocket);
				m_csSocket.Unlock();
				break;
			}

			m_csSocket.Unlock();
		}

		closesocket(s);
	}

	return TRUE;
}

LPDISPATCH CBoxSocket::AcceptDispatch(void)
{
	CBoxObject<CBoxSocket> pSocket;

	if(Accept(pSocket))
	{
		pSocket->ExternalAddRef();
		return pSocket;
	}

	return NULL;
}

LPDISPATCH CBoxSocket::SSLAcceptDispatch(void)
{
	CBoxObject<CBoxSocket> pSocket;

	while(Accept(pSocket))
	{
		if(pSocket->SSLAccept(theApp.getSSL_CTX()) != 0)
		{
			pSocket.Release();
			continue;
		}

		pSocket->ExternalAddRef();
		return pSocket;
	}

	return NULL;
}

long CBoxSocket::SSLConnect(void)
{
	if(m_hSocket == INVALID_SOCKET)
		return SOCKET_ERROR;

	if(m_SSLConnect != NULL)
		return 0;

	m_SSLConnect.create(theApp.getSSL_CTX());
	if(m_SSLConnect == NULL)
		return SOCKET_ERROR;

	if(m_SSLConnect.set_fd((int)m_hSocket) != 1)
		return SOCKET_ERROR;

	if(m_SSLConnect.connect() <= 0)
		return SOCKET_ERROR;

	return 0;
}

long CBoxSocket::SSLAccept(CSSLContext& SSLContext)
{
	if(m_hSocket == INVALID_SOCKET)
		return SOCKET_ERROR;

	if(m_SSLConnect != NULL)
		return 0;

	m_SSLConnect.create(SSLContext);
	if(m_SSLConnect == NULL)
		return SOCKET_ERROR;

	if(m_SSLConnect.set_fd((int)m_hSocket) != 1)
		return SOCKET_ERROR;

	if(m_SSLConnect.accept() <= 0)
		return SOCKET_ERROR;

	return 0;
}

void CBoxSocket::SSLClose(void)
{
	m_SSLConnect.free();
}

void CBoxSocket::Clear(void)
{
	m_hSocket = INVALID_SOCKET;
	m_nPos = 0;
	m_nSize = 0;
	m_nLastLineChar = 0;
	m_bEcho = FALSE;
	m_ulBindIP = INADDR_NONE;
	m_usBindPort = 0;


	m_csSocket.Lock();

	m_SSLConnect.free();

	if(m_pListen != NULL)
	{
		m_pListen->m_csSocket.Lock();
		m_pListen->m_linkAccept.Remove(this);

		if(m_dwAcceptIP)
		{
			WORD& wCount = m_pListen->m_mapAccept[m_dwAcceptIP];
			wCount --;
			if(wCount <= 0)m_pListen->m_mapAccept.RemoveKey(m_dwAcceptIP);
		}
		m_pListen->m_csSocket.Unlock();
		m_pListen.Release();
	}

	m_dwAcceptIP = 0;

	CBoxSocket* pSocket;

	pSocket = m_linkAccept.GetHead();
	while(pSocket)
	{
		pSocket->Break();
		pSocket = (CBoxSocket*)pSocket->m_pNext;
	}

	VariantClear(&m_varCookie);

	m_csSocket.Unlock();
}

void CBoxSocket::Break()
{
	SOCKET s = m_hSocket;

	if(s != INVALID_SOCKET)
	{
		m_hSocket = INVALID_SOCKET;

		closesocket(s);
	}
}

void CBoxSocket::Close(void)
{
	SOCKET s = m_hSocket;

	if(s != INVALID_SOCKET)
	{
		m_hSocket = INVALID_SOCKET;

		shutdown(s, SD_BOTH);
		closesocket(s);
	}

	Clear();
}

BOOL CBoxSocket::get_Eof(void)
{
	if(m_hSocket == INVALID_SOCKET)
		return TRUE;

	if(m_nPos < m_nSize)
		return FALSE;

	TIMEVAL tm = {0, 0};
	fd_set fds = {1, m_hSocket};

	return select(0, &fds, NULL, NULL, &tm) < 0;
}

void CBoxSocket::put_Timeout(long tm)
{
	if(m_hSocket != INVALID_SOCKET)
	{
		setsockopt(m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&tm, sizeof(tm));
		setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tm, sizeof(tm));
	}
}

int CBoxSocket::ReadBuffer(char *pbuf, int len)
{
	int nSize = 0, nCount = 0;

	if(m_hSocket == INVALID_SOCKET)
		return SOCKET_ERROR;

	if(m_hSocket != INVALID_SOCKET && len > 0)
	{
		if(m_nLastLineChar != 0)
		{
			m_nSize = Recv(m_buf, sizeof(m_buf));
			if(m_nSize == 0 || m_nSize == SOCKET_ERROR)
			{
				Close();
				return SOCKET_ERROR;
			}
			m_nPos = 0;
		}

		if(m_nPos < m_nSize)
		{
			if(m_nLastLineChar != 0)
			{
				if(m_nLastLineChar + m_buf[m_nPos] == 23)
					m_nPos ++;
				m_nLastLineChar = 0;
			}

			nCount = m_nSize - m_nPos;
			if(nCount > len)
				nCount = len;

			CopyMemory(pbuf, m_buf + m_nPos, nCount);
			m_nPos += nCount;
			nSize += nCount;

			return nSize;
		}

		nCount = Recv(pbuf, len);
		if(nCount == 0 || nCount == SOCKET_ERROR)
		{
			Close();
			return SOCKET_ERROR;
		}
		nSize += nCount;
	}

	return nSize;
}

int CBoxSocket::ReadBlock(char *pbuf, int len)
{
	int nSize = 0, nCount = 0;

	if(m_hSocket == INVALID_SOCKET)
		return SOCKET_ERROR;

	if(m_hSocket != INVALID_SOCKET && len > 0)
	{
		if(m_nLastLineChar != 0)
		{
			m_nSize = Recv(m_buf, sizeof(m_buf));
			if(m_nSize == 0 || m_nSize == SOCKET_ERROR)
			{
				Close();
				return SOCKET_ERROR;
			}
			m_nPos = 0;
		}

		if(m_nPos < m_nSize)
		{
			if(m_nLastLineChar != 0)
			{
				if(m_nLastLineChar + m_buf[m_nPos] == 23)
					m_nPos ++;
				m_nLastLineChar = 0;
			}

			nCount = m_nSize - m_nPos;
			if(nCount > len)
				nCount = len;

			CopyMemory(pbuf, m_buf + m_nPos, nCount);
			m_nPos += nCount;
			nSize += nCount;
		}

		while(nSize < len)
		{
			nCount = Recv(pbuf + nSize, len - nSize);
			if(nCount == 0 || nCount == SOCKET_ERROR)
			{
				nCount = SOCKET_ERROR;
				Close();
				break;
			}
			nSize += nCount;
		}
	}

	return nSize;
}

VARIANT CBoxSocket::BinaryRead(long nSize)
{
	CBoxBinPtr varPtr(nSize);

	long nSize1 = ReadBlock(varPtr, nSize);
	if(nSize1 == SOCKET_ERROR)
		AfxThrowOleException(TYPE_E_IOERROR);

	return varPtr.GetVariant(nSize1);
}

int CBoxSocket::ReadString(CString &str)
{
	if(m_hSocket == INVALID_SOCKET)
		return SOCKET_ERROR;

	int nPos;
	str.Empty();

	if(m_nLastLineChar != 0)
	{
		if(!m_nSize)
		{
			m_nSize = Recv(m_buf, sizeof(m_buf));
			if(m_nSize == 0 || m_nSize == SOCKET_ERROR)
			{
				Close();
				return 0;
			}

			if(m_bEcho)
				Send(m_buf, m_nSize);

			m_nPos = 0;
		}

		if(m_nLastLineChar + m_buf[m_nPos] == 23)
			m_nPos ++;
		m_nLastLineChar = 0;
	}

	while(1)
	{
		if(m_nPos == m_nSize)
		{
			m_nSize = Recv(m_buf, sizeof(m_buf));
			if(m_nSize == 0 || m_nSize == SOCKET_ERROR)
			{
				Close();
				return 0;
			}

			if(m_bEcho)
				Send(m_buf, m_nSize);

			m_nPos = 0;
		}

		nPos = m_nPos;
		while(nPos < m_nSize && !IsLineChar(m_buf[nPos]))
			nPos ++;

		str.Append(&m_buf[m_nPos], nPos - m_nPos);

		m_nPos = nPos;

		if(m_nPos < m_nSize && IsLineChar(m_buf[m_nPos]))
		{
			m_nLastLineChar = m_buf[m_nPos ++];

			if(m_nPos < m_nSize)
			{
				if(m_nLastLineChar + m_buf[m_nPos] == 23)
					m_nPos ++;
				m_nLastLineChar = 0;
			}
			break;
		}
	}

	return 0;
}

BSTR CBoxSocket::ReadLine(void)
{
	CString str;

	if(ReadString(str) == SOCKET_ERROR)
		AfxThrowOleException(TYPE_E_IOERROR);
	return str.AllocSysString();
}

BSTR CBoxSocket::ReadPassword(void)
{
	DWORD bEcho = m_bEcho;

	m_bEcho = FALSE;
	BSTR bstr = ReadLine();

	m_bEcho = bEcho;
	if(m_bEcho)WriteBlock("\r\n", 2);

	return bstr;
}

int CBoxSocket::WriteBlock(const char *pbuf, int len)
{
	int nSize = 0, nCount = 0;

	if(m_hSocket != INVALID_SOCKET && len > 0)
	{
		while(len > 0 && nCount != SOCKET_ERROR)
		{
			nCount = Send(pbuf, len);
			if(nCount == SOCKET_ERROR)
			{
				Close();
				nSize = SOCKET_ERROR;
				break;
			}

			pbuf += nCount;
			len -= nCount;
			nSize += nCount;
		}
	}

	return nSize;
}

void CBoxSocket::BinaryWrite(VARIANT &varData)
{
	if(m_hSocket == INVALID_SOCKET)
		AfxThrowOleException(TYPE_E_IOERROR);

	CBoxBinPtr varPtr(varData);

	long nSize = WriteBlock(varPtr, varPtr.m_nSize);

	if(nSize != varPtr.m_nSize)
		AfxThrowOleException(TYPE_E_IOERROR);
}

void CBoxSocket::Write(LPCTSTR pstrText)
{
	if(m_hSocket == INVALID_SOCKET)
		return;

	LPCSTR pstr = pstrText;

	WriteBlock(pstr, (int)strlen(pstr));
}

void CBoxSocket::WriteLine(LPCTSTR pstrText)
{
	if(m_hSocket == INVALID_SOCKET)
		return;

	Write(pstrText);
	WriteBlock("\r\n", 2);
}

void CBoxSocket::WriteBlankLines(long nLine)
{
	if(m_hSocket == INVALID_SOCKET)
		return;

	if(nLine > 0)
	{
		char strNewLine[1024];

		for(int i = 0; i < 1024; i += 2)
		{
			strNewLine[i] = _T('\r');
			strNewLine[i + 1] = _T('\n');
		}

		while(nLine > 512)
		{
			WriteBlock(strNewLine, 1024);
			nLine -= 512;
		}

		if(nLine > 0)
			WriteBlock(strNewLine, nLine * 2);
	}
}
