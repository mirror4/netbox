#include "StdAfx.h"
#include "BSimpleDictionary.h"

STDMETHODIMP CBSimpleDictionary::get_Item(VARIANT VarKey, VARIANT *pVariantReturn)
{
	HRESULT hr;

	m_cs.Enter();
	hr = m_dict.get_Item(VarKey, pVariantReturn);
	m_cs.Leave();

	return hr;
}

STDMETHODIMP CBSimpleDictionary::get__NewEnum(IUnknown **ppEnumReturn)
{
	HRESULT hr;

	m_cs.Enter();
	hr = getNewEnum(this, ppEnumReturn);
	m_cs.Leave();

	return hr;
}

STDMETHODIMP CBSimpleDictionary::get_Count(long *cStrRet)
{
	m_cs.Enter();
	*cStrRet = (long)m_dict.GetCount();
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBSimpleDictionary::get_Key(VARIANT VarKey, VARIANT *pvar)
{
	HRESULT hr;

	m_cs.Enter();
	hr = m_dict.get_Key(VarKey, pvar);
	m_cs.Leave();

	return hr;
}

STDMETHODIMP CBSimpleDictionary::Exists(VARIANT VarKey, VARIANT_BOOL *pExists)
{
	HRESULT hr;

	m_cs.Enter();
	hr = m_dict.Exists(VarKey, pExists);
	m_cs.Leave();

	return hr;
}

STDMETHODIMP CBSimpleDictionary::get_Items(VARIANT *pItemsArray)
{
	HRESULT hr;

	m_cs.Enter();
	hr = m_dict.get_Items(pItemsArray);
	m_cs.Leave();

	return hr;
}

STDMETHODIMP CBSimpleDictionary::get_Keys(VARIANT *pKeysArray)
{
	HRESULT hr;

	m_cs.Enter();
	hr = m_dict.get_Keys(pKeysArray);
	m_cs.Leave();

	return hr;
}

