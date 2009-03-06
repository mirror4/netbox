#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_BOOL, 0, 0, 0, -1}},
	{0x18, {VT_I4, 0, 0, 0, 0}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x0\0" L""}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x0\0" L""}},
	{0x18, {VT_I4, 0, 0, 0, -1}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// Flags
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// getResponseHeader
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// Open
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BOOL}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// Read
	{{{NULL}, VT_I4}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	// Send
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// setRequestHeader
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// setProxy
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[3], 0x31}},
	// setProxyCredentials
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// WaitForResponse
	{{{NULL}, VT_I4}, {(ULONG_PTR)&s_mElemDescEx[4], 0x31}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Flags", {0x0000000A, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 68, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"dataAvailable", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_I2}}, 0}},
	{L"readyState", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_I2}}, 0}},
	{L"status", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_I2}}, 0}},
	{L"statusText", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"TotalBytes", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Flags", {0x0000000A, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 64, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Abort", {0x00000006, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"getAllResponseHeaders", {0x00000007, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"getResponseHeader", {0x00000008, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 56, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Open", {0x00000009, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 5, 2, 60, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Read", {0x0000000B, NULL, &s_mElemDesc[7], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 72, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"ReadAll", {0x0000000C, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 76, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Send", {0x0000000D, NULL, &s_mElemDesc[8], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 1, 80, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"setRequestHeader", {0x0000000E, NULL, &s_mElemDesc[9], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 84, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"setProxy", {0x0000000F, NULL, &s_mElemDesc[11], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 0, 88, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"setProxyCredentials", {0x00000010, NULL, &s_mElemDesc[14], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 92, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"WaitForResponse", {0x00000011, NULL, &s_mElemDesc[16], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 96, 0, {{{NULL}, VT_BOOL}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IHttpRequest* pObject = (IHttpRequest*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUT(0x0000000A)	//Flags
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_Flags(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000001)	//dataAvailable
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_dataAvailable(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000002)	//readyState
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_readyState(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000003)	//status
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_status(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000004)	//statusText
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_statusText(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000005)	//TotalBytes
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_TotalBytes(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000A)	//Flags
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_Flags(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000006)	//Abort
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->Abort();
		return hr;
	}

	IF_INVOKE_FUNC(0x00000007)	//getAllResponseHeaders
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->getAllResponseHeaders(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000008)	//getResponseHeader
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->getResponseHeader(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000009)	//Open
	{
		if(cArgs1 > 5)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM(VT_BSTR, 1)
		INVOKE_PARAM_DEF(VT_BOOL, 2, 0)
		INVOKE_PARAM_OPT(VT_VARIANT, 3)
		INVOKE_PARAM_OPT(VT_VARIANT, 4)

		hr = pObject->Open(v0, v1, v2, v3, v4);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000B)	//Read
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I4, 0, 1)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->Read(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000C)	//ReadAll
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->ReadAll(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000D)	//Send
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_OPT(VT_VARIANT, 0)

		hr = pObject->Send(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000E)	//setRequestHeader
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM(VT_BSTR, 1)

		hr = pObject->setRequestHeader(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000F)	//setProxy
	{
		if(cArgs1 > 3)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I2, 0)
		INVOKE_PARAM_DEF(VT_BSTR, 1, 2)
		INVOKE_PARAM_DEF(VT_BSTR, 2, 3)

		hr = pObject->setProxy(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000010)	//setProxyCredentials
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM(VT_BSTR, 1)

		hr = pObject->setProxyCredentials(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000011)	//WaitForResponse
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I4, 0, 4)
		INVOKE_PARAM_RET(VT_BOOL, 1)

		hr = pObject->WaitForResponse(v0, v1);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IHttpRequest>::g_typeinfo(__uuidof(IHttpRequest), L"IHttpRequest", s_mData, 18, _Invoke);

