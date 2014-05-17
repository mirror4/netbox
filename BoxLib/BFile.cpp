#include "StdAfx.h"
#include "BFile.h"
#include "BVarType.h"

// IBaseStream
STDMETHODIMP CBFile::Attach(IUnknown *SrcStream)
{
	return E_NOTIMPL;
}

// IFileStream
HRESULT CBFile::Create(BSTR bstrName, VARIANT_BOOL bOverwrite)
{
	Close();

	if(IS_NT)
		m_hFile.m_h = ::CreateFileW(bstrName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
			bOverwrite ? CREATE_ALWAYS : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
	else
		m_hFile.m_h = ::CreateFileA(CBStringA(bstrName), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
			bOverwrite ? CREATE_ALWAYS : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_hFile.m_h = NULL;
		return GetErrorResult();
	}

	m_dwType = STGTY_STORAGE;

	return S_OK;
}

HRESULT GetBoxStream(LPCWSTR bstrFile, IStream **ppStream);
extern "C" HANDLE __stdcall NewCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
extern "C" HANDLE __stdcall NewCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

STDMETHODIMP CBFile::Open(BSTR pstrName, short nMode, short nShare)
{
	Close();

	if((pstrName[0] == L'\\' || pstrName[0] == L'/') && pstrName[1] != L'\\' && pstrName[1] != L'/')
	{
		if(nMode != modeRead)
			return E_INVALIDARG;

		return GetBoxStream(pstrName, &m_pStream);
	}

	LPCWSTR pstrTemp = wcschr(pstrName, L':');

	if(pstrTemp && pstrTemp > pstrName + 1)
	{
		if(nMode != modeRead)
			return E_INVALIDARG;

		if(!_wcsnicmp(pstrName, L"netbox:/", 8))
			return GetBoxStream(pstrName + 7, &m_pStream);

		return URLOpenBlockingStream(NULL, CString(pstrName), &m_pStream, 0, NULL); 
	}

	static DWORD modes[] = {0, GENERIC_READ, GENERIC_WRITE, GENERIC_READ | GENERIC_WRITE};

	if(IS_NT)
		m_hFile.m_h = ::NewCreateFileW(pstrName, modes[nMode & modeReadWrite], nShare, NULL,
			nMode & modeCreate ? OPEN_ALWAYS : OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	else
		m_hFile.m_h = ::NewCreateFileA(CBStringA(pstrName), modes[nMode & modeReadWrite], nShare, NULL,
			nMode & modeCreate ? OPEN_ALWAYS : OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_hFile.m_h = NULL;
		return GetErrorResult();
	}

	if(nMode & modeAppend)
		::SetFilePointer(m_hFile, 0, NULL, FILE_END);

	m_dwType = STGTY_STORAGE;

	return S_OK;
}
