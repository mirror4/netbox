#pragma once

#include <openssl/evp.h>

class __declspec(uuid("94650000-0000-0000-0000-436970686572"))
CBCipher : public CBDispatch<ICipher>
{
	DECLARE_CLASS(CBCipher)

public:
	CBCipher(void);
	~CBCipher(void);

public:
	// ICipher
	STDMETHOD(get__NewEnum)(IUnknown** ppEnumReturn);
	STDMETHOD(get_Name)(BSTR *pVal);
	STDMETHOD(get_BlockSize)(short *pVal);
	STDMETHOD(get_CipherMode)(short *pVal);
	STDMETHOD(get_KeySize)(short *pVal);
	STDMETHOD(get_Key)(VARIANT *pVal);
	STDMETHOD(put_Key)(VARIANT Val);
	STDMETHOD(GenerateKey)(short iSize);
	STDMETHOD(get_IVSize)(short *pVal);
	STDMETHOD(get_IV)(VARIANT *pVal);
	STDMETHOD(put_IV)(VARIANT Val);
	STDMETHOD(GenerateIV)();
	STDMETHOD(get_Padding)(short *pVal);
	STDMETHOD(put_Padding)(short Val);
	STDMETHOD(Create)(BSTR bstrAlgo = L"DES", short iMode = -1);
	STDMETHOD(Encrypt)(VARIANT varInput, VARIANT varOutput, VARIANT* pVal);
	STDMETHOD(Decrypt)(VARIANT varInput, VARIANT varOutput, VARIANT* pVal);

public:
	ULONG GetEnum(VARIANT *rgVar, ULONG pos, ULONG count);

private:
	HRESULT do_crypt(VARIANT varInput, VARIANT varOutput, VARIANT* pVal, int enc);

private:
	EVP_CIPHER_CTX m_ctx;
	CBAutoPtr<BYTE> m_pKey;
	CBAutoPtr<BYTE> m_pIV;
	int m_iKeySize;
	BOOL m_bPadding;
};

