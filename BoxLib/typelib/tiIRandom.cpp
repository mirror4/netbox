#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_I2, 0, 0, 0, 16}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// Generate
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Generate", {0x00000000, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 28, 0, {{{NULL}, VT_VARIANT}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IRandom* pObject = (IRandom*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_FUNC(0x00000000)	//Generate
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I2, 0, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->Generate(v0, v1);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IRandom>::g_typeinfo(__uuidof(IRandom), L"IRandom", s_mData, 1, _Invoke);

