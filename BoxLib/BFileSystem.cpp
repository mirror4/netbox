#include "StdAfx.h"
#include "BFileSystem.h"
#include "BPath.h"
#include "BSimpleDictionary.h"
#include <shlwapi.h>
#include <shellapi.h>

//	CBFileSystem::CBFileInfo::
STDMETHODIMP CBFileSystem::CBFileInfo::get_Path(BSTR *pbstrPath)
{
	*pbstrPath = m_strPath.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBFileSystem::CBFileInfo::get_Name(BSTR *pbstrName)
{
	*pbstrName = m_strName.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBFileSystem::CBFileInfo::get_AbsolutePath(BSTR *pbstrPath)
{
	*pbstrPath = m_strAbsolutePath.AllocSysString();
	return S_OK;
}

STDMETHODIMP CBFileSystem::CBFileInfo::get_Attributes(short *pfa)
{
	*pfa = m_nAttr;
	return S_OK;
}

STDMETHODIMP CBFileSystem::CBFileInfo::get_DateCreated(DATE *pdate)
{
	*pdate = m_dateCreated;
	return S_OK;
}

STDMETHODIMP CBFileSystem::CBFileInfo::get_DateLastModified(DATE *pdate)
{
	*pdate = m_dateLastModified;
	return S_OK;
}

STDMETHODIMP CBFileSystem::CBFileInfo::get_DateLastAccessed(DATE *pdate)
{
	*pdate = m_dateLastAccessed;
	return S_OK;
}

STDMETHODIMP CBFileSystem::CBFileInfo::get_Size(DOUBLE *pvarSize)
{
	*pvarSize = m_fSize;
	return S_OK;
}

STDMETHODIMP CBFileSystem::CBFileInfo::get_Type(BSTR *pbstrType)
{
	SHFILEINFOW fi;

	ZeroMemory(&fi,sizeof(fi));
	SHGetFileInfoW(m_strAbsolutePath, 0, &fi, sizeof(fi), SHGFI_TYPENAME);

	*pbstrType = ::SysAllocString(fi.szTypeName);

	return S_OK;
}

STDMETHODIMP CBFileSystem::CBFileInfo::get_ContentType(BSTR* pbstrType)
{
	if(!(m_nAttr & FILE_ATTRIBUTE_DIRECTORY))
	{
		LPWSTR pwszMimeType = 0;

		if (SUCCEEDED(FindMimeFromData(NULL, m_strAbsolutePath, NULL, 0, NULL, 0, &pwszMimeType, 0)))
		{
			*pbstrType = ::SysAllocString(pwszMimeType);
			CoTaskMemFree(pwszMimeType);
		}else *pbstrType = ::SysAllocString(L"application/octet-stream");
	}

	return S_OK;
}

HRESULT CBFileSystem::CBFileInfo::GetFileInfo(BSTR FilePath)
{
	CComBSTR bstr;
	HRESULT hr;

	hr = g_pFileSystem->GetFullPathName(FilePath, &bstr);
	if(FAILED(hr))return hr;

	m_strAbsolutePath = bstr;

	if(FilePath[0] == '\\' && FilePath[1] != '\\')
	{
		CBPath path(FilePath);

		path.RemoveBackslash();
		m_strPath = path;
	}else m_strPath = m_strAbsolutePath;

	if(IS_NT)
	{
		WIN32_FIND_DATAW fd;
		HANDLE hd;

		hd = ::FindFirstFileW(bstr, &fd);
		if(hd == INVALID_HANDLE_VALUE)
			return GetErrorResult();
		::FindClose(hd);

		FillData(&fd);
	}else
	{
		WIN32_FIND_DATAA fd;
		HANDLE hd;

		hd = ::FindFirstFileA(CBStringA(bstr), &fd);
		if(hd == INVALID_HANDLE_VALUE)
			return GetErrorResult();
		::FindClose(hd);

		FillData(&fd);
	}

	return S_OK;
}

//	CBFileSystem::CBFolder::
CBFileSystem::CBFolder::~CBFolder()
{
	if(m_hdNotify != INVALID_HANDLE_VALUE)
		FindCloseChangeNotification(m_hdNotify);
}

STDMETHODIMP CBFileSystem::CBFolder::get_Files(IStringList **ppfiles)
{
	if(!m_pFiles)
	{
		HRESULT hr;

		hr = GetFolderInfo();
		if(FAILED(hr))return hr;
	}

	return m_pFiles.QueryInterface(ppfiles);
}

STDMETHODIMP CBFileSystem::CBFolder::get_IsModified(VARIANT_BOOL* pVal)
{
	if(m_hdNotify == INVALID_HANDLE_VALUE)
		*pVal = VARIANT_FALSE;
	else *pVal = ::WaitForSingleObject(m_hdNotify, 0) == WAIT_OBJECT_0 ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CBFileSystem::CBFolder::get_SubFolders(IStringList **ppfolders)
{
	if(!m_pFolders)
	{
		HRESULT hr;

		hr = GetFolderInfo();
		if(FAILED(hr))return hr;
	}

	return m_pFolders.QueryInterface(ppfolders);
}

HRESULT CBFileSystem::CBFolder::GetFileInfo(BSTR FilePath)
{
	HRESULT hr = CBFileInfo::GetFileInfo(FilePath);

	if(m_strAbsolutePath.IsEmpty())
		return hr;

	m_hdNotify = IS_NT ? FindFirstChangeNotificationW(m_strAbsolutePath, TRUE, 0x1f) :
		m_hdNotify = FindFirstChangeNotificationA(CBStringA(m_strAbsolutePath), TRUE, 0x1f);

	return S_OK;
}

inline HANDLE FindFirst(CBString& strFileName, LPWIN32_FIND_DATAA lpFindFileData)
{
	return FindFirstFileA(CBStringA(strFileName) + "*.*", lpFindFileData);
}

inline HANDLE FindFirst(CBString& strFileName, LPWIN32_FIND_DATAW lpFindFileData)
{
	return FindFirstFileW(strFileName + L"*.*", lpFindFileData);
}

inline BOOL FindNext(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData)
{
	return FindNextFileA(hFindFile, lpFindFileData);
}

inline BOOL FindNext(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData)
{
	return FindNextFileW(hFindFile, lpFindFileData);
}

template <class T>
BOOL FillFiles(T *pfd, CBString& path, CBString& apath, CBList* pFiles, CBList* pFolders, CRBMap<CBKeyString, int>& mapItems)
{
	HANDLE hd;

	hd = ::FindFirst(apath, pfd);
	if(hd == INVALID_HANDLE_VALUE)
		return FALSE;

	BOOL bFindNext = TRUE;

	while(bFindNext)
	{
		CBString strName(pfd->cFileName);

		if(!mapItems.Lookup(strName))
		{
			mapItems.SetAt(strName, 0);
			if(pfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(pfd->cFileName[0] != '.')
				{
					CBComPtr<CBFileSystem::CBFolder> pFolder;
					CBDispatchPtr pDisp;

					pFolder.CreateInstance();
					pFolder->FillData(pfd);
					pFolder->m_strPath = path + strName;
					pFolder->m_strAbsolutePath = apath + strName;

					pDisp = pFolder;
					pFolders->AddValue(pDisp);
				}
			}else
			{
				CBComPtr<CBFileSystem::CBFileInfo> pFile;

				pFile.CreateInstance();
				pFile->FillData(pfd);
				pFile->m_strPath = path + strName;
				pFile->m_strAbsolutePath = apath + strName;

				pFiles->AddValue(pFile);
			}
		}

		bFindNext = FindNext(hd, pfd);
	}

	::FindClose(hd);

	return TRUE;
}

HRESULT CBFileSystem::CBFolder::GetFolderInfo()
{
	CBComPtr<CBList> pFiles;
	CBComPtr<CBList> pFolders;
	CBPath path(m_strPath);
	CBPath apath(m_strAbsolutePath);
	CRBMap<CBKeyString, int> mapItems;

	path.AddBackslash();
	apath.AddBackslash();

	pFiles.CreateInstance();
	pFolders.CreateInstance();

	if(IS_NT)
	{
		WIN32_FIND_DATAW fd;

		if(!FillFiles(&fd, path.m_strPath, apath.m_strPath, pFiles, pFolders, mapItems))
			return GetErrorResult();
	}else
	{
		WIN32_FIND_DATAA fd;

		if(!FillFiles(&fd, path.m_strPath, apath.m_strPath, pFiles, pFolders, mapItems))
			return GetErrorResult();
	}

	pFiles->Sort();
	pFolders->Sort();

	m_pFiles = pFiles;
	m_pFolders = pFolders;

	return S_OK;
}

class CBPackage : public CBObject
{
public:
	HRESULT Load(BSTR strPath)
	{

		return S_OK;
	}
};

static CBSimpleDictionary s_dictPackages;
static CBCriticalSection s_csFileSystem;
static CBString s_strBasePath;
static CBString s_strAppName;
static CBString s_strExeName;
CBComPtr<CBFileSystem> g_pFileSystem;

inline CBString MapLocalPath(BSTR strPath)
{
	CBPath path(s_strBasePath);

	if(strPath[0] == '\\' && strPath[1] != '\\')
	{
		CBPath path1(strPath);

		path.Append((LPCWSTR)path1 + 1);
	}else path.Append(strPath);

	path.RemoveBackslash();

	return path.m_strPath;
}

CBFileSystem::CBFileSystem()
{
	if(IS_NT)
	{
		WCHAR szModule[_MAX_PATH];

		if(::GetModuleFileNameW(NULL, szModule, _MAX_PATH))
			s_strExeName = szModule;
	}else
	{
		CHAR szModule[_MAX_PATH];

		if(::GetModuleFileNameA(NULL, szModule, _MAX_PATH))
			s_strExeName = szModule;
	}

	SetRuntimeFile(s_strExeName);
}

CBFileSystem::~CBFileSystem()
{
	s_strBasePath.Empty();
	s_strAppName.Empty();
	s_strExeName.Empty();

	s_dictPackages.RemoveAll();
}

STDMETHODIMP CBFileSystem::get_Packages(ISimpleDictionary **pVal)
{
	return s_dictPackages.QueryInterface(pVal);
}

STDMETHODIMP CBFileSystem::LoadPackage(BSTR Path, BSTR Package, IPackage** pVal)
{
	s_dictPackages.RemoveAll();

	return S_OK;
}

STDMETHODIMP CBFileSystem::RemovePackage(BSTR Path)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBFileSystem::BuildPath(BSTR Path, BSTR Name, BSTR *pbstrResult)
{
	CBPath path(Path);

	if(Name[0])path += Name;
	path.RemoveBackslash();

	*pbstrResult = path.AllocSysString();

	return S_OK;
}

STDMETHODIMP CBFileSystem::CopyFile(BSTR Source, BSTR Destination, VARIANT_BOOL OverWriteFiles)
{
	if(!(IS_NT ? ::CopyFileW(MapLocalPath(Source), MapLocalPath(Destination), OverWriteFiles == VARIANT_FALSE ? TRUE : FALSE) :
		::CopyFileA(CBStringA(MapLocalPath(Source)), CBStringA(MapLocalPath(Destination)), OverWriteFiles == VARIANT_FALSE ? TRUE : FALSE)))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBFileSystem::CreateFolder(BSTR FolderSpec)
{
	if(!(IS_NT ? ::CreateDirectoryW(MapLocalPath(FolderSpec), NULL) :
		::CreateDirectoryA(CBStringA(MapLocalPath(FolderSpec)), NULL)))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBFileSystem::DeleteFile(BSTR FileSpec, VARIANT_BOOL bForce)
{
	CBString strPath;

	strPath = MapLocalPath(FileSpec);

	if(!(IS_NT ? ::DeleteFileW(strPath) :
		::DeleteFileA(CBStringA(strPath))))
	{
		HRESULT hr = GetErrorResult();

		if(hr == E_ACCESSDENIED && bForce)
		{
			hr = SetFileAttributes(FileSpec, 0);
			if(FAILED(hr))return hr;

			if(!(IS_NT ? ::DeleteFileW(strPath) :
				::DeleteFileA(CBStringA(strPath))))
				return GetErrorResult();

			return S_OK;
		}

		return hr;
	}

	return S_OK;
}

STDMETHODIMP CBFileSystem::DeleteFolder(BSTR FolderSpec)
{
	if(!(IS_NT ? ::RemoveDirectoryW(MapLocalPath(FolderSpec)) :
		::RemoveDirectoryA(CBStringA(MapLocalPath(FolderSpec)))))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBFileSystem::FileExists(BSTR FileSpec, VARIANT_BOOL *pfExists)
{
	*pfExists = (IS_NT ? ::PathFileExistsW(MapLocalPath(FileSpec)) :
		::PathFileExistsA(CBStringA(MapLocalPath(FileSpec)))) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CBFileSystem::GetBaseName(BSTR Path, BSTR *pbstrResult)
{
	CBPath path(Path);

	*pbstrResult = path.GetBaseName().AllocSysString();

	return S_OK;
}

STDMETHODIMP CBFileSystem::GetExtensionName(BSTR Path, BSTR *pbstrResult)
{
	CBPath path(Path);

	*pbstrResult = ::SysAllocString(path.GetExtension());

	return S_OK;
}

STDMETHODIMP CBFileSystem::GetFile(BSTR FilePath, IFile **ppfile)
{
	CBComPtr<CBFileInfo> pFile;
	HRESULT hr;

	pFile.CreateInstance();
	hr = pFile->GetFileInfo(FilePath);
	if(FAILED(hr))return hr;

	*ppfile = pFile.Detach();

	return S_OK;
}

STDMETHODIMP CBFileSystem::GetFileName(BSTR Path, BSTR *pbstrResult)
{
	CBPath path(Path);

	*pbstrResult = ::SysAllocString(path.GetFileName());

	return S_OK;
}

STDMETHODIMP CBFileSystem::GetFolder(BSTR FolderPath, IFolder **ppfolder)
{
	CBComPtr<CBFolder> pFolder;
	HRESULT hr;

	pFolder.CreateInstance();
	hr = pFolder->GetFileInfo(FolderPath);
	if(FAILED(hr))return hr;

	*ppfolder = pFolder.Detach();

	return S_OK;
}

STDMETHODIMP CBFileSystem::GetFullPathName(BSTR Path, BSTR *pbstrResult)
{
	if(IS_NT)
	{
		WCHAR buffer[_MAX_PATH] = L"";
		WCHAR buffer1[_MAX_PATH] = L"";
		LPWSTR p;

		if(!::GetFullPathNameW(MapLocalPath(Path), _MAX_PATH, buffer, &p))
			return GetErrorResult();

		if(!::GetLongPathNameW(buffer, buffer1, _MAX_PATH))
			return GetErrorResult();

		*pbstrResult = ::SysAllocString(buffer1);
	}else
	{
		CHAR buffer[_MAX_PATH] = "";
		CHAR buffer1[_MAX_PATH] = "";
		LPSTR p;

		if(!::GetFullPathNameA(CBStringA(MapLocalPath(Path)), _MAX_PATH, buffer, &p))
			return GetErrorResult();

		if(!::GetLongPathNameA(buffer, buffer1, _MAX_PATH))
			return GetErrorResult();

		*pbstrResult = CBString(buffer1).AllocSysString();
	}

	return S_OK;
}

STDMETHODIMP CBFileSystem::GetParentFolderName(BSTR Path, BSTR *pbstrResult)
{
	CBPath path(Path);

	path.StripPath(TRUE);

	*pbstrResult = path.AllocSysString();

	return S_OK;
}

STDMETHODIMP CBFileSystem::GetTempName(BSTR Path, BSTR *pbstrResult)
{
	if(!Path[0])
	{
		char szTempPath[MAX_PATH];
		char szTempFile[MAX_PATH];

		::GetTempPathA(MAX_PATH, szTempPath);
		if(!::GetTempFileNameA(szTempPath, "~", 0, szTempFile))
		{
			::GetSystemDirectoryA(szTempPath, MAX_PATH);
			::GetTempFileNameA(szTempPath, "~", 0, szTempFile);
		}

		*pbstrResult = CBString(szTempFile).AllocSysString();
	}else
	{
		if(IS_NT)
		{
			WCHAR szTempFile[MAX_PATH];

			if(!::GetTempFileNameW(MapLocalPath(Path), L"~", 0, szTempFile))
				return GetErrorResult();
			*pbstrResult = CBString(szTempFile).AllocSysString();
		}else
		{
			char szTempFile[MAX_PATH];

			if(!::GetTempFileNameA(CBStringA(MapLocalPath(Path)), "~", 0, szTempFile))
				return GetErrorResult();
			*pbstrResult = CBString(szTempFile).AllocSysString();
		}
	}

	return S_OK;
}

STDMETHODIMP CBFileSystem::IsFolder(BSTR FolderSpec, VARIANT_BOOL *pfExists)
{
	*pfExists = (IS_NT ? ::PathIsDirectoryW(MapLocalPath(FolderSpec)) :
		::PathIsDirectoryA(CBStringA(MapLocalPath(FolderSpec)))) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CBFileSystem::MapPath(BSTR Path, BSTR* pbstrResult)
{
	*pbstrResult = MapLocalPath(Path).AllocSysString();
	return S_OK;
}

STDMETHODIMP CBFileSystem::MoveFile(BSTR Source, BSTR Destination, VARIANT_BOOL bUntilReboot)
{
	CBString strSource, strDest;

	strSource = MapLocalPath(Source);
	if (bUntilReboot && !SysStringLen(Destination))
	{
		if(IS_NT)
		{
			if(!MoveFileExW(strSource, NULL, MOVEFILE_DELAY_UNTIL_REBOOT))
				return GetErrorResult();
		}else
		{
			char Src[_MAX_PATH];

			if(!::GetShortPathNameA(CBStringA(Source), Src, sizeof(Src)))
				return GetErrorResult();

			if(!::WritePrivateProfileStringA("Rename", "NUL", Src, "WININIT.INI"))
				return GetErrorResult();
		}

		return S_OK;
	}

	strDest = MapLocalPath(Destination);

	if(!(IS_NT ? ::MoveFileW(strSource, strDest) :
		::MoveFileA(CBStringA(strSource), CBStringA(strDest))))
		if(!bUntilReboot)
			return GetErrorResult();
		else
		{
			if(IS_NT)
			{
				if(!MoveFileExW(strSource, strDest, MOVEFILE_DELAY_UNTIL_REBOOT))
					return GetErrorResult();
			}else
			{
				char Src[_MAX_PATH];

				if(!::GetShortPathNameA(CBStringA(Source), Src, sizeof(Src)))
					return GetErrorResult();

				if(!::WritePrivateProfileStringA("Rename", CBStringA(strDest), Src, "WININIT.INI"))
					return GetErrorResult();
			}
		}

	return S_OK;
}

STDMETHODIMP CBFileSystem::SetFileAttributes(BSTR Path, short attr)
{
	if(!(IS_NT ? ::SetFileAttributesW(MapLocalPath(Path), attr) : ::SetFileAttributesA(CBStringA(MapLocalPath(Path)), attr)))
		return GetErrorResult();

	return S_OK;
}

HRESULT CBFileSystem::CreateFile(BSTR bstrName, VARIANT_BOOL bOverwrite, IBaseStream **ppts)
{
	return E_NOTIMPL;
}

HRESULT CBFileSystem::OpenFile(BSTR bstrName, short nMode, short nShare, IBaseStream **ppts)
{
	return E_NOTIMPL;
}

HRESULT CBFileSystem::SetRuntimeFile(LPCWSTR pstrFile)
{
	CBString strPath;

	if(IS_NT)
	{
		WCHAR buffer[_MAX_PATH] = L"";
		WCHAR buffer1[_MAX_PATH] = L"";
		LPWSTR p;

		if(!::GetFullPathNameW(pstrFile, _MAX_PATH, buffer, &p))
			return GetErrorResult();

		if(!::GetLongPathNameW(buffer, buffer1, _MAX_PATH))
			return GetErrorResult();

		strPath = buffer1;
	}else
	{
		CHAR buffer[_MAX_PATH] = "";
		CHAR buffer1[_MAX_PATH] = "";
		LPSTR p;

		if(!::GetFullPathNameA(CBStringA(pstrFile), _MAX_PATH, buffer, &p))
			return GetErrorResult();

		if(!::GetLongPathNameA(buffer, buffer1, _MAX_PATH))
			return GetErrorResult();

		strPath = buffer1;
	}

	s_strAppName = strPath;
	s_strBasePath = s_strAppName.Left(s_strAppName.ReverseFind('\\') + 1);

	if(IS_NT)
		SetCurrentDirectoryW(s_strBasePath);
	else SetCurrentDirectoryA(CBStringA(s_strBasePath));

	return S_OK;
}

