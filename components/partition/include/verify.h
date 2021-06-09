/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __VERIFY_H__
#define __VERIFY_H__

#include <stdint.h>

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

#ifdef __cplusplus
}
#endif
#endif