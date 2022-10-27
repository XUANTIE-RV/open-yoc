/**
 * \file md.h
 *
 * \brief Generic message digest wrapper
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
#ifndef SC_SC_MBEDTLS_MD_H
#define SC_SC_MBEDTLS_MD_H

#include <stddef.h>

#include "sec_crypto_errcode.h"

#define SC_MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE                SC_FEATURE_UNAVAILABLE  /**< The selected feature is not available. */
#define SC_MBEDTLS_ERR_MD_BAD_INPUT_DATA                     SC_BAD_INPUT_DATA  /**< Bad input parameters to function. */
#define SC_MBEDTLS_ERR_MD_ALLOC_FAILED                       SC_ALLOC_FAILED  /**< Failed to allocate memory. */
#define SC_MBEDTLS_ERR_MD_FILE_IO_ERROR                      SC_FAIL  /**< Opening or reading of file failed. */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SC_MBEDTLS_MD_NONE=0,
    SC_MBEDTLS_MD_MD2,
    SC_MBEDTLS_MD_MD4,
    SC_MBEDTLS_MD_MD5,
    SC_MBEDTLS_MD_SHA1,
    SC_MBEDTLS_MD_SHA224,
    SC_MBEDTLS_MD_SHA256,
    SC_MBEDTLS_MD_SHA384,
    SC_MBEDTLS_MD_SHA512,
    SC_MBEDTLS_MD_RIPEMD160,
} sc_mbedtls_md_type_t;

#if defined(MBEDTLS_SHA512_C)
#define SC_MBEDTLS_MD_MAX_SIZE         64  /* longest known is SHA512 */
#else
#define SC_MBEDTLS_MD_MAX_SIZE         32  /* longest known is SHA256 or less */
#endif

/**
 * Opaque struct defined in md_internal.h
 */
typedef struct sc_mbedtls_md_info_t sc_mbedtls_md_info_t;

/**
 * Generic message digest context.
 */
typedef struct {
    /** Information about the associated message digest */
    const sc_mbedtls_md_info_t *md_info;

    /** Digest-specific context */
    void *md_ctx;

    /** HMAC part of the context */
    void *hmac_ctx;
} sc_mbedtls_md_context_t;

/**
 * \brief Returns the list of digests supported by the generic digest module.
 *
 * \return          a statically allocated array of digests, the last entry
 *                  is 0.
 */
const int *sc_mbedtls_md_list( void );

/**
 * \brief           Returns the message digest information associated with the
 *                  given digest name.
 *
 * \param md_name   Name of the digest to search for.
 *
 * \return          The message digest information associated with md_name or
 *                  NULL if not found.
 */
const sc_mbedtls_md_info_t *sc_mbedtls_md_info_from_string( const char *md_name );

/**
 * \brief           Returns the message digest information associated with the
 *                  given digest type.
 *
 * \param md_type   type of digest to search for.
 *
 * \return          The message digest information associated with md_type or
 *                  NULL if not found.
 */
const sc_mbedtls_md_info_t *sc_mbedtls_md_info_from_type( sc_mbedtls_md_type_t md_type );

/**
 * \brief           Initialize a md_context (as NONE)
 *                  This should always be called first.
 *                  Prepares the context for sc_mbedtls_md_setup() or sc_mbedtls_md_free().
 */
void sc_mbedtls_md_init( sc_mbedtls_md_context_t *ctx );

/**
 * \brief           Free and clear the internal structures of ctx.
 *                  Can be called at any time after sc_mbedtls_md_init().
 *                  Mandatory once sc_mbedtls_md_setup() has been called.
 */
void sc_mbedtls_md_free( sc_mbedtls_md_context_t *ctx );

#if ! defined(MBEDTLS_DEPRECATED_REMOVED)
#if defined(MBEDTLS_DEPRECATED_WARNING)
#define MBEDTLS_DEPRECATED    __attribute__((deprecated))
#else
#define MBEDTLS_DEPRECATED
#endif
/**
 * \brief           Select MD to use and allocate internal structures.
 *                  Should be called after sc_mbedtls_md_init() or sc_mbedtls_md_free().
 *                  Makes it necessary to call sc_mbedtls_md_free() later.
 *
 * \deprecated      Superseded by sc_mbedtls_md_setup() in 2.0.0
 *
 * \param ctx       Context to set up.
 * \param md_info   Message digest to use.
 *
 * \returns         \c 0 on success,
 *                  \c SC_MBEDTLS_ERR_MD_BAD_INPUT_DATA on parameter failure,
 *                  \c SC_MBEDTLS_ERR_MD_ALLOC_FAILED memory allocation failure.
 */
int sc_mbedtls_md_init_ctx( sc_mbedtls_md_context_t *ctx, const sc_mbedtls_md_info_t *md_info ) MBEDTLS_DEPRECATED;
#undef MBEDTLS_DEPRECATED
#endif /* MBEDTLS_DEPRECATED_REMOVED */

/**
 * \brief           Select MD to use and allocate internal structures.
 *                  Should be called after sc_mbedtls_md_init() or sc_mbedtls_md_free().
 *                  Makes it necessary to call sc_mbedtls_md_free() later.
 *
 * \param ctx       Context to set up.
 * \param md_info   Message digest to use.
 * \param hmac      0 to save some memory if HMAC will not be used,
 *                  non-zero is HMAC is going to be used with this context.
 *
 * \returns         \c 0 on success,
 *                  \c SC_MBEDTLS_ERR_MD_BAD_INPUT_DATA on parameter failure,
 *                  \c SC_MBEDTLS_ERR_MD_ALLOC_FAILED memory allocation failure.
 */
int sc_mbedtls_md_setup( sc_mbedtls_md_context_t *ctx, const sc_mbedtls_md_info_t *md_info, int hmac );

/**
 * \brief           Clone the state of an MD context
 *
 * \note            The two contexts must have been setup to the same type
 *                  (cloning from SHA-256 to SHA-512 make no sense).
 *
 * \warning         Only clones the MD state, not the HMAC state! (for now)
 *
 * \param dst       The destination context
 * \param src       The context to be cloned
 *
 * \return          \c 0 on success,
 *                  \c SC_MBEDTLS_ERR_MD_BAD_INPUT_DATA on parameter failure.
 */
int sc_mbedtls_md_clone( sc_mbedtls_md_context_t *dst,
                      const sc_mbedtls_md_context_t *src );

/**
 * \brief           Returns the size of the message digest output.
 *
 * \param md_info   message digest info
 *
 * \return          size of the message digest output in bytes.
 */
unsigned char sc_mbedtls_md_get_size( const sc_mbedtls_md_info_t *md_info );

/**
 * \brief           Returns the type of the message digest output.
 *
 * \param md_info   message digest info
 *
 * \return          type of the message digest output.
 */
sc_mbedtls_md_type_t sc_mbedtls_md_get_type( const sc_mbedtls_md_info_t *md_info );

/**
 * \brief           Returns the name of the message digest output.
 *
 * \param md_info   message digest info
 *
 * \return          name of the message digest output.
 */
const char *sc_mbedtls_md_get_name( const sc_mbedtls_md_info_t *md_info );

/**
 * \brief           Prepare the context to digest a new message.
 *                  Generally called after sc_mbedtls_md_setup() or sc_mbedtls_md_finish().
 *                  Followed by sc_mbedtls_md_update().
 *
 * \param ctx       generic message digest context.
 *
 * \returns         0 on success, SC_MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int sc_mbedtls_md_starts( sc_mbedtls_md_context_t *ctx );

/**
 * \brief           Generic message digest process buffer
 *                  Called between sc_mbedtls_md_starts() and sc_mbedtls_md_finish().
 *                  May be called repeatedly.
 *
 * \param ctx       Generic message digest context
 * \param input     buffer holding the  datal
 * \param ilen      length of the input data
 *
 * \returns         0 on success, SC_MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int sc_mbedtls_md_update( sc_mbedtls_md_context_t *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief           Generic message digest final digest
 *                  Called after sc_mbedtls_md_update().
 *                  Usually followed by sc_mbedtls_md_free() or sc_mbedtls_md_starts().
 *
 * \param ctx       Generic message digest context
 * \param output    Generic message digest checksum result
 *
 * \returns         0 on success, SC_MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int sc_mbedtls_md_finish( sc_mbedtls_md_context_t *ctx, unsigned char *output );

/**
 * \brief          Output = message_digest( input buffer )
 *
 * \param md_info  message digest info
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   Generic message digest checksum result
 *
 * \returns        0 on success, SC_MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                 verification fails.
 */
int sc_mbedtls_md( const sc_mbedtls_md_info_t *md_info, const unsigned char *input, size_t ilen,
        unsigned char *output );


/**
 * \brief           Set HMAC key and prepare to authenticate a new message.
 *                  Usually called after sc_mbedtls_md_setup() or sc_mbedtls_md_hmac_finish().
 *
 * \param ctx       HMAC context
 * \param key       HMAC secret key
 * \param keylen    length of the HMAC key in bytes
 *
 * \returns         0 on success, SC_MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int sc_mbedtls_md_hmac_starts( sc_mbedtls_md_context_t *ctx, const unsigned char *key,
                    size_t keylen );

/**
 * \brief           Generic HMAC process buffer.
 *                  Called between sc_mbedtls_md_hmac_starts() or sc_mbedtls_md_hmac_reset()
 *                  and sc_mbedtls_md_hmac_finish().
 *                  May be called repeatedly.
 *
 * \param ctx       HMAC context
 * \param input     buffer holding the  data
 * \param ilen      length of the input data
 *
 * \returns         0 on success, SC_MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int sc_mbedtls_md_hmac_update( sc_mbedtls_md_context_t *ctx, const unsigned char *input,
                    size_t ilen );

/**
 * \brief           Output HMAC.
 *                  Called after sc_mbedtls_md_hmac_update().
 *                  Usually followed by sc_mbedtls_md_hmac_reset(),
 *                  sc_mbedtls_md_hmac_starts(), or sc_mbedtls_md_free().
 *
 * \param ctx       HMAC context
 * \param output    Generic HMAC checksum result
 *
 * \returns         0 on success, SC_MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int sc_mbedtls_md_hmac_finish( sc_mbedtls_md_context_t *ctx, unsigned char *output);

/**
 * \brief           Prepare to authenticate a new message with the same key.
 *                  Called after sc_mbedtls_md_hmac_finish() and before
 *                  sc_mbedtls_md_hmac_update().
 *
 * \param ctx       HMAC context to be reset
 *
 * \returns         0 on success, SC_MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                  verification fails.
 */
int sc_mbedtls_md_hmac_reset( sc_mbedtls_md_context_t *ctx );

/**
 * \brief          Output = Generic_HMAC( hmac key, input buffer )
 *
 * \param md_info  message digest info
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key in bytes
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   Generic HMAC-result
 *
 * \returns        0 on success, SC_MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                 verification fails.
 */
int sc_mbedtls_md_hmac( const sc_mbedtls_md_info_t *md_info, const unsigned char *key, size_t keylen,
                const unsigned char *input, size_t ilen,
                unsigned char *output );

/* Internal use */
int sc_mbedtls_md_process( sc_mbedtls_md_context_t *ctx, const unsigned char *data );

#ifdef __cplusplus
}
#endif

#endif /* SC_SC_MBEDTLS_MD_H */
