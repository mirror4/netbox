#include "StdAfx.h"

static ELEMDESC s_mElemDesc[] = 
{
	// AutoCreateDirectory
	{{{NULL}, VT_BOOL}},
	// CreateDirectory
	{{{NULL}, VT_BSTR}},
	// CopyTo
	{{{NULL}, VT_BSTR}},
	{{{NULL}, VT_BSTR}},
	// MoveTo
	{{{NULL}, VT_BSTR}},
	{{{NULL}, VT_BSTR}},
	// Link
	{{{NULL}, VT_BSTR}},
	{{{NULL}, VT_BSTR}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"AutoCreateDirectory", {0x00000064, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 0, 0, {{{NULL}, VT_BOOL}}}, VT_BYREF | VT_BOOL},
	{L"AutoCreateDirectory", {0x00000064, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_PROPERTYPUT, CC_STDCALL, 1, 0, 0, 0, {{{NULL}, VT_EMPTY}}}, VT_BOOL},
	{L"CreateDirectory", {0x00000065, NULL, &s_mElemDesc[1], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 0, 0, {{{NULL}, VT_EMPTY}}}, VT_BSTR},
	{L"CopyTo", {0x00000066, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 0, 0, {{{NULL}, VT_EMPTY}}}, VT_BSTR, VT_BSTR},
	{L"MoveTo", {0x00000067, NULL, &s_mElemDesc[4], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 0, 0, {{{NULL}, VT_EMPTY}}}, VT_BSTR, VT_BSTR},
	{L"Link", {0x00000068, NULL, &s_mElemDesc[6], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 0, 0, {{{NULL}, VT_EMPTY}}}, VT_BSTR, VT_BSTR}
};

CBTypeInfo CBDispatch<IVariantDirectory>::g_typeinfo(__uuidof(IVariantDirectory), L"IVariantDirectory", s_mData, 6);

