#include "StdAfx.h"
#include "ti.h"

static ELEMDESC s_mElemDesc[] = 
{
	// Item
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// Key
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Exists
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Item", {0x00000000, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 1, 1, 28, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"_NewEnum", {0xFFFFFFFC, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_UNKNOWN}}, 1}},
	{L"Count", {0x60020002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Key", {0x60020003, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 1, 0, 40, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Items", {0x60020005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Keys", {0x60020006, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Exists", {0x60020004, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 44, 0, {{{NULL}, VT_BOOL}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IRequestDictionary* pObject = (IRequestDictionary*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE(0x00000000)	//Item
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_OPT(VT_VARIANT, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->get_Item(v0, v1);

		INVOKE_DISP_PUT(1);
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

	IF_INVOKE_PROPERTYGET(0x60020002)	//Count
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_Count(v0);
		return hr;
	}

	IF_INVOKE(0x60020003)	//Key
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->get_Key(v0, v1);

		INVOKE_DISP_PUT(1);
		return hr;
	}

	IF_INVOKE(0x60020005)	//Items
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Items(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE(0x60020006)	//Keys
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Keys(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_FUNC(0x60020004)	//Exists
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_RET(VT_BOOL, 1)

		hr = pObject->Exists(v0, v1);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IRequestDictionary>::g_typeinfo(__uuidof(IRequestDictionary), L"IRequestDictionary", s_mData, 7, _Invoke);

