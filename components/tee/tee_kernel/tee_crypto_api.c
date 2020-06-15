/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tee_crypto_api.h>
#include <tee_internel_api.h>
#include <string.h>
#include <crypto.h>

#if (CONFIG_ALGO_DES3 > 0)
static des_context des3_ctx;
#endif

#if (CONFIG_ALGO_AES > 0)
static aes_context aes_ctx;
#endif

static struct tee_operationhandle_t ta_oprhandle_s;
static tee_operationhandle ta_oprhandle;

tee_result tee_allocateoperation(tee_operationhandle *operation, uint32_t algorithm,
                                 uint32_t mode, uint32_t maxkeysize)
{
    if (operation == NULL) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    memset(&ta_oprhandle_s, 0, sizeof(struct tee_operationhandle_t));

    ta_oprhandle = &ta_oprhandle_s;

    ta_oprhandle->operation_info.operationclass = TEE_OPERATION_CIPHER;
    ta_oprhandle->operation_info.mode           = mode;
    ta_oprhandle->operation_info.algorithm      = algorithm;
    ta_oprhandle->operation_info.maxKeysize     = maxkeysize;
    ta_oprhandle->operation_info.keysize        = 0;

    ta_oprhandle->op_state                      = TEE_OPERATION_STATE_INITIAL;

    *operation                                  = ta_oprhandle;

    return TEE_SUCCESS;
}

void tee_freeoperation(tee_operationhandle operation)
{
    ta_oprhandle                                = NULL;
}

void tee_resetoperation(tee_operationhandle operation)
{
    ta_oprhandle->op_state                      = TEE_OPERATION_STATE_INITIAL;

    if (operation->operation_info.algorithm == TEE_ALG_DES3_ECB_NOPAD) {
#if (CONFIG_ALGO_DES3 > 0)
        tee_des_reset(operation->key.ctx);
#endif
    }
}

tee_result tee_setoperationkey(tee_operationhandle operation, tee_objecthandle key)
{
    if ((operation == NULL) || (operation->op_state == TEE_OPERATION_STATE_ACTIVE)) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    if (key == NULL) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    switch (operation->operation_info.algorithm) {
        case TEE_ALG_DES3_ECB_NOPAD:
        case TEE_ALG_DES_ECB_NOPAD:
        case TEE_ALG_AES_CBC_NOPAD:
        case TEE_ALG_AES_ECB_NOPAD:
            operation->key.keylen = key->attr[0].content.ref.length;
            operation->key.key = key->attr[0].content.ref.buffer;
            break;

        case TEE_ALG_RSASSA_PKCS1_V1_5_SHA1:
            operation->key.n_len = key->attr[0].content.ref.length;
            operation->key.n = key->attr[0].content.ref.buffer;
            operation->key.e_len = key->attr[1].content.ref.length;
            operation->key.e = key->attr[1].content.ref.buffer;
            operation->key.d_len = key->attr[2].content.ref.length;
            operation->key.d = key->attr[2].content.ref.buffer;
            break;

        default:
            break;
    }

    operation->operation_info.handlestate |= TEE_HANDLE_FLAG_KEY_SET;
    operation->operation_info.keysize = operation->key.keylen;

    return TEE_SUCCESS;
}

void tee_generaterandom(void *randombuffer, uint32_t randombufferlen)
{
    if ((randombuffer == NULL) || (randombufferlen == 0)) {
        return;
    }

    tee_rand_gen(randombuffer, randombufferlen);
}

void tee_digestupdate(tee_operationhandle operation, void *chunk, uint32_t chunksize)
{
    if ((operation == NULL) || (chunk == NULL)) {
        return;
    }

    switch (operation->operation_info.algorithm) {
#if (CONFIG_ALGO_MD5 > 0)

        case TEE_ALG_MD5:
#endif
#if (CONFIG_ALGO_SHA > 0)
        case TEE_ALG_SHA1:
        case TEE_ALG_SHA256:
        case TEE_ALG_SHA224:
        case TEE_ALG_SHA384:
        case TEE_ALG_SHA512:
#endif
            tee_hash_update(chunk, chunksize, operation->dig_ctx);
            break;

        default:
            break;
    }
}

tee_result tee_digestdofinal(tee_operationhandle operation, void *chunk, uint32_t chunklen,
                             void *hash, uint32_t *hashlen)
{
    if ((operation == NULL) || (hash == NULL) || (hashlen == NULL)) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    if ((chunklen > 0) && (chunk == NULL)) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    switch (operation->operation_info.algorithm) {
#if (CONFIG_ALGO_MD5 > 0)

        case TEE_ALG_MD5:
#endif
#if (CONFIG_ALGO_SHA > 0)
        case TEE_ALG_SHA1:
        case TEE_ALG_SHA256:
        case TEE_ALG_SHA224:
        case TEE_ALG_SHA384:
        case TEE_ALG_SHA512:
#endif
            if (chunklen > 0) {
                tee_hash_update(chunk, chunklen, operation->dig_ctx);
            }

            tee_hash_final(hash, operation->dig_ctx);
            break;

        default:
            break;
    }

    *hashlen = operation->operation_info.digestlength;

    return TEE_SUCCESS;
}

void tee_cipherinit(tee_operationhandle operation, void *iv, uint32_t ivlen)
{
    if (operation == NULL) {
        return;
    }

    switch (operation->operation_info.algorithm) {
#if (CONFIG_ALGO_DES3 > 0)

        case TEE_ALG_DES3_ECB_NOPAD:
            operation->key.ctx = (void *)&des3_ctx;
            tee_des_init(DES3_ECB,
                         (operation->operation_info.mode == TEE_MODE_ENCRYPT) ? true : false,
                         operation->key.key,
                         operation->key.keylen ,
                         NULL,
                         operation->key.ctx);
            break;

        case TEE_ALG_DES_ECB_NOPAD:
            operation->key.ctx = (void *)&des3_ctx;
            tee_des_init(DES_ECB,
                         (operation->operation_info.mode == TEE_MODE_ENCRYPT) ? true : false,
                         operation->key.key,
                         operation->key.keylen ,
                         NULL,
                         operation->key.ctx);
            break;
#endif
#if (CONFIG_ALGO_AES > 0)

        case TEE_ALG_AES_CBC_NOPAD:
            operation->key.iv    = iv;
            operation->key.ivlen = ivlen;
            operation->key.ctx   = (void *)&aes_ctx;
            tee_aes_init(AES_CBC,
                         (operation->operation_info.mode == TEE_MODE_ENCRYPT) ? true : false,
                         operation->key.key,
                         NULL,
                         operation->key.keylen,
                         operation->key.iv ,
                         operation->key.ctx);
            break;

        case TEE_ALG_AES_ECB_NOPAD:
            operation->key.ctx   = (void *)&aes_ctx;
            tee_aes_init(AES_ECB,
                         (operation->operation_info.mode == TEE_MODE_ENCRYPT) ? true : false,
                         operation->key.key,
                         NULL,
                         operation->key.keylen,
                         NULL,
                         operation->key.ctx);
            break;
#endif

        default:
            break;
    }
}

tee_result tee_cipherupdate(tee_operationhandle operation, void *srcdata, uint32_t srclen,
                            void *destdata, uint32_t *destlen)
{
    if ((operation == NULL) || (srcdata == NULL) || (destdata == NULL) || (destlen == NULL)) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    switch (operation->operation_info.algorithm) {
#if (CONFIG_ALGO_DES3 > 0)

        case TEE_ALG_DES3_ECB_NOPAD:
        case TEE_ALG_DES_ECB_NOPAD:
            tee_des_process(srcdata, destdata, srclen, operation->key.ctx);
            *destlen = srclen;
            break;
#endif
#if (CONFIG_ALGO_AES > 0)

        case TEE_ALG_AES_CBC_NOPAD:
        case TEE_ALG_AES_ECB_NOPAD:
            tee_aes_process(srcdata, destdata, srclen, operation->key.ctx);
            *destlen = srclen;
            break;
#endif

        default:
            break;
    }

    return TEE_SUCCESS;
}

tee_result tee_cipherdofinal(tee_operationhandle operation, void *srcdata, uint32_t srclen,
                             void *destdata, uint32_t *destlen)
{
    if ((operation == NULL) || (srcdata == NULL) || (destdata == NULL) || (destlen == NULL)) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    switch (operation->operation_info.algorithm) {
#if (CONFIG_ALGO_DES3 > 0)

        case TEE_ALG_DES3_ECB_NOPAD:
        case TEE_ALG_DES_ECB_NOPAD:
            tee_des_finish(srcdata, srclen, destdata, destlen, SYM_NOPAD, operation->key.ctx);
            break;
#endif
#if (CONFIG_ALGO_AES > 0)

        case TEE_ALG_AES_CBC_NOPAD:
        case TEE_ALG_AES_ECB_NOPAD:
            tee_aes_finish(srcdata, srclen, destdata, destlen, SYM_NOPAD, operation->key.ctx);
            break;
#endif

        default:
            break;
    }

    tee_resetoperation(operation);
    return TEE_SUCCESS;
}


