/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "tee_client_api.h"
#include "tee_entry.h"
#include "csi_tee_common.h"

int32_t csi_tee_get_manifest_info(uint8_t *out, uint32_t *out_len, char *name)
{
    teec_parameter p[4];
    uint32_t t;

    PARAM_CHECK(out);
    PARAM_CHECK(out_len);
    PARAM_CHECK(name);

    TEEC_2PARAMS(p, out, out_len, name, 0);

    t = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_INPUT, \
                         TEEC_NONE, TEEC_NONE);

    return tee_send(TEE_CMD_MANIFEST_INFO, p, t);
}

