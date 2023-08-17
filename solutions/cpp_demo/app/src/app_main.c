/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <aos/kernel.h>
#include <ulog/ulog.h>
#include "app_main.h"

#define TAG "app"

extern void cxx_system_init(void);
extern int run_cpp_case();
int main(int argc, char *argv[])
{
    cxx_system_init();
    board_yoc_init();
    LOGI(TAG, "app start........\n");
    run_cpp_case();
    while (1) {
        aos_msleep(3000);
    };
}
