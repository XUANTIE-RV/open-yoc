/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "string.h"
#include "tee_debug.h"

extern void process_ta_command(uint32_t recv_msg);

int dispatcher_main(int param1, int param2, int param3, int param4)
{
    (void)param2;
    (void)param3;
    (void)param4;

    TEE_LOGD("--- Enter TEE ---\n");

    process_ta_command(param1);

    TEE_LOGD("--- Exit TEE ---\n");
    return 0;
}
