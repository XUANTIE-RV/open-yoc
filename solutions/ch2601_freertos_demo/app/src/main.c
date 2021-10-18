/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "main.h"
#include "app_init.h"
#include "oled.h"

#define TAG "app"

static aos_task_t app_task1_handle;
static aos_task_t app_task2_handle;

static void application_task1_entry(void *arg)
{
    while (1) {
        LOGD(TAG, "Hello task1! YoC");
        aos_msleep(1000);
    }

    aos_task_exit(0);
}

static void application_task2_entry(void *arg)
{
    while (1) {
        LOGD(TAG, "Hello task2! YoC");
        aos_msleep(1000);
    }

    aos_task_exit(0);
}

int main(void)
{
    board_yoc_init();
    LOGD(TAG, "%s\n", aos_get_app_version());
    oled_init();

    aos_task_new_ext(&app_task1_handle, "app_task1", application_task1_entry,
                     NULL, 4096, AOS_DEFAULT_APP_PRI);

    aos_task_new_ext(&app_task2_handle, "app_task2", application_task2_entry,
                     NULL, 4096, AOS_DEFAULT_APP_PRI);

    while (1) {
        aos_msleep(1000);
    }

    return 0;
}

