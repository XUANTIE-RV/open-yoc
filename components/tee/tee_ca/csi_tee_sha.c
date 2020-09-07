/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "tee_client_api.h"
#include "tee_entry.h"
#include "csi_tee_common.h"

#if (CONFIG_ALGO_SHA > 0)

int32_t csi_tee_sha(const uint8_t *in, uint32_t in_len,
                    uint8_t *out,
                    tee_sha_type_t type,
                    tee_hash_op_e hash_op,
                    void *ctx)
{
    teec_parameter p[4];
    uint32_t t;
    static uint8_t ctx_tmp[224 + 8];

    if (type >= TEE_SHA_MAX) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    if (hash_op == TEE_HASH_OP_NONE) {
        ctx = ctx == NULL ? ctx_tmp : ctx;
        PARAM_CHECK(in);
        PARAM_CHECK(in_len);
        PARAM_CHECK(out);
    } else if (hash_op == TEE_HASH_OP_START) {
        PARAM_CHECK(ctx);
    } else if (hash_op == TEE_HASH_OP_UPDATA) {
        PARAM_CHECK(in);
        PARAM_CHECK(in_len);
        PARAM_CHECK(ctx);
    } else if (hash_op == TEE_HASH_OP_FINISH) {
        PARAM_CHECK(out);
        PARAM_CHECK(ctx);
    } else {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    TEEC_4PARAMS(p, in, in_len, out, 0, type, hash_op, ctx, 0);

    t = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, \
                         TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INOUT);

    return tee_send(TEE_CMD_SHA_DIGEST, p, t);
}
#endif

