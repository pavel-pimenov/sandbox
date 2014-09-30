
#ifndef POLARSSL_HEADER_INCLUDED
#define  POLARSSL_HEADER_INCLUDED

#define NS_ENABLE_POLARSSL

#ifdef NS_ENABLE_POLARSSL

#include "polarssl/ssl.h"
#include "polarssl/ctr_drbg.h"
#if defined(POLARSSL_SSL_CACHE_C)
#include "polarssl/ssl_cache.h"
#endif

struct MG_SSL_CTX
{
    ssl_context ssl_ctx;
    x509_crt    cert;
    pk_context  key; 
    ctr_drbg_context ctr_drbg;
#ifdef POLARSSL_SSL_CACHE_C
    ssl_cache_context cache;
#endif
};
struct MG_X509_STORE_CTX
{
    void* dummy;
};
struct SSL_METHOD
{
    void* dummy;
};
typedef struct ssl_context SSL;  
typedef struct MG_SSL_CTX SSL_CTX; 
typedef struct MG_X509_STORE_CTX X509_STORE_CTX; 
typedef struct MG_SSL_METHOD SSL_METHOD;

long  SSL_CTX_ctrl(SSL_CTX *ctx,int cmd, long larg, void *parg);
SSL_CTX* SSL_CTX_new(const SSL_METHOD *meth);
SSL*     SSL_new(SSL_CTX *ctx);

int   SSL_library_init(void);
int	  SSL_get_error(const SSL *s,int ret_code);

void  SSL_free(SSL *ssl);
void  SSL_CTX_free(SSL_CTX *ssl_ctx);
void  SSL_CTX_set_verify(SSL_CTX *ctx,int mode,int (*callback)(int, X509_STORE_CTX *));
int   SSL_CTX_load_verify_locations(SSL_CTX *ctx, const char *CAfile,	const char *CApath);
int	  SSL_CTX_use_certificate_file(SSL_CTX *ctx, const char *file, int type);
int	  SSL_CTX_use_PrivateKey_file(SSL_CTX *ctx, const char *file, int type);
int	  SSL_CTX_use_certificate_chain_file(SSL_CTX *ctx, const char *file);
const SSL_METHOD *SSLv23_server_method(void);	
const SSL_METHOD *SSLv23_client_method(void);

int	  SSL_set_fd(SSL *s, int fd);
int   SSL_accept(SSL *ssl);
int   SSL_connect(SSL *ssl);
int   SSL_read(SSL *ssl,void *buf,int num);
int   SSL_write(SSL *ssl,const void *buf,int num);

#define SSL_VERIFY_PEER			0x01
#define SSL_VERIFY_FAIL_IF_NO_PEER_CERT	0x02

#define SSL_ERROR_WANT_READ		2
#define SSL_ERROR_WANT_WRITE		3
#define SSL_CTRL_MODE				33

#define X509_FILETYPE_PEM	1
#define SSL_FILETYPE_PEM	X509_FILETYPE_PEM


#define SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER 0x00000002L
#define SSL_CTX_set_mode(ctx,op) \
	SSL_CTX_ctrl((ctx),SSL_CTRL_MODE,(op),NULL)

#endif

#endif
