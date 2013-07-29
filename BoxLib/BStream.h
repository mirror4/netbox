#pragma once

#include "BDictionary.h"

class __declspec(uuid("94650000-0000-0000-0000-53747265616D"))
CBStream : public CBDispatch<IBaseStream>,
				public IStream,
				public IPersistStreamInit
{
	DECLARE_CLASS(CBStream)

	INTERFACE_BEGIN_EX(CBDispatch<IBaseStream>)
		STREAM_INTERFACE
		PERSIST_INTERFACE
	INTERFACE_END

public:
	CBStream(void) :
		m_ulSize(0),
		m_ulPosition(0),
		m_dwType(STGTY_STREAM),
		m_chLastCrLf(0),
		m_ulCodePage(0),
		m_ulLastWrittren(0)
	{}

	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

	ULONGLONG GetLength(void)
	{
		STATSTG ss;

		HRESULT hr = Stat(&ss, 0);
		if(FAILED(hr))return 0;

		return ss.cbSize.QuadPart;
	}

	ULONGLONG Seek(ULONGLONG pos, UINT nFrom = begin)
	{
		LARGE_INTEGER lVal;
		ULARGE_INTEGER rVal = {0};

		lVal.QuadPart = pos;
		Seek(lVal, nFrom, &rVal);

		return rVal.QuadPart;
	}

	void SeekToBegin(void)
	{
		Seek(0);
	}

	ULONGLONG SeekToEnd(void)
	{
		return Seek(0, end);
	}

	HRESULT ReadLine(CBStringA& str);
	CBStringA ReadLine();
	void WriteLine(LPCSTR str);
	void WriteText(LPCSTR str);

	HRESULT SetHandle(HANDLE h, DWORD dwType = STGTY_STORAGE);

protected:
	virtual HRESULT InternalRead(void *pv, ULONG cb, ULONG *pcbRead);
	virtual HRESULT InternalWrite(const void *pv, ULONG cb, ULONG *pcbWritten);
	virtual HRESULT InternalEOS(VARIANT_BOOL *pEOS);

public:
	// IPersist
	STDMETHOD(GetClassID)(CLSID *pClassID);

public:
	// IPersistStream
	STDMETHOD(IsDirty)(void);
	STDMETHOD(Load)(IStream *pStm);
	STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty);
	STDMETHOD(GetSizeMax)(ULARGE_INTEGER *pcbSize);

public:
	// IPersistStreamInit
	STDMETHOD(InitNew)(void);

public:
	// ISequentialStream
	STDMETHOD(Read)(void *pv, ULONG cb, ULONG *pcbRead = NULL);
	STDMETHOD(Write)(const void *pv, ULONG cb, ULONG *pcbWritten = NULL);

public:
	// IStream
	STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
	STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize);
	STDMETHOD(CopyTo)(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
	STDMETHOD(Commit)(DWORD grfCommitFlags);
	STDMETHOD(Revert)(void);
	STDMETHOD(LockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	STDMETHOD(UnlockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	STDMETHOD(Stat)(STATSTG *pstatstg, DWORD grfStatFlag);
	STDMETHOD(Clone)(IStream **ppstm);

public:
	// IBaseStream
	STDMETHOD(get_CodePage)(long *pVal);
	STDMETHOD(put_CodePage)(long newVal);
	STDMETHOD(get_EOS)(VARIANT_BOOL *pEOS);
	STDMETHOD(get_Handle)(long *pVal);
	STDMETHOD(get_lastModify)(DATE *pVal);
	STDMETHOD(get_Position)(DOUBLE *pVal);
	STDMETHOD(put_Position)(DOUBLE newVal);
	STDMETHOD(get_Size)(DOUBLE *pVal);
	STDMETHOD(put_Size)(DOUBLE newVal);
	STDMETHOD(get_Type)(short *pVal);

	STDMETHOD(Open)(long nSize);
	STDMETHOD(Read)(long nSize, VARIANT *pVal);
	STDMETHOD(Write)(VARIANT varBuffer);
	STDMETHOD(ReadLine)(BSTR *pVal);
	STDMETHOD(WriteLine)(BSTR Var);
	STDMETHOD(WriteBlankLines)(short Var);
	STDMETHOD(ReadRawLine)(VARIANT* pVal);
	STDMETHOD(ReadText)(long nSize, BSTR *pVal);
	STDMETHOD(WriteText)(BSTR str);
	STDMETHOD(ReadVariant)(short mode, VARIANT *pVal);
	STDMETHOD(WriteVariant)(VARIANT var, short mode = 7);
	STDMETHOD(ReadBoolean)(VARIANT_BOOL *pVal);
	STDMETHOD(WriteBoolean)(VARIANT_BOOL Var);
	STDMETHOD(ReadByte)(short *pVal);
	STDMETHOD(WriteByte)(short Var);
	STDMETHOD(ReadInt)(short *pVal);
	STDMETHOD(WriteInt)(short Var);
	STDMETHOD(ReadLong)(long *pVal);
	STDMETHOD(WriteLong)(long Var);
	STDMETHOD(ReadFloat)(float *pVal);
	STDMETHOD(WriteFloat)(float Var);
	STDMETHOD(ReadDouble)(DOUBLE *pVal);
	STDMETHOD(WriteDouble)(DOUBLE Var);
	STDMETHOD(ReadDate)(DATE *pVal);
	STDMETHOD(WriteDate)(DATE Var);
	STDMETHOD(ReadObject)(short mode, IDispatch **pVal);
	STDMETHOD(WriteObject)(IDispatch *Var, short mode);
	STDMETHOD(Close)(void);
	STDMETHOD(Attach)(IUnknown *SrcStream);
	STDMETHOD(CopyTo)(VARIANT varDestStream, DOUBLE nSize = -1);
	STDMETHOD(CopyFrom)(VARIANT varSrcStream, DOUBLE nSize = -1);
	STDMETHOD(setEOS)(void);
	STDMETHOD(Flush)(void);
	STDMETHOD(get_LastWritten)(long *pVal);

private:
	HRESULT ReadBuffer();
	HRESULT inline ReadBlock(void *pv, ULONG cb)
	{
		char *pBuffer = (char*)pv;
		HRESULT hr;
		ULONG cbRead;

		while(cb)
		{
			hr = Read(pBuffer, cb, &cbRead);
			if(FAILED(hr))return hr;
			if(cbRead == 0)
				return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

			cb -= cbRead;
			pBuffer += cbRead;
		}

		return S_OK;
	}

protected:
	CHandle m_hFile;
	CComQIPtr<IStream> m_pStream;
	DWORD m_dwType;

protected:
	static HRESULT CopyStream(IStream* pdst, IStream* psrc, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);

	CBLockResource m_qlStream;
	CBAutoPtr<char> m_chBuffer;
	ULONG m_ulSize;
	ULONG m_ulPosition;
	char m_chLastCrLf;
	ULONG m_ulCodePage;
	ULONG m_ulLastWrittren;

public:
	static HRESULT GetStream(VARIANT* pvar, IStream** pStm, BOOL bOpen = TRUE);
};

