#pragma once

#include "NBRDlg.h"
#include <openssl\md5.h>
#include <openssl\rand.h>

#pragma pack(push, 2) 
typedef struct
{
	BYTE	bWidth;               // Width of the image
	BYTE	bHeight;              // Height of the image (times 2)
	BYTE	bColorCount;          // Number of colors in image (0 if >=8bpp)
	BYTE	bReserved;            // Reserved
	WORD	wPlanes;              // Color Planes
	WORD	wBitCount;            // Bits per pixel
	DWORD	dwBytesInRes;         // how many bytes in this resource?
	WORD	nID;                  // the ID
} MEMICONDIRENTRY, *LPMEMICONDIRENTRY;
typedef struct 
{
	WORD			idReserved;   // Reserved
	WORD			idType;       // resource type (1 for icons)
	WORD			idCount;      // how many images?
	MEMICONDIRENTRY	idEntries[0]; // the entries for each image
} MEMICONDIR, *LPMEMICONDIR;

// These next two structs represent how the icon information is stored
// in an ICO file.
typedef struct
{
	BYTE	bWidth;               // Width of the image
	BYTE	bHeight;              // Height of the image (times 2)
	BYTE	bColorCount;          // Number of colors in image (0 if >=8bpp)
	BYTE	bReserved;            // Reserved
	WORD	wPlanes;              // Color Planes
	WORD	wBitCount;            // Bits per pixel
	DWORD	dwBytesInRes;         // how many bytes in this resource?
	DWORD	dwImageOffset;        // where in the file is this image
} ICONDIRENTRY, *LPICONDIRENTRY;
typedef struct 
{
	WORD			idReserved;   // Reserved
	WORD			idType;       // resource type (1 for icons)
	WORD			idCount;      // how many images?
	ICONDIRENTRY	idEntries[0]; // the entries for each image
} ICONDIR, *LPICONDIR;
#pragma pack(push) 

class CIconFile : public CFile
{
public:
	CIconFile(){}
	~CIconFile(){}

	BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL)
	{
		if (!CFile::Open(lpszFileName, nOpenFlags, pError))
			return false;
		
		ICONDIR idir;
		if (Read(&idir, sizeof(ICONDIR)) != sizeof(ICONDIR))
		{
			Close();
			return false;
		}
		
		UINT n = sizeof(ICONDIRENTRY) * idir.idCount;
		m_pBuf.Attach(new BYTE[n + sizeof(ICONDIR)]);
		memcpy(m_pBuf.m_p, (LPVOID)&idir, sizeof(ICONDIR));
		if (Read(m_pBuf.m_p + sizeof(ICONDIR), n) != n)
		{
			Close();
			m_pBuf.Free();
			return false;
		}
		
		m_pdir = (LPICONDIR)m_pBuf.m_p;
		return true;
	}

	UINT GetCount()
	{
		return m_pBuf?m_pdir->idCount:0;
	}

	ICONDIRENTRY& operator [](UINT n)
	{
		if (n>=0 && n<GetCount())
			return m_pdir->idEntries[n];
		AfxThrowMemoryException();
	}

private:
	LPICONDIR m_pdir;
	CAutoPtr<BYTE> m_pBuf;
};

class CWin32Res : public CFile
{
public:
	CWin32Res(){}
	~CWin32Res(){}

	PIMAGE_RESOURCE_DATA_ENTRY directoryFindEntry(PIMAGE_RESOURCE_DIRECTORY proot, DWORD dwoffset, int ipdepth, LPCTSTR lpName, LPCTSTR lpType, WORD wLanguage)
	{
		PIMAGE_RESOURCE_DIRECTORY pird;
		PIMAGE_RESOURCE_DIRECTORY_ENTRY pirde;
		BOOL bName;
		
		pird = (PIMAGE_RESOURCE_DIRECTORY)((BYTE *)proot+dwoffset);
		pirde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((BYTE *)pird+sizeof(IMAGE_RESOURCE_DIRECTORY));
		for (int i=0;i<pird->NumberOfNamedEntries+pird->NumberOfIdEntries;i++)
		{
			switch(ipdepth)
			{
				case 0:
					if (pirde[i].NameIsString && ((DWORD)lpType & 0xFFFF0000))
					{
						CStringW str, str1;

						str.Append((LPCWSTR)((BYTE *)proot+pirde[i].NameOffset+2), (WORD)*((BYTE *)proot+pirde[i].NameOffset));
						str1 = lpType;
						bName = !str.Compare(str1);
					}
					else
					{
						bName = (DWORD)pirde[i].Name==(DWORD)lpType;
					}
					break;
				case 1:
					if (pirde[i].NameIsString && ((DWORD)lpName & 0xFFFF0000))
					{
						CStringW str, str1;

						str.Append((LPCWSTR)((BYTE *)proot+pirde[i].NameOffset+2), (WORD)*((BYTE *)proot+pirde[i].NameOffset));
						str1 = lpName;
						bName = !str.Compare(str1);
					}
					else
					{
						bName = (DWORD)pirde[i].Name==(DWORD)lpName;
					}
					break;
				case 2:
					bName = pirde[i].Name==(DWORD)wLanguage;
					break;
				default:
					return NULL;
			}
			if (bName)
			{
				if (pirde[i].DataIsDirectory)
				{
					PIMAGE_RESOURCE_DATA_ENTRY p = directoryFindEntry(proot, pirde[i].OffsetToDirectory, ipdepth+1, lpName, lpType, wLanguage);
					if (p) return p;
				}
				else
				{
					return (PIMAGE_RESOURCE_DATA_ENTRY)((BYTE *)proot+pirde[i].OffsetToData);
				}
			}
		}
		return NULL;
	}

	BOOL FindResource(LPCTSTR lpName, LPCTSTR lpType, WORD wLanguage, DWORD &dwPosition, DWORD &dwSize)
	{
		PIMAGE_NT_HEADERS pinth;
		PIMAGE_SECTION_HEADER pish;
		PIMAGE_DOS_HEADER pidh;

		pidh = (PIMAGE_DOS_HEADER)m_pBuffer.m_p;
		pinth = (PIMAGE_NT_HEADERS)(m_pBuffer.m_p+pidh->e_lfanew);
		pish = IMAGE_FIRST_SECTION(pinth);
		PIMAGE_RESOURCE_DATA_ENTRY pirde;

		DWORD l = (DWORD)(LPBYTE)&(pinth->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY])-(DWORD)(LPBYTE)pidh;

		for (int i=0;i<pinth->FileHeader.NumberOfSections;i++)
		{
			if (!lstrcmpA((LPCSTR)pish[i].Name, ".rsrc"))
			{
				if (pirde = directoryFindEntry((PIMAGE_RESOURCE_DIRECTORY)(m_pBuffer+pish[i].PointerToRawData), 0, 0, lpName, lpType, wLanguage))
				{
					dwPosition = pish[i].PointerToRawData+pirde->OffsetToData-pish[i].VirtualAddress;
					dwSize = pirde->Size;
					return true;
				}
			}
		}

		return false;
	}

	BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL)
	{
		if (!CFile::Open(lpszFileName, nOpenFlags, pError))
			return false;

		UINT uLength = (UINT)GetLength();
		m_pBuffer.Attach(new BYTE[uLength]);
		if (Read(m_pBuffer, uLength) != uLength)
		{
			Close();
			return false;
		}

		PIMAGE_DOS_HEADER pidh;

		pidh = (PIMAGE_DOS_HEADER)m_pBuffer.m_p;
		if (pidh->e_lfanew==0)
		{
			Close();
			return false;
		}

		PIMAGE_NT_HEADERS pinth;

		pinth = (PIMAGE_NT_HEADERS)(m_pBuffer.m_p+pidh->e_lfanew);
		if (pinth->Signature!=0x00004550)
		{
			Close();
			return false;
		}
		
		if (pinth->FileHeader.Machine!=IMAGE_FILE_MACHINE_I386)
		{
			Close();
			return false;
		}

		return true;
	}

private:
	CAutoPtr<BYTE> m_pBuffer;
};

class CLogDialog;

// CWorker

class CWorker : public CWinThread
{
public:
	CWorker();

	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void StartWorker(CLogDialog * pLog);

private:
	BOOL StartBuild();
	BOOL PackFile();
	BOOL PackOneFile(CString strFileName);
	BOOL EndBuild();
	BOOL DoBuild();
	BOOL LogLastError(LPCTSTR pstrFile);

	BOOL CWorker::ReplaceIcon(LPCTSTR pstrFile);
	BOOL CWorker::ReplaceVersionInfo(LPCTSTR pstrFile);
	BOOL CWorker::ReplaceManifest(LPCTSTR pstrFile);
	BOOL CWorker::ReplaceAppdata(LPCTSTR pstrFile);

	CStringW m_strRootPath;
	CString m_strLogString;
	int m_logCount;

private:
	CLogDialog * m_pwndLog;
	CNBRDlg * m_pwndMain;

	CFile m_file;
	CMemFile m_IndexFile;
	DWORD m_dwPackInfo[3];
	BYTE m_MD5Pass[MD5_DIGEST_LENGTH];
};


