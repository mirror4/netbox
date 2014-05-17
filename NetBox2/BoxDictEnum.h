#pragma once

#include "BoxDictionary.h"
#include "BoxObject.h"

#pragma warning(disable : 4584)
class CBoxDictEnum :
	public IUnknown,
	public IEnumVARIANT
{
public:
	CBoxDictEnum(CBoxDictionary* pDict);
	CBoxDictEnum(CBoxDictEnum* pEnum);

	~CBoxDictEnum(void);

public:
	// IUnknown
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj);

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT *rgVar, ULONG *pCeltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)(void);
	STDMETHOD(Clone)(IEnumVARIANT **ppEnum);

private:
	long m_dwRef;
	int m_nPos;
	CBoxObject<CBoxDictionary> m_pDict;
};
#pragma warning(default : 4584)
