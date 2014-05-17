#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_I2, 0, 0, 0, 7}},
	{0x18, {VT_I2, 0, 0, 0, 7}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x2\0" L","}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x2\0" L","}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// AddHead
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// AddTail
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Load
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// Save
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	// Join
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	// Split
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[3], 0x31}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Item", {0x00000000, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Count", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Items", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"AddHead", {0x00000003, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 40, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"AddTail", {0x00000004, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 44, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"GetHead", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"GetTail", {0x00000006, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"RemoveHead", {0x00000007, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 56, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"RemoveTail", {0x00000008, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 60, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"RemoveAll", {0x00000009, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 64, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Load", {0x0000000A, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 68, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Save", {0x0000000B, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 72, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Join", {0x0000000C, NULL, &s_mElemDesc[6], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 76, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Split", {0x0000000D, NULL, &s_mElemDesc[7], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 80, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IQueue* pObject = (IQueue*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE(0x00000000)	//Item
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Item(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000001)	//Count
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_Count(v0);
		return hr;
	}

	IF_INVOKE(0x00000002)	//Items
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Items(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_FUNC(0x00000003)	//AddHead
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)

		hr = pObject->AddHead(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000004)	//AddTail
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)

		hr = pObject->AddTail(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000005)	//GetHead
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->GetHead(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000006)	//GetTail
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->GetTail(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000007)	//RemoveHead
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->RemoveHead(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000008)	//RemoveTail
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->RemoveTail(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000009)	//RemoveAll
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->RemoveAll();
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000A)	//Load
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 0)

		hr = pObject->Load(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000B)	//Save
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 1)

		hr = pObject->Save(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000C)	//Join
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_BSTR, 0, 2)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->Join(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000D)	//Split
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_BSTR, 1, 3)

		hr = pObject->Split(v0, v1);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IQueue>::g_typeinfo(__uuidof(IQueue), L"IQueue", s_mData, 14, _Invoke);

