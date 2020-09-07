/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "tee_client_api.h"
#include "tee_entry.h"
#include "csi_tee_common.h"

#if (CONFIG_ALGO_HMAC > 0)
int32_t csi_tee_hmac(const uint8_t *in, uint32_t  in_len,
                     const uint8_t *key, uint32_t key_len,
                     uint8_t *out,
                     tee_hmac_type_e type,
                     tee_hash_op_e hash_op,
                     uint32_t *ctx)
{
    teec_parameter p[4];
    uint32_t t;

    PARAM_CHECK(in);
    PARAM_CHECK(in_len);
    PARAM_CHECK(key);
    PARAM_CHECK(key_len);
    PARAM_CHECK(out);

    if (type != TEE_HMAC_SHA1) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    if (hash_op >= TEE_HASH_OP_MAX) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    if (hash_op != TEE_HASH_OP_NONE && ctx == NULL)  {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    TEEC_4PARAMS(p, in, in_len, key, key_len, out, type, ctx, hash_op);

    t = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, \
                         TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_INOUT);

    return tee_send(TEE_CMD_HMAC, p, t);
}
#endif

