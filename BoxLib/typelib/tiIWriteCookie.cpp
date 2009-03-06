#include "StdAfx.h"
#include "ti.h"

static ELEMDESC s_mElemDesc[] = 
{
	// Item
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// Expires
	{{{NULL}, VT_DATE}, {NULL, 0x1}},
	// Domain
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// Path
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// Secure
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Item", {0x00000000, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 2, 0, 28, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Expires", {0x60020001, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Domain", {0x60020002, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 36, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Path", {0x60020003, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 40, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Secure", {0x60020004, NULL, &s_mElemDesc[5], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 44, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"HasKeys", {0x60020005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"_NewEnum", {0xFFFFFFFC, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_UNKNOWN}}, 1}},
	{L"Items", {0x60020008, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 56, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Keys", {0x60020009, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 60, 0, {{{NULL}, VT_VARIANT}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IWriteCookie* pObject = (IWriteCookie*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUT(0x00000000)	//Item
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_OPT(VT_VARIANT, 0)
		INVOKE_PARAM_NAMED_VALUE(VT_BSTR, 1)

		hr = pObject->put_Item(v0, v1);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x60020001)	//Expires
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_DATE, 0)

		hr = pObject->put_Expires(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x60020002)	//Domain
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BSTR, 0)

		hr = pObject->put_Domain(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x60020003)	//Path
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BSTR, 0)

		hr = pObject->put_Path(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x60020004)	//Secure
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_Secure(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x60020005)	//HasKeys
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_HasKeys(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0xFFFFFFFC)	//_NewEnum
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_UNKNOWN, 0)

		hr = pObject->get__NewEnum(v0);
		return hr;
	}

	IF_INVOKE(0x60020008)	//Items
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Items(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE(0x60020009)	//Keys
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Keys(v0);

		INVOKE_DISP;
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IWriteCookie>::g_typeinfo(__uuidof(IWriteCookie), L"IWriteCookie", s_mData, 9, _Invoke);

