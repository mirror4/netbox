#include "StdAfx.h"
#include "NetBox2.h"
#include "BoxFile.h"
#include <atlutil.h>
#include <BoxLib.h>
#include "BoxPath.h"

class CBoxFileStream : public CBClass<IStream>
{
public:
	CBoxFileStream() : m_pFile(NULL)
	{}

	~CBoxFileStream()
	{
		if(m_pFile)delete m_pFile;
	}

public:
	STDMETHOD(Read)(void *pDest, ULONG nMaxLen, ULONG *pnRead)
	{
		if(!m_pFile)return E_NOTIMPL;

		ULONG cb = m_pFile->Read(pDest, nMaxLen);

		if(pnRead)
			*pnRead = cb;

		if(cb == 0)return S_FALSE;

		return S_OK;
	}

	STDMETHOD(Write)(const void * pv, ULONG cb, ULONG * pcbWritten)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER * pLibNewPosition)
	{
		if(!m_pFile)return E_NOTIMPL;

		ULONGLONG pos = m_pFile->Seek(dlibMove.QuadPart, dwOrigin);

		if(pLibNewPosition)
			pLibNewPosition->QuadPart = pos;

		return S_OK;
	}

	HRESULT __stdcall SetSize(ULARGE_INTEGER /*libNewSize*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall CopyTo(IStream * /*pStream*/, ULARGE_INTEGER /*cb*/, ULARGE_INTEGER * /*pcbRead*/, ULARGE_INTEGER * /*pcbWritten*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Commit(DWORD /*grfCommitFlags*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Revert()
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall LockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall UnlockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Stat(STATSTG * pstatstg, DWORD grfStatFlag)
	{
		if(!m_pFile)return E_NOTIMPL;
		SYSTEMTIME st;

		ZeroMemory(pstatstg, sizeof(STATSTG));
		pstatstg->type = STGTY_STORAGE;

		pstatstg->cbSize.LowPart = (DWORD)m_pFile->GetLength();
		st = g_pFile->GetFileTime(m_pFile);
		SystemTimeToFileTime(&st, &pstatstg->mtime);

		return S_OK;
	}

	HRESULT __stdcall Clone(IStream ** /*ppstm*/)
	{
		return E_NOTIMPL;
	}

	HRESULT Open(LPCWSTR bstrFile)
	{
		CString str(bstrFile);
		CBoxPath path;

		LPCTSTR pstrExt = NULL;
		if(str.GetLength() > 4)
			pstrExt = (LPCTSTR)str + str.GetLength() - 4;

		path.Combine(_T("\\"), str);

		m_pFile = g_pFile->Open(path.m_strPath);

		if(!m_pFile || m_pFile == BOX_FOLDER)
		{
			m_pFile = NULL;

			return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		}

		if(m_pFile->IsKindOf(RUNTIME_CLASS(CMemFile)) && IsExeFile(pstrExt))
			return HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);

		return S_OK;
	}

private:
	CFile *m_pFile;
};

HRESULT GetBoxStream(LPCWSTR bstrFile, IStream **ppStream)
{
	CBComPtr<CBoxFileStream> pStream;
	HRESULT hr;

	pStream.CreateInstance();

	hr = pStream->Open(bstrFile);
	if(FAILED(hr))return hr;

	return pStream.QueryInterface(ppStream);
}

CBoxFile* g_pFile;
CString CBoxFile::m_strBasePath;
CString CBoxFile::m_strAppName;
CString CBoxFile::m_strExeName;
CString CBoxFile::m_strStartup;

CBoxFile::CBoxFile()
{
	if(m_strExeName.IsEmpty())
	{
		TCHAR szModule[_MAX_PATH];

		if(::GetModuleFileName(NULL, szModule, _MAX_PATH))
		{
			m_strExeName = szModule;
			SetRuntimeFile(szModule);
		}
	}
}

CBoxFile::~CBoxFile()
{
	m_strExeName.Empty();
	m_strAppName.Empty();
	m_strBasePath.Empty();
}

void CBoxFile::SetRuntimeFile(LPCTSTR pstrFile)
{
	TCHAR buffer[_MAX_PATH] = _T("");
	LPTSTR p;

	GetFullPathName(pstrFile, _MAX_PATH, buffer, &p);

	m_strAppName = buffer;
	m_strBasePath = m_strAppName.Left(m_strAppName.ReverseFind('\\') + 1);

	SetCurrentDirectory(m_strBasePath);
}

CFile* CBoxFile::Open(LPCTSTR pstrName)
{
	CFile* pFile = new CBoxSafeFile;
	CString strPath;

	if (theApp.m_pSystem == NULL)
		strPath = theApp.m_pSystem->m_pFolders->MapPath(pstrName);

	if(strPath.IsEmpty())
		strPath = m_strBasePath + (pstrName + 1);

	if(pFile->Open(strPath, CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary))
		return pFile;

	delete pFile;

	if(PathIsDirectory(strPath))
		return BOX_FOLDER;

	return NULL;
}

SYSTEMTIME CBoxFile::GetFileTime(CFile* pFile)
{
	SYSTEMTIME tms;
	FILETIME tm;

	::GetFileTime(pFile->m_hFile, NULL, NULL, &tm);
	FileTimeToSystemTime(&tm, &tms);
	tms.wMilliseconds = 0;
	tms.wDayOfWeek = 0;
	return tms;
}

void CBoxFile::GetFileTimeString(CFile* pFile, CString& strTime)
{
	SYSTEMTIME tms = GetFileTime(pFile);
	TimeToString(tms, strTime);
}
