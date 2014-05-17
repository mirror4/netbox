#pragma once

#include "BCollection.h"

class __declspec(uuid("94650000-0000-0000-4964-656E74697479"))
CBIdentity : public CBDispatch<IIdentity>
{
	DECLARE_CLASS(CBIdentity)

public:
	//	IIdentity
	STDMETHOD(get_Item)(BSTR strKey, LONG nCount, VARIANT *pvar);
	STDMETHOD(get__NewEnum)(IUnknown **ppEnumReturn);
	STDMETHOD(get_Items)(VARIANT *pItemsArray);
	STDMETHOD(get_Keys)(VARIANT *pKeysArray);
	STDMETHOD(Add)(BSTR strKey, VARIANT var);
	STDMETHOD(Exists)(BSTR strKey, VARIANT_BOOL *pExists);
	STDMETHOD(GenGUIDString)(BSTR* retVal);

public:
	ULONG GetEnum(VARIANT *rgVar, ULONG pos, ULONG count)
	{
		CBLock l(&m_cs);
		return m_dict.GetEnum(rgVar, pos, count);
	}

private:
	CBDict<VARIANT> m_dict;
	CBCriticalSection m_cs;
};

