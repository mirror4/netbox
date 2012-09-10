// Worker.cpp : implementation file
//

#include "stdafx.h"
#include <atlpath.h>
#include "NBR.h"
#include "Worker.h"
#include "LogDialog.h"

#include "ZipFile.h"

#define MK_9465 0x94659465

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char* memfind(const void *buf, size_t count, const char* str, size_t scount)
{
	const char *ptr;
	const char *ptr1;

	ptr = (const char*)buf;
	count -= scount - 1;

	while(count && (ptr1 = (const char*)memchr(ptr, str[0], count)))
	{
		if(!memcmp(str, ptr1, scount))
			return ptr1;

		count -= ptr1 - ptr + 1;
		ptr = ptr1 + 1;
	}

	return NULL;
}
// CWorker

CWorker::CWorker()
{
	m_bAutoDelete = FALSE;
}

BOOL CWorker::LogLastError(LPCTSTR pstrFile)
{
	LPTSTR lpMsgBuf;
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL ))
	{
		m_pwndLog->WriteLog(_T("\r\n") + GetMessage(L"file") + _T(": "));
		m_pwndLog->WriteLog(pstrFile);

		m_pwndLog->WriteLog(_T("\r\n") + GetMessage(L"error") + _T(": "));
		m_pwndLog->WriteLog(lpMsgBuf);
		LocalFree(lpMsgBuf);
	}

	return FALSE;
}

BOOL CWorker::ReplaceIcon(LPCTSTR pstrFile)
{
	CIconFile icon;
	CWin32Res res;
	CString str;
	CPath path;

	m_pwndMain->m_wndIcon.GetWindowText(str);
	if (str.IsEmpty())
		return true;
	path.m_strPath = m_strRootPath;
	path.Append(str);
	str = path.m_strPath;
	if (!icon.Open(str, CFile::modeRead))
		return LogLastError(str);

	UINT n = icon.GetCount();
	if (!n) return LogLastError(_T("Icon Error"));

	if (!res.Open(pstrFile, CFile::modeRead))
		return LogLastError(str);

	DWORD dwPos, dwSize, index;
	CAutoPtr<ICONDIR> pIcon;
	CAutoPtr<MEMICONDIR> pMemIcon;
	CAutoPtr<BYTE> pbmBits;

	if (!res.FindResource(_T("NETBOX"), RT_GROUP_ICON, 1033, dwPos, dwSize))
		return LogLastError(_T("Icon of NetBox has not found!"));

	pMemIcon.Attach((MEMICONDIR *)new BYTE[dwSize]);
	res.Seek(dwPos, CFile::begin);
	res.Read((LPVOID)pMemIcon, dwSize);

	int icons[3] = {0, 0, 0};
	for (UINT i=0;i<n;i++)
	{
		if (icon[i].bWidth == 16 && icon[i].bHeight == 16)
		{
			index = 0;
		}
		else if (icon[i].bWidth == 32 && icon[i].bHeight == 32)
		{
			index = 1;
		}
		else if (icon[i].bWidth == 48 && icon[i].bHeight == 48)
		{
			index = 2;
		}
		else continue;

		icons[index] = 1;
		if (res.FindResource(MAKEINTRESOURCE(pMemIcon->idEntries[index].nID), RT_ICON, 1033, dwPos, dwSize))
		{
			if (dwSize < icon[i].dwBytesInRes)
				continue;
			pbmBits.Attach(new BYTE[icon[i].dwBytesInRes]);
			icon.Seek(icon[i].dwImageOffset, CFile::begin);
			icon.Read((LPVOID)pbmBits, icon[i].dwBytesInRes);
			m_file.Seek(dwPos, CFile::begin);
			m_file.Write((LPVOID)pbmBits, icon[i].dwBytesInRes);
			pbmBits.Free();
		}
	}

	if (!res.FindResource(_T("NETBOX"), RT_GROUP_ICON, 1033, dwPos, dwSize))
		return LogLastError(_T("Icon of NetBox has not found!"));

	UINT nIcons = sizeof(icons)/sizeof(int);
	pMemIcon->idCount = 0;
	for (UINT i=0;i<nIcons;i++)
	{
		if (icons[i])
			pMemIcon->idCount++;
		else
			for (UINT j=i+1;j<nIcons;j++)
				pMemIcon->idEntries[j-1] = pMemIcon->idEntries[j];
	}
	m_file.Seek(dwPos, CFile::begin);
	m_file.Write((LPVOID)pMemIcon, dwSize);
	return true;
}

BOOL CWorker::ReplaceVersionInfo(LPCTSTR pstrFile)
{
	CWin32Res res;
	if (!res.Open(pstrFile, CFile::modeRead))
		return LogLastError(pstrFile);

	DWORD dwPos, dwSize;

	if (!res.FindResource(MAKEINTRESOURCE(1), RT_VERSION, 1033, dwPos, dwSize))
		return LogLastError(_T("VersionInfo of NetBox has not found!"));

	CAutoPtr<BYTE> pBits;

	pBits.Attach(new BYTE[dwSize]);
	res.Seek(dwPos, CFile::begin);
	res.Read((LPVOID)pBits, dwSize);

	CStringW strW, strRepW;
	CString str;
	LPCTSTR pstrfinds[] = {_T("FileDescription"), _T("FileVersion"), _T("ProductVersion"), _T("ProductName"), _T("LegalCopyright")};
	CEdit* ppwndedits[] = {&m_pwndMain->m_wndFileDescription, &m_pwndMain->m_wndFileVersion, &m_pwndMain->m_wndProductVersion, &m_pwndMain->m_wndProductName, &m_pwndMain->m_wndLegalCopyright};
	UINT pmaxlen[] = {128, 16, 16, 128, 128};
	BYTE pspace[256];

	memset(pspace, 0, sizeof(pspace));
	for (UINT i=0;i<5;i++)
	{
		ppwndedits[i]->GetWindowText(str);
		if (!str.IsEmpty())
		{
			strW = pstrfinds[i];
			strRepW = str;
			if (strRepW.GetLength()>pmaxlen[i])
				strRepW.Truncate(pmaxlen[i]);
			LPBYTE p = (LPBYTE)memfind(pBits, dwSize, (const char *)(LPCWSTR)strW, strW.GetLength()*2);
			if (p)
			{
				m_file.Seek(dwPos+(p-pBits.m_p)+strW.GetLength()*2+(strW.GetLength()%2?4:2), CFile::begin);
				m_file.Write((LPVOID)(LPCWSTR)strRepW, strRepW.GetLength()*2);
				m_file.Write((LPVOID)pspace, (pmaxlen[i]-strRepW.GetLength())*2);
			}
		}
	}
	return true;
}

BOOL CWorker::ReplaceManifest(LPCTSTR pstrFile)
{
	CWin32Res res;
	if (!res.Open(pstrFile, CFile::modeRead))
		return LogLastError(pstrFile);

	DWORD dwPos, dwSize;
	CAutoPtr<BYTE> pBits;
	CString str;
	
	m_pwndMain->m_wndPRI.GetWindowText(str);

	if (!res.FindResource(MAKEINTRESOURCE(1), RT_MANIFEST, 1033, dwPos, dwSize))
		return LogLastError(_T("Manifest of NetBox has not found!"));

	pBits.Attach(new BYTE[dwSize+1]);
	res.Seek(dwPos, CFile::begin);
	res.Read((LPVOID)pBits, dwSize);
	pBits.m_p[dwSize] = 0;
	
	if (!str.Compare(_T("asInvoker")))
	{
		LPBYTE p = (LPBYTE)strstr((char *)pBits.m_p, "requireAdministrator");
		if (p)
		{
			m_file.Seek(dwPos+(p-pBits.m_p), CFile::begin);
			m_file.Write("asInvoker\"           ", 21);
		}
	}
	else
	{
		LPBYTE p = (LPBYTE)strstr((char *)pBits.m_p, "asInvoker");
		if (p)
		{
			m_file.Seek(dwPos+(p-pBits.m_p), CFile::begin);
			m_file.Write("requireAdministrator\"", 21);
		}
	}
	return true;
}

BOOL CWorker::ReplaceAppdata(LPCTSTR pstrFile)
{
	CWin32Res res;
	if (!res.Open(pstrFile, CFile::modeRead))
		return LogLastError(pstrFile);

	DWORD dwPos, dwSize, dwAppdata;
	CString str;
	
	if (!m_pwndMain->IsDlgButtonChecked(IDC_EMBEDVBS))
		return true;

	if (!res.FindResource(MAKEINTRESOURCE(1), "APPDATA", 1033, dwPos, dwSize))
		return LogLastError(_T("Appdata of NetBox has not found!"));

	res.Seek(dwPos, CFile::begin);
	res.Read((LPVOID)&dwAppdata, sizeof(dwAppdata));
	dwAppdata |= 1;
	m_file.Seek(dwPos, CFile::begin);
	m_file.Write((LPVOID)&dwAppdata, sizeof(dwAppdata));
	return true;
}

BOOL CWorker::StartBuild()
{
	BYTE buffer[2048];
	UINT nSize;
	MD5_CTX m_ctxMD5;
	CString str;
	CPath path;

	m_strRootPath = m_pwndMain->m_strSrcFolder;
	m_pwndMain->m_wndOutput.GetWindowText(str);
	path.m_strPath = m_strRootPath;
	path.Append(str);
	str = path.m_strPath;
	m_pwndLog->WriteLog(GetMessage(L"outFile") + _T(": \""));
	m_pwndLog->WriteLog(str);
	if(!m_pwndLog->WriteLog(_T("\" ...\r\n\r\n")))
		return FALSE;

	if(!m_file.Open(str, CFile::shareDenyRead | CFile::modeReadWrite | CFile::typeBinary | CFile::modeCreate))
		return LogLastError(str);

	ZeroMemory(m_dwPackInfo, sizeof(m_dwPackInfo));
	MD5_Init(&m_ctxMD5);

	if(m_pwndMain->IsDlgButtonChecked(IDC_LIBRARY))
	{
		RAND_bytes(buffer, sizeof(buffer));
		m_file.Write(buffer, (*(WORD*)(buffer + sizeof(buffer) - 2)) % sizeof(buffer));
		m_file.SeekToBegin();
	}else
	{
		CFile fileNetBox;

		str = theApp.m_strBasePath + (m_pwndMain->IsDlgButtonChecked(IDC_APPLICATION)?_T("NetBox.exe"):_T("NetBoxCon.exe"));

		if(!fileNetBox.Open(str, CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary))
			return LogLastError(str);

		while(nSize = fileNetBox.Read(buffer, sizeof(buffer)))
			m_file.Write(buffer, nSize);

		fileNetBox.Close();

//		废弃NetBox用户签名证书
//		m_file.Seek(0x7c, 0);
//		m_file.Write(&theApp.m_certinfo.m_nDevID, 4);

//Replace Icon Now!
		
		if (!ReplaceIcon(str))
			return FALSE;

		if (!ReplaceVersionInfo(str))
			return FALSE;

		if (!ReplaceManifest(str))
			return FALSE;

		if (!ReplaceAppdata(str))
			return FALSE;

//End Replace
		m_file.SeekToBegin();

	//Code for checking signcode
		DWORD nSignAddress = 0, nChecksumAddress = 0;
		IMAGE_DOS_HEADER idh;
		IMAGE_NT_HEADERS inth;

		m_file.SeekToBegin();
		m_file.Read(&idh, sizeof(IMAGE_DOS_HEADER));
		if (idh.e_lfanew==0)
		{
			if(!m_pwndLog->WriteLog(_T("IMAGE_NT_HEADERS ERROR!\r\n\r\n")))
				return FALSE;
		}
		nChecksumAddress = idh.e_lfanew+((DWORD)(LPBYTE)&inth.OptionalHeader.CheckSum-(DWORD)(LPBYTE)&inth);
		nSignAddress = idh.e_lfanew+((DWORD)(LPBYTE)&inth.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY]-(DWORD)(LPBYTE)&inth);
	//End

		m_file.SeekToBegin();

	//Code for skip signcode
		DWORD nCount1 = nChecksumAddress;
		do
		{
			if(nCount1 > sizeof(buffer))
				nSize = m_file.Read(buffer, sizeof(buffer));
			else nSize = m_file.Read(buffer, nCount1);

			nCount1 -= nSize;

			MD5_Update(&m_ctxMD5, buffer, nSize);
		}while(nSize == sizeof(buffer));
		m_file.Seek(sizeof(DWORD), CFile::current);
		m_dwPackInfo[0]=nChecksumAddress+sizeof(DWORD);
		nCount1 = nSignAddress-m_dwPackInfo[0];
		do
		{
			if(nCount1 > sizeof(buffer))
				nSize = m_file.Read(buffer, sizeof(buffer));
			else nSize = m_file.Read(buffer, nCount1);

			nCount1 -= nSize;

			MD5_Update(&m_ctxMD5, buffer, nSize);
		}while(nSize == sizeof(buffer));
		m_file.Seek(sizeof(IMAGE_DATA_DIRECTORY), CFile::current);
		m_dwPackInfo[0]=nSignAddress+sizeof(IMAGE_DATA_DIRECTORY);
	//End
	}

	do
	{
		nSize = m_file.Read(buffer, sizeof(buffer));
		MD5_Update(&m_ctxMD5, buffer, nSize);
		m_dwPackInfo[0] += nSize;
	}while(nSize == sizeof(buffer));

	MD5_Final(m_MD5Pass, &m_ctxMD5);
//密码计算有可能受到代码签名时修改PE IMAGE_DATA_DIRECTORY CHECKSUM的问题，需要测试。
//测试完毕，代码签名会按照DWORD进行填充

	m_dwPackInfo[1] = 0;
	m_dwPackInfo[2] = MK_9465;

	RAND_bytes(buffer, MD5_DIGEST_LENGTH);
	m_file.Write(buffer, MD5_DIGEST_LENGTH);

	for(int i = 0; i < MD5_DIGEST_LENGTH; i ++)
		m_MD5Pass[i] ^= buffer[i];

	m_IndexFile.Write(buffer, 512);

/*	CFile file;

	str = theApp.m_strBasePath + _T("nbdw.lic");
	if(file.Open(str, CFile::modeRead | CFile::shareDenyNone))
	{
		char buffer[512];

		file.Read(buffer, 512);

		file.Close();

		m_IndexFile.Write(buffer, 512);
	}else return LogLastError(str);*/

	return TRUE;
}

BOOL CWorker::PackOneFile(CString strFileName)
{
	CString str;
	CStringW strFile(strFileName);

	DWORD dwPos = (DWORD)m_file.GetPosition();

	if(strFile[strFile.GetLength() - 1] == _T('\\'))
		m_strLogString += GetMessage(L"Folder") + _T(": ");
	else m_strLogString += GetMessage(L"file") + _T(": ");
	m_strLogString += strFile;
	m_strLogString += _T("\r\n");
	m_logCount ++;

	strFile = m_strRootPath + strFile;

	if(m_logCount > 3)
	{
		if(!m_pwndLog->WriteLog(m_strLogString))return FALSE;
		m_logCount = 0;
		m_strLogString.Empty();
	}

	if(strFile[strFile.GetLength() - 1] == _T('\\'))
	{
		strFile.ReleaseBuffer(strFile.GetLength() - 1);
		m_IndexFile.Write((LPCWSTR)strFile + m_strRootPath.GetLength() - 1,
					(strFile.GetLength() + 2 - m_strRootPath.GetLength()) * sizeof(WCHAR));

		dwPos = 0xFFFFFFFF;
		m_IndexFile.Write(&dwPos, sizeof(DWORD));
	}else
	{
		m_IndexFile.Write((LPCWSTR)strFile + m_strRootPath.GetLength() - 1,
					(strFile.GetLength() + 2 - m_strRootPath.GetLength()) * sizeof(WCHAR));
		m_IndexFile.Write(&dwPos, sizeof(DWORD));

		CZipFile zipFile;
		CFile f;

		zipFile.SetKey(m_MD5Pass);

		zipFile.Open(m_file, CFile::modeWrite);
		str = strFile;
		if(f.Open(str, CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary))
		{
			CFileStatus fs;
			CBDate d;

			f.GetStatus(fs);
			d = fs.m_mtime.GetTime();
			m_IndexFile.Write(&fs.m_size, sizeof(fs.m_size));
			m_IndexFile.Write(&d, sizeof(d));

			if(f.GetLength() > 4096)
			{
				if(!m_pwndLog->WriteLog(m_strLogString))return FALSE;
				m_logCount = 0;
				m_strLogString.Empty();
			}

			static char buf[10240];
			long n;

			while(n = f.Read(buf, sizeof(buf)))
			{
				zipFile.Write(buf, n);
				if(n = sizeof(buf) && !m_pwndLog->WriteLog(""))return FALSE;
			}
		}else return LogLastError(str);
	}

	return TRUE;
}

BOOL CWorker::PackFile()
{
	int n = m_pwndMain->m_wndSource.GetItemCount();
	CString strStartup;

	m_logCount = 0;

	if(m_pwndMain->IsDlgButtonChecked(IDC_APPLICATION))
	{
		m_pwndMain->m_wndStartup.GetWindowText(strStartup);
		if(!PackOneFile(strStartup))return FALSE;
	}

	for(int i = 0; i < n; i ++)
		if(m_pwndMain->m_wndSource.GetCheck(i))
		{
			CString str;

			str = m_pwndMain->m_wndSource.GetItemText(i, 0);
			if(m_pwndMain->m_wndSource.GetItemText(i, 1).IsEmpty())
				str.AppendChar('\\');

			if(strStartup.Compare(str))
				if(!PackOneFile(str))return FALSE;
		}

	if(!m_strLogString.IsEmpty() && !m_pwndLog->WriteLog(m_strLogString))return FALSE;

	return TRUE;
}

BOOL CWorker::EndBuild()
{
	if(!m_pwndLog->WriteLog(_T("\r\n") + GetMessage(L"index") + _T("...\r\n")))return FALSE;

	CZipFile zipFile;

	zipFile.SetKey(m_MD5Pass);

	m_IndexFile.Write(L"", sizeof(WCHAR));
	m_IndexFile.SeekToBegin();
	m_dwPackInfo[1] = (DWORD)m_file.GetPosition();

	zipFile.Open(m_file, CFile::modeWrite);

	char buf[1024];
	long n;

	while(n = m_IndexFile.Read(buf, sizeof(buf)))
		zipFile.Write(buf, n);

	zipFile.Close();

	DWORD dwLength = (DWORD)m_file.GetPosition();
	dwLength = (dwLength+sizeof(m_dwPackInfo)) % 16;
	if (dwLength)
	{
		dwLength = 16 - dwLength;
		memset(buf, 0, dwLength);
		m_file.Write(buf, dwLength);
	}

	m_file.Write(m_dwPackInfo, sizeof(m_dwPackInfo));

	//可以写 (DWORD)m_file.GetPosition() 去0x80处用于签名以后的NetBox寻找 m_dwPackInfo
	//m_file.Seek(0x80, CFile::begin);
	//m_file.Write(m_dwPackInfo, sizeof(m_dwPackInfo));
	//End

	m_file.Seek(m_dwPackInfo[0] + MD5_DIGEST_LENGTH, CFile::begin);

	MD5_CTX m_ctxMD5;
	int nSize;
	static char HEX[] = "0123456789ABCDEF";
	char str[41];
	int i, p;

	MD5_Init(&m_ctxMD5);
	do
	{
		nSize = m_file.Read(buf, sizeof(buf));
		MD5_Update(&m_ctxMD5, buf, nSize);
	}while(nSize == sizeof(buf));

	MD5_Final(m_MD5Pass, &m_ctxMD5);

	for(i = 0, p = 0; i < 16; i ++)
	{
		str[p ++] = HEX[m_MD5Pass[i] / 16];
		str[p ++] = HEX[m_MD5Pass[i] % 16];
		if((i == 4) || (i == 6) || (i == 8) || (i == 10))
			str[p ++] = '-';
	}
	str[p] = 0;

	m_file.Close();

	if(!m_pwndLog->WriteLog(_T("\r\nPROG_Info: ")))return FALSE;
	if(!m_pwndLog->WriteLog(str))return FALSE;
	if(!m_pwndLog->WriteLog(_T("\r\n")))return FALSE;

	return TRUE;
}

BOOL CWorker::DoBuild()
{
	if(!StartBuild())return FALSE;
	if(!PackFile())return FALSE;
	if(!EndBuild())return FALSE;

	return TRUE;
}

BOOL CWorker::InitInstance()
{
	Sleep(100);

	m_pwndLog->WriteLog(_T("------------------ ") + GetMessage(L"Starting") + _T(" ------------------\r\n\r\n"));

	if(!DoBuild())
		m_pwndLog->WriteLog(_T("\r\n---------------------- ") + GetMessage(L"optCancel") + _T(" ----------------------\r\n"));
	else
		m_pwndLog->WriteLog(_T("\r\n---------------------- ") + GetMessage(L"done") + _T(" ----------------------\r\n"));

	return FALSE;
}

int CWorker::ExitInstance()
{
	m_pwndLog->PostMessage(WM_COMMAND, IDOK);

	return FALSE;
}

// CWorker message handlers

void CWorker::StartWorker(CLogDialog * pLog)
{
	m_pwndMain = (CNBRDlg *)theApp.m_pMainWnd;
	m_pwndLog = pLog;
	CreateThread();
}
