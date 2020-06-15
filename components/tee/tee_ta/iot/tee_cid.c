/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tee_internel_api.h>
#include <tee_msg_cmd.h>
#include <string.h>
#include <tee_debug.h>
#include <kp.h>

#if (CONFIG_DEV_CID > 0)

#define CID_LEN     32

#if (CONFIG_TB_KP == 0)
static const char cid_device_id[CID_LEN + 1] = "Y0482208A0D6E011A";
#endif

int tee_core_cid(tee_param params[4])
{
    uint32_t *cid_size;

    cid_size = (uint32_t *)params[0].memref.size;

    if (*cid_size < CID_LEN) {
        TEE_LOGE("cid size too mall\n");
        return TEE_ERROR_BAD_PARAMETERS;
    }

#if (CONFIG_TB_KP > 0)
    uint32_t keyaddr;
    int ret;
    ret = kp_get_key(KEY_CID, &keyaddr);

    if (ret < 0) {
        TEE_LOGE("get key cid fail, %d\n", ret);
        *cid_size = 0;
        return TEE_ERROR_GENERIC;
    }

    *cid_size = ret;
    memcpy(params[0].memref.buffer, (uint32_t *)keyaddr, *cid_size);
#else
    memcpy(params[0].memref.buffer, cid_device_id, CID_LEN);
    *cid_size = strlen(cid_device_id);
#endif
    return TEE_SUCCESS;
}

#endif
