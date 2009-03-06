#include "StdAfx.h"
#include "BConsole.h"

CBConsole::CBConsole(void)
{
	m_hInput = ::GetStdHandle(STD_INPUT_HANDLE);
	m_hOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
}

HRESULT CBConsole::InternalEOS(VARIANT_BOOL *pEOS)
{
	return E_NOTIMPL;
}

HRESULT CBConsole::InternalRead(void *pv, ULONG cb, ULONG *pcbRead)
{
	if(!::ReadConsole(m_hInput, pv, cb, pcbRead, NULL))
		return GetErrorResult();

	return S_OK;
}

HRESULT CBConsole::InternalWrite(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	if(!::WriteConsole(m_hOutput, pv, cb, pcbWritten, NULL))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBConsole::get_Caption(BSTR *pVal)
{
	if(IS_NT)
	{
		CBString strBuffer;

		strBuffer.ReleaseBuffer(::GetConsoleTitleW(strBuffer.GetBuffer(1024), 1024));
		*pVal = strBuffer.AllocSysString();
	}else
	{
		CBStringA strBuffer;

		strBuffer.ReleaseBuffer(::GetConsoleTitle(strBuffer.GetBuffer(1024), 1024));
		*pVal = strBuffer.AllocSysString();
	}

	return S_OK;
}

STDMETHODIMP CBConsole::put_Caption(BSTR pVal)
{
	if(IS_NT)
		::SetConsoleTitleW(pVal);
	else
		::SetConsoleTitleA(CBStringA(pVal));

	return S_OK;
}

