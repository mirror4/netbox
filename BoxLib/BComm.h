#pragma once

#include "BStream.h"

class __declspec(uuid("94650000-0000-0000-0000-0000436F6D6D"))
CBComm : public CBDispatch<ICommStream, CBStream>
{
	DECLARE_CLASS_EX(CBComm, CBStream)

public:
	// IBaseStream
	STDMETHOD(Attach)(IUnknown *SrcStream);
	STDMETHOD(Open)(long nSize);

public:
	// ICommStream
	STDMETHOD(get_BaudRate)(short* pVal);
	STDMETHOD(put_BaudRate)(short newVal);
	STDMETHOD(get_DataBits)(short* pVal);
	STDMETHOD(put_DataBits)(short newVal);
	STDMETHOD(get_Handshaking)(short* pVal);
	STDMETHOD(put_Handshaking)(short newVal);
	STDMETHOD(get_Parity)(short* pVal);
	STDMETHOD(put_Parity)(short newVal);
	STDMETHOD(get_ReadTimeout)(long *pVal);
	STDMETHOD(put_ReadTimeout)(long pVal);
	STDMETHOD(get_StopBits)(short* pVal);
	STDMETHOD(put_StopBits)(short newVal);
	STDMETHOD(get_WriteTimeout)(long *pVal);
	STDMETHOD(put_WriteTimeout)(long pVal);

	STDMETHOD(Open)(short lPort, VARIANT varMode = VARIANT_ERROR);
	STDMETHOD(Config)(VARIANT varMode = VARIANT_ERROR);

	STDMETHOD(get_ReadBuffer)(long *pVal);
	STDMETHOD(put_ReadBuffer)(long pVal);
	STDMETHOD(get_WriteBuffer)(long *pVal);
	STDMETHOD(put_WriteBuffer)(long pVal);

protected:
	virtual HRESULT InternalRead(void *pv, ULONG cb, ULONG *pcbRead);
	virtual HRESULT InternalWrite(const void *pv, ULONG cb, ULONG *pcbWritten);
	virtual HRESULT InternalEOS(VARIANT_BOOL *pEOS);

private:
	char m_strName[64];
	DCB m_dcb;
};

