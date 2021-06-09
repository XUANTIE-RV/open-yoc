/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "app_main.h"
#include "app_init.h"
#include "yoc/init.h"

#define TAG "app"

int main()
{
   board_base_init();
   board_yoc_init();
   
    LOGI(TAG, "%s\n", aos_get_app_version());

    while (1) {
        LOGI(TAG, "Hello world! YoC");
        aos_msleep(1000);
    }

    return 0;
}

