#pragma once
#include "bstream.h"

class __declspec(uuid("94650000-0000-0000-0043-6F6E736F6C65"))
CBConsole : public CBDispatch<IConsole, CBStream>
{
	DECLARE_CLASS_EX(CBConsole, CBStream)

public:
	CBConsole(void);

protected:
	virtual HRESULT InternalRead(void *pv, ULONG cb, ULONG *pcbRead);
	virtual HRESULT InternalWrite(const void *pv, ULONG cb, ULONG *pcbWritten);
	virtual HRESULT InternalEOS(VARIANT_BOOL *pEOS);

public:
	// IBaseStream
	STDMETHOD(Write)(VARIANT varBuffer){return E_NOTIMPL;}

public:
	// IConsole
	STDMETHOD(get_Caption)(BSTR *pVal);
	STDMETHOD(put_Caption)(BSTR pVal);

private:
	HANDLE m_hInput, m_hOutput;
};
