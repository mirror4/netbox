
#pragma once

#include <openssl\rsa.h>

// CBoxRSA command target

class CBoxRSA : public CBoxSafeObject
{
	DECLARE_DYNCREATE(CBoxRSA)
public:
	CBoxRSA(void) :
		m_pRSA(NULL),
		m_nPadding(1)
	{}

	~CBoxRSA(void)
	{
		free();
	}

private:
	void free(void)
	{
		if(m_pRSA != NULL)
			RSA_free(m_pRSA);
	}

public:
	afx_msg void GenerateKey(VARIANT &var);
	afx_msg long get_KeySize(void);

	afx_msg BOOL get_IsPrivateKey(void);
	afx_msg VARIANT get_PrivateKey(void);
	afx_msg void put_PrivateKey(VARIANT &var);

	afx_msg VARIANT get_PublicKey(void);
	afx_msg void put_PublicKey(VARIANT &var);

	afx_msg VARIANT get_Key(void);

	afx_msg VARIANT Encrypt(VARIANT& var);
	afx_msg VARIANT Decrypt(VARIANT& var);

	DECLARE_DISPATCH_MAP()

private:
	RSA *m_pRSA;
	long m_nPadding;
};


