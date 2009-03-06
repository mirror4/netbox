#pragma once

#include "BCollection.h"

class __declspec(uuid("94650000-0056-6172-6961-6E744C697374"))
CBList : public CBDispatch<IStringList>
{
	DECLARE_CLASS(CBList)

public:
	~CBList(void);

public:
	STDMETHOD(get_Item)(VARIANT i, VARIANT *pVariantReturn);
	STDMETHOD(get_Count)(long *cStrRet);
	STDMETHOD(get__NewEnum)(IUnknown **ppEnumReturn);
	STDMETHOD(get_Items)(VARIANT *pItemsArray);

	STDMETHOD(get_Default)(VARIANT *pVariantReturn);

	template<class TYPE>
	void AddValue(const TYPE& value)
	{
		VARIANT var = {VT_EMPTY};

		*(CComVariant*)&var = value;
		m_arrayVariant.Add(var);
	}

	HRESULT get_Item(int i, VARIANT *pVariantReturn)
	{
		if(i >=0 && i < (int)m_arrayVariant.GetCount())
			VariantCopy(pVariantReturn, &m_arrayVariant[i]);
		else return DISP_E_BADINDEX;

		return S_OK;
	}

	void RemoveAll()
	{
		for(int i = 0; i < (int)m_arrayVariant.GetCount(); i ++)
			VariantClear(&m_arrayVariant[i]);

		m_arrayVariant.RemoveAll();
	}

	void Remove(UINT i)
	{
		if(i < (int)m_arrayVariant.GetCount())
		{
			VariantClear(&m_arrayVariant[i]);
			m_arrayVariant.RemoveAt(i);
		}
	}

	UINT GetCount(void)
	{
		return m_arrayVariant.GetCount();
	}

	CBString GetString(UINT i)
	{
		CBString str;

		if(i < m_arrayVariant.GetCount() && m_arrayVariant[i].vt == VT_BSTR)
			str = m_arrayVariant[i].bstrVal;

		return str;
	}

	void Sort(void);

public:
	ULONG GetEnum(VARIANT *rgVar, ULONG pos, ULONG count)
	{
		ULONG i = 0;

		while(count > 0 && pos < (int)m_arrayVariant.GetCount())
		{
			VariantCopy(rgVar, &m_arrayVariant[pos]);
			count --;
			pos ++;
			rgVar ++;
			i ++;
		}

		return i;
	}

protected:
	CAtlArray<VARIANT> m_arrayVariant;
};

