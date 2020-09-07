/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef BATTERY_API_H
#define BATTERY_API_H

#include <devices/battery.h>

#define DEVICE_BATTERY_NAME "battery_fetch"

void battery_fetch_register(battery_pin_config_t *config, int idx);

#endif
