#include "StdAfx.h"
#include "BCipher.h"
#include "BVarType.h"
#include "BMStream.h"
#include "BStream.h"
#include "BCollection.h"

#include <openssl\rand.h>

static struct
{
	LPCWSTR	Name;
	const EVP_CIPHER *(*ecb)(void);
	const EVP_CIPHER *(*cbc)(void);
	const EVP_CIPHER *(*cfb)(void);
	const EVP_CIPHER *(*ofb)(void);
} s_CipherAlgos[]= 
{
	{L"AES-128", EVP_aes_128_ecb, EVP_aes_128_cbc, EVP_aes_128_cfb, EVP_aes_128_ofb},
	{L"AES-192", EVP_aes_192_ecb, EVP_aes_192_cbc, EVP_aes_192_cfb, EVP_aes_192_ofb},
	{L"AES-256", EVP_aes_256_ecb, EVP_aes_256_cbc, EVP_aes_256_cfb, EVP_aes_256_ofb},
	{L"BLOWFISH", EVP_bf_ecb,	EVP_bf_cbc, EVP_bf_cfb, EVP_bf_ofb},
	{L"CAST5", EVP_cast5_ecb, EVP_cast5_cbc, EVP_cast5_cfb, EVP_cast5_ofb},
	{L"DES", EVP_des_ecb, EVP_des_cbc, EVP_des_cfb, EVP_des_ofb},
	{L"DES-EDE", EVP_des_ede_ecb, EVP_des_ede_cbc, EVP_des_ede_cfb, EVP_des_ede_ofb},
	{L"DES-EDE3", EVP_des_ede3_ecb, EVP_des_ede3_cbc, EVP_des_ede3_cfb, EVP_des_ede3_ofb},
	{L"DESX", EVP_desx_cbc, EVP_desx_cbc, EVP_desx_cbc, EVP_desx_cbc},
	{L"IDEA", EVP_idea_ecb, EVP_idea_cbc, EVP_idea_cfb, EVP_idea_ofb},
	{L"RC2", EVP_rc2_ecb, EVP_rc2_cbc, EVP_rc2_cfb, EVP_rc2_ofb},
	{L"RC2-40", EVP_rc2_40_cbc, EVP_rc2_40_cbc, EVP_rc2_40_cbc, EVP_rc2_40_cbc},
	{L"RC2-64", EVP_rc2_64_cbc, EVP_rc2_64_cbc, EVP_rc2_64_cbc, EVP_rc2_64_cbc},
	{L"RC4", EVP_rc4, EVP_rc4, EVP_rc4, EVP_rc4},
	{L"RC4-40", EVP_rc4_40, EVP_rc4_40, EVP_rc4_40, EVP_rc4_40}
};

extern WCHAR s_strAlgoError[];

CBCipher::CBCipher(void) : m_iKeySize(0), m_bPadding(TRUE)
{
	EVP_CIPHER_CTX_init(&m_ctx);
}

CBCipher::~CBCipher(void)
{
	EVP_CIPHER_CTX_cleanup(&m_ctx);
}

ULONG CBCipher::GetEnum(VARIANT *rgVar, ULONG pos, ULONG count)
{
	ULONG i = 0;

	while(count > 0 && pos < sizeof(s_CipherAlgos) / sizeof(s_CipherAlgos[0]))
	{
		rgVar->vt = VT_BSTR;
		rgVar->bstrVal = ::SysAllocString(s_CipherAlgos[pos].Name);
		count --;
		pos ++;
		rgVar ++;
		i ++;
	}

	return i;
}

STDMETHODIMP CBCipher::get__NewEnum(IUnknown **ppEnumReturn)
{
	return getNewEnum(this, ppEnumReturn);
}

STDMETHODIMP CBCipher::get_Name(BSTR *pVal)
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	*pVal = CBString(OBJ_nid2sn(EVP_CIPHER_CTX_nid(&m_ctx))).AllocSysString();

	return S_OK;
}

STDMETHODIMP CBCipher::get_BlockSize(short *pVal)
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	*pVal = EVP_CIPHER_CTX_block_size(&m_ctx);

	return S_OK;
}

STDMETHODIMP CBCipher::get_CipherMode(short *pVal)
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	*pVal = (short)EVP_CIPHER_CTX_mode(&m_ctx);

	return S_OK;
}

STDMETHODIMP CBCipher::get_KeySize(short *pVal)
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	*pVal = EVP_CIPHER_CTX_key_length(&m_ctx);

	return S_OK;
}

STDMETHODIMP CBCipher::get_Key(VARIANT *pVal)
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	if(m_iKeySize)
	{
		CBVarPtr varPtr;
		HRESULT hr;

		hr = varPtr.Create(m_iKeySize);
		if(FAILED(hr))return hr;

		CopyMemory(varPtr.m_pData, m_pKey, m_iKeySize);

		return varPtr.GetVariant(pVal);
	}

	return S_OK;
}

STDMETHODIMP CBCipher::put_Key(VARIANT Val)
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	HRESULT hr;
	CBVarPtr varPtr;

	hr = varPtr.Attach(Val);
	if(FAILED(hr))return hr;

	if(varPtr.m_nSize > EVP_MAX_KEY_LENGTH)
		return E_INVALIDARG;

	if(!EVP_CIPHER_CTX_set_key_length(&m_ctx, m_iKeySize))
		return E_INVALIDARG;

	m_pKey.Allocate(m_iKeySize = varPtr.m_nSize);
	CopyMemory(m_pKey, varPtr.m_pData, m_iKeySize);

	return S_OK;
}

STDMETHODIMP CBCipher::GenerateKey(short iSize)
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	if(iSize < 0)
		m_iKeySize = EVP_CIPHER_CTX_key_length(&m_ctx);

	if(iSize > EVP_MAX_KEY_LENGTH)
		return E_INVALIDARG;

	if(!EVP_CIPHER_CTX_set_key_length(&m_ctx, iSize))
		return E_INVALIDARG;

	m_pKey.Allocate(m_iKeySize = iSize);

	RAND_bytes(m_pKey, m_iKeySize);

	return S_OK;
}

STDMETHODIMP CBCipher::get_IVSize(short *pVal)
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	if(EVP_CIPHER_CTX_mode(&m_ctx) < 2)
		*pVal = 0;
	else *pVal = EVP_CIPHER_CTX_iv_length(&m_ctx);

	return S_OK;
}

STDMETHODIMP CBCipher::get_IV(VARIANT *pVal)
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	if(EVP_CIPHER_CTX_iv_length(&m_ctx) == 0 || EVP_CIPHER_CTX_mode(&m_ctx) < 2)
		return E_NOTIMPL;

	if(m_pIV)
	{
		CBVarPtr varPtr;
		HRESULT hr;

		hr = varPtr.Create(EVP_CIPHER_CTX_iv_length(&m_ctx));
		if(FAILED(hr))return hr;

		CopyMemory(varPtr.m_pData, m_pIV, EVP_CIPHER_CTX_iv_length(&m_ctx));

		return varPtr.GetVariant(pVal);
	}

	return S_OK;
}

STDMETHODIMP CBCipher::put_IV(VARIANT Val)
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	if(EVP_CIPHER_CTX_iv_length(&m_ctx) == 0 || EVP_CIPHER_CTX_mode(&m_ctx) < 2)
		return E_NOTIMPL;

	HRESULT hr;
	CBVarPtr varPtr;

	hr = varPtr.Attach(Val);
	if(FAILED(hr))return hr;

	if(varPtr.m_nSize != EVP_CIPHER_CTX_iv_length(&m_ctx))
		return E_INVALIDARG;

	m_pIV.Allocate(EVP_CIPHER_CTX_iv_length(&m_ctx));
	CopyMemory(m_pIV, varPtr.m_pData, EVP_CIPHER_CTX_iv_length(&m_ctx));

	return S_OK;
}

STDMETHODIMP CBCipher::GenerateIV()
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	if(EVP_CIPHER_CTX_iv_length(&m_ctx) == 0 || EVP_CIPHER_CTX_mode(&m_ctx) < 2)
		return E_NOTIMPL;

	m_pIV.Allocate(EVP_CIPHER_CTX_iv_length(&m_ctx));

	RAND_bytes(m_pIV, EVP_CIPHER_CTX_iv_length(&m_ctx));

	return S_OK;
}

STDMETHODIMP CBCipher::get_Padding(short *pVal)
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	*pVal = m_bPadding ? 1 : 0;

	return S_OK;
}

STDMETHODIMP CBCipher::put_Padding(short Val)
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	m_bPadding = Val > 0;

	return S_OK;
}

STDMETHODIMP CBCipher::Create(BSTR bstrAlgo, short iMode)
{
	int i;

	EVP_CIPHER_CTX_cleanup(&m_ctx);

	int iAlgo = -1;
	for(i = 0; i < sizeof(s_CipherAlgos) / sizeof(s_CipherAlgos[0]); i ++)
		if(!_wcsicmp(s_CipherAlgos[i].Name, bstrAlgo))
		{
			iAlgo = i;
			break;
		}

	const EVP_CIPHER *pEVP;

	if(iAlgo == -1)
	{
		pEVP = EVP_get_cipherbyname(CStringA(bstrAlgo));

		if(!pEVP)
			return E_INVALIDARG;
	}else
	{
		switch(iMode)
		{
		case 0:
		case 1:
		case -1:pEVP = s_CipherAlgos[i].ecb();break;
		case 2:pEVP = s_CipherAlgos[i].cbc();break;
		case 3:pEVP = s_CipherAlgos[i].cfb();break;
		case 4:pEVP = s_CipherAlgos[i].ofb();break;
		default:return E_INVALIDARG;
		};
	}

	if(iMode != -1 && iMode != EVP_CIPHER_mode(pEVP))
		return E_INVALIDARG;

	m_iKeySize = EVP_CIPHER_key_length(pEVP);
	m_pKey.Allocate(m_iKeySize);
	m_pIV.Free();

	m_bPadding = TRUE;

	EVP_CipherInit_ex(&m_ctx, pEVP, NULL, m_pKey, m_pIV, 0);

	return S_OK;
}

HRESULT CBCipher::do_crypt(VARIANT varInput, VARIANT varOutput, VARIANT* pVal, int enc)
{
	if(!m_ctx.cipher)return SetErrorInfo(s_strAlgoError);

	if(m_iKeySize == EVP_CIPHER_key_length(m_ctx.cipher))
		EVP_CipherInit_ex(&m_ctx, m_ctx.cipher, NULL, m_pKey, m_pIV, enc);
	else
	{
		EVP_CipherInit_ex(&m_ctx, m_ctx.cipher, NULL, NULL, NULL, enc);
		EVP_CIPHER_CTX_set_key_length(&m_ctx, m_iKeySize);
		EVP_CipherInit_ex(&m_ctx, NULL, NULL, m_pKey, m_pIV, enc);
	}

	if(!m_bPadding)EVP_CIPHER_CTX_set_padding(&m_ctx, 0);

	HRESULT hr;
	CBComPtr<IStream> pSrcStream;
	CBComPtr<IStream> pDescStream1;
	IStream* pDescStream;
	CBTempStream mStream;

	hr = CBStream::GetStream(&varInput, &pSrcStream);
	if(FAILED(hr))return hr;

	if(varOutput.vt != VT_ERROR)
	{
		hr = CBStream::GetStream(&varOutput, &pDescStream1, FALSE);
		if(FAILED(hr))return hr;
		pDescStream = pDescStream1;
	}else pDescStream = &mStream;

	BYTE inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
	ULONG inlen, outlen, wrlen;

	while(1)
	{
		hr = pSrcStream->Read(inbuf, sizeof(inbuf), &inlen);
		if(FAILED(hr))return hr;
		if(inlen == 0)break;

		if(!EVP_CipherUpdate(&m_ctx, outbuf, (int*)&outlen, inbuf, inlen))
			return E_FAIL;

		hr = pDescStream->Write(outbuf, outlen, &wrlen);
		if(FAILED(hr))return hr;
	}

	if(!EVP_CipherFinal_ex(&m_ctx, outbuf, (int*)&outlen))
		return E_FAIL;

	hr = pDescStream->Write(outbuf, outlen, &wrlen);
	if(FAILED(hr))return hr;

	if(mStream.GetLength())
		return mStream.GetVariant(pVal);

	return S_OK;
}

STDMETHODIMP CBCipher::Encrypt(VARIANT varInput, VARIANT varOutput, VARIANT* pVal)
{
	return do_crypt(varInput, varOutput, pVal, 1);
}

STDMETHODIMP CBCipher::Decrypt(VARIANT varInput, VARIANT varOutput, VARIANT* pVal)
{
	return do_crypt(varInput, varOutput, pVal, 0);
}

