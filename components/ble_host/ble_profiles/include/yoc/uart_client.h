#ifndef _UART_CLIENT_H
#define _UART_CLIENT_H

#include <aos/ble.h>
#include <stdio.h>
#include "yoc/uart_profile.h"

typedef struct client_config {
    uint8_t       conn_def_on;
    uint8_t       auto_conn_mac_size;
    dev_addr_t *  auto_conn_mac;
    dev_addr_t    temp_conn_dev_set;
    conn_param_t *temp_conn_param;
} client_config;

typedef struct _uart_profile_handler {
    dev_addr_t peer_addr;
    uint8_t    notify_enabled;
    uint16_t   uart_handle;
    uint16_t   uart_end_handle;
    uint16_t   uart_char_handle;
    uint16_t   uart_ccc_handle;
} uart_profile_handler;

typedef struct _uart_client {
    client_config        client_conf;
    uart_profile_handler uart_profile;
} uart_client;

typedef struct _ble_uart_client_t {
    int16_t conn_handle;
    // uint16_t uart_svc_handle;
    uart_rx_data_cb uart_recv;
    ble_event_cb    uart_event_callback;
    conn_param_t *  conn_param;
    uint8_t         conn_update_def_on : 1;
    uint8_t         update_param_flag  : 1;
    uint8_t         mtu_exchanged      : 1;
    uint16_t        mtu;
    uart_client     client_data;
} ble_uart_client_t;

uart_handle_t ble_prf_uart_client_init(ble_uart_client_t *service);
int           ble_prf_uart_client_scan_start();
dev_addr_t *  ble_prf_found_dev_get();
int           ble_prf_uart_client_conn(dev_addr_t *conn_mac, conn_param_t *conn_param);
int           ble_prf_uart_client_disconn(uart_handle_t handle);
int           ble_prf_uart_client_send(uart_handle_t handle, const char *data, int length, bt_uart_send_cb *cb);
int           ble_prf_uart_client_conn_param_update(uart_handle_t handle, conn_param_t *param);

/// deprecated fucntion uart_client_init, use ble_prf_uart_client_init instead
static inline __bt_deprecated uart_handle_t uart_client_init(ble_uart_client_t *service)
{
    return ble_prf_uart_client_init(service);
}
/// deprecated fucntion uart_client_scan_start, use ble_prf_uart_client_scan_start instead
static inline __bt_deprecated int uart_client_scan_start()
{
    return ble_prf_uart_client_scan_start();
}
/// deprecated fucntion found_dev_get, use ble_prf_found_dev_get instead
static inline __bt_deprecated dev_addr_t *found_dev_get()
{
    return ble_prf_found_dev_get();
}
/// deprecated fucntion uart_client_conn, use ble_prf_uart_client_conn instead
static inline __bt_deprecated int uart_client_conn(dev_addr_t *conn_mac, conn_param_t *conn_param)
{
    return ble_prf_uart_client_conn(conn_mac, conn_param);
}
/// deprecated fucntion uart_client_disconn, use ble_prf_uart_client_disconn instead
static inline __bt_deprecated int uart_client_disconn(uart_handle_t handle)
{
    return ble_prf_uart_client_disconn(handle);
}
/// deprecated fucntion uart_client_send, use ble_prf_uart_client_send instead
static inline __bt_deprecated int uart_client_send(uart_handle_t handle, const char *data, int length,
                                                   bt_uart_send_cb *cb)
{
    return ble_prf_uart_client_send(handle, data, length, cb);
}
/// deprecated fucntion uart_client_conn_param_update, use ble_prf_uart_client_conn_param_update instead
static inline __bt_deprecated int uart_client_conn_param_update(uart_handle_t handle, conn_param_t *param)
{
    return ble_prf_uart_client_conn_param_update(handle, param);
}

#endif
