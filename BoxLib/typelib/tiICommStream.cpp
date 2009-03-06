#include "StdAfx.h"
#include "ti.h"

static ELEMDESC s_mElemDesc[] = 
{
	// BaudRate
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// DataBits
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// Handshaking
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// Parity
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// ReadTimeout
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// StopBits
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// WriteTimeout
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// Config
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// Open
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"BaudRate", {0x00000065, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DataBits", {0x00000066, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 40, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Handshaking", {0x00000067, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 48, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Parity", {0x00000068, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 56, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ReadTimeout", {0x00000069, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 64, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"StopBits", {0x0000006A, NULL, &s_mElemDesc[5], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 72, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"WriteTimeout", {0x0000006B, NULL, &s_mElemDesc[6], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 80, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"BaudRate", {0x00000065, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_I2}}, 0}},
	{L"DataBits", {0x00000066, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_I2}}, 0}},
	{L"Handshaking", {0x00000067, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_I2}}, 0}},
	{L"Parity", {0x00000068, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_I2}}, 0}},
	{L"ReadTimeout", {0x00000069, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 60, 0, {{{NULL}, VT_I4}}, 0}},
	{L"StopBits", {0x0000006A, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 68, 0, {{{NULL}, VT_I2}}, 0}},
	{L"WriteTimeout", {0x0000006B, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 76, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Config", {0x0000006C, NULL, &s_mElemDesc[7], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 1, 84, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Open", {0x0000006D, NULL, &s_mElemDesc[8], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 1, 88, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	ICommStream* pObject = (ICommStream*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUT(0x00000065)	//BaudRate
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_BaudRate(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000066)	//DataBits
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_DataBits(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000067)	//Handshaking
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_Handshaking(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000068)	//Parity
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_Parity(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000069)	//ReadTimeout
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_ReadTimeout(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000006A)	//StopBits
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_StopBits(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000006B)	//WriteTimeout
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_WriteTimeout(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000065)	//BaudRate
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_BaudRate(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000066)	//DataBits
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_DataBits(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000067)	//Handshaking
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Handshaking(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000068)	//Parity
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Parity(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000069)	//ReadTimeout
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_ReadTimeout(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000006A)	//StopBits
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_StopBits(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000006B)	//WriteTimeout
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_WriteTimeout(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000006C)	//Config
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_OPT(VT_VARIANT, 0)

		hr = pObject->Config(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000006D)	//Open
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I2, 0)
		INVOKE_PARAM_OPT(VT_VARIANT, 1)

		hr = pObject->Open(v0, v1);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<ICommStream>::g_typeinfo(__uuidof(ICommStream), L"ICommStream", s_mData, 16, _Invoke);

