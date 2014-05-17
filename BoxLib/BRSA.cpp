#include "StdAfx.h"
#include "BRSA.h"
#include "BVarType.h"
#include <openssl\rsa.h>

void CBRSA::free(void)
{
	if(m_pRSA != NULL)
	{
		RSA_free((RSA*)m_pRSA);
		m_pRSA = NULL;
	}
}

STDMETHODIMP CBRSA::get_IsPrivateKey(VARIANT_BOOL *pVal)
{
	if(m_pRSA == NULL)return E_NOTIMPL;

	*pVal = VARIANT_FALSE;
	if(((RSA*)m_pRSA)->n && ((RSA*)m_pRSA)->e && ((RSA*)m_pRSA)->d && ((RSA*)m_pRSA)->p &&
		((RSA*)m_pRSA)->q && ((RSA*)m_pRSA)->dmp1 && ((RSA*)m_pRSA)->dmq1 && ((RSA*)m_pRSA)->iqmp)
		*pVal = VARIANT_TRUE;

	return S_OK;
}

STDMETHODIMP CBRSA::get_Key(VARIANT *pVal)
{
	if(m_pRSA == NULL)return E_NOTIMPL;

	if(((RSA*)m_pRSA)->n && ((RSA*)m_pRSA)->e && ((RSA*)m_pRSA)->d && ((RSA*)m_pRSA)->p &&
		((RSA*)m_pRSA)->q && ((RSA*)m_pRSA)->dmp1 && ((RSA*)m_pRSA)->dmq1 && ((RSA*)m_pRSA)->iqmp)
		return get_PrivateKey(pVal);

	return get_PublicKey(pVal);
}

STDMETHODIMP CBRSA::get_KeySize(short *pVal)
{
	if(m_pRSA == NULL)return E_NOTIMPL;

	*pVal = 0;
	if(m_pRSA)*pVal = BN_num_bits(((RSA*)m_pRSA)->n);

	return S_OK;
}

STDMETHODIMP CBRSA::get_Padding(short *pVal)
{
	*pVal = m_nPadding;
	return S_OK;
}

STDMETHODIMP CBRSA::put_Padding(short newVal)
{
	m_nPadding = newVal;
	return S_OK;
}

STDMETHODIMP CBRSA::get_PrivateKey(VARIANT *pVal)
{
	if(m_pRSA && ((RSA*)m_pRSA)->n && ((RSA*)m_pRSA)->e && ((RSA*)m_pRSA)->d && ((RSA*)m_pRSA)->p &&
		((RSA*)m_pRSA)->q && ((RSA*)m_pRSA)->dmp1 && ((RSA*)m_pRSA)->dmq1 && ((RSA*)m_pRSA)->iqmp)
	{
		int nSize;

		if((nSize = i2d_RSAPrivateKey((RSA*)m_pRSA, NULL)) < 0)
			return E_NOTIMPL;

		CBVarPtr varPtr;
		varPtr.Create(nSize);

		i2d_RSAPrivateKey((RSA*)m_pRSA, (unsigned char **)&varPtr.m_pData);

		return varPtr.GetVariant(pVal);
	}

	return E_NOTIMPL;
}

STDMETHODIMP CBRSA::put_PrivateKey(VARIANT newVal)
{
	CBVarPtr varPtr;

	HRESULT hr = varPtr.Attach(newVal);
	if(FAILED(hr))return hr;

	free();

	m_pRSA = d2i_RSAPrivateKey((RSA**)&m_pRSA, (const BYTE**)&varPtr.m_pData, varPtr.m_nSize);
	if(m_pRSA == NULL)return E_INVALIDARG;

	return S_OK;
}

STDMETHODIMP CBRSA::get_PublicKey(VARIANT *pVal)
{
	if(m_pRSA == NULL)return E_NOTIMPL;

	int nSize;

	if((nSize = i2d_RSAPublicKey((RSA*)m_pRSA, NULL)) < 0)
		return E_NOTIMPL;

	CBVarPtr varPtr;
	varPtr.Create(nSize);

	i2d_RSAPublicKey((RSA*)m_pRSA, (unsigned char **)&varPtr.m_pData);

	return varPtr.GetVariant(pVal);
}

STDMETHODIMP CBRSA::put_PublicKey(VARIANT newVal)
{
	CBVarPtr varPtr;

	HRESULT hr = varPtr.Attach(newVal);
	if(FAILED(hr))return hr;

	free();

	m_pRSA = d2i_RSAPublicKey((RSA**)&m_pRSA, (const BYTE**)&varPtr.m_pData, varPtr.m_nSize);
	if(m_pRSA == NULL)return E_INVALIDARG;

	return S_OK;
}

STDMETHODIMP CBRSA::Decrypt(VARIANT varData, VARIANT *pVal)
{
	if(m_pRSA == NULL)return E_NOTIMPL;

	CBVarPtr varPtr;
	int nSize;

	HRESULT hr = varPtr.Attach(varData);
	if(FAILED(hr))return hr;

	CBVarPtr varVal;
	varVal.Create(RSA_size((RSA*)m_pRSA));

	if(((RSA*)m_pRSA)->n && ((RSA*)m_pRSA)->e && ((RSA*)m_pRSA)->d && ((RSA*)m_pRSA)->p &&
		((RSA*)m_pRSA)->q && ((RSA*)m_pRSA)->dmp1 && ((RSA*)m_pRSA)->dmq1 && ((RSA*)m_pRSA)->iqmp)
		nSize = RSA_private_decrypt(varPtr.m_nSize, varPtr.m_pData, varVal.m_pData, (RSA*)m_pRSA, m_nPadding);
	else nSize = RSA_public_decrypt(varPtr.m_nSize, varPtr.m_pData, varVal.m_pData, (RSA*)m_pRSA, m_nPadding);

	if(nSize == -1)return E_INVALIDARG;

	return varVal.GetVariant(pVal, nSize);
}

STDMETHODIMP CBRSA::Encrypt(VARIANT varData, VARIANT *pVal)
{
	if(m_pRSA == NULL)return E_NOTIMPL;

	CBVarPtr varPtr;
	int nSize;

	HRESULT hr = varPtr.Attach(varData);
	if(FAILED(hr))return hr;

	CBVarPtr varVal;
	varVal.Create(RSA_size((RSA*)m_pRSA));

	if(((RSA*)m_pRSA)->n && ((RSA*)m_pRSA)->e && ((RSA*)m_pRSA)->d && ((RSA*)m_pRSA)->p &&
		((RSA*)m_pRSA)->q && ((RSA*)m_pRSA)->dmp1 && ((RSA*)m_pRSA)->dmq1 && ((RSA*)m_pRSA)->iqmp)
		nSize = RSA_private_encrypt(varPtr.m_nSize, varPtr.m_pData, varVal.m_pData, (RSA*)m_pRSA, m_nPadding);
	else nSize = RSA_public_encrypt(varPtr.m_nSize, varPtr.m_pData, varVal.m_pData, (RSA*)m_pRSA, m_nPadding);

	if(nSize == -1)return E_INVALIDARG;

	return varVal.GetVariant(pVal, nSize);
}

STDMETHODIMP CBRSA::GenerateKey(VARIANT varSize)
{
	int nKeySize = 512;

	if(varSize.vt != VT_ERROR)
		nKeySize = varGetNumber(varSize, 512);

	free();
	m_pRSA = RSA_generate_key(nKeySize, RSA_F4, NULL, NULL);

	return S_OK;
}

