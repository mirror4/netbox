#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_BOOL, 0, 0, 0, -1}},
	{0x18, {VT_I2, 0, 0, 0, 1}},
	{0x18, {VT_I2, 0, 0, 0, 3}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// Create
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BOOL}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// Open
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Create", {0x00000065, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 28, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Open", {0x00000066, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IFileStream* pObject = (IFileStream*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_FUNC(0x00000065)	//Create
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_BOOL, 1, 0)

		hr = pObject->Create(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000066)	//Open
	{
		if(cArgs1 > 3)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 1)
		INVOKE_PARAM_DEF(VT_I2, 2, 2)

		hr = pObject->Open(v0, v1, v2);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IFileStream>::g_typeinfo(__uuidof(IFileStream), L"IFileStream", s_mData, 2, _Invoke);

