#pragma once

#include "SSLContext.h"

class CSSLConnect
{
public:

	CSSLConnect() : m_pSSLConnect(NULL)
	{}

	~CSSLConnect(void)
	{
		close();
	}

	operator SSL*(void)
	{	return m_pSSLConnect;}

	SSL* operator=(SSL* p)
	{
		close();

		m_pSSLConnect = p;
		return m_pSSLConnect;
	}

	SSL* operator=(CSSLConnect& p)
	{
		return operator=(p.m_pSSLConnect);
	}


	int accept()
	{	return SSL_accept(m_pSSLConnect);}

	int add_client_CA(X509 *x)
	{	return SSL_add_client_CA(m_pSSLConnect, x);}

    int check_private_key()
	{	return SSL_check_private_key(m_pSSLConnect);}

	int clear()
	{	return SSL_clear(m_pSSLConnect);}
	

	long clear_num_renegotiations()
	{	return SSL_clear_num_renegotiations(m_pSSLConnect);}

	void close()
	{
		if(m_pSSLConnect != NULL)
		{
			shutdown();
			free();
		}
	}

	int connect()
	{	return SSL_connect(m_pSSLConnect);}

	void copy_session_id(SSL *f)
	{	SSL_copy_session_id(m_pSSLConnect, f);}

	void create(CSSLContext& SSLContext)
	{
		close();
		m_pSSLConnect = SSLContext.newConnect();
	}

	long ctrl(int cmd, long larg, char *parg)
	{	return SSL_ctrl(m_pSSLConnect, cmd, larg, parg);}

	int do_handshake()
	{	return SSL_do_handshake(m_pSSLConnect);}

	SSL *dup()
	{	return SSL_dup(m_pSSLConnect);}

	void free()
	{
		if(m_pSSLConnect != NULL)
		{
			SSL_free(m_pSSLConnect);
			m_pSSLConnect = NULL;
		}
	}

	SSL_CTX *get_SSL_CTX()
	{	return SSL_get_SSL_CTX(m_pSSLConnect);}

	char *get_app_data()
	{	return (char *)SSL_get_app_data(m_pSSLConnect);}

	X509 *get_certificate()
	{	return SSL_get_certificate(m_pSSLConnect);}

	const char *get_cipher()
	{	return SSL_get_cipher(m_pSSLConnect);}

	int get_cipher_bits(int *alg_bits)
	{	return SSL_get_cipher_bits(m_pSSLConnect, alg_bits);}

	const char *get_cipher_list(int n)
	{	return SSL_get_cipher_list(m_pSSLConnect, n);}

	const char *get_cipher_name()
	{	return SSL_get_cipher_name(m_pSSLConnect);}

	char *get_cipher_version()
	{	return SSL_get_cipher_version(m_pSSLConnect);}

	STACK *get_ciphers()
	{	return SSL_get_ciphers(m_pSSLConnect);}

	STACK *get_client_CA_list()
	{	return SSL_get_client_CA_list(m_pSSLConnect);}

	SSL_CIPHER *get_current_cipher()
	{	return SSL_get_current_cipher(m_pSSLConnect);}

	long get_default_timeout()
	{	return SSL_get_default_timeout(m_pSSLConnect);}

	int get_error(int i)
	{	return SSL_get_error(m_pSSLConnect, i);}

	char *get_ex_data(int idx)
	{	return (char *)SSL_get_ex_data(m_pSSLConnect, idx);}

	int get_fd()
	{	return SSL_get_fd(m_pSSLConnect);}

	STACK *get_peer_cert_chain()
	{	return SSL_get_peer_cert_chain(m_pSSLConnect);}

	X509 *get_peer_certificate()
	{	return SSL_get_peer_certificate(m_pSSLConnect);}

	EVP_PKEY *get_privatekey()
	{	return SSL_get_privatekey(m_pSSLConnect);}

	int get_quiet_shutdown()
	{	return SSL_get_quiet_shutdown(m_pSSLConnect);}

	BIO *get_rbio()
	{	return SSL_get_rbio(m_pSSLConnect);}

	int get_read_ahead()
	{	return SSL_get_read_ahead(m_pSSLConnect);}

	SSL_SESSION *get_session()
	{	return SSL_get_session(m_pSSLConnect);}

	char *get_shared_ciphers(char *buf, int len)
	{	return SSL_get_shared_ciphers(m_pSSLConnect, buf, len);}

	int get_shutdown()
	{	return SSL_get_shutdown(m_pSSLConnect);}

	SSL_METHOD *get_ssl_method()
	{	return SSL_get_ssl_method(m_pSSLConnect);}

	int get_state()
	{	return SSL_get_state(m_pSSLConnect);}

	int get_verify_mode()
	{	return SSL_get_verify_mode(m_pSSLConnect);}

	long get_verify_result()
	{	return SSL_get_verify_result(m_pSSLConnect);}

	const char *get_version()
	{	return SSL_get_version(m_pSSLConnect);}

	BIO *get_wbio()
	{	return SSL_get_wbio(m_pSSLConnect);}

	int in_accept_init()
	{	return SSL_in_accept_init(m_pSSLConnect);}

	int in_before()
	{	return SSL_in_before(m_pSSLConnect);}

	int in_connect_init()
	{	return SSL_in_connect_init(m_pSSLConnect);}

	int in_init()
	{	return SSL_in_init(m_pSSLConnect);}

	int is_init_finished()
	{	return SSL_is_init_finished(m_pSSLConnect);}

	long num_renegotiations()
	{	return SSL_num_renegotiations(m_pSSLConnect);}

	int peek(void *buf, int num)
	{	return SSL_peek(m_pSSLConnect, buf, num);}

	int pending()
	{	return SSL_pending(m_pSSLConnect);}

	int read(void *buf, int num)
	{	return SSL_read(m_pSSLConnect, buf, num);}

	int renegotiate()
	{	return SSL_renegotiate(m_pSSLConnect);}

	const char *rstate_string()
	{	return SSL_rstate_string(m_pSSLConnect);}

	const char *rstate_string_long()
	{	return SSL_rstate_string_long(m_pSSLConnect);}

	long session_reused()
	{	return SSL_session_reused(m_pSSLConnect);}

	void set_accept_state()
	{	SSL_set_accept_state(m_pSSLConnect);}

	void set_app_data(char *arg)
	{	SSL_set_app_data(m_pSSLConnect, arg);}

	void set_bio(BIO *rbio, BIO *wbio)
	{	SSL_set_bio(m_pSSLConnect, rbio, wbio);}

	int set_cipher_list(char *str)
	{	return SSL_set_cipher_list(m_pSSLConnect, str);}

	void set_client_CA_list(STACK *list)
	{	SSL_set_client_CA_list(m_pSSLConnect, list);}

	void set_connect_state()
	{	SSL_set_connect_state(m_pSSLConnect);}

	int set_ex_data(int idx, char *arg)
	{	return SSL_set_ex_data(m_pSSLConnect, idx, arg);}

	int set_fd(int fd)
	{	return SSL_set_fd(m_pSSLConnect, fd);}

	void set_options(unsigned long op)
	{	SSL_set_options(m_pSSLConnect, op);}

	void set_quiet_shutdown(int mode)
	{	SSL_set_quiet_shutdown(m_pSSLConnect, mode);}

	void set_read_ahead(int yes)
	{	SSL_set_read_ahead(m_pSSLConnect, yes);}

	int set_rfd(int fd)
	{	return SSL_set_rfd(m_pSSLConnect, fd);}

	int set_session(SSL_SESSION *session)
	{	return SSL_set_session(m_pSSLConnect, session);}

	void set_shutdown(int mode)
	{	SSL_set_shutdown(m_pSSLConnect, mode);}

	int set_ssl_method(SSL_METHOD *meth)
	{	return SSL_set_ssl_method(m_pSSLConnect, meth);}

	int set_wfd(int fd)
	{	return SSL_set_wfd(m_pSSLConnect, fd);}

	int shutdown()
	{	return SSL_shutdown(m_pSSLConnect);}

	int state()
	{	return SSL_state(m_pSSLConnect);}

	const char *state_string()
	{	return SSL_state_string(m_pSSLConnect);}

	const char *state_string_long()
	{	return SSL_state_string_long(m_pSSLConnect);}

	long total_renegotiations()
	{	return SSL_total_renegotiations(m_pSSLConnect);}

	int use_PrivateKey(EVP_PKEY *pkey)
	{	return SSL_use_PrivateKey(m_pSSLConnect, pkey);}

	int use_PrivateKey_ASN1(int type, unsigned char *d, long len)
	{	return SSL_use_PrivateKey_ASN1(type, m_pSSLConnect, d, len);}

	int use_PrivateKey_file(char *file, int type)
	{	return SSL_use_PrivateKey_file(m_pSSLConnect, file, type);}

	int use_RSAPrivateKey(RSA *rsa)
	{	return SSL_use_RSAPrivateKey(m_pSSLConnect, rsa);}

	int use_RSAPrivateKey_ASN1(unsigned char *d, long len)
	{	return SSL_use_RSAPrivateKey_ASN1(m_pSSLConnect, d, len);}

	int use_RSAPrivateKey_file(char *file, int type)
	{	return SSL_use_RSAPrivateKey_file(m_pSSLConnect, file, type);}

	int use_certificate(X509 *x)
	{	return SSL_use_certificate(m_pSSLConnect, x);}

	int use_certificate_ASN1(unsigned char *d, int len)
	{	return SSL_use_certificate_ASN1(m_pSSLConnect, d, len);}

	int use_certificate_file(char *file, int type)
	{	return SSL_use_certificate_file(m_pSSLConnect, file, type);}

	int version()
	{	return SSL_version(m_pSSLConnect);}

	int want()
	{	return SSL_want(m_pSSLConnect);}

	int want_nothing()
	{	return SSL_want_nothing(m_pSSLConnect);}

	int want_read()
	{	return SSL_want_read(m_pSSLConnect);}

	int want_write()
	{	return SSL_want_write(m_pSSLConnect);}

	int write(const void *buf, int num)
	{	return SSL_write(m_pSSLConnect, buf, num);}

private:
	SSL *m_pSSLConnect;
};
