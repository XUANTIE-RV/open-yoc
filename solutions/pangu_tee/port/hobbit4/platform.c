/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <intc.h>

int hal_platform_init(void)
{
    hal_intc_init();

    return 0;
}

