#pragma once

#include "BoxLib.h"
#include "BVarType.h"

class CBBlockStream : public CBClass<IStream>
{
public:
	CBBlockStream(LPBYTE pData, DWORD dwSize) :
		m_pData(pData),
		m_dwSize(dwSize),
		m_pvarPtr(NULL),
		m_dwPosition(0),
		m_dwCount(0)
	{
	}

	CBBlockStream(void) :
		m_pData(NULL),
		m_dwSize(0),
		m_pvarPtr(NULL),
		m_dwPosition(0),
		m_dwCount(0)
	{
	}

	~CBBlockStream(void)
	{
		Clear();
	}

	HRESULT AttachVariant(VARIANT* pvar)
	{
		HRESULT hr;

		m_pvarPtr = new CBVarPtr();

		hr = m_pvarPtr->Attach(pvar);
		if(FAILED(hr))return hr;

		m_pData = m_pvarPtr->m_pData;
		m_dwSize = m_pvarPtr->m_nSize;

		return S_OK;
	}

	HRESULT AttachVariant(VARIANT& var)
	{
		return AttachVariant(&var);
	}

	void Clear(void)
	{
		m_pData = NULL;
		m_pBuffer.Free();
		m_dwSize = 0;
		m_dwPosition = 0;
		m_dwCount = 0;
		if(m_pvarPtr)
		{
			delete m_pvarPtr;
			m_pvarPtr = NULL;
		}
	}

	STDMETHOD(Read)(void *pDest, ULONG nMaxLen, ULONG *pnRead)
	{
		if(m_pData == NULL)
			return E_NOTIMPL;

		DWORD dwToRead = min((m_dwSize - m_dwPosition), nMaxLen);

		memcpy(pDest, m_pData + m_dwPosition, dwToRead);
		m_dwPosition += dwToRead;
		if(m_dwCount < m_dwPosition)m_dwCount = m_dwPosition;

		if(pnRead != NULL)
			*pnRead = dwToRead;

		if(nMaxLen != dwToRead)
			return HRESULT_FROM_WIN32( ERROR_HANDLE_EOF );

		return S_OK;
	}

	STDMETHOD(Write)(const void * pv, ULONG cb, ULONG * pcbWritten)
	{
		if(m_pData != NULL)
			return E_NOTIMPL;

		if(cb)
		{
			if(m_dwSize - m_dwPosition < (int)cb)
			{
				while(m_dwSize - m_dwPosition < (int)cb)
				{
					if(m_dwSize < 4096)m_dwSize += 1024;
					else m_dwSize += m_dwSize / 4;

					if(m_dwSize < 0)
						return DISP_E_OVERFLOW;
				}

				CAutoVectorPtr<BYTE> pBuffer;

				if(!pBuffer.Allocate(m_dwSize))
					return E_OUTOFMEMORY;

				if(m_dwPosition)
					CopyMemory(pBuffer, m_pBuffer, m_dwPosition);

				m_pBuffer = pBuffer;
			}

			CopyMemory(m_pBuffer + m_dwPosition, pv, cb);
			m_dwPosition += cb;
			if(m_dwCount < m_dwPosition)m_dwCount = m_dwPosition;
		}

		if(pcbWritten)*pcbWritten = cb;

		return S_OK;
	}


	HRESULT __stdcall Seek(LARGE_INTEGER /*dlibMove*/, DWORD /*dwOrigin*/, ULARGE_INTEGER * /*pLibNewPosition*/)
	{
		return E_NOTIMPL;
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

	HRESULT __stdcall Stat(STATSTG * /*pstatstg*/, DWORD /*grfStatFlag*/)
	{
		return E_NOTIMPL;
	}

	HRESULT __stdcall Clone(IStream ** /*ppstm*/)
	{
		return E_NOTIMPL;
	}

public:
	CAutoVectorPtr<BYTE> m_pBuffer;
	DWORD	m_dwSize;
	DWORD	m_dwPosition;
	DWORD	m_dwCount;

private:
	LPBYTE	m_pData;
	CBVarPtr* m_pvarPtr;
};
