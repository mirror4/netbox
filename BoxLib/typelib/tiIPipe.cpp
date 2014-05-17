#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_I4, 0, 0, 0, 0}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// Open
	{{{NULL}, VT_I4}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"dataAvailable", {0x00000066, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Open", {0x00000065, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 28, 0, {{{NULL}, VT_DISPATCH}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IPipe* pObject = (IPipe*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYGET(0x00000066)	//dataAvailable
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_dataAvailable(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000065)	//Open
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I4, 0, 0)
		INVOKE_PARAM_RET(VT_DISPATCH, 1)

		hr = pObject->Open(v0, v1);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IPipe>::g_typeinfo(__uuidof(IPipe), L"IPipe", s_mData, 2, _Invoke);

