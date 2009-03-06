#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x6\0" L"MD5"}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// Create
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// Update
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Final
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"_NewEnum", {0xFFFFFFFC, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_UNKNOWN}}, 1}},
	{L"Name", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"HashSize", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_I2}}, 0}},
	{L"Create", {0x00000003, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 40, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Update", {0x00000004, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 44, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Final", {0x00000000, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 1, 48, 0, {{{NULL}, VT_VARIANT}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IHash* pObject = (IHash*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYGET(0xFFFFFFFC)	//_NewEnum
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_UNKNOWN, 0)

		hr = pObject->get__NewEnum(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000001)	//Name
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Name(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000002)	//HashSize
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_HashSize(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000003)	//Create
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_BSTR, 0, 0)

		hr = pObject->Create(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000004)	//Update
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)

		hr = pObject->Update(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000000)	//Final
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_OPT(VT_VARIANT, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->Final(v0, v1);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IHash>::g_typeinfo(__uuidof(IHash), L"IHash", s_mData, 6, _Invoke);

