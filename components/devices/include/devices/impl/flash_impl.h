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

#ifndef __FLASH_IMPL_H__
#define __FLASH_IMPL_H__

#include <stdint.h>

#include <devices/driver.h>
#include <devices/flash.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct flash_driver {
    driver_t    drv;
    int         (*read)(rvm_dev_t *dev, uint32_t addroff, void *buff, int32_t bytesize);
    int         (*program)(rvm_dev_t *dev, uint32_t dstaddr, const void *srcbuf, int32_t bytesize);
    int         (*erase)(rvm_dev_t *dev, int32_t addroff, int32_t blkcnt);
    int         (*get_info)(rvm_dev_t *dev, rvm_hal_flash_dev_info_t *info);
} flash_driver_t;

#ifdef __cplusplus
}
#endif

#endif
