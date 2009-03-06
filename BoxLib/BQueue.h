#pragma once

#include "BVariant.h"
#include "BThread.h"

class __declspec(uuid("94650000-0000-0000-0000-005175657565"))
CBQueue : public CBDispatch<IQueue>,
				public IPersistStreamInit
{
	DECLARE_CLASS(CBQueue)

	INTERFACE_BEGIN_EX(CBDispatch<IQueue>)
		PERSIST_INTERFACE
	INTERFACE_END

public:
	~CBQueue(void)
	{
		RemoveAll();
	}

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
	// IQueue
	STDMETHOD(get_Item)(VARIANT *pvar);
	STDMETHOD(get_Count)(long *cStrRet);
	STDMETHOD(get_Items)(VARIANT *pItemsArray);

	STDMETHOD(AddHead)(VARIANT var);
	STDMETHOD(AddTail)(VARIANT var);
	STDMETHOD(GetHead)(VARIANT *var);
	STDMETHOD(GetTail)(VARIANT *var);
	STDMETHOD(RemoveHead)(VARIANT *var);
	STDMETHOD(RemoveTail)(VARIANT *var);
	STDMETHOD(RemoveAll)(void);
	STDMETHOD(Load)(VARIANT VarSrc, short mode = 7);
	STDMETHOD(Save)(VARIANT VarDesc, short mode = 7);
	STDMETHOD(Join)(BSTR bstrDelimiter, BSTR* pvar);
	STDMETHOD(Split)(BSTR bstrExpression, BSTR bstrDelimiter);

private:
	CBCriticalSection m_cs;
	CAtlList<VARIANT> m_listItems;
};

