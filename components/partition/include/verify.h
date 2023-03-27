/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __VERIFY_H__
#define __VERIFY_H__

#include <stdint.h>
#include <yoc/partition.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DIGEST_HASH_NONE   = 0,
    DIGEST_HASH_SHA1   = 1, // 20bytes
    DIGEST_HASH_MD5    = 2, // 16bytes
    DIGEST_HASH_SHA224 = 3, // 28bytes
    DIGEST_HASH_SHA256 = 4, // 32bytes
    DIGEST_HASH_SHA384 = 5, // 48bytes
    DIGEST_HASH_SHA512 = 6, // 64bytes
    DIGEST_HASH_MAX
} digest_sch_e;

typedef enum {
    SIGNATURE_NONE     = 0,
    SIGNATURE_RSA_1024 = 1,
    SIGNATURE_RSA_2048 = 2,
    SIGNATURE_ECC_256  = 3,
    SIGNATURE_ECC_160  = 4,
    SIGNATURE_MAX
} signature_sch_e;

int get_length_with_digest_type(digest_sch_e type);
int get_length_with_signature_type(signature_sch_e type);

#if defined(CONFIG_COMP_SEC_CRYPTO) && CONFIG_COMP_SEC_CRYPTO
#include <sec_crypto_sha.h>

typedef struct {
    sc_sha_t sc_sha;
    sc_sha_context_t sc_ctx;
    digest_sch_e ds_type;
    void *priv;
} sha_context_t;

sha_context_t* sha_init(digest_sch_e ds, partition_info_t *part_info);
int sha_start(sha_context_t *ctx);
int sha_update(sha_context_t *ctx, const void *input, uint32_t ilen, int from_mem);
int sha_finish(sha_context_t *ctx, void *output, uint32_t *olen);
int sha_deinit(sha_context_t *ctx);
#endif

#ifdef __cplusplus
}
#endif
#endif