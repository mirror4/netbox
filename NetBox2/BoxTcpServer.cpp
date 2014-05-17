// BoxTcpServer.cpp : implementation file
//

#include "stdafx.h"
#include "NetBox2.h"
#include "BoxTcpServer.h"

// CBoxTcpServer

IMPLEMENT_DYNCREATE(CBoxTcpServer, CBoxJobServer)

CBoxTcpServer::CBoxTcpServer() :
	m_bRunning(FALSE),
	m_bSSL(FALSE),
	m_nConnectTimeout(0),
	m_pListenThread(NULL),
	m_pPreReadThread(NULL)
{
	m_pSocket.CreateInstance();

	m_pListenThread = AfxBeginThread(staticListenThread, this, THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);

	if(IS_WINNT)
	{
		m_pPreReadThread = AfxBeginThread(staticPreReadThread, this, THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);
		m_pPreReadThread->ResumeThread();
	}
}

CBoxTcpServer::~CBoxTcpServer()
{
	Close();
}

BEGIN_DISPATCH_MAP(CBoxTcpServer, CBoxJobServer)
	DISP_PROPERTY_EX(CBoxTcpServer, "Running", get_Running, SetNotSupported, VT_BOOL)

	DISP_FUNCTION(CBoxTcpServer, "Create", Create, VT_I4, VTS_BSTR VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CBoxTcpServer, "Close", Close, VT_EMPTY, VTS_NONE)

	DISP_FUNCTION(CBoxTcpServer, "Start", Start, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxTcpServer, "Stop", Stop, VT_EMPTY, VTS_NONE)

	DISP_PROPERTY(CBoxTcpServer, "ConnectTimeout", m_nConnectTimeout, VT_I4)

	DISP_FUNCTION(CBoxTcpServer, "EnableSSL", EnableSSL, VT_I4, VTS_BSTR VTS_BSTR)
	DISP_PROPERTY_EX(CBoxTcpServer, "SSLSessionTimeout", get_SSLSessionTimeout, put_SSLSessionTimeout, VT_I4)

	DISP_PROPERTY_EX(CBoxJobServer, "ConnectLimit", get_ConnectLimit, put_ConnectLimit, VT_I4)

	DISP_FUNCTION(CBoxJobServer, "Add", AddSocket, VT_EMPTY, VTS_VARIANT)

	DISP_PROPERTY_EX(CBoxJobServer, "LocalPort", get_LocalPort, SetNotSupported, VT_I4)
END_DISPATCH_MAP()

// CBoxTcpServer message handlers

VOID CALLBACK CBoxTcpServer::IoCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	CBoxSocket* pSocket = (CBoxSocket*)((BYTE*)lpOverlapped - (BYTE*)&((CBoxSocket*)0)->m_pAsyn);

	if(!dwErrorCode && dwNumberOfBytesTransfered)
	{
		CComVariant var;

		pSocket->m_nSize = dwNumberOfBytesTransfered;
		pSocket->m_nPos = 0;

		var = (LPDISPATCH)pSocket->GetInterface(&IID_IDispatch);;

		((CBoxTcpServer*)pSocket->m_pContext)->Add(var);
	}else pSocket->Break();

	pSocket->ExternalRelease();
}

void CALLBACK CBoxTcpServer::PreRead(ULONG_PTR dwParam)
{
	CBoxSocket* pSocket = (CBoxSocket*)dwParam;
	if(!ReadFileEx((HANDLE)pSocket->m_hSocket, pSocket->m_buf, sizeof(pSocket->m_buf), &pSocket->m_pAsyn, IoCompletionRoutine))
	{
		pSocket->Break();
		pSocket->ExternalRelease();
	}
}

VOID CALLBACK CBoxTcpServer::SendIoCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	CSendData* pSend = (CSendData*)lpOverlapped;

	if(!dwErrorCode)
	{
		pSend->m_nStart += dwNumberOfBytesTransfered;
		pSend->m_nLen -= dwNumberOfBytesTransfered;

		if(pSend->m_nLen)
			SendData((ULONG_PTR)pSend);
		else
		{
			CComVariant var;

			var = (LPDISPATCH)pSend->m_pSocket->GetInterface(&IID_IDispatch);
			pSend->m_pSocket->m_csSocket.Lock();
			pSend->m_pSocket->m_iAsynSendStatus = 2;
			pSend->m_pSocket->m_csSocket.Unlock();

			((CBoxTcpServer*)pSend->m_pSocket->m_pContext)->Add(var);

			delete pSend;
		}
	}else
	{
		pSend->m_pSocket->Break();
		delete pSend;
	}
}

void CALLBACK CBoxTcpServer::SendData(ULONG_PTR dwParam)
{
	CSendData* pSend = (CSendData*)dwParam;
	BOOL bRet = 0;

	if(!WriteFileEx((HANDLE)pSend->m_pSocket->m_hSocket, pSend->m_pBuf + pSend->m_nStart, pSend->m_nLen, pSend, SendIoCompletionRoutine))
	{
		pSend->m_pSocket->Break();
		delete pSend;
	}
}

void CBoxTcpServer::SendData(CBoxSocket* pSocket, long retVal)
{
	pSocket->m_csSocket.Lock();
	if (pSocket->m_iAsynSendStatus!=-1)
	{
		pSocket->m_csSocket.Unlock();
		return;
	}
	pSocket->m_iAsynSendStatus = 1;//Sending
	CSendData* pSend = new CSendData;
	ZeroMemory(pSend, sizeof(CSendData));
	pSend->m_pSocket = pSocket;
	pSend->m_pBuf.Attach(pSocket->m_pAsynSendBuff);
	pSend->m_nLen = pSocket->m_iAsynSendBuff;
	pSocket->m_pAsynSendBuff = NULL;
	pSocket->m_iAsynSendBuff = 0;
	pSocket->m_csSocket.Unlock();

	pSend->m_nStart = 0;
	pSend->m_retVal = retVal;

	QueueUserAPC(SendData, m_pPreReadThread->m_hThread, (ULONG_PTR)pSend);
}

void CBoxTcpServer::OnJobEnd(VARIANT& var, long retVal)
{
	CBoxObject<CBoxSocket> pSocket;

	pSocket = var.pdispVal;

	if(pSocket->IsSocket())
		SendData(pSocket, retVal);

	if(retVal)
	{
		if(pSocket->IsSocket())
			if(pSocket->IsSSL() || !IS_WINNT || !pSocket->IsEmpty())
				Add(var);
			else
				QueueUserAPC(PreRead, m_pPreReadThread->m_hThread, (ULONG_PTR)pSocket.Detach());
	}
}

BOOL CBoxTcpServer::OnJobStart(VARIANT& var)
{
	CBoxObject<CBoxSocket> pSocket;

	pSocket = var.pdispVal;

	BOOL bSent = FALSE;
	pSocket->m_csSocket.Lock();
	if (pSocket->m_iAsynSendStatus == 2)
	{
		pSocket->m_iAsynSendStatus = 0;
		bSent = TRUE;
	}
	pSocket->m_csSocket.Unlock();
	if (bSent)
	{
		CBoxJobWorker* pJobWorker = (CBoxJobWorker*)((BYTE*)&var - (BYTE*)&((CBoxJobWorker*)0)->m_varJob);
		pJobWorker->OnAcceptEx(L"OnSent");
		return FALSE;
	}

	if(!m_bSSL || pSocket->IsSSL() || !pSocket->SSLAccept(m_SSL_CTX))
		return TRUE;

	return FALSE;
}

UINT CBoxTcpServer::staticPreReadThread(void* p)
{
	theApp.SetThreadName(_T("TCPReader"));
	CBThread th;

	((CBoxTcpServer*)p)->PreReadThread();
	return 0;
}

void CBoxTcpServer::PreReadThread()
{
	while(m_pPreReadThread)SleepEx(INFINITE, TRUE);
}

void CALLBACK CBoxTcpServer::StopPreThread(ULONG_PTR dwParam)
{
	InterlockedExchangePointer((void**)&((CBoxTcpServer*)dwParam)->m_pPreReadThread, NULL);
}

UINT CBoxTcpServer::staticListenThread(void* p)
{
	theApp.SetThreadName(_T("TCPListener"));
	CBThread th;

	((CBoxTcpServer*)p)->ListenThread();
	return 0;
}

long CBoxTcpServer::get_ConnectLimit(void)
{
	return m_pSocket->m_nConnectLimit;
}

void CBoxTcpServer::put_ConnectLimit(long nConnectLimit)
{
	m_pSocket->m_nConnectLimit = nConnectLimit;
}

void CBoxTcpServer::ListenThread()
{
	CBoxObject<CBoxSocket> pSocket;
	CComVariant var;

	while(m_pSocket->Accept(pSocket))
	{
		pSocket->m_pContext = this;

		if(m_nConnectTimeout != 0)
			pSocket->put_Timeout(m_nConnectTimeout);
		var = (LPDISPATCH)pSocket;

		if(m_bSSL || !IS_WINNT)
			Add(var);
		else
			QueueUserAPC(PreRead, m_pPreReadThread->m_hThread, (ULONG_PTR)pSocket.Detach());

		pSocket.Release();
		var.Clear();
	}

	InterlockedExchangePointer((void**)&m_pListenThread, NULL);
}

long CBoxTcpServer::Create(LPCTSTR pstrSocketAddress, UINT nSocketPort, LPCTSTR pstrAcceptClass)
{
	CComVariant var;

	CBoxJobServer::Create(pstrAcceptClass, &var);

	return m_pSocket->Listen(pstrSocketAddress, nSocketPort);
}

long CBoxTcpServer::get_LocalPort(void)
{
	return m_pSocket->get_LocalPort();
}

void CBoxTcpServer::Start(void)
{
	if(m_pListenThread && !m_bRunning)
	{
		m_bRunning = TRUE;
		m_pListenThread->ResumeThread();
	}
}

void CBoxTcpServer::Stop(void)
{
	if(m_pListenThread && m_bRunning)
	{
		m_pListenThread->SuspendThread();
		m_bRunning = FALSE;
	}
}

void CBoxTcpServer::Close(void)
{
	Start();
	m_pSocket->Break();
	m_pSocket->Close();

	while(m_pListenThread)
		Sleep(1);

	CBoxJobServer::Close();

	while(m_pSocket->m_dwRef > 1)
		Sleep(1);

	if(m_pPreReadThread)
	{
		QueueUserAPC(StopPreThread, m_pPreReadThread->m_hThread, (ULONG_PTR)this);

		while(m_pPreReadThread)
			Sleep(1);
	}
}

void CBoxTcpServer::AddSocket(VARIANT& varSocket)
{
	while(varSocket.vt == (VT_VARIANT | VT_BYREF))
		varSocket = *varSocket.pvarVal;

	if(varSocket.vt != VT_DISPATCH)
		AfxThrowOleException(DISP_E_TYPEMISMATCH);

	CBoxObject<CBoxSocket> pSocket;

	pSocket = varSocket.pdispVal;

	if(pSocket == NULL || !pSocket->IsKindOf(RUNTIME_CLASS(CBoxSocket)))
		AfxThrowOleException(TYPE_E_TYPEMISMATCH);

	if(!m_pSocket->BindSocket(pSocket))
		AfxThrowOleException(E_HANDLE);

	pSocket->m_pContext = this;

	if(m_nConnectTimeout != 0)
		pSocket->put_Timeout(m_nConnectTimeout);

	if(m_bSSL || pSocket->IsSSL() || !IS_WINNT || !pSocket->IsEmpty())
		Add(varSocket);
	else
		QueueUserAPC(PreRead, m_pPreReadThread->m_hThread, (ULONG_PTR)pSocket.Detach());
}

BOOL CBoxTcpServer::get_Running(void)
{
	return m_pListenThread && m_bRunning;
}

long CBoxTcpServer::EnableSSL(LPCTSTR PrivateKey, LPCTSTR pstrCertificate)
{
	if(m_SSL_CTX.use_certificate_file(BOX_CT2CA(pstrCertificate)) <= 0)
		return -1;
	if(m_SSL_CTX.use_PrivateKey_file(BOX_CT2CA(PrivateKey)) <= 0)
		return -1;

	if(!m_SSL_CTX.check_private_key())
		return -1;

	m_bSSL = TRUE;

	return 0;
}

long CBoxTcpServer::get_SSLSessionTimeout(void)
{
	return m_SSL_CTX.get_timeout();
}

void CBoxTcpServer::put_SSLSessionTimeout(long tm)
{
	m_SSL_CTX.set_timeout(tm);
}
