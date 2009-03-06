#include "StdAfx.h"
#include "ti.h"

static ELEMDESC s_mElemDesc[] = 
{
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"FileName", {0x00000000, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"CheckSum", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Files", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"Folders", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"DeveloperID", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_I4}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IPackage* pObject = (IPackage*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYGET(0x00000000)	//FileName
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_FileName(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000001)	//CheckSum
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_CheckSum(v0);
		return hr;
	}

	IF_INVOKE(0x00000002)	//Files
	{
		INVOKE_PARAM_RET(VT_DISPATCH, 0)

		hr = pObject->get_Files(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE(0x00000003)	//Folders
	{
		INVOKE_PARAM_RET(VT_DISPATCH, 0)

		hr = pObject->get_Folders(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000004)	//DeveloperID
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_DeveloperID(v0);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IPackage>::g_typeinfo(__uuidof(IPackage), L"IPackage", s_mData, 5, _Invoke);

