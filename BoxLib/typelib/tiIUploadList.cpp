#include "StdAfx.h"
#include "ti.h"

static ELEMDESC s_mElemDesc[] = 
{
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"FileName", {0x60020003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Size", {0x60020004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_I4}}, 0}},
	{L"ContentType", {0x60020005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_BSTR}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IUploadList* pObject = (IUploadList*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYGET(0x60020003)	//FileName
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_FileName(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x60020004)	//Size
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_Size(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x60020005)	//ContentType
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_ContentType(v0);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IUploadList>::g_typeinfo(__uuidof(IUploadList), L"IUploadList", s_mData, 3, _Invoke);

