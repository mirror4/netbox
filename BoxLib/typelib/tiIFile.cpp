#include "StdAfx.h"
#include "ti.h"

static ELEMDESC s_mElemDesc[] = 
{
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Path", {0x00000000, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"AbsolutePath", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Name", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Attributes", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_I2}}, 0}},
	{L"DateCreated", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_DATE}}, 0}},
	{L"DateLastModified", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_DATE}}, 0}},
	{L"DateLastAccessed", {0x00000006, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_DATE}}, 0}},
	{L"Size", {0x00000007, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 56, 0, {{{NULL}, VT_R8}}, 0}},
	{L"Type", {0x00000008, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 60, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"ContentType", {0x00000009, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 64, 0, {{{NULL}, VT_BSTR}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IFile* pObject = (IFile*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYGET(0x00000000)	//Path
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Path(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000001)	//AbsolutePath
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_AbsolutePath(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000002)	//Name
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Name(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000003)	//Attributes
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Attributes(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000004)	//DateCreated
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_DATE, 0)

		hr = pObject->get_DateCreated(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000005)	//DateLastModified
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_DATE, 0)

		hr = pObject->get_DateLastModified(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000006)	//DateLastAccessed
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_DATE, 0)

		hr = pObject->get_DateLastAccessed(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000007)	//Size
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_R8, 0)

		hr = pObject->get_Size(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000008)	//Type
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Type(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000009)	//ContentType
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_ContentType(v0);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IFile>::g_typeinfo(__uuidof(IFile), L"IFile", s_mData, 10, _Invoke);

