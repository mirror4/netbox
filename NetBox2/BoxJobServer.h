
#pragma once

#include "BoxTimeCache.h"
#include "BoxContents.h"
#include "BoxScriptObject.h"

class CBoxJobServer;

class CBoxJobWorker : public CWinThread,
					public CBoxLinkNode
{
public:
	virtual BOOL InitInstance();
	virtual long OnAccept(void);
	virtual void Stop(void);

	CComVariant m_varJob;

	CBoxJobServer*	m_pServer;
	CBoxObject<CBoxDictionary> m_pServerInfo;

	long OnAcceptEx(LPCOLESTR lpszMethod);

protected:
	CBoxObject<CBoxScriptObject> m_pScriptObject;
	CComDispatchDriver m_pAcceptDisp;
};

// CBoxJobServer command target

class CBoxJobServer : public CBoxContents,
					public CBoxLinkNode
{
	DECLARE_DYNCREATE(CBoxJobServer)

public:
	CBoxJobServer();
	virtual ~CBoxJobServer();

	static void OnTimeIdle(__int64 nNowTime);
	virtual void OnSleepTimer(__int64 nNowTime);

	virtual CBoxJobWorker* CreateNewWorker(void);
	virtual BOOL OnJobStart(VARIANT& var);
	virtual void OnJobEnd(VARIANT& var, long retVal);

	long m_bWorkerInited;

public:
	afx_msg void Create(LPCTSTR pstrClass, VARIANT* varCount);
	afx_msg void Add(VARIANT& var);
	afx_msg void Clear(void);
	afx_msg void Stop(void);
	afx_msg void Close(void);
	afx_msg long get_Workers(void);
	afx_msg long get_idleWorkers(void);
	afx_msg long get_queueWorks(void);
	afx_msg long get_totalWorks(void);
	DECLARE_DISPATCH_MAP()

protected:
	CString m_strAcceptClass;

private:
	friend CBoxJobWorker;

	long m_nTimeout;
	CCriticalSection m_csQueue;
	HANDLE m_hLockRead;
	HANDLE m_hLockWrite;
	long m_nQueueThread;
	long m_nThreadCount;
	long m_nMaxWorkers;
	long m_ntotalWorks;
	BOOL m_bAutoStop;
	CList<CComVariant> m_JobQueue;

	CString m_strError;

	CBoxLink<CBoxJobWorker> m_linkActive;
	CCriticalSection m_csList;

	CBoxTimeCache::CCache m_time;

	BOOL _put(VARIANT& v, DWORD dwMilliseconds = INFINITE);
	BOOL _get(CComVariant& v, DWORD dwMilliseconds = INFINITE);

	void SetActive(CBoxJobWorker* pAccept, BOOL bActive = TRUE)
	{
		m_csList.Lock();

		if(bActive)m_linkActive.AddHead(pAccept);
		else m_linkActive.Remove(pAccept);

		m_csList.Unlock();
	}

	void SetThreadCount(BOOL bAdd = TRUE)
	{
		if(bAdd)InterlockedIncrement(&m_nThreadCount);
		else InterlockedDecrement(&m_nThreadCount);
	}
};

