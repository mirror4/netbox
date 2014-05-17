#pragma once

#include "BoxLib.h"
#include "BVariant.h"
#include "BVarType.h"
#include "BThread.h"

template <class T>
class CBKeyStringT
{
public:
	CBKeyStringT() throw()
	{}

	CBKeyStringT(const CBKeyStringT& src) throw()
	{
		m_str = src.m_str;
	}

	CBKeyStringT(const CBString& src) throw()
	{
		m_str = src;
	}

	CBKeyStringT(const CBStringA& src) throw()
	{
		m_str = src;
	}

	CBKeyStringT(LPCWSTR str) throw() :
		m_str(str)
	{
	}

	CBKeyStringT(LPCSTR str) throw() :
		m_str(str)
	{
	}

	BSTR AllocSysString(void) const
	{
		return m_str.AllocSysString();
	}

	friend bool operator==( const CBKeyStringT& str1, const CBKeyStringT& str2 ) throw()
	{
		return(str1.m_str.CompareNoCase(str2.m_str) == 0);
	}

	friend bool operator<( const CBKeyStringT& str1, const CBKeyStringT& str2 ) throw()
	{
		return(str1.m_str.CompareNoCase(str2.m_str) < 0);
	}

	friend bool operator>( const CBKeyStringT& str1, const CBKeyStringT& str2 ) throw()
	{
		return(str1.m_str.CompareNoCase(str2.m_str) > 0);
	}

	T m_str;
};

typedef CBKeyStringT< CBString > CBKeyString;
typedef CBKeyStringT< CBStringA > CBKeyStringA;

template <class T>
class CBEnumVARIANT : public CBComObject,
				public IEnumVARIANT
{
	INTERFACE_BEGIN
		INTERFACE_DEF(IEnumVARIANT)
	INTERFACE_END

public:
	CBEnumVARIANT(T* pCollection, ULONG ulPos = 0) :
		m_nPos(ulPos), m_pCollection(pCollection)
	{}

public:
	// IEnumVARIANT
	STDMETHOD(Next)(ULONG celt, VARIANT *rgVar, ULONG *pCeltFetched)
	{
		ULONG i;

		if(rgVar == NULL)
			return E_INVALIDARG;

		if(pCeltFetched != NULL)
			*pCeltFetched = 0;
		else if(celt != 1)
			return E_INVALIDARG;

		for(i = 0; i < celt; i ++)
			VariantInit(&rgVar[i]);

		i = m_pCollection->GetEnum(rgVar, m_nPos, celt);

		m_nPos += i;

		if(pCeltFetched != NULL)
			*pCeltFetched = i;

		return (i < celt) ? S_FALSE : S_OK;
	}

	STDMETHOD(Skip)(ULONG celt)
	{
		m_nPos += celt;
		return S_OK;
	}

	STDMETHOD(Reset)(void)
	{
		m_nPos = 0;
		return S_OK;
	}

	STDMETHOD(Clone)(IEnumVARIANT **ppEnum)
	{
		*ppEnum = new CBEnumVARIANT<T>(m_pCollection, m_nPos);
		return S_OK;
	}

private:
	ULONG m_nPos;
	CBComPtr<T> m_pCollection;
};

template <class T>
HRESULT getNewEnum(T* pThis, IUnknown **ppEnumReturn)
{
	CBComPtr< CBEnumVARIANT<T> > pEnum;

	pEnum.Attach(new CBEnumVARIANT<T>(pThis));
	return pEnum.QueryInterface(ppEnumReturn);
}

template <typename T>
class CBDict
{
public:
	HRESULT Lookup(CBString strKey, T **pVarReturn, BOOL bCreateNew = FALSE)
	{
		CRBMap<CBKeyString, T>::CPair* pPair = m_mapItems.Lookup(strKey);

		if(pPair != NULL)*pVarReturn = (T*)&pPair->m_value;
		else
		{
			if(bCreateNew)
			{
				T newVal;

				pPair = (CRBMap<CBKeyString, T>::CPair*)m_mapItems.SetAt(strKey, newVal);
				m_posArray.Add(pPair);
				*pVarReturn = (T*)&pPair->m_value;
			}
			return S_FALSE;
		}

		return S_OK;
	}

	T& operator[](CBString strKey)
	{
		T* p;

		Lookup(strKey, &p, TRUE);
		return *p;
	}

	HRESULT Lookup(VARIANT Var, T **pVarReturn, BOOL bCreateNew = FALSE)
	{
		if(Var.vt == VT_I4 || Var.vt == VT_I2)
		{
			long i = varGetNumber(Var) - 1;

			if(i >=0 && i < (int)m_posArray.GetCount())
				*pVarReturn = (T*)&((CRBMap<CBKeyString, T>::CPair*)m_posArray[i])->m_value;
			else return DISP_E_BADINDEX;

			return S_OK;
		}

		CBString strKey;

		HRESULT hr = varGetString(Var, strKey);
		if(FAILED(hr))return hr;

		return Lookup(strKey, pVarReturn, bCreateNew);
	}

	T& operator[](VARIANT Var)
	{
		T* p;

		Lookup(Var, &p, TRUE);
		return *p;
	}

	HRESULT Exists(BSTR strKey, VARIANT_BOOL* pExists)
	{
		*pExists = m_mapItems.Lookup(strKey) ? VARIANT_TRUE : VARIANT_FALSE;
		return S_OK;
	}

	HRESULT Exists(VARIANT Var, VARIANT_BOOL* pExists)
	{
		if(Var.vt == VT_I4 || Var.vt == VT_I2)
		{
			long i = varGetNumber(Var) - 1;

			*pExists = (i >=0 && i < (int)m_posArray.GetCount()) ? VARIANT_TRUE : VARIANT_FALSE;
			return S_OK;
		}

		CBString strKey;

		HRESULT hr = varGetString(Var, strKey);
		if(FAILED(hr))return hr;

		*pExists = m_mapItems.Lookup(strKey) ? VARIANT_TRUE : VARIANT_FALSE;
		return S_OK;
	}

	HRESULT get_Item(VARIANT Var, VARIANT *pVariantReturn)
	{
		T* pVal = NULL;

		HRESULT hr = Lookup(Var, &pVal);
		if(hr != S_OK)return hr;

		*(CComVariant*)pVariantReturn = *pVal;
		return S_OK;
	}

	HRESULT get_Key(VARIANT VarKey, VARIANT *pvar)
	{
		if(VarKey.vt == VT_I4 || VarKey.vt == VT_I2)
		{
			long i = varGetNumber(VarKey) - 1;

			if(i >=0 && i < (int)m_posArray.GetCount())
				*(CComVariant*)pvar = ((CRBMap<CBKeyString, T>::CPair*)m_posArray[i])->m_key.m_str;
			else return DISP_E_BADINDEX;

			return S_OK;
		}

		CBString strKey;

		HRESULT hr = varGetString(VarKey, strKey);
		if(FAILED(hr))return hr;

		CRBMap<CBKeyString, T>::CPair* pPair = m_mapItems.Lookup(strKey);

		if(pPair != NULL)
			*(CComVariant*)pvar = pPair->m_key.m_str;

		return S_OK;
	}

	HRESULT get_Items(VARIANT *pItemsArray)
	{
		CComSafeArray<VARIANT> bstrArray;
		VARIANT* pVar;
		HRESULT hr;

		hr = bstrArray.Create((ULONG)m_posArray.GetCount());
		if(FAILED(hr))return hr;

		pVar = (VARIANT*)bstrArray.m_psa->pvData;

		for(int i = 0; i < (int)m_posArray.GetCount(); i ++)
			*(CComVariant*)&pVar[i] = ((CRBMap<CBKeyString, T>::CPair*)m_posArray[i])->m_value;

		pItemsArray->vt = VT_ARRAY | VT_VARIANT;
		pItemsArray->parray = bstrArray.Detach();

		return S_OK;
	}

	HRESULT get_Keys(VARIANT *pKeysArray)
	{
		CComSafeArray<VARIANT> bstrArray;
		VARIANT* pVar;
		HRESULT hr;

		hr = bstrArray.Create((ULONG)m_posArray.GetCount());
		if(FAILED(hr))return hr;

		pVar = (VARIANT*)bstrArray.m_psa->pvData;

		for(int i = 0; i < (int)m_posArray.GetCount(); i ++)
			*(CComVariant*)&pVar[i] = ((CRBMap<CBKeyString, T>::CPair*)m_posArray[i])->m_key.m_str;

		pKeysArray->vt = VT_ARRAY | VT_VARIANT;
		pKeysArray->parray = bstrArray.Detach();

		return S_OK;
	}

	size_t GetCount(void) const
	{
		return m_posArray.GetCount();
	}

	void Remove(CBString strKey)
	{
		CRBMap<CBKeyString, T>::CPair* pPair = m_mapItems.Lookup(strKey);
		
		if(pPair != NULL)
		{
			for(int i = 0; i < (int)m_posArray.GetCount(); i ++)
				if(m_posArray[i] == pPair)
				{
					m_posArray.RemoveAt(i);
					break;
				}
			m_mapItems.RemoveAt(pPair);
		}
	}

	void Remove(int index)
	{
		if(i >=0 && i < (int)m_posArray.GetCount())
		{
			m_mapItems.RemoveAt(m_posArray[i]);
			m_posArray.RemoveAt(i);
		}
	}

	void RemoveAll()
	{
		m_mapItems.RemoveAll();
		m_posArray.RemoveAll();
	}

	BOOL GetAssoc(UINT i, CBKeyString& strKey, T& value)
	{
		if(i >=0 && i < (int)m_posArray.GetCount())
		{
			strKey = ((CRBMap<CBKeyString, T>::CPair*)m_posArray[i])->m_key;
			value = ((CRBMap<CBKeyString, T>::CPair*)m_posArray[i])->m_value;
			return TRUE;
		}

		return FALSE;
	}

	HRESULT Save(IStream *pStm)
	{
		CComVariant var;
		VARTYPE vt = VT_BSTR;
		UINT i;
		HRESULT hr;
		ULONG cbStrLen;

		for(i = 0; i < (int)m_posArray.GetCount(); i ++)
		{
			hr = pStm->Write(&vt, sizeof(VARTYPE), NULL);
			if (FAILED(hr))return hr;

			cbStrLen = ((CRBMap<CBKeyString, T>::CPair*)m_posArray[i])->m_key.m_str.GetLength() * sizeof(OLECHAR)  + sizeof(OLECHAR);

			hr = pStm->Write((void*) &cbStrLen, sizeof(cbStrLen), NULL);
			if (FAILED(hr))return hr;

			hr = pStm->Write((void*)(LPCWSTR)((CRBMap<CBKeyString, T>::CPair*)m_posArray[i])->m_key.m_str, cbStrLen, NULL);
			if (FAILED(hr))return hr;

			var = ((CRBMap<CBKeyString, T>::CPair*)m_posArray[i])->m_value;

			hr = ((CBVariant*)&var)->WriteToStream(pStm);
			if(FAILED(hr))return hr;
		}

		vt = 0xffff;
		return pStm->Write(&vt, sizeof(VARTYPE), NULL);
	}

	void Sort()
	{
		POSITION pos;
		UINT apos;

		pos = m_mapItems.GetHeadPosition();

		apos = 0;
		while(pos)
			m_posArray[apos ++] = m_mapItems.GetNext(pos);
	}

public:
	ULONG GetEnum(VARIANT *rgVar, ULONG pos, ULONG count)
	{
		ULONG i = 0;

		while(count > 0 && pos < (int)m_posArray.GetCount())
		{
			*(CComVariant*)rgVar = ((CRBMap<CBKeyString, T>::CPair*)m_posArray[pos])->m_key.m_str;
			count --;
			pos ++;
			rgVar ++;
			i ++;
		}

		return i;
	}

private:
	CRBMap<CBKeyString, T> m_mapItems;
	CAtlArray<POSITION> m_posArray;
};

