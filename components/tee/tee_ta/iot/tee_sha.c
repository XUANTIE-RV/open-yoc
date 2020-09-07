/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tee_internel_api.h>
#include <tee_crypto_api.h>
#include <tee_storage_api.h>
#include <tee_msg_cmd.h>
#include <crypto.h>

#if (CONFIG_ALGO_SHA > 0)

const static uint32_t sha_param[5][3] = {
    {SHA1,   TEE_ALG_SHA1,   SHA1_HASH_SIZE  },
    {SHA256, TEE_ALG_SHA256, SHA256_HASH_SIZE},
    {SHA224, TEE_ALG_SHA224, SHA224_HASH_SIZE},
    {SHA384, TEE_ALG_SHA384, SHA384_HASH_SIZE},
    {SHA512, TEE_ALG_SHA512, SHA512_HASH_SIZE}
};

int tee_core_sha(tee_param params[4])
{
    int ret;
    tee_operationhandle digest_handler;
    hash_context        *hash_ctx;

    uint8_t *msg     = params[0].memref.buffer;
    uint32_t msglen  = params[0].memref.size;
    uint8_t *hash    = params[1].memref.buffer;
    uint32_t hashlen = params[1].memref.size;
    uint32_t type = params[2].value.a;
    uint32_t mode = params[2].value.b;
    uint32_t ctx = params[3].value.a;

    TEE_LOGD("msg %x, msglen %d, hash %x, hashlen %d, type %d, mode %d, ctx %x\n",
             msg, msglen, hash, hashlen, type, mode, ctx);

    if (mode >= 4) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    if ((mode == 0 || mode == 1) && type > 4) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    hash_ctx = (hash_context *)ctx;

    if (mode == 2 || mode == 3) {
        switch (hash_ctx->hashtype) {
            case SHA1:
                type = 0;
                break;

            case SHA224:
                type = 2;
                break;

            case SHA256:
                type = 1;
                break;

            case SHA384:
                type = 3;
                break;

            case SHA512:
                type = 4;
                break;

            default:
                return TEE_ERROR_BAD_PARAMETERS;
        }
    }

    ret = tee_allocateoperation(&digest_handler, sha_param[type][1], TEE_MODE_DIGEST, 0);

    if (ret) {
        goto fail;
    }

    digest_handler->operation_info.digestlength = sha_param[type][2];


    hash_ctx ->hashctx = (uint8_t *)ctx + sizeof(hash_context);
    digest_handler->dig_ctx = (void *)hash_ctx;

    switch (mode) {
        /*digest*/
        case 0:
            ret = tee_hash_digest(sha_param[type][0], msg, msglen, hash);
            if (ret) {
                goto fail;
            }

            break;

        /*sha start*/
        case 1:
            ret = tee_hash_init(sha_param[type][0], digest_handler->dig_ctx);

            if (ret) {
                goto fail;
            }

            break;

        /*sha update*/
        case 2:
            tee_digestupdate(digest_handler, msg, msglen);
            break;

        /*sha finish*/
        case 3:
            ret = tee_digestdofinal(digest_handler, NULL, 0, hash, &hashlen);

            if (ret) {
                goto fail;
            }

            ret = tee_hash_reset(digest_handler->dig_ctx);

            if (ret) {
                goto fail;
            }

            break;

        default:
            break;
    }

fail:
    return ret;
}
#endif
