#include "StdAfx.h"
#include "BDate.h"

static CBComPtr<CBDateEx> s_pNow;
static CBComPtr<CBDateEx> s_pNowFast;
static HANDLE s_hThread;
static BOOL s_bExit;
static SYSTEMTIME s_st;

static DWORD WINAPI timeProc(void *p)
{
	SYSTEMTIME st;

	while(s_bExit)
	{
		::Sleep(1);

		GetSystemTime(&st);
		CBComPtr<CBDateEx> pNow;

		pNow.CreateInstance();
		pNow->m_date = st;
		s_pNowFast = pNow;

		st.wMilliseconds = 0;

		if(st.wSecond != s_st.wSecond)
		{
			s_st = st;
			pNow.CreateInstance();
			pNow->m_date = st;
			pNow->m_strGMT = pNow->m_date;
			s_pNow = pNow;
		}
	}

	return 0;
}

void CBDateEx::Startup()
{
	if(!s_hThread)
	{
		DWORD dwTheradId;

		GetSystemTime(&s_st);

		s_pNowFast.CreateInstance();
		s_pNowFast->m_date = s_st;

		s_st.wMilliseconds = 0;
		s_pNow.CreateInstance();
		s_pNow->m_date = s_st;
		s_pNow->m_strGMT = s_pNow->m_date;

		s_hThread = ::CreateThread(NULL, 0, timeProc, 0, 0, &dwTheradId);
	}
}

void CBDateEx::Cleanup()
{
	if(s_hThread)
	{
		s_bExit = TRUE;
		::WaitForSingleObject(s_hThread, INFINITE);
		::CloseHandle(s_hThread);
		s_hThread = NULL;
		s_pNow.Release();
	}
}

void CBDateEx::GetDate(CBDateEx** pDate)
{
	*(CBComPtr<CBDateEx>*)pDate = s_pNow;
}

void CBDateEx::GetFastDate(CBDateEx** pDate)
{
	*(CBComPtr<CBDateEx>*)pDate = s_pNowFast;
}

