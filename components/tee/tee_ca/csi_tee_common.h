/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_COMMON_H
#define HAL_COMMON_H

#include "tee_client_api.h"
#include "tee_entry.h"
#include "drv_tee.h"
#include "drv_errno.h"

#define ERR_TEE(errno) (CSI_DRV_ERRNO_TEE_BASE | errno)
#define PARAM_CHECK(p) \
    if (0 == p) \
    { \
        return ERR_TEE(DRV_ERROR_PARAMETER); \
    }

teec_result tee_send(uint32_t cmd_id, teec_parameter params[4], uint32_t params_type);
teec_result tee_send_xor(uint32_t cmd_id, teec_parameter params[4], uint32_t params_type);
#endif
