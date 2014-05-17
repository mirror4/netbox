#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_I2, 0, 0, 0, -1}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x6\0" L"DES"}},
	{0x18, {VT_I2, 0, 0, 0, -1}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// Key
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// IV
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Padding
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// GenerateKey
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// Create
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	// Encrypt
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// Decrypt
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Key", {0x00000005, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 52, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"IV", {0x00000008, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 68, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Padding", {0x0000000A, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 80, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"_NewEnum", {0xFFFFFFFC, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_UNKNOWN}}, 1}},
	{L"Name", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"BlockSize", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_I2}}, 0}},
	{L"CipherMode", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_I2}}, 0}},
	{L"KeySize", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_I2}}, 0}},
	{L"Key", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"IVSize", {0x00000007, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 60, 0, {{{NULL}, VT_I2}}, 0}},
	{L"IV", {0x00000008, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 64, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Padding", {0x0000000A, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 76, 0, {{{NULL}, VT_I2}}, 0}},
	{L"GenerateKey", {0x00000006, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 56, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"GenerateIV", {0x00000009, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 72, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Create", {0x0000000B, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 84, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Encrypt", {0x0000000C, NULL, &s_mElemDesc[6], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 1, 88, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Decrypt", {0x0000000D, NULL, &s_mElemDesc[8], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 1, 92, 0, {{{NULL}, VT_VARIANT}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	ICipher* pObject = (ICipher*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUT(0x00000005)	//Key
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_VARIANT, 0)

		hr = pObject->put_Key(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000008)	//IV
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_VARIANT, 0)

		hr = pObject->put_IV(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000000A)	//Padding
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_Padding(v0);
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

	IF_INVOKE_PROPERTYGET(0x00000001)	//Name
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Name(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000002)	//BlockSize
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_BlockSize(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000003)	//CipherMode
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_CipherMode(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000004)	//KeySize
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_KeySize(v0);
		return hr;
	}

	IF_INVOKE(0x00000005)	//Key
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_Key(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000007)	//IVSize
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_IVSize(v0);
		return hr;
	}

	IF_INVOKE(0x00000008)	//IV
	{
		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->get_IV(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000A)	//Padding
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Padding(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000006)	//GenerateKey
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I2, 0, 0)

		hr = pObject->GenerateKey(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000009)	//GenerateIV
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->GenerateIV();
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000B)	//Create
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_BSTR, 0, 1)
		INVOKE_PARAM_DEF(VT_I2, 1, 2)

		hr = pObject->Create(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000C)	//Encrypt
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_OPT(VT_VARIANT, 1)
		INVOKE_PARAM_RET(VT_VARIANT, 2)

		hr = pObject->Encrypt(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000D)	//Decrypt
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_OPT(VT_VARIANT, 1)
		INVOKE_PARAM_RET(VT_VARIANT, 2)

		hr = pObject->Decrypt(v0, v1, v2);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<ICipher>::g_typeinfo(__uuidof(ICipher), L"ICipher", s_mData, 17, _Invoke);

