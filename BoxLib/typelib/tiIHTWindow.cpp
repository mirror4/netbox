#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// ContextMenu
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Open
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// OpenModalDialog
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"ContextMenu", {0x00000065, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ContextMenu", {0x00000065, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Argument", {0x00000066, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Browser", {0x00000067, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"Open", {0x00000068, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 1, 44, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"OpenModalDialog", {0x00000069, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 4, 1, 48, 0, {{{NULL}, VT_VARIANT}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IHTWindow* pObject = (IHTWindow*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUT(0x00000065)	//ContextMenu
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_ContextMenu(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000065)	//ContextMenu
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_ContextMenu(v0);
		return hr;
	}

	IF_INVOKE(0x00000066)	//Argument
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Argument(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE(0x00000067)	//Browser
	{
		INVOKE_PARAM_RET(VT_DISPATCH, 0)

		hr = pObject->get_Browser(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_FUNC(0x00000068)	//Open
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_OPT(VT_VARIANT, 1)

		hr = pObject->Open(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000069)	//OpenModalDialog
	{
		if(cArgs1 > 4)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 0)
		INVOKE_PARAM_DEF(VT_I2, 2, 1)
		INVOKE_PARAM_OPT(VT_VARIANT, 3)
		INVOKE_PARAM_RET(VT_VARIANT, 4)

		hr = pObject->OpenModalDialog(v0, v1, v2, v3, v4);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IHTWindow>::g_typeinfo(__uuidof(IHTWindow), L"IHTWindow", s_mData, 6, _Invoke);

