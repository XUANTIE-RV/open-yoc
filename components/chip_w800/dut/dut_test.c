/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include "stdint.h"
#include "dut/hal/common.h"
#include "dut/hal/ble.h"

extern int tls_get_mac_addr(uint8_t * mac);
extern int tls_set_mac_addr(uint8_t * mac);

int dut_hal_mac_get(uint8_t addr[6])
{
    return tls_get_mac_addr(addr);
}

int dut_hal_mac_store(uint8_t addr[6])
{
    return tls_set_mac_addr(addr);
}

