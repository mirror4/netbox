#include "StdAfx.h"
#include "BHttpRequest.h"
#include "BVarType.h"
#include <wininet.h>
#include <atlutil.h>

HANDLE CBHttpRequest::m_hSession;
static char strUserAgent[64];
static DWORD s_dwReqID;
static CRBMap<DWORD, CBHttpRequest*> s_mapReq;
static CBCriticalSection s_cs;

CBHttpRequest::CBHttpRequest(void) :
	m_hConnection(NULL),
	m_hFile(NULL),
	m_nReadyState(0),
	m_dwDataAvailable(0),
	m_dwTotalBytes(0),
	m_dwStatus(0),
	m_eventComplete(TRUE),
	m_dwReqID(0),
	m_dwFlags(INTERNET_FLAG_RELOAD | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS)
{
	if(m_hSession == NULL)
	{
		DWORD dataSize;
		HKEY hKey;
		LONG result;

		dataSize = sizeof(strUserAgent);
		result = ::RegOpenKeyEx(HKEY_CURRENT_USER,
								"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
								0, KEY_QUERY_VALUE, &hKey);

		if(result == ERROR_SUCCESS)
		{
			result = ::RegQueryValueEx(hKey, "User Agent", NULL, NULL,(LPBYTE) strUserAgent, &dataSize);
			RegCloseKey(hKey);
		}

		m_hSession = InternetOpen(strUserAgent, PRE_CONFIG_INTERNET_ACCESS, NULL, NULL, INTERNET_FLAG_ASYNC);

		InternetSetStatusCallback(m_hSession, (INTERNET_STATUS_CALLBACK)&staticStatusCallback);
	}
}

CBHttpRequest::~CBHttpRequest(void)
{
	Abort();
}

void CBHttpRequest::ReadBuffer(void)
{
	m_dwRead = 0;

	m_eventComplete.Reset();

	if(InternetReadFile(m_hFile, &m_aBuffer, sizeof(m_aBuffer), &m_dwRead))
		StatusCallback(INTERNET_STATUS_REQUEST_COMPLETE);
	else if(GetLastError() != ERROR_IO_PENDING)
	{
		m_dwRead = 0;
		StatusCallback(INTERNET_STATUS_REQUEST_COMPLETE);
	}
}

void CBHttpRequest::StatusCallback(DWORD dwInternetStatus)
{
	switch(dwInternetStatus)
	{
	case INTERNET_STATUS_HANDLE_CREATED:
	case INTERNET_STATUS_RESOLVING_NAME:
		if(!m_nReadyState)
			m_nReadyState = 1;
		break;
	case INTERNET_STATUS_REQUEST_COMPLETE:
		switch(m_nReadyState)
		{
		case 1:
			m_mStream.Clear();

			m_nReadyState = 2;
			m_dwTotalBytes = atol(QueryInfo(HTTP_QUERY_CONTENT_LENGTH));
			m_dwStatus = atol(QueryInfo(HTTP_QUERY_STATUS_CODE));

			ReadBuffer();
			break;
		case 2:
			m_nReadyState = 3;
		case 3:
			InterlockedExchange((LONG*)&m_dwDataAvailable, (LONG)m_dwRead);
			if(m_dwRead == 0)m_nReadyState = 4;

			m_eventComplete.Set();
			break;
		}
		break;
	}
}

void CALLBACK CBHttpRequest::staticStatusCallback(HINTERNET hInternet, DWORD ReqID, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	CBHttpRequest* pHttp = NULL;

	s_cs.Enter();
	s_mapReq.Lookup(ReqID, pHttp);
	s_cs.Leave();

	if(pHttp)pHttp->StatusCallback(dwInternetStatus);
}

CBStringA CBHttpRequest::QueryInfo(DWORD i)
{
	CBStringA str;

	if(m_nReadyState < 2)return str;

	DWORD dwLen = 0;
	HttpQueryInfo(m_hFile, i, NULL, &dwLen, 0);
	LPSTR pstr = str.GetBufferSetLength(dwLen);

	if (HttpQueryInfo(m_hFile, i, pstr, &dwLen, NULL))
		str.ReleaseBuffer(dwLen);
	else
		str.ReleaseBuffer(0);

	return str;
}

STDMETHODIMP CBHttpRequest::Abort(void)
{
	s_cs.Enter();
	s_mapReq.RemoveKey(m_dwReqID);
	s_cs.Leave();

	if(m_hFile)
	{
		InternetCloseHandle(m_hFile);
		m_hFile = NULL;
	}

	if(m_hConnection)
	{
		InternetCloseHandle(m_hConnection);
		m_hConnection = NULL;
	}

	m_mStream.Clear();
	m_dwTotalBytes = 0;
	m_dwStatus = 0;
	m_nReadyState = 0;
	m_dwReadPos = 0;
	m_dwDataAvailable = 0;
	m_eventComplete.Reset();

	return S_OK;
}

STDMETHODIMP CBHttpRequest::get_dataAvailable(short *pVal)
{
	if(!m_eventComplete.Wait(0))*pVal = 0;
	else *pVal = (short)(m_dwDataAvailable - m_dwReadPos);
	return S_OK;
}

STDMETHODIMP CBHttpRequest::getAllResponseHeaders(BSTR *retVal)
{
	CBStringA strHeaders;
	int i;

	strHeaders = QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF);

	for(i = 0; i < strHeaders.GetLength(); i ++)
		if((BYTE)strHeaders[i] < 0x20)break;

	while(i < strHeaders.GetLength() && (BYTE)strHeaders[i] < 0x20)
		i ++;

	*retVal = strHeaders.Mid(i).AllocSysString();
	return S_OK;
}

STDMETHODIMP CBHttpRequest::getResponseHeader(BSTR strName, BSTR *retVal)
{
	if(m_nReadyState < 2)
		return S_OK;

	CBStringA str(strName);
	LPSTR pstr = str.GetBuffer();
	DWORD dwLen = str.GetLength();

	if(!HttpQueryInfo(m_hFile, HTTP_QUERY_CUSTOM, pstr, &dwLen, 0) && 
		GetLastError()==ERROR_INSUFFICIENT_BUFFER)
	{
		pstr = str.GetBufferSetLength(dwLen);
		HttpQueryInfo(m_hFile, HTTP_QUERY_CUSTOM, pstr, &dwLen, NULL);
	}

	str.ReleaseBuffer(dwLen);
	*retVal = str.AllocSysString();

	return S_OK;
}

STDMETHODIMP CBHttpRequest::Open(BSTR strMethod, BSTR strUrl, VARIANT_BOOL bAsync, VARIANT varUser, VARIANT varPassword)
{
	CUrl url;
	CStringA strObject;
	CStringA strUser;
	CStringA strPassword;

	Abort();

	s_cs.Enter();
	s_dwReqID ++;
	m_dwReqID = s_dwReqID;
	s_mapReq.SetAt(m_dwReqID, this);
	s_cs.Leave();

	url.CrackUrl(CBStringA(strUrl));
	m_bAsync = (bAsync != VARIANT_FALSE);

	strObject = url.GetUrlPath();
	strObject.Append(url.GetExtraInfo());

	if(varUser.vt != VT_ERROR)
	{
		HRESULT hr = varGetString(varUser, strUser);
		if(FAILED(hr))return hr;
	}

	if(varPassword.vt != VT_ERROR)
	{
		HRESULT hr = varGetString(varPassword, strPassword);
		if(FAILED(hr))return hr;
	}

	m_hConnection = InternetConnect(m_hSession, url.GetHostName(), url.GetPortNumber(),
			strUser.IsEmpty() ? NULL : (LPCSTR)strUser, strPassword.IsEmpty() ? NULL : (LPCSTR)strPassword,
			INTERNET_SERVICE_HTTP, 0, m_dwReqID);
	if(m_hConnection == NULL)
		return GetErrorResult();

	m_hFile = HttpOpenRequest(m_hConnection, CBStringA(strMethod), strObject, NULL, NULL, NULL, m_dwFlags, m_dwReqID);
	if(m_hFile == NULL)
		return GetErrorResult();

	m_eventComplete.Set();

	return S_OK;
}

STDMETHODIMP CBHttpRequest::get_Flags(long *flags)
{
	*flags = m_dwFlags;
	return S_OK;
}

STDMETHODIMP CBHttpRequest::put_Flags(long flags)
{
	m_dwFlags = flags;

	return S_OK;
}

STDMETHODIMP CBHttpRequest::Read(long nSize, VARIANT *retVal)
{
	DWORD dwTempSize = m_dwDataAvailable;

	if(!m_eventComplete.Wait(0))
		return SetErrorInfo(L"Response is not yet available.");

	if(m_nReadyState < 3)
		return SetErrorInfo(L"Response is not yet available.");

	if(nSize <= 0 || nSize > (long)(dwTempSize - m_dwReadPos))
		nSize = dwTempSize - m_dwReadPos;

	CBVarPtr varPtr;

	varPtr.Create(nSize);

	if(nSize)
	{
		CopyMemory(varPtr.m_pData, m_aBuffer + m_dwReadPos, nSize);
		m_dwReadPos += nSize;
		if(m_dwReadPos == m_dwDataAvailable)
		{
			m_dwDataAvailable = 0;
			m_dwReadPos = 0;

			ReadBuffer();
		}
	}

	return varPtr.GetVariant(retVal);
}

STDMETHODIMP CBHttpRequest::ReadAll(VARIANT *retVal)
{
	CBAutoPtr<BYTE> pBuffer;
	int nCount = 0;
	int nSize;

	m_eventComplete.Wait();
	if(m_nReadyState < 2)
		return SetErrorInfo(L"Response is not yet available.");

	nSize = sizeof(m_aBuffer);
	if(!pBuffer.Allocate(nSize))
		return E_OUTOFMEMORY;

	while(m_nReadyState < 4 || m_dwDataAvailable > 0)
	{
		if(nCount + (int)(m_dwDataAvailable - m_dwReadPos) > nSize)
		{
			CBAutoPtr<BYTE> pBuffer1;
			int nSize1;

			if(nSize < sizeof(m_aBuffer) * 4)nSize1 = nSize + sizeof(m_aBuffer);
			else nSize1 = nSize + nSize / 4;

			if(!pBuffer1.Allocate(nSize1))
				return E_OUTOFMEMORY;

			CopyMemory(pBuffer1, pBuffer, nSize);

			nSize = nSize1;
			pBuffer = pBuffer1;
		}

		CopyMemory(pBuffer + nCount, m_aBuffer + m_dwReadPos, m_dwDataAvailable - m_dwReadPos);
		nCount += m_dwDataAvailable - m_dwReadPos;

		m_dwDataAvailable = 0;
		m_dwReadPos = 0;

		ReadBuffer();

		m_eventComplete.Wait();
	}

	CBVarPtr varPtr;

	HRESULT hr = varPtr.Create(nCount);
	if(FAILED(hr))return hr;

	CopyMemory(varPtr.m_pData, pBuffer, nCount);
	return varPtr.GetVariant(retVal);
}

STDMETHODIMP CBHttpRequest::get_readyState(short *pVal)
{
	*pVal = m_nReadyState;

	if(*pVal == 4 && !m_eventComplete.Wait(0))
		*pVal = 3;

	return S_OK;
}

STDMETHODIMP CBHttpRequest::Send(VARIANT varBody)
{
	if(m_nReadyState != 1)
		return SetErrorInfo(L"Request not initialized.");

	m_eventComplete.Reset();

	BOOL bRet;
	HRESULT hr;

	if(varBody.vt == VT_BSTR)
	{
		CBStringA str;

		str = varBody.bstrVal;

		if(str.GetLength())
		{
			hr = m_mStream.Write((LPVOID)(LPCSTR)str, str.GetLength(), NULL);
			if(FAILED(hr))return hr;
		}
	}else if(varBody.vt != VT_ERROR)
	{
		CBVarPtr varPtr;

		HRESULT hr = varPtr.Attach(varBody);
		if(FAILED(hr))return hr;

		if(varPtr.m_nSize)
		{
			hr = m_mStream.Write(varPtr.m_pData, varPtr.m_nSize, NULL);
			if(FAILED(hr))return hr;
		}
	}

	bRet = HttpSendRequest(m_hFile, NULL, 0, m_mStream.m_pBuffer, m_mStream.m_dwCount);

	if(bRet)StatusCallback(INTERNET_STATUS_REQUEST_COMPLETE);
	else if(GetLastError() != ERROR_IO_PENDING)
	{
		m_nReadyState = 4;
		m_eventComplete.Set();

		return GetErrorResult();
	}

	if(!m_bAsync)m_eventComplete.Wait();

	return S_OK;
}

STDMETHODIMP CBHttpRequest::setRequestHeader(BSTR strHeader, BSTR strValue)
{
	CString str;

	str = strHeader;
	str.Append(": ");
	str += strValue;
	str.Append("\r\n");

	HttpAddRequestHeaders(m_hFile, str, str.GetLength(), HTTP_ADDREQ_FLAG_ADD);
	return S_OK;
}

STDMETHODIMP CBHttpRequest::get_status(short *pVal)
{
	*pVal = (short)m_dwStatus;
	return S_OK;
}

STDMETHODIMP CBHttpRequest::get_statusText(BSTR *pVal)
{
	*pVal = QueryInfo(HTTP_QUERY_STATUS_TEXT).AllocSysString();
	return S_OK;
}

STDMETHODIMP CBHttpRequest::get_TotalBytes(long *pVal)
{
	*pVal = m_dwTotalBytes;
	return S_OK;
}

STDMETHODIMP CBHttpRequest::setProxy(short ProxySetting, BSTR strProxyServer, BSTR strBypassList)
{
	CBStringA strServer(strProxyServer), strBypass(strBypassList);
	INTERNET_PROXY_INFO ipi = {ProxySetting, strServer, strBypass};

	if(!InternetSetOption(NULL, INTERNET_OPTION_PROXY, &ipi, sizeof(ipi)))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBHttpRequest::setProxyCredentials(BSTR strUserName, BSTR strPassword)
{
	CBStringA strUser(strUserName), strPass(strPassword);

	if(!InternetSetOption(m_hFile, INTERNET_OPTION_PROXY_USERNAME, (LPSTR)(LPCSTR)strUser, strUser.GetLength() + 1))
		return GetErrorResult();
	if(!InternetSetOption(m_hFile, INTERNET_OPTION_PROXY_PASSWORD, (LPSTR)(LPCSTR)strPass, strPass.GetLength() + 1))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBHttpRequest::WaitForResponse(long nTimeout, VARIANT_BOOL *retVal)
{
	*retVal = m_eventComplete.Wait(nTimeout) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CBHttpRequest::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	BYTE *ptr = (BYTE *)pv;
	int cbLeft = (int)cb;
	int n;

	m_eventComplete.Wait();
	if(m_nReadyState < 2)
		return SetErrorInfo(L"Response is not yet available.");

	while(cbLeft > 0 && (m_nReadyState < 4 || m_dwDataAvailable > 0))
	{
		n = m_dwDataAvailable - m_dwReadPos;
		if(n > cbLeft)n = cbLeft;

		CopyMemory(ptr, m_aBuffer + m_dwReadPos, n);
		ptr += n;
		cbLeft -= n;

		m_dwReadPos += n;

		if(m_dwReadPos == m_dwDataAvailable)
		{
			m_dwDataAvailable = 0;
			m_dwReadPos = 0;

			ReadBuffer();

			if(cbLeft > 0)
				m_eventComplete.Wait();
		}
	}

	if(pcbRead)
		*pcbRead = cb - cbLeft;

	return S_OK;
}

STDMETHODIMP CBHttpRequest::Write(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	if(m_nReadyState != 1)
		return SetErrorInfo(L"Request not initialized.");

	return m_mStream.Write(pv, cb, pcbWritten);
}

