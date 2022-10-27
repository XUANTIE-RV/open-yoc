/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_BATTERY_H
#define DEVICE_BATTERY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

typedef enum {
    VOLTAGE = 0,
    CURRENT
} hal_battery_attr_t;

typedef enum {
    REMOVED = 0,
} hal_battery_event_t;

struct _battery_voltage {
    int volt;
};

struct _battery_current {
    int ampere;
};

typedef struct _battery_voltage battery_voltage_t;
typedef struct _battery_current battery_current_t;

typedef int (*battery_event_cb_t)(hal_battery_event_t event);

typedef struct battery_pin_config {
    int adc_channel;
    int pin;
    battery_event_cb_t event_cb;
} battery_pin_config_t;

#define battery_open(name) device_open(name)
#define battery_open_id(name, id) device_open_id(name, id)
#define battery_close(dev) device_close(dev)

int battery_fetch(aos_dev_t *dev, hal_battery_attr_t attr);
int battery_getvalue(aos_dev_t *dev, hal_battery_attr_t attr, void *value, size_t size);
int battery_event_cb(hal_battery_event_t event);

#ifdef __cplusplus
}
#endif

#endif
