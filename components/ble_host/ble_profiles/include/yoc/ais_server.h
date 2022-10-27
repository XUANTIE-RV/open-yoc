/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef __AIS_SERVER_H
#define __AIS_SERVER_H
#include "yoc/ble_ais.h"

typedef enum {
    AIS_SERVER_GATT_EVENT_CONN,
    AIS_SERVER_GATT_EVENT_DISCONN,
    AIS_SERVER_GATT_EVENT_MTU_EXCHANGE,
    AIS_SERVER_GATT_EVENT_DISCOVER_SUCCEED,
    AIS_SERVER_GATT_EVENT_NOTIFY,
    AIS_SERVER_GATT_EVENT_WRITE,
} ais_server_event_en;

int  ble_prf_ais_server_init(ais_cb cb);
void ble_prf_ais_server_indicate(uint8_t msg_id, uint8_t cmd, uint8_t *p_msg, uint16_t len);
void ble_prf_ais_server_notify(uint8_t msg_id, uint8_t cmd, uint8_t *p_msg, uint16_t len);
int  ble_prf_ais_server_disconnect();

/// deprecated fucntion ble_ais_server_init, use ble_prf_ais_server_init instead
static inline __bt_deprecated int ble_ais_server_init(ais_cb cb)
{
    return ble_prf_ais_server_init(cb);
}
/// deprecated fucntion ble_ais_server_indicate, use ble_prf_ais_server_indicate instead
static inline __bt_deprecated void ble_ais_server_indicate(uint8_t msg_id, uint8_t cmd, uint8_t *p_msg, uint16_t len)
{
    return ble_prf_ais_server_indicate(msg_id, cmd, p_msg, len);
}

/// deprecated fucntion ble_ais_server_notify, use ble_prf_ais_server_notify instead
static inline __bt_deprecated void ble_ais_server_notify(uint8_t msg_id, uint8_t cmd, uint8_t *p_msg, uint16_t len)
{
    return ble_prf_ais_server_notify(msg_id, cmd, p_msg, len);
}

/// deprecated fucntion ble_ais_server_disconnect, use ble_prf_ais_server_disconnect instead
static inline __bt_deprecated int ble_ais_server_disconnect()
{
    return ble_prf_ais_server_disconnect();
}

#endif
