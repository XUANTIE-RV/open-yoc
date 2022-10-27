/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "aos/cli.h"
#include "app_main.h"
#include "app_init.h"

#define TAG "app"

int main(void)
{
    board_yoc_init();

    while (1) {
        LOGD(TAG, "Hello world! YoC");
        aos_msleep(3000);
    }

    return 0;
}

