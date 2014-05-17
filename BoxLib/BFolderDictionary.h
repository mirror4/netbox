#pragma once

#include "BCollection.h"

class __declspec(uuid("94650000-6572-4469-6374-696F6E617279"))
CBFolderDictionary : public CBDispatch<IRequestDictionary>
{
	DECLARE_CLASS(CBFolderDictionary)

public:
	// IRequestDictionary
	STDMETHOD(get_Item)(VARIANT VarKey, VARIANT *pVariantReturn);
	STDMETHOD(get__NewEnum)(IUnknown **ppEnumReturn);
	STDMETHOD(get_Count)(long *cStrRet);
	STDMETHOD(get_Key)(VARIANT VarKey, VARIANT *pvar);

	STDMETHOD(Exists)(VARIANT VarKey, VARIANT_BOOL *pExists);

	STDMETHOD(get_Items)(VARIANT *pItemsArray);
	STDMETHOD(get_Keys)(VARIANT *pKeysArray);

public:
	ULONG GetEnum(VARIANT *rgVar, ULONG pos, ULONG count)
	{
		return m_dict.GetEnum(rgVar, pos, count);
	}

	template<class TYPE>
	void AddValue(CBString strKey, const TYPE& value)
	{
		*(CComVariant*)&m_dict[strKey] = value;
	}

private:
	CBDict<VARIANT> m_dict;
	CBCriticalSection m_cs;
};

