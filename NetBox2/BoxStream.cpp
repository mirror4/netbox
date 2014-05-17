// BoxStream.cpp : implementation file
//

#include "stdafx.h"
#include "BoxStream.h"

// CBoxStream

IMPLEMENT_DYNAMIC(CBoxStream, CBoxSafeObject)

BEGIN_INTERFACE_MAP(CBoxStream, CBoxSafeObject)
	INTERFACE_PART(CBoxStream, IID_IStream, Stream)
END_INTERFACE_MAP()


// CBoxStream message handlers
STDMETHODIMP_(ULONG) CBoxStream::XStream::AddRef()
{
	METHOD_PROLOGUE_EX_(CBoxStream, Stream)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBoxStream::XStream::Release()
{
	METHOD_PROLOGUE_EX_(CBoxStream, Stream)
	return pThis->ExternalRelease();
}

STDMETHODIMP CBoxStream::XStream::QueryInterface(REFIID iid, LPVOID far* ppvObj)     
{
	METHOD_PROLOGUE_EX_(CBoxStream, Stream)
	return pThis->ExternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP CBoxStream::XStream::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	METHOD_PROLOGUE_EX_(CBoxStream, Stream)
	return pThis->StreamRead(pv, cb, pcbRead);
}

HRESULT CBoxStream::StreamRead(void *pv, ULONG cb, ULONG *pcbRead)
{
	return OLECMDERR_E_NOTSUPPORTED;
}

STDMETHODIMP CBoxStream::XStream::Write(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	METHOD_PROLOGUE_EX_(CBoxStream, Stream)
	return pThis->StreamWrite(pv, cb, pcbWritten);
}

HRESULT CBoxStream::StreamWrite(const void *pv, ULONG cb, ULONG *pcbWritten)
{
	return OLECMDERR_E_NOTSUPPORTED;
}

STDMETHODIMP CBoxStream::XStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
	METHOD_PROLOGUE_EX_(CBoxStream, Stream)
	return pThis->StreamSeek(dlibMove, dwOrigin, plibNewPosition);
}

HRESULT CBoxStream::StreamSeek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
	return OLECMDERR_E_NOTSUPPORTED;
}

STDMETHODIMP CBoxStream::XStream::SetSize(ULARGE_INTEGER libNewSize)
{
	return OLECMDERR_E_NOTSUPPORTED;
}

STDMETHODIMP CBoxStream::XStream::CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
	return OLECMDERR_E_NOTSUPPORTED;
}

STDMETHODIMP CBoxStream::XStream::Commit(DWORD grfCommitFlags)
{
	return OLECMDERR_E_NOTSUPPORTED;
}

STDMETHODIMP CBoxStream::XStream::Revert( void)
{
	return OLECMDERR_E_NOTSUPPORTED;
}

STDMETHODIMP CBoxStream::XStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return OLECMDERR_E_NOTSUPPORTED;
}

STDMETHODIMP CBoxStream::XStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return OLECMDERR_E_NOTSUPPORTED;
}

STDMETHODIMP CBoxStream::XStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
	return OLECMDERR_E_NOTSUPPORTED;
}

STDMETHODIMP CBoxStream::XStream::Clone(IStream **ppstm)
{
	return OLECMDERR_E_NOTSUPPORTED;
}
