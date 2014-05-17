#include "StdAfx.h"
#include "BPipe.h"
#include "BVarType.h"

// IBaseStream

STDMETHODIMP CBPipe::get_lastModify(DATE *pVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBPipe::get_Position(DOUBLE *pVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBPipe::put_Position(DOUBLE newVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBPipe::get_Size(DOUBLE *pVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBPipe::put_Size(DOUBLE newVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBPipe::Open(long nSize)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBPipe::Attach(IUnknown *SrcStream)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBPipe::setEOS(void)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBPipe::Flush(void)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBPipe::get_EOS(VARIANT_BOOL *pEOS)
{
	DWORD dwAvail = 0;
	
	if (PeekNamedPipe(m_hFile, NULL, 0, NULL, &dwAvail, NULL))
		*pEOS = VARIANT_FALSE;
	else
		*pEOS = VARIANT_TRUE;
	return S_OK;
}

// IPipe

STDMETHODIMP CBPipe::get_dataAvailable(long *pVal)
{
	DWORD dwAvail = 0;
	
	if (PeekNamedPipe(m_hFile, NULL, 0, NULL, &dwAvail, NULL))
		*pVal = (long)dwAvail;
	else
		*pVal = 0;
	return S_OK;
}

STDMETHODIMP CBPipe::Open(long nSize, IPipe** retVal)
{
	Close();

	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	CHandle hRd;
	if (!CreatePipe(&hRd.m_h, &m_hFile.m_h, &saAttr, nSize))
		return HRESULT_FROM_WIN32(GetLastError());
	m_dwType = STGTY_STREAM;

	CBComPtr<CBPipe> pPipe;
	pPipe.CreateInstance();
	pPipe->SetHandle(hRd.Detach(), STGTY_STREAM);
	return pPipe.QueryInterface(retVal);
}

