/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_WDT_H_
#define _DEVICE_WDT_H_

#include <stdio.h>
#include <stdlib.h>
#include <devices/device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*rvm_hal_wdt_callback)(rvm_dev_t *dev, void *arg);

#define rvm_hal_wdt_open(name)        rvm_hal_device_open(name)
#define rvm_hal_wdt_close(dev)        rvm_hal_device_close(dev)

/**
  \brief       Set the WDT value
  \param[in]   dev    Pointer to device object.
  \param[in]   ms     The timeout value(ms)
  \return      0 on success, else on fail.
*/
int rvm_hal_wdt_set_timeout(rvm_dev_t *dev, uint32_t ms);

/**
  \brief       Start the WDT
  \param[in]   dev    Pointer to device object.
  \return      0 on success, else on fail.
*/
int rvm_hal_wdt_start(rvm_dev_t *dev);

/**
  \brief       Stop the WDT
  \param[in]   dev    Pointer to device object.
  \return      0 on success, else on fail.
*/
int rvm_hal_wdt_stop(rvm_dev_t *dev);

/**
  \brief       Feed the WDT
  \param[in]   dev    Pointer to device object.
  \return      0 on success, else on fail.
*/
int rvm_hal_wdt_feed(rvm_dev_t *dev);

/**
  \brief       Get the remaining time to timeout
  \param[in]   dev    Pointer to device object.
  \return      The remaining time of WDT(ms)
*/
uint32_t rvm_hal_wdt_get_remaining_time(rvm_dev_t *dev);

/**
  \brief       Attach the callback handler to WDT
  \param[in]   dev    Pointer to device object.
  \param[in]   callback    Callback function
  \param[in]   arg         Callback's param
  \return      0 on success, else on fail.
*/
int rvm_hal_wdt_attach_callback(rvm_dev_t *dev, rvm_hal_wdt_callback callback, void *arg);

/**
  \brief       Detach the callback handler
  \param[in]   dev    Pointer to device object.
  \return      0 on success, else on fail.
*/
int rvm_hal_wdt_detach_callback(rvm_dev_t *dev);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_wdt.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
