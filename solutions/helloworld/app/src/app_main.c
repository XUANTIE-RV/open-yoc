/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <aos/kernel.h>
#include <ulog/ulog.h>
#include "app_main.h"

#define TAG "app"

int main(int argc, char *argv[])
{
    board_yoc_init();
    LOGI(TAG, "app start........\n");
    while (1) {
        LOGI(TAG, "hello world");
        aos_msleep(3000);
    };
}