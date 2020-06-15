/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "tee_client_api.h"
#include "tee_entry.h"
#include "csi_tee_common.h"

int32_t csi_tee_xor(uint8_t *out, uint32_t *out_len)
{
    teec_parameter p[4];
    uint32_t t;

    PARAM_CHECK(out);
    PARAM_CHECK(out_len);
    PARAM_CHECK(*out_len);

    t = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    TEEC_1PARAMS(p, out, out_len);

    return tee_send_xor(TEE_CMD_XOR, p, t);
}



