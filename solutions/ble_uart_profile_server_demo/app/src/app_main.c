/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/ble.h"
#include "app_main.h"
#include "app_init.h"
#include "common/log.h"

#include "yoc/uart_server.h"

#define TAG         "UART SERVER"
#define DEVICE_NAME "YoC UART SERVER"

ble_uart_server_t g_uart_server;
uart_handle_t     g_uart_server_handler = NULL;
aos_sem_t         sync_sem;
uint8_t           g_adv_start_flag = 0;
uint8_t           g_ccc_value      = 0;
uint8_t           flag1            = AD_FLAG_GENERAL | AD_FLAG_NO_BREDR;
ad_data_t         ad_def[2]        = {
    { .type = AD_DATA_TYPE_FLAGS, .data = &flag1, .len = 1 },
    { .type = AD_DATA_TYPE_UUID128_ALL, .data = UUID128(YOC_UART_SERVICE_UUID), .len = 16 },
};

static void conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED) {
        connect_info_t info = { 0 };
        ble_stack_connect_info_get(e->conn_handle, &info);
        LOGI(TAG, "+CONNECTED:%02x:%02x:%02x:%02x:%02x:%02x,%02x", info.peer_addr.val[5], info.peer_addr.val[4],
             info.peer_addr.val[3], info.peer_addr.val[2], info.peer_addr.val[1], info.peer_addr.val[0],
             info.peer_addr.type);
    } else {
        LOGI(TAG, "+DISCONNECTED:%02x", e->err);
        g_adv_start_flag = 0;
        g_ccc_value      = 0;
    }
}

static void mtu_exchange(ble_event_en event, void *event_data)
{
    LOGI(TAG, "");
}

static int uart_server_profile_recv(const uint8_t *data, int length)
{
    LOGI(TAG, "+DATA:%s", bt_hex(data, length));
    return 0;
}

static int user_event_callback(ble_event_en event, void *event_data)
{
    switch (event) {
        // common event
        case EVENT_GAP_CONN_CHANGE:
            conn_change(event, event_data);
            break;

        case EVENT_GATT_MTU_EXCHANGE:
            mtu_exchange(event, event_data);
            break;

        default:
            break;
    }

    return 0;
}

static int start_adv(void)
{
    ad_data_t ad[3] = { 0 };

    uint8_t flag = AD_FLAG_GENERAL | AD_FLAG_NO_BREDR;

    ad[0].type = AD_DATA_TYPE_FLAGS;
    ad[0].data = (uint8_t *)&flag;
    ad[0].len  = 1;

    ad[1].type = AD_DATA_TYPE_UUID128_ALL;
    ad[1].data = UUID128(YOC_UART_SERVICE_UUID);
    ad[1].len  = 16;

    adv_param_t param = {
        ADV_IND, ad, NULL, BLE_ARRAY_NUM(ad), 0, ADV_FAST_INT_MIN_2, ADV_FAST_INT_MAX_2,
    };

    return ble_prf_uart_server_adv_control(1, &param);
}

int main()
{
    int          ret;
    init_param_t init = {
        .dev_name     = DEVICE_NAME,
        .dev_addr     = NULL,
        .conn_num_max = 1,
    };

    board_yoc_init();

    LOGI(TAG, "Bluetooth uart server demo!");

    aos_sem_new(&sync_sem, 0);

    ble_stack_init(&init);
    ble_stack_setting_load();

    g_uart_server.uart_recv           = uart_server_profile_recv;
    g_uart_server.uart_event_callback = user_event_callback;
    g_uart_server_handler             = ble_prf_uart_server_init(&g_uart_server);

    if (!g_uart_server_handler) {
        LOGE(TAG, "init ble uart server demo failed");
    }

    char data[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    while (1) {
        aos_sem_wait(&sync_sem, 2000);

        if (g_uart_server.conn_handle >= 0) {
            if (g_uart_server.server_data.tx_ccc_value == CCC_VALUE_NOTIFY) {
                ret = ble_prf_uart_server_send(g_uart_server_handler, data, sizeof(data), NULL);

                if (ret) {
                    LOGE(TAG, "send data failed");
                }
            }
        } else {
            if (!g_adv_start_flag) {
                ret = start_adv();

                if (ret) {
                    LOGE(TAG, "adv start failed");
                } else {
                    g_adv_start_flag = 1;
                }
            }
        }
    }

    return 0;
}
