#include "StdAfx.h"
#include "BHash.h"
#include "BVarType.h"
#include "BCollection.h"

#include <openssl\md2.h>
#include <openssl\MD4.h>
#include <openssl\md5.h>
#include <openssl\MDC2.h>
#include <openssl\ripemd.h>
#include <openssl\SHA.h>
#include "SHA2.h"

WCHAR s_strAlgoError[] = L"Algorithm not initialized.";
static struct
{
	LPCWSTR	Name;
	short	Size;
	int (*Init)(void *);
	int (*Update)(void *, const unsigned char *, unsigned long);
	int (*Final)(unsigned char *, void *);
} s_HashAlgos[]= 
{
	{L"MD2", MD2_DIGEST_LENGTH,
		(int (*)(void *))MD2_Init,
		(int (*)(void *, const unsigned char *, unsigned long))MD2_Update,
		(int (*)(unsigned char *, void *))MD2_Final
	},
	{L"MD4", MD4_DIGEST_LENGTH,
		(int (*)(void *))MD4_Init,
		(int (*)(void *, const unsigned char *, unsigned long))MD4_Update,
		(int (*)(unsigned char *, void *))MD4_Final
	},
	{L"MD5", MD5_DIGEST_LENGTH,
		(int (*)(void *))MD5_Init,
		(int (*)(void *, const unsigned char *, unsigned long))MD5_Update,
		(int (*)(unsigned char *, void *))MD5_Final
	},
	{L"MDC2", MDC2_DIGEST_LENGTH,
		(int (*)(void *))MDC2_Init,
		(int (*)(void *, const unsigned char *, unsigned long))MDC2_Update,
		(int (*)(unsigned char *, void *))MDC2_Final
	},
	{L"RIPEMD160", RIPEMD160_DIGEST_LENGTH,
		(int (*)(void *))RIPEMD160_Init,
		(int (*)(void *, const unsigned char *, unsigned long))RIPEMD160_Update,
		(int (*)(unsigned char *, void *))RIPEMD160_Final
	},
	{L"SHA", SHA_DIGEST_LENGTH,
		(int (*)(void *))SHA_Init,
		(int (*)(void *, const unsigned char *, unsigned long))SHA_Update,
		(int (*)(unsigned char *, void *))SHA_Final
	},
	{L"SHA1", SHA_DIGEST_LENGTH,
		(int (*)(void *))SHA1_Init,
		(int (*)(void *, const unsigned char *, unsigned long))SHA1_Update,
		(int (*)(unsigned char *, void *))SHA1_Final
	},
	{L"SHA256", SHA256_DIGEST_LENGTH,
		(int (*)(void *))SHA256_Init,
		(int (*)(void *, const unsigned char *, unsigned long))SHA256_Update,
		(int (*)(unsigned char *, void *))SHA256_Final
	},
	{L"SHA384", SHA384_DIGEST_LENGTH,
		(int (*)(void *))SHA384_Init,
		(int (*)(void *, const unsigned char *, unsigned long))SHA384_Update,
		(int (*)(unsigned char *, void *))SHA384_Final
	},
	{L"SHA512", SHA512_DIGEST_LENGTH,
		(int (*)(void *))SHA512_Init,
		(int (*)(void *, const unsigned char *, unsigned long))SHA512_Update,
		(int (*)(unsigned char *, void *))SHA512_Final
	}
};

ULONG CBHash::GetEnum(VARIANT *rgVar, ULONG pos, ULONG count)
{
	ULONG i = 0;

	while(count > 0 && pos < sizeof(s_HashAlgos) / sizeof(s_HashAlgos[0]))
	{
		rgVar->vt = VT_BSTR;
		rgVar->bstrVal = ::SysAllocString(s_HashAlgos[pos].Name);
		count --;
		pos ++;
		rgVar ++;
		i ++;
	}

	return i;
}

STDMETHODIMP CBHash::get__NewEnum(IUnknown **ppEnumReturn)
{
	return getNewEnum(this, ppEnumReturn);
}

STDMETHODIMP CBHash::get_Name(BSTR *pVal)
{
	if(m_iAlgo < 0)return SetErrorInfo(s_strAlgoError);

	*pVal = ::SysAllocString(s_HashAlgos[m_iAlgo].Name);

	return S_OK;
}

STDMETHODIMP CBHash::get_HashSize(short *pVal)
{
	if(m_iAlgo < 0)return SetErrorInfo(s_strAlgoError);

	*pVal = s_HashAlgos[m_iAlgo].Size;
	return S_OK;
}

STDMETHODIMP CBHash::Create(BSTR bstrAlgo)
{
	m_iAlgo = -1;
	for(int i = 0; i < sizeof(s_HashAlgos) / sizeof(s_HashAlgos[0]); i ++)
		if(!wcsicmp(s_HashAlgos[i].Name, bstrAlgo))
		{
			m_iAlgo = i;
			break;
		}

	if(m_iAlgo == -1)
		return E_INVALIDARG;

	s_HashAlgos[m_iAlgo].Init(&m_ctx);
	return S_OK;
}

STDMETHODIMP CBHash::Update(VARIANT varData)
{
	if(m_iAlgo < 0)return SetErrorInfo(s_strAlgoError);

	CBVarPtr varPtr;

	HRESULT hr = varPtr.Attach(varData);
	if(FAILED(hr))return hr;

	s_HashAlgos[m_iAlgo].Update(&m_ctx, varPtr.m_pData, varPtr.m_nSize);

	return S_OK;
}

STDMETHODIMP CBHash::Final(VARIANT varData, VARIANT *retVal)
{
	if(m_iAlgo < 0)return SetErrorInfo(s_strAlgoError);

	if(varData.vt != VT_ERROR)
		Update(varData);

	CBVarPtr varPtr;

	varPtr.Create(s_HashAlgos[m_iAlgo].Size);
	s_HashAlgos[m_iAlgo].Final(varPtr.m_pData, &m_ctx);
	s_HashAlgos[m_iAlgo].Init(&m_ctx);

	return varPtr.GetVariant(retVal);
}

