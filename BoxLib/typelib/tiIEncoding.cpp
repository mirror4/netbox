#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I4, 0, 0, 0, 0}},
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I4, 0, 0, 0, 0}},
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
	// Base32Decode
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// Base32Encode
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// Base64Decode
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// Base64Encode
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	// BinToStr
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	// CsvDecode
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// CsvEncode
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Deflate
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// HexDecode
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// HexEncode
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[3], 0x31}},
	// HtmlEncode
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// HtmlFormat
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// Inflate
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// JSEncode
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// SQLEncode
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// StrToBin
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {(ULONG_PTR)&s_mElemDescEx[4], 0x31}},
	// UrlDecode
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// UrlEncode
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// xmlEncode
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// FormatMessage
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[5], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[6], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[7], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[8], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[9], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[10], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[11], 0x31}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[12], 0x31}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Base32Decode", {0x00000001, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 28, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Base32Encode", {0x00000002, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 32, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Base64Decode", {0x00000003, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 36, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Base64Encode", {0x00000004, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 40, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"BinToStr", {0x00000005, NULL, &s_mElemDesc[6], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 44, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"CsvDecode", {0x00000006, NULL, &s_mElemDesc[8], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 48, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"CsvEncode", {0x00000007, NULL, &s_mElemDesc[9], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 52, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Deflate", {0x00000008, NULL, &s_mElemDesc[10], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 56, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"HexDecode", {0x00000009, NULL, &s_mElemDesc[11], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 60, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"HexEncode", {0x0000000A, NULL, &s_mElemDesc[12], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 64, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"HtmlEncode", {0x0000000B, NULL, &s_mElemDesc[14], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 68, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"HtmlFormat", {0x0000000C, NULL, &s_mElemDesc[15], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 72, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Inflate", {0x0000000D, NULL, &s_mElemDesc[16], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 76, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"JSEncode", {0x0000000E, NULL, &s_mElemDesc[17], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 80, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"SQLEncode", {0x0000000F, NULL, &s_mElemDesc[18], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 84, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"StrToBin", {0x00000010, NULL, &s_mElemDesc[19], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 88, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"UrlDecode", {0x00000011, NULL, &s_mElemDesc[21], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 92, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"UrlEncode", {0x00000012, NULL, &s_mElemDesc[22], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 96, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"xmlEncode", {0x00000013, NULL, &s_mElemDesc[23], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 100, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"FormatMessage", {0x00000014, NULL, &s_mElemDesc[24], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 9, 0, 104, 0, {{{NULL}, VT_BSTR}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IEncoding* pObject = (IEncoding*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_FUNC(0x00000001)	//Base32Decode
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->Base32Decode(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000002)	//Base32Encode
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 0)
		INVOKE_PARAM_RET(VT_BSTR, 2)

		hr = pObject->Base32Encode(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000003)	//Base64Decode
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->Base64Decode(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000004)	//Base64Encode
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 1)
		INVOKE_PARAM_RET(VT_BSTR, 2)

		hr = pObject->Base64Encode(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000005)	//BinToStr
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_DEF(VT_I4, 1, 2)
		INVOKE_PARAM_RET(VT_BSTR, 2)

		hr = pObject->BinToStr(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000006)	//CsvDecode
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->CsvDecode(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000007)	//CsvEncode
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->CsvEncode(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000008)	//Deflate
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->Deflate(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000009)	//HexDecode
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->HexDecode(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000A)	//HexEncode
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 3)
		INVOKE_PARAM_RET(VT_BSTR, 2)

		hr = pObject->HexEncode(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000B)	//HtmlEncode
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->HtmlEncode(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000C)	//HtmlFormat
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->HtmlFormat(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000D)	//Inflate
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->Inflate(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000E)	//JSEncode
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->JSEncode(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000F)	//SQLEncode
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->SQLEncode(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000010)	//StrToBin
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_I4, 1, 4)
		INVOKE_PARAM_RET(VT_VARIANT, 2)

		hr = pObject->StrToBin(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000011)	//UrlDecode
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->UrlDecode(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000012)	//UrlEncode
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->UrlEncode(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000013)	//xmlEncode
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->xmlEncode(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000014)	//FormatMessage
	{
		if(cArgs1 > 9)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_BSTR, 1, 5)
		INVOKE_PARAM_DEF(VT_BSTR, 2, 6)
		INVOKE_PARAM_DEF(VT_BSTR, 3, 7)
		INVOKE_PARAM_DEF(VT_BSTR, 4, 8)
		INVOKE_PARAM_DEF(VT_BSTR, 5, 9)
		INVOKE_PARAM_DEF(VT_BSTR, 6, 10)
		INVOKE_PARAM_DEF(VT_BSTR, 7, 11)
		INVOKE_PARAM_DEF(VT_BSTR, 8, 12)
		INVOKE_PARAM_RET(VT_BSTR, 9)

		hr = pObject->FormatMessage(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IEncoding>::g_typeinfo(__uuidof(IEncoding), L"IEncoding", s_mData, 20, _Invoke);

