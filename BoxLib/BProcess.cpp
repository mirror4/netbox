#include "StdAfx.h"
#include "BProcess.h"

CBProcess::CBProcess(void) : m_hProcess(NULL)
{
	
}

CBProcess::~CBProcess(void)
{
}

STDMETHODIMP CBProcess::get_ExitCode(LONG* newVal)
{
	if (!GetExitCodeProcess(m_hProcess, (LPDWORD)newVal))
		return S_OK;
	return HRESULT_FROM_WIN32(GetLastError());
}

STDMETHODIMP CBProcess::get_ProcessID(LONG* newVal)
{
	DWORD dwRet = GetProcessId(m_hProcess);
	if (!dwRet)
		return HRESULT_FROM_WIN32(GetLastError());
	*newVal = (LONG)dwRet;
	return S_OK;
}

STDMETHODIMP CBProcess::get_Status(LONG* newVal)
{
	DWORD dwRet = WaitForSingleObject(m_hProcess, 0);
	switch (dwRet)
	{
		case WAIT_TIMEOUT:
			*newVal = 0;
			break;
		case WAIT_OBJECT_0:
			*newVal = 1;
			break;
		case WAIT_FAILED:
			return HRESULT_FROM_WIN32(GetLastError());
		default:
			return E_UNEXPECTED;
	}
	return S_OK;
}

STDMETHODIMP CBProcess::get_FileName(BSTR* newVal)
{
	CBString str;
	DWORD dwLen;

	//GetProcessImageFileName For Windows 5.1(XP 2003) later
	DWORD (WINAPI *GetModuleFileNameExW)(HANDLE,HMODULE,LPWSTR,DWORD) = NULL;
	HMODULE hmod = ::LoadLibrary("Psapi.dll");
	if(hmod == NULL)
		return HRESULT_FROM_WIN32(GetLastError());

	GetModuleFileNameExW = (DWORD (WINAPI *)(HANDLE,HMODULE,LPWSTR,DWORD))GetProcAddress(hmod, "GetModuleFileNameExW");
	if(GetModuleFileNameExW == NULL)
	{
		FreeLibrary(hmod);
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (!(dwLen = GetModuleFileNameExW(m_hProcess, NULL, str.GetBufferSetLength(MAX_PATH), MAX_PATH)))
	{
		FreeLibrary(hmod);
		str.ReleaseBuffer(0);
		return HRESULT_FROM_WIN32(GetLastError());
	}
	FreeLibrary(hmod);
	str.ReleaseBuffer(dwLen);
	*newVal = str.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBProcess::Terminate()
{
	if (!TerminateProcess(m_hProcess, 0))
		return S_OK;
	return HRESULT_FROM_WIN32(GetLastError());
}

STDMETHODIMP CBProcess::get_StdIn(IPipe** ppStream)
{
	return m_pPipe_StdIn.QueryInterface(ppStream);
}

STDMETHODIMP CBProcess::get_StdOut(IPipe** ppStream)
{
	return m_pPipe_StdOut.QueryInterface(ppStream);
}

STDMETHODIMP CBProcess::get_StdErr(IPipe** ppStream)
{
	return m_pPipe_StdErr.QueryInterface(ppStream);
}

void CBProcess::SetHandle(HANDLE hProcess, HANDLE hStdIn, HANDLE hStdOut, HANDLE hStdErr)
{
	m_hProcess.m_h = hProcess;

	if (hStdIn)
	{
		m_pPipe_StdIn.CreateInstance();
		m_pPipe_StdIn->SetHandle(hStdIn, STGTY_STREAM);
	}

	if (hStdOut)
	{
		m_pPipe_StdOut.CreateInstance();
		m_pPipe_StdOut->SetHandle(hStdOut, STGTY_STREAM);
	}

	if (hStdErr)
	{
		m_pPipe_StdErr.CreateInstance();
		m_pPipe_StdErr->SetHandle(hStdErr, STGTY_STREAM);
	}
}