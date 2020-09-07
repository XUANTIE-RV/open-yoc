/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include "tee_client_api.h"
#include "tee_entry.h"
#include "csi_tee_common.h"

#if (CONFIG_SYS_FREQ > 0)

/****** system freqence operation type *****/
typedef enum {
    SET_FREQ    = 0, ///< SET
    GET_FREQ    = 1  ///< GET
} opr_type_e;

int32_t csi_tee_set_sys_freq(uint32_t clk_src, uint32_t clk_val)
{
    teec_parameter p[4];
    uint32_t t;

    TEEC_2PARAMS(p, SET_FREQ, clk_src, clk_val, 0);

    t = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, \
                         TEEC_MEMREF_TEMP_INPUT, TEEC_NONE);

    return tee_send(TEE_CMD_SYS_FREQ, p, t);
}

int32_t csi_tee_get_sys_freq(uint32_t *clk_val)
{
    teec_parameter p[4];
    uint32_t t;

    TEEC_2PARAMS(p, GET_FREQ, clk_val, 0, 0);

    t = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, \
                         TEEC_NONE, TEEC_NONE);

    return tee_send(TEE_CMD_SYS_FREQ, p, t);

}
#endif

