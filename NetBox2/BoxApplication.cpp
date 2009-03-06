// BoxApplication.cpp : implementation file
//

#include "stdafx.h"
#include "NetBox2.h"
#include "BoxApplication.h"

// CBoxApplication

IMPLEMENT_DYNAMIC(CBoxApplication, CBoxSafeObject)

BEGIN_DISPATCH_MAP(CBoxApplication, CBoxSafeObject)
	DISP_PROPERTY_PARAM_ID(CBoxApplication, "Item", 0, get_Item, put_Item, VT_VARIANT, VTS_VARIANT)

	DISP_PROPERTY_EX(CBoxApplication, "Contents", get_Contents, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CBoxApplication, "_NewEnum", DISPID_NEWENUM, get__NewEnum, SetNotSupported, VT_UNKNOWN)

	DISP_FUNCTION(CBoxApplication, "Lock", Lock, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxApplication, "Unlock", Unlock, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CBoxApplication, "ApplicationID", get_ApplicationID, SetNotSupported, VT_BSTR)
END_DISPATCH_MAP()

// CBoxApplication message handlers

LPDISPATCH CBoxApplication::get_Contents(void)
{
	m_pContents->AddRef();
	return m_pContents;
}

VARIANT CBoxApplication::get_Item(VARIANT& varKey)
{
	while(varKey.vt == (VT_VARIANT | VT_BYREF))
		varKey = *varKey.pvarVal;

	VARIANT var;
	HRESULT hr;

	VariantInit(&var);
	hr = m_pContents->get_Item(varKey, &var);
	if(FAILED(hr))AfxThrowOleException(hr);

	return var;
}

void CBoxApplication::put_Item(VARIANT& varKey, VARIANT& var)
{
	while(varKey.vt == (VT_VARIANT | VT_BYREF))
		varKey = *varKey.pvarVal;

	while(var.vt == (VT_VARIANT | VT_BYREF))
		var = *var.pvarVal;

	HRESULT hr;

	hr = m_pContents->put_Item(varKey, var);
	if(FAILED(hr))AfxThrowOleException(hr);
}

LPUNKNOWN CBoxApplication::get__NewEnum(void)
{
	LPUNKNOWN pEnum = NULL;

	m_pContents->get__NewEnum(&pEnum);
	return pEnum;
}

void CBoxApplication::Lock(void)
{
	theApp.m_pSystem->Lock(m_strHostKey);
}

void CBoxApplication::Unlock(void)
{
	theApp.m_pSystem->Unlock(m_strHostKey);
}

BSTR CBoxApplication::get_ApplicationID(void)
{
	return m_strHostKey.AllocSysString();
}