#pragma once

#include "BoxLib.h"
#include "BThread.h"
#include "BMStream.h"

class __declspec(uuid("94650000-0000-0000-0000-000048747470"))
CBHttpRequest : public CBDispatch<IHttpRequest>,
						public IStream
{
	DECLARE_CLASS(CBHttpRequest)

	INTERFACE_BEGIN_EX(CBDispatch<IHttpRequest>)
		STREAM_INTERFACE
	INTERFACE_END

public:
	CBHttpRequest(void);
	~CBHttpRequest(void);

public:
	// IStream
	STDMETHOD(Read)(void *pv, ULONG cb, ULONG *pcbRead);
	STDMETHOD(Write)(const void *pv, ULONG cb, ULONG *pcbWritten);


	HRESULT __stdcall Seek(LARGE_INTEGER /*dlibMove*/, DWORD /*dwOrigin*/, ULARGE_INTEGER * /*pLibNewPosition*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall SetSize(ULARGE_INTEGER /*libNewSize*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall CopyTo(IStream * /*pStream*/, ULARGE_INTEGER /*cb*/, ULARGE_INTEGER * /*pcbRead*/, ULARGE_INTEGER * /*pcbWritten*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Commit(DWORD /*grfCommitFlags*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Revert()
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall LockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall UnlockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Stat(STATSTG * /*pstatstg*/, DWORD /*grfStatFlag*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Clone(IStream ** /*ppstm*/)
	{
		return E_NOTIMPL;
	}

public:
	// IBHttpRequest
	STDMETHOD(Abort)( void);
	STDMETHOD(get_dataAvailable)(short *pVal);
	STDMETHOD(getAllResponseHeaders)(BSTR *retVal);
	STDMETHOD(getResponseHeader)(BSTR strName, BSTR *retVal);
	STDMETHOD(Open)(BSTR strMethod, BSTR strUrl, VARIANT_BOOL bAsync, VARIANT varUser, VARIANT varPassword);
	STDMETHOD(get_Flags)(long *flags);
	STDMETHOD(put_Flags)(long flags);
	STDMETHOD(Read)(long nSize, VARIANT *retVal);
	STDMETHOD(ReadAll)(VARIANT *retVal);
	STDMETHOD(get_readyState)(short *pVal);
	STDMETHOD(Send)(VARIANT varBody);
	STDMETHOD(setRequestHeader)(BSTR strHeader, BSTR strValue);
	STDMETHOD(get_status)(short *pVal);
	STDMETHOD(get_statusText)(BSTR *pVal);
	STDMETHOD(get_TotalBytes)(long *pVal);
	STDMETHOD(setProxy)(short ProxySetting, BSTR strProxyServer = L"", BSTR strBypassList = L"");
	STDMETHOD(setProxyCredentials)(BSTR strUserName, BSTR strPassword);
	STDMETHOD(WaitForResponse)(long nTimeout, VARIANT_BOOL *retVal);

private:
	static HANDLE m_hSession;
	HANDLE m_hConnection;
	HANDLE m_hFile;
	int m_nReadyState;
	BOOL m_bAsync;
	CBEvent m_eventComplete;
	BYTE m_aBuffer[1024];
	DWORD m_dwRead;
	DWORD m_dwDataAvailable;
	DWORD m_dwReadPos;
	DWORD m_dwTotalBytes;
	DWORD m_dwStatus;
	DWORD m_dwFlags;

	DWORD m_dwReqID;

	CBBlockStream m_mStream;

	CBStringA QueryInfo(DWORD i);
	void ReadBuffer(void);

	void StatusCallback(DWORD dwInternetStatus);
	static void CALLBACK staticStatusCallback(HANDLE hInternet, DWORD http, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
};

