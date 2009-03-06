#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x0\0" L""}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x0\0" L""}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x0\0" L""}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x0\0" L""}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x0\0" L""}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x0\0" L""}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x0\0" L""}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x0\0" L""}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// GetMessage
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// FormatMessage
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[3], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[4], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[5], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[6], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[7], 0x31}},
	// AddMessage
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// RemoveMessage
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Count", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Language", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"CodePage", {0x00000006, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Names", {0x00000007, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 56, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Messages", {0x00000008, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 60, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"GetMessage", {0x00000000, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 28, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"FormatMessage", {0x00000001, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 9, 0, 32, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"AddMessage", {0x00000002, NULL, &s_mElemDesc[10], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 36, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"RemoveMessage", {0x00000003, NULL, &s_mElemDesc[12], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 40, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IMessageColl* pObject = (IMessageColl*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYGET(0x00000004)	//Count
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_Count(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000005)	//Language
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Language(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000006)	//CodePage
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_CodePage(v0);
		return hr;
	}

	IF_INVOKE(0x00000007)	//Names
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Names(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE(0x00000008)	//Messages
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Messages(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_FUNC(0x00000000)	//GetMessage
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->GetMessage(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000001)	//FormatMessage
	{
		if(cArgs1 > 9)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_BSTR, 1, 0)
		INVOKE_PARAM_DEF(VT_BSTR, 2, 1)
		INVOKE_PARAM_DEF(VT_BSTR, 3, 2)
		INVOKE_PARAM_DEF(VT_BSTR, 4, 3)
		INVOKE_PARAM_DEF(VT_BSTR, 5, 4)
		INVOKE_PARAM_DEF(VT_BSTR, 6, 5)
		INVOKE_PARAM_DEF(VT_BSTR, 7, 6)
		INVOKE_PARAM_DEF(VT_BSTR, 8, 7)
		INVOKE_PARAM_RET(VT_BSTR, 9)

		hr = pObject->FormatMessage(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000002)	//AddMessage
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM(VT_BSTR, 1)

		hr = pObject->AddMessage(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000003)	//RemoveMessage
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)

		hr = pObject->RemoveMessage(v0);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IMessageColl>::g_typeinfo(__uuidof(IMessageColl), L"IMessageColl", s_mData, 9, _Invoke);

