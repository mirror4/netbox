// BoxJobServer.cpp : implementation file
//

#include "stdafx.h"
#include "BoxJobServer.h"
#include <openssl\err.h>
#include "NetBox2.h"
#include "BoxPath.h"

static CCriticalSection g_csActiveJobServer;
static CBoxLink<CBoxJobServer> g_mapActiveJobServer;

// CBoxJobServer

IMPLEMENT_DYNCREATE(CBoxJobServer, CBoxContents)

BOOL CBoxJobWorker::InitInstance()
{
	HRESULT hr = E_FAIL;

	AfxOleInit();

	theApp.SetThreadName(_T("JobWorker"));
	CBThread th;

	if(!m_pServer->m_strAcceptClass.IsEmpty())
	{
		int nError;

		m_pScriptObject.CreateInstance();
		nError = m_pScriptObject->Load(m_pServer->m_strAcceptClass);
		if(nError == 0)nError = m_pScriptObject->Init();
		theApp.m_pSystem->ClearLock();

		if(m_pServer->m_nThreadCount == 1)
		{
			if(nError == 404)
				m_pServer->m_strError = _T("File not found : ") + m_pServer->m_strAcceptClass;
			else if(nError != 0)
			{
				CBoxScript::CScriptError &error = m_pScriptObject->GetErrorMessage();

				m_pServer->m_strError.Format(_T("File \"%s\", Line %d : "), error.m_strFile, error.m_nLine);
				m_pServer->m_strError += error.m_strDescription;
			}
		}

		if(nError == 0)
			m_pAcceptDisp.Attach(m_pScriptObject->get_Object());
		else m_pScriptObject.Release();
	}

	long retVal;

	InterlockedExchange(&m_pServer->m_bWorkerInited, TRUE);

	while(m_pServer->_get(m_varJob, m_pServer->m_nTimeout * 60000))
	{
		if(m_varJob.vt == VT_EMPTY)
			break;

		m_pServer->SetActive(this);
		retVal = 0;

		if(m_pServer->OnJobStart(m_varJob))
			retVal = OnAccept();

		m_pServer->OnJobEnd(m_varJob, retVal);
		m_pServer->SetActive(this, FALSE);

		m_varJob.Clear();

		InterlockedIncrement(&m_pServer->m_ntotalWorks);
	}

	m_pAcceptDisp.Release();
	m_pScriptObject.Release();

	m_pServer->SetThreadCount(FALSE);

	ERR_remove_state(0);

	return FALSE;
}

long CBoxJobWorker::OnAccept(void)
{
	long retVal = 0;

	if(m_pAcceptDisp != NULL)
	{
		CComVariant var((LPDISPATCH)m_pServer->m_pContents);
		CComVariant varRet;

		m_pAcceptDisp.Invoke2(L"OnAccept", &m_varJob, &var, &varRet);

		theApp.m_pSystem->ClearLock();

		varRet.ChangeType(VT_I4);
		retVal = varRet.lVal;
	}

	return retVal;
}

void CBoxJobWorker::Stop()
{
	if(m_pScriptObject != NULL)
		m_pScriptObject->Interrupt();

	m_pScriptObject.Release();
}

CBoxJobServer::CBoxJobServer() :
	m_hLockRead(NULL),
	m_hLockWrite(NULL),
	m_nTimeout(10),
	m_nQueueThread(0),
	m_ntotalWorks(0),
	m_nThreadCount(0),
	m_bWorkerInited(TRUE),
	m_bAutoStop(FALSE),
	m_nMaxWorkers(256)
{
}

CBoxJobServer::~CBoxJobServer()
{
	Close();
}

void CBoxJobServer::OnTimeIdle(__int64 nNowTime)
{
	CSingleLock l(&g_csActiveJobServer, TRUE);
	CBoxJobServer* pServer = g_mapActiveJobServer.GetHead();

	while(pServer != NULL)
	{
		pServer->OnSleepTimer(nNowTime);
		pServer = (CBoxJobServer*)pServer->m_pNext;
	}
}

void CBoxJobServer::OnSleepTimer(__int64 nNowTime)
{
	if(m_bWorkerInited && m_nQueueThread == 0 && m_nThreadCount < m_nMaxWorkers)
	{
		m_bWorkerInited = FALSE;

		CBoxJobWorker* pAccept = CreateNewWorker();

		pAccept->m_pServer = this;
		pAccept->m_pServerInfo = m_pContents;
		SetThreadCount();
		if(!pAccept->CreateThread())
		{
			SetThreadCount(FALSE);
			delete pAccept;
		}
	}
}

CBoxJobWorker* CBoxJobServer::CreateNewWorker(void)
{
	return new CBoxJobWorker();
}

BOOL CBoxJobServer::OnJobStart(VARIANT& var)
{
	return TRUE;
}

void CBoxJobServer::OnJobEnd(VARIANT& var, long retVal)
{
}

BEGIN_DISPATCH_MAP(CBoxJobServer, CBoxContents)
	DISP_FUNCTION(CBoxJobServer, "Create", Create, VT_EMPTY, VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CBoxJobServer, "Add", Add, VT_EMPTY, VTS_VARIANT)
	DISP_FUNCTION(CBoxJobServer, "Stop", Stop, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxJobServer, "Clear", Clear, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxJobServer, "Close", Close, VT_EMPTY, VTS_NONE)

	DISP_PROPERTY(CBoxJobServer, "Timeout", m_nTimeout, VT_I4)
	DISP_PROPERTY(CBoxJobServer, "MaxWorkers", m_nMaxWorkers, VT_I4)
	DISP_PROPERTY(CBoxJobServer, "AutoStop", m_bAutoStop, VT_BOOL)

	DISP_PROPERTY_EX(CBoxJobServer, "Workers", get_Workers, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CBoxJobServer, "idleWorkers", get_idleWorkers, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CBoxJobServer, "queueWorks", get_queueWorks, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CBoxJobServer, "totalWorks", get_totalWorks, SetNotSupported, VT_I4)
END_DISPATCH_MAP()

// CBoxJobServer message handlers

BOOL CBoxJobServer::_put(VARIANT& v, DWORD dwMilliseconds)
{
	if(WaitForSingleObject(m_hLockWrite, dwMilliseconds) != WAIT_OBJECT_0)return FALSE;

	m_csQueue.Lock();
	m_JobQueue.AddTail(v);
	m_csQueue.Unlock();

	::ReleaseSemaphore(m_hLockRead, 1, NULL);

	return TRUE;
}

BOOL CBoxJobServer::_get(CComVariant& v, DWORD dwMilliseconds)
{
	::InterlockedIncrement(&m_nQueueThread);
	if(WaitForSingleObject(m_hLockRead, dwMilliseconds) != WAIT_OBJECT_0)
	{
		::InterlockedDecrement(&m_nQueueThread);
		return FALSE;
	}
	::InterlockedDecrement(&m_nQueueThread);

	m_csQueue.Lock();
	v = m_JobQueue.RemoveHead();
	m_csQueue.Unlock();

	::ReleaseSemaphore(m_hLockWrite, 1, NULL);

	return TRUE;
}

void CBoxJobServer::Create(LPCTSTR pstrClass, VARIANT* varCount)
{
	if(m_hLockRead != NULL)
		return;

	if(pstrClass)
	{
		CScriptHost* pNowScript = CScriptHost::GetCurrentScript();
		CBoxPath path;

		if(pNowScript != NULL && pstrClass[0] != _T('\\'))
			path.Combine(pNowScript->m_strScriptName.Left(pNowScript->m_strScriptName.ReverseFind(_T('\\')) + 1), pstrClass);
		else path.Combine(pstrClass);

		m_strAcceptClass = path.m_strPath;
	}

	int nCount = 4096;

	if(varCount->vt != VT_ERROR)
	{
		CComVariant varTemp;

		varTemp.ChangeType(VT_BOOL, varCount);
		nCount = varTemp.lVal;
		if(nCount < 128)nCount = 4096;
	}

	m_hLockRead = ::CreateSemaphore(NULL, 0, nCount, NULL);
	m_hLockWrite = ::CreateSemaphore(NULL, nCount, nCount, NULL);

	g_csActiveJobServer.Lock();
	g_mapActiveJobServer.AddHead(this);
	g_csActiveJobServer.Unlock();

	while(m_nQueueThread == 0)
		Sleep(100);

	if(!m_strError.IsEmpty())
	{
		Close();
		AfxThrowOleDispatchException(0x4005, m_strError, 0);
	}
}

void CBoxJobServer::Add(VARIANT& var)
{
	if(m_hLockRead == NULL)
		return;

	while(var.vt == (VT_VARIANT | VT_BYREF))
		var = *var.pvarVal;

	_put(var);
}

void CBoxJobServer::Clear(void)
{
	if(m_hLockRead == NULL)
		return;

	CComVariant var;

	while(_get(var, 0))var.Clear();
}

void CBoxJobServer::Stop(void)
{
	if(m_hLockRead == NULL)
		return;

	CBoxJobWorker* pAccept;

	Clear();

	m_csList.Lock();
	while(pAccept = m_linkActive.GetHead())
	{
		while(pAccept != NULL)
		{
			pAccept->Stop();
			pAccept = (CBoxJobWorker*)pAccept->m_pNext;
		}

		m_csList.Unlock();
		Sleep(1);
		m_csList.Lock();
	}
	m_csList.Unlock();
}

void CBoxJobServer::Close(void)
{
	if(m_hLockRead == NULL)
		return;

	if(m_bAutoStop)Stop();

	CComVariant var;

	m_csQueue.Lock();
	while(m_JobQueue.GetCount())
	{
		m_csQueue.Unlock();
		Sleep(1);
		m_csQueue.Lock();
	}
	m_csQueue.Unlock();

	g_csActiveJobServer.Lock();
	g_mapActiveJobServer.Remove(this);
	g_csActiveJobServer.Unlock();

	while(m_nThreadCount > m_nQueueThread)
		Sleep(1);

	while(m_nThreadCount)
	{
		if(m_nQueueThread)
			_put(var, 0);

		Sleep(1);
	}

	CloseHandle(m_hLockRead);
	CloseHandle(m_hLockWrite);

	m_hLockRead = NULL;
	m_hLockWrite = NULL;

	m_csQueue.Lock();
	m_JobQueue.RemoveAll();
	m_csQueue.Unlock();
}

long CBoxJobServer::get_Workers(void)
{
	return m_nThreadCount;
}

long CBoxJobServer::get_idleWorkers(void)
{
	return m_nQueueThread;
}

long CBoxJobServer::get_queueWorks(void)
{
	m_csQueue.Lock();
	long ret = (long)m_JobQueue.GetCount();
	m_csQueue.Unlock();

	return ret;
}

long CBoxJobServer::get_totalWorks(void)
{
	return m_ntotalWorks;
}

