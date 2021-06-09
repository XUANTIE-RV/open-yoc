/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "app_main.h"
#include "app_init.h"

#define TAG "BLE SHELL"

int main()
{
    board_yoc_init();
    LOGI(TAG, "Bluetooth shell");
    return 0;
}

