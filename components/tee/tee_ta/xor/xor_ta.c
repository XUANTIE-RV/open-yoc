/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tee_internel_api.h>
#include <tee_crypto_api.h>
#include <tee_storage_api.h>
#include <tee_msg_cmd.h>
#include <tee_srv.h>
#include <string.h>

#define  XOR_TA_UUID   {0x5445455f, 0x4944, 0x325f,                             \
                       {0x53, 0x52, 0x56, 0x5f, 0x55, 0x55, 0x19, 0x33 }        \
                       }

static tee_result _xor_ta_createentrypoint(void)
{
    return TEE_SUCCESS;
}

static void _xor_ta_destroyentrypoint(void)
{

}

static tee_result _xor_ta_opensessionentrypoint(uint32_t  paramtypes,
                                                tee_param params[4],
                                                void    **sessioncontext)
{
    return TEE_SUCCESS;
}

static void _xor_ta_closesessionentrypoint(void *sessioncontext)
{

}

static int xor_sample(tee_param *params)
{
    int ret = TEE_SUCCESS;
    uint8_t *xor_buf = (uint8_t *)params[0].memref.buffer;
    uint32_t *xor_buf_size = (uint32_t *)params[0].memref.size;

    xor_buf[0] = 0x55;
    xor_buf[1] = 0xaa;
    xor_buf[2] = 0x5a;
    xor_buf[3] = 0xa5;

    *xor_buf_size = 4;

    return ret;
}

static tee_result _xor_ta_invokecommandentrypoint(void     *sessioncontext,
                                                  uint32_t  commandid,
                                                  uint32_t  paramtypes,
                                                  tee_param params[4])
{

    int ret = TEE_SUCCESS;

    TEE_LOGI("TEE XOR get cmd id: %d\n", commandid);

    switch(commandid) {
        case TEE_CMD_XOR:
            ret = xor_sample(params);
        break;
        default:
        break;
    }
    return ret;
}

TEE_SRV_DATA_START(_xor_ta_createentrypoint,
                   _xor_ta_destroyentrypoint,
                   _xor_ta_opensessionentrypoint,
                   _xor_ta_closesessionentrypoint,
                   _xor_ta_invokecommandentrypoint)
TEE_SRV_UUID_PROPERTY("gpd.ta.appID", XOR_TA_UUID)
TEE_SRV_BOOL_PROPERTY("gpd.ta.singleInstance", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.multiSession", true)
TEE_SRV_BOOL_PROPERTY("gpd.ta.instanceKeepAlive", true)
TEE_SRV_INT_PROPERTY("gpd.ta.dataSize", 0x1000)
TEE_SRV_INT_PROPERTY("gpd.ta.stackSize", 0x1000)
TEE_SRV_DATA_END

