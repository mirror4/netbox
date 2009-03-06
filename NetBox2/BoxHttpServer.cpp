#include "StdAfx.h"
#include "NetBox2.h"
#include "BoxHttpServer.h"
#include "BoxHttpHost.h"
#include "BoxHttpAccept.h"
#include <BPath.h>

IMPLEMENT_DYNCREATE(CBoxHttpServer, CBoxTcpServer)

CBoxHttpServer::CBoxHttpServer(void) :
		m_nLastCheckTime(0),
		m_pSendThread(NULL),
		m_nDownloadWorkers(0),
		m_ntotalBytes(0),
		m_nMaxRequestEntity(0)
{
	m_pContents->SetReadOnly(TRUE);

	if(IS_WINNT)
	{
		m_pSendThread = AfxBeginThread(staticSendThread, this, THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED);
		m_pSendThread->ResumeThread();
	}
}

CBoxHttpServer::~CBoxHttpServer(void)
{
	Close();
}

CBoxJobWorker* CBoxHttpServer::CreateNewWorker(void)
{
	return new CBoxHttpAccept();
}

void CBoxHttpServer::OnSleepTimer(__int64 nNowTime)
{
    CBoxTcpServer::OnSleepTimer(nNowTime);

	if(m_nLastCheckTime != nNowTime)
	{
		m_nLastCheckTime = nNowTime;

		CBoxObject<CBoxHttpHost> pHost;
		CComVariant var;
		int i;
		int count = m_pContents->get_Count();

		for(i = 0; i < count; i ++)
		{
			var.Clear();
			m_pContents->GetValue(i, var);
			if(var.vt == VT_DISPATCH)
			{
				pHost = var.pdispVal;
				if(pHost != NULL)
					pHost->ClearSession(m_nLastCheckTime);
			}
		}
	}
}

BEGIN_DISPATCH_MAP(CBoxHttpServer, CBoxTcpServer)
	DISP_FUNCTION(CBoxHttpServer, "Create", Create, VT_I4, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CBoxHttpServer, "Close", Close, VT_EMPTY, VTS_NONE)

	DISP_FUNCTION(CBoxHttpServer, "AddHost", AddHost, VT_DISPATCH, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CBoxHttpServer, "AttachHost", AttachHost, VT_EMPTY, VTS_BSTR VTS_DISPATCH)

	DISP_FUNCTION(CBoxHttpServer, "Start", Start, VT_EMPTY, VTS_NONE)

	DISP_PROPERTY_EX(CBoxHttpServer, "DownloadWorkers", get_DownloadWorkers, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CBoxHttpServer, "totalBytes", get_totalBytes, SetNotSupported, VT_I4)
	DISP_PROPERTY(CBoxHttpServer, "MaxRequestEntity", m_nMaxRequestEntity, VT_I4)
END_DISPATCH_MAP()

long CBoxHttpServer::get_DownloadWorkers(void)
{
	return m_nDownloadWorkers;
}

long CBoxHttpServer::get_totalBytes(void)
{
	return m_ntotalBytes;
}

VOID CALLBACK CBoxHttpServer::IoCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	CSendFile* pSend = (CSendFile*)lpOverlapped;

	if(!dwErrorCode)
	{
		pSend->m_nStart += dwNumberOfBytesTransfered;
		pSend->m_nLen -= dwNumberOfBytesTransfered;

		pSend->m_nPos += dwNumberOfBytesTransfered;

		InterlockedExchangeAdd(&((CBoxHttpServer*)pSend->m_pSocket->m_pContext)->m_ntotalBytes, dwNumberOfBytesTransfered);

		if(pSend->m_nLen)
			SendFile((ULONG_PTR)pSend);
		else
		{
			CComVariant var;

			var = (LPDISPATCH)pSend->m_pSocket->GetInterface(&IID_IDispatch);;

			((CBoxHttpServer*)pSend->m_pSocket->m_pContext)->OnJobEnd(var, pSend->m_retVal);

			delete pSend;
		}
	}else
	{
		pSend->m_pSocket->Break();
		delete pSend;
	}
}

void CALLBACK CBoxHttpServer::SendFile(ULONG_PTR dwParam)
{
	CSendFile* pSend = (CSendFile*)dwParam;
	BOOL bRet = 0;

	if(pSend->m_pBuf == NULL)
	{
		if(pSend->m_nPos == pSend->m_nSize)
		{
			long l = sizeof(pSend->buf);

			if(l > pSend->m_nLen)
				l = pSend->m_nLen;

			pSend->m_nPos = 0;
			pSend->m_nSize = pSend->m_pFile->Read(pSend->buf, l);
		}

		if(pSend->m_nSize)
			bRet = WriteFileEx((HANDLE)pSend->m_pSocket->m_hSocket, pSend->buf + pSend->m_nPos,
				pSend->m_nSize - pSend->m_nPos, pSend, IoCompletionRoutine);
	}else
		bRet = WriteFileEx((HANDLE)pSend->m_pSocket->m_hSocket, pSend->m_pBuf + pSend->m_nStart,
				pSend->m_nLen, pSend, IoCompletionRoutine);

	if(!bRet)
	{
		pSend->m_pSocket->Break();
		delete pSend;
	}
}

void CBoxHttpServer::SendFile(CBoxSocket* pSocket, CFile* pFile, long nStart, long nLen, long retVal)
{
	CSendFile* pSend = new CSendFile;

	ZeroMemory(pSend, sizeof(CSendFile));

	pSend->m_pSocket = pSocket;
	pSend->m_pFile = pFile;
	pSend->m_nStart = nStart;
	pSend->m_nLen = nLen;
	pSend->m_retVal = retVal;

	if(pFile->IsKindOf(RUNTIME_CLASS(CMemFile)))
	{
		char *bufptr;

		pFile->GetBufferPtr(CFile::bufferRead, 0, (void**)&pSend->m_pBuf, (void**)&bufptr);
	}else
		pFile->Seek(nStart, CFile::begin);

	QueueUserAPC(SendFile, m_pSendThread->m_hThread, (ULONG_PTR)pSend);
}

UINT CBoxHttpServer::staticSendThread(void* p)
{
	theApp.SetThreadName(_T("HTTPSender"));
	CBThread th;

	((CBoxHttpServer*)p)->SendThread();
	return 0;
}

void CBoxHttpServer::SendThread()
{
	while(m_pSendThread)SleepEx(INFINITE, TRUE);
}

void CALLBACK CBoxHttpServer::StopSendThread(ULONG_PTR dwParam)
{
	InterlockedExchangePointer((void**)&((CBoxHttpServer*)dwParam)->m_pSendThread, NULL);
}

long CBoxHttpServer::Create(LPCTSTR pstrSocketAddress, UINT nSocketPort)
{
	return CBoxTcpServer::Create(pstrSocketAddress, nSocketPort, NULL);
}

void CBoxHttpServer::Close(void)
{
	CBoxTcpServer::Close();

	if(m_pSendThread)
	{
		QueueUserAPC(StopSendThread, m_pSendThread->m_hThread, (ULONG_PTR)this);

		while(m_pSendThread)
			Sleep(1);
	}

	CBoxObject<CBoxHttpHost> pHost;
	CComVariant var;
	int i;
	int count = m_pContents->get_Count();

	for(i = 0; i < count; i ++)
	{
		var.Clear();
		m_pContents->GetValue(i, var);
		if(var.vt == VT_DISPATCH)
		{
			pHost = var.pdispVal;
			if(pHost != NULL)
				pHost->OnEnd();
		}
	}
}

void CBoxHttpServer::Start(void)
{
	CBoxObject<CBoxHttpHost> pHost;
	CComVariant var;
	int i;
	int count = m_pContents->get_Count();

	for(i = 0; i < count; i ++)
	{
		var.Clear();
		m_pContents->GetValue(i, var);
		if(var.vt == VT_DISPATCH)
		{
			pHost = var.pdispVal;
			if(pHost != NULL)
				pHost->OnStart();
		}
	}

	CBoxTcpServer::Start();
}

LPDISPATCH CBoxHttpServer::AddHost(LPCTSTR pstrName, LPCTSTR pstrRoot)
{
	CBPath path;
	CBoxObject<CBoxHttpHost> pHost;

	pHost.CreateInstance();

	path = pstrRoot;
	path.RemoveBackslash();
	pHost->InitHost(CString(path));

	m_pContents->SetValue(pstrName, (LPDISPATCH)pHost);
	pHost->ExternalAddRef();
	return pHost;
}

void CBoxHttpServer::AttachHost(LPCTSTR pstrName, LPDISPATCH pdispHost)
{
	CBoxObject<CBoxHttpHost> pHost;

	pHost = pdispHost;

	if(pHost == NULL || !pHost->IsKindOf(RUNTIME_CLASS(CBoxHttpHost)))
		AfxThrowOleException(TYPE_E_TYPEMISMATCH);

	m_pContents->SetValue(pstrName, (LPDISPATCH)pHost);
}
