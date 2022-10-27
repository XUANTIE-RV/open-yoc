/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_REGISTER_H
#define DEVICE_REGISTER_H

#include <stdint.h>
#include <aos/list.h>
#include <aos/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_PM_ACTIVE_STATE          1
#define DEVICE_PM_LOW_POWER_STATE       2
#define DEVICE_PM_SUSPEND_STATE         3
#define DEVICE_PM_OFF_STATE             4

typedef struct _dev_obj {
    slist_t     node;
    uint8_t     id;
    uint8_t     busy;
    uint16_t    ref;
    aos_mutex_t mutex;
    void        *drv;
    void        *config;
} aos_dev_t;

/**
  \brief       open one device
  \param[in]   name     device name
  \param[in]   id       device index to be register
  \return      null on error, device object pointer on success
*/
aos_dev_t *device_open(const char *name);

aos_dev_t *device_open_id(const char *name, int id);

/**
  \brief       close one device
  \param[in]   dev      device name
  \return      <0 error, >0 return device close error
*/
int device_close(aos_dev_t *dev);

int device_is_busy(void);
void device_manage_power(int pm_state);

#ifdef __cplusplus
}
#endif

#endif
