/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <aos/kernel.h>

/*
POSIX.1-2001, POSIX.1-2008.
*/
__attribute__((weak)) uint32_t sleep(uint32_t seconds)
{
    if (seconds) {
        aos_msleep(seconds * 1000);
    }

    return 0;
}

