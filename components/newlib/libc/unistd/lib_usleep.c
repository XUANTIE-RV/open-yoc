/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <aos/kernel.h>

uint32_t usleep(uint32_t usec)
{
    if (usec) {
        if (usec < 1000) {
            usec = 1000;
        }

        aos_msleep(usec / 1000);
    }

    return 0;
}

