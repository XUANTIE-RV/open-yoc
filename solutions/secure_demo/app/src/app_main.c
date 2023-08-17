/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <aos/kernel.h>
#include <drv/des.h>
#include <drv/aes.h>
#include <drv/hmac.h>
#include <ulog/ulog.h>
#include "app_main.h"

#define TAG "app"

int main(int argc, char *argv[])
{


    board_yoc_init();

    // LOGI(TAG, "app start........\n");
	// se_test();
    // LOGI(TAG, "se_test end........\n");
    // while (1) {
    //     LOGI(TAG, "hello world");
    //     aos_msleep(3000);
    // };
}