#pragma once

#include <openssl\ssl.h>

class CSSLContext
{
public:
	CSSLContext(void) : m_pSSLContext(NULL)
	{
		m_pSSLContext = SSL_CTX_new(SSLv23_method());

		set_options(SSL_OP_ALL);
		set_session_cache_mode(SSL_SESS_CACHE_BOTH);
	}

	~CSSLContext(void)
	{	if(m_pSSLContext != NULL)SSL_CTX_free(m_pSSLContext);}

	operator SSL_CTX*(void)
	{	return m_pSSLContext;}

	SSL_CTX* operator=(SSL_CTX* p)
	{
		if(m_pSSLContext != NULL)
			SSL_CTX_free(m_pSSLContext);

		m_pSSLContext = p;
		return m_pSSLContext;
	}

	SSL_CTX* operator=(CSSLContext& p)
	{	return operator=(p.m_pSSLContext);}


	int add_client_CA(X509 *x)
	{	return SSL_CTX_add_client_CA(m_pSSLContext, x);}

	long add_extra_chain_cert(X509 *x509)
	{	return SSL_CTX_add_extra_chain_cert(m_pSSLContext, x509);}

	int add_session(SSL_SESSION *c)
	{	return SSL_CTX_add_session(m_pSSLContext, c);}

	int check_private_key()
	{	return SSL_CTX_check_private_key(m_pSSLContext);}

	long ctrl(int cmd, long larg, char *parg)
	{	return SSL_CTX_ctrl(m_pSSLContext, cmd, larg, parg);}

	void flush_sessions(long tm)
	{	SSL_CTX_flush_sessions(m_pSSLContext, tm);}

	char *get_app_data()
	{	return (char *)SSL_CTX_get_app_data(m_pSSLContext);}

	X509_STORE *get_cert_store()
	{	return SSL_CTX_get_cert_store(m_pSSLContext);}

	STACK *get_client_CA_list()
	{	return SSL_CTX_get_client_CA_list(m_pSSLContext);}

	char *get_ex_data(int idx)
	{	return (char *)SSL_CTX_get_ex_data(m_pSSLContext, idx);}

	int get_quiet_shutdown()
	{	return SSL_CTX_get_quiet_shutdown(m_pSSLContext);}

	int get_session_cache_mode()
	{	return SSL_CTX_get_session_cache_mode(m_pSSLContext);}

	long get_timeout()
	{	return SSL_CTX_get_timeout(m_pSSLContext);}

	int get_verify_mode()
	{	return SSL_CTX_get_verify_mode(m_pSSLContext);}

	int load_verify_locations(char *CAfile, char *CApath)
	{	return SSL_CTX_load_verify_locations(m_pSSLContext, CAfile, CApath);}

	long need_tmp_RSA()
	{	return SSL_CTX_need_tmp_RSA(m_pSSLContext);}

	SSL *newConnect()
	{	return SSL_new(m_pSSLContext);}

	int remove_session(SSL_SESSION *c)
	{	return SSL_CTX_remove_session(m_pSSLContext, c);}

	int sess_accept()
	{	return SSL_CTX_sess_accept(m_pSSLContext);}

	int sess_accept_good()
	{	return SSL_CTX_sess_accept_good(m_pSSLContext);}

	int sess_accept_renegotiate()
	{	return SSL_CTX_sess_accept_renegotiate(m_pSSLContext);}

	int sess_cache_full()
	{	return SSL_CTX_sess_cache_full(m_pSSLContext);}

	int sess_cb_hits()
	{	return SSL_CTX_sess_cb_hits(m_pSSLContext);}

	int sess_connect()
	{	return SSL_CTX_sess_connect(m_pSSLContext);}

	int sess_connect_good()
	{	return SSL_CTX_sess_connect_good(m_pSSLContext);}

	int sess_connect_renegotiate()
	{	return SSL_CTX_sess_connect_renegotiate(m_pSSLContext);}

	int sess_get_cache_size()
	{	return SSL_CTX_sess_get_cache_size(m_pSSLContext);}

	int sess_hits()
	{	return SSL_CTX_sess_hits(m_pSSLContext);}

	int sess_misses()
	{	return SSL_CTX_sess_misses(m_pSSLContext);}

	int sess_number()
	{	return SSL_CTX_sess_number(m_pSSLContext);}

	void sess_set_cache_size(long t)
	{	SSL_CTX_sess_set_cache_size(m_pSSLContext,t);}

	int sess_timeouts()
	{	return SSL_CTX_sess_timeouts(m_pSSLContext);}

    LHASH *sessions()
	{	return SSL_CTX_sessions(m_pSSLContext);}

    void set_app_data(void *arg)
	{	SSL_CTX_set_app_data(m_pSSLContext, arg);}

    void set_cert_store(X509_STORE *cs)
	{	SSL_CTX_set_cert_store(m_pSSLContext, cs);}

    int set_cipher_list(char *str)
	{	return SSL_CTX_set_cipher_list(m_pSSLContext, str);}

    void set_client_CA_list(STACK *list)
	{	SSL_CTX_set_client_CA_list(m_pSSLContext, list);}

    void set_default_read_ahead(int m)
	{	SSL_CTX_set_default_read_ahead(m_pSSLContext, m);}

    int set_default_verify_paths()
	{	return SSL_CTX_set_default_verify_paths(m_pSSLContext);}

    int set_ex_data(int idx, char *arg)
	{	return SSL_CTX_set_ex_data(m_pSSLContext, idx, arg);}

    void set_options(unsigned long op)
	{	SSL_CTX_set_options(m_pSSLContext, op);}

    void set_quiet_shutdown(int mode)
	{	SSL_CTX_set_quiet_shutdown(m_pSSLContext, mode);}

    void set_session_cache_mode(int mode)
	{	SSL_CTX_set_session_cache_mode(m_pSSLContext, mode);}

    int set_ssl_version(SSL_METHOD *meth)
	{	return SSL_CTX_set_ssl_version(m_pSSLContext, meth);}

    void set_timeout(long t)
	{	SSL_CTX_set_timeout(m_pSSLContext, t);}

    long set_tmp_dh(DH *dh)
	{	return SSL_CTX_set_tmp_dh(m_pSSLContext, dh);}

    long set_tmp_rsa(RSA *rsa)
	{	return SSL_CTX_set_tmp_rsa(m_pSSLContext, rsa);}

    int use_PrivateKey(EVP_PKEY *pkey)
	{	return SSL_CTX_use_PrivateKey(m_pSSLContext, pkey);}

    int use_PrivateKey_ASN1(int type, unsigned char *d, long len)
	{	return SSL_CTX_use_PrivateKey_ASN1(type, m_pSSLContext, d, len);}

    int use_PrivateKey_file(const char *file, int type = SSL_FILETYPE_PEM)
	{	return SSL_CTX_use_PrivateKey_file(m_pSSLContext, (char*)file, type);}

    int use_RSAPrivateKey(RSA *rsa)
	{	return SSL_CTX_use_RSAPrivateKey(m_pSSLContext, rsa);}

    int use_RSAPrivateKey_ASN1(unsigned char *d, long len)
	{	return SSL_CTX_use_RSAPrivateKey_ASN1(m_pSSLContext, d, len);}

    int use_RSAPrivateKey_file(const char *file, int type = SSL_FILETYPE_PEM)
	{	return SSL_CTX_use_RSAPrivateKey_file(m_pSSLContext, (char*)file, type);}

    int use_certificate(X509 *x)
	{	return SSL_CTX_use_certificate(m_pSSLContext, x);}

    int use_certificate_ASN1(int len, unsigned char *d)
	{	return SSL_CTX_use_certificate_ASN1(m_pSSLContext, len, d);}

    int use_certificate_file(const char *file, int type = SSL_FILETYPE_PEM)
	{	return SSL_CTX_use_certificate_file(m_pSSLContext, (char*)file, type);}

private:
	SSL_CTX* m_pSSLContext;
};
