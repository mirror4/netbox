#include "StdAfx.h"
#include "ti.h"

static ELEMDESC s_mElemDesc[] = 
{
	// Arguments
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// ShowCmd
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// Load
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// Resolve
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Arguments", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Arguments", {0x00000001, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Description", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Description", {0x00000002, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 40, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Hotkey", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Hotkey", {0x00000003, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 48, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"IconLocation", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"IconLocation", {0x00000004, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 56, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"IDList", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 60, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"IDList", {0x00000005, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 64, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"TargetPath", {0x00000006, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 68, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"TargetPath", {0x00000006, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 72, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"WindowStyle", {0x00000007, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 76, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"WindowStyle", {0x00000007, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 80, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"WorkingDirectory", {0x00000008, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 84, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"WorkingDirectory", {0x00000008, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 88, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Load", {0x00000009, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 92, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Save", {0x0000000A, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 96, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Resolve", {0x0000000B, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 100, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IBShellLink* pObject = (IBShellLink*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYGET(0x00000001)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Arguments(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000001)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BSTR, 0)

		hr = pObject->put_Arguments(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000002)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Description(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000002)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BSTR, 0)

		hr = pObject->put_Description(v0);
		return hr;
	}


	IF_INVOKE_PROPERTYGET(0x00000003)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_Hotkey(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000003)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_Hotkey(v0);
		return hr;
	}


	IF_INVOKE_PROPERTYGET(0x00000004)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_IconLocation(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000004)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BSTR, 0)

		hr = pObject->put_IconLocation(v0);
		return hr;
	}


	IF_INVOKE_PROPERTYGET(0x00000005)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_IDList(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000005)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BSTR, 0)

		hr = pObject->put_IDList(v0);
		return hr;
	}


	IF_INVOKE_PROPERTYGET(0x00000006)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Path(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000006)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BSTR, 0)

		hr = pObject->put_Path(v0);
		return hr;
	}


	IF_INVOKE_PROPERTYGET(0x00000007)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_ShowCmd(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000007)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_ShowCmd(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000008)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_WorkingDirectory(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000008)
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BSTR, 0)

		hr = pObject->put_WorkingDirectory(v0);
		return hr;
	}



	IF_INVOKE_FUNC(0x00000009)	//Load
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)

		hr = pObject->Load(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000A)	//Save
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)

		hr = pObject->Save(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000B)	//Resolve
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)

		hr = pObject->Resolve(v0, v1);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IBShellLink>::g_typeinfo(__uuidof(IBShellLink), L"IBShellLink", s_mData, 19, _Invoke);

