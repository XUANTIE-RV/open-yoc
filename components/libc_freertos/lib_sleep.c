/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdint.h>

extern uint32_t msleep(uint32_t ms);
__attribute__((weak)) uint32_t sleep(uint32_t seconds)
{
    if (seconds) {
        msleep(seconds * 1000);
    }

    return 0;
}

