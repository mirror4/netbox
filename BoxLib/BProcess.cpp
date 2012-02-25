#include "StdAfx.h"
#include "BProcess.h"

CBProcess::CBProcess(void) : m_hProcess(NULL)
{
	
}

CBProcess::~CBProcess(void)
{
	if (m_hProcess != NULL)
		CloseHandle(m_hProcess);
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

void CBProcess::SetHandle(HANDLE hProcess)
{
	m_hProcess = hProcess;
}