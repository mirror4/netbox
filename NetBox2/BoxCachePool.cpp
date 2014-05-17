#include "StdAfx.h"
#include "BoxCachePool.h"
#include "NetBox2.h"
#include <BHook.h>
#include <BoxLib.h>

CBoxCachePool::CBoxCachePool(CBoxFile* pOldFile)
{
	m_pOldFile = pOldFile;
}

CBoxCachePool::~CBoxCachePool(void)
{
	if(m_pOldFile != NULL)
		delete m_pOldFile;
}

CFile* CBoxCachePool::Open(LPCTSTR pstrName)
{
	if(m_pOldFile == NULL)
		return NULL;

	CBoxObject<CBoxCacheNode> pItem;
	CFile* pOldFile;

	CSingleLock l(&m_cs, TRUE);
	if(m_cache.Lookup(pstrName, pItem))
	{
		l.Unlock();

		if(pItem->m_pBuffer == NULL)
			return BOX_FOLDER;

		return new CBoxCacheFile(pItem);
	}
	l.Unlock();

	pOldFile = m_pOldFile->Open(pstrName);
	if(pOldFile == NULL)
		return pOldFile;

	if(pOldFile == BOX_FOLDER)
	{
		pItem.CreateInstance();
		if(pItem == NULL)
			return pOldFile;

		l.Lock();
		m_cache.Add(pstrName, pItem);
		l.Unlock();

		return pOldFile;
	}

	if(!pOldFile->IsKindOf(RUNTIME_CLASS(CMemFile)))
		return pOldFile;

	pItem.CreateInstance();
	if(pItem == NULL)
		return pOldFile;

	pItem->m_nSize = (int)pOldFile->GetLength();

	pItem->m_tm = m_pOldFile->GetFileTime(pOldFile);
	m_pOldFile->GetFileTimeString(pOldFile, pItem->m_strTime);

	pItem->m_pBuffer = ((CMemFile*)pOldFile)->Detach();

	l.Lock();
	m_cache.Add(pstrName, pItem);
	l.Unlock();

	delete pOldFile;

	return new CBoxCacheFile(pItem);
}

void CBoxCachePool::SetCacheNode(LPCTSTR pstrName, CMemFile* pFile, SYSTEMTIME tm)
{
	CBoxObject<CBoxCacheNode> pItem;

	pItem.CreateInstance();

	if(pItem == NULL)
		return;

	if(pFile)
	{
		pItem->m_nSize = (int)pFile->GetLength();

		pItem->m_tm = tm;
		TimeToString(tm, pItem->m_strTime);

		pItem->m_pBuffer = pFile->Detach();
	}

	CSingleLock l(&m_cs, TRUE);
	m_cache.Add(pstrName, pItem);
	l.Unlock();
}

SYSTEMTIME CBoxCachePool::GetFileTime(CFile* pFile)
{
	if(pFile->IsKindOf(RUNTIME_CLASS(CMemFile)))
		return ((CBoxCacheFile*)pFile)->GetFileTime();

	if(m_pOldFile != NULL)
		return m_pOldFile->GetFileTime(pFile);

	SYSTEMTIME tm = {0};
	return tm;
}

void CBoxCachePool::GetFileTimeString(CFile* pFile, CString& strTime)
{
	if(pFile->IsKindOf(RUNTIME_CLASS(CMemFile)))
		return ((CBoxCacheFile*)pFile)->GetFileTimeString(strTime);

	if(m_pOldFile != NULL)
		m_pOldFile->GetFileTimeString(pFile, strTime);
}

static HANDLE (__stdcall *RealCreateFileA)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE) = CreateFileA;

static __declspec(thread) BOOL th_bInHook;

void CBoxCachePool::CheckOpenFile(LPCTSTR pstrFile, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwFlagsAndAttributes, HANDLE& hd)
{
	if(th_bInHook)return;

	th_bInHook = TRUE;

	CBAutoPtr<TCHAR, 1024> buffer;
	LPTSTR p;
	DWORD dwSize;

	if(!(dwSize = GetFullPathName(pstrFile, _MAX_PATH, buffer, &p)) ||
		_tcsnicmp(buffer, g_pFile->m_strBasePath, g_pFile->m_strBasePath.GetLength()) ||
		(dwSize > 4 && IsExeFile(buffer + dwSize - 4)))
	{
		th_bInHook = FALSE;
		return;
	}

	CFile* pFile = g_pFile->Open(buffer + g_pFile->m_strBasePath.GetLength() - 1);

	if(!pFile || pFile == BOX_FOLDER)
	{
		th_bInHook = FALSE;
		return;
	}

	if(!(dwDesiredAccess & GENERIC_WRITE))
	{
		CBAutoPtr<TCHAR, _MAX_PATH> szTempPath, szTempFile;

		GetTempPath(MAX_PATH, szTempPath);
		if(!GetTempFileName(szTempPath, _T("~"), 0, szTempFile))
		{
			GetSystemDirectory(szTempPath, MAX_PATH);
			GetTempFileName(szTempPath, _T("~"), 0, szTempFile);
		}

		hd = RealCreateFileA(szTempFile, dwDesiredAccess | GENERIC_WRITE, dwShareMode, lpSecurityAttributes, CREATE_ALWAYS, dwFlagsAndAttributes | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
	}else
	{
		p = buffer + g_pFile->m_strBasePath.GetLength();
		while(*p)
		{
			while(*p && *p != _T('\\'))p++;

			if(*p)
			{
				*p = 0;
				CreateDirectory(buffer, NULL);
				*p ++ = _T('\\');
			}
		}

		hd = RealCreateFileA(pstrFile, dwDesiredAccess, dwShareMode, lpSecurityAttributes, CREATE_ALWAYS, dwFlagsAndAttributes, NULL);
	}

	if(hd == INVALID_HANDLE_VALUE)
	{
		delete pFile;
		th_bInHook = FALSE;
		return;
	}

	dwSize = 0;

	while(dwSize = pFile->Read(buffer, 1024))
	{
		if(!WriteFile(hd, buffer, dwSize, &dwSize, NULL))
		{
			CloseHandle(hd);
			delete pFile;
			th_bInHook = FALSE;
			return;
		}
	}

	delete pFile;

	SetFilePointer(hd, 0, NULL, 0);

	th_bInHook = FALSE;
	return;
}

extern "C" HANDLE __stdcall NewCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	HANDLE hd = INVALID_HANDLE_VALUE;

	hd = RealCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	if(hd != INVALID_HANDLE_VALUE || theApp.m_pSystem == NULL)return hd;

	DWORD dwError = ::GetLastError();

	if(dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_PATH_NOT_FOUND)
		CBoxCachePool::CheckOpenFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwFlagsAndAttributes, hd);

	if(hd == INVALID_HANDLE_VALUE)::SetLastError(dwError);

	return hd;
}
static CBHook m_CreateFileA(CreateFileA, NewCreateFileA);

static HANDLE (__stdcall *RealCreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE) = CreateFileW;
extern "C" HANDLE __stdcall NewCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	HANDLE hd = INVALID_HANDLE_VALUE;

	hd = RealCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	if(hd != INVALID_HANDLE_VALUE || theApp.m_pSystem == NULL)return hd;

	DWORD dwError = ::GetLastError();

	if(dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_PATH_NOT_FOUND)
		CBoxCachePool::CheckOpenFile(CString(lpFileName), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwFlagsAndAttributes, hd);

	if(hd == INVALID_HANDLE_VALUE)::SetLastError(dwError);

	return hd;
}
static CBHook m_CreateFileW(CreateFileW, NewCreateFileW);

