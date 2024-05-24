/*
 * Copyright (C) 2019-2024 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "aos/cli.h"
#include "app_main.h"
#include "app_init.h"

#define TAG "app"

int main(void)
{
    board_yoc_init();
#if defined(CONFIG_SMP) && CONFIG_SMP
    extern void smp_example(void);
    smp_example();
#endif
    while (1) {
        // LOGD(TAG, "Hello world! YoC");
        aos_msleep(3000);
    }

    return 0;
}

