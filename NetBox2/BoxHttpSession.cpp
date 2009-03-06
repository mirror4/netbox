// BoxHttpSession.cpp : implementation file
//

#include "stdafx.h"
#include "BoxHttpHost.h"
#include "BoxHttpSession.h"
#include "BoxHttpContext.h"
#include "NetBox2.h"

// CBoxHttpSession

IMPLEMENT_DYNAMIC(CBoxHttpSession, CBoxSafeObject)

BEGIN_DISPATCH_MAP(CBoxHttpSession, CBoxSafeObject)
	DISP_PROPERTY_PARAM_ID(CBoxHttpSession, "Item", 0, get_Item, put_Item, VT_VARIANT, VTS_VARIANT)

	DISP_PROPERTY_EX(CBoxHttpSession, "Contents", get_Contents, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CBoxHttpSession, "_NewEnum", DISPID_NEWENUM, get__NewEnum, SetNotSupported, VT_UNKNOWN)

	DISP_PROPERTY_EX(CBoxHttpSession, "StaticObjects", get_StaticObjects, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CBoxHttpSession, "SessionID", get_SessionID, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CBoxHttpSession, "SessionKey", get_SessionKey, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CBoxHttpSession, "LastAccessTime", get_LastAccessTime, SetNotSupported, VT_DATE)
	DISP_PROPERTY_EX(CBoxHttpSession, "Timeout", get_Timeout, put_Timeout, VT_I4)
	DISP_FUNCTION(CBoxHttpSession, "Abandon", Abandon, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxHttpSession, "Reset", Reset, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

// CBoxHttpSession message handlers

LPDISPATCH CBoxHttpSession::get_Contents(void)
{
	m_pContents->AddRef();
	return m_pContents;
}

VARIANT CBoxHttpSession::get_Item(VARIANT& varKey)
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

void CBoxHttpSession::put_Item(VARIANT& varKey, VARIANT& var)
{
	while(varKey.vt == (VT_VARIANT | VT_BYREF))
		varKey = *varKey.pvarVal;

	while(var.vt == (VT_VARIANT | VT_BYREF))
		var = *var.pvarVal;

	HRESULT hr;

	hr = m_pContents->put_Item(varKey, var);
	if(FAILED(hr))AfxThrowOleException(hr);
}

LPUNKNOWN CBoxHttpSession::get__NewEnum(void)
{
	LPUNKNOWN pEnum = NULL;

	m_pContents->get__NewEnum(&pEnum);
	return pEnum;
}

long CBoxHttpSession::get_SessionID(void)
{
	return (long)this;
}

BSTR CBoxHttpSession::get_SessionKey(void)
{
	return m_strKey.AllocSysString();
}

DATE CBoxHttpSession::get_LastAccessTime(void)
{
	CBDate d;
	FILETIME ft;

	FileTimeToLocalFileTime((FILETIME*)&m_tmLastTime, &ft);
	d = ft;
	return d;
}

LPDISPATCH CBoxHttpSession::get_StaticObjects(void)
{
	return NULL;
}

void CBoxHttpSession::put_Timeout(long tm)
{
}

long CBoxHttpSession::get_Timeout(void)
{
	return m_nTimeout;
}

void CBoxHttpSession::Reset(void)
{
	if(!m_strNodeKey.IsEmpty())
		m_pHost->ResetSession(this);
}

void CBoxHttpSession::Abandon(void)
{
	if(!m_strNodeKey.IsEmpty())
		m_pHost->AbandonSession(this);
}
