#include "StdAfx.h"
#include "ti.h"

static ELEMDESC s_mElemDesc[] = 
{
	// Padding
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// PrivateKey
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// PublicKey
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Decrypt
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Encrypt
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// GenerateKey
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Padding", {0x00000004, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 44, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"PrivateKey", {0x00000005, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 52, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"PublicKey", {0x00000006, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 60, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"IsPrivateKey", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Key", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"KeySize", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_I2}}, 0}},
	{L"Padding", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_I2}}, 0}},
	{L"PrivateKey", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"PublicKey", {0x00000006, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 56, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Decrypt", {0x00000007, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 64, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Encrypt", {0x00000008, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 68, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"GenerateKey", {0x00000009, NULL, &s_mElemDesc[5], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 1, 72, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IRSA* pObject = (IRSA*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUT(0x00000004)	//Padding
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_Padding(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000005)	//PrivateKey
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_VARIANT, 0)

		hr = pObject->put_PrivateKey(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000006)	//PublicKey
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_VARIANT, 0)

		hr = pObject->put_PublicKey(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000001)	//IsPrivateKey
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_IsPrivateKey(v0);
		return hr;
	}

	IF_INVOKE(0x00000002)	//Key
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Key(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000003)	//KeySize
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_KeySize(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000004)	//Padding
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Padding(v0);
		return hr;
	}

	IF_INVOKE(0x00000005)	//PrivateKey
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_PrivateKey(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE(0x00000006)	//PublicKey
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_PublicKey(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_FUNC(0x00000007)	//Decrypt
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->Decrypt(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000008)	//Encrypt
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->Encrypt(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000009)	//GenerateKey
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_OPT(VT_VARIANT, 0)

		hr = pObject->GenerateKey(v0);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IRSA>::g_typeinfo(__uuidof(IRSA), L"IRSA", s_mData, 12, _Invoke);

