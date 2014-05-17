#pragma once

#include <zlib\zlib.h>
#include <zlib\zutil.h>
#include <openssl\rc4.h>
#include <openssl\md5.h>

#ifndef Z_BUFSIZE
#define Z_BUFSIZE 16384
#endif

// CZip command target

class CZipFile : public CFile
{
public:
	CZipFile();

	virtual ~CZipFile();

	BOOL Open(CFile* pFile, UINT nOpenFlags);

	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual void Write(const void* lpBuf, UINT nCount);

	virtual void Close();

	void SetKey(const unsigned char *data)
	{
		RC4_set_key(&m_key, MD5_DIGEST_LENGTH, data);
		m_bHasRC4 = TRUE;
	}

private:
	void clear();
	UINT RC4Read(void* lpBuf, UINT nCount);
	void RC4Write(const void* lpBuf, UINT nCount);

	Byte m_Buffer[Z_BUFSIZE];
	z_stream stream;
	BOOL m_bEof;
	UINT m_nOpenFlags;

	CFile *m_pFile;

	RC4_KEY m_key;
	BOOL m_bHasRC4;
};
