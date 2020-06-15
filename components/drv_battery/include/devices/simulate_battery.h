/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __SIMULATE_BATTERT_H__
#define __SIMULATE_BATTERT_H__

#include <stdint.h>
#include <devices/battery.h>
#ifdef __cplusplus
extern "C" {
#endif

void battery_simulate_register(battery_pin_config_t *config, int idx);

#ifdef __cplusplus
}
#endif

#endif