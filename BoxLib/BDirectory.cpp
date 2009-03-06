#include "StdAfx.h"
#include "BDirectory.h"

HRESULT CBDirectory::getItem(BSTR strPath, CRBMap<CBVariant, CComVariant>::CPair** ppPair, BOOL bAutoCreate)
{
	return S_OK;
}

STDMETHODIMP CBDirectory::get_AutoCreateDirectory(VARIANT_BOOL *pbACD)
{
	m_cs.Enter();
	*pbACD = m_bAutoCreateDirectory ? VARIANT_TRUE : VARIANT_FALSE;
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDirectory::put_AutoCreateDirectory(VARIANT_BOOL bACD)
{
	m_cs.Enter();
	m_bAutoCreateDirectory = (bACD == VARIANT_TRUE);
	m_cs.Leave();

	return S_OK;
}

STDMETHODIMP CBDirectory::CreateDirectory(BSTR strPath)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBDirectory::CopyTo(BSTR strFrom, BSTR strTo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBDirectory::MoveTo(BSTR strFrom, BSTR strTo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBDirectory::Link(BSTR strFrom, BSTR strTo)
{
	return E_NOTIMPL;
}


