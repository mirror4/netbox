#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0x18, {VT_I2, 0, 0, 0, 0}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// AllowClose
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Border
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Caption
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// DropShadow
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Height
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// Icon
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// LayeredAlpha
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// Left
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// MaximizeBox
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Maximized
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// MinimizeBox
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Minimized
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// ResizAble
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// ScrollBar
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Title
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// ToolWindow
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Top
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// TopMost
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Width
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// WindowEdge
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Visible
	{{{NULL}, VT_BOOL}, {NULL, 0x1}},
	// Drag
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// MoveTo
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// MoveWindow
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// ResizeTo
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	// EndDialog
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// SetTransparentMask
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[3], 0x31}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[4], 0x31}},
	// SetMinTrackSize
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[5], 0x31}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[6], 0x31}},
	// SetMaxTrackSize
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[7], 0x31}},
	{{{NULL}, VT_I2}, {(ULONG_PTR)&s_mElemDescEx[8], 0x31}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"AllowClose", {0x00000001, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Border", {0x00000002, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 40, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Caption", {0x00000003, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 48, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DropShadow", {0x00000006, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 64, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Height", {0x00000007, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 72, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Icon", {0x00000009, NULL, &s_mElemDesc[5], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 84, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"LayeredAlpha", {0x0000000A, NULL, &s_mElemDesc[6], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 92, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Left", {0x0000000B, NULL, &s_mElemDesc[7], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 100, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"MaximizeBox", {0x0000000C, NULL, &s_mElemDesc[8], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 108, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Maximized", {0x0000000D, NULL, &s_mElemDesc[9], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 116, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"MinimizeBox", {0x0000000E, NULL, &s_mElemDesc[10], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 124, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Minimized", {0x0000000F, NULL, &s_mElemDesc[11], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 132, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ResizAble", {0x00000010, NULL, &s_mElemDesc[12], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 140, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ScrollBar", {0x00000011, NULL, &s_mElemDesc[13], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 148, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Title", {0x00000012, NULL, &s_mElemDesc[14], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 156, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ToolWindow", {0x00000013, NULL, &s_mElemDesc[15], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 164, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Top", {0x00000014, NULL, &s_mElemDesc[16], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 172, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"TopMost", {0x00000015, NULL, &s_mElemDesc[17], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 180, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Width", {0x00000016, NULL, &s_mElemDesc[18], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 188, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"WindowEdge", {0x00000017, NULL, &s_mElemDesc[19], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 196, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Visible", {0x00000018, NULL, &s_mElemDesc[20], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 204, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"AllowClose", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Border", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Caption", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"clientHeight", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 52, 0, {{{NULL}, VT_I2}}, 0}},
	{L"clientWidth", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 56, 0, {{{NULL}, VT_I2}}, 0}},
	{L"DropShadow", {0x00000006, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 60, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Height", {0x00000007, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 68, 0, {{{NULL}, VT_I2}}, 0}},
	{L"HWND", {0x00000008, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 76, 0, {{{NULL}, VT_I4}}, 0}},
	{L"Icon", {0x00000009, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 80, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"LayeredAlpha", {0x0000000A, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 88, 0, {{{NULL}, VT_I2}}, 0}},
	{L"Left", {0x0000000B, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 96, 0, {{{NULL}, VT_I2}}, 0}},
	{L"MaximizeBox", {0x0000000C, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 104, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Maximized", {0x0000000D, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 112, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"MinimizeBox", {0x0000000E, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 120, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Minimized", {0x0000000F, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 128, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"ResizAble", {0x00000010, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 136, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"ScrollBar", {0x00000011, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 144, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Title", {0x00000012, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 152, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"ToolWindow", {0x00000013, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 160, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Top", {0x00000014, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 168, 0, {{{NULL}, VT_I2}}, 0}},
	{L"TopMost", {0x00000015, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 176, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Width", {0x00000016, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 184, 0, {{{NULL}, VT_I2}}, 0}},
	{L"WindowEdge", {0x00000017, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 192, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Visible", {0x00000018, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 200, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"Center", {0x00000019, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 208, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Close", {0x0000001A, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 212, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Drag", {0x0000001B, NULL, &s_mElemDesc[21], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 216, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Focus", {0x0000001C, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 220, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"isClosed", {0x0000001D, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 224, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"isActived", {0x0000001E, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 228, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"MoveTo", {0x0000001F, NULL, &s_mElemDesc[22], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 232, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"MoveWindow", {0x00000020, NULL, &s_mElemDesc[24], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 4, 0, 236, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ResizeTo", {0x00000021, NULL, &s_mElemDesc[28], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 240, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"ShowDialog", {0x00000022, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 244, 0, {{{NULL}, VT_VARIANT}}, 0}},
	{L"EndDialog", {0x00000023, NULL, &s_mElemDesc[30], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 1, 248, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"SetTransparentMask", {0x00000024, NULL, &s_mElemDesc[31], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 5, 0, 252, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"SetMinTrackSize", {0x00000025, NULL, &s_mElemDesc[36], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 256, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"SetMaxTrackSize", {0x00000026, NULL, &s_mElemDesc[38], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 260, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IWindow* pObject = (IWindow*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE_PROPERTYPUT(0x00000001)	//AllowClose
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_AllowClose(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000002)	//Border
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_Border(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000003)	//Caption
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_Caption(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000006)	//DropShadow
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_DropShadow(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000007)	//Height
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_Height(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000009)	//Icon
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BSTR, 0)

		hr = pObject->put_Icon(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000000A)	//LayeredAlpha
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_LayeredAlpha(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000000B)	//Left
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_Left(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000000C)	//MaximizeBox
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_MaximizeBox(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000000D)	//Maximized
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_Maximized(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000000E)	//MinimizeBox
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_MinimizeBox(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x0000000F)	//Minimized
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_Minimized(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000010)	//ResizAble
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_ResizAble(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000011)	//ScrollBar
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_ScrollBar(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000012)	//Title
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BSTR, 0)

		hr = pObject->put_Title(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000013)	//ToolWindow
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_ToolWindow(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000014)	//Top
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_Top(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000015)	//TopMost
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_TopMost(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000016)	//Width
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_I2, 0)

		hr = pObject->put_Width(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000017)	//WindowEdge
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_WindowEdge(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYPUT(0x00000018)	//Visible
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_NAMED_VALUE(VT_BOOL, 0)

		hr = pObject->put_Visible(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000001)	//AllowClose
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_AllowClose(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000002)	//Border
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_Border(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000003)	//Caption
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_Caption(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000004)	//clientHeight
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_clientHeight(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000005)	//clientWidth
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_clientWidth(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000006)	//DropShadow
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_DropShadow(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000007)	//Height
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Height(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000008)	//HWND
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I4, 0)

		hr = pObject->get_HWND(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000009)	//Icon
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Icon(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000A)	//LayeredAlpha
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_LayeredAlpha(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000B)	//Left
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Left(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000C)	//MaximizeBox
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_MaximizeBox(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000D)	//Maximized
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_Maximized(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000E)	//MinimizeBox
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_MinimizeBox(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x0000000F)	//Minimized
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_Minimized(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000010)	//ResizAble
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_ResizAble(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000011)	//ScrollBar
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_ScrollBar(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000012)	//Title
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BSTR, 0)

		hr = pObject->get_Title(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000013)	//ToolWindow
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_ToolWindow(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000014)	//Top
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Top(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000015)	//TopMost
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_TopMost(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000016)	//Width
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_I2, 0)

		hr = pObject->get_Width(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000017)	//WindowEdge
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_WindowEdge(v0);
		return hr;
	}

	IF_INVOKE_PROPERTYGET(0x00000018)	//Visible
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->get_Visible(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000019)	//Center
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->Center();
		return hr;
	}

	IF_INVOKE_FUNC(0x0000001A)	//Close
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->Close();
		return hr;
	}

	IF_INVOKE_FUNC(0x0000001B)	//Drag
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I2, 0, 0)

		hr = pObject->Drag(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000001C)	//Focus
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		hr = pObject->Focus();
		return hr;
	}

	IF_INVOKE_FUNC(0x0000001D)	//isClosed
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->isClosed(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000001E)	//isActived
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_BOOL, 0)

		hr = pObject->isActived(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000001F)	//MoveTo
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I2, 0)
		INVOKE_PARAM(VT_I2, 1)

		hr = pObject->MoveTo(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000020)	//MoveWindow
	{
		if(cArgs1 > 4)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 4)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I2, 0)
		INVOKE_PARAM(VT_I2, 1)
		INVOKE_PARAM(VT_I2, 2)
		INVOKE_PARAM(VT_I2, 3)

		hr = pObject->MoveWindow(v0, v1, v2, v3);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000021)	//ResizeTo
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_I2, 0)
		INVOKE_PARAM(VT_I2, 1)

		hr = pObject->ResizeTo(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000022)	//ShowDialog
	{
		if(cArgs1 > 0)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_RET(VT_VARIANT, 0)

		hr = pObject->ShowDialog(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000023)	//EndDialog
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_OPT(VT_VARIANT, 0)

		hr = pObject->EndDialog(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000024)	//SetTransparentMask
	{
		if(cArgs1 > 5)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_I2, 1, 1)
		INVOKE_PARAM_DEF(VT_I2, 2, 2)
		INVOKE_PARAM_DEF(VT_I2, 3, 3)
		INVOKE_PARAM_DEF(VT_I2, 4, 4)

		hr = pObject->SetTransparentMask(v0, v1, v2, v3, v4);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000025)	//SetMinTrackSize
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I2, 0, 5)
		INVOKE_PARAM_DEF(VT_I2, 1, 6)

		hr = pObject->SetMinTrackSize(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000026)	//SetMaxTrackSize
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_I2, 0, 7)
		INVOKE_PARAM_DEF(VT_I2, 1, 8)

		hr = pObject->SetMaxTrackSize(v0, v1);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IWindow>::g_typeinfo(__uuidof(IWindow), L"IWindow", s_mData, 59, _Invoke);

