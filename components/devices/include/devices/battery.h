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
} rvm_hal_battery_attr_t;

typedef enum {
    REMOVED = 0,
} rvm_hal_battery_event_t;

struct _battery_voltage {
    int volt;
};

struct _battery_current {
    int ampere;
};

typedef struct _battery_voltage rvm_hal_battery_voltage_t;
typedef struct _battery_current rvm_hal_battery_current_t;

typedef int (*battery_event_cb_t)(rvm_hal_battery_event_t event);

typedef struct battery_pin_config {
    int adc_channel;
    int pin;
    battery_event_cb_t event_cb;
} rvm_hal_battery_pin_config_t;

#define rvm_hal_battery_open(name) rvm_hal_device_open(name)
#define rvm_hal_battery_close(dev) rvm_hal_device_close(dev)

int rvm_hal_battery_fetch(rvm_dev_t *dev, rvm_hal_battery_attr_t attr);
int rvm_hal_battery_getvalue(rvm_dev_t *dev, rvm_hal_battery_attr_t attr, void *value, size_t size);
int rvm_hal_battery_event_cb(rvm_hal_battery_event_t event);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_battery.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
