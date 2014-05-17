// BoxDES.cpp : implementation file
//

#include "stdafx.h"
#include "BoxDES.h"
#include "BoxBinPtr.h"

#include <openssl\rand.h>

// CBoxDES

IMPLEMENT_DYNCREATE(CBoxDES, CBoxSafeObject)

CBoxDES::CBoxDES(void)
{
	Clear();
}

BEGIN_DISPATCH_MAP(CBoxDES, CBoxSafeObject)
	DISP_PROPERTY_EX(CBoxDES, "KeySize", get_KeySize, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CBoxDES, "BlockSize", get_BlockSize, SetNotSupported, VT_I4)

	DISP_PROPERTY_EX(CBoxDES, "Key", get_Key, put_Key, VT_VARIANT)
	DISP_PROPERTY_EX(CBoxDES, "IV", get_IV, put_IV, VT_VARIANT)

	DISP_PROPERTY_EX(CBoxDES, "Mode", get_Mode, put_Mode, VT_I4)

	DISP_FUNCTION(CBoxDES, "GenerateKey", GenerateKey, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CBoxDES, "GenerateIV", GenerateIV, VT_EMPTY, VTS_NONE)

	DISP_FUNCTION(CBoxDES, "Decrypt", Decrypt, VT_VARIANT, VTS_VARIANT)
	DISP_FUNCTION(CBoxDES, "Encrypt", Encrypt, VT_VARIANT, VTS_VARIANT)

	DISP_FUNCTION(CBoxDES, "Clear", Clear, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

// CBoxDES message handlers

void CBoxDES::Clear(void)
{
	ZeroMemory(&m_aIV, sizeof(m_aIV));
	ZeroMemory(&m_aKey, sizeof(m_aKey));
	ZeroMemory(&m_desKey, sizeof(m_desKey));

	m_nMode = 0;
}

long CBoxDES::get_KeySize(void)
{
	return DES_KEY_SZ;
}

long CBoxDES::get_BlockSize(void)
{
	return DES_KEY_SZ;
}

void CBoxDES::GenerateIV(void)
{
	RAND_bytes(m_aIV, DES_KEY_SZ);
}

VARIANT CBoxDES::get_IV(void)
{
	return CBoxBinPtr(m_aIV, DES_KEY_SZ);
}

void CBoxDES::put_IV(VARIANT& varIV)
{
	CBoxBinPtr(varIV).CopyData(m_aIV, DES_KEY_SZ);
}

void CBoxDES::GenerateKey(void)
{
	RAND_bytes(m_aKey, DES_KEY_SZ);

	des_set_key(&m_aKey,m_desKey);
}

VARIANT CBoxDES::get_Key(void)
{
	return CBoxBinPtr(m_aKey, DES_KEY_SZ);
}

long CBoxDES::get_Mode(void)
{
	return m_nMode;
}

void CBoxDES::put_Mode(long nMode)
{
	if(nMode >= 0 && nMode <= 3)
		m_nMode = nMode;
}

void CBoxDES::put_Key(VARIANT& varKey)
{
	CBoxBinPtr(varKey).CopyData(m_aKey, DES_KEY_SZ);

	des_set_key(&m_aKey,m_desKey);
}

VARIANT CBoxDES::Encrypt(VARIANT& varData)
{
	CBoxBinPtr varPtr(varData);
	CBoxBinPtr varPtr1(((varPtr.m_nSize + DES_KEY_SZ - 1) / DES_KEY_SZ) * DES_KEY_SZ);
	int i = 0;

	if(varPtr.m_nSize >= DES_KEY_SZ)
		while(i < (varPtr.m_nSize - DES_KEY_SZ + 1))
			i += do_des(varPtr.m_pData + i, varPtr1.m_pData + i, varPtr.m_nSize - i, DES_ENCRYPT);

	if(i < varPtr.m_nSize)
	{
		BYTE aPending[DES_KEY_SZ];

		ZeroMemory(aPending, sizeof(aPending));
		CopyMemory(aPending, varPtr.m_pData + i, varPtr.m_nSize - i);
		do_des(aPending, varPtr1.m_pData + i, DES_KEY_SZ, DES_ENCRYPT);
	}

	return varPtr1;
}

VARIANT CBoxDES::Decrypt(VARIANT& varData)
{
	CBoxBinPtr varPtr(varData);

	if(varPtr.m_nSize % DES_KEY_SZ != 0)
		AfxThrowOleException(TYPE_E_TYPEMISMATCH);

	CBoxBinPtr varPtr1(varPtr.m_nSize);
	int i;

	for(i = 0; i < varPtr.m_nSize;)
		i += do_des(varPtr.m_pData + i, varPtr1.m_pData + i, varPtr.m_nSize - i, DES_DECRYPT);

	return varPtr1;
}

