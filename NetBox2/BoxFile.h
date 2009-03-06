#pragma once

#define	BOX_FOLDER		(CFile*)-1

class CBoxFile
{
public:
	CBoxFile();
	virtual ~CBoxFile();

public:
	static CString m_strBasePath;
	static CString m_strAppName;
	static CString m_strExeName;
	static CString m_strStartup;

public:
	void SetRuntimeFile(LPCTSTR pstrFile);

public:
	virtual CFile* Open(LPCTSTR pstrName);
	virtual SYSTEMTIME GetFileTime(CFile* pFile);
	virtual void GetFileTimeString(CFile* pFile, CString& strTime);
};

extern CBoxFile* g_pFile;

class CBoxSafeFile : public CFile
{
public:
	virtual ULONGLONG Seek(LONGLONG lOff, UINT nFrom)
	{
		try
		{
			return CFile::Seek(lOff, nFrom);
		}catch(CException* e)
		{
			e->Delete();
		}

		return 0;
	}

	virtual ULONGLONG GetLength() const
	{
		try
		{
			return CFile::GetLength();
		}catch(CException* e)
		{
			e->Delete();
		}

		return 0;
	}

	virtual UINT Read(void* lpBuf, UINT nCount)
	{
		try
		{
			return CFile::Read(lpBuf, nCount);
		}catch(CException* e)
		{
			e->Delete();
		}

		return 0;
	}

	virtual void Write(const void* lpBuf, UINT nCount)
	{
		try
		{
			CFile::Write(lpBuf, nCount);
		}catch(CException* e)
		{
			e->Delete();
		}
	}

	virtual void Close()
	{
		try
		{
			CFile::Close();
		}catch(CException* e)
		{
			e->Delete();
		}
	}
};

