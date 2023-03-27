/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _AT_BLE_H
#define _AT_BLE_H

#include "at_ble_def_config.h"
#include "aos/ble.h"
#include <stddef.h>
#include <devices/device.h>


typedef struct _temp_ad_set {
    uint8_t ad_len;
    uint8_t ad[31];
} temp_ad_set;
typedef struct _slave_load {
    uint8_t adv_def_on;
    uint8_t conn_update_def_on;
    conn_param_t conn_param;
    adv_param_t param;
    temp_ad_set ad;
    temp_ad_set sd;
} slave_load;

typedef struct _master_load {
    uint8_t conn_def_on;
    dev_addr_t auto_conn_info[AUTO_CONN_BT_MAX];
    uint8_t auto_conn_num;
} master_load;

typedef struct _uart_service_load {
    slave_load slave_conf;
    master_load master_conf;
} uart_service_load;

typedef struct _at_conf_load {
    uint8_t bt_name[CONFIG_BT_DEVICE_NAME_MAX];
    dev_addr_t addr;
    uint8_t role;
    uint8_t tx_power;
    uint8_t sleep_mode;
    int   baud;
    uart_service_load uart_conf;
} at_conf_load;

typedef struct _at_bt_uart_send_cb {
    void (*start)(int err, void *cb_data);
    void (*end)(int err, void *cb_data);
} at_bt_uart_send_cb;

typedef  int (*at_mode_recv)(uint8_t, char *, int, at_bt_uart_send_cb *);
typedef  int (*uart_mode_recv)(char *, int, at_bt_uart_send_cb *);


typedef struct {
    at_conf_load *at_config;
} at_server_handler;


typedef struct {
    uart_mode_recv uart_mode_recv_cb;
    at_mode_recv at_mode_recv_cb;
} at_ble_cb;

enum BT_EVENT {
    AT_BT_ADV_OFF,
    AT_BT_ADV_ON,
    AT_BT_CONNECT,
    AT_BT_DISCONNECT,
    AT_BT_CONNECT_UPDATE,
    AT_BT_DEV_FIND,
    AT_BT_TX_DATA,
};
enum BT_ROLE {
    SLAVE,
    MASTER,
    NO_ROLE,
};
enum AT_MODE {
    AT_MODE,
    UART_MODE,
};

enum SLEEP_MODE {
    NO_SLEEP,
    SLEEP,
    STANDBY,
};

enum {
    DEF_OFF,
    DEF_ON,
    LOAD_FAIL,
};

typedef struct _tx_power {
    int set_power;
    int set_value;
} tx_power;

#ifdef CONFIG_UNITTEST_MODE
#define _STATIC
#else
#define _STATIC static
#endif

#define AT_ERR_BT_KV_SAVE_PARAM (-30)
#define AT_ERR_AT_SERVICE_NOT_INITIALIZED (-31)
#define AT_ERR_CMD_NOT_SUPPORTED (-32)
#define AT_ERR_BT_KV_UNLOAD (-33)
#define AT_ERR_BT_TX_LEN (-34)
#define AT_ERR_BT_TX_FAILED (-35)

#define AT_BLE_UART_BUFFER_SIZE 1024
#define BLE_BASE_ERR  0x00000000
#define AT_BASE_ERR   0x000003E8

#define ERR_BLE(errno) (-BLE_BASE_ERR + errno)
#define ERR_AT(errno)  (-AT_BASE_ERR  + errno)

#define AT_BACK_RET_INT_HEAD(cmd, val)        atserver_send("\r\n%s:%d", cmd + 2, val)

at_server_handler *at_ble_init();
void at_ble_uartmode_recv(rvm_dev_t *dev);
int at_ble_event_register(const at_ble_cb cb_conf);

#endif

