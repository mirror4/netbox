#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_I2, 0, 0, 0, 2}},
	{0x18, {VT_I4, 0, 0, 0, 0}},
	{0x18, {VT_I4, 0, 0, 0, -1}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// CommandTimeout
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// Synchronous
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// Open
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// Execute
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	{{{NULL}, VT_I4}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	// GetTable
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"CommandTimeout", {0x00000009, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 64, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Synchronous", {0x0000000A, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 72, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"CommandTimeout", {0x00000009, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 60, 0, {{{NULL}, VT_I2}}, 0}},
	{L"Synchronous", {0x0000000A, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 68, 0, {{{NULL}, VT_I2}}, 0}},
	{L"TotalReadTimes", {0x0000000B, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 76, 0, {{{NULL}, VT_I4}}, 0}},
	{L"TotalReadBytes", {0x0000000C, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 80, 0, {{{NULL}, VT_R8}}, 0}},
	{L"TotalWriteTimes", {0x0000000D, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 84, 0, {{{NULL}, VT_I4}}, 0}},
	{L"TotalWriteBytes", {0x0000000E, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 88, 0, {{{NULL}, VT_R8}}, 0}},
	{L"TotalExecTime", {0x0000000F, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 92, 0, {{{NULL}, VT_R8}}, 0}},
	{L"Open", {0x00000001, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 28, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Close", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Execute", {0x00000003, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 0, 36, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"GetTable", {0x00000004, NULL, &s_mElemDesc[7], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 40, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"BeginTrans", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"CommitTrans", {0x00000006, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"RollbackTrans", {0x00000007, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Shrink", {0x00000008, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 56, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IDatabase* pObject = (IDatabase*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUT(0x00000009)	//CommandTimeout
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_CommandTimeout(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000000A)	//Synchronous
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_Synchronous(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000009)	//CommandTimeout
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_CommandTimeout(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000A)	//Synchronous
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Synchronous(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000B)	//TotalReadTimes
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_TotalReadTimes(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000C)	//TotalReadBytes
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_R8, 0)

		hr = pObject->get_TotalReadBytes(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000D)	//TotalWriteTimes
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_TotalWriteTimes(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000E)	//TotalWriteBytes
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_R8, 0)

		hr = pObject->get_TotalWriteBytes(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000F)	//TotalExecTime
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_R8, 0)

		hr = pObject->get_TotalExecTime(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000001)	//Open
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 0)

		hr = pObject->Open(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000002)	//Close
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->Close();
		return hr;
	}

	IF_INVOKE_FUNC(0x00000003)	//Execute
	{
		if(cArgs1 > 3)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_I4, 1, 1)
		INVOKE_PARAM_DEF(VT_I4, 2, 2)
		INVOKE_PARAM_RET(VT_DISPATCH, 3)

		hr = pObject->Execute(v0, v1, v2, v3);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000004)	//GetTable
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_DISPATCH, 1)

		hr = pObject->GetTable(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000005)	//BeginTrans
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->BeginTrans();
		return hr;
	}

	IF_INVOKE_FUNC(0x00000006)	//CommitTrans
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->CommitTrans();
		return hr;
	}

	IF_INVOKE_FUNC(0x00000007)	//RollbackTrans
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->RollbackTrans();
		return hr;
	}

	IF_INVOKE_FUNC(0x00000008)	//Shrink
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->Shrink();
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IDatabase>::g_typeinfo(__uuidof(IDatabase), L"IDatabase", s_mData, 17, _Invoke);

