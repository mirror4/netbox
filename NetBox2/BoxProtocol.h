#pragma once
#include "urlmon.h"
#include <BoxLib.h>

class CBoxProtocol : public CBClass<IInternetProtocol>,
					public IInternetProtocolInfo
{
	INTERFACE_BEGIN_EX(CBClass<IInternetProtocol>)
		INTERFACE_DEF_IID(IID_IInternetProtocolRoot, IInternetProtocol)
		INTERFACE_DEF(IInternetProtocolInfo)
	INTERFACE_END

public:
	CBoxProtocol(void) : m_pFile(NULL)
	{}

	~CBoxProtocol(void)
	{
		Clear();
	}

public:
	STDMETHOD(Start)(LPCWSTR szUrl, IInternetProtocolSink *pIProtSink,
            IInternetBindInfo *pIBindInfo, DWORD grfSTI, DWORD dwReserved);
    STDMETHOD(Continue)(PROTOCOLDATA *pStateInfo);
    STDMETHOD(Abort)(HRESULT hrReason,DWORD dwOptions);
    STDMETHOD(Terminate)(DWORD dwOptions);
    STDMETHOD(Suspend)();
    STDMETHOD(Resume)();
    STDMETHOD(Read)(void *pv,ULONG cb,ULONG *pcbRead);
    STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
    STDMETHOD(LockRequest)(DWORD dwOptions);
    STDMETHOD(UnlockRequest)();

public:
	STDMETHOD(ParseUrl)(LPCWSTR pwzUrl, PARSEACTION ParseAction, DWORD dwParseFlags, LPWSTR pwzResult, DWORD cchResult, DWORD *pcchResult, DWORD dwReserved);
	STDMETHOD(CombineUrl)(LPCWSTR pwzBaseUrl, LPCWSTR pwzRelativeUrl, DWORD dwCombineFlags, LPWSTR pwzResult, DWORD cchResult, DWORD *pcchResult, DWORD dwReserved);
	STDMETHOD(CompareUrl)(LPCWSTR pwzUrl1, LPCWSTR pwzUrl2, DWORD dwCompareFlags);
	STDMETHOD(QueryInfo)(LPCWSTR pwzUrl, QUERYOPTION QueryOption, DWORD dwQueryFlags, LPVOID pBuffer, DWORD cbBuffer, DWORD *pcbBuf, DWORD dwReserved);

private:
	void Clear();

	CFile*	m_pFile;
	CComPtr<IInternetProtocolSink> m_pProtSink;
};
