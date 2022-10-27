/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _BT_IBECONS_H_
#define _BT_IBECONS_H_

typedef struct _beacon_info {
    uint8_t id[2];
    uint8_t type[2];
    uint8_t uuid[16];
    uint8_t major[2];
    uint8_t minor[2];
    uint8_t measure_power;
} beacon_info;

int ble_prf_ibeacon_start(uint8_t _id[2], uint8_t _uuid[16], uint8_t _major[2], uint8_t _minor[2],
                          uint8_t _measure_power, char *_sd);

int ble_prf_ibeacon_stop(void);

/// deprecated fucntion ibeacon_start, use ble_prf_ibeacon_start instead
static inline __bt_deprecated int ibeacon_start(uint8_t _id[2], uint8_t _uuid[16], uint8_t _major[2], uint8_t _minor[2],
                                                uint8_t _measure_power, char *_sd)
{
    return ble_prf_ibeacon_start(_id, _uuid, _major, _minor, _measure_power, _sd);
}

/// deprecated fucntion ibeacon_stop, use ble_prf_ibeacon_stop instead
static inline __bt_deprecated int ibeacon_stop(void)
{
    return ble_prf_ibeacon_stop();
}

#endif
