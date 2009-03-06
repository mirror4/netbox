#pragma once

#include "BoxMemFile.h"

class CBoxMemReadStream : public IStream
{
public:
	CBoxMemReadStream(LPBYTE pData, DWORD dwSize)
	{
		SetData(pData, dwSize);
	}

	void SetData(LPBYTE pData, DWORD dwSize)
	{
		m_pData = pData;
		m_dwSize = dwSize;
		m_dwBytesRead = 0;
	}

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv)
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_IStream) ||
			InlineIsEqualGUID(riid, IID_ISequentialStream))
		{
			*ppv = static_cast<IStream *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef()
	{
		return 1;
	}

	ULONG __stdcall Release()
	{
		return 1;
	}

	HRESULT __stdcall Read(void *pDest, ULONG nMaxLen, ULONG *pnRead)
	{
		DWORD dwToRead = min((m_dwSize - m_dwBytesRead), nMaxLen);

		memcpy(pDest, m_pData + m_dwBytesRead, dwToRead);
		m_dwBytesRead += dwToRead;
		if(pnRead != NULL)
			*pnRead = dwToRead;

		return S_OK;
	}

	HRESULT __stdcall Write(const void * /*pv*/, ULONG /*cb*/, ULONG * /*pcbWritten*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER * pLibNewPosition)
	{
		if(dwOrigin == STREAM_SEEK_CUR)
			dlibMove.QuadPart += m_dwBytesRead;
		else if(dwOrigin == STREAM_SEEK_END)
			dlibMove.QuadPart += m_dwSize;

		m_dwBytesRead = (DWORD)dlibMove.QuadPart;
		if(m_dwBytesRead > m_dwSize)
			m_dwBytesRead = m_dwSize;

		if(pLibNewPosition)
			pLibNewPosition->QuadPart = m_dwBytesRead;
		return S_OK;
	}

	HRESULT __stdcall SetSize(ULARGE_INTEGER /*libNewSize*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall CopyTo(IStream * /*pStream*/, ULARGE_INTEGER /*cb*/, 
		ULARGE_INTEGER * /*pcbRead*/, ULARGE_INTEGER * /*pcbWritten*/)
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

	HRESULT __stdcall Stat(STATSTG * /*pstatstg*/, DWORD /*grfStatFlag*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Clone(IStream ** /*ppstm*/)
	{
		return E_NOTIMPL;
	}

private:
	LPBYTE	m_pData;
	DWORD	m_dwSize;
	DWORD	m_dwBytesRead;
};

class CBoxMemWriteStream : public IStream
{
public:
	LPBYTE GetData(DWORD& dwSize)
	{
		LPBYTE bufptr, bufptr1;

		dwSize = (DWORD)m_file.GetLength();

		m_file.SeekToBegin();
		m_file.GetBufferPtr(CFile::bufferRead, 0, (void**)&bufptr, (void**)&bufptr1);
		return bufptr;
	}

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv)
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_IStream) ||
			InlineIsEqualGUID(riid, IID_ISequentialStream))
		{
			*ppv = static_cast<IStream *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef()
	{
		return 1;
	}

	ULONG __stdcall Release()
	{
		return 1;
	}

	HRESULT __stdcall Read(void *pDest, ULONG nMaxLen, ULONG *pnRead)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Write(const void * pv, ULONG cb, ULONG * pcbWritten)
	{
		m_file.Write(pv, cb);
		if(pcbWritten != NULL)
			*pcbWritten = cb;
		return S_OK;
	}

	HRESULT __stdcall Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER * pLibNewPosition)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall SetSize(ULARGE_INTEGER /*libNewSize*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall CopyTo(IStream * /*pStream*/, ULARGE_INTEGER /*cb*/, 
		ULARGE_INTEGER * /*pcbRead*/, ULARGE_INTEGER * /*pcbWritten*/)
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

	HRESULT __stdcall Stat(STATSTG * /*pstatstg*/, DWORD /*grfStatFlag*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Clone(IStream ** /*ppstm*/)
	{
		return E_NOTIMPL;
	}

private:
	CBoxMemFile m_file;
};
