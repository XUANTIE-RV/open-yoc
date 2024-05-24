/*
 * Copyright (C) 2019-2024 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "aos/cli.h"
#include "app_main.h"
#include "app_init.h"

#define TAG "app"

extern void example_main();

int main(void)
{
    board_yoc_init();
    LOGI(TAG, "kernel version : %s\r\n", aos_kernel_version_get());
    example_main();
    while (1) {
        aos_msleep(3000);
    }
    return 0;
}

