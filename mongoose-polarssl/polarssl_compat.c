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

#include <stdlib.h>

#ifdef NS_ENABLE_POLARSSL

#include "polarssl/entropy.h"
#include "polarssl/ctr_drbg.h"
#include "polarssl/certs.h"
#include "polarssl/x509.h"
#include "polarssl/ssl.h"
#include "polarssl/net.h"
#include "polarssl/error.h"
#if defined(POLARSSL_SSL_CACHE_C)
#include "polarssl/ssl_cache.h"
#endif

#define DEBUG_LEVEL 0
// TODO - fix copy-paste
#define DBG(x) do { printf("[POLAR-SSL] %-20s ", __FUNCTION__); printf x; putchar('\n'); \
  fflush(stdout); } while(0)
// TODO - fix copy-paste
// TODO - remove debug
static void my_debug( void *ctx, int level, const char *str )
{
    ((void) level);

    fprintf( (FILE *) ctx, "%s", str );
    fflush(  (FILE *) ctx  );
}

SSL* SSL_new(SSL_CTX *ctx)
{
    DBG(("SSL_new %p", ctx));
    return &ctx->ssl_ctx;
}

SSL_CTX* SSL_CTX_new(const SSL_METHOD *meth)
{
    int ret;
    SSL_CTX* ctx = (SSL_CTX *) calloc(1,sizeof(SSL_CTX));
    DBG(("SSL_CTX_new %p", meth));
    if( ( ret = ssl_init( &ctx->ssl_ctx) ) != 0 )
    {
        printf( " failed\n  ! ssl_init returned %d\n\n", ret );
        return NULL;
    }
    ssl_set_endpoint( &ctx->ssl_ctx, SSL_IS_SERVER );
    ssl_set_authmode( &ctx->ssl_ctx, SSL_VERIFY_NONE );

    ssl_set_rng( &ctx->ssl_ctx, ctr_drbg_random, &ctx->ctr_drbg );
    ssl_set_dbg( &ctx->ssl_ctx, my_debug, stdout );

#if defined(POLARSSL_SSL_CACHE_C)
    ssl_set_session_cache( &ctx->ssl_ctx, ssl_cache_get, &ctx->cache,
                                 ssl_cache_set, &ctx->cache );
#endif

    return ctx;
}
void  SSL_CTX_free(SSL_CTX *ssl)
{
    DBG(("SSL_CTX_free %p", ssl));
#if defined(POLARSSL_SSL_CACHE_C)
    ssl_cache_free( &ssl->cache );
#endif
    ctr_drbg_free( &ssl->ctr_drbg);
    x509_crt_free(&ssl->cert);
    pk_free(&ssl->key);
    free(ssl);
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
int	  SSL_CTX_use_certificate_file(SSL_CTX *ctx, const char *file, int type) 
{
  DBG((__FUNCTION__)); 
  x509_crt_init( &ctx->cert);
  if(type == SSL_FILETYPE_PEM)
   if( x509_crt_parse_file( &ctx->cert, file ) == 0) 
      return 1;
  return 0;
}
int	  SSL_CTX_use_PrivateKey_file(SSL_CTX *ctx, const char *file, int type) 
{ 
  DBG((__FUNCTION__)); 
  pk_init( &ctx->key );
  if(type == SSL_FILETYPE_PEM)
   if( pk_parse_keyfile( &ctx->key, file, "" ) == 0 )
       return 1;
   return 0;
}
int	  SSL_CTX_use_certificate_chain_file(SSL_CTX *ctx, const char *file) { DBG((__FUNCTION__)); }
const SSL_METHOD *SSLv23_client_method(void) { DBG((__FUNCTION__)); }

int	  SSL_set_fd(SSL *s, int fd) 
{
     DBG((__FUNCTION__)); 
     ssl_set_bio(s, net_recv, &fd,
                     net_send, &fd);
     return 1; // TODO - fix magic 
}
int   SSL_accept(SSL *ssl) 
{
    int ret;
    DBG((__FUNCTION__)); 
    while( ( ret = ssl_handshake( ssl ) ) != 0 )
    {
        if( ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE )
        {
            printf( " failed\n  ! ssl_handshake returned %d\n\n", ret );
            return 0;
        }
    }
    return 1;
/*
if( ( ret = net_accept( listen_fd, &client_fd, NULL ) ) != 0 )
    {
        printf( " failed\n  ! net_accept returned %d\n\n", ret );
        goto exit;
    }
*/

}
int   SSL_connect(SSL *ssl) { DBG((__FUNCTION__)); }
int   SSL_read(SSL *ssl,void *buf,int num) { DBG((__FUNCTION__)); }
int   SSL_write(SSL *ssl,const void *buf,int num) { DBG((__FUNCTION__)); }
long  SSL_CTX_ctrl(SSL_CTX *ctx,int cmd, long larg, void *parg) { DBG((__FUNCTION__)); }



static entropy_context g_entropy;
static ssl_context g_ssl;

void SSL_library_destroy(void)
{
    DBG(("SSL_library_destroy"));
    ssl_free( &g_ssl );
    entropy_free( &g_entropy );
}
int SSL_library_init(void)
{
    int ret = 0;
    DBG(("SSL_library_init"));
    memset( &g_ssl, 0, sizeof(ssl_context) );
    entropy_init( &g_entropy );
#if defined(POLARSSL_DEBUG_C)
    debug_set_threshold( DEBUG_LEVEL );
#endif

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
