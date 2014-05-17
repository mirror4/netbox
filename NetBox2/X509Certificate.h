#pragma once

#include <openssl\ssl.h>

class CX509Certificate
{
public:

	CX509Certificate(void) : m_pCertificate(NULL)
	{
	}

	~CX509Certificate(void)
	{
		free();
	}

	operator X509*(void)
	{	return m_pCertificate;}

	X509* operator=(X509* p)
	{
		if(m_pCertificate != NULL)
			X509_free(m_pCertificate);

		m_pCertificate = p;
		return m_pCertificate;
	}

	X509* operator=(CX509Certificate& p)
	{	return operator=(p.m_pCertificate);}


	void free(void)
	{
		if(m_pCertificate != NULL)
		{
			X509_free(m_pCertificate);
			m_pCertificate = NULL;
		}
	}

private:
	X509*    m_pCertificate;
};
