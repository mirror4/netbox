#include "StdAfx.h"
#include "ti.h"

static PARAMDESCEX s_mElemDescEx[] = 
{
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x0\0" L""}},
	{0x18, {VT_BOOL, 0, 0, 0, -1}},
	{0x18, {VT_BOOL, 0, 0, 0, 0}},
	{0x18, {VT_BSTR, 0, 0, 0, 4 + (LONGLONG)L"\x0\0" L""}},
	{0x18, {VT_BOOL, 0, 0, 0, 0}},
	{0}
};

static ELEMDESC s_mElemDesc[] = 
{
	// LoadPackage
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// RemovePackage
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// BuildPath
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[0], 0x31}},
	// CopyFile
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BOOL}, {(ULONG_PTR)&s_mElemDescEx[1], 0x31}},
	// CreateFolder
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// DeleteFile
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BOOL}, {(ULONG_PTR)&s_mElemDescEx[2], 0x31}},
	// DeleteFolder
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// FileExists
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// GetBaseName
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// GetExtensionName
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// GetFile
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// GetFileName
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// GetFolder
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// GetFullPathName
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// GetParentFolderName
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// GetTempName
	{{{NULL}, VT_BSTR}, {(ULONG_PTR)&s_mElemDescEx[3], 0x31}},
	// IsFolder
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// MapPath
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	// MoveFile
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_BOOL}, {(ULONG_PTR)&s_mElemDescEx[4], 0x31}},
	// SetFileAttributes
	{{{NULL}, VT_BSTR}, {NULL, 0x1}},
	{{{NULL}, VT_I2}, {NULL, 0x1}},
	{0}
};

static CBTypeInfo::METHOD_ENTRY s_mData[] =
{
	{L"Packages", {0x00000001, NULL, NULL, FUNC_DISPATCH, INVOKE_PROPERTYGET, CC_STDCALL, 0, 0, 28, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"LoadPackage", {0x00000002, NULL, &s_mElemDesc[0], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 32, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"RemovePackage", {0x00000003, NULL, &s_mElemDesc[2], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 36, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"BuildPath", {0x00000004, NULL, &s_mElemDesc[3], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 40, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"CopyFile", {0x00000005, NULL, &s_mElemDesc[5], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 0, 44, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"CreateFolder", {0x00000006, NULL, &s_mElemDesc[8], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 48, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DeleteFile", {0x00000007, NULL, &s_mElemDesc[9], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 52, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"DeleteFolder", {0x00000008, NULL, &s_mElemDesc[11], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 56, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"FileExists", {0x00000009, NULL, &s_mElemDesc[12], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 60, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"GetBaseName", {0x0000000A, NULL, &s_mElemDesc[13], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 64, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"GetExtensionName", {0x0000000B, NULL, &s_mElemDesc[14], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 68, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"GetFile", {0x0000000C, NULL, &s_mElemDesc[15], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 72, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"GetFileName", {0x0000000D, NULL, &s_mElemDesc[16], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 76, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"GetFolder", {0x0000000E, NULL, &s_mElemDesc[17], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 80, 0, {{{NULL}, VT_DISPATCH}}, 0}},
	{L"GetFullPathName", {0x0000000F, NULL, &s_mElemDesc[18], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 84, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"GetParentFolderName", {0x00000010, NULL, &s_mElemDesc[19], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 88, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"GetTempName", {0x00000011, NULL, &s_mElemDesc[20], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 92, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"IsFolder", {0x00000012, NULL, &s_mElemDesc[21], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 96, 0, {{{NULL}, VT_BOOL}}, 0}},
	{L"MapPath", {0x00000013, NULL, &s_mElemDesc[22], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 1, 0, 100, 0, {{{NULL}, VT_BSTR}}, 0}},
	{L"MoveFile", {0x00000014, NULL, &s_mElemDesc[23], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 3, 0, 104, 0, {{{NULL}, VT_EMPTY}}, 0}},
	{L"SetFileAttributes", {0x00000015, NULL, &s_mElemDesc[26], FUNC_DISPATCH, INVOKE_FUNC, CC_STDCALL, 2, 0, 108, 0, {{{NULL}, VT_EMPTY}}, 0}}
};

static HRESULT _Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	IFileSystem* pObject = (IFileSystem*)pvInstance;
	HRESULT hr;
	UINT cArgs = pDispParams->cArgs;
	UINT cArgs1 = cArgs - pDispParams->cNamedArgs;

	hr;cArgs;cArgs1;

	IF_INVOKE(0x00000001)	//Packages
	{
		INVOKE_PARAM_RET(VT_DISPATCH, 0)

		hr = pObject->get_Packages(v0);

		INVOKE_DISP;
		return hr;
	}

	IF_INVOKE_FUNC(0x00000002)	//LoadPackage
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM(VT_BSTR, 1)
		INVOKE_PARAM_RET(VT_DISPATCH, 2)

		hr = pObject->LoadPackage(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000003)	//RemovePackage
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)

		hr = pObject->RemovePackage(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000004)	//BuildPath
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_BSTR, 1, 0)
		INVOKE_PARAM_RET(VT_BSTR, 2)

		hr = pObject->BuildPath(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000005)	//CopyFile
	{
		if(cArgs1 > 3)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM(VT_BSTR, 1)
		INVOKE_PARAM_DEF(VT_BOOL, 2, 1)

		hr = pObject->CopyFile(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000006)	//CreateFolder
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)

		hr = pObject->CreateFolder(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000007)	//DeleteFile
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_DEF(VT_BOOL, 1, 2)

		hr = pObject->DeleteFile(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000008)	//DeleteFolder
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)

		hr = pObject->DeleteFolder(v0);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000009)	//FileExists
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BOOL, 1)

		hr = pObject->FileExists(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000A)	//GetBaseName
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->GetBaseName(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000B)	//GetExtensionName
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->GetExtensionName(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000C)	//GetFile
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_DISPATCH, 1)

		hr = pObject->GetFile(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000D)	//GetFileName
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->GetFileName(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000E)	//GetFolder
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_DISPATCH, 1)

		hr = pObject->GetFolder(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x0000000F)	//GetFullPathName
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->GetFullPathName(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000010)	//GetParentFolderName
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->GetParentFolderName(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000011)	//GetTempName
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		INVOKE_PARAM_DEF(VT_BSTR, 0, 3)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->GetTempName(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000012)	//IsFolder
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BOOL, 1)

		hr = pObject->IsFolder(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000013)	//MapPath
	{
		if(cArgs1 > 1)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 1)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM_RET(VT_BSTR, 1)

		hr = pObject->MapPath(v0, v1);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000014)	//MoveFile
	{
		if(cArgs1 > 3)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM(VT_BSTR, 1)
		INVOKE_PARAM_DEF(VT_BOOL, 2, 4)

		hr = pObject->MoveFile(v0, v1, v2);
		return hr;
	}

	IF_INVOKE_FUNC(0x00000015)	//SetFileAttributes
	{
		if(cArgs1 > 2)
			return DISP_E_BADPARAMCOUNT;

		if(cArgs1 < 2)
			return DISP_E_PARAMNOTOPTIONAL;

		INVOKE_PARAM(VT_BSTR, 0)
		INVOKE_PARAM(VT_I2, 1)

		hr = pObject->SetFileAttributes(v0, v1);
		return hr;
	}

	return DISP_E_MEMBERNOTFOUND;
}

CBTypeInfo CBDispatch<IFileSystem>::g_typeinfo(__uuidof(IFileSystem), L"IFileSystem", s_mData, 21, _Invoke);

