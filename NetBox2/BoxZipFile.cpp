#include "StdAfx.h"
#include "boxzipfile.h"
#include "ZipFile.h"
#include "BoxMemFile.h"
//#include "..\nbr\certinfo.h"
#include <BDate.h>

#define MK_9465 0x94659465
DWORD g_dwAppMark = 0;
static BYTE ZipKey[MD5_DIGEST_LENGTH];
BYTE APP_MD5[MD5_DIGEST_LENGTH];
BYTE PROG_MD5[MD5_DIGEST_LENGTH];
//int s_AppDevID, s_BadDevID;

static void msgVirus()
{
	::MessageBox(NULL, "Application was modified by a virus !!!", "Virus Alert", 0);
	::ExitProcess(0);
}

CBoxZipFile::CBoxZipFile(void)
{
	DWORD dwPackInfo[3];
	CBoxMemFile memFile;
//	CCertInfo cert;
	CBDate* pd;

	if(m_fileMe.Open(m_strExeName, CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary))
	{
		FILETIME tm;

		::GetFileTime(m_fileMe, NULL, NULL, &tm);
		FileTimeToSystemTime(&tm, &m_tm);
		m_tm.wMilliseconds = 0;
		m_tm.wDayOfWeek = 0;

//Code for checking signcode
		UINT nSize;
		char buf[1024];
		DWORD nCount = 0, nSignAddress = 0, nChecksumAddress = 0;
		IMAGE_DOS_HEADER idh;
		IMAGE_NT_HEADERS inth;

		m_fileMe.Read(&idh, sizeof(IMAGE_DOS_HEADER));
		if (idh.e_lfanew==0)
			return;
		nChecksumAddress = idh.e_lfanew+((DWORD)(LPBYTE)&inth.OptionalHeader.CheckSum-(DWORD)(LPBYTE)&inth);
		nSignAddress = idh.e_lfanew+((DWORD)(LPBYTE)&inth.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY]-(DWORD)(LPBYTE)&inth);
		m_fileMe.Seek(idh.e_lfanew, CFile::begin);
		m_fileMe.Read(&inth, sizeof(IMAGE_NT_HEADERS));
		if (inth.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress!=0)
		{
			nCount = (DWORD)m_fileMe.Seek((LONGLONG)(inth.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress-sizeof(dwPackInfo)), CFile::begin) + sizeof(dwPackInfo);
			m_fileMe.Read(dwPackInfo, sizeof(dwPackInfo));
		}
		if (nCount == 0)
		{
			nCount = (DWORD)m_fileMe.Seek(-(LONGLONG)sizeof(dwPackInfo), CFile::end) + sizeof(dwPackInfo);
			m_fileMe.Read(dwPackInfo, sizeof(dwPackInfo));
		}

//		DWORD nCount = (DWORD)m_fileMe.Seek(-(LONGLONG)sizeof(dwPackInfo), CFile::end) + sizeof(dwPackInfo);
//		m_fileMe.Read(dwPackInfo, sizeof(dwPackInfo));

		g_dwAppMark = dwPackInfo[2];
		if(g_dwAppMark == MK_9465)
			nCount = dwPackInfo[0];

		m_fileMe.SeekToBegin();

		MD5_CTX ctxMD5;

//Self-check
/*		m_fileMe.Seek(0x80, CFile::begin);
		m_fileMe.Read(PROG_MD5, sizeof(PROG_MD5));

		DWORD nCount1 = nCount - 0x90;

		MD5_Init(&ctxMD5);
		do
		{
			if(nCount1 > sizeof(buf))
				nSize = m_fileMe.Read(buf, sizeof(buf));
			else nSize = m_fileMe.Read(buf, nCount1);

			nCount1 -= nSize;

			MD5_Update(&ctxMD5, buf, nSize);
		}while(nSize == sizeof(buf));

		MD5_Final(APP_MD5, &ctxMD5);
*/
//		for(int i = 0; i < MD5_DIGEST_LENGTH; i ++)
//			if(PROG_MD5[i] != APP_MD5[i])
//			{
//				__asm
//				{
//					mov ecx, fs:[4]
//					sub ecx, 4
//					mov edi, esp
//					mov esp, ecx
//					sub ecx, edi
//					cld
//					rep stosb
//					xor ebp, ebp
//					mov esi, ebp
//					mov edi, ebp
//					jmp msgVirus
//				}
//			}

		m_fileMe.SeekToBegin();

		MD5_Init(&ctxMD5);
//Code for skip signcode
		DWORD nCount1 = nChecksumAddress;
		do
		{
			if(nCount1 > sizeof(buf))
				nSize = m_fileMe.Read(buf, sizeof(buf));
			else nSize = m_fileMe.Read(buf, nCount1);

			nCount1 -= nSize;

			MD5_Update(&ctxMD5, buf, nSize);
		}while(nSize == sizeof(buf));
		m_fileMe.Seek(sizeof(DWORD), CFile::current);
		nCount1 = nSignAddress-(nChecksumAddress+sizeof(DWORD));
		do
		{
			if(nCount1 > sizeof(buf))
				nSize = m_fileMe.Read(buf, sizeof(buf));
			else nSize = m_fileMe.Read(buf, nCount1);

			nCount1 -= nSize;

			MD5_Update(&ctxMD5, buf, nSize);
		}while(nSize == sizeof(buf));
		m_fileMe.Seek(sizeof(IMAGE_DATA_DIRECTORY), CFile::current);
		nCount-=nSignAddress+sizeof(IMAGE_DATA_DIRECTORY);
//End
		do
		{
			if(nCount > sizeof(buf))
				nSize = m_fileMe.Read(buf, sizeof(buf));
			else nSize = m_fileMe.Read(buf, nCount);

			nCount -= nSize;

			MD5_Update(&ctxMD5, buf, nSize);
		}while(nSize == sizeof(buf));

		MD5_Final(APP_MD5, &ctxMD5);

		if(g_dwAppMark != MK_9465)
			return;

		m_fileMe.Read(ZipKey, MD5_DIGEST_LENGTH);
		for(int i = 0; i < MD5_DIGEST_LENGTH; i ++)
			ZipKey[i] ^= APP_MD5[i] | 0;

		MD5_Init(&ctxMD5);
		do
		{
			nSize = m_fileMe.Read(buf, sizeof(buf));
			MD5_Update(&ctxMD5, buf, nSize);
		}while(nSize == sizeof(buf));

		MD5_Final(PROG_MD5, &ctxMD5);

		m_fileMe.Seek(dwPackInfo[1], CFile::begin);
		CZipFile zipFile;
		long n;

		zipFile.SetKey(ZipKey);

		zipFile.Open(&m_fileMe,CFile::modeRead);
		while(n = zipFile.Read(buf, sizeof(buf)))
			memFile.Write(buf, n);
		zipFile.Close();

		WCHAR* strBuffer;
		WCHAR* pstr;
		DWORD* pPos;

		nSize = (DWORD)memFile.GetLength();

		memFile.SeekToBegin();
		memFile.Read(buf, 512);

//		cert.SetLic(buf);

		strBuffer = new WCHAR[nSize / 2 - 256];
		if(strBuffer)
		{
			memFile.Read(strBuffer, nSize - 512);
			pstr = (WCHAR*)strBuffer;

			m_mapIndex.InitHashTable(3049);
			while(*pstr)
			{
				CString strTemp;
				pPos = (DWORD*)(pstr + wcslen(pstr) + 1);
				strTemp = pstr;
				if(m_strStartup.IsEmpty())m_strStartup = strTemp;
				m_mapIndex[strTemp] = (void*)pPos[0];

				if(*pPos != 0xFFFFFFFF)
				{
					pPos += 3;
					pd = (CBDate*)pPos;
//					cert.CheckDate(*pd);
					pstr = (WCHAR*)(pd + 1);
				}else pstr = (WCHAR*)(pPos + 1);
			}

			delete strBuffer;
		}
	}
	else
	{
		MessageBox(NULL, m_strExeName + " Access is denied.", "Error", MB_ICONSTOP | MB_OK);
		ExitProcess(0);
	}
/*
	if(s_AppDevID = cert.m_nDevID)
	{
		if(!cert.m_bValid)
			s_BadDevID = s_AppDevID;
		else if(cert.m_dateTimeout)
		{
			CBDate d;

			d.GetTime();
			if(d > cert.m_dateTimeout)
				s_BadDevID = s_AppDevID;
		}
	}else s_BadDevID = -1;*/
}

CFile* CBoxZipFile::Open(LPCTSTR pstrName)
{
	CString str;

	if(pstrName[0] != _T('\\'))
		str = _T('\\');
	str += pstrName;

	str.MakeLower();

	DWORD dwPos = 0;
	m_mapIndex.Lookup(str, *(void**)&dwPos);

	if(dwPos == 0)
		return CBoxFile::Open(pstrName);
	if(dwPos == 0xFFFFFFFF)
		return BOX_FOLDER;

	CBoxMemFile* pMemFile = new CBoxMemFile;

	CZipFile zipFile;
	char buf[1024];
	long n;

	zipFile.SetKey(ZipKey);

	CSingleLock l(&m_cs, TRUE);
	m_fileMe.Seek(dwPos, CFile::begin);
	zipFile.Open(&m_fileMe, CFile::modeRead);
	while(n = zipFile.Read(buf, sizeof(buf)))
		pMemFile->Write(buf, n);
	l.Unlock();

	pMemFile->SeekToBegin();

	if(pMemFile->GetLength() == 0)
	{
		pMemFile->SetLength(1);
		pMemFile->SetLength(0);
	}
	return pMemFile;
}

SYSTEMTIME CBoxZipFile::GetFileTime(CFile* pFile)
{
	if(pFile->IsKindOf(RUNTIME_CLASS(CMemFile)))
		return m_tm;

	return CBoxFile::GetFileTime(pFile);
}
