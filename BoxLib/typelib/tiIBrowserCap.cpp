#include "StdAfx.h"
#include "ti.h"

static ELEMDESC s_mElemDesc[] = 
{
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"ActiveXControls", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"AOL", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"BackgroundSounds", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Beta", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"CDF", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Cookies", {0x00000006, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Crawler", {0x00000007, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"CSSVersion", {0x00000008, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 56, 0, {{{NULL}, VT_I2}}, 0}},
	{L"Frames", {0x00000009, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 60, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"IFrames", {0x0000000A, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 64, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"JavaApplets", {0x0000000B, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 68, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"JavaScript", {0x0000000C, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 72, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"MajorVersion", {0x0000000D, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 76, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"MinorVersion", {0x0000000E, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 80, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Name", {0x0000000F, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 84, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"NetCLR", {0x00000010, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 88, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Platform", {0x00000011, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 92, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Tables", {0x00000012, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 96, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Type", {0x00000013, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 100, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"VBScript", {0x00000014, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 104, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Version", {0x00000015, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 108, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"WAP", {0x00000016, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 112, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Win16", {0x00000017, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 116, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"UserAgent", {0x00000018, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 120, 0, {{{NULL}, VT_BSTR}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IBrowserCap* pObject = (IBrowserCap*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYGET(0x00000001)	//ActiveXControls
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_ActiveXControls(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000002)	//AOL
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_AOL(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000003)	//BackgroundSounds
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_BackgroundSounds(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000004)	//Beta
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_Beta(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000005)	//CDF
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_CDF(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000006)	//Cookies
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_Cookies(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000007)	//Crawler
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_Crawler(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000008)	//CSSVersion
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_CSSVersion(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000009)	//Frames
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_Frames(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000A)	//IFrames
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_IFrames(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000B)	//JavaApplets
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_JavaApplets(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000C)	//JavaScript
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_JavaScript(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000D)	//MajorVersion
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_MajorVersion(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000E)	//MinorVersion
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_MinorVersion(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000F)	//Name
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Name(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000010)	//NetCLR
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_NetCLR(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000011)	//Platform
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Platform(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000012)	//Tables
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_Tables(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000013)	//Type
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Type(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000014)	//VBScript
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_VBScript(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000015)	//Version
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Version(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000016)	//WAP
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_WAP(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000017)	//Win16
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_Win16(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000018)	//UserAgent
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_UserAgent(v0);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IBrowserCap>::g_typeinfo(__uuidof(IBrowserCap), L"IBrowserCap", s_mData, 24, _Invoke);

