/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/ble.h"
#include "app_main.h"
#include "app_init.h"
#include "pin_name.h"
#include "common/log.h"
#include "yoc/uart_client.h"

#define TAG         "UART CLIENT"
#define DEVICE_NAME "YoC UART CLIENT"

ble_uart_client_t g_uart_client;
uart_handle_t     g_uart_client_handler = NULL;
aos_sem_t         sync_sem;
uint8_t           g_scan_start_flag = 0;

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
        g_scan_start_flag = 0;
    }
}

static void mtu_exchange(ble_event_en event, void *event_data)
{
    LOGI(TAG, "");
}

static int uart_client_profile_recv(const uint8_t *data, int length)
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

int main()
{
    int          ret;
    init_param_t init = {
        .dev_name     = DEVICE_NAME,
        .dev_addr     = NULL,
        .conn_num_max = 1,
    };

    board_yoc_init();

    LOGI(TAG, "Bluetooth uart client demo!");
    aos_sem_new(&sync_sem, 0);

    ble_stack_init(&init);

    ble_stack_setting_load();

    g_uart_client.uart_recv                           = uart_client_profile_recv;
    g_uart_client.uart_event_callback                 = user_event_callback;
    g_uart_client.client_data.client_conf.conn_def_on = 1;
    g_uart_client_handler                             = ble_prf_uart_client_init(&g_uart_client);

    if (!g_uart_client_handler) {
        LOGE(TAG, "init ble uart client demo failed");
    }

    char data[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    while (1) {

        aos_sem_wait(&sync_sem, 2000);

        if (g_uart_client.conn_handle >= 0) {
            if (g_uart_client.client_data.uart_profile.notify_enabled) {
                ret = ble_prf_uart_client_send(g_uart_client_handler, data, sizeof(data), NULL);

                if (ret) {
                    LOGE(TAG, "send data failed");
                }
            }

        } else {
            if (!g_scan_start_flag) {
                ret = ble_prf_uart_client_scan_start();

                if (ret) {
                    LOGE(TAG, "scan start failed");
                } else {
                    g_scan_start_flag = 1;
                }
            }
        }
    }

    return 0;
}
