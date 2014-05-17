#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_I4, 0, 0, 0, 0}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// GetLanguage
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// AddLanguage
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// RemoveLanguage
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// Load
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Save
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Count", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Languages", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"GetLanguage", {0x00000000, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 28, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"AddLanguage", {0x00000001, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 32, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"RemoveLanguage", {0x00000002, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 36, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Load", {0x00000005, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 48, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Save", {0x00000006, NULL, &s_mElemDesc[5], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 52, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IMessageManager* pObject = (IMessageManager*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYGET(0x00000003)	//Count
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_Count(v0);
		return hr;
	}

	IF_INVOKE(0x00000004)	//Languages
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Languages(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_FUNC(0x00000000)	//GetLanguage
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_DISPATCH, 1)

		hr = pObject->GetLanguage(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000001)	//AddLanguage
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_I4, 1, 0)
		INVOKE_PARAM_RET(VT_DISPATCH, 2)

		hr = pObject->AddLanguage(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000002)	//RemoveLanguage
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)

		hr = pObject->RemoveLanguage(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000005)	//Load
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)

		hr = pObject->Load(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000006)	//Save
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)

		hr = pObject->Save(v0);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IMessageManager>::g_typeinfo(__uuidof(IMessageManager), L"IMessageManager", s_mData, 7, _Invoke);

