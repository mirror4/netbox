#include "StdAfx.h"
#include "ti.h"

static ELEMDESC s_mElemDesc[] = 
{
	// Value
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// AbsolutePosition
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// Value
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// Move
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// Load
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// Save
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Value", {0x00000000, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 2, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"AbsolutePosition", {0x0000000C, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 84, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Value", {0x00000000, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 1, 1, 28, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"BOF", {0x0000000A, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 72, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"EOF", {0x0000000B, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 76, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"AbsolutePosition", {0x0000000C, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 80, 0, {{{NULL}, VT_I4}}, 0}},
	{L"ResultCount", {0x0000000D, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 88, 0, {{{NULL}, VT_I4}}, 0}},
	{L"RecordCount", {0x0000000E, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 92, 0, {{{NULL}, VT_I4}}, 0}},
	{L"FirstPosition", {0x0000000F, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 96, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Fields", {0x00000010, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 100, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"NextRecordset", {0x00000011, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 104, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"ReadTimes", {0x00000012, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 108, 0, {{{NULL}, VT_I4}}, 0}},
	{L"ReadBytes", {0x00000013, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 112, 0, {{{NULL}, VT_R8}}, 0}},
	{L"WriteTimes", {0x00000014, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 116, 0, {{{NULL}, VT_I4}}, 0}},
	{L"WriteBytes", {0x00000015, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 120, 0, {{{NULL}, VT_R8}}, 0}},
	{L"ExecTime", {0x00000016, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 124, 0, {{{NULL}, VT_R8}}, 0}},
	{L"Move", {0x00000001, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 36, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"MoveFirst", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"MoveLast", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"MoveNext", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"MovePrevious", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Load", {0x00000006, NULL, &s_mElemDesc[5], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 56, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Save", {0x00000007, NULL, &s_mElemDesc[6], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 60, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"AddNew", {0x00000008, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 64, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Update", {0x00000009, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 68, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IRecordset* pObject = (IRecordset*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUT(0x00000000)	//Value
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_NAMED_VALUE(VT_VARIANT, 1)

		hr = pObject->put_Value(v0, v1);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000000C)	//AbsolutePosition
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_AbsolutePosition(v0);
		return hr;
	}

	IF_INVOKE(0x00000000)	//Value
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_OPT(VT_VARIANT, 0)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->get_Value(v0, v1);

		INVOKE_DISP_PUT(1);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000A)	//BOF
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_BOF(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000B)	//EOF
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_EOF(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000C)	//AbsolutePosition
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_AbsolutePosition(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000D)	//ResultCount
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_ResultCount(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000E)	//RecordCount
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_RecordCount(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000F)	//FirstPosition
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_FirstPosition(v0);
		return hr;
	}

	IF_INVOKE(0x00000010)	//Fields
	{
		INVOKE_PARAM_RET(VT_DISPATCH, 0)

		hr = pObject->get_Fields(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE(0x00000011)	//NextRecordset
	{
		INVOKE_PARAM_RET(VT_DISPATCH, 0)

		hr = pObject->get_NextRecordset(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000012)	//ReadTimes
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_ReadTimes(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000013)	//ReadBytes
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_R8, 0)

		hr = pObject->get_ReadBytes(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000014)	//WriteTimes
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_WriteTimes(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000015)	//WriteBytes
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_R8, 0)

		hr = pObject->get_WriteBytes(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000016)	//ExecTime
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_R8, 0)

		hr = pObject->get_ExecTime(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000001)	//Move
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)

		hr = pObject->Move(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000002)	//MoveFirst
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->MoveFirst();
		return hr;
	}

	IF_INVOKE_FUNC(0x00000003)	//MoveLast
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->MoveLast();
		return hr;
	}

	IF_INVOKE_FUNC(0x00000004)	//MoveNext
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->MoveNext();
		return hr;
	}

	IF_INVOKE_FUNC(0x00000005)	//MovePrevious
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->MovePrevious();
		return hr;
	}

	IF_INVOKE_FUNC(0x00000006)	//Load
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)

		hr = pObject->Load(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000007)	//Save
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)

		hr = pObject->Save(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000008)	//AddNew
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->AddNew();
		return hr;
	}

	IF_INVOKE_FUNC(0x00000009)	//Update
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->Update();
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IRecordset>::g_typeinfo(__uuidof(IRecordset), L"IRecordset", s_mData, 25, _Invoke);

