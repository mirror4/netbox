#include "StdAfx.h"
#include "BFile.h"
#include "BVarType.h"
#include "BVariant.h"
#include "BMStream.h"

HRESULT CBStream::GetStream(VARIANT* pvar, IStream** pStm, BOOL bOpen)
{
	if(pvar->vt == VT_BSTR)
	{
		UINT len = ::SysStringByteLen(pvar->bstrVal);
		BOOL bIsBin = FALSE;
		BYTE* pData = (BYTE*)pvar->bstrVal;

		if(len & 1 || len == 0)
			bIsBin = TRUE;
		else
		{
			len /= 2;
			if(len > MAX_PATH ||
				pvar->bstrVal[len - 1] == 0xffff ||
				pvar->bstrVal[0] == ' ')
				bIsBin = TRUE;
			else
			{
				UINT i;
				BSTR bstr = pvar->bstrVal;
				WCHAR ch;

				for(i = 0; i < len; i ++)
				{
					ch = bstr[i];
					if(ch < ' ' || ch == '#' || ch == '%' || ch == '&' || ch == '*' || ch == '|'
						 || ch == '\"' || ch == '<' || ch == '>')
					{
						bIsBin = TRUE;
						break;
					}
				}
			}
		}

		if(!bIsBin)
		{
			HRESULT hr;
			CBComPtr<CBFile> pStream;

			hr = pStream.CreateInstance();
			if(FAILED(hr))return hr;

			if(bOpen)
				hr = pStream->Open(pvar->bstrVal);
			else
				hr = pStream->Create(pvar->bstrVal);

			if(FAILED(hr))return hr;

			*pStm = pStream.Detach();

			return S_OK;
		}
	}

	if((pvar->vt == VT_UNKNOWN || pvar->vt == VT_DISPATCH) && pvar->punkVal)
	{
		CComQIPtr<IStream> pStream = pvar->punkVal;

		if(pStream != NULL)
		{
			*pStm = pStream.Detach();
			return S_OK;
		}
	}

	if(bOpen)
	{
		HRESULT hr;
		CBBlockStream* pStream = new CBBlockStream();

		hr = pStream->AttachVariant(pvar);
		if(FAILED(hr))
		{
			delete pStream;
			return hr;
		}

		*pStm = pStream;

		return S_OK;
	}

	return TYPE_E_TYPEMISMATCH;
}

#define LB_BLOCKSIZE	4096

void CBTempStream::CBLockBytes::SetSize(ULONG cb)
{
	int i;

	m_pBuffers.SetCount((cb + LB_BLOCKSIZE - 1) / LB_BLOCKSIZE);

	if(m_size < cb)
		for(i = (m_size + LB_BLOCKSIZE - 1) / LB_BLOCKSIZE; i < (int)m_pBuffers.GetCount(); i ++)
			m_pBuffers[i].Allocate(LB_BLOCKSIZE);

	m_size = cb;
}

ULONG CBTempStream::CBLockBytes::ReadAt(ULONG ulOffset, void* pv, ULONG cb)
{
	if(ulOffset >= m_size)
		return 0;

	int i, pos, size, cbRead;
	BYTE* ptr = (BYTE*)pv;

	cb += ulOffset;
	if(cb > m_size)
		cb = m_size;

	cbRead = cb - ulOffset;

	while(ulOffset < cb)
	{
		i = ulOffset / LB_BLOCKSIZE;
		pos = ulOffset % LB_BLOCKSIZE;
		size = cb - ulOffset;
		if(size + pos > LB_BLOCKSIZE)
			size = LB_BLOCKSIZE - pos;

		CopyMemory(ptr, m_pBuffers[i] + pos, size);

		ptr += size;
		ulOffset += size;
	}

	return cbRead;
}

void CBTempStream::CBLockBytes::WriteAt(ULONG ulOffset, void const* pv, ULONG cb)
{
	int i, pos, size;
	const BYTE* ptr = (const BYTE*)pv;

	if(ulOffset + cb > m_size)
	{
		m_pBuffers.SetCount((ulOffset + cb + LB_BLOCKSIZE - 1) / LB_BLOCKSIZE);

		for(i = (m_size + LB_BLOCKSIZE - 1) / LB_BLOCKSIZE; i < (int)m_pBuffers.GetCount(); i ++)
			m_pBuffers[i].Allocate(LB_BLOCKSIZE);

		m_size = ulOffset + cb;
	}

	cb += ulOffset;
	while(ulOffset < cb)
	{
		i = ulOffset / LB_BLOCKSIZE;
		pos = ulOffset % LB_BLOCKSIZE;
		size = cb - ulOffset;
		if(size + pos > LB_BLOCKSIZE)
			size = LB_BLOCKSIZE - pos;

		CopyMemory(m_pBuffers[i] + pos, ptr, size);

		ptr += size;
		ulOffset += size;
	}
}

CBTempStream::CBTempStream(ULONG nSize) :
	m_ulPosition(0),
	m_nSize(nSize)
{
	m_pLockBytes.CreateInstance();
}

// ISequentialStream
STDMETHODIMP CBTempStream::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	if(pv == NULL)
		return STG_E_INVALIDPOINTER;

	ULONG n, cbRead;

	if(pcbRead == NULL)pcbRead = &cbRead;
	*pcbRead = 0;

	m_cs.Enter();

	if(!m_nSize || m_ulPosition < m_nSize)
	{
		*pcbRead = n = m_pLockBytes->ReadAt(m_ulPosition, pv, cb);
		m_ulPosition += n;

		cb -= n;
		pv = (void *)((char*)pv + n);
	}

	if(m_hTempFile != NULL)
	{
		if(SetFilePointer(m_hTempFile, m_ulPosition - m_nSize, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			m_cs.Leave();
			return GetErrorResult();
		}

		ULONG cbRead1 = 0;

		if(!::ReadFile(m_hTempFile, pv, cb, &cbRead1, NULL))
		{
			m_cs.Leave();
			return GetErrorResult();
		}

		m_ulPosition += cbRead1;
		*pcbRead += cbRead1;
	}

	m_cs.Leave();

	if(*pcbRead < cb)return S_FALSE;

	return S_OK;
}

STDMETHODIMP CBTempStream::Write(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	if(pv == NULL)
		return STG_E_INVALIDPOINTER;

	ULONG cbWritten;

	if(pcbWritten == NULL)pcbWritten = &cbWritten;
	*pcbWritten = 0;

	m_cs.Enter();

	if(!m_nSize || m_ulPosition < m_nSize)
	{
		ULONG cb1 = cb;

		if(m_nSize && (m_nSize - m_ulPosition < cb1))
			cb1 = m_nSize - m_ulPosition;

		m_pLockBytes->WriteAt(m_ulPosition, pv, cb1);
		m_ulPosition += cb1;
		*pcbWritten = cb1;

		cb -= cb1;
		pv = (const void *)((const char*)pv + cb1);
	}

	if(cb > 0)
	{
		if(m_hTempFile == NULL)
		{
			char szTempPath[MAX_PATH];
			char szTempFile[MAX_PATH];

			::GetTempPathA(MAX_PATH, szTempPath);
			if(!::GetTempFileNameA(szTempPath, "~", 0, szTempFile))
			{
				::GetSystemDirectoryA(szTempPath, MAX_PATH);
				::GetTempFileNameA(szTempPath, "~", 0, szTempFile);
			}

			m_hTempFile.m_h = ::CreateFileA(szTempFile, GENERIC_READ | GENERIC_WRITE, 0, NULL,
				OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, 0);

			if (m_hTempFile == INVALID_HANDLE_VALUE)
			{
				m_hTempFile.m_h = NULL;
				m_cs.Leave();

				return GetErrorResult();
			}

			m_pLockBytes->SetSize(m_nSize);
		}

		if(SetFilePointer(m_hTempFile, m_ulPosition - m_nSize, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			m_cs.Leave();
			return GetErrorResult();
		}

		ULONG cbWritten1 = 0;

		if(!::WriteFile(m_hTempFile, pv, cb, &cbWritten1, NULL))
		{
			m_cs.Leave();
			return GetErrorResult();
		}

		m_ulPosition += cbWritten1;
		*pcbWritten += cbWritten1;
	}

	m_cs.Leave();

	return S_OK;
}

// IStream
STDMETHODIMP CBTempStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
	m_cs.Enter();

	if(dwOrigin == SEEK_CUR)
		dlibMove.QuadPart += m_ulPosition;
	else if(dwOrigin == SEEK_END)
	{
		dlibMove.QuadPart += m_pLockBytes->GetSize();
		if(m_hTempFile != NULL)
		{
			ULARGE_INTEGER cbSize;

			cbSize.LowPart = GetFileSize(m_hTempFile, &cbSize.HighPart);
			if(cbSize.LowPart == INVALID_FILE_SIZE)
			{
				HRESULT hr = GetErrorResult();
				if(FAILED(hr))
				{
					m_cs.Leave();
					return hr;
				}
			}

			dlibMove.QuadPart += cbSize.QuadPart;
		}
	}

	if(dlibMove.HighPart)
	{
		m_cs.Leave();
		return DISP_E_OVERFLOW;
	}
	m_ulPosition = dlibMove.LowPart;

	m_cs.Leave();

	if(plibNewPosition)
		plibNewPosition->QuadPart = dlibMove.LowPart;

	return S_OK;
}

STDMETHODIMP CBTempStream::SetSize(ULARGE_INTEGER libNewSize)
{
	if(libNewSize.HighPart)
		return DISP_E_OVERFLOW;

	m_cs.Enter();
	if(!m_nSize || libNewSize.LowPart <= m_nSize)
	{
		m_pLockBytes->SetSize(libNewSize.LowPart);
		m_hTempFile.Close();
	}else
	{
		m_pLockBytes->SetSize(m_nSize);

		if(SetFilePointer(m_hTempFile, libNewSize.LowPart - m_nSize, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			m_cs.Leave();
			return GetErrorResult();
		}

		if(!SetEndOfFile(m_hTempFile))
		{
			m_cs.Leave();
			return GetErrorResult();
		}
	}
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBTempStream::CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTempStream::Commit(DWORD grfCommitFlags)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTempStream::Revert(void)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTempStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTempStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBTempStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
	ZeroMemory(pstatstg, sizeof(STATSTG));

	pstatstg->type = STGTY_LOCKBYTES;

	m_cs.Enter();
	pstatstg->cbSize.LowPart = m_pLockBytes->GetSize();

	if(m_hTempFile)
	{
		LARGE_INTEGER cbSize = {0};

		cbSize.LowPart = SetFilePointer(m_hTempFile, 0, &cbSize.HighPart, FILE_END);

		if(cbSize.LowPart == INVALID_SET_FILE_POINTER)
		{
			HRESULT hr = GetErrorResult();
			if(FAILED(hr))
			{
				m_cs.Leave();
				return hr;
			}
		}

		pstatstg->cbSize.QuadPart += cbSize.QuadPart;
	}
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBTempStream::Clone(IStream **ppstm)
{
	return E_NOTIMPL;
}

