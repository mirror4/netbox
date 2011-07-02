#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_BOOL, 0, 0, 0, -1}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x2\0" L","}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x2\0" L","}},
	{0x18, {VT_I4, 0, 0, 0, -1}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// Add
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Insert
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Remove
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// Sort
	{{{NULL}, VT_BOOL}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// Join
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	// Split
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	// toJson
	{{{NULL}, VT_I4}, {(ULONG_PTR)&s_mElemDescEx[3], 0x31}},
	// fromJson
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"_NewEnum", {0xFFFFFFFC, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_UNKNOWN}}, 1}},
	{L"Items", {0x00000010, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Count", {0x00000011, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Add", {0x00000012, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 40, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Insert", {0x00000013, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 44, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Remove", {0x00000014, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 48, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"RemoveAll", {0x00000015, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Sort", {0x00000016, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 56, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Join", {0x00000017, NULL, &s_mElemDesc[5], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 60, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Split", {0x00000018, NULL, &s_mElemDesc[6], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 64, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"toJson", {0x00000019, NULL, &s_mElemDesc[7], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 68, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"fromJson", {0x0000001A, NULL, &s_mElemDesc[8], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 72, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IVariantList* pObject = (IVariantList*)pvInstance;
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

	IF_INVOKE(0x00000010)	//Items
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Items(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000011)	//Count
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_Count(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000012)	//Add
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)

		hr = pObject->Add(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000013)	//Insert
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_VARIANT, 1)

		hr = pObject->Insert(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000014)	//Remove
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)

		hr = pObject->Remove(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000015)	//RemoveAll
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->RemoveAll();
		return hr;
	}

	IF_INVOKE_FUNC(0x00000016)	//Sort
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_BOOL, 0, 0)

		hr = pObject->Sort(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000017)	//Join
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_BSTR, 0, 1)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->Join(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000018)	//Split
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_BSTR, 1, 2)

		hr = pObject->Split(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000019)	//toJson
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I4, 0, 3)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->toJson(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000001A)	//fromJson
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM(VT_BSTR, 0)
		
		hr = pObject->fromJson(v0);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IVariantList>::g_typeinfo(__uuidof(IVariantList), L"IVariantList", s_mData, 12, _Invoke);

