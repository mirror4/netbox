#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_I4, 0, 0, 0, -1}},
	{0x18, {VT_I2, 0, 0, 0, 7}},
	{0x18, {VT_I2, 0, 0, 0, 7}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// Item
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Item
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Item
	{{{NULL}, VT_I4}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// Load
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	// Save
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Item", {0x00000000, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUTREF, CC_STDCALL, 2, 0, 36, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Item", {0x00000000, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 2, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Item", {0x00000000, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 1, 0, 28, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Load", {0x00000001, NULL, &s_mElemDesc[5], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 40, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Save", {0x00000002, NULL, &s_mElemDesc[7], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 44, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IVariantStruct* pObject = (IVariantStruct*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUTREF(0x00000000)	//Item
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM_NAMED(VT_VARIANT, 1)

		hr = pObject->putref_Item(v0, v1);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000000)	//Item
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM_NAMED_VALUE(VT_VARIANT, 1)

		hr = pObject->put_Item(v0, v1);
		return hr;
	}

	IF_INVOKE(0x00000000)	//Item
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I4, 0, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->get_Item(v0, v1);

		INVOKE_DISP_PUT(1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000001)	//Load
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 1)

		hr = pObject->Load(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000002)	//Save
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 2)

		hr = pObject->Save(v0, v1);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IVariantStruct>::g_typeinfo(__uuidof(IVariantStruct), L"IVariantStruct", s_mData, 5, _Invoke);

