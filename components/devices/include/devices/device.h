/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_REGISTER_H
#define DEVICE_REGISTER_H

#include <stdint.h>
#include <aos/list.h>
#include <aos/kernel.h>
#include <devices/driver.h>
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _dev_obj {
    slist_t     node;
    uint8_t     id;
    uint8_t     busy;
    uint16_t    ref;
    aos_mutex_t mutex;
    driver_t    *drv;
    void        *config;
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
    devfs_node_t devfs_node;
#endif
} rvm_dev_t;
// #define aos_dev_t rvm_dev_t;

/**
  \brief       open one device by device name
  \param[in]   name     device name, such as uart0, iic0
  \return      null on error, device object pointer on success
*/
rvm_dev_t *rvm_hal_device_open(const char *name);

/**
  \brief       close one device
  \param[in]   dev      device name
  \return      <0 error, >0 return device close error
*/
int rvm_hal_device_close(rvm_dev_t *dev);

/**
  \brief       get device object by device name
  \param[in]   name     device name
  \param[in]   id       device index
  \return      null on error, device object pointer on success
*/
rvm_dev_t *rvm_hal_device_find(const char *name, int id);

/**
  \brief       allocate new node for new device driver
  \param[in]   drv      device driver object
  \param[in]   size     device driver object size
  \param[in]   id       device node index
  \retrun      null on error, device object pointer on success
*/
rvm_dev_t *rvm_hal_device_new(driver_t *drv, int size, int id);

/**
  \brief       free a device node
  \param[in]   dev      device driver object
  \return      none
*/
void rvm_hal_device_free(rvm_dev_t *dev);

/**
  \brief       device enter(leave)lpm
  \param[in]   dev      device driver object
  \param[in]   state    1:enter lpm 0:leave lpm
  \return      =0 success other is error
*/
int rvm_hal_device_lpm_enable(rvm_dev_t *dev, int pm_state);

/**
  \brief       all devices enter(leave)lpm
  \param[in]   pm_state   1:enter lpm 0:leave lpm
  \return      none
*/
void rvm_hal_devices_lpm_enable(int pm_state);

#ifdef __cplusplus
}
#endif

#endif
