/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/ble.h"
#include "app_main.h"
#include "app_init.h"
#include "pin_name.h"
#include <yoc/hrs.h>
#include "aos/kv.h"


#define TAG "DEMO"

#define DEVICE_NAME "YoC HRS"
#define DEVICE_ADDR {0xCC,0x3B,0xE3,0x88,0xBF,0xC0}
#define MEA_CHA_FLAG 0X00

uint8_t g_hrs_mea_level = 0;

int16_t g_conn_hanlde = 0xFFFF;
int16_t adv_onging = 0;
hrs_t g_hrs;
hrs_handle_t g_hrs_handle = NULL;
char node_name[40] = {0};

static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED) {
        g_conn_hanlde = e->conn_handle;
        LOGI(TAG, "Connected");
    } else {
        g_conn_hanlde = 0xFFFF;
        LOGI(TAG, "Disconnected");
        adv_onging = 0;
    }
}

static void conn_param_update(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_param_update_t *e = event_data;

    LOGI(TAG, "LE conn param updated: int 0x%04x lat %d to %d\n", e->interval,
         e->latency, e->timeout);
}

static void mtu_exchange(ble_event_en event, void *event_data)
{
    evt_data_gatt_mtu_exchange_t *e = (evt_data_gatt_mtu_exchange_t *)event_data;

    if (e->err == 0) {
        LOGI(TAG, "mtu exchange, MTU %d", ble_stack_gatt_mtu_get(e->conn_handle));
    } else {
        LOGE(TAG, "mtu exchange fail, %d", e->err);
    }
}

static int event_callback(ble_event_en event, void *event_data)
{
    LOGD(TAG, "event %x", event);

    switch (event) {
        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;

        case EVENT_GAP_CONN_PARAM_UPDATE:
            conn_param_update(event, event_data);
            break;

        case EVENT_GATT_MTU_EXCHANGE:
            mtu_exchange(event, event_data);
            break;

        default:
            LOGD(TAG, "Unhandle event %x", event);
            break;
    }

    return 0;
}

static int start_adv(void)
{
    int ret;
    ad_data_t ad[3] = {0};

    uint8_t flag = AD_FLAG_GENERAL | AD_FLAG_NO_BREDR;
    ad[0].type = AD_DATA_TYPE_FLAGS;
    ad[0].data = (uint8_t *)&flag;
    ad[0].len = 1;

    uint8_t uuid16_list[] = {0x0d, 0x18}; /* UUID_HRS */
    ad[1].type = AD_DATA_TYPE_UUID16_ALL;
    ad[1].data = (uint8_t *)uuid16_list;
    ad[1].len = sizeof(uuid16_list);


    ad[2].type = AD_DATA_TYPE_NAME_COMPLETE;
    ad[2].data = (uint8_t *)node_name;
    ad[2].len = strlen(node_name);

    adv_param_t param = {
        ADV_IND,
        ad,
        NULL,
        BLE_ARRAY_NUM(ad),
        0,
        ADV_FAST_INT_MIN_1,
        ADV_FAST_INT_MAX_1,
    };

    ret = ble_stack_adv_start(&param);

    if (ret) {
        LOGE(TAG, "hrs adv start fail %d!", ret);
    } else {
        LOGI(TAG, "hrs adv start!");
        adv_onging = 1;
    }

    return ret;
}

static ble_event_cb_t ble_cb = {
    .callback = event_callback,
};

int main()
{
    uint8_t mea_data[2] = {0, 60};
    uint8_t mea_size = sizeof(mea_data) / sizeof(mea_data[0]);
    dev_addr_t addr = {DEV_ADDR_LE_RANDOM, DEVICE_ADDR};
    int length = 40, ret = -1;

    g_hrs_mea_level = 0;
    g_conn_hanlde = 0xFFFF;
    adv_onging = 0;
    g_hrs_handle = NULL;

    board_yoc_init();

    memset(node_name, 0X00, sizeof(node_name));

    LOGI(TAG, "Bluetooth HRS demo!");
    ret = aos_kv_get("HRS_NAME", node_name, &length);

    if (ret != 0 || strlen(node_name) == 0) {
        sprintf(node_name, DEVICE_NAME);
    }

    LOGI(TAG, "DEV_NAME:%s", node_name);
    init_param_t init = {
        .dev_name = NULL,
        .dev_addr = &addr,
        .conn_num_max = 1,
    };
    ble_stack_init(&init);

    ble_stack_event_register(&ble_cb);

    g_hrs_handle = hrs_init(&g_hrs);

    if (g_hrs_handle == NULL) {
        LOGE(TAG, "HRS init FAIL!!!!");
        return -1;
    }

    while (1) {
        if (!adv_onging) {
            ret = start_adv();

            if (ret != 0) {
                LOGI(TAG, "adv fail:%d", ret);
                return -1;
            }
        }

        mea_data[0] = MEA_CHA_FLAG;
        mea_data[1]++;
        ret = hrs_measure_level_update(g_hrs_handle, mea_data, mea_size);

        if (ret != 0) {
            LOGD(TAG, "update fail:%d", ret);
        }

        if (mea_data[1] > 180) {
            mea_data[1] = 60;
        }

        aos_msleep(1000);
    }

    return 0;
}

