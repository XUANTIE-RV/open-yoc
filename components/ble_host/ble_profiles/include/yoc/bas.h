/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#ifndef _BT_BAS_H_
#define _BT_BAS_H_

typedef struct _bas_t {
    uint16_t conn_handle;
    uint16_t bas_svc_handle;
    int16_t  ccc;
    uint8_t  battery_level;
    slist_t  next;
} bas_t;

typedef bas_t *bas_handle_t;

bas_handle_t ble_prf_bas_init(bas_t *bas);
int          ble_prf_bas_level_update(bas_handle_t handle, uint8_t level);

/// deprecated fucntion bas_init, use ble_prf_bas_init instead
static inline __bt_deprecated bas_handle_t bas_init(bas_t *bas)
{
    return ble_prf_bas_init(bas);
}

/// deprecated fucntion bas_level_update, use ble_prf_bas_level_update instead
static inline __bt_deprecated int bas_level_update(bas_handle_t handle, uint8_t level)
{
    return ble_prf_bas_level_update(handle, level);
}

#endif
