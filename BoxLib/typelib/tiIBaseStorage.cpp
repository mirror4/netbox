#include "StdAfx.h"

static ELEMDESC s_mElemDesc[] = 
{
	// Create
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// Open
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// CreateFile
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// OpenFile
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// CreateFolder
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	// OpenFolder
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_VARIANT}, {NULL, 0x11}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Create", {0x00000001, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 1, 28, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Open", {0x00000002, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 1, 32, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"Close", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"CreateFile", {0x00000004, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 1, 40, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"OpenFile", {0x00000005, NULL, &s_mElemDesc[6], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 2, 44, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"CreateFolder", {0x00000006, NULL, &s_mElemDesc[9], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 1, 48, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"OpenFolder", {0x00000007, NULL, &s_mElemDesc[11], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 1, 52, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"_NewEnum", {0xFFFFFFFC, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 56, 0, {{{NULL}, VT_UNKNOWN}}, 1}}
};

CBTypeInfo CBDispatch<IBaseStorage>::g_typeinfo(__uuidof(IBaseStorage), L"IBaseStorage", s_mData, 8);

