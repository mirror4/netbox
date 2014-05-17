// BoxHttpRequest.cpp : implementation file
//

#include "stdafx.h"
#include "BoxHttpRequest.h"
#include "BoxBinPtr.h"
#include <BVarType.h>

// CBoxHttpRequest

IMPLEMENT_DYNAMIC(CBoxHttpRequest, CBoxStream)

BEGIN_DISPATCH_MAP(CBoxHttpRequest, CBoxStream)
	DISP_PROPERTY_PARAM_ID(CBoxHttpRequest, "Item", 0, get_Item, SetNotSupported, VT_VARIANT, VTS_VARIANT)

	DISP_PROPERTY_EX(CBoxHttpRequest, "QueryString", get_QueryString, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CBoxHttpRequest, "Form", get_Form, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CBoxHttpRequest, "Cookies", get_Cookies, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CBoxHttpRequest, "ServerVariables", get_ServerVariables, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CBoxHttpRequest, "ClientCertificate", get_ClientCertificate, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CBoxHttpRequest, "Browser", get_Browser, SetNotSupported, VT_DISPATCH)

	DISP_PROPERTY_EX(CBoxHttpRequest, "TotalBytes", get_TotalBytes, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CBoxHttpRequest, "ContentType", get_ContentType, SetNotSupported, VT_BSTR)
	DISP_FUNCTION(CBoxHttpRequest, "BinaryRead", BinaryRead, VT_VARIANT, VTS_I4)

END_DISPATCH_MAP()

HRESULT CBoxHttpRequest::StreamRead(void *pv, ULONG cb, ULONG *pcbRead)
{
	ULONG lRead = m_pAccept->ReadFromClient((char *)pv, cb);
	if(pcbRead != NULL)
		*pcbRead = lRead;
	return S_OK;
}

// CBoxHttpRequest message handlers

void CBoxHttpRequest::CheckClientCertificate(void)
{
	if(m_pClientCertificate == NULL)
		m_pClientCertificate.CreateInstance();
}

LPDISPATCH CBoxHttpRequest::get_ClientCertificate(void)
{
	CheckClientCertificate();

	m_pClientCertificate->AddRef();
	return m_pClientCertificate;
}

void CBoxHttpRequest::CheckCookies(void)
{
	if(m_pCookies == NULL)
		m_pCookies = m_pAccept->GetCookie();
}

LPDISPATCH CBoxHttpRequest::get_Cookies(void)
{
	CheckCookies();

	m_pCookies->AddRef();
	return m_pCookies;
}

void CBoxHttpRequest::CheckForm(void)
{
	if(m_pForm == NULL)
	{
		m_pForm.CreateInstance();
		m_pAccept->FillForm(m_pForm);
	}
}

LPDISPATCH CBoxHttpRequest::get_Form(void)
{
	CheckForm();

	m_pForm->AddRef();
	return m_pForm;
}

LPDISPATCH CBoxHttpRequest::get_Browser(void)
{
	if(m_pBrowserCaps == NULL)
		m_pBrowserCaps.Attach(new CBBrowserCaps(m_pAccept->GetUserAgent()));

	m_pBrowserCaps->AddRef();
	return m_pBrowserCaps;
}

void CBoxHttpRequest::CheckQueryString(void)
{
	if(m_pQueryString == NULL)
	{
		m_pQueryString.CreateInstance();
		m_pAccept->FillQueryString(m_pQueryString);
	}
}

LPDISPATCH CBoxHttpRequest::get_QueryString(void)
{
	CheckQueryString();

	m_pQueryString->AddRef();
	return m_pQueryString;
}

void CBoxHttpRequest::CheckServerVariables(void)
{
	if(m_pServerVariables == NULL)
	{
		m_pServerVariables.CreateInstance();
		m_pAccept->FillServerVariables(m_pServerVariables);
	}
}

LPDISPATCH CBoxHttpRequest::get_ServerVariables(void)
{
	CheckServerVariables();

	m_pServerVariables->AddRef();
	return m_pServerVariables;
}

VARIANT CBoxHttpRequest::get_Item(VARIANT& varName)
{
	while(varName.vt == (VT_VARIANT | VT_BYREF))
		varName = *varName.pvarVal;

	VARIANT var = {0};

	CBString strKey;

	HRESULT hr = varGetString(varName, strKey);
	if(FAILED(hr))return var;

	CheckQueryString();
	if(m_pQueryString->Lookup(strKey, &var))
		return var;

	CheckForm();
	if(m_pForm->Lookup(strKey, &var))
		return var;

	CheckCookies();
	if(m_pCookies->Lookup(strKey, &var))
		return var;

	CheckServerVariables();
	m_pServerVariables->get_Item(varName, &var);

	return var;
}

long CBoxHttpRequest::get_TotalBytes(void)
{
	return m_pAccept->GetContentLength();
}

BSTR CBoxHttpRequest::get_ContentType(void)
{
	return m_pAccept->GetContentType();
}

VARIANT CBoxHttpRequest::BinaryRead(long count)
{
	CBoxBinPtr varPtr(count);

	return varPtr.GetVariant(m_pAccept->ReadFromClient(varPtr, count));
}
