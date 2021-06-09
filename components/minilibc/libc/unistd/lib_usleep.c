/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <aos/kernel.h>
#include <errno.h>

int usleep(useconds_t usec)
{
    if (usec && (usec <= 1000000)) {
        if (usec < 1000) {
            usec = 1000;
        }

        aos_msleep(usec / 1000);
        return 0;
    }

    return -EINVAL;
}

