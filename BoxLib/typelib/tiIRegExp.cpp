#include "StdAfx.h"
#include "ti.h"

static ELEMDESC s_mElemDesc[] = 
{
	// Pattern
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// Global
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// IgnoreCase
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Execute
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// Replace
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// Test
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Pattern", {0x00000001, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Global", {0x00000002, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 40, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"IgnoreCase", {0x00000003, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 48, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Pattern", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Global", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"IgnoreCase", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Execute", {0x00000004, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 52, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"Replace", {0x00000005, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 56, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Test", {0x00000006, NULL, &s_mElemDesc[6], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 60, 0, {{{NULL}, VT_BOOL}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IRegExp* pObject = (IRegExp*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUT(0x00000001)	//Pattern
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BSTR, 0)

		hr = pObject->put_Pattern(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000002)	//Global
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_Global(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000003)	//IgnoreCase
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_IgnoreCase(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000001)	//Pattern
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Pattern(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000002)	//Global
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_Global(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000003)	//IgnoreCase
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_IgnoreCase(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000004)	//Execute
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_DISPATCH, 1)

		hr = pObject->Execute(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000005)	//Replace
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM(VT_BSTR, 1)
		INVOKE_PARAM_RET(VT_BSTR, 2)

		hr = pObject->Replace(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000006)	//Test
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BOOL, 1)

		hr = pObject->Test(v0, v1);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IRegExp>::g_typeinfo(__uuidof(IRegExp), L"IRegExp", s_mData, 9, _Invoke);

