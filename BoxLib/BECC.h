#pragma once

class __declspec(uuid("94650000-0000-0000-0000-000000454343"))
CBECC : public CBDispatch<IECC>
{
	DECLARE_CLASS(CBECC)
	APARTMENT_MODE

public:
	CBECC(void) : m_pECC(NULL), m_nPadding(1)
	{}

	~CBECC(void)
	{
		free();
	}

public:
	STDMETHOD(get_IsPrivateKey)(VARIANT_BOOL *pVal);
	STDMETHOD(get_Key)(VARIANT *pVal);
	STDMETHOD(get_KeySize)(short *pVal);
	STDMETHOD(get_Parameter)(VARIANT *pVal);
	STDMETHOD(put_Parameter)(VARIANT newVal);
	STDMETHOD(get_PrivateKey)(VARIANT *pVal);
	STDMETHOD(put_PrivateKey)(VARIANT newVal);
	STDMETHOD(get_PublicKey)(VARIANT *pVal);
	STDMETHOD(put_PublicKey)(VARIANT newVal);
	STDMETHOD(Decrypt)(VARIANT varData, VARIANT *pVal);
	STDMETHOD(Encrypt)(VARIANT varData, VARIANT *pVal);
	STDMETHOD(GenerateKey)(VARIANT varSize = VARIANT_ERROR);

	STDMETHOD(get_DSASize)(short *pVal);
	STDMETHOD(DSASign)(VARIANT varData, VARIANT *pVal);
	STDMETHOD(DSAVerify)(VARIANT varData, VARIANT varSig, VARIANT_BOOL *retVal);

private:
	void *m_pECC;
	short m_nPadding;

	void free(void);
};
