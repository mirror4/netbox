#include "StdAfx.h"
#include "BoxProtocol.h"
#include "BoxPath.h"
#include "BoxFile.h"
#include <wininet.h>

void CBoxProtocol::Clear()
{
	if(m_pFile && m_pFile != BOX_FOLDER)
		delete m_pFile;

	m_pFile = NULL;

	m_pProtSink.Release();
}

STDMETHODIMP CBoxProtocol::Start(LPCWSTR szUrl, IInternetProtocolSink *pIProtSink, IInternetBindInfo *pIBindInfo,
									 DWORD grfSTI, DWORD dwReserved)
{
	if(!szUrl || !pIProtSink) return E_POINTER;
	if(_wcsnicmp(szUrl, L"netbox:", 7))return E_POINTER;

	if (grfSTI & PI_PARSE_URL)return S_OK;

	Clear();

	CBoxPath path;
	CString str;
	int n;

	str = szUrl + 7;

	LPCTSTR pstrExt = _T(".");
	if(str.GetLength() > 4)
		pstrExt = (LPCTSTR)str + str.GetLength() - 4;

	n = str.Find("?");
	if(n != -1)str = str.Left(n);
	n = str.Find("#");
	if(n != -1)str = str.Left(n);

	path.Combine(_T("\\"), str);

	m_pProtSink = pIProtSink;

	if((m_pFile = g_pFile->Open(path.m_strPath)) && m_pFile != BOX_FOLDER)
	{
		if(m_pFile->IsKindOf(RUNTIME_CLASS(CMemFile)) && IsExeFile(pstrExt))
			return INET_E_SECURITY_PROBLEM;

		LPWSTR pwszMimeType = 0;
		if (SUCCEEDED(FindMimeFromData(NULL, szUrl, NULL, 0, NULL, 0, &pwszMimeType, 0)))
			pIProtSink->ReportProgress(BINDSTATUS_VERIFIEDMIMETYPEAVAILABLE, pwszMimeType);
		else m_pProtSink->ReportProgress(BINDSTATUS_VERIFIEDMIMETYPEAVAILABLE, L"text/html");
		if (pwszMimeType)
			CoTaskMemFree(pwszMimeType);
/*
		CString strUrl;
		TCHAR strCacheFile[MAX_PATH];
		
		strUrl = szUrl;

		if(!CreateUrlCacheEntry(strUrl, 0, pstrExt + 1, strCacheFile, 0))
			return E_POINTER;

		CFile fileCache;

		if(!fileCache.Open(strCacheFile, CFile::modeWrite))
		{
			DeleteUrlCacheEntry(strUrl);
			return E_POINTER;
		}

		char buffer[1024];
		UINT cbSize;

		while(cbSize = m_pFile->Read(buffer, sizeof(buffer)))
			fileCache.Write(buffer, cbSize);
		fileCache.Close();

		SYSTEMTIME st;
		FILETIME ft, et;

		ZeroMemory(&et, sizeof(et));
		st = g_pFile->GetFileTime(m_pFile);
		SystemTimeToFileTime(&st, &ft);

		m_pFile->SeekToBegin();

		CommitUrlCacheEntry(strUrl, strCacheFile, et, ft, NORMAL_CACHE_ENTRY, NULL, 0, pstrExt + 1, 0);

		m_pProtSink->ReportProgress(BINDSTATUS_CACHEFILENAMEAVAILABLE, CStringW(strCacheFile));
*/
		return m_pProtSink->ReportData(BSCF_FIRSTDATANOTIFICATION | BSCF_LASTDATANOTIFICATION | BSCF_DATAFULLYAVAILABLE, 1, 0);
	}else
	{
		Clear();
		return E_POINTER;
	}

	return S_OK;
}

STDMETHODIMP CBoxProtocol::Continue(PROTOCOLDATA *pProtocolData)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBoxProtocol::Abort(HRESULT hrReason, DWORD dwOptions)
{
	HRESULT hr = E_FAIL;

	if (SUCCEEDED(hrReason))hrReason = E_ABORT;

	if (m_pProtSink)hr = m_pProtSink->ReportResult(hrReason, 0, 0);
	return hr;
}

STDMETHODIMP CBoxProtocol::Terminate(DWORD dwOptions)
{
	Clear();
	return S_OK;
}

STDMETHODIMP CBoxProtocol::Suspend()
{
	return E_NOTIMPL;
}

STDMETHODIMP CBoxProtocol::Resume()
{	
	return E_NOTIMPL;
}

STDMETHODIMP CBoxProtocol::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	*pcbRead = 0;

	if(m_pFile)
		*pcbRead = m_pFile->Read(pv, cb);

	if(!*pcbRead)
	{
		if (m_pProtSink)
			m_pProtSink->ReportResult(S_OK, 0, 0);

		return S_FALSE;
	}

	return S_OK;
}

STDMETHODIMP CBoxProtocol::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBoxProtocol::LockRequest(DWORD dwOptions)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBoxProtocol::UnlockRequest()
{
	return E_NOTIMPL;
}

STDMETHODIMP CBoxProtocol::ParseUrl(LPCWSTR pwzUrl, PARSEACTION ParseAction, DWORD dwParseFlags, LPWSTR pwzResult, DWORD cchResult, DWORD *pcchResult, DWORD dwReserved)
{
	if(ParseAction == PARSE_SECURITY_DOMAIN)
	{
		wcsncpy(pwzResult, L"netbox:/", 9);
		*pcchResult = 18;
		return S_OK;
	}else if(ParseAction == PARSE_CANONICALIZE)
	{
		*pcchResult = wcslen(pwzUrl) * 2 + 2;
		wcsncpy(pwzResult, pwzUrl, *pcchResult / 2);
		return S_OK;
	}

	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CBoxProtocol::CombineUrl(LPCWSTR pwzBaseUrl, LPCWSTR pwzRelativeUrl, DWORD dwCombineFlags, LPWSTR pwzResult, DWORD cchResult, DWORD *pcchResult, DWORD dwReserved)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CBoxProtocol::CompareUrl(LPCWSTR pwzUrl1, LPCWSTR pwzUrl2, DWORD dwCompareFlags)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CBoxProtocol::QueryInfo(LPCWSTR pwzUrl, QUERYOPTION QueryOption, DWORD dwQueryFlags, LPVOID pBuffer, DWORD cbBuffer, DWORD *pcbBuf, DWORD dwReserved)
{
	return INET_E_DEFAULT_ACTION;
}

