#include "StdAfx.h"
#include "ti.h"

static ELEMDESC s_mElemDesc[] = 
{
	// PacketSize
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// ReceiveTimeout
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// BroadCast
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Bind
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// SendTo
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"PacketSize", {0x00000007, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 56, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ReceiveTimeout", {0x00000008, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 64, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"BroadCast", {0x00000009, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 72, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"LocalAddress", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"LocalPort", {0x00000006, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_I4}}, 0}},
	{L"PacketSize", {0x00000007, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_I4}}, 0}},
	{L"ReceiveTimeout", {0x00000008, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 60, 0, {{{NULL}, VT_I4}}, 0}},
	{L"BroadCast", {0x00000009, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 68, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Bind", {0x00000001, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 28, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"SendTo", {0x00000002, NULL, &s_mElemDesc[5], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"RecvFrom", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"Close", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IUDPSocket* pObject = (IUDPSocket*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUT(0x00000007)	//PacketSize
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_PacketSize(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000008)	//ReceiveTimeout
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_ReceiveTimeout(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000009)	//BroadCast
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_BroadCast(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000005)	//LocalAddress
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_LocalAddress(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000006)	//LocalPort
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_LocalPort(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000007)	//PacketSize
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_PacketSize(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000008)	//ReceiveTimeout
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_ReceiveTimeout(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000009)	//BroadCast
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_BroadCast(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000001)	//Bind
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM(VT_I4, 1)

		hr = pObject->Bind(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000002)	//SendTo
	{
		if(cArgs1 > 3)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 3)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM(VT_VARIANT, 2)

		hr = pObject->SendTo(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000003)	//RecvFrom
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_DISPATCH, 0)

		hr = pObject->RecvFrom(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000004)	//Close
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->Close();
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IUDPSocket>::g_typeinfo(__uuidof(IUDPSocket), L"IUDPSocket", s_mData, 12, _Invoke);

