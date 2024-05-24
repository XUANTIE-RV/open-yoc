/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <drv/tick.h>

/* time(uS) since run */
unsigned long long clock()
{
    return csi_tick_get_us();
}

/* run time since run (seconds)*/
long time()
{
    return csi_tick_get_us() / 1000000;
}

