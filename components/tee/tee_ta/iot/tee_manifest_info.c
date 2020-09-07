/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tee_internel_api.h>
#include <tee_msg_cmd.h>
#include <string.h>
#include <mtb.h>

#define OS_VERSION_S "os_v"
#define SYS_PARTION "sys_p"


int tee_core_manifest_info(tee_param params[4])
{
    uint8_t *buf = params[0].memref.buffer;
    uint32_t size = params[0].memref.size;
    char *name = params[1].memref.buffer;

    if (0 == strncmp(name, OS_VERSION_S, sizeof(OS_VERSION_S))) {
        return mtb_get_os_version(buf, (uint32_t *)size);
    }

    if (0 == strncmp(name, SYS_PARTION, sizeof(SYS_PARTION))) {
        return mtb_get_partition_buf(buf, (uint32_t *)size);
    }

    return mtb_get_img_buf(buf, (uint32_t *)size, name);
}

