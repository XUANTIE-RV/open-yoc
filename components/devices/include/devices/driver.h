/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_DRIVER_H
#define DEVICE_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <aos/list.h>
#include <aos/debug.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _dev_obj rvm_dev_t;
typedef struct _driver_obj driver_t;

struct _driver_obj {
    slist_t node;
    char   *name;
    char   *type;
    int16_t ref;
    int16_t device_id;

    rvm_dev_t *(*init)(driver_t *drv, void *config, int id);
    void    (*uninit)(rvm_dev_t *dev);
    int     (*lpm)(rvm_dev_t *dev, int state);
    int     (*open)(rvm_dev_t *dev);
    int     (*close)(rvm_dev_t *dev);
    int     (*clk_en)(rvm_dev_t *dev, bool enable);
};

#define DRIVER(dev)  ((driver_t*)(dev->drv))

/**
  \brief       register device object into device driver framework.
  \param[in]   drv      device driver object
  \param[in]   count    device number
  \return      -1 error, 0 success.
*/
int rvm_driver_register(driver_t *drv, void *config, int idx);

/**
  \brief       remove device object from device driver framework.
  \param[in]   name     device name
  \return      -1 error, 0 success
*/
int rvm_driver_unregister(const char *name);


#ifdef __cplusplus
}
#endif

#endif
