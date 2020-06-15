/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef TEE_CRYPTO_API_H
#define TEE_CRYPTO_API_H

#include "tee_debug.h"
#include "tee_common.h"
#include "tee_storage_api.h"

typedef enum {
    TEE_ALG_AES_ECB_NOPAD                = 0x10000010,
    TEE_ALG_AES_CBC_NOPAD                = 0x10000110,
    TEE_ALG_AES_CTR                      = 0x10000210,
    TEE_ALG_AES_CTS                      = 0x10000310,
    TEE_ALG_AES_XTS                      = 0x10000410,
    TEE_ALG_AES_CBC_MAC_NOPAD            = 0x30000110,
    TEE_ALG_AES_CBC_MAC_PKCS5            = 0x30000510,
    TEE_ALG_AES_CMAC                     = 0x30000610,
    TEE_ALG_AES_CCM                      = 0x40000710,
    TEE_ALG_AES_GCM                      = 0x40000810,
    TEE_ALG_DES_ECB_NOPAD                = 0x10000011,
    TEE_ALG_DES_CBC_NOPAD                = 0x10000111,
    TEE_ALG_DES_CBC_MAC_NOPAD            = 0x30000111,
    TEE_ALG_DES_CBC_MAC_PKCS5            = 0x30000511,
    TEE_ALG_DES3_ECB_NOPAD               = 0x10000013,
    TEE_ALG_DES3_CBC_NOPAD               = 0x10000113,
    TEE_ALG_DES3_CBC_MAC_NOPAD           = 0x30000113,
    TEE_ALG_DES3_CBC_MAC_PKCS5           = 0x30000513,
    TEE_ALG_RSASSA_PKCS1_V1_5_MD5        = 0x70001830,
    TEE_ALG_RSASSA_PKCS1_V1_5_SHA1       = 0x70002830,
    TEE_ALG_RSASSA_PKCS1_V1_5_SHA224     = 0x70003830,
    TEE_ALG_RSASSA_PKCS1_V1_5_SHA256     = 0x70004830,
    TEE_ALG_RSASSA_PKCS1_V1_5_SHA384     = 0x70005830,
    TEE_ALG_RSASSA_PKCS1_V1_5_SHA512     = 0x70006830,
    TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA1   = 0x70212930,
    TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA224 = 0x70313930,
    TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256 = 0x70414930,
    TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA384 = 0x70515930,
    TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA512 = 0x70616930,
    TEE_ALG_RSAES_PKCS1_V1_5             = 0x60000130,
    TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1   = 0x60210230,
    TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA224 = 0x60310230,
    TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA256 = 0x60410230,
    TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA384 = 0x60510230,
    TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA512 = 0x60610230,
    TEE_ALG_RSA_NOPAD                    = 0x60000030,
    TEE_ALG_DSA_SHA1                     = 0x70002131,
    TEE_ALG_DH_DERIVE_SHARED_SECRET      = 0x80000032,
    TEE_ALG_MD5                          = 0x50000001,
    TEE_ALG_SHA1                         = 0x50000002,
    TEE_ALG_SHA224                       = 0x50000003,
    TEE_ALG_SHA256                       = 0x50000004,
    TEE_ALG_SHA384                       = 0x50000005,
    TEE_ALG_SHA512                       = 0x50000006,
    TEE_ALG_HMAC_MD5                     = 0x30000001,
    TEE_ALG_HMAC_SHA1                    = 0x30000002,
    TEE_ALG_HMAC_SHA224                  = 0x30000003,
    TEE_ALG_HMAC_SHA256                  = 0x30000004,
    TEE_ALG_HMAC_SHA384                  = 0x30000005,
    TEE_ALG_HMAC_SHA512                  = 0x30000006,
    TEE_ALG_ECDSA_P192                   = 0x70001042,
    TEE_ALG_ECDSA_P224                   = 0x70002042,
    TEE_ALG_ECDSA_P256                   = 0x70003042,
    TEE_ALG_ECDSA_P384                   = 0x70004042,
    TEE_ALG_ECDSA_P521                   = 0x70005042
} algo_ident;

typedef enum {
    TEE_MODE_ENCRYPT = 0,
    TEE_MODE_DECRYPT = 1,
    TEE_MODE_SIGN    = 2,
    TEE_MODE_VERIFY  = 3,
    TEE_MODE_MAC     = 4,
    TEE_MODE_DIGEST  = 5,
    TEE_MODE_DERIVE  = 6
} tee_operationmode;

typedef struct {
    uint32_t algorithm;
    uint32_t operationclass;
    uint32_t mode;
    uint32_t digestlength;
    uint32_t maxKeysize;
    uint32_t keysize;
    uint32_t requiredkeyusage;
    uint32_t handlestate;
} tee_operationinfo;

typedef struct {
    uint32_t keysize;
    uint32_t requiredkeyusage;
} tee_operationinfokey;

struct operation_key_t {
    void    *ctx;
    void    *key;
    uint32_t keylen;
    void    *iv;
    uint32_t ivlen;
    void    *n;
    uint32_t n_len;
    void    *e;
    uint32_t e_len;
    void    *d;
    uint32_t d_len;
};

struct tee_operationhandle_t {
    tee_operationinfo       operation_info;
    struct operation_key_t  key;
    void                   *dig_ctx;
    uint32_t                op_state;
};

typedef struct tee_operationhandle_t *tee_operationhandle;

tee_result tee_allocateoperation(tee_operationhandle *operation, uint32_t algorithm,
                                 uint32_t mode, uint32_t maxkeysize);
void tee_freeoperation(tee_operationhandle operation);
void tee_getoperationinfo(tee_operationhandle operation, tee_operationinfo *operationinfo);
void tee_resetoperation(tee_operationhandle operation);
tee_result tee_setoperationkey(tee_operationhandle operation, tee_objecthandle key);
void tee_copyoperation(tee_operationhandle dstoperation, tee_operationhandle srcoperation);

void tee_digestupdate(tee_operationhandle operation, void *chunk, uint32_t chunksize);
tee_result tee_digestdofinal(tee_operationhandle operation, void *chunk, uint32_t chunklen,
                             void *hash, uint32_t *hashlen);

void tee_cipherinit(tee_operationhandle operation, void *iv, uint32_t ivlen);
tee_result tee_cipherupdate(tee_operationhandle operation, void *srcdata, uint32_t srclen,
                            void *destdata, uint32_t *destlen);
tee_result tee_cipherdofinal(tee_operationhandle operation, void *srcdata, uint32_t srclen,
                             void *destdata, uint32_t *destlen);

void tee_macinit(tee_operationhandle operation, void *iv, uint32_t ivlen);
void tee_macupdate(tee_operationhandle operation, void *chunk, uint32_t chunksize);
tee_result tee_maccomputefinal(tee_operationhandle operation, void *message, uint32_t messagelen,
                               void *mac, uint32_t *maclen);
tee_result tee_maccomparefinal(tee_operationhandle operation, void *message, uint32_t messagelen,
                               void *mac, uint32_t maclen);

tee_result tee_asymmetricencrypt(tee_operationhandle operation, tee_attribute *params,
                                 uint32_t paramcount, void *srcdata, uint32_t srclen,
                                 void *destdata, uint32_t *destlen);
tee_result tee_asymmetricDecrypt(tee_operationhandle operation, tee_attribute *params,
                                 uint32_t paramcount, void *srcdata, uint32_t srclen,
                                 void *destdata, uint32_t *destlen);
tee_result tee_asymmetricSignDigest(tee_operationhandle operation, tee_attribute *params,
                                    uint32_t paramcount, void *digest, uint32_t digestlen,
                                    void *signature, uint32_t *signaturelen);
tee_result tee_asymmetricVerifyDigest(tee_operationhandle operation, tee_attribute *params,
                                      uint32_t paramcount, void *digest, uint32_t digestlen,
                                      void *signature, uint32_t signaturelen);

void tee_generaterandom(void *randombuffer, uint32_t randombufferlen);
#endif

