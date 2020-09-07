/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tee_debug.h>
#include <reboot.h>

void tee_defs_entry(void)
{
    TEE_LOGE("tee defense trap \n");

    reboot();
}
