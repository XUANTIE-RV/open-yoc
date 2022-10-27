/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include "app_main.h"
#include "app_init.h"
#include <aos/aos.h>

#define TAG "BLE SHELL"

int main()
{
    board_yoc_init();
    LOGI(TAG, "Bluetooth Mesh shell");
    return 0;
}
