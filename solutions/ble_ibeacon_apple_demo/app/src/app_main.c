/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/ble.h"
#include "app_main.h"
#include "app_init.h"
#include <yoc/ibeacons.h>
#include "pin_name.h"

#define TAG  "DEMO"
#define NAME "WeChat Beacon"
#define DEVICE_ADDR                                                                                                    \
    {                                                                                                                  \
        0xCE, 0x3B, 0xE3, 0x82, 0xBA, 0xC0                                                                             \
    }

uint8_t BEACON_ID[2] = { 0X4C, 0X00 }; // Must not be used for any purposes not specified by Apple.
// user define
uint8_t TANCENT_UUID[16]
    = { 0XFD, 0XA5, 0X06, 0X93, 0XA4, 0XE2, 0X4F, 0XB1, 0XAF, 0XCF, 0XC6, 0XEB, 0X07, 0X64, 0X78, 0X25 }; // 16 BYTE
uint8_t MAJOR[2] = { 0X27, 0X11 };
uint8_t MINOR[2] = { 0X30, 0X30 };

int main()
{
    int          ret;
    dev_addr_t   addr = { DEV_ADDR_LE_RANDOM, DEVICE_ADDR };
    init_param_t init = {
        .dev_name     = NULL,
        .dev_addr     = &addr,
        .conn_num_max = 0,
    };

    board_yoc_init();
    ble_stack_init(&init);
    ble_stack_setting_load();
    LOGI(TAG, "Bluetooth apple ibeacon demo!");

    ret = ble_prf_ibeacon_start(BEACON_ID, TANCENT_UUID, MAJOR, MINOR, 0xBA, NAME);

    if (ret) {
        LOGE(TAG, "ibeacon start fail %d!", ret);
    } else {
        LOGI(TAG, "ibeacon start!");
    }

    return 0;
}
