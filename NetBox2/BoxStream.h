
#pragma once


// CBoxStream command target

class CBoxStream : public CBoxSafeObject
{
	DECLARE_DYNAMIC(CBoxStream)
public:
	BEGIN_INTERFACE_PART(Stream, IStream)
		STDMETHOD(Read)(void *pv, ULONG cb, ULONG *pcbRead);
		STDMETHOD(Write)(const void *pv, ULONG cb, ULONG *pcbWritten);
		STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
		STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize);
		STDMETHOD(CopyTo)(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
		STDMETHOD(Commit)(DWORD grfCommitFlags);
		STDMETHOD(Revert)( void);
		STDMETHOD(LockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
		STDMETHOD(UnlockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
		STDMETHOD(Stat)(STATSTG *pstatstg, DWORD grfStatFlag);
		STDMETHOD(Clone)(IStream **ppstm);
	END_INTERFACE_PART(Stream)

	virtual HRESULT StreamRead(void *pv, ULONG cb, ULONG *pcbRead);
	virtual HRESULT StreamWrite(const void *pv, ULONG cb, ULONG *pcbWritten);
	virtual HRESULT StreamSeek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);

protected:
	DECLARE_INTERFACE_MAP()
};
