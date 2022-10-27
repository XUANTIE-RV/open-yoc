/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _BT_HRS_H_
#define _BT_HRS_H_

typedef struct _hrs_t {
    uint16_t conn_handle;
    uint16_t hrs_svc_handle;
    int16_t  mea_ccc;
    uint8_t  hrs_mea_flag;
    uint8_t  hrs_mea_level;
    slist_t  next;
} hrs_t;

typedef hrs_t *hrs_handle_t;

hrs_handle_t ble_prf_hrs_init(hrs_t *hrs);
int          ble_prf_hrs_measure_level_update(hrs_handle_t handle, uint8_t *data, uint8_t length);

/// deprecated fucntion hrs_init, use ble_prf_hrs_init instead
static inline __bt_deprecated hrs_handle_t hrs_init(hrs_t *hrs)
{
    return ble_prf_hrs_init(hrs);
}

/// deprecated fucntion hrs_measure_level_update, use ble_prf_hrs_measure_level_update instead
static inline __bt_deprecated int hrs_measure_level_update(hrs_handle_t handle, uint8_t *data, uint8_t length)
{
    return ble_prf_hrs_measure_level_update(handle, data, length);
}

#endif
