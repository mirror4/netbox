// BoxSafeObject.cpp : implementation file
//

#include "stdafx.h"


// CBoxSafeObject

IMPLEMENT_DYNAMIC(CBoxSafeObject, CCmdTarget)

BEGIN_INTERFACE_MAP(CBoxSafeObject, CCmdTarget)
	INTERFACE_PART(CBoxSafeObject, IID_IObjectSafety, ObjectSafety)
END_INTERFACE_MAP()

// CBoxSafeObject message handlers

STDMETHODIMP_(ULONG) CBoxSafeObject::XObjectSafety::AddRef()
{
	METHOD_PROLOGUE_EX_(CBoxSafeObject, ObjectSafety)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBoxSafeObject::XObjectSafety::Release()
{
	METHOD_PROLOGUE_EX_(CBoxSafeObject, ObjectSafety)
	return pThis->ExternalRelease();
}

STDMETHODIMP CBoxSafeObject::XObjectSafety::QueryInterface(REFIID iid, LPVOID far* ppvObj)
{
	METHOD_PROLOGUE_EX_(CBoxSafeObject, ObjectSafety)

	return pThis->ExternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP CBoxSafeObject::XObjectSafety::GetInterfaceSafetyOptions(REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions)
{
	if (!s_bObjectSafety)
		return E_NOINTERFACE;

	if (pdwSupportedOptions == NULL || pdwEnabledOptions == NULL)
		return E_POINTER;

	*pdwSupportedOptions = 3;
	*pdwEnabledOptions   = 1;

	return S_OK;
}

STDMETHODIMP CBoxSafeObject::XObjectSafety::SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions)
{
	if (!s_bObjectSafety)
		return E_NOINTERFACE;

	return S_OK;
}
