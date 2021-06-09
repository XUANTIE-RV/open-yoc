/**
 * \file rsa.h
 *
 * \brief The RSA public-key cryptosystem
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
#ifndef SC_MBEDTLS_RSA_H
#define SC_MBEDTLS_RSA_H

#include "crypto_config.h"

#include "crypto_bignum.h"
#include "crypto_md.h"
#include "sec_crypto_errcode.h"

/*
 * RSA Error codes
 */
#define SC_MBEDTLS_ERR_RSA_BAD_INPUT_DATA                    SC_BAD_INPUT_DATA  /**< Bad input parameters to function. */
#define SC_MBEDTLS_ERR_RSA_INVALID_PADDING                   SC_INVALID_PADDING  /**< Input data contains invalid padding and is rejected. */
#define SC_MBEDTLS_ERR_RSA_KEY_GEN_FAILED                    SC_KEY_GEN_FAILED  /**< Something failed during generation of a key. */
#define SC_MBEDTLS_ERR_RSA_KEY_CHECK_FAILED                  SC_KEY_CHECK_FAILED  /**< Key failed to pass the library's validity check. */
#define SC_MBEDTLS_ERR_RSA_PUBLIC_FAILED                     SC_PUBLIC_FAILED  /**< The public key operation failed. */
#define SC_MBEDTLS_ERR_RSA_PRIVATE_FAILED                    SC_PRIVATE_FAILED  /**< The private key operation failed. */
#define SC_MBEDTLS_ERR_RSA_VERIFY_FAILED                     SC_VERIFY_FAILED  /**< The PKCS#1 verification failed. */
#define SC_MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE                  SC_OUTPUT_TOO_LARGE  /**< The output buffer for decryption is not large enough. */
#define SC_MBEDTLS_ERR_RSA_RNG_FAILED                        SC_RNG_FAILED  /**< The random generator failed to generate non-zeros. */

/*
 * RSA constants
 */
#define SC_MBEDTLS_RSA_PUBLIC      0
#define SC_MBEDTLS_RSA_PRIVATE     1

#define SC_MBEDTLS_RSA_PKCS_V15    0
#define SC_MBEDTLS_RSA_PKCS_V21    1

#define SC_MBEDTLS_RSA_SIGN        1
#define SC_MBEDTLS_RSA_CRYPT       2

#define SC_MBEDTLS_RSA_SALT_LEN_ANY    -1

/*
 * The above constants may be used even if the RSA module is compile out,
 * eg for alternative (PKCS#11) RSA implemenations in the PK layers.
 */
#if defined(MBEDTLS_RSA_C)

#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief          RSA context structure
 */
typedef struct
{
    int ver;                    /*!<  always 0          */
    size_t len;                 /*!<  size(N) in chars  */

    sc_mbedtls_mpi N;                      /*!<  public modulus    */
    sc_mbedtls_mpi E;                      /*!<  public exponent   */

    sc_mbedtls_mpi D;                      /*!<  private exponent  */
    sc_mbedtls_mpi P;                      /*!<  1st prime factor  */
    sc_mbedtls_mpi Q;                      /*!<  2nd prime factor  */
    sc_mbedtls_mpi DP;                     /*!<  D % (P - 1)       */
    sc_mbedtls_mpi DQ;                     /*!<  D % (Q - 1)       */
    sc_mbedtls_mpi QP;                     /*!<  1 / (Q % P)       */

    sc_mbedtls_mpi RN;                     /*!<  cached R^2 mod N  */
    sc_mbedtls_mpi RP;                     /*!<  cached R^2 mod P  */
    sc_mbedtls_mpi RQ;                     /*!<  cached R^2 mod Q  */

    sc_mbedtls_mpi Vi;                     /*!<  cached blinding value     */
    sc_mbedtls_mpi Vf;                     /*!<  cached un-blinding value  */

    int padding;                /*!<  SC_MBEDTLS_RSA_PKCS_V15 for 1.5 padding and
                                      MBEDTLS_RSA_PKCS_v21 for OAEP/PSS         */
    int hash_id;                /*!<  Hash identifier of sc_mbedtls_md_type_t as
                                      specified in the sc_mbedtls_md.h header file
                                      for the EME-OAEP and EMSA-PSS
                                      encoding                          */
#if defined(MBEDTLS_THREADING_C)
    mbedtls_threading_mutex_t mutex;    /*!<  Thread-safety mutex       */
#endif
}
sc_mbedtls_rsa_context;

/**
 * \brief          Initialize an RSA context
 *
 *                 Note: Set padding to SC_MBEDTLS_RSA_PKCS_V21 for the RSAES-OAEP
 *                 encryption scheme and the RSASSA-PSS signature scheme.
 *
 * \param ctx      RSA context to be initialized
 * \param padding  SC_MBEDTLS_RSA_PKCS_V15 or SC_MBEDTLS_RSA_PKCS_V21
 * \param hash_id  SC_MBEDTLS_RSA_PKCS_V21 hash identifier
 *
 * \note           The hash_id parameter is actually ignored
 *                 when using SC_MBEDTLS_RSA_PKCS_V15 padding.
 *
 * \note           Choice of padding mode is strictly enforced for private key
 *                 operations, since there might be security concerns in
 *                 mixing padding modes. For public key operations it's merely
 *                 a default value, which can be overriden by calling specific
 *                 rsa_rsaes_xxx or rsa_rsassa_xxx functions.
 *
 * \note           The chosen hash is always used for OEAP encryption.
 *                 For PSS signatures, it's always used for making signatures,
 *                 but can be overriden (and always is, if set to
 *                 SC_MBEDTLS_MD_NONE) for verifying them.
 */
void sc_mbedtls_rsa_init( sc_mbedtls_rsa_context *ctx,
               int padding,
               int hash_id);

/**
 * \brief          Set padding for an already initialized RSA context
 *                 See \c sc_mbedtls_rsa_init() for details.
 *
 * \param ctx      RSA context to be set
 * \param padding  SC_MBEDTLS_RSA_PKCS_V15 or SC_MBEDTLS_RSA_PKCS_V21
 * \param hash_id  SC_MBEDTLS_RSA_PKCS_V21 hash identifier
 */
void sc_mbedtls_rsa_set_padding( sc_mbedtls_rsa_context *ctx, int padding, int hash_id);

/**
 * \brief          Generate an RSA keypair
 *
 * \param ctx      RSA context that will hold the key
 * \param f_rng    RNG function
 * \param p_rng    RNG parameter
 * \param nbits    size of the public key in bits
 * \param exponent public exponent (e.g., 65537)
 *
 * \note           sc_mbedtls_rsa_init() must be called beforehand to setup
 *                 the RSA context.
 *
 * \return         0 if successful, or an SC_MBEDTLS_ERR_RSA_XXX error code
 */
int sc_mbedtls_rsa_gen_key( sc_mbedtls_rsa_context *ctx,
                 int (*f_rng)(void *, unsigned char *, size_t),
                 void *p_rng,
                 unsigned int nbits, int exponent );

/**
 * \brief          Check a public RSA key
 *
 * \param ctx      RSA context to be checked
 *
 * \return         0 if successful, or an SC_MBEDTLS_ERR_RSA_XXX error code
 */
int sc_mbedtls_rsa_check_pubkey( const sc_mbedtls_rsa_context *ctx );

/**
 * \brief          Check a private RSA key
 *
 * \param ctx      RSA context to be checked
 *
 * \return         0 if successful, or an SC_MBEDTLS_ERR_RSA_XXX error code
 */
int sc_mbedtls_rsa_check_privkey( const sc_mbedtls_rsa_context *ctx );

/**
 * \brief          Check a public-private RSA key pair.
 *                 Check each of the contexts, and make sure they match.
 *
 * \param pub      RSA context holding the public key
 * \param prv      RSA context holding the private key
 *
 * \return         0 if successful, or an SC_MBEDTLS_ERR_RSA_XXX error code
 */
int sc_mbedtls_rsa_check_pub_priv( const sc_mbedtls_rsa_context *pub, const sc_mbedtls_rsa_context *prv );

/**
 * \brief          Do an RSA public key operation
 *
 * \param ctx      RSA context
 * \param input    input buffer
 * \param output   output buffer
 *
 * \return         0 if successful, or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           This function does NOT take care of message
 *                 padding. Also, be sure to set input[0] = 0 or ensure that
 *                 input is smaller than N.
 *
 * \note           The input and output buffers must be large
 *                 enough (eg. 128 bytes if RSA-1024 is used).
 */
int sc_mbedtls_rsa_public( sc_mbedtls_rsa_context *ctx,
                const unsigned char *input,
                unsigned char *output );

/**
 * \brief          Do an RSA private key operation
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for blinding)
 * \param p_rng    RNG parameter
 * \param input    input buffer
 * \param output   output buffer
 *
 * \return         0 if successful, or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The input and output buffers must be large
 *                 enough (eg. 128 bytes if RSA-1024 is used).
 */
int sc_mbedtls_rsa_private( sc_mbedtls_rsa_context *ctx,
                 int (*f_rng)(void *, unsigned char *, size_t),
                 void *p_rng,
                 const unsigned char *input,
                 unsigned char *output );

/**
 * \brief          Generic wrapper to perform a PKCS#1 encryption using the
 *                 mode from the context. Add the message padding, then do an
 *                 RSA operation.
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for padding and PKCS#1 v2.1 encoding
 *                               and SC_MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     SC_MBEDTLS_RSA_PUBLIC or SC_MBEDTLS_RSA_PRIVATE
 * \param ilen     contains the plaintext length
 * \param input    buffer holding the data to be encrypted
 * \param output   buffer that will hold the ciphertext
 *
 * \return         0 if successful, or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int sc_mbedtls_rsa_pkcs1_encrypt( sc_mbedtls_rsa_context *ctx,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng,
                       int mode, size_t ilen,
                       const unsigned char *input,
                       unsigned char *output );

/**
 * \brief          Perform a PKCS#1 v1.5 encryption (RSAES-PKCS1-v1_5-ENCRYPT)
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for padding and SC_MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     SC_MBEDTLS_RSA_PUBLIC or SC_MBEDTLS_RSA_PRIVATE
 * \param ilen     contains the plaintext length
 * \param input    buffer holding the data to be encrypted
 * \param output   buffer that will hold the ciphertext
 *
 * \return         0 if successful, or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int sc_mbedtls_rsa_rsaes_pkcs1_v15_encrypt( sc_mbedtls_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng,
                                 int mode, size_t ilen,
                                 const unsigned char *input,
                                 unsigned char *output );

/**
 * \brief          Perform a PKCS#1 v2.1 OAEP encryption (RSAES-OAEP-ENCRYPT)
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for padding and PKCS#1 v2.1 encoding
 *                               and SC_MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     SC_MBEDTLS_RSA_PUBLIC or SC_MBEDTLS_RSA_PRIVATE
 * \param label    buffer holding the custom label to use
 * \param label_len contains the label length
 * \param ilen     contains the plaintext length
 * \param input    buffer holding the data to be encrypted
 * \param output   buffer that will hold the ciphertext
 *
 * \return         0 if successful, or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int sc_mbedtls_rsa_rsaes_oaep_encrypt( sc_mbedtls_rsa_context *ctx,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng,
                            int mode,
                            const unsigned char *label, size_t label_len,
                            size_t ilen,
                            const unsigned char *input,
                            unsigned char *output );

/**
 * \brief          Generic wrapper to perform a PKCS#1 decryption using the
 *                 mode from the context. Do an RSA operation, then remove
 *                 the message padding
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Only needed for SC_MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     SC_MBEDTLS_RSA_PUBLIC or SC_MBEDTLS_RSA_PRIVATE
 * \param olen     will contain the plaintext length
 * \param input    buffer holding the encrypted data
 * \param output   buffer that will hold the plaintext
 * \param output_max_len    maximum length of the output buffer
 *
 * \return         0 if successful, or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used) otherwise
 *                 an error is thrown.
 */
int sc_mbedtls_rsa_pkcs1_decrypt( sc_mbedtls_rsa_context *ctx,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng,
                       int mode, size_t *olen,
                       const unsigned char *input,
                       unsigned char *output,
                       size_t output_max_len );

/**
 * \brief          Perform a PKCS#1 v1.5 decryption (RSAES-PKCS1-v1_5-DECRYPT)
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Only needed for SC_MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     SC_MBEDTLS_RSA_PUBLIC or SC_MBEDTLS_RSA_PRIVATE
 * \param olen     will contain the plaintext length
 * \param input    buffer holding the encrypted data
 * \param output   buffer that will hold the plaintext
 * \param output_max_len    maximum length of the output buffer
 *
 * \return         0 if successful, or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used) otherwise
 *                 an error is thrown.
 */
int sc_mbedtls_rsa_rsaes_pkcs1_v15_decrypt( sc_mbedtls_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng,
                                 int mode, size_t *olen,
                                 const unsigned char *input,
                                 unsigned char *output,
                                 size_t output_max_len );

/**
 * \brief          Perform a PKCS#1 v2.1 OAEP decryption (RSAES-OAEP-DECRYPT)
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Only needed for SC_MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     SC_MBEDTLS_RSA_PUBLIC or SC_MBEDTLS_RSA_PRIVATE
 * \param label    buffer holding the custom label to use
 * \param label_len contains the label length
 * \param olen     will contain the plaintext length
 * \param input    buffer holding the encrypted data
 * \param output   buffer that will hold the plaintext
 * \param output_max_len    maximum length of the output buffer
 *
 * \return         0 if successful, or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The output buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used) otherwise
 *                 an error is thrown.
 */
int sc_mbedtls_rsa_rsaes_oaep_decrypt( sc_mbedtls_rsa_context *ctx,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng,
                            int mode,
                            const unsigned char *label, size_t label_len,
                            size_t *olen,
                            const unsigned char *input,
                            unsigned char *output,
                            size_t output_max_len );

/**
 * \brief          Generic wrapper to perform a PKCS#1 signature using the
 *                 mode from the context. Do a private RSA operation to sign
 *                 a message digest
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for PKCS#1 v2.1 encoding and for
 *                               SC_MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     SC_MBEDTLS_RSA_PUBLIC or SC_MBEDTLS_RSA_PRIVATE
 * \param md_alg   a SC_MBEDTLS_MD_XXX (use SC_MBEDTLS_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for SC_MBEDTLS_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer that will hold the ciphertext
 *
 * \return         0 if the signing operation was successful,
 *                 or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 *
 * \note           In case of PKCS#1 v2.1 encoding, see comments on
 * \note           \c sc_mbedtls_rsa_rsassa_pss_sign() for details on md_alg and hash_id.
 */
int sc_mbedtls_rsa_pkcs1_sign( sc_mbedtls_rsa_context *ctx,
                    int (*f_rng)(void *, unsigned char *, size_t),
                    void *p_rng,
                    int mode,
                    sc_mbedtls_md_type_t md_alg,
                    unsigned int hashlen,
                    const unsigned char *hash,
                    unsigned char *sig );

/**
 * \brief          Perform a PKCS#1 v1.5 signature (RSASSA-PKCS1-v1_5-SIGN)
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Only needed for SC_MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     SC_MBEDTLS_RSA_PUBLIC or SC_MBEDTLS_RSA_PRIVATE
 * \param md_alg   a SC_MBEDTLS_MD_XXX (use SC_MBEDTLS_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for SC_MBEDTLS_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer that will hold the ciphertext
 *
 * \return         0 if the signing operation was successful,
 *                 or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int sc_mbedtls_rsa_rsassa_pkcs1_v15_sign( sc_mbedtls_rsa_context *ctx,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng,
                               int mode,
                               sc_mbedtls_md_type_t md_alg,
                               unsigned int hashlen,
                               const unsigned char *hash,
                               unsigned char *sig );

/**
 * \brief          Perform a PKCS#1 v2.1 PSS signature (RSASSA-PSS-SIGN)
 *
 * \param ctx      RSA context
 * \param f_rng    RNG function (Needed for PKCS#1 v2.1 encoding and for
 *                               SC_MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     SC_MBEDTLS_RSA_PUBLIC or SC_MBEDTLS_RSA_PRIVATE
 * \param md_alg   a SC_MBEDTLS_MD_XXX (use SC_MBEDTLS_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for SC_MBEDTLS_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer that will hold the ciphertext
 *
 * \return         0 if the signing operation was successful,
 *                 or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 *
 * \note           The hash_id in the RSA context is the one used for the
 *                 encoding. md_alg in the function call is the type of hash
 *                 that is encoded. According to RFC 3447 it is advised to
 *                 keep both hashes the same.
 */
int sc_mbedtls_rsa_rsassa_pss_sign( sc_mbedtls_rsa_context *ctx,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng,
                         int mode,
                         sc_mbedtls_md_type_t md_alg,
                         unsigned int hashlen,
                         const unsigned char *hash,
                         unsigned char *sig );

/**
 * \brief          Generic wrapper to perform a PKCS#1 verification using the
 *                 mode from the context. Do a public RSA operation and check
 *                 the message digest
 *
 * \param ctx      points to an RSA public key
 * \param f_rng    RNG function (Only needed for SC_MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     SC_MBEDTLS_RSA_PUBLIC or SC_MBEDTLS_RSA_PRIVATE
 * \param md_alg   a SC_MBEDTLS_MD_XXX (use SC_MBEDTLS_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for SC_MBEDTLS_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer holding the ciphertext
 *
 * \return         0 if the verify operation was successful,
 *                 or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 *
 * \note           In case of PKCS#1 v2.1 encoding, see comments on
 *                 \c sc_mbedtls_rsa_rsassa_pss_verify() about md_alg and hash_id.
 */
int sc_mbedtls_rsa_pkcs1_verify( sc_mbedtls_rsa_context *ctx,
                      int (*f_rng)(void *, unsigned char *, size_t),
                      void *p_rng,
                      int mode,
                      sc_mbedtls_md_type_t md_alg,
                      unsigned int hashlen,
                      const unsigned char *hash,
                      const unsigned char *sig );

/**
 * \brief          Perform a PKCS#1 v1.5 verification (RSASSA-PKCS1-v1_5-VERIFY)
 *
 * \param ctx      points to an RSA public key
 * \param f_rng    RNG function (Only needed for SC_MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     SC_MBEDTLS_RSA_PUBLIC or SC_MBEDTLS_RSA_PRIVATE
 * \param md_alg   a SC_MBEDTLS_MD_XXX (use SC_MBEDTLS_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for SC_MBEDTLS_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer holding the ciphertext
 *
 * \return         0 if the verify operation was successful,
 *                 or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int sc_mbedtls_rsa_rsassa_pkcs1_v15_verify( sc_mbedtls_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng,
                                 int mode,
                                 sc_mbedtls_md_type_t md_alg,
                                 unsigned int hashlen,
                                 const unsigned char *hash,
                                 const unsigned char *sig );

/**
 * \brief          Perform a PKCS#1 v2.1 PSS verification (RSASSA-PSS-VERIFY)
 *                 (This is the "simple" version.)
 *
 * \param ctx      points to an RSA public key
 * \param f_rng    RNG function (Only needed for SC_MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     SC_MBEDTLS_RSA_PUBLIC or SC_MBEDTLS_RSA_PRIVATE
 * \param md_alg   a SC_MBEDTLS_MD_XXX (use SC_MBEDTLS_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for SC_MBEDTLS_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer holding the ciphertext
 *
 * \return         0 if the verify operation was successful,
 *                 or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 *
 * \note           The hash_id in the RSA context is the one used for the
 *                 verification. md_alg in the function call is the type of
 *                 hash that is verified. According to RFC 3447 it is advised to
 *                 keep both hashes the same. If hash_id in the RSA context is
 *                 unset, the md_alg from the function call is used.
 */
int sc_mbedtls_rsa_rsassa_pss_verify( sc_mbedtls_rsa_context *ctx,
                           int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng,
                           int mode,
                           sc_mbedtls_md_type_t md_alg,
                           unsigned int hashlen,
                           const unsigned char *hash,
                           const unsigned char *sig );

/**
 * \brief          Perform a PKCS#1 v2.1 PSS verification (RSASSA-PSS-VERIFY)
 *                 (This is the version with "full" options.)
 *
 * \param ctx      points to an RSA public key
 * \param f_rng    RNG function (Only needed for SC_MBEDTLS_RSA_PRIVATE)
 * \param p_rng    RNG parameter
 * \param mode     SC_MBEDTLS_RSA_PUBLIC or SC_MBEDTLS_RSA_PRIVATE
 * \param md_alg   a SC_MBEDTLS_MD_XXX (use SC_MBEDTLS_MD_NONE for signing raw data)
 * \param hashlen  message digest length (for SC_MBEDTLS_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param mgf1_hash_id message digest used for mask generation
 * \param expected_salt_len Length of the salt used in padding, use
 *                 SC_MBEDTLS_RSA_SALT_LEN_ANY to accept any salt length
 * \param sig      buffer holding the ciphertext
 *
 * \return         0 if the verify operation was successful,
 *                 or an SC_MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 *
 * \note           The hash_id in the RSA context is ignored.
 */
int sc_mbedtls_rsa_rsassa_pss_verify_ext( sc_mbedtls_rsa_context *ctx,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng,
                               int mode,
                               sc_mbedtls_md_type_t md_alg,
                               unsigned int hashlen,
                               const unsigned char *hash,
                               sc_mbedtls_md_type_t mgf1_hash_id,
                               int expected_salt_len,
                               const unsigned char *sig );

/**
 * \brief          Copy the components of an RSA context
 *
 * \param dst      Destination context
 * \param src      Source context
 *
 * \return         0 on success,
 *                 SC_MBEDTLS_ERR_MPI_ALLOC_FAILED on memory allocation failure
 */
int sc_mbedtls_rsa_copy( sc_mbedtls_rsa_context *dst, const sc_mbedtls_rsa_context *src );

/**
 * \brief          Free the components of an RSA key
 *
 * \param ctx      RSA Context to free
 */
void sc_mbedtls_rsa_free( sc_mbedtls_rsa_context *ctx );
/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int sc_mbedtls_rsa_self_test( int verbose );

int rsa_rand(void *rng_state, unsigned char *output, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_RSA_C */

#endif /* rsa.h */
