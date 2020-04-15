/*
 *  The RSA public-key cryptosystem
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
/*
 *  The following sources were referenced in the design of this implementation
 *  of the RSA algorithm:
 *
 *  [1] A method for obtaining digital signatures and public-key cryptosystems
 *      R Rivest, A Shamir, and L Adleman
 *      http://people.csail.mit.edu/rivest/pubs.html#RSA78
 *
 *  [2] Handbook of Applied Cryptography - 1997, Chapter 8
 *      Menezes, van Oorschot and Vanstone
 *
 *  [3] Malware Guard Extension: Using SGX to Conceal Cache Attacks
 *      Michael Schwarz, Samuel Weiser, Daniel Gruss, Clémentine Maurice and
 *      Stefan Mangard
 *      https://arxiv.org/abs/1702.08719v2
 *
 */

#include "mbedtls/rsa.h"

#include <string.h>

#if defined(MBEDTLS_PKCS1_V21)
#include "mbedtls/md.h"
#endif

#if defined(MBEDTLS_PKCS1_V15) && !defined(__OpenBSD__)
#include <stdlib.h>
#endif

#ifdef MBEDTLS_RSA_ALT

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#define mbedtls_calloc calloc
#define mbedtls_free   free
#endif
#include "drv/tee.h"

/*
 * Initialize an RSA context
 */
void mbedtls_rsa_init(mbedtls_rsa_context *ctx,
                      int padding,
                      int hash_id)
{
    memset(ctx, 0, sizeof(mbedtls_rsa_context));
    ctx->padding = padding;
    ctx->hash_id = hash_id;
}

/*
 * Check a public RSA key
 */
int mbedtls_rsa_check_pubkey(const mbedtls_rsa_context *ctx)
{
    if (!ctx->N.p || !ctx->E.p) {
        return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);
    }

    if ((ctx->N.p[0] & 1) == 0 ||
        (ctx->E.p[0] & 1) == 0) {
        return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);
    }

    if (mbedtls_mpi_bitlen(&ctx->N) < 128 ||
        mbedtls_mpi_bitlen(&ctx->N) > MBEDTLS_MPI_MAX_BITS) {
        return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);
    }

    if (mbedtls_mpi_bitlen(&ctx->E) < 2 ||
        mbedtls_mpi_cmp_mpi(&ctx->E, &ctx->N) >= 0) {
        return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);
    }

    return (0);
}

/*
 * Check a private RSA key
 */
int mbedtls_rsa_check_privkey(const mbedtls_rsa_context *ctx)
{
#ifndef CK_SAVE_MEM_L3
    int ret;
    mbedtls_mpi PQ, DE, P1, Q1, H, I, G, G2, L1, L2, DP, DQ, QP;

    if ((ret = mbedtls_rsa_check_pubkey(ctx)) != 0) {
        return (ret);
    }

    if (!ctx->P.p || !ctx->Q.p || !ctx->D.p) {
        return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);
    }

    mbedtls_mpi_init(&PQ);
    mbedtls_mpi_init(&DE);
    mbedtls_mpi_init(&P1);
    mbedtls_mpi_init(&Q1);
    mbedtls_mpi_init(&H);
    mbedtls_mpi_init(&I);
    mbedtls_mpi_init(&G);
    mbedtls_mpi_init(&G2);
    mbedtls_mpi_init(&L1);
    mbedtls_mpi_init(&L2);
    mbedtls_mpi_init(&DP);
    mbedtls_mpi_init(&DQ);
    mbedtls_mpi_init(&QP);

    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&PQ, &ctx->P, &ctx->Q));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&DE, &ctx->D, &ctx->E));
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&P1, &ctx->P, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&Q1, &ctx->Q, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&H, &P1, &Q1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_gcd(&G, &ctx->E, &H));

    MBEDTLS_MPI_CHK(mbedtls_mpi_gcd(&G2, &P1, &Q1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_div_mpi(&L1, &L2, &H, &G2));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&I, &DE, &L1));

    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&DP, &ctx->D, &P1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&DQ, &ctx->D, &Q1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_inv_mod(&QP, &ctx->Q, &ctx->P));

    /*
     * Check for a valid PKCS1v2 private key
     */
    if (mbedtls_mpi_cmp_mpi(&PQ, &ctx->N) != 0 ||
        mbedtls_mpi_cmp_mpi(&DP, &ctx->DP) != 0 ||
        mbedtls_mpi_cmp_mpi(&DQ, &ctx->DQ) != 0 ||
        mbedtls_mpi_cmp_mpi(&QP, &ctx->QP) != 0 ||
        mbedtls_mpi_cmp_int(&L2, 0) != 0 ||
        mbedtls_mpi_cmp_int(&I, 1) != 0 ||
        mbedtls_mpi_cmp_int(&G, 1) != 0) {
        ret = MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
    }

cleanup:
    mbedtls_mpi_free(&PQ);
    mbedtls_mpi_free(&DE);
    mbedtls_mpi_free(&P1);
    mbedtls_mpi_free(&Q1);
    mbedtls_mpi_free(&H);
    mbedtls_mpi_free(&I);
    mbedtls_mpi_free(&G);
    mbedtls_mpi_free(&G2);
    mbedtls_mpi_free(&L1);
    mbedtls_mpi_free(&L2);
    mbedtls_mpi_free(&DP);
    mbedtls_mpi_free(&DQ);
    mbedtls_mpi_free(&QP);

    if (ret == MBEDTLS_ERR_RSA_KEY_CHECK_FAILED) {
        return (ret);
    }

    if (ret != 0) {
        return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED + ret);
    }

#endif
    return (0);
}

/*
 * Check if contexts holding a public and private key match
 */
int mbedtls_rsa_check_pub_priv(const mbedtls_rsa_context *pub, const mbedtls_rsa_context *prv)
{
    if (mbedtls_rsa_check_pubkey(pub) != 0 ||
        mbedtls_rsa_check_privkey(prv) != 0) {
        return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);
    }

    if (mbedtls_mpi_cmp_mpi(&pub->N, &prv->N) != 0 ||
        mbedtls_mpi_cmp_mpi(&pub->E, &prv->E) != 0) {
        return (MBEDTLS_ERR_RSA_KEY_CHECK_FAILED);
    }

    return (0);
}

/*
 * Add the message padding, then do an RSA operation
 */
int mbedtls_rsa_pkcs1_encrypt(mbedtls_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng,
                              int mode, size_t ilen,
                              const unsigned char *input,
                              unsigned char *output)
{
    int ret;
    unsigned int output_len = 128;
    unsigned char *key;
    unsigned int key_size = 0;
    (void)f_rng;
    (void)p_rng;
    (void)mode;

    /*for now Only support RSA1024 and RSA2048*/
    if (ctx->len != 128 && ctx->len != 256) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    switch (ctx->padding) {
#if defined(MBEDTLS_PKCS1_V15)

        case MBEDTLS_RSA_PKCS_V15:
            key_size = mbedtls_mpi_size(&ctx->N);
            key = (unsigned char *)mbedtls_calloc(1, key_size * 2);

            if (!key) {
                mbedtls_printf("alloc key fail\n");
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }

            mbedtls_mpi_write_binary(&ctx->N, key, key_size);
            mbedtls_mpi_write_binary(&ctx->E, key + key_size, key_size);

            unsigned char *new_output = mbedtls_calloc(1, 256);
            mbedtls_printf("input %p output %p\n", input, output);
            ret = csi_tee_rsa_encrypt(input, ilen,
                                      key, key_size * 2,
                                      new_output, &output_len,
                                      TEE_RSA_PKCS1_PADDING);
            memcpy(output, new_output, 256);
            mbedtls_free(key);
            mbedtls_free(new_output);

            if (ret) {
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }

            return 0;
#endif

        default:
            return (MBEDTLS_ERR_RSA_INVALID_PADDING);
    }
}

/*
 * Do an RSA operation, then remove the message padding
 */
int mbedtls_rsa_pkcs1_decrypt(mbedtls_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng,
                              int mode, size_t *olen,
                              const unsigned char *input,
                              unsigned char *output,
                              size_t output_max_len)
{
    int ret;
    unsigned char *key;
    unsigned int key_size = 0;

    (void)f_rng;
    (void)p_rng;
    (void)mode;
    (void)output_max_len;

    /*for now Only support RSA1024 and RSA2048*/
    if (ctx->len != 128 && ctx->len != 256) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    switch (ctx->padding) {
#if defined(MBEDTLS_PKCS1_V15)

        case MBEDTLS_RSA_PKCS_V15:
            key_size = mbedtls_mpi_size(&ctx->N);
            key = (unsigned char *)mbedtls_calloc(1, key_size * 3);

            if (!key) {
                mbedtls_printf("alloc key fail\n");
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }

            mbedtls_mpi_write_binary(&ctx->N, key, key_size);
            mbedtls_mpi_write_binary(&ctx->E, key + key_size, key_size);
            mbedtls_mpi_write_binary(&ctx->D, key + key_size + key_size, key_size);
            ret = csi_tee_rsa_decrypt(input, ctx->len,
                                      key, key_size * 3,
                                      output, olen,
                                      TEE_RSA_PKCS1_PADDING);
            mbedtls_free(key);

            if (ret) {
                return MBEDTLS_ERR_RSA_KEY_GEN_FAILED;
            }

            break;
#endif

        default:
            return (MBEDTLS_ERR_RSA_INVALID_PADDING);
    }

    return 0;
}

/*
 * Do an RSA operation to sign the message digest
 */
int mbedtls_rsa_pkcs1_sign(mbedtls_rsa_context *ctx,
                           int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng,
                           int mode,
                           mbedtls_md_type_t md_alg,
                           unsigned int hashlen,
                           const unsigned char *hash,
                           unsigned char *sig)
{
    unsigned char type;
    int ret;
    unsigned int sig_len = 0;
    unsigned char *key;
    unsigned int key_size = 0;

    (void)f_rng;
    (void)p_rng;
    (void)mode;

    /*for now Only support RSA1024 and RSA2048*/
    if (ctx->len != 128 && ctx->len != 256) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    if (md_alg != MBEDTLS_MD_NONE) {
        const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(md_alg);
        hashlen = mbedtls_md_get_size(md_info);
    }

    if (md_alg == MBEDTLS_MD_MD5) {
        type = TEE_RSA_MD5;
    } else if (md_alg == MBEDTLS_MD_SHA1) {
        type = TEE_RSA_SHA1;
    } else if (md_alg == MBEDTLS_MD_SHA256) {
        type = TEE_RSA_SHA256;
    } else {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    switch (ctx->padding) {
#if defined(MBEDTLS_PKCS1_V15)

        case MBEDTLS_RSA_PKCS_V15:
            key_size = mbedtls_mpi_size(&ctx->N);
            key = (unsigned char *)mbedtls_calloc(1, key_size * 3);

            if (!key) {
                mbedtls_printf("alloc key fail\n");
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }

            mbedtls_mpi_write_binary(&ctx->N, key, key_size);
            mbedtls_mpi_write_binary(&ctx->E, key + key_size, key_size);
            mbedtls_mpi_write_binary(&ctx->D, key + key_size + key_size, key_size);
            ret = csi_tee_rsa_sign(hash, hashlen, key, key_size * 3, sig, &sig_len, type);
            mbedtls_free(key);

            if (ret) {
                return MBEDTLS_ERR_RSA_PRIVATE_FAILED;
            }

            break;
#endif

        default:
            return (MBEDTLS_ERR_RSA_INVALID_PADDING);
    }

    return 0;
}

/*
 * Do an RSA operation and check the message digest
 */
int mbedtls_rsa_pkcs1_verify(mbedtls_rsa_context *ctx,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng,
                             int mode,
                             mbedtls_md_type_t md_alg,
                             unsigned int hashlen,
                             const unsigned char *hash,
                             const unsigned char *sig)
{
    unsigned char type;
    int ret;
    unsigned char *key;
    unsigned int key_size = 0;

    (void)f_rng;
    (void)p_rng;
    (void)mode;

    /*for now Only support RSA1024 and RSA2048*/
    if (ctx->len != 128 && ctx->len != 256) {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    if (md_alg != MBEDTLS_MD_NONE) {
        const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(md_alg);
        hashlen = mbedtls_md_get_size(md_info);
    }

    if (md_alg == MBEDTLS_MD_MD5) {
        type = TEE_RSA_MD5;
    } else if (md_alg == MBEDTLS_MD_SHA1) {
        type = TEE_RSA_SHA1;
    } else if (md_alg == MBEDTLS_MD_SHA256) {
        type = TEE_RSA_SHA256;
    } else {
        return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    switch (ctx->padding) {
#if defined(MBEDTLS_PKCS1_V15)

        case MBEDTLS_RSA_PKCS_V15:
            key_size = mbedtls_mpi_size(&ctx->N);
            key = (unsigned char *)mbedtls_calloc(1, key_size * 2);

            if (!key) {
                mbedtls_printf("alloc key fail\n");
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }

            mbedtls_mpi_write_binary(&ctx->N, key, key_size);
            mbedtls_mpi_write_binary(&ctx->E, key + key_size, key_size);
            unsigned char *new_sig = mbedtls_calloc(1, 256);
            memcpy(new_sig, sig, 256);

            ret = csi_tee_rsa_verify((uint8_t *)hash, hashlen, key, key_size * 2, (uint8_t *)new_sig, ctx->len, type);
            mbedtls_free(new_sig);

            mbedtls_free(key);

            if (ret) {
                return MBEDTLS_ERR_RSA_PUBLIC_FAILED;
            }

            break;
#endif

        default:
            return (MBEDTLS_ERR_RSA_INVALID_PADDING);
    }

    return 0;
}

/*
 * Free the components of an RSA key
 */
void mbedtls_rsa_free(mbedtls_rsa_context *ctx)
{

    mbedtls_mpi_free(&ctx->Q);
    mbedtls_mpi_free(&ctx->P);
    mbedtls_mpi_free(&ctx->D);
    mbedtls_mpi_free(&ctx->E);
    mbedtls_mpi_free(&ctx->N);
    mbedtls_mpi_free(&ctx->QP);
    mbedtls_mpi_free(&ctx->DQ);
    mbedtls_mpi_free(&ctx->DP);

#if defined(MBEDTLS_THREADING_C)
    mbedtls_mutex_free(&ctx->mutex);
#endif
}
#endif
