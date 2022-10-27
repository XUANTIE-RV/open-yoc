/**
 * \file md_wrap.c
 *
 * \brief Generic message digest wrapper for mbed TLS
 *
 * \author Adriaan de Jong <dejong@fox-it.com>
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if (defined(CONFIG_SEC_CRYPTO_SHA_SW) || defined(CONFIG_SEC_CRYPTO_RSA_SW))

#include "crypto_md_internal.h"
#include "crypto_sha1.h"
#include "crypto_sha256.h"

#include <stdlib.h>
#define mbedtls_calloc    calloc
#define mbedtls_free       free

static void sha1_starts_wrap( void *ctx )
{
    sc_mbedtls_sha1_starts( (sc_mbedtls_sha1_context *) ctx );
}

static void sha1_update_wrap( void *ctx, const unsigned char *input,
                              size_t ilen )
{
    sc_mbedtls_sha1_update( (sc_mbedtls_sha1_context *) ctx, input, ilen );
}

static void sha1_finish_wrap( void *ctx, unsigned char *output )
{
    sc_mbedtls_sha1_finish( (sc_mbedtls_sha1_context *) ctx, output );
}

static void *sha1_ctx_alloc( void )
{
    void *ctx = mbedtls_calloc( 1, sizeof( sc_mbedtls_sha1_context ) );

    if( ctx != NULL )
        sc_mbedtls_sha1_init( (sc_mbedtls_sha1_context *) ctx );

    return( ctx );
}

static void sha1_clone_wrap( void *dst, const void *src )
{
    sc_mbedtls_sha1_clone( (sc_mbedtls_sha1_context *) dst,
                  (const sc_mbedtls_sha1_context *) src );
}

static void sha1_ctx_free( void *ctx )
{
    sc_mbedtls_sha1_free( (sc_mbedtls_sha1_context *) ctx );
    mbedtls_free( ctx );
}

static void sha1_process_wrap( void *ctx, const unsigned char *data )
{
    sc_mbedtls_sha1_process( (sc_mbedtls_sha1_context *) ctx, data );
}

const sc_mbedtls_md_info_t sc_mbedtls_sha1_info = {
    SC_MBEDTLS_MD_SHA1,
    "SHA1",
    20,
    64,
    sha1_starts_wrap,
    sha1_update_wrap,
    sha1_finish_wrap,
    sc_mbedtls_sha1,
    sha1_ctx_alloc,
    sha1_ctx_free,
    sha1_clone_wrap,
    sha1_process_wrap,
};

#if defined(CONFIG_SEC_CRYPTO_SHA_SW)
static void sha224_starts_wrap( void *ctx )
{
    sc_mbedtls_sha256_starts( (sc_mbedtls_sha256_context *) ctx, 1 );
}

static void sha224_update_wrap( void *ctx, const unsigned char *input,
                                size_t ilen )
{
    sc_mbedtls_sha256_update( (sc_mbedtls_sha256_context *) ctx, input, ilen );
}

static void sha224_finish_wrap( void *ctx, unsigned char *output )
{
    sc_mbedtls_sha256_finish( (sc_mbedtls_sha256_context *) ctx, output );
}

static void sha224_wrap( const unsigned char *input, size_t ilen,
                    unsigned char *output )
{
    sc_mbedtls_sha256( input, ilen, output, 1 );
}

static void *sha224_ctx_alloc( void )
{
    void *ctx = mbedtls_calloc( 1, sizeof( sc_mbedtls_sha256_context ) );

    if( ctx != NULL )
        sc_mbedtls_sha256_init( (sc_mbedtls_sha256_context *) ctx );

    return( ctx );
}

static void sha224_ctx_free( void *ctx )
{
    sc_mbedtls_sha256_free( (sc_mbedtls_sha256_context *) ctx );
    mbedtls_free( ctx );
}

static void sha224_clone_wrap( void *dst, const void *src )
{
    sc_mbedtls_sha256_clone( (sc_mbedtls_sha256_context *) dst,
                    (const sc_mbedtls_sha256_context *) src );
}

static void sha224_process_wrap( void *ctx, const unsigned char *data )
{
    sc_mbedtls_sha256_process( (sc_mbedtls_sha256_context *) ctx, data );
}

const sc_mbedtls_md_info_t sc_mbedtls_sha224_info = {
    SC_MBEDTLS_MD_SHA224,
    "SHA224",
    28,
    64,
    sha224_starts_wrap,
    sha224_update_wrap,
    sha224_finish_wrap,
    sha224_wrap,
    sha224_ctx_alloc,
    sha224_ctx_free,
    sha224_clone_wrap,
    sha224_process_wrap,
};

static void sha256_starts_wrap( void *ctx )
{
    sc_mbedtls_sha256_starts( (sc_mbedtls_sha256_context *) ctx, 0 );
}

static void sha256_wrap( const unsigned char *input, size_t ilen,
                    unsigned char *output )
{
    sc_mbedtls_sha256( input, ilen, output, 0 );
}

const sc_mbedtls_md_info_t sc_mbedtls_sha256_info = {
    SC_MBEDTLS_MD_SHA256,
    "SHA256",
    32,
    64,
    sha256_starts_wrap,
    sha224_update_wrap,
    sha224_finish_wrap,
    sha256_wrap,
    sha224_ctx_alloc,
    sha224_ctx_free,
    sha224_clone_wrap,
    sha224_process_wrap,
};
#else
const sc_mbedtls_md_info_t sc_mbedtls_sha224_info = {
    SC_MBEDTLS_MD_SHA224,
    "SHA224",
    28,
    64,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

const sc_mbedtls_md_info_t sc_mbedtls_sha256_info = {
    SC_MBEDTLS_MD_SHA256,
    "SHA256",
    32,
    64,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};
#endif
#endif