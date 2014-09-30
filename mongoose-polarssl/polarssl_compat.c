// Copyright (c) 2004-2013 Sergey Lyubka
// Copyright (c) 2013-2014 Cesanta Software Limited
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#include "polarssl_compat.h"

#ifdef NS_ENABLE_POLARSSL

#include "polarssl/entropy.h"
#include "polarssl/ctr_drbg.h"
#include "polarssl/certs.h"
#include "polarssl/x509.h"
#include "polarssl/ssl.h"
#include "polarssl/net.h"
#include "polarssl/error.h"
#include "polarssl/debug.h"
#if defined(POLARSSL_SSL_CACHE_C)
#include "polarssl/ssl_cache.h"
#endif

#define DEBUG_LEVEL 0
// TODO - fix copy-paste
#define DBG(x) do { printf("[POLAR-SSL] %-20s ", __FUNCTION__); printf x; putchar('\n'); \
  fflush(stdout); } while(0)
// TODO - fix copy-paste

SSL* SSL_new(SSL_CTX *ctx)
{
    DBG(("SSL_new %p", ctx));
    return 0;
}
SSL_CTX* SSL_CTX_new(const SSL_METHOD *meth)
{
    DBG(("SSL_CTX_new %p", meth));
    return 0;
}
void  SSL_CTX_free(SSL_CTX *ssl_ctx)
{
    DBG(("SSL_CTX_free %p", ssl_ctx));
}
int	  SSL_get_error(const SSL *s,int ret_code)
{
    DBG(("SSL_get_error %p %d", s, ret_code));
}
const SSL_METHOD *SSLv23_server_method(void)
{
    DBG((__FUNCTION__));
}
void  SSL_free(SSL *ssl) { DBG((__FUNCTION__)); }
void  SSL_CTX_set_verify(SSL_CTX *ctx,int mode,int (*callback)(int, X509_STORE_CTX *)) { DBG((__FUNCTION__)); }
int   SSL_CTX_load_verify_locations(SSL_CTX *ctx, const char *CAfile,	const char *CApath) { DBG((__FUNCTION__)); }
int	  SSL_CTX_use_certificate_file(SSL_CTX *ctx, const char *file, int type) { DBG((__FUNCTION__)); }
int	  SSL_CTX_use_PrivateKey_file(SSL_CTX *ctx, const char *file, int type) { DBG((__FUNCTION__)); }
int	  SSL_CTX_use_certificate_chain_file(SSL_CTX *ctx, const char *file) { DBG((__FUNCTION__)); }
const SSL_METHOD *SSLv23_client_method(void) { DBG((__FUNCTION__)); }

int	  SSL_set_fd(SSL *s, int fd) { DBG((__FUNCTION__)); }
int   SSL_accept(SSL *ssl) { DBG((__FUNCTION__)); }
int   SSL_connect(SSL *ssl) { DBG((__FUNCTION__)); }
int   SSL_read(SSL *ssl,void *buf,int num) { DBG((__FUNCTION__)); }
int   SSL_write(SSL *ssl,const void *buf,int num) { DBG((__FUNCTION__)); }
long  SSL_CTX_ctrl(SSL_CTX *ctx,int cmd, long larg, void *parg) { DBG((__FUNCTION__)); }



static entropy_context g_entropy;
static ctr_drbg_context g_ctr_drbg;
static ssl_context g_ssl;
static x509_crt g_srvcert;
static pk_context g_pkey;
#if defined(POLARSSL_SSL_CACHE_C)
static ssl_cache_context g_cache;
#endif

void SSL_library_destroy(void)
{
    DBG(("SSL_library_destroy"));
    x509_crt_free( &g_srvcert );
    pk_free( &g_pkey );
    ssl_free( &g_ssl );
#if defined(POLARSSL_SSL_CACHE_C)
    ssl_cache_free( &g_cache );
#endif
    ctr_drbg_free( &g_ctr_drbg );
    entropy_free( &g_entropy );
}
int SSL_library_init(void)
{
    int ret = 0;
    DBG(("SSL_library_init"));
    memset( &g_ssl, 0, sizeof(ssl_context) );
    x509_crt_init( &g_srvcert );
    pk_init( &g_pkey );
    entropy_init( &g_entropy );
#if defined(POLARSSL_DEBUG_C)
    debug_set_threshold( DEBUG_LEVEL );
#endif

    if( ( ret = ssl_init( &g_ssl ) ) != 0 )
    {
        printf( " failed\n  ! ssl_init returned %d\n\n", ret );
        return ret;
    }

#if 0
    /*
     * This demonstration program uses embedded test certificates.
     * Instead, you may want to use x509_crt_parse_file() to read the
     * server and CA certificates, as well as pk_parse_keyfile().
     */
    ret = x509_crt_parse( &g_srvcert, (const unsigned char *) test_srv_crt,
                          strlen( test_srv_crt ) );
    if( ret != 0 )
    {
        printf( " failed\n  !  x509_crt_parse returned %d\n\n", ret );
        return ret;
    }

    ret = x509_crt_parse( &g_srvcert, (const unsigned char *) test_ca_list,
                          strlen( test_ca_list ) );
    if( ret != 0 )
    {
        printf( " failed\n  !  x509_crt_parse returned %d\n\n", ret );
        return ret;
    }

    ret =  pk_parse_key( &g_pkey, (const unsigned char *) test_srv_key,
                         strlen( test_srv_key ), NULL, 0 );
    if( ret != 0 )
    {
        printf( " failed\n  !  pk_parse_key returned %d\n\n", ret );
        return ret;
    }
#endif
}
#endif // NS_ENABLE_POLARSSL
