#include "StdAfx.h"
#include ".\bidentity.h"

STDMETHODIMP CBIdentity::get_Item(BSTR strKey, LONG nCount, VARIANT *pvar)
{
	VARIANT *pvarFind;

	if(nCount < 0)nCount = 1;

	CBLock l(&m_cs);

	if(m_dict.Lookup(strKey, &pvarFind) == S_OK)
	{
		*pvar = *pvarFind;

		switch (pvarFind->vt)
		{
		case VT_UI1:
		case VT_I1:
			pvarFind->cVal += (BYTE)nCount;
			break;
		case VT_I2:
		case VT_UI2:
		case VT_BOOL:
			pvarFind->iVal += (WORD)nCount;
			break;
		case VT_I4:
		case VT_UI4:
		case VT_INT:
		case VT_UINT:
		case VT_ERROR:
			pvarFind->lVal += nCount;
			break;
		case VT_I8:
		case VT_UI8:
			pvarFind->llVal += nCount;
			break;
		case VT_R4:
			pvarFind->fltVal += nCount;
			break;
		case VT_R8:
		case VT_CY:
		case VT_DATE:
			pvarFind->dblVal += nCount;
			break;
		}
	}

	return S_OK;
}

STDMETHODIMP CBIdentity::get__NewEnum(IUnknown **ppEnumReturn)
{
	return getNewEnum(this, ppEnumReturn);
}

STDMETHODIMP CBIdentity::get_Items(VARIANT *pItemsArray)
{
	CBLock l(&m_cs);

	return m_dict.get_Items(pItemsArray);
}

STDMETHODIMP CBIdentity::get_Keys(VARIANT *pKeysArray)
{
	CBLock l(&m_cs);

	return m_dict.get_Keys(pKeysArray);
}

STDMETHODIMP CBIdentity::Add(BSTR strKey, VARIANT var)
{
	if(!CBVariant::GetSize(var.vt))
		return DISP_E_TYPEMISMATCH;

	VARIANT* pvar;

	CBLock l(&m_cs);

	if(m_dict.Lookup(strKey, &pvar, TRUE) == S_OK)
		return 0x800A01C9;

	*pvar = var;

	return S_OK;
}

STDMETHODIMP CBIdentity::Exists(BSTR strKey, VARIANT_BOOL *pExists)
{
	CBLock l(&m_cs);

	return m_dict.Exists(strKey, pExists);
}

STDMETHODIMP CBIdentity::GenGUIDString(BSTR* retVal)
{
	GUID id;
	LPOLESTR ppsz = NULL;
	HRESULT hr;

	hr = CoCreateGuid(&id);
	if(FAILED(hr))return hr;

	hr = ::StringFromCLSID(id, &ppsz);
	if(FAILED(hr))return hr;

	*retVal = ::SysAllocString(ppsz);

	CoTaskMemFree(ppsz);

	return S_OK;
}

