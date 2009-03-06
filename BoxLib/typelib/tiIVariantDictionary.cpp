#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_BOOL, 0, 0, 0, -1}},
	{0x18, {VT_I2, 0, 0, 0, 7}},
	{0x18, {VT_I2, 0, 0, 0, 7}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x2\0" L"="}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x2\0" L","}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x2\0" L"="}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x2\0" L","}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// Item
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Item
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// ArrayMode
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Item
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// Key
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Remove
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Add
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Exists
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Sort
	{{{NULL}, VT_BOOL}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// Load
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	// Save
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	// Join
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[3], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[4], 0x31}},
	// Split
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[5], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[6], 0x31}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Item", {0x00000000, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUTREF, CC_STDCALL, 2, 0, 36, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Item", {0x00000000, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 2, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ArrayMode", {0x6002000A, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 72, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Item", {0x00000000, NULL, &s_mElemDesc[5], FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 1, 1, 28, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Key", {0x60020003, NULL, &s_mElemDesc[6], FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 1, 0, 40, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Count", {0x60020004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_I4}}, 0}},
	{L"_NewEnum", {0xFFFFFFFC, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_UNKNOWN}}, 1}},
	{L"Items", {0x60020008, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 60, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Keys", {0x60020009, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 64, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"ArrayMode", {0x6002000A, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 68, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Remove", {0x60020006, NULL, &s_mElemDesc[7], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 52, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"RemoveAll", {0x60020007, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 56, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Add", {0x6002000B, NULL, &s_mElemDesc[8], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 76, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Exists", {0x6002000C, NULL, &s_mElemDesc[10], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 80, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Sort", {0x6002000D, NULL, &s_mElemDesc[11], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 84, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Load", {0x6002000E, NULL, &s_mElemDesc[12], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 88, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Save", {0x6002000F, NULL, &s_mElemDesc[14], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 92, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Join", {0x60020010, NULL, &s_mElemDesc[16], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 96, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Split", {0x60020011, NULL, &s_mElemDesc[18], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 0, 100, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IVariantDictionary* pObject = (IVariantDictionary*)pvInstance;
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

		INVOKE_PARAM(VT_VARIANT, 0)
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

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_NAMED_VALUE(VT_VARIANT, 1)

		hr = pObject->put_Item(v0, v1);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x6002000A)	//ArrayMode
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_ArrayMode(v0);
		return hr;
	}

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

	IF_INVOKE_PROPERTYGET(0x60020004)	//Count
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_Count(v0);
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

	IF_INVOKE(0x60020008)	//Items
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Items(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE(0x60020009)	//Keys
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Keys(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x6002000A)	//ArrayMode
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_ArrayMode(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x60020006)	//Remove
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)

		hr = pObject->Remove(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x60020007)	//RemoveAll
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->RemoveAll();
		return hr;
	}

	IF_INVOKE_FUNC(0x6002000B)	//Add
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM(VT_VARIANT, 1)

		hr = pObject->Add(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x6002000C)	//Exists
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

	IF_INVOKE_FUNC(0x6002000D)	//Sort
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_BOOL, 0, 0)

		hr = pObject->Sort(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x6002000E)	//Load
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

	IF_INVOKE_FUNC(0x6002000F)	//Save
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

	IF_INVOKE_FUNC(0x60020010)	//Join
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_BSTR, 0, 3)
		INVOKE_PARAM_DEF(VT_BSTR, 1, 4)
		INVOKE_PARAM_RET(VT_BSTR, 2)

		hr = pObject->Join(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x60020011)	//Split
	{
		if(cArgs1 > 3)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_BSTR, 1, 5)
		INVOKE_PARAM_DEF(VT_BSTR, 2, 6)

		hr = pObject->Split(v0, v1, v2);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IVariantDictionary>::g_typeinfo(__uuidof(IVariantDictionary), L"IVariantDictionary", s_mData, 19, _Invoke);

