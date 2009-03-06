
#pragma once

#include <afxtempl.h>
#include "BoxStream.h"
#include "SSLConnect.h"
#include "X509Certificate.h"
// CBoxSocket command target

class CBoxSocket : public CBoxStream,
					public CBoxLinkNode
{
	DECLARE_DYNCREATE(CBoxSocket)

public:
	CBoxSocket();
	~CBoxSocket();

	void Attach(SOCKET s)
	{
		Close();
		m_hSocket = s;
	}

	SOCKET Detach(void)
	{
		SOCKET s = m_hSocket;
		Clear();
		return s;
	}

	BOOL Accept(CBoxObject<CBoxSocket>& pSocket);

	void Break();

	long SSLAccept(CSSLContext& SSLContext);

	BOOL IsEmpty(void)
	{
		if(m_hSocket == INVALID_SOCKET)
			return TRUE;

		return m_nPos == m_nSize;
	}

	BOOL IsSocket(void)
	{
		return m_hSocket != INVALID_SOCKET;
	}

	BOOL IsSSL(void)
	{
		return m_SSLConnect != NULL;
	}

	int WriteBlock(const char *pbuf, int len);
	int ReadBlock(char *pbuf, int len);
	int ReadBuffer(char *pbuf, int len);
	int ReadString(CString &str);

	int Send(const char *pbuf, int len)
	{
		if(m_SSLConnect != NULL)
			return m_SSLConnect.write(pbuf, len);
		else return send(m_hSocket, pbuf, len, 0);
	}

	int Recv(char *pbuf, int len)
	{
		if(m_SSLConnect != NULL)
			return m_SSLConnect.read(pbuf, len);
		else return recv(m_hSocket, pbuf, len, 0);
	}

	BOOL BindSocket(CBoxObject<CBoxSocket>& pSocket)
	{
		if(m_hSocket == INVALID_SOCKET)
			return FALSE;

		pSocket->m_csSocket.Lock();

		if(pSocket->m_pListen != NULL)
		{
			pSocket->m_pListen->m_csSocket.Lock();
			pSocket->m_pListen->m_linkAccept.Remove(pSocket);

			if(pSocket->m_dwAcceptIP)
			{
				WORD& wCount = pSocket->m_pListen->m_mapAccept[pSocket->m_dwAcceptIP];
				wCount --;
				if(wCount <= 0)pSocket->m_pListen->m_mapAccept.RemoveKey(pSocket->m_dwAcceptIP);
			}
			pSocket->m_pListen->m_csSocket.Unlock();
			pSocket->m_pListen.Release();
		}

		pSocket->m_pListen = this;

		m_csSocket.Lock();
		m_linkAccept.AddHead(pSocket);
		m_csSocket.Unlock();

		pSocket->m_csSocket.Unlock();

		return TRUE;
	}

public:
	virtual HRESULT StreamRead(void *pv, ULONG cb, ULONG *pcbRead);
	virtual HRESULT StreamWrite(const void *pv, ULONG cb, ULONG *pcbWritten);

public:
	BOOL m_bEcho;

	afx_msg void Close(void);
	afx_msg long Connect(LPCTSTR pstrHostAddress, UINT nHostPort);
	afx_msg long Listen(LPCTSTR pstrSocketAddress, UINT nSocketPort);
	afx_msg LPDISPATCH AcceptDispatch(void);

	afx_msg LPDISPATCH SSLAcceptDispatch(void);
	afx_msg long SSLConnect(void);
	afx_msg void SSLClose(void);

	afx_msg BSTR get_LocalAddress(void);
	afx_msg long get_LocalPort(void);

	afx_msg BSTR get_RemoteAddress(void);
	afx_msg long get_RemotePort(void);

	afx_msg VARIANT get_Cookie(void);
	afx_msg void put_Cookie(VARIANT *pVal);

	afx_msg long get_dataAvailable(void);

	afx_msg BSTR Resolve(LPCTSTR pstrHostAddress);

	afx_msg long get_Handle(void);

	afx_msg BOOL get_Eof(void);
	afx_msg void put_Timeout(long tm);

	afx_msg VARIANT BinaryRead(long nSize);
	afx_msg BSTR ReadLine(void);
	afx_msg BSTR ReadPassword(void);

	afx_msg void BinaryWrite(VARIANT &varData);
	afx_msg void Write(LPCTSTR pstrText);
	afx_msg void WriteLine(LPCTSTR pstrText);
	afx_msg void WriteBlankLines(long nLine);

	DECLARE_DISPATCH_MAP()

public:
	void Clear(void);

	void *m_pContext;
	SOCKET m_hSocket;

	char m_buf[4096];
	int m_nPos, m_nSize;
	char m_nLastLineChar;
	OVERLAPPED m_pAsyn;

	CSSLConnect m_SSLConnect;
	CX509Certificate m_crtPeer;

	CBoxObject<CBoxSocket> m_pListen;
	CBoxLink<CBoxSocket> m_linkAccept;
	CMap<DWORD, DWORD, WORD, WORD> m_mapAccept;
	long m_nConnectLimit;
	DWORD m_dwAcceptIP;

	VARIANT m_varCookie;

	CCriticalSection m_csSocket;
};

