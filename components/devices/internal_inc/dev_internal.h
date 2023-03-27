/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_INTERNAL_H_
#define _DEVICE_INTERNAL_H_

#include <stdint.h>
#include <devices/driver.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
  \brief       get device object by device name and id
  \param[in]   name     device name
  \param[in]   id       device index
  \return      null on error, device object pointer on success
*/
rvm_dev_t *device_open_id(const char *name, int id);

/**
  \brief       lock device to prevent to access
  \param[in]   dev      device driver object
  \return      none
*/
int device_lock(rvm_dev_t *dev);

/**
  \brief       unlock device to allow to access
  \param[in]   dev      device driver object
  \return      none
*/
int device_unlock(rvm_dev_t *dev);

/**
  \brief       valid a device 's name
  \param[in]   dev      device driver object
  \param[in]   name     device name
  \return      =0 success other is error
*/
int device_valid(rvm_dev_t *dev, const char *name);


/**
  \brief       device is busy
  \return      1 is on busy, 0 is in idle
*/
int device_is_busy(void);


#ifdef __cplusplus
}
#endif

#endif
