/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <rtthread.h>

__attribute__((weak)) uint32_t msleep(uint32_t ms)
{
    if (ms) {
        rt_thread_mdelay(ms);
    }

    return 0;
}

