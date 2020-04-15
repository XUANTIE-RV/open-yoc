/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <yoc_config.h>

#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/cli.h"
#include "main.h"
#include "app_init.h"

#define TAG "app"

int main()
{
    board_yoc_init();
    LOGD(TAG, "%s\n", aos_get_app_version());

    while (1) {
        LOGD(TAG, "Hello world! YoC");
        aos_msleep(1000);
    }

    return 0;
}

