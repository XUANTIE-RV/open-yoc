 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
