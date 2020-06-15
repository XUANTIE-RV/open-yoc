/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "tee_client_api.h"
#include "tee_entry.h"
#include "csi_tee_common.h"

#if (CONFIG_ALGO_LPM > 0)

int32_t csi_tee_enter_lpm(uint32_t gate, uint32_t irqid, tee_lpm_mode_e mode)
{
    teec_parameter p[4];
    uint32_t t;

    if (mode >= TEE_LPM_MODE_MAX) {
        return ERR_TEE(DRV_ERROR_PARAMETER);
    }

    TEEC_2PARAMS(p, gate, irqid, mode, 0);

    t = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, \
                         TEEC_NONE, TEEC_NONE);

    return tee_send(TEE_CMD_LPM, p, t);
}
#endif

