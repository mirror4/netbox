#include "StdAfx.h"
#include "BECC.h"
#include "BVarType.h"
#include <openssl/ec.h>
#include <openssl/ecdsa.h>

void CBECC::free(void)
{
	if (m_pECC)
	{
		EC_KEY_free((EC_KEY*)m_pECC);
		m_pECC = NULL;
	}
}

STDMETHODIMP CBECC::get_IsPrivateKey(VARIANT_BOOL *pVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBECC::get_Key(VARIANT *pVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBECC::get_KeySize(short *pVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBECC::get_Parameter(VARIANT *pVal)
{
	if (m_pECC == NULL)
		return E_NOTIMPL;

	int nSize;

	if((nSize = i2d_ECParameters((EC_KEY*)m_pECC, NULL)) < 0)
		return E_NOTIMPL;

	CBVarPtr varPtr;
	varPtr.Create(nSize);

	if (!i2d_ECParameters((EC_KEY*)m_pECC, (unsigned char **)&varPtr.m_pData))
		return E_INVALIDARG;

	return varPtr.GetVariant(pVal);
}

STDMETHODIMP CBECC::put_Parameter(VARIANT newVal)
{
	if (newVal.vt == VT_UI1 || newVal.vt == VT_I2 || newVal.vt == VT_I4 || newVal.vt == VT_R4 || newVal.vt == VT_R8)
	{
		int nid = varGetNumber(newVal, 409);

		free();

		m_pECC = EC_KEY_new_by_curve_name(nid);
		if(m_pECC == NULL)
			return E_INVALIDARG;
	}
	else
	{
		CBVarPtr varPtr;

		HRESULT hr = varPtr.Attach(newVal);
		if(FAILED(hr))return hr;

		free();

		m_pECC = d2i_ECParameters((EC_KEY**)&m_pECC, (const BYTE**)&varPtr.m_pData, varPtr.m_nSize);
		if(m_pECC == NULL)return E_INVALIDARG;
	}
	return S_OK;
}

STDMETHODIMP CBECC::get_PrivateKey(VARIANT *pVal)
{
	if (m_pECC == NULL)
		return E_NOTIMPL;

	if (!EC_KEY_check_key((EC_KEY*)m_pECC))
		return E_NOTIMPL;

	int nSize;

	if((nSize = i2d_ECPrivateKey((EC_KEY*)m_pECC, NULL)) < 0)
		return E_NOTIMPL;

	CBVarPtr varPtr;
	varPtr.Create(nSize);

	if (!i2d_ECPrivateKey((EC_KEY*)m_pECC, (unsigned char **)&varPtr.m_pData))
		return E_INVALIDARG;

	return varPtr.GetVariant(pVal);
}

STDMETHODIMP CBECC::put_PrivateKey(VARIANT newVal)
{
	CBVarPtr varPtr;

	HRESULT hr = varPtr.Attach(newVal);
	if(FAILED(hr))return hr;

	free();

	m_pECC = d2i_ECPrivateKey((EC_KEY**)&m_pECC, (const BYTE**)&varPtr.m_pData, varPtr.m_nSize);
	if(m_pECC == NULL)return E_INVALIDARG;

	return S_OK;
}

STDMETHODIMP CBECC::get_PublicKey(VARIANT *pVal)
{
	if (m_pECC == NULL)
		return E_NOTIMPL;

	if (!EC_KEY_check_key((EC_KEY*)m_pECC))
		return E_NOTIMPL;

	int nSize;

	if((nSize = i2o_ECPublicKey((EC_KEY*)m_pECC, NULL)) < 0)
		return E_NOTIMPL;

	CBVarPtr varPtr;
	varPtr.Create(nSize);

	if (!i2o_ECPublicKey((EC_KEY*)m_pECC, (unsigned char **)&varPtr.m_pData))
		return E_INVALIDARG;

	return varPtr.GetVariant(pVal);
}

STDMETHODIMP CBECC::put_PublicKey(VARIANT newVal)
{
	CBVarPtr varPtr;

	HRESULT hr = varPtr.Attach(newVal);
	if(FAILED(hr))return hr;

	m_pECC = o2i_ECPublicKey((EC_KEY**)&m_pECC, (const BYTE**)&varPtr.m_pData, varPtr.m_nSize);
	if(m_pECC == NULL)return E_INVALIDARG;

	return S_OK;
}

STDMETHODIMP CBECC::Decrypt(VARIANT varData, VARIANT *pVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBECC::Encrypt(VARIANT varData, VARIANT *pVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBECC::GenerateKey(VARIANT varNID)
{
	int nid = 409;		//#define NID_X9_62_prime192v1		409
	if(varNID.vt == VT_ERROR)
	{
		if(m_pECC == NULL)
		{
			m_pECC = EC_KEY_new_by_curve_name(nid);
			if(m_pECC == NULL)
				return E_INVALIDARG;
		}
	}
	else
	{
		free();
		nid = varGetNumber(varNID, 409);
		m_pECC = EC_KEY_new_by_curve_name(nid);
		if(m_pECC == NULL)
			return E_INVALIDARG;
	}

	if (!EC_KEY_generate_key((EC_KEY*)m_pECC))
		return E_INVALIDARG;

	return S_OK;
}

STDMETHODIMP CBECC::get_DSASize(short *pVal)
{
	if (m_pECC == NULL)
		return E_NOTIMPL;

	*pVal = ECDSA_size((EC_KEY*)m_pECC);
	return S_OK;
}

STDMETHODIMP CBECC::DSASign(VARIANT varData, VARIANT *pVal)
{
	if(m_pECC == NULL)return E_NOTIMPL;

	if (!EC_KEY_check_key((EC_KEY*)m_pECC))
		return E_NOTIMPL;

	CBVarPtr varPtr;

	HRESULT hr = varPtr.Attach(varData);
	if(FAILED(hr))return hr;

	int nSize = ECDSA_size((EC_KEY*)m_pECC);
	CBVarPtr varVal;
	varVal.Create(nSize);

	if (!ECDSA_sign(0, varPtr.m_pData, varPtr.m_nSize, varVal.m_pData, (unsigned int *)&nSize, (EC_KEY*)m_pECC))
		return E_INVALIDARG;

	return varVal.GetVariant(pVal, nSize);
}

STDMETHODIMP CBECC::DSAVerify(VARIANT varData, VARIANT varSig, VARIANT_BOOL *retVal)
{
	if(m_pECC == NULL)return E_NOTIMPL;

	if (!EC_KEY_check_key((EC_KEY*)m_pECC))
		return E_NOTIMPL;

	CBVarPtr varPtrData, varPtrSig;

	HRESULT hr = varPtrData.Attach(varData);
	if(FAILED(hr))return hr;

	hr = varPtrSig.Attach(varSig);
	if(FAILED(hr))return hr;

	int n = ECDSA_verify(0, varPtrData.m_pData, varPtrData.m_nSize, varPtrSig.m_pData, varPtrSig.m_nSize, (EC_KEY*)m_pECC);
	if (n == -1)
		return E_INVALIDARG;

	*retVal = n ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}
