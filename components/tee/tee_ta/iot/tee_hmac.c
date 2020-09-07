/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <tee_common.h>
#include <crypto.h>
#include <hmac.h>
#include <tee_debug.h>

#if (CONFIG_ALGO_HMAC > 0)
int tee_core_hmac(tee_param params[4])
{
    uint8_t *input = params[0].memref.buffer;
    uint32_t in_len = params[0].memref.size;
    uint8_t *key = params[1].memref.buffer;
    uint32_t key_len = params[1].memref.size;
    uint8_t *output = params[2].memref.buffer;
    uint8_t type = params[2].memref.size;
    void *ctx = (void *)(uint32_t *)params[3].value.a;
    uint8_t hash_op = params[3].value.b;
    hmac_context_t ctx_tmp;

    TEE_LOGD("TEE input 0x%x, in_len %d, key 0x%x, key_len %d, output 0x%x, type %d, ctx 0x%x, hash_op %d\n",
             input, in_len, key, key_len, output, type, ctx, hash_op);
    TEE_HEX_DUMP("input:", input, in_len);
    TEE_HEX_DUMP("key:", key, key_len);

    switch (hash_op) {
        /*HMAC once*/
        case 0:
            hmac_init(&ctx_tmp, type);
            hmac_start(&ctx_tmp, key, key_len);
            hmac_update(&ctx_tmp, input, in_len);
            hmac_finish(&ctx_tmp, output);
            break;

        /*HMAC start*/
        case 1:
            hmac_init((hmac_context_t *)ctx, type);
            hmac_start((hmac_context_t *)ctx, key, key_len);
            break;

        /*HMAC update*/
        case 2:
            hmac_update((hmac_context_t *)ctx, input, in_len);
            break;

        /*HMAC finish*/
        case 3:
            hmac_finish((hmac_context_t *)ctx, output);
            break;

        default:
            break;
    }

    return 0;
}

#endif
