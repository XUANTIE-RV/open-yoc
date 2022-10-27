#ifndef _UART_SERVER_H
#define _UART_SERVER_H

#include <aos/ble.h>
#include <stdio.h>
#include "yoc/uart_profile.h"

typedef struct _uart_server {
    uint8_t      adv_def_on;
    adv_param_t *advParam;
    ccc_value_en tx_ccc_value;
} uart_server;

typedef struct _ble_uart_server_t {
    int16_t         conn_handle;
    uint16_t        uart_svc_handle;
    uart_rx_data_cb uart_recv;
    ble_event_cb    uart_event_callback;
    conn_param_t *  conn_param;
    uint8_t         conn_update_def_on : 1;
    uint8_t         update_param_flag  : 1;
    uint8_t         mtu_exchanged      : 1;
    uint16_t        mtu;
    uart_server     server_data;
} ble_uart_server_t;

uart_handle_t ble_prf_uart_server_init(ble_uart_server_t *service);
int           ble_prf_uart_server_send(uart_handle_t handle, const char *data, int length, bt_uart_send_cb *cb);
int           ble_prf_uart_server_disconn(uart_handle_t handle);
int           ble_prf_uart_server_adv_control(uint8_t adv_on, adv_param_t *adv_param);
int           ble_prf_uart_server_conn_param_update(uart_handle_t handle, conn_param_t *param);

/// deprecated fucntion uart_server_init, use ble_prf_uart_server_init instead
static inline __bt_deprecated uart_handle_t uart_server_init(ble_uart_server_t *service)
{
    return ble_prf_uart_server_init(service);
}
/// deprecated fucntion uart_server_send, use ble_prf_uart_server_send instead
static inline __bt_deprecated int uart_server_send(uart_handle_t handle, const char *data, int length,
                                                   bt_uart_send_cb *cb)
{
    return ble_prf_uart_server_send(handle, data, length, cb);
}
/// deprecated fucntion uart_server_disconn, use ble_prf_uart_server_disconn instead
static inline __bt_deprecated int uart_server_disconn(uart_handle_t handle)
{
    return ble_prf_uart_server_disconn(handle);
}
/// deprecated fucntion uart_server_adv_control, use ble_prf_uart_server_adv_control instead
static inline __bt_deprecated int uart_server_adv_control(uint8_t adv_on, adv_param_t *adv_param)
{
    return ble_prf_uart_server_adv_control(adv_on, adv_param);
}
/// deprecated fucntion uart_server_conn_param_update, use ble_prf_uart_server_conn_param_update instead
static inline __bt_deprecated int uart_server_conn_param_update(uart_handle_t handle, conn_param_t *param)
{
    return ble_prf_uart_server_conn_param_update(handle, param);
}

#endif
