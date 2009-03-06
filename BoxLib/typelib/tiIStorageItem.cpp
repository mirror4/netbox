#include "StdAfx.h"

static ELEMDESC s_mElemDesc[] = 
{
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Name", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"Type", {0x00000002, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 32, 0, {{{NULL}, VT_I2}}, 0}},
	{L"Size", {0x00000003, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 36, 0, {{{NULL}, VT_I4}}, 0}},
	{L"DateCreated", {0x00000004, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 40, 0, {{{NULL}, VT_DATE}}, 0}},
	{L"DateLastAccessed", {0x00000005, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 44, 0, {{{NULL}, VT_DATE}}, 0}},
	{L"DateLastModified", {0x00000006, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 48, 0, {{{NULL}, VT_DATE}}, 0}}
};

CBTypeInfo CBDispatch<IStorageItem>::g_typeinfo(__uuidof(IStorageItem), L"IStorageItem", s_mData, 6);

