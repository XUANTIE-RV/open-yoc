/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_DRIVER_H
#define DEVICE_DRIVER_H

#include <stdint.h>
#include <string.h>

#include <aos/list.h>
#include <aos/log.h>
#include <aos/debug.h>

#include <devices/device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _driver_obj driver_t;

struct _driver_obj {
    slist_t node;
    char   *name;
    char   *type;
    int16_t ref;
    int16_t device_id;

    aos_dev_t * (*init)(driver_t *drv, void *config, int id);
    void    (*uninit)(aos_dev_t *dev);
    int     (*lpm)(aos_dev_t *dev, int state);
    int     (*open)(aos_dev_t *dev);
    int     (*close)(aos_dev_t *dev);
};

#define DRIVER(dev)  ((driver_t*)(dev->drv))

/**
  \brief       register device object into device driver framework.
  \param[in]   drv      device driver object
  \param[in]   count    device number
  \return      <0 error, >0 return dev count.
*/
int driver_register(driver_t *drv, void *config, int idx);

/**
  \brief       remove device object from device driver framework.
  \param[in]   name     device name
  \return      <0 error, >0 return operation result
*/
int driver_unregister(const char *name);

/**
  \brief       get device object by device name
  \param[in]   name     device name
  \param[in]   id       device index
  \return      null on error, device object pointer on success
*/
aos_dev_t *device_find(const char *name, int id);

/**
  \brief       allocate new node for new device driver
  \param[in]   drv      device driver object
  \param[in]   size     device driver object size
  \param[in]   id       device node index
  \retrun      null on error, device object pointer on success
*/
aos_dev_t *device_new(driver_t *drv, int size, int id);

/**
  \brief       free a device node
  \param[in]   dev      device driver object
  \return      none
*/
void device_free(aos_dev_t *dev);

/**
  \brief       lock device to prevent to access
  \param[in]   dev      device driver object
  \return      none
*/
int device_lock(aos_dev_t *dev);

/**
  \brief       unlock device to allow to access
  \param[in]   dev      device driver object
  \return      none
*/
int device_unlock(aos_dev_t *dev);

/**
  \brief       valid a device 's name
  \param[in]   dev      device driver object
  \param[in]   name     device name
  \return      =0 success other is error
*/
int device_valid(aos_dev_t *dev, const char *name);

int device_tags(aos_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif
