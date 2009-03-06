// BoxTimeCache.cpp : implementation file
//

#include "stdafx.h"
#include "NetBox2.h"
#include "BoxTimeCache.h"
#include "BoxJobServer.h"
#include "ScriptHost.h"
#include "sice.h"

static CCriticalSection g_csBoxTimeCache;
static CBoxTimeCache* g_pBoxTimeCache;

// CBoxTimeCache

CBoxTimeCache::CBoxTimeCache()
{
	GetNewTime();

	m_bAutoDelete = FALSE;
	CreateThread();
}

void CBoxTimeCache::OnFinalRelease()
{
	CSingleLock l(&g_csBoxTimeCache, TRUE);
	g_pBoxTimeCache = NULL;
	l.Unlock();

	WaitForSingleObject(m_hThread, INFINITE);

	CWinThread::OnFinalRelease();
}

BOOL CBoxTimeCache::InitInstance()
{
	AfxOleInit();

	theApp.SetThreadName(_T("TimeCache"));
	CBThread th;

	while(m_dwRef != 0)
	{
		GetNewTime();
		Sleep(1);
	}

	return FALSE;
}

void CBoxTimeCache::GetNewTime(void)
{
	SYSTEMTIME st;
	__int64 ft;

	GetSystemTime(&st);
	st.wMilliseconds = 0;
	SystemTimeToFileTime(&st, (FILETIME*)&ft);

	if(m_pTime == NULL)
	{
		m_pTime.CreateInstance();
		m_pTime->m_time = ft;
		TimeToString(st, m_pTime->m_strTime);
		return;
	}

	CBoxJobServer::OnTimeIdle(ft);
	CScriptHost::CheckTimeout(ft);

	if(m_pTime->m_time != ft)
	{
		CBoxObject<CTime> pTime;

		pTime.CreateInstance();
		pTime->m_time = ft;
		TimeToString(st, pTime->m_strTime);

		CSingleLock l(&m_cs, TRUE);
		m_pTime = pTime;
	}
}

void CBoxTimeCache::GetNewTimeCache(CBoxObject<CBoxTimeCache>& pBoxTimeCache)
{
	CSingleLock l(&g_csBoxTimeCache, TRUE);

	if(g_pBoxTimeCache == NULL)
	{
		pBoxTimeCache.CreateInstance();
		g_pBoxTimeCache = pBoxTimeCache;
	}else
		pBoxTimeCache = g_pBoxTimeCache;
}

// CBoxTimeCache message handlers

