/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tee_internel_api.h>
#include <tee_crypto_api.h>
#include <tee_storage_api.h>
#include <tee_msg_cmd.h>
#include <string.h>

#if (CONFIG_ALGO_AES > 0)
int tee_core_aes(tee_param params[4], uint32_t algo, uint32_t mode)
{
    tee_result  ret;
    tee_objecthandle keyobj;
    tee_operationhandle handler;
    tee_objectinfo info;
    uint32_t writebytes;
    uint8_t *msg     = params[0].memref.buffer;
    uint32_t msglen  = params[0].memref.size;
    uint8_t *key     = params[1].memref.buffer;
    uint32_t keylen  = params[1].memref.size;
    uint8_t *iv      = params[2].memref.buffer;
    uint32_t ivlen   = params[2].memref.size;
    uint8_t *out     = params[3].memref.buffer;
    uint32_t outlen  = params[3].memref.size;

    TEE_LOGD("msglen:%d, outlen:%d, keylen: %d, alg:%x, mode: %d\n", msglen, *(uint32_t *)outlen, keylen, algo, mode);

    tee_allocatetransientobject(TEE_TYPE_AES, keylen, &keyobj);
    tee_setkey(keyobj, key, keylen);
    tee_getobjectinfo(keyobj, &info);
    tee_allocateoperation(&handler, algo, mode, info.maxobjectsize);
    tee_setoperationkey(handler, keyobj);

    switch (handler->operation_info.algorithm) {
        case TEE_ALG_AES_CBC_NOPAD:
        case TEE_ALG_AES_ECB_NOPAD:
            tee_cipherinit(handler, iv, ivlen);
            break;

        default:
            return TEE_ERROR_BAD_PARAMETERS;
    }

    ret = tee_cipherupdate(handler, msg, msglen, out, &writebytes);

    if (ret != TEE_SUCCESS) {
        TEE_LOGE("tee_cipherupdate %x\n", ret);
        return ret;
    }

    ret = tee_cipherdofinal(handler, msg, msglen, out, &writebytes);

    if (ret != TEE_SUCCESS) {
        TEE_LOGE("tee_cipherdofinal %x\n", ret);
        return ret;
    }

    *(uint32_t *)outlen = writebytes;

    tee_freeoperation(handler);
    tee_freetransientobject(&keyobj);
    return TEE_SUCCESS;
}

#endif


