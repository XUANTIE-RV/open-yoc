/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifndef DEVICE_TIMER_H
#define DEVICE_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

typedef void (*rvm_hal_timer_callback)(rvm_dev_t *dev, void *arg);

#define rvm_hal_timer_open(name) rvm_hal_device_open(name)
#define rvm_hal_timer_close(dev) rvm_hal_device_close(dev)

/**
  \brief       Start TIMER
  \param[in]   dev          Pointer to device object.
  \param[in]   timeout_us   The timeout for TIMER, microsecond
  \return      0 on success, else on fail.
*/
int rvm_hal_timer_start(rvm_dev_t *dev, uint32_t timeout_us);

/**
  \brief       Start TIMER as oneshot mode
  \param[in]   dev          Pointer to device object.
  \param[in]   timeout_us   The timeout for TIMER, microsecond
  \return      0 on success, else on fail.
*/
int rvm_hal_timer_oneshot_start(rvm_dev_t *dev, uint32_t timeout_us);

/**
  \brief       Stop TIMER
  \param[in]   dev      Pointer to device object.
  \return      0 on success, else on fail.
*/
int rvm_hal_timer_stop(rvm_dev_t *dev);

/**
  \brief       Attach the callback handler to TIMER
  \param[in]   dev          Pointer to device object.
  \param[in]   callback     Callback function
  \param[in]   arg          Callback's param
  \return      0 on success, else on fail.
*/
int rvm_hal_timer_attach_callback(rvm_dev_t *dev, rvm_hal_timer_callback callback, void *arg);

/**
  \brief       Detach the callback handler
  \param[in]   dev      Pointer to device object.
  \return      0 on success, else on fail.
*/
int rvm_hal_timer_detach_callback(rvm_dev_t *dev);

/**
  \brief       Get TIMER remaining value
  \param[in]   dev      Pointer to device object.
  \return      remaining value
*/
uint32_t rvm_hal_timer_get_remaining_value(rvm_dev_t *dev);

/**
  \brief       Get TIMER load value
  \param[in]   dev      Pointer to device object.
  \return      Load value
*/
uint32_t rvm_hal_timer_get_load_value(rvm_dev_t *dev);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_timer.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
