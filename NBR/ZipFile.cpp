// ZipFile.cpp : implementation file
//

#include "stdafx.h"
#include "ZipFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CZipFile

CZipFile::CZipFile()
{
	ZeroMemory(&stream, sizeof(stream));
	m_bEof = FALSE;
	ZeroMemory(&m_key, sizeof(m_key));
}

CZipFile::~CZipFile()
{
	if (m_hFile != INVALID_HANDLE_VALUE && !m_bCloseOnDelete)
		clear();
}

BOOL CZipFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError)
{
	int err;

	if(nOpenFlags & CFile::modeReadWrite)
		return FALSE;

	nOpenFlags |= CFile::typeBinary;
	m_nOpenFlags = nOpenFlags;

	if(!CFile::Open(lpszFileName, nOpenFlags, pError))
		return FALSE;

	if(nOpenFlags & CFile::modeWrite)
	{
		err = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
		stream.next_out = m_Buffer;
	}else
	{
		stream.next_in  = m_Buffer;

		err = inflateInit2(&stream, -MAX_WBITS);
	}
	stream.avail_out = Z_BUFSIZE;

	if (err != Z_OK)
	{
		CFile::Close();
		return FALSE;
	}

	return TRUE;
}

BOOL CZipFile::Open(HANDLE hFile, UINT nOpenFlags)
{
	int err;

	if(nOpenFlags & CFile::modeReadWrite)
		return FALSE;

	nOpenFlags |= CFile::typeBinary;
	m_nOpenFlags = nOpenFlags;

	if(nOpenFlags & CFile::modeWrite)
	{
		err = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
		stream.next_out = m_Buffer;
	}else
	{
		stream.next_in  = m_Buffer;

		err = inflateInit2(&stream, -MAX_WBITS);
	}
	stream.avail_out = Z_BUFSIZE;

	if (err != Z_OK)
		return FALSE;

	m_hFile = hFile;
	m_bCloseOnDelete = FALSE;

	return TRUE;
}

UINT CZipFile::Read(void* lpBuf, UINT nCount)
{
	if(m_bEof)
		return 0;

	stream.next_out = (Byte *)lpBuf;
	stream.avail_out = nCount;

	while (stream.avail_out != 0)
	{
		if (stream.avail_in == 0  && !m_bEof)
		{
			stream.avail_in = (uInt)RC4Read(m_Buffer, Z_BUFSIZE);
            if (stream.avail_in == 0)
				m_bEof = TRUE;
            stream.next_in = m_Buffer;
        }
		if (inflate(&stream, Z_NO_FLUSH) == Z_STREAM_END)
			break;

		if(m_bEof)break;
	}

	return nCount - stream.avail_out;
}

void CZipFile::Write(const void* lpBuf, UINT nCount)
{
	stream.next_in = (Bytef*)lpBuf;
	stream.avail_in = nCount;

	while (stream.avail_in != 0)
	{
		if (stream.avail_out == 0)
		{
			RC4Write(m_Buffer, Z_BUFSIZE);
			stream.next_out = m_Buffer;
			stream.avail_out = Z_BUFSIZE;
		}
		if (deflate(&stream, Z_NO_FLUSH) != Z_OK)
			CFileException::ThrowOsError((LONG)::GetLastError(), m_strFileName);
	}
}

void CZipFile::clear()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
		if(m_nOpenFlags & CFile::modeWrite)
		{
			uInt len;
			int done = 0;
			int z_err;

			stream.avail_in = 0; /* should be zero already anyway */

			for (;;)
			{
				len = Z_BUFSIZE - stream.avail_out;

				if (len != 0)
				{
					RC4Write(m_Buffer, len);
					stream.next_out = m_Buffer;
					stream.avail_out = Z_BUFSIZE;
				}
				if (done) break;
				z_err = deflate(&stream, Z_FINISH);

				if (len == 0 && z_err == Z_BUF_ERROR) z_err = Z_OK;

				done = (stream.avail_out != 0 || z_err == Z_STREAM_END);
		 
				if (z_err != Z_OK && z_err != Z_STREAM_END)
					break;
			}

			deflateEnd(&stream);

			done = 0;
			RC4Write(&done, 1);
		}else
			inflateEnd(&stream);

	CZipFile();
}

void CZipFile::Close()
{
	clear();
	if (m_bCloseOnDelete)
		CFile::Close();

	m_hFile = INVALID_HANDLE_VALUE;
}

UINT CZipFile::RC4Read(void* lpBuf, UINT nCount)
{
	UINT nSize = CFile::Read(lpBuf, nCount);

	RC4(&m_key, nSize, (BYTE *)lpBuf, (BYTE *)lpBuf);
	return nSize;
}

void CZipFile::RC4Write(const void* lpBuf, UINT nCount)
{
	RC4(&m_key, nCount, (BYTE *)lpBuf, (BYTE *)lpBuf);
	CFile::Write(lpBuf, nCount);
}

