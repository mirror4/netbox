#include "StdAfx.h"
#include "BCookie.h"
#include "BEncoding.h"

STDMETHODIMP CBCookie::CBWriteCookie::put_Item(VARIANT Key, BSTR rhs)
{
	CBComPtr<CBCookie> pCookie;

	pCookie = m_pCookie;
	if(pCookie != NULL)
		return pCookie->put_Item(Key, rhs);

	return E_NOTIMPL;
}

STDMETHODIMP CBCookie::CBWriteCookie::put_Expires(DATE rhs)
{
	CBComPtr<CBCookie> pCookie;

	pCookie = m_pCookie;
	if(pCookie != NULL)
		return pCookie->put_Expires(rhs);

	return E_NOTIMPL;
}

STDMETHODIMP CBCookie::CBWriteCookie::put_Domain(BSTR rhs)
{
	CBComPtr<CBCookie> pCookie;

	pCookie = m_pCookie;
	if(pCookie != NULL)
		return pCookie->put_Domain(rhs);

	return E_NOTIMPL;
}

STDMETHODIMP CBCookie::CBWriteCookie::put_Path(BSTR rhs)
{
	CBComPtr<CBCookie> pCookie;

	pCookie = m_pCookie;
	if(pCookie != NULL)
		return pCookie->put_Path(rhs);

	return E_NOTIMPL;
}

STDMETHODIMP CBCookie::CBWriteCookie::put_Secure(VARIANT_BOOL rhs)
{
	CBComPtr<CBCookie> pCookie;

	pCookie = m_pCookie;
	if(pCookie != NULL)
		return pCookie->put_Secure(rhs);

	return E_NOTIMPL;
}

STDMETHODIMP CBCookie::CBWriteCookie::get_HasKeys(VARIANT_BOOL *pfHasKeys)
{
	CBComPtr<CBCookie> pCookie;

	pCookie = m_pCookie;
	if(pCookie != NULL)
		return pCookie->get_HasKeys(pfHasKeys);

	return E_NOTIMPL;
}

STDMETHODIMP CBCookie::CBWriteCookie::get__NewEnum(IUnknown **ppEnumReturn)
{
	CBComPtr<CBCookie> pCookie;

	pCookie = m_pCookie;
	if(pCookie != NULL)
		return pCookie->get__NewEnum(ppEnumReturn);

	return E_NOTIMPL;
}

STDMETHODIMP CBCookie::CBWriteCookie::get_Items(VARIANT *pItemsArray)
{
	CBComPtr<CBCookie> pCookie;

	pCookie = m_pCookie;
	if(pCookie != NULL)
		return pCookie->get_Items(pItemsArray);

	return E_NOTIMPL;
}

STDMETHODIMP CBCookie::CBWriteCookie::get_Keys(VARIANT *pKeysArray)
{
	CBComPtr<CBCookie> pCookie;

	pCookie = m_pCookie;
	if(pCookie != NULL)
		return pCookie->get_Keys(pKeysArray);

	return E_NOTIMPL;
}

STDMETHODIMP CBCookie::put_Item(VARIANT Key, BSTR rhs)
{
	m_bModified = TRUE;

	if(Key.vt == VT_ERROR)
	{
		m_dict.RemoveAll();
		m_strDefault = rhs;

		return S_OK;
	}

	m_dict[Key] = rhs;

	UINT nCount = m_dict.GetCount();
	UINT i;
	CBKeyString strKey;
	CBString strValue;

	m_strDefault.Empty();
	for(i = 0; i < nCount; i ++)
	{
		if(i > 0)m_strDefault += '&';

		m_dict.GetAssoc(i, strKey, strValue);
		m_strDefault += CBEncoding::UrlEncode(CBStringA(strKey.m_str));
		m_strDefault += '=';
		m_strDefault += CBEncoding::UrlEncode(CBStringA(strValue));
	}

	return S_OK;
}

STDMETHODIMP CBCookie::put_Expires(DATE rhs)
{
	m_bModified = TRUE;

	m_dateExpires = rhs;
	return S_OK;
}

STDMETHODIMP CBCookie::put_Domain(BSTR rhs)
{
	m_bModified = TRUE;

	m_strDomain = rhs;
	return S_OK;
}

STDMETHODIMP CBCookie::put_Path(BSTR rhs)
{
	m_bModified = TRUE;

	m_strPath = rhs;
	return S_OK;
}

STDMETHODIMP CBCookie::put_Secure(VARIANT_BOOL rhs)
{
	m_bModified = TRUE;

	m_bSecure = (rhs == VARIANT_TRUE);
	return S_OK;
}

STDMETHODIMP CBCookie::get_HasKeys(VARIANT_BOOL *pfHasKeys)
{
	*pfHasKeys = m_dict.GetCount() ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CBCookie::get__NewEnum(IUnknown **ppEnumReturn)
{
	return getNewEnum(this, ppEnumReturn);
}

STDMETHODIMP CBCookie::get_Item(VARIANT VarKey, VARIANT *pVariantReturn)
{
	if(VarKey.vt == VT_ERROR)
	{
		pVariantReturn->vt = VT_BSTR;
		pVariantReturn->bstrVal = m_strDefault.AllocSysString();

		return S_OK;
	}

	return m_dict.get_Item(VarKey, pVariantReturn);
}

STDMETHODIMP CBCookie::get_Count(long *cStrRet)
{
	*cStrRet = (long)m_dict.GetCount();
	return S_OK;
}

STDMETHODIMP CBCookie::get_Key(VARIANT VarKey, VARIANT *pvar)
{
	return m_dict.get_Key(VarKey, pvar);
}

STDMETHODIMP CBCookie::get_Items(VARIANT *pItemsArray)
{
	return m_dict.get_Items(pItemsArray);
}

STDMETHODIMP CBCookie::get_Keys(VARIANT *pKeysArray)
{
	return m_dict.get_Keys(pKeysArray);
}

CBStringA CBCookie::RanderCookie(void)
{
	CBStringA strRet;
	CBKeyString strKey;
	CBString strValue;

	if(m_dict.GetCount())strRet = m_strDefault;
	else strRet = CBEncoding::UrlEncode(CBStringA(m_strDefault));

	if(m_dateExpires != NULL)
	{
		strRet.Append("; expires=", 10);
		strRet += m_dateExpires;
	}

	if(!m_strDomain.IsEmpty())
	{
		strRet.Append("; domain=", 9);
		strRet += m_strDomain;
	}

	strRet.Append("; path=", 7);
	strRet += m_strPath;

	if(m_bSecure)strRet.Append("; secure", 8);

	return strRet;
}

HRESULT CBCookie::ParseUrlEncodeString(LPCSTR pstr, UINT nSize)
{
	LPCSTR pstrTemp;
	CBString strKey, strValue;
	LPCSTR pdefstr = pstr;
	UINT nDefSize = nSize;

	m_dict.RemoveAll();

	while(nSize)
	{
		pstrTemp = pstr;
		while(nSize && *pstr != '=')
		{
			pstr ++;
			nSize --;
		}
		if(!nSize)break;

		if(pstr > pstrTemp)
			strKey = CBEncoding::UrlDecode(pstrTemp, (UINT)(pstr - pstrTemp));
		else strKey.Empty();

		if(nSize)
		{
			nSize --;
			pstr ++;
		}

		pstrTemp = pstr;
		while(nSize && *pstr != '&')
		{
			pstr ++;
			nSize --;
		}

		if(!strKey.IsEmpty())
			if(pstr > pstrTemp)
				strValue = CBEncoding::UrlDecode(pstrTemp, (UINT)(pstr - pstrTemp));
			else strValue.Empty();

		if(nSize)
		{
			nSize --;
			pstr ++;
		}

		if(!strKey.IsEmpty())AddValue(strKey, strValue);
	}

	if(m_dict.GetCount())m_strDefault =  CBString(pdefstr, nDefSize);
	else m_strDefault = CBEncoding::UrlDecode(pdefstr, nDefSize);

	return S_OK;
}
