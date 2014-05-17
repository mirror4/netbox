#pragma once

class __declspec(uuid("94650000-0000-0000-0000-000000525341"))
CBRSA : public CBDispatch<IRSA>
{
	DECLARE_CLASS(CBRSA)
	APARTMENT_MODE

public:
	CBRSA(void) : m_pRSA(NULL), m_nPadding(1)
	{}

	~CBRSA(void)
	{
		free();
	}

public:
	STDMETHOD(get_IsPrivateKey)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Key)(VARIANT *pVal);
	STDMETHOD(get_KeySize)(short *pVal);
	STDMETHOD(get_Padding)(short *pVal);
	STDMETHOD(put_Padding)(short newVal);
	STDMETHOD(get_PrivateKey)(VARIANT *pVal);
	STDMETHOD(put_PrivateKey)(VARIANT newVal);
	STDMETHOD(get_PublicKey)(VARIANT *pVal);
	STDMETHOD(put_PublicKey)(VARIANT newVal);
	STDMETHOD(Decrypt)(VARIANT varData, VARIANT *pVal);
	STDMETHOD(Encrypt)(VARIANT varData, VARIANT *pVal);
	STDMETHOD(GenerateKey)(VARIANT varSize = VARIANT_ERROR);

private:
	void *m_pRSA;
	short m_nPadding;

	void free(void);
};
