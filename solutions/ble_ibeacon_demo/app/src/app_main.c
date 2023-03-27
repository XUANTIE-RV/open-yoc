/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/ble.h"
#include "app_main.h"
#include "app_init.h"

#define TAG         "DEMO"
#define DEVICE_NAME "YoC_IBEACON"

#define DEVICE_ADDR                                                                                                    \
    {                                                                                                                  \
        0xCC, 0x3B, 0xE3, 0x89, 0xBA, 0xC0                                                                             \
    }
//#define     E_UID           0X0
#define E_URL 0X10
//#define     E_TLM           0X20

int main()
{
    int          ret;
    dev_addr_t   addr = { DEV_ADDR_LE_RANDOM, DEVICE_ADDR };
    init_param_t init = {
        .dev_name     = DEVICE_NAME,
        .dev_addr     = &addr,
        .conn_num_max = 0,
    };
    ad_data_t ad[3] = { 0 };

    board_yoc_init();

    LOGI(TAG, "Bluetooth ibeacon demo!");

    ble_stack_init(&init);
    ble_stack_setting_load();

    uint8_t flag = AD_FLAG_NO_BREDR;
    ad[0].type   = AD_DATA_TYPE_FLAGS;
    ad[0].data   = (uint8_t *)&flag;
    ad[0].len    = 1;

    uint8_t uuid16_list[] = { 0xaa, 0xfe };
    ad[1].type            = AD_DATA_TYPE_UUID16_ALL;
    ad[1].data            = (uint8_t *)uuid16_list;
    ad[1].len             = sizeof(uuid16_list);

#ifdef E_UID
    // UID Data
    uint8_t url_list[] = {
        0xaa,  0xfe,                                                 /* Eddystone UUID */
        E_UID,                                                       /* Frame Type*/
        0x00,                                                        /* Tx Power at 0 m*/
        0x0B,                                                        /*10 Bit NAME SPACE */
        0xB8,  0x1b, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, /* 6 Bit Instance*/
        0x00,  0x00, 0x00, 0x00, 0x00, 0x00,                         /*RFU MUST 0*/
        0x00,
    };
    ad[2].type = AD_DATA_TYPE_SVC_DATA16;
    ad[2].data = (uint8_t *)url_list;
    ad[2].len  = sizeof(url_list);
#endif

#ifdef E_URL
    uint8_t url_list[] = {
        0xaa,  0xfe,                                         /* Eddystone UUID */
        E_URL,                                               /* Eddystone-URL frame type */
        0x00,                                                /* Calibrated Tx power at 0m */
        0x00,                                                /* URL Scheme Prefix http://www. */
        'c',   'o',  'p', '.', 'c', '-', 's', 'k', 'y', 0x07 /* .com */
    };
    ad[2].type = AD_DATA_TYPE_SVC_DATA16;
    ad[2].data = (uint8_t *)url_list;
    ad[2].len  = sizeof(url_list);
#endif

#ifdef E_TLM
    // TLM Data
    uint8_t info_list[] = {
        0xaa,  0xfe,             /* Eddystone UUID */
        E_TLM,                   /* Eddystone-TLM frame type */
        0x00,                    /* TLM VerSion value=00 */
        0x0B,                    /* VBATT[0],1mV/bit */
        0xB8,                    /* VBATT[1],1mV/bit */
        0x1b,                    /* beacon temperature */
        0x00,                    /* beacon temperature */
        0x00,                    /* adv PDU count */
        0x00,  0x00, 0x80, 0x00, /* time since power-on reboot*/
        0x00,  0x80, 0x00,
    };
    ad[2].type = AD_DATA_TYPE_SVC_DATA16;
    ad[2].data = (uint8_t *)info_list;
    ad[2].len  = sizeof(info_list);
#endif

    adv_param_t param = {
        ADV_NONCONN_IND, ad, NULL, BLE_ARRAY_NUM(ad), 0, ADV_FAST_INT_MIN_2, ADV_FAST_INT_MAX_2,
    };

    ret = ble_stack_adv_start(&param);

    if (ret) {
        LOGE(TAG, "ibeacon start fail %d!", ret);
    } else {
        LOGI(TAG, "ibeacon start!");
    }

    return 0;
}
