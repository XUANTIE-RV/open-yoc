/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <aos/kernel.h>
#include <aos/yloop.h>
#include "app_main.h"

int main(void)
{
    board_yoc_init();
    app_exception_init();
    app_fota_init();
#ifdef CONFIG_WIFI_DRIVER_BL606P
    aos_loop_run();
#endif
    while(1) {
        aos_msleep(5000);
    }
}
