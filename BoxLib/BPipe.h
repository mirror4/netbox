#pragma once

#include "BStream.h"

class __declspec(uuid("94650000-0000-0000-0000-000050697065"))
CBPipe : public CBDispatch<IPipe, CBStream>
{
	DECLARE_CLASS_EX(CBPipe, CBStream)

public:
	// IBaseStream
	STDMETHOD(get_EOS)(VARIANT_BOOL *pEOS);

	STDMETHOD(get_lastModify)(DATE *pVal);
	STDMETHOD(get_Position)(DOUBLE *pVal);
	STDMETHOD(put_Position)(DOUBLE newVal);
	STDMETHOD(get_Size)(DOUBLE *pVal);
	STDMETHOD(put_Size)(DOUBLE newVal);

	STDMETHOD(Open)(long nSize);
	STDMETHOD(Attach)(IUnknown *SrcStream);
	STDMETHOD(setEOS)(void);
	STDMETHOD(Flush)(void);

public:
	// IPipe
	STDMETHOD(Open)(long nSize, IPipe** retVal);
	STDMETHOD(get_dataAvailable)(long *pVal);
	
};

