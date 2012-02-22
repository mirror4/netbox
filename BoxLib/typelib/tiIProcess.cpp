#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"ExitCode", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_I4}}, 0}},
	{L"ProcessID", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Status", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_I4}}, 0}},
	{L"FileName", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Terminate", {0x00000010, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IProcess* pObject = (IProcess*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYGET(0x00000001)	//ExitCode
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_ExitCode(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000002)	//ProcessID
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_ProcessID(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000003)	//Status
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_Status(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000004)	//FileName
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_FileName(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000010)	//Terminate
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->Terminate();
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IProcess>::g_typeinfo(__uuidof(IProcess), L"IProcess", s_mData, 5, _Invoke);
