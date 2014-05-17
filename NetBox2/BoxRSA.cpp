// BoxRSA.cpp : implementation file
//

#include "stdafx.h"
#include "BoxRSA.h"
#include "BoxBinPtr.h"


// CBoxRSA

IMPLEMENT_DYNCREATE(CBoxRSA, CBoxSafeObject)

BEGIN_DISPATCH_MAP(CBoxRSA, CBoxSafeObject)
	DISP_FUNCTION(CBoxRSA, "GenerateKey", GenerateKey, VT_EMPTY, VTS_VARIANT)
	DISP_PROPERTY_EX(CBoxRSA, "KeySize", get_KeySize, SetNotSupported, VT_I4)

	DISP_PROPERTY_EX_ID(CBoxRSA, "Key", 0, get_Key, SetNotSupported, VT_VARIANT)

	DISP_PROPERTY_EX(CBoxRSA, "IsPrivateKey", get_IsPrivateKey, SetNotSupported, VT_BOOL)
	DISP_PROPERTY_EX(CBoxRSA, "PrivateKey", get_PrivateKey, put_PrivateKey, VT_VARIANT)

	DISP_PROPERTY_EX(CBoxRSA, "PublicKey", get_PublicKey, put_PublicKey, VT_VARIANT)

	DISP_PROPERTY(CBoxRSA, "Padding", m_nPadding, VT_I4)

	DISP_FUNCTION(CBoxRSA, "Encrypt", Encrypt, VT_VARIANT, VTS_VARIANT)
	DISP_FUNCTION(CBoxRSA, "Decrypt", Decrypt, VT_VARIANT, VTS_VARIANT)
END_DISPATCH_MAP()

// CBoxRSA message handlers

void CBoxRSA::GenerateKey(VARIANT &var)
{
	int nKeySize = 512;

	if(var.vt == VT_I4 || var.vt == VT_I2 || var.vt == VT_UI4 || var.vt == VT_UI2)
		nKeySize = var.lVal;
	else if(var.vt != VT_ERROR)
	{
		CComVariant var1;

		var1.ChangeType(VT_I4, &var);
		if(var1.vt == VT_I4)
			nKeySize = var1.lVal;
	}

	free();
	m_pRSA = RSA_generate_key(nKeySize, RSA_F4, NULL, NULL);
}

long CBoxRSA::get_KeySize(void)
{
	if(m_pRSA == NULL)
		return 0;

	return BN_num_bits(m_pRSA->n);
}

BOOL CBoxRSA::get_IsPrivateKey(void)
{
	if(m_pRSA == NULL)
		return FALSE;

	return m_pRSA->n && m_pRSA->e && m_pRSA->d && m_pRSA->p &&
		m_pRSA->q && m_pRSA->dmp1 && m_pRSA->dmq1 && m_pRSA->iqmp;
}

VARIANT CBoxRSA::get_PrivateKey(void)
{
	int nSize;

	if(!get_IsPrivateKey())
		AfxThrowMemoryException();

	if((nSize = i2d_RSAPrivateKey(m_pRSA, NULL)) < 0)
		AfxThrowMemoryException();

	CBoxBinPtr varPtr(nSize);

	i2d_RSAPrivateKey(m_pRSA, (unsigned char **)&varPtr.m_pData);

	return varPtr;
}

void CBoxRSA::put_PrivateKey(VARIANT &var)
{
	CBoxBinPtr varPtr(var);

	free();
	m_pRSA = d2i_RSAPrivateKey(&m_pRSA, (const BYTE**)&varPtr.m_pData, varPtr.m_nSize);

	if(m_pRSA == NULL)
		AfxThrowOleException(TYPE_E_TYPEMISMATCH);
}

VARIANT CBoxRSA::get_PublicKey(void)
{
	int nSize;

	if(m_pRSA == NULL)
		AfxThrowMemoryException();

	if((nSize = i2d_RSAPublicKey(m_pRSA, NULL)) < 0)
		AfxThrowMemoryException();

	CBoxBinPtr varPtr(nSize);

	i2d_RSAPublicKey(m_pRSA, (unsigned char **)&varPtr.m_pData);

	return varPtr;
}

void CBoxRSA::put_PublicKey(VARIANT &var)
{
	CBoxBinPtr varPtr(var);

	free();
	m_pRSA = d2i_RSAPublicKey(&m_pRSA, (const BYTE**)&varPtr.m_pData, varPtr.m_nSize);

	if(m_pRSA == NULL)
		AfxThrowOleException(TYPE_E_TYPEMISMATCH);
}

VARIANT CBoxRSA::get_Key(void)
{
	int nSize;
	int (*i2d)(const RSA *, unsigned char **) = i2d_RSAPublicKey;

	if(m_pRSA == NULL)
		AfxThrowMemoryException();

	if(get_IsPrivateKey())
		i2d = i2d_RSAPrivateKey;

	if((nSize = i2d(m_pRSA, NULL)) < 0)
		AfxThrowMemoryException();

	CBoxBinPtr varPtr(nSize);

	i2d(m_pRSA, (unsigned char **)&varPtr.m_pData);

	return varPtr;
}

VARIANT CBoxRSA::Encrypt(VARIANT& var)
{
	if(m_pRSA == NULL)
		AfxThrowMemoryException();

	CBoxBinPtr varPtr(var);
	CBoxBinPtr varPtr1(RSA_size(m_pRSA));
	int nSize;

	if(get_IsPrivateKey())
		nSize = RSA_private_encrypt(varPtr.m_nSize, varPtr, varPtr1, m_pRSA, m_nPadding);
	else
		nSize = RSA_public_encrypt(varPtr.m_nSize, varPtr, varPtr1, m_pRSA, m_nPadding);

	if(nSize == -1)
		AfxThrowOleException(TYPE_E_TYPEMISMATCH);

	return varPtr1.GetVariant(nSize);
}

VARIANT CBoxRSA::Decrypt(VARIANT& var)
{
	if(m_pRSA == NULL)
		AfxThrowMemoryException();

	CBoxBinPtr varPtr(var);
	CBoxBinPtr varPtr1(RSA_size(m_pRSA));
	int nSize;

	if(get_IsPrivateKey())
		nSize = RSA_private_decrypt(varPtr.m_nSize, varPtr, varPtr1, m_pRSA, m_nPadding);
	else
		nSize = RSA_public_decrypt(varPtr.m_nSize, varPtr, varPtr1, m_pRSA, m_nPadding);

	if(nSize == -1)
		AfxThrowOleException(TYPE_E_TYPEMISMATCH);

	return varPtr1.GetVariant(nSize);
}

