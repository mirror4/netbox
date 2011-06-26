#include "StdAfx.h"
#include ".\bhtmlwindow.h"

static CAtlArray<CBString> s_TrustedSites;

_ATL_FUNC_INFO CBHtmlWindow::StatusTextChangeInfo = {CC_STDCALL, VT_EMPTY, 1, {VT_BSTR}};
_ATL_FUNC_INFO CBHtmlWindow::DownloadBeginInfo = {CC_STDCALL, VT_EMPTY, 0, {NULL}};
_ATL_FUNC_INFO CBHtmlWindow::NewWindow2Info = {CC_STDCALL, VT_EMPTY, 2, {VT_BYREF|VT_BOOL,VT_BYREF|VT_DISPATCH}}; 
_ATL_FUNC_INFO CBHtmlWindow::CommandStateChangeInfo = {CC_STDCALL, VT_EMPTY, 2, {VT_I4,VT_BOOL}};
_ATL_FUNC_INFO CBHtmlWindow::BeforeNavigate2Info = {CC_STDCALL, VT_EMPTY, 7, {VT_DISPATCH,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_BOOL}};
_ATL_FUNC_INFO CBHtmlWindow::ProgressChangeInfo = {CC_STDCALL, VT_EMPTY, 2, {VT_I4,VT_I4}};
_ATL_FUNC_INFO CBHtmlWindow::NavigateComplete2Info = {CC_STDCALL, VT_EMPTY, 2, {VT_DISPATCH, VT_BYREF|VT_VARIANT}};
_ATL_FUNC_INFO CBHtmlWindow::OnVisibleInfo = {CC_STDCALL, VT_EMPTY, 1, {VT_BOOL}};
_ATL_FUNC_INFO CBHtmlWindow::NavigateErrorInfo = {CC_STDCALL, VT_EMPTY, 5, {VT_DISPATCH,VT_VARIANT,VT_VARIANT,VT_VARIANT,VT_BYREF|VT_BOOL}};

class CBHtmlWindowHost : public CBClass<IServiceProvider>,
						public IInternetSecurityManager
{
	INTERFACE_BEGIN_EX(CBClass<IServiceProvider>)
		INTERFACE_DEF(IInternetSecurityManager)
	INTERFACE_END

public:
    //	IInternetSecurityManager
	STDMETHOD(SetSecuritySite)(IInternetSecurityMgrSite*)
	{
		return INET_E_DEFAULT_ACTION;
	}

	STDMETHOD(GetSecuritySite)(IInternetSecurityMgrSite **ppSite)
	{
		*ppSite = NULL;
		return INET_E_DEFAULT_ACTION;
	}

	STDMETHOD(MapUrlToZone)(LPCWSTR pcwUrl, DWORD *pdwZone, DWORD dwFlag)
	{
/*		CStringW NetBoxProt("netbox://");

		if (wcsncmp(pcwUrl, NetBoxProt, NetBoxProt.GetLength()) == 0)
		{
			*pdwZone = URLZONE_LOCAL_MACHINE;
			return S_OK;
		}
*/		
		return INET_E_DEFAULT_ACTION;
	}

	STDMETHOD(GetSecurityId)(LPCWSTR,BYTE*,DWORD*,DWORD)
	{
		return INET_E_DEFAULT_ACTION;
	}

	STDMETHOD(ProcessUrlAction)(LPCWSTR pwszUrl, DWORD dwAction, BYTE* pPolicy, DWORD cbPolicy,
				BYTE* pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved)
	{
		for (UINT i=0;i<s_TrustedSites.GetCount();i++)
		{
			if (!_wcsnicmp(pwszUrl, s_TrustedSites[i], s_TrustedSites[i].GetLength()))
			{
				DWORD dwPolicy = URLPOLICY_ALLOW;
				HRESULT hr = S_OK;
			    
				// Policies are DWORD values, so we need at least 4 bytes.
				if (cbPolicy < 4)
					return E_OUTOFMEMORY;

				switch(dwAction)
				{
					case URLACTION_CREDENTIALS_USE:
						dwPolicy = URLPOLICY_CREDENTIALS_SILENT_LOGON_OK;
						break;
					case URLACTION_AUTHENTICATE_CLIENT:
						dwPolicy = URLPOLICY_AUTHENTICATE_CLEARTEXT_OK;
						break;
					case URLACTION_JAVA_PERMISSIONS:
						dwPolicy = URLPOLICY_JAVA_LOW;
						break;
					case URLACTION_CHANNEL_SOFTDIST_PERMISSIONS:
						dwPolicy = URLPOLICY_CHANNEL_SOFTDIST_AUTOINSTALL;
						break;
				}

				if (dwPolicy != URLPOLICY_ALLOW)
					hr = S_FALSE;

				*(DWORD*)pPolicy = dwPolicy;

				return hr;
			}
		}
		
		return INET_E_DEFAULT_ACTION;
	}

	STDMETHOD(QueryCustomPolicy)(LPCWSTR,REFGUID,BYTE**,DWORD*,BYTE*,DWORD,DWORD)
	{
		return INET_E_DEFAULT_ACTION;
	}

	STDMETHOD(SetZoneMapping)(DWORD,LPCWSTR,DWORD)
	{
		return INET_E_DEFAULT_ACTION;
	}

	STDMETHOD(GetZoneMappings)(DWORD,IEnumString**,DWORD)
	{
		return INET_E_DEFAULT_ACTION;
	}

public:
    //	IServiceProvider
	STDMETHOD(QueryService)(REFGUID guidService, REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		if (guidService == SID_SInternetSecurityManager && riid == IID_IInternetSecurityManager)
			return QueryInterface(riid, ppvObject);
		return E_NOINTERFACE;
	}
};

CBHtmlWindow::CBHtmlWindow(void)
{
	CBComPtr<CBHtmlWindowHost> pHost;

	pHost.CreateInstance();

	HRESULT hr;
	CBComPtr<IAxWinAmbientDispatch> pAxWinAmbient;
	CComPtr<IObjectWithSite> spObject;

	hr = QueryHost(&spObject);
	if (SUCCEEDED(hr))spObject->SetSite((IServiceProvider*)pHost);

	hr = QueryHost(&pAxWinAmbient);
	if (SUCCEEDED(hr))
		pAxWinAmbient->put_DocHostFlags(DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_FLAT_SCROLLBAR |
			DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | DOCHOSTUIFLAG_THEME);
}

LRESULT CBHtmlWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_spBrowser)
	{
		DispEventUnadvise(m_spBrowser, &DIID_DWebBrowserEvents2);
		m_spBrowser.Release();
	}

	return CBWindow::OnDestroy(uMsg, wParam, lParam, bHandled);
}

HRESULT CBHtmlWindow::get_ContextMenu(VARIANT_BOOL *pVal)
{
	if(!IsWindow())return E_HANDLE;

	HRESULT hr;
	CBComPtr<IAxWinAmbientDispatch> pAxWinAmbient;

	hr = QueryHost(__uuidof(IAxWinAmbientDispatch), (void**)&pAxWinAmbient);
	if(FAILED(hr))return hr;

	return pAxWinAmbient->get_AllowContextMenu(pVal);
}

HRESULT CBHtmlWindow::put_ContextMenu(VARIANT_BOOL newVal)
{
	if(!IsWindow())return E_HANDLE;

	HRESULT hr;
	CBComPtr<IAxWinAmbientDispatch> pAxWinAmbient;

	hr = QueryHost(__uuidof(IAxWinAmbientDispatch), (void**)&pAxWinAmbient);
	if(FAILED(hr))return hr;

	return pAxWinAmbient->put_AllowContextMenu(newVal);
}

HRESULT CBHtmlWindow::get_Argument(VARIANT *pVal)
{
	if(!IsWindow())return E_HANDLE;

	return VariantCopy(pVal, &m_varArgument);
}

HRESULT CBHtmlWindow::get_Browser(IDispatch** pVal)
{
	if(!IsWindow())return E_HANDLE;
	if(m_spBrowser == NULL)return E_POINTER;

	return m_spBrowser->QueryInterface(pVal);
}

HRESULT CBHtmlWindow::Open(BSTR strURL, VARIANT varArg)
{
	static VARIANT varEmpty;

	if(!wcschr(strURL, ':'))
		return E_INVALIDARG;

	m_varArgument = varArg;

	HRESULT hr = CreateControl(strURL);
	if(FAILED(hr))return hr;

	hr = QueryControl(IID_IWebBrowser2, (void**)&m_spBrowser);
	if(FAILED(hr))return hr;

	m_spBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);

	return DispEventAdvise(m_spBrowser, &DIID_DWebBrowserEvents2);
}

HRESULT CBHtmlWindow::OpenModalDialog(BSTR strUrl, short Width, short Height, VARIANT varArg, VARIANT *pVal)
{
	CBComPtr< CBDispatch<IHTWindow, CBWindow> > pWin;

	pWin.Attach((CBDispatch<IHTWindow, CBWindow>*)new CBHtmlWindow());

	if(Width && Height)
		pWin->ResizeTo(Width, Height);
	pWin->put_ResizAble(0);
	pWin->Center();
	pWin->put_ScrollBar(0);
	pWin->Open(strUrl, varArg);
	return pWin->ShowDialog(pVal);
}

void CBHtmlWindow::AddTrustedSite(LPCTSTR str)
{
	s_TrustedSites.Add(CBString(str));
}
