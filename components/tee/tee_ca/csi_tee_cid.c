/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "tee_client_api.h"
#include "tee_entry.h"
#include "csi_tee_common.h"

#if (CONFIG_DEV_CID > 0)

int32_t csi_tee_get_cid(uint8_t *out, uint32_t *out_len)
{
    teec_parameter p[4];
    uint32_t t;

    PARAM_CHECK(out);
    PARAM_CHECK(out_len);
    PARAM_CHECK(*out_len);

    t = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    TEEC_1PARAMS(p, out, out_len);

    return tee_send(TEE_CMD_GET_ID, p, t);
}
#endif
