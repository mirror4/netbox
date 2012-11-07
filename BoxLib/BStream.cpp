#include "StdAfx.h"
#include "BStream.h"
#include "BVarType.h"
#include "BVariant.h"
#include "BMStream.h"
#include "BDate.h"
#include "BCodepage.h"

#define BUFFER_SIZE 1024

HRESULT CBStream::InternalRead(void *pv, ULONG cb, ULONG *pcbRead)
{
	*pcbRead = 0;

	CBComPtr<IStream> pStream;
	if((pStream = m_pStream) != NULL)
		return pStream->Read(pv, cb, pcbRead);

	if(m_hFile.m_h == NULL)
		return E_HANDLE;

	if(!::ReadFile(m_hFile, pv, cb, pcbRead, NULL))
		return GetErrorResult();

	if(*pcbRead < cb)return S_FALSE;

	return S_OK;
}

HRESULT CBStream::InternalWrite(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	CBComPtr<IStream> pStream;

	if((pStream = m_pStream) != NULL)
		return pStream->Write(pv, cb, pcbWritten);

	if(m_hFile.m_h == NULL)
		return E_HANDLE;

	if(!::WriteFile(m_hFile, pv, cb, pcbWritten, NULL))
		return GetErrorResult();

	return S_OK;
}

HRESULT CBStream::InternalEOS(VARIANT_BOOL *pEOS)
{
	LARGE_INTEGER lVal = {0};
	STATSTG ss;

	HRESULT hr = Seek(lVal, SEEK_CUR, (ULARGE_INTEGER*)&lVal);
	if(FAILED(hr))return hr;

	hr = Stat(&ss, 0);
	if(FAILED(hr))return hr;

	*pEOS = ss.cbSize.QuadPart > (ULONGLONG)lVal.QuadPart ? VARIANT_FALSE : VARIANT_TRUE;

	return S_OK;
}

HRESULT CBStream::ReadBuffer()
{
	HRESULT hr;
	ULONG cbRead;

	if(m_ulPosition == m_ulSize)
		m_ulSize = 0;
	else if(m_ulPosition > 0)
	{
		MoveMemory(m_chBuffer, m_chBuffer + m_ulPosition, m_ulSize - m_ulPosition);
		m_ulSize -= m_ulPosition;
	}
	m_ulPosition = 0;

	hr = InternalRead(m_chBuffer + m_ulSize, BUFFER_SIZE - m_ulSize, &cbRead);
	if(FAILED(hr))return hr;

	m_ulSize += cbRead;

	if(m_chLastCrLf != 0 && m_ulSize != 0)
	{
		if(m_chLastCrLf + m_chBuffer[0] == 23)
			m_ulPosition = 1;
		m_chLastCrLf = 0;
	}

	return hr;
}

// ISequentialStream
STDMETHODIMP CBStream::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	QLock(m_qlStream);

	HRESULT hr = S_OK;

	if(m_chLastCrLf != 0)
	{
		hr = ReadBuffer();
		if(FAILED(hr))return hr;
	}

	if(m_ulSize > m_ulPosition)
	{
		if(cb > m_ulSize - m_ulPosition)
		{
			cb = m_ulSize - m_ulPosition;
			hr = S_FALSE;
		}

		CopyMemory(pv, m_chBuffer + m_ulPosition, cb);

		m_ulPosition += cb;

		if(pcbRead != NULL)
			*pcbRead = cb;

		return S_OK;
	}

	if(cb > 0)
	{
		ULONG cbRead;

		if(pcbRead == NULL)pcbRead = &cbRead;

		if(pv == NULL)
			return STG_E_INVALIDPOINTER;

		hr = InternalRead(pv, cb, pcbRead);
		if(FAILED(hr))return hr;

	}else if(pcbRead != 0)
		*pcbRead = 0;

	return hr;
}

STDMETHODIMP CBStream::Write(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	if(pv == NULL)
		return STG_E_INVALIDPOINTER;

	ULONG cbWritten = 0;

	if(pcbWritten == NULL)pcbWritten = &cbWritten;

	if(m_dwType == STGTY_STORAGE)
	{
		QLock(m_qlStream);

		HRESULT hr;

		if(m_chLastCrLf != 0)
		{
			hr = ReadBuffer();
			if(FAILED(hr))return hr;
		}

		if(m_ulSize > m_ulPosition)
		{
			LARGE_INTEGER lVal = {0};

			hr = Seek(lVal, SEEK_CUR, NULL);
			if(FAILED(hr))return hr;
		}
	}

	return InternalWrite(pv, cb, pcbWritten);
}

// IStream
STDMETHODIMP CBStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
	QLock(m_qlStream);

	if(dwOrigin == SEEK_CUR)
		dlibMove.QuadPart -= m_ulSize - m_ulPosition;

	m_ulSize = 0;
	m_ulPosition = 0;
	m_chLastCrLf = 0;

	l.Unlock();

	CBComPtr<IStream> pStream;

	if((pStream = m_pStream) != NULL)
		return pStream->Seek(dlibMove, dwOrigin, plibNewPosition);

	if(m_hFile.m_h == NULL)
		return E_HANDLE;

	if(m_dwType != STGTY_STORAGE)
		return E_NOTIMPL;

	dlibMove.LowPart = SetFilePointer(m_hFile, dlibMove.LowPart, &dlibMove.HighPart, dwOrigin);

	if(dlibMove.LowPart == INVALID_SET_FILE_POINTER)
	{
		HRESULT hr = GetErrorResult();
		if(FAILED(hr))return hr;
	}

	if(plibNewPosition != NULL)
		*(LARGE_INTEGER*)plibNewPosition = dlibMove;

	return S_OK;
}

STDMETHODIMP CBStream::SetSize(ULARGE_INTEGER libNewSize)
{
	QLock(m_qlStream);

	m_ulSize = 0;
	m_ulPosition = 0;
	m_chLastCrLf = 0;

	l.Unlock();

	CBComPtr<IStream> pStream;

	if((pStream = m_pStream) != NULL)
		return pStream->SetSize(libNewSize);

	if(m_hFile.m_h == NULL)
		return E_HANDLE;

	if(m_dwType != STGTY_STORAGE)
		return E_NOTIMPL;

	LARGE_INTEGER lVal = {0};
	HRESULT hr;

	hr = Seek(lVal, SEEK_CUR, (ULARGE_INTEGER*)&lVal);
	if(FAILED(hr))return hr;

	hr = Seek(*(LARGE_INTEGER*)&libNewSize, SEEK_SET, NULL);
	if(FAILED(hr))return hr;

	if(!SetEndOfFile(m_hFile))
		return GetErrorResult();

	return Seek(lVal, SEEK_SET, NULL);
}

HRESULT CBStream::CopyStream(IStream* pdst, IStream* psrc, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
	if(pdst == NULL || psrc == NULL)
		return STG_E_INVALIDPOINTER;

	__int64 cbRead = 0, cbWritten = 0;
	char copyBuff[1024];
	ULONG cbLeft;
	HRESULT hr = S_OK;


	while(cb.QuadPart)
	{
		cbLeft = (ULONG)(cb.QuadPart > sizeof(copyBuff) ? sizeof(copyBuff) : cb.QuadPart);

		hr = psrc->Read(copyBuff, cbLeft, &cbLeft);
		if (FAILED(hr) || cbLeft == 0)break;

		cbRead += cbLeft;
		cb.QuadPart -= cbLeft;

		hr = pdst->Write(copyBuff, cbLeft, &cbLeft);
		if (FAILED(hr))break;
		cbWritten += cbLeft;
	}

	if(pcbRead != NULL)pcbRead->QuadPart = cbRead;
	if(pcbWritten != NULL)pcbWritten->QuadPart = cbWritten;

	return hr;
}

STDMETHODIMP CBStream::CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
	return CopyStream(pstm, this, cb, pcbRead, pcbWritten);
}

STDMETHODIMP CBStream::Commit(DWORD grfCommitFlags)
{
	CBComPtr<IStream> pStream;

	if((pStream = m_pStream) != NULL)
		return pStream->Commit(grfCommitFlags);

	return E_NOTIMPL;
}

STDMETHODIMP CBStream::Revert(void)
{
	CBComPtr<IStream> pStream;

	if((pStream = m_pStream) != NULL)
		return pStream->Revert();

	return E_NOTIMPL;
}

STDMETHODIMP CBStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	CBComPtr<IStream> pStream;

	if((pStream = m_pStream) != NULL)
		return pStream->LockRegion(libOffset, cb, dwLockType);

	if(m_hFile.m_h == NULL)
		return E_HANDLE;

	if(m_dwType != STGTY_STORAGE)
		return E_NOTIMPL;

	if (!::LockFile(m_hFile, libOffset.LowPart, libOffset.HighPart, cb.LowPart, cb.HighPart))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	CBComPtr<IStream> pStream;

	if((pStream = m_pStream) != NULL)
		return pStream->UnlockRegion(libOffset, cb, dwLockType);

	if(m_hFile.m_h == NULL)
		return E_HANDLE;

	if(m_dwType != STGTY_STORAGE)
		return E_NOTIMPL;

	if (!::UnlockFile(m_hFile, libOffset.LowPart, libOffset.HighPart, cb.LowPart, cb.HighPart))
		return GetErrorResult();

	return S_OK;
}

STDMETHODIMP CBStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
	CBComPtr<IStream> pStream;

	if((pStream = m_pStream) != NULL)
		return pStream->Stat(pstatstg, grfStatFlag);

	if(pstatstg == NULL)
		return STG_E_INVALIDPOINTER;

	if(m_hFile.m_h == NULL)
		return E_HANDLE;

	if(m_dwType != STGTY_STORAGE)
		return E_NOTIMPL;

	ZeroMemory(pstatstg, sizeof(STATSTG));
	pstatstg->type = m_dwType;
	if(!GetFileTime(m_hFile, &pstatstg->ctime, &pstatstg->atime, &pstatstg->mtime))
		return GetErrorResult();

	pstatstg->cbSize.LowPart = GetFileSize(m_hFile, &pstatstg->cbSize.HighPart);
	if(pstatstg->cbSize.LowPart == INVALID_FILE_SIZE)
	{
		HRESULT hr = GetErrorResult();
		if(FAILED(hr))return hr;
	}

	return S_OK;
}

STDMETHODIMP CBStream::Clone(IStream **ppstm)
{
	CBComPtr<IStream> pStream;

	if((pStream = m_pStream) != NULL)
		return pStream->Clone(ppstm);

	return E_NOTIMPL;
}

	// IBaseStream
STDMETHODIMP CBStream::get_CodePage(long *pVal)
{
	*pVal = m_ulCodePage;
	return S_OK;
}

STDMETHODIMP CBStream::put_CodePage(long newVal)
{
	CPINFO cpi;

	if(!GetCPInfo(newVal, &cpi))
		return E_INVALIDARG;

	m_ulCodePage = newVal;
	return S_OK;
}

STDMETHODIMP CBStream::get_EOS(VARIANT_BOOL *pEOS)
{
	QLock(m_qlStream);

	HRESULT hr;

	if(m_chLastCrLf != 0)
	{
		hr = ReadBuffer();
		if(FAILED(hr))return hr;
	}

	if(m_ulSize > m_ulPosition)
	{
		*pEOS = VARIANT_FALSE;

		return S_OK;
	}

	return InternalEOS(pEOS);
}

STDMETHODIMP CBStream::get_Handle(long *pVal)
{
	*pVal = (long)m_hFile.m_h;

	return S_OK;
}

STDMETHODIMP CBStream::get_lastModify(DATE *pVal)
{
	STATSTG ss;

	HRESULT hr = Stat(&ss, 0);
	if(FAILED(hr))return hr;

	*(CBDate*)pVal = ss.mtime;

	return S_OK;
}

STDMETHODIMP CBStream::get_Position(DOUBLE *pVal)
{
	LARGE_INTEGER lVal = {0};
	HRESULT hr;

	hr = Seek(lVal, SEEK_CUR, (ULARGE_INTEGER*)&lVal);
	if(FAILED(hr))return hr;

	*pVal = (DOUBLE)lVal.QuadPart;

	return S_OK;
}

STDMETHODIMP CBStream::put_Position(DOUBLE newVal)
{
	LARGE_INTEGER lVal;
	lVal.QuadPart = (LONGLONG)newVal;

	return Seek(lVal, newVal >= 0 ? SEEK_SET : SEEK_END, NULL);
}

STDMETHODIMP CBStream::get_Size(DOUBLE *pVal)
{
	STATSTG ss;

	HRESULT hr = Stat(&ss, 0);
	if(FAILED(hr))return hr;

	*pVal = (DOUBLE)ss.cbSize.QuadPart;
	return S_OK;
}

STDMETHODIMP CBStream::put_Size(DOUBLE newVal)
{
	ULARGE_INTEGER lVal;

	lVal.QuadPart = (ULONGLONG)newVal;

	return SetSize(lVal);
}

STDMETHODIMP CBStream::get_Type(short *pVal)
{
	*pVal = (short)m_dwType;
	return S_OK;
}

STDMETHODIMP CBStream::Read(long nSize, VARIANT *pVal)
{
	if(nSize == 0)
		return S_OK;

	CBVarPtr varPtr;
	ULONG nRead;
	HRESULT hr;

	if(nSize < 0)
	{
		DOUBLE nTotalSize;
		DOUBLE nPosition;

		hr = get_Size(&nTotalSize);
		if(SUCCEEDED(hr))
		{
			hr = get_Position(&nPosition);
			if(SUCCEEDED(hr))
			{
				if(nTotalSize - nPosition > 0x7fffffff)
					return DISP_E_OVERFLOW;
				nSize = (long)(nTotalSize - nPosition);
			}
		}
	}

	if(nSize < 0)
	{
		ULARGE_INTEGER cb = {0xffffffff, 0xffffffff};
		CBTempStream mStream;

		hr = CopyStream(&mStream, this, cb, NULL, &cb);
		if(FAILED(hr))return hr;

		if(cb.QuadPart == 0)
			return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

		if(cb.HighPart != 0)
			return DISP_E_OVERFLOW;

		return mStream.GetVariant(pVal);
	}

	hr = varPtr.Create(nSize);
	if(FAILED(hr))return hr;

	hr = Read(varPtr.m_pData, nSize, &nRead);
	if(FAILED(hr))return hr;

	if(nRead == 0)
		return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

	return varPtr.GetVariant(pVal, nRead);
}

STDMETHODIMP CBStream::Write(VARIANT varBuffer)
{
	CBVarPtr varPtr;
	HRESULT hr;

	hr = varPtr.Attach(varBuffer);
	if(FAILED(hr))return hr;

	return Write(varPtr.m_pData, varPtr.m_nSize);
}

HRESULT CBStream::ReadLine(CBStringA& str)
{
	str.Empty();

	QLock(m_qlStream);
	HRESULT hr;
	ULONG ulPos;

	if(m_chBuffer == NULL)m_chBuffer.Allocate(BUFFER_SIZE);

	while(1)
	{
		if(m_ulPosition == m_ulSize)
		{
			hr = ReadBuffer();
			if(FAILED(hr))return hr;

			if(m_ulPosition == m_ulSize)
			{
				if(str.IsEmpty())
					return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
				break;
			}
		}

		ulPos = m_ulPosition;
		while(ulPos < m_ulSize && !iscrlf(m_chBuffer[ulPos]))
			ulPos ++;

		if(ulPos > m_ulPosition)
		{
			str.Append(&m_chBuffer[m_ulPosition], ulPos - m_ulPosition);
			m_ulPosition = ulPos;
		}

		if(m_ulPosition < m_ulSize)
		{
			m_chLastCrLf = m_chBuffer[m_ulPosition ++];

			if(m_ulPosition < m_ulSize)
			{
				if(m_chLastCrLf + m_chBuffer[m_ulPosition] == 23)
					m_ulPosition ++;

				m_chLastCrLf = 0;
			}

			break;
		}
	}

	return S_OK;
}

CBStringA CBStream::ReadLine()
{
	CBStringA str;
	ReadLine(str);

	return str;
}

STDMETHODIMP CBStream::ReadLine(BSTR *pVal)
{
	CBStringA str;
	HRESULT hr = ReadLine(str);
	if(FAILED(hr))return hr;

	CBCodePage cp(m_ulCodePage);
	*pVal = str.AllocSysString();

	return S_OK;
}

void CBStream::WriteLine(LPCSTR str)
{
	CBStringA strBuffer(str);

	strBuffer.Append("\r\n", 2);

	Write((LPCSTR)strBuffer, strBuffer.GetLength());
}

STDMETHODIMP CBStream::WriteLine(BSTR Var)
{
	CBCodePage cp(m_ulCodePage);
	CBStringA strBuffer(Var, ::SysStringLen(Var));

	strBuffer.Append("\r\n", 2);

	return Write((LPCSTR)strBuffer, strBuffer.GetLength());
}

STDMETHODIMP CBStream::WriteBlankLines(short Var)
{
	char buffer[256];
	int i, c;
	HRESULT hr;

	if(Var < 0)
		return E_INVALIDARG;

	while(Var)
	{
		c = Var;
		if(c > sizeof(buffer) / 2)
			c = sizeof(buffer) / 2;

		for(i = 0; i < c; i ++)
		{
			buffer[i * 2] = '\r';
			buffer[i * 2 + 1] = '\n';
		}

		hr = Write(buffer, i * 2);
		if(FAILED(hr))return hr;

		Var -= c;
	}

	return S_OK;
}

STDMETHODIMP CBStream::ReadRawLine(VARIANT* pVal)
{
	CBStringA str;
	HRESULT hr = ReadLine(str);
	if(FAILED(hr))return hr;

	CBVarPtr varPtr;

	varPtr.Create(str.GetLength());
	CopyMemory(varPtr.m_pData, (LPCSTR)str, str.GetLength());

	return varPtr.GetVariant(pVal);
}

STDMETHODIMP CBStream::ReadText(long nSize, BSTR *pVal)
{
	HRESULT hr;

	if(nSize < 0)
	{
		DOUBLE nTotalSize;
		DOUBLE nPosition;

		hr = get_Size(&nTotalSize);
		if(SUCCEEDED(hr))
		{
			hr = get_Position(&nPosition);
			if(SUCCEEDED(hr))
			{
				if(nTotalSize - nPosition > 0x7fffffff)
					return DISP_E_OVERFLOW;
				nSize = (long)(nTotalSize - nPosition);
			}
		}
	}

	CBStringA str;
	if(nSize < 0)
	{
		ULARGE_INTEGER cb = {0xffffffff, 0xffffffff};
		CBTempStream mStream;

		hr = CopyStream(&mStream, this, cb, NULL, &cb);
		if(FAILED(hr))return hr;

		if(cb.QuadPart == 0)
			return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

		mStream.Write("", 1);

		CBCodePage cp(m_ulCodePage);

		mStream.SeekToBegin();
		hr = mStream.Read(str.GetBuffer(cb.LowPart), cb.LowPart);
		if(FAILED(hr))return hr;
		str.ReleaseBuffer(cb.LowPart);

		*pVal = str.AllocSysString();

		return S_OK;
	}

	ULONG ulPos;
	int bLastLead = 0;

	CBCodePage cp(m_ulCodePage);

	QLock(m_qlStream);

	if(m_chBuffer == NULL)m_chBuffer.Allocate(BUFFER_SIZE);

	while(nSize)
	{
		if(m_ulPosition == m_ulSize)
		{
			hr = ReadBuffer();
			if(FAILED(hr))return hr;

			if(m_ulPosition == m_ulSize)
			{
				if(str.IsEmpty())
					return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
				break;
			}
		}

		ulPos = m_ulPosition;

		if(m_ulCodePage == CP_UTF8)
			while(ulPos < m_ulSize && nSize)
			{
				static const char utf8_length[] =
				{
					1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0xc0-0xcf */
					1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0xd0-0xdf */
					2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* 0xe0-0xef */
					3,3,3,3,3,3,3,3,4,4,4,4,5,5,0,0  /* 0xf0-0xff */
				};

				if(bLastLead)
				{
					bLastLead --;
					if(((BYTE)m_chBuffer[ulPos] ^ 0x80) >= 0x40)
					{
						bLastLead = 0;
						continue;
					}
					if(bLastLead == 0)nSize --;
				}else if((BYTE)m_chBuffer[ulPos] >= 0xc0)
				{
					bLastLead = utf8_length[(BYTE)m_chBuffer[ulPos] - 0xc0];
					if(bLastLead == 0)nSize --;
				}else nSize --;

				ulPos ++;
			}
		else
			while(ulPos < m_ulSize && nSize)
			{
				if(bLastLead)
				{
					nSize --;
					bLastLead = FALSE;
				}else if(cp.IsDBCSLeadByte(m_chBuffer[ulPos]))
					bLastLead = TRUE;
				else nSize --;

				ulPos ++;
			}

		if(ulPos > m_ulPosition)
		{
			str.Append(&m_chBuffer[m_ulPosition], ulPos - m_ulPosition);
			m_ulPosition = ulPos;
		}
	}

	*pVal = str.AllocSysString();

	return S_OK;
}

void CBStream::WriteText(LPCSTR str)
{
	Write(str, strlen(str));
}

STDMETHODIMP CBStream::WriteText(BSTR str)
{
	CBCodePage cp(m_ulCodePage);
	CBStringA strBuffer(str, ::SysStringLen(str));

	return Write((LPCSTR)strBuffer, strBuffer.GetLength());
}

STDMETHODIMP CBStream::ReadVariant(short mode, VARIANT *pVal)
{
	SetPersistMode(mode);
	return ((CBVariant*)pVal)->LoadFromStream(this);
}

STDMETHODIMP CBStream::WriteVariant(VARIANT var, short mode)
{
	SetPersistMode(mode);
	return ((CBVariant*)&var)->WriteToStream(this);
}

STDMETHODIMP CBStream::ReadBoolean(VARIANT_BOOL *pVal)
{
	return ReadBlock(pVal, 4);
}

STDMETHODIMP CBStream::WriteBoolean(VARIANT_BOOL Var)
{
	return Write(&Var, 4);
}

STDMETHODIMP CBStream::ReadByte(short *pVal)
{
	return ReadBlock(pVal, 1);
}

STDMETHODIMP CBStream::WriteByte(short Var)
{
	return Write(&Var, 1);
}

STDMETHODIMP CBStream::ReadInt(short *pVal)
{
	return ReadBlock(pVal, 2);
}

STDMETHODIMP CBStream::WriteInt(short Var)
{
	return Write(&Var, 2);
}

STDMETHODIMP CBStream::ReadLong(long *pVal)
{
	return ReadBlock(pVal, 4);
}

STDMETHODIMP CBStream::WriteLong(long Var)
{
	return Write(&Var, 4);
}

STDMETHODIMP CBStream::ReadFloat(float *pVal)
{
	return ReadBlock(pVal, 4);
}

STDMETHODIMP CBStream::WriteFloat(float Var)
{
	return Write(&Var, 4);
}

STDMETHODIMP CBStream::ReadDouble(DOUBLE *pVal)
{
	return ReadBlock(pVal, 8);
}

STDMETHODIMP CBStream::WriteDouble(DOUBLE Var)
{
	return Write(&Var, 8);
}

STDMETHODIMP CBStream::ReadDate(DATE *pVal)
{
	return ReadBlock(pVal, 4);
}

STDMETHODIMP CBStream::WriteDate(DATE Var)
{
	return Write(&Var, 4);
}

STDMETHODIMP CBStream::ReadObject(short mode, IDispatch **pVal)
{
	SetPersistMode(mode);
	return LoadObjectFromStream(this, CLSID_NULL, (void**)pVal);
}

STDMETHODIMP CBStream::WriteObject(IDispatch *Var, short mode)
{
	SetPersistMode(mode);
	return WriteObjectToStream(Var, this);
}

STDMETHODIMP CBStream::Close(void)
{
	m_hFile.Close();
	m_dwType = STGTY_STREAM;

	m_ulCodePage = 0;

	QLock(m_qlStream);

	m_pStream.Release();

	m_ulSize = 0;
	m_ulPosition = 0;
	m_chLastCrLf = 0;

	return S_OK;
}

STDMETHODIMP CBStream::Attach(IUnknown *SrcStream)
{
	Close();

	m_pStream = SrcStream;

	if(m_pStream == NULL)
		return DISP_E_TYPEMISMATCH;

	STATSTG ss;

	HRESULT hr = Stat(&ss, 0);
	if(SUCCEEDED(hr))
		m_dwType = ss.type;

	return S_OK;
}

STDMETHODIMP CBStream::Open(long nSize)
{
	Close();

	m_pStream.Attach(new CBTempStream(nSize));
	m_dwType = STGTY_LOCKBYTES;

	return S_OK;
}

STDMETHODIMP CBStream::CopyTo(VARIANT varDestStream, DOUBLE nSize)
{
	CComPtr<IStream> pStream;
	ULARGE_INTEGER cb;

	HRESULT hr = CBStream::GetStream(&varDestStream, &pStream, FALSE);
	if(FAILED(hr))return hr;

	if(nSize >= 0)
		cb.QuadPart = (ULONGLONG)nSize;
	else
	{
		cb.LowPart = 0xffffffff;
		cb.HighPart = 0xffffffff;
	}

	return CopyStream(pStream, this, cb, NULL, NULL);
}

STDMETHODIMP CBStream::CopyFrom(VARIANT varSrcStream, DOUBLE nSize)
{
	CComPtr<IStream> pStream;
	ULARGE_INTEGER cb;

	HRESULT hr = CBStream::GetStream(&varSrcStream, &pStream);
	if(FAILED(hr))return hr;

	if(nSize >= 0)
		cb.QuadPart = (ULONGLONG)nSize;
	else
	{
		cb.LowPart = 0xffffffff;
		cb.HighPart = 0xffffffff;
	}

	return CopyStream(this, pStream, cb, NULL, NULL);
}

STDMETHODIMP CBStream::setEOS(void)
{
	LARGE_INTEGER lVal = {0};

	HRESULT hr = Seek(lVal, SEEK_CUR, (ULARGE_INTEGER*)&lVal);
	if(FAILED(hr))return hr;

	return SetSize(*(ULARGE_INTEGER*)&lVal);
}

STDMETHODIMP CBStream::GetClassID(CLSID *pClassID)
{
	*pClassID = __uuidof(CBStream);
	return S_OK;
}

STDMETHODIMP CBStream::IsDirty(void)
{
	return S_OK;
}

STDMETHODIMP CBStream::Load(IStream *pStm)
{
	if(m_dwType != STGTY_LOCKBYTES)
		return E_NOTIMPL;

	HRESULT hr;
	DOUBLE nPos, nSize;

	hr = ReadStream(pStm, &nSize, sizeof(DOUBLE));
	if(FAILED(hr))return hr;

	hr = ReadStream(pStm, &nPos, sizeof(DOUBLE));
	if(FAILED(hr))return hr;

	ULARGE_INTEGER cb;
	
	cb.QuadPart = (ULONGLONG)nSize;

	hr = CopyStream(this, pStm, cb, NULL, NULL);
	if(FAILED(hr))return hr;

	hr = put_Position(nPos);
	if(FAILED(hr))return hr;

	return S_OK;
}

STDMETHODIMP CBStream::Save(IStream *pStm, BOOL fClearDirty)
{
	if(m_dwType != STGTY_LOCKBYTES)
		return E_NOTIMPL;

	HRESULT hr;
	DOUBLE nPos, nSize;

	hr = get_Position(&nPos);
	if(FAILED(hr))return hr;

	hr = get_Size(&nSize);
	if(FAILED(hr))return hr;

	hr = pStm->Write(&nSize, sizeof(DOUBLE), NULL);
	if(FAILED(hr))return hr;

	hr = pStm->Write(&nPos, sizeof(DOUBLE), NULL);
	if(FAILED(hr))return hr;

	hr = put_Position(0);
	if(FAILED(hr))return hr;

	ULARGE_INTEGER cb;

	cb.QuadPart = (ULONGLONG)nSize;

	hr = CopyStream(pStm, this, cb, NULL, NULL);
	if(FAILED(hr))return hr;

	hr = put_Position(nPos);
	if(FAILED(hr))return hr;

	return S_OK;
}

STDMETHODIMP CBStream::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBStream::InitNew(void)
{
	return Open(0x100000);
}
