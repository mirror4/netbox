#pragma once

#include "BCollection.h"

class __declspec(uuid("94650000-6C65-4469-6374-696F6E617279"))
CBSimpleDictionary : public CBDispatch<ISimpleDictionary>
{
	DECLARE_CLASS(CBSimpleDictionary)

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
		m_cs.Enter();
		ULONG ulRet = m_dict.GetEnum(rgVar, pos, count);
		m_cs.Leave();

		return ulRet;
	}

	template<class TYPE>
	void AddValue(CBString strKey, const TYPE& value)
	{
		m_cs.Enter();
		m_dict[strKey] = value;
		m_cs.Leave();
	}

	void RemoveValue(CBString strKey)
	{
		m_cs.Enter();
		m_dict.Remove(strKey);
		m_cs.Leave();
	}

	void RemoveAll(void)
	{
		m_cs.Enter();
		m_dict.RemoveAll();
		m_cs.Leave();
	}

	void Sort()
	{
		m_cs.Enter();
		m_dict.Sort();
		m_cs.Leave();
	}

protected:
	CBDict<CComVariant> m_dict;
	CBCriticalSection m_cs;
};

