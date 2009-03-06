#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I2, 0, 0, 0, 1}},
	{0x18, {VT_I2, 0, 0, 0, 1}},
	{0x18, {VT_I2, 0, 0, 0, 1}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// Bpp
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// Quality
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// TransparentColor
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// FillColor
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// FillStyle
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// FontBold
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// FontBkColor
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// FontBkMode
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// FontColor
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// FontEscapement
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// FontItalic
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// FontName
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// FontSize
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// FontUnderline
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// PenColor
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// PenStyle
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// PenWidth
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// TextAlignment
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// Load
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// Save
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	// Create
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// Resample
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	// Crop
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// Flip
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[3], 0x31}},
	// Resize
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[4], 0x31}},
	// Rotate
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// GetPixelColor
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// SetPixelColor
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// DrawArc
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// DrawChord
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// DrawEllipse
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// DrawLine
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// DrawPie
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// DrawPolyBezier
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// DrawPolygon
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// DrawPolyline
	{{{NULL}, VT_VARIANT}, {NULL, 0x1}},
	// DrawRectangle
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	// DrawText
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// Paint
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_I4}, {NULL, 0x1}},
	{{{NULL}, VT_DISPATCH}, {NULL, 0x1}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Bpp", {0x00000008, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 60, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Quality", {0x00000009, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 68, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"TransparentColor", {0x0000000A, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 76, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"FillColor", {0x00000012, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 112, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"FillStyle", {0x00000013, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 120, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"FontBold", {0x00000014, NULL, &s_mElemDesc[5], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 128, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"FontBkColor", {0x00000015, NULL, &s_mElemDesc[6], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 136, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"FontBkMode", {0x00000016, NULL, &s_mElemDesc[7], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 144, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"FontColor", {0x00000017, NULL, &s_mElemDesc[8], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 152, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"FontEscapement", {0x00000018, NULL, &s_mElemDesc[9], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 160, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"FontItalic", {0x00000019, NULL, &s_mElemDesc[10], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 168, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"FontName", {0x0000001A, NULL, &s_mElemDesc[11], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 176, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"FontSize", {0x0000001B, NULL, &s_mElemDesc[12], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 184, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"FontUnderline", {0x0000001C, NULL, &s_mElemDesc[13], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 192, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"PenColor", {0x0000001D, NULL, &s_mElemDesc[14], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 200, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"PenStyle", {0x0000001E, NULL, &s_mElemDesc[15], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 208, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"PenWidth", {0x0000001F, NULL, &s_mElemDesc[16], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 216, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"TextAlignment", {0x00000020, NULL, &s_mElemDesc[17], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 224, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Type", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_I2}}, 0}},
	{L"Height", {0x00000006, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Width", {0x00000007, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Bpp", {0x00000008, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 56, 0, {{{NULL}, VT_I2}}, 0}},
	{L"Quality", {0x00000009, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 64, 0, {{{NULL}, VT_I2}}, 0}},
	{L"TransparentColor", {0x0000000A, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 72, 0, {{{NULL}, VT_I4}}, 0}},
	{L"FillColor", {0x00000012, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 108, 0, {{{NULL}, VT_I4}}, 0}},
	{L"FillStyle", {0x00000013, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 116, 0, {{{NULL}, VT_I2}}, 0}},
	{L"FontBold", {0x00000014, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 124, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"FontBkColor", {0x00000015, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 132, 0, {{{NULL}, VT_I4}}, 0}},
	{L"FontBkMode", {0x00000016, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 140, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"FontColor", {0x00000017, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 148, 0, {{{NULL}, VT_I4}}, 0}},
	{L"FontEscapement", {0x00000018, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 156, 0, {{{NULL}, VT_I4}}, 0}},
	{L"FontItalic", {0x00000019, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 164, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"FontName", {0x0000001A, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 172, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"FontSize", {0x0000001B, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 180, 0, {{{NULL}, VT_I2}}, 0}},
	{L"FontUnderline", {0x0000001C, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 188, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"PenColor", {0x0000001D, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 196, 0, {{{NULL}, VT_I4}}, 0}},
	{L"PenStyle", {0x0000001E, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 204, 0, {{{NULL}, VT_I2}}, 0}},
	{L"PenWidth", {0x0000001F, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 212, 0, {{{NULL}, VT_I2}}, 0}},
	{L"TextAlignment", {0x00000020, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 220, 0, {{{NULL}, VT_I2}}, 0}},
	{L"Load", {0x00000001, NULL, &s_mElemDesc[18], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 28, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Save", {0x00000002, NULL, &s_mElemDesc[20], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Create", {0x00000003, NULL, &s_mElemDesc[22], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 0, 36, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Clear", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"IsTransparent", {0x0000000B, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 80, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Resample", {0x0000000C, NULL, &s_mElemDesc[25], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 0, 84, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"Clone", {0x0000000D, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 88, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"Crop", {0x0000000E, NULL, &s_mElemDesc[28], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 4, 0, 92, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Flip", {0x0000000F, NULL, &s_mElemDesc[32], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 96, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Resize", {0x00000010, NULL, &s_mElemDesc[33], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 0, 100, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Rotate", {0x00000011, NULL, &s_mElemDesc[36], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 104, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"GetPixelColor", {0x00000021, NULL, &s_mElemDesc[37], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 228, 0, {{{NULL}, VT_I4}}, 0}},
	{L"SetPixelColor", {0x00000022, NULL, &s_mElemDesc[39], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 0, 232, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DrawArc", {0x00000023, NULL, &s_mElemDesc[42], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 6, 0, 236, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DrawChord", {0x00000024, NULL, &s_mElemDesc[48], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 6, 0, 240, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DrawEllipse", {0x00000025, NULL, &s_mElemDesc[54], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 4, 0, 244, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DrawLine", {0x00000026, NULL, &s_mElemDesc[58], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 4, 0, 248, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DrawPie", {0x00000027, NULL, &s_mElemDesc[62], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 6, 0, 252, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DrawPolyBezier", {0x00000028, NULL, &s_mElemDesc[68], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 256, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DrawPolygon", {0x00000029, NULL, &s_mElemDesc[69], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 260, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DrawPolyline", {0x0000002A, NULL, &s_mElemDesc[70], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 264, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DrawRectangle", {0x0000002B, NULL, &s_mElemDesc[71], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 4, 0, 268, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DrawText", {0x0000002C, NULL, &s_mElemDesc[75], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 0, 272, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Paint", {0x0000002D, NULL, &s_mElemDesc[78], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 0, 276, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IImage* pObject = (IImage*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUT(0x00000008)	//Bpp
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_Bpp(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000009)	//Quality
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_Quality(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000000A)	//TransparentColor
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_TransparentColor(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000012)	//FillColor
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_FillColor(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000013)	//FillStyle
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_FillStyle(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000014)	//FontBold
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_FontBold(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000015)	//FontBkColor
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_FontBkColor(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000016)	//FontBkMode
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_FontBkMode(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000017)	//FontColor
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_FontColor(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000018)	//FontEscapement
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_FontEscapement(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000019)	//FontItalic
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_FontItalic(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000001A)	//FontName
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BSTR, 0)

		hr = pObject->put_FontName(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000001B)	//FontSize
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_FontSize(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000001C)	//FontUnderline
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_FontUnderline(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000001D)	//PenColor
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I4, 0)

		hr = pObject->put_PenColor(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000001E)	//PenStyle
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_PenStyle(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000001F)	//PenWidth
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_PenWidth(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000020)	//TextAlignment
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_TextAlignment(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000005)	//Type
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Type(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000006)	//Height
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_Height(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000007)	//Width
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_Width(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000008)	//Bpp
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Bpp(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000009)	//Quality
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Quality(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000A)	//TransparentColor
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_TransparentColor(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000012)	//FillColor
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_FillColor(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000013)	//FillStyle
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_FillStyle(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000014)	//FontBold
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_FontBold(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000015)	//FontBkColor
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_FontBkColor(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000016)	//FontBkMode
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_FontBkMode(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000017)	//FontColor
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_FontColor(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000018)	//FontEscapement
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_FontEscapement(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000019)	//FontItalic
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_FontItalic(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000001A)	//FontName
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_FontName(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000001B)	//FontSize
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_FontSize(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000001C)	//FontUnderline
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_FontUnderline(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000001D)	//PenColor
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_PenColor(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000001E)	//PenStyle
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_PenStyle(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000001F)	//PenWidth
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_PenWidth(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000020)	//TextAlignment
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_TextAlignment(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000001)	//Load
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 0)

		hr = pObject->Load(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000002)	//Save
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 1)

		hr = pObject->Save(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000003)	//Create
	{
		if(cArgs1 > 3)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 3)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM(VT_I2, 2)

		hr = pObject->Create(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000004)	//Clear
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->Clear();
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000B)	//IsTransparent
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->IsTransparent(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000C)	//Resample
	{
		if(cArgs1 > 3)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM_DEF(VT_I2, 2, 2)
		INVOKE_PARAM_RET(VT_DISPATCH, 3)

		hr = pObject->Resample(v0, v1, v2, v3);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000D)	//Clone
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_DISPATCH, 0)

		hr = pObject->Clone(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000E)	//Crop
	{
		if(cArgs1 > 4)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 4)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM(VT_I4, 2)
		INVOKE_PARAM(VT_I4, 3)

		hr = pObject->Crop(v0, v1, v2, v3);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000F)	//Flip
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I2, 0, 3)

		hr = pObject->Flip(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000010)	//Resize
	{
		if(cArgs1 > 3)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM_DEF(VT_I2, 2, 4)

		hr = pObject->Resize(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000011)	//Rotate
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)

		hr = pObject->Rotate(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000021)	//GetPixelColor
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM_RET(VT_I4, 2)

		hr = pObject->GetPixelColor(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000022)	//SetPixelColor
	{
		if(cArgs1 > 3)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 3)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM(VT_I4, 2)

		hr = pObject->SetPixelColor(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000023)	//DrawArc
	{
		if(cArgs1 > 6)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 6)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM(VT_I4, 2)
		INVOKE_PARAM(VT_I4, 3)
		INVOKE_PARAM(VT_I4, 4)
		INVOKE_PARAM(VT_I4, 5)

		hr = pObject->DrawArc(v0, v1, v2, v3, v4, v5);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000024)	//DrawChord
	{
		if(cArgs1 > 6)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 6)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM(VT_I4, 2)
		INVOKE_PARAM(VT_I4, 3)
		INVOKE_PARAM(VT_I4, 4)
		INVOKE_PARAM(VT_I4, 5)

		hr = pObject->DrawChord(v0, v1, v2, v3, v4, v5);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000025)	//DrawEllipse
	{
		if(cArgs1 > 4)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 4)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM(VT_I4, 2)
		INVOKE_PARAM(VT_I4, 3)

		hr = pObject->DrawEllipse(v0, v1, v2, v3);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000026)	//DrawLine
	{
		if(cArgs1 > 4)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 4)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM(VT_I4, 2)
		INVOKE_PARAM(VT_I4, 3)

		hr = pObject->DrawLine(v0, v1, v2, v3);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000027)	//DrawPie
	{
		if(cArgs1 > 6)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 6)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM(VT_I4, 2)
		INVOKE_PARAM(VT_I4, 3)
		INVOKE_PARAM(VT_I4, 4)
		INVOKE_PARAM(VT_I4, 5)

		hr = pObject->DrawPie(v0, v1, v2, v3, v4, v5);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000028)	//DrawPolyBezier
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)

		hr = pObject->DrawPolyBezier(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000029)	//DrawPolygon
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)

		hr = pObject->DrawPolygon(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000002A)	//DrawPolyline
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_VARIANT, 0)

		hr = pObject->DrawPolyline(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000002B)	//DrawRectangle
	{
		if(cArgs1 > 4)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 4)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM(VT_I4, 2)
		INVOKE_PARAM(VT_I4, 3)

		hr = pObject->DrawRectangle(v0, v1, v2, v3);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000002C)	//DrawText
	{
		if(cArgs1 > 3)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 3)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM(VT_BSTR, 2)

		hr = pObject->DrawText(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000002D)	//Paint
	{
		if(cArgs1 > 3)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 3)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I4, 0)
		INVOKE_PARAM(VT_I4, 1)
		INVOKE_PARAM(VT_DISPATCH, 2)

		hr = pObject->Paint(v0, v1, v2);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IImage>::g_typeinfo(__uuidof(IImage), L"IImage", s_mData, 63, _Invoke);

