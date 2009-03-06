#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_I4, 0, 0, 0, 1048576}},
	{0x18, {VT_I4, 0, 0, 0, -1}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x0\0" L""}},
	{0x18, {VT_I2, 0, 0, 0, 1}},
	{0x18, {VT_I4, 0, 0, 0, -1}},
	{0x18, {VT_I2, 0, 0, 0, 7}},
	{0x18, {VT_I2, 0, 0, 0, 7}},
	{0x18, {VT_I2, 0, 0, 0, 7}},
	{0x18, {VT_I2, 0, 0, 0, 7}},
	{0x18, {VT_R8, 0, 0, 0, -1}},
	{0x18, {VT_R8, 0, 0, 0, -1}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// CodePage
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// Position
	{{{NULL}, VT_R8}, {NULL, 0x1}},
	// Size
	{{{NULL}, VT_R8}, {NULL, 0x1}},
	// Open
	{{{NULL}, VT_I4}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// Read
	{{{NULL}, VT_I4}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	// Write
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// WriteLine
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	// WriteBlankLines
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[3], 0x31}},
	// ReadText
	{{{NULL}, VT_I4}, {(ULONG_PTR)&s_mElemDescEx[4], 0x31}},
	// WriteText
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// ReadVariant
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[5], 0x31}},
	// WriteVariant
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[6], 0x31}},
	// WriteBoolean
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// WriteByte
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// WriteInt
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// WriteLong
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// WriteFloat
	{{{NULL}, VT_R4}, {NULL, 0x1}},
	// WriteDouble
	{{{NULL}, VT_R8}, {NULL, 0x1}},
	// WriteDate
	{{{NULL}, VT_DATE}, {NULL, 0x1}},
	// ReadObject
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[7], 0x31}},
	// WriteObject
	{{{NULL}, VT_DISPATCH}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[8], 0x31}},
	// Attach
	{{{NULL}, VT_UNKNOWN}, {NULL, 0x1}},
	// CopyTo
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_R8}, {(ULONG_PTR)&s_mElemDescEx[9], 0x31}},
	// CopyFrom
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_R8}, {(ULONG_PTR)&s_mElemDescEx[10], 0x31}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"CodePage", {0x00000001, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Position", {0x00000005, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 52, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Size", {0x00000006, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 60, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"CodePage", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_I4}}, 0}},
	{L"EOS", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Handle", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_I4}}, 0}},
	{L"lastModify", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_DATE}}, 0}},
	{L"Position", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_R8}}, 0}},
	{L"Size", {0x00000006, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 56, 0, {{{NULL}, VT_R8}}, 0}},
	{L"Type", {0x00000007, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 64, 0, {{{NULL}, VT_I2}}, 0}},
	{L"Open", {0x00000008, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 68, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Read", {0x00000009, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 72, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"Write", {0x0000000A, NULL, &s_mElemDesc[5], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 76, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ReadLine", {0x0000000B, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 80, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"WriteLine", {0x0000000C, NULL, &s_mElemDesc[6], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 84, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"WriteBlankLines", {0x0000000D, NULL, &s_mElemDesc[7], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 88, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ReadRawLine", {0x0000000E, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 92, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"ReadText", {0x0000000F, NULL, &s_mElemDesc[8], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 96, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"WriteText", {0x00000010, NULL, &s_mElemDesc[9], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 100, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ReadVariant", {0x00000011, NULL, &s_mElemDesc[10], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 104, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"WriteVariant", {0x00000012, NULL, &s_mElemDesc[11], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 108, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ReadBoolean", {0x00000013, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 112, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"WriteBoolean", {0x00000014, NULL, &s_mElemDesc[13], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 116, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ReadByte", {0x00000015, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 120, 0, {{{NULL}, VT_I2}}, 0}},
	{L"WriteByte", {0x00000016, NULL, &s_mElemDesc[14], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 124, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ReadInt", {0x00000017, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 128, 0, {{{NULL}, VT_I2}}, 0}},
	{L"WriteInt", {0x00000018, NULL, &s_mElemDesc[15], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 132, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ReadLong", {0x00000019, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 136, 0, {{{NULL}, VT_I4}}, 0}},
	{L"WriteLong", {0x0000001A, NULL, &s_mElemDesc[16], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 140, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ReadFloat", {0x0000001B, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 144, 0, {{{NULL}, VT_R4}}, 0}},
	{L"WriteFloat", {0x0000001C, NULL, &s_mElemDesc[17], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 148, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ReadDouble", {0x0000001D, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 152, 0, {{{NULL}, VT_R8}}, 0}},
	{L"WriteDouble", {0x0000001E, NULL, &s_mElemDesc[18], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 156, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ReadDate", {0x0000001F, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 160, 0, {{{NULL}, VT_DATE}}, 0}},
	{L"WriteDate", {0x00000020, NULL, &s_mElemDesc[19], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 164, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ReadObject", {0x00000021, NULL, &s_mElemDesc[20], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 168, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"WriteObject", {0x00000022, NULL, &s_mElemDesc[21], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 172, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Close", {0x00000023, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 176, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Attach", {0x00000024, NULL, &s_mElemDesc[23], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 180, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"CopyTo", {0x00000025, NULL, &s_mElemDesc[24], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 184, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"CopyFrom", {0x00000026, NULL, &s_mElemDesc[26], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 188, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"setEOS", {0x00000027, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 192, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IBaseStream* pObject = (IBaseStream*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUT(0x00000001)	//CodePage
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_CodePage(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000005)	//Position
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_R8, 0)

		hr = pObject->put_Position(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000006)	//Size
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_R8, 0)

		hr = pObject->put_Size(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000001)	//CodePage
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_CodePage(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000002)	//EOS
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_EOS(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000003)	//Handle
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_Handle(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000004)	//lastModify
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_DATE, 0)

		hr = pObject->get_lastModify(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000005)	//Position
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_R8, 0)

		hr = pObject->get_Position(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000006)	//Size
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_R8, 0)

		hr = pObject->get_Size(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000007)	//Type
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Type(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000008)	//Open
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I4, 0, 0)

		hr = pObject->Open(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000009)	//Read
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I4, 0, 1)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->Read(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000A)	//Write
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)

		hr = pObject->Write(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000B)	//ReadLine
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->ReadLine(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000C)	//WriteLine
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_BSTR, 0, 2)

		hr = pObject->WriteLine(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000D)	//WriteBlankLines
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I2, 0, 3)

		hr = pObject->WriteBlankLines(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000E)	//ReadRawLine
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->ReadRawLine(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000F)	//ReadText
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I4, 0, 4)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->ReadText(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000010)	//WriteText
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)

		hr = pObject->WriteText(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000011)	//ReadVariant
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I2, 0, 5)
		INVOKE_PARAM_RET(VT_VARIANT, 1)

		hr = pObject->ReadVariant(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000012)	//WriteVariant
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 6)

		hr = pObject->WriteVariant(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000013)	//ReadBoolean
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->ReadBoolean(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000014)	//WriteBoolean
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BOOL, 0)

		hr = pObject->WriteBoolean(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000015)	//ReadByte
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->ReadByte(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000016)	//WriteByte
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I2, 0)

		hr = pObject->WriteByte(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000017)	//ReadInt
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->ReadInt(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000018)	//WriteInt
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I2, 0)

		hr = pObject->WriteInt(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000019)	//ReadLong
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->ReadLong(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000001A)	//WriteLong
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)

		hr = pObject->WriteLong(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000001B)	//ReadFloat
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_R4, 0)

		hr = pObject->ReadFloat(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000001C)	//WriteFloat
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_R4, 0)

		hr = pObject->WriteFloat(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000001D)	//ReadDouble
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_R8, 0)

		hr = pObject->ReadDouble(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000001E)	//WriteDouble
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_R8, 0)

		hr = pObject->WriteDouble(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000001F)	//ReadDate
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_DATE, 0)

		hr = pObject->ReadDate(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000020)	//WriteDate
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_DATE, 0)

		hr = pObject->WriteDate(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000021)	//ReadObject
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I2, 0, 7)
		INVOKE_PARAM_RET(VT_DISPATCH, 1)

		hr = pObject->ReadObject(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000022)	//WriteObject
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_DISPATCH, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 8)

		hr = pObject->WriteObject(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000023)	//Close
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->Close();
		return hr;
	}

	IF_INVOKE_FUNC(0x00000024)	//Attach
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_UNKNOWN, 0)

		hr = pObject->Attach(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000025)	//CopyTo
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_DEF(VT_R8, 1, 9)

		hr = pObject->CopyTo(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000026)	//CopyFrom
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_DEF(VT_R8, 1, 10)

		hr = pObject->CopyFrom(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000027)	//setEOS
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->setEOS();
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IBaseStream>::g_typeinfo(__uuidof(IBaseStream), L"IBaseStream", s_mData, 42, _Invoke);

