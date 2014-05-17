#pragma once

#include "BoxJobServer.h"
#include "BoxSocket.h"

// CBoxTcpServer

class CBoxTcpServer : public CBoxJobServer
{
	DECLARE_DYNCREATE(CBoxTcpServer)

public:
	CBoxTcpServer();
	virtual ~CBoxTcpServer();

	virtual void OnJobEnd(VARIANT& var, long retVal);
	virtual BOOL OnJobStart(VARIANT& var);

public:
	afx_msg BOOL get_Running(void);

	afx_msg long Create(LPCTSTR pstrSocketAddress, UINT nSocketPort, LPCTSTR pstrAcceptClass);
	afx_msg void Start(void);
	afx_msg void Stop(void);
	afx_msg void Close(void);
	afx_msg void AddSocket(VARIANT& varSocket);

	afx_msg long EnableSSL(LPCTSTR PrivateKey, LPCTSTR pstrCertificate);
	afx_msg long get_SSLSessionTimeout(void);
	afx_msg void put_SSLSessionTimeout(long tm);

	afx_msg long get_ConnectLimit(void);
	afx_msg void put_ConnectLimit(long nConnectLimit);

	long get_LocalPort(void);

	DECLARE_DISPATCH_MAP()

protected:
	CBoxObject<CBoxSocket> m_pSocket;
	BOOL m_bRunning;

	BOOL m_bSSL;
	long m_nConnectTimeout;

	CSSLContext m_SSL_CTX;

private:
	static UINT staticListenThread(void* p);
	void ListenThread();

	CWinThread *m_pListenThread;

private:
	static VOID CALLBACK IoCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
	static void CALLBACK PreRead(ULONG_PTR dwParam);

	static UINT staticPreReadThread(void* p);
	void PreReadThread();
	static void CALLBACK StopPreThread(ULONG_PTR dwParam);

	CWinThread *m_pPreReadThread;

private:
	class CSendData : public OVERLAPPED
	{
	public:
		~CSendData(void)
		{
		}

		CBoxObject<CBoxSocket> m_pSocket;
		long m_nStart;
		long m_nLen;
		long m_retVal;

		CAutoPtr<char> m_pBuf;
	};

	static void CALLBACK SendData(ULONG_PTR dwParam);
	static VOID CALLBACK SendIoCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
	void SendData(CBoxSocket* pSocket, long retVal);
	long OnSent(CBoxJobWorker* pJobWorker);
};
