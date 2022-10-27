/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _BT_HIDS_H_
#define _BT_HIDS_H_

typedef void *hids_handle_t;

typedef enum {
    HIDS_IDX_SVC,
    HIDS_IDX_REPORT_MAP_CHAR,
    HIDS_IDX_REPORT_MAP_VAL,
    HIDS_IDX_REPORT_INPUT_CHAR,
    HIDS_IDX_REPORT_INPUT_VAL,
    HIDS_IDX_REPORT_INPUT_REF,
    HIDS_IDX_REPORT_INPUT_CCC,
    HIDS_IDX_REPORT_OUTPUT_CHAR,
    HIDS_IDX_REPORT_OUTPUT_VAL,
    HIDS_IDX_REPORT_OUTPUT_REF,
    // HIDS_IDX_REPORT_FEATURE_CHAR,
    // HIDS_IDX_REPORT_FEATURE_VAL,
    // HIDS_IDX_REPORT_FEATURE_DES,
    HIDS_IDX_BOOT_KB_INPUT_REPORT_CHAR,
    HIDS_IDX_BOOT_KB_INPUT_REPORT_VAL,
    HIDS_IDX_BOOT_KB_INPUT_REPORT_CCC,
    HIDS_IDX_BOOT_KB_OUTPUT_REPORT_CHAR,
    HIDS_IDX_BOOT_KB_OUTPUT_REPORT_VAL,
    HIDS_IDX_INFO_CHAR,
    HIDS_IDX_INFO_VAL,
    HIDS_IDX_CTRL_CHAR,
    HIDS_IDX_CTRL_VAL,
    HIDS_IDX_PROTOCOL_MODE_CHAR,
    HIDS_IDX_PROTOCOL_MODE_VAL,

    HIDS_IDX_MAX,
} hids_event_e;

enum {
    REPORT_MAP    = 0x00,
    REPORT_INPUT  = 0x01,
    REPORT_OUTPUT = 0x02,
    // REPORT_FEATURE = 0x03,

    REPORT_MAX,
};

enum {
    HIDS_BOOT_PROTOCOL_MODE   = 0x00,
    HIDS_REPORT_PROTOCOL_MODE = 0x01,
};

typedef void (*hids_event_cb)(hids_event_e event, void *event_data); // Event call back.

hids_handle_t ble_prf_hids_init(uint8_t mode);
int           ble_prf_hids_set_data_map(uint8_t u_data[], uint16_t len, uint8_t u_type);
int           ble_prf_hids_key_send(hids_handle_t handle, uint8_t *key_code, uint16_t us_len);
int           ble_prf_hids_notify_send(hids_handle_t handle, uint8_t *key_code, uint16_t us_len);
int           ble_prf_hids_regist(int32_t idx, hids_event_cb cb_event);

/// deprecated fucntion hids_init, use ble_prf_hids_init instead
static inline __bt_deprecated hids_handle_t hids_init(uint8_t mode)
{
    return ble_prf_hids_init(mode);
}

/// deprecated fucntion set_data_map, use ble_prf_hids_set_data_map instead
static inline __bt_deprecated int set_data_map(uint8_t u_data[], uint16_t len, uint8_t u_type)
{
    return ble_prf_hids_set_data_map(u_data, len, u_type);
}

/// deprecated fucntion hids_key_send, use ble_prf_hids_key_send instead
static inline __bt_deprecated int hids_key_send(hids_handle_t handle, uint8_t *key_code, uint16_t us_len)
{
    return ble_prf_hids_key_send(handle, key_code, us_len);
}

/// deprecated fucntion hids_notify_send, use ble_prf_hids_notify_send instead
static inline __bt_deprecated int hids_notify_send(hids_handle_t handle, uint8_t *key_code, uint16_t us_len)
{
    return ble_prf_hids_notify_send(handle, key_code, us_len);
}

/// deprecated fucntion init_hids_call_func, use ble_prf_hids_regist instead
static inline __bt_deprecated int init_hids_call_func(int32_t idx, hids_event_cb cb_event)
{
    return ble_prf_hids_regist(idx, cb_event);
}

#endif
