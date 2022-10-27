/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _TAG_BLE_OTA_
#define _TAG_BLE_OTA_

#include <stdint.h>

typedef enum {
    OTA_ST_IDLE = 0,
    OTA_ST_START,
    OTA_ST_REQUEST,
    OTA_ST_DL,
    OTA_ST_FLASH,
    OTA_ST_STOP,
    OTA_ST_COMPLETE,
} ota_state_en;

typedef void (*ota_event_callback_t)(ota_state_en ota_state);

int ble_prf_ota_servrer_init(ota_event_callback_t cb);

void ble_prf_ota_service_process();

/// deprecated fucntion ble_ota_init, use ble_prf_ota_servrer_init instead
static inline __bt_deprecated int ble_ota_init(ota_event_callback_t cb)
{
    return ble_prf_ota_servrer_init(cb);
}

/// deprecated fucntion ble_ota_init, use ble_prf_ota_servrer_init instead
static inline __bt_deprecated void ble_ota_process()
{
    return ble_prf_ota_service_process();
}

#endif /* _TAG_BLE_OTA_ */
