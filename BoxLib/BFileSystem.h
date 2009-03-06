#pragma once

#include "BFile.h"
#include "BDate.h"
#include "BList.h"

class CBFileSystem : public CBDispatch<IFileSystem>
{
public:
	class __declspec(uuid("94650000-0000-0000-4669-6C65496E666F"))
	CBFileInfo : public CBDispatch<IFile>
	{
		DECLARE_CLASS(CBFile)

	public:
		CBFileInfo(void) : m_nAttr(0), m_fSize(0)
		{}

	public:
		//	IFile
		STDMETHOD(get_Path)(BSTR *pbstrPath);
		STDMETHOD(get_Name)(BSTR *pbstrName);
		STDMETHOD(get_AbsolutePath)(BSTR *pbstrPath);
		STDMETHOD(get_Attributes)(short *pfa);
		STDMETHOD(get_DateCreated)(DATE *pdate);
		STDMETHOD(get_DateLastModified)(DATE *pdate);
		STDMETHOD(get_DateLastAccessed)(DATE *pdate);
		STDMETHOD(get_Size)(DOUBLE *pvarSize);
		STDMETHOD(get_Type)(BSTR *pbstrType);
		STDMETHOD(get_ContentType)(BSTR* pbstrType);

	public:
		template <class T>
		void FillData(T *pfd)
		{
			m_strName = pfd->cFileName;
			m_nAttr = (BYTE)pfd->dwFileAttributes;

			m_dateCreated = pfd->ftCreationTime;
			m_dateLastModified = pfd->ftLastWriteTime;
			m_dateLastAccessed = pfd->ftLastAccessTime;

			m_fSize = (pfd->nFileSizeHigh << 32) + pfd->nFileSizeLow;
		}

		HRESULT GetFileInfo(BSTR FilePath);

	public:
		CBString m_strAbsolutePath;
		CBString m_strPath;
		CBString m_strName;
		short m_nAttr;
		CBDate m_dateCreated;
		CBDate m_dateLastModified;
		CBDate m_dateLastAccessed;
		DOUBLE m_fSize;
	};

	class __declspec(uuid("94650000-0000-0000-0000-466F6C646572"))
	CBFolder : public CBDispatch<IFolder, CBFileInfo>
	{
		DECLARE_CLASS_EX(CBFolder, CBFileInfo)

	public:
		CBFolder() : m_hdNotify(INVALID_HANDLE_VALUE)
		{}

		~CBFolder();

	public:
		//	IFolder
		STDMETHOD(get_Files)(IStringList **ppfiles);
		STDMETHOD(get_IsModified)(VARIANT_BOOL* pVal);
		STDMETHOD(get_SubFolders)(IStringList **ppfolders);

	public:
		HRESULT GetFileInfo(BSTR FilePath);
		HRESULT GetFolderInfo();

	public:
		CBComPtr<CBList> m_pFiles;
		CBComPtr<CBList> m_pFolders;
		HANDLE m_hdNotify;
	};

public:
	CBFileSystem();
	~CBFileSystem();

public:
	//	IFileSystem
	STDMETHOD(get_Packages)(ISimpleDictionary **pVal);
	STDMETHOD(LoadPackage)(BSTR Path, BSTR Package, IPackage** pVal);
	STDMETHOD(RemovePackage)(BSTR Path);
	STDMETHOD(BuildPath)(BSTR Path, BSTR Name, BSTR *pbstrResult);
	STDMETHOD(CopyFile)(BSTR Source, BSTR Destination, VARIANT_BOOL OverWriteFiles = -1);
	STDMETHOD(CreateFolder)(BSTR FolderSpec);
	STDMETHOD(DeleteFile)(BSTR FileSpec, VARIANT_BOOL bForce = 0);
	STDMETHOD(DeleteFolder)(BSTR FolderSpec);
	STDMETHOD(FileExists)(BSTR FileSpec, VARIANT_BOOL *pfExists);
	STDMETHOD(GetBaseName)(BSTR Path, BSTR *pbstrResult);
	STDMETHOD(GetExtensionName)(BSTR Path, BSTR *pbstrResult);
	STDMETHOD(GetFile)(BSTR FilePath, IFile **ppfile);
	STDMETHOD(GetFileName)(BSTR Path, BSTR *pbstrResult);
	STDMETHOD(GetFolder)(BSTR FolderPath, IFolder **ppfolder);
	STDMETHOD(GetFullPathName)(BSTR Path, BSTR *pbstrResult);
	STDMETHOD(GetParentFolderName)(BSTR Path, BSTR *pbstrResult);
    STDMETHOD(GetTempName)(BSTR Path, BSTR* pbstrResult);
	STDMETHOD(IsFolder)(BSTR FolderSpec, VARIANT_BOOL *pfExists);
	STDMETHOD(MapPath)(BSTR Path, BSTR* pbstrResult);
	STDMETHOD(MoveFile)(BSTR Source, BSTR Destination, VARIANT_BOOL bUntilReboot = 0);
	STDMETHOD(SetFileAttributes)(BSTR Path, short attr);

public:
	static HRESULT CreateFile(BSTR bstrName, VARIANT_BOOL bOverwrite, IBaseStream **ppts);
	static HRESULT OpenFile(BSTR bstrName, short nMode, short nShare, IBaseStream **ppts);
	HRESULT SetRuntimeFile(LPCWSTR pstrFile);
};

extern CBComPtr<CBFileSystem> g_pFileSystem;

