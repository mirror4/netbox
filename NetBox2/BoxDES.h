
#pragma once

#include <openssl\des.h>
// CBoxDES command target

class CBoxDES : public CBoxSafeObject
{
	DECLARE_DYNCREATE(CBoxDES)

public:
	CBoxDES(void);

public:
	afx_msg VARIANT Encrypt(VARIANT& varData);
	afx_msg VARIANT Decrypt(VARIANT& varData);
	afx_msg void GenerateKey(void);
	afx_msg void GenerateIV(void);

	afx_msg void Clear(void);

	afx_msg VARIANT get_IV(void);
	afx_msg void put_IV(VARIANT& varIV);
	afx_msg VARIANT get_Key(void);
	afx_msg void put_Key(VARIANT& varKey);

	afx_msg long get_Mode(void);
	afx_msg void put_Mode(long nMode);

	afx_msg long get_KeySize(void);
	afx_msg long get_BlockSize(void);

	DECLARE_DISPATCH_MAP()

private:
	int do_des(BYTE* in, BYTE* out, int nSize, BOOL e)
	{
		switch(m_nMode)
		{
		case 0:
			des_ecb_encrypt((des_cblock*)in, (des_cblock*)out, m_desKey, e);
			return DES_KEY_SZ;
		case 1:
			{
				int num = 0;

				des_ofb64_encrypt(in, out, nSize, m_desKey, &m_aIV, &num);
			}
			return nSize;
		case 2:
			des_ncbc_encrypt(in, out, nSize, m_desKey, &m_aIV, e);
			return nSize;
		case 3:
			{
				int num = 0;

				des_cfb64_encrypt(in, out, nSize, m_desKey, &m_aIV, &num, e);
			}
			return nSize;
		}

		return -1;
	}

	des_key_schedule m_desKey;

	des_cblock m_aKey;
	des_cblock m_aIV;
	long m_nMode;
};


