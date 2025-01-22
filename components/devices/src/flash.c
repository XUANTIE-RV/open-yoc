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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <dev_internal.h>
#include <devices/impl/flash_impl.h>

#define FLASH_DRIVER(dev)  ((flash_driver_t*)(dev->drv))
#define FLASH_VAILD(dev) do { \
    if (device_valid(dev, "flash") != 0 && device_valid(dev, "eflash") != 0 && device_valid(dev, "spinand") != 0) \
        return -1; \
} while(0)

/**
  \brief       Notify a flash to read data.
  \param[in]   dev  Pointer to device object.
  \param[in]   addroff flash address to be read
  \param[in]   buffer buffer to store data
  \param[in]   bytesize data size to be read
  \return      >=0 on success, -1 on fail.
*/
int rvm_hal_flash_read(rvm_dev_t *dev, int32_t addroff, void *buff, int32_t bytesize)
{
    int ret;

    FLASH_VAILD(dev);

    device_lock(dev);
    ret = FLASH_DRIVER(dev)->read(dev, addroff, buff, bytesize);
    device_unlock(dev);

    return ret;
}

/**
  \brief       Notify a flash to fetch data.
  \param[in]   dev  Pointer to device object.
  \param[in]   dstaddr  flash address to be written
  \param[in]   srcbuf   buffer with data to be written
  \param[in]   bytesize data size to be written
  \return      0 on success, -1 on fail.
*/
int rvm_hal_flash_program(rvm_dev_t *dev, int32_t dstaddr, const void *srcbuf, int32_t bytesize)
{
    int ret;

    FLASH_VAILD(dev);

    device_lock(dev);
    ret = FLASH_DRIVER(dev)->program(dev, dstaddr, srcbuf, bytesize);
    device_unlock(dev);

    return ret;
}

/**
  \brief       Notify a flash to fetch data.
  \param[in]   dev  Pointer to device object.
  \param[in]   addroff  destinate address
  \param[in]   blkcnt  erased block count
  \return      0 on success, -1 on fail.
*/
int rvm_hal_flash_erase(rvm_dev_t *dev, int32_t addroff, int32_t blkcnt)
{
    int ret;

    FLASH_VAILD(dev);

    device_lock(dev);
    ret = FLASH_DRIVER(dev)->erase(dev, addroff, blkcnt);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}

/**
  \brief       Get info from a flash device.
  \param[in]   dev  Pointer to device object.
  \return      flash info.
*/
int rvm_hal_flash_get_info(rvm_dev_t *dev, rvm_hal_flash_dev_info_t *info)
{
    int ret;

    aos_check_param(info);
    FLASH_VAILD(dev);

    device_lock(dev);
    ret = FLASH_DRIVER(dev)->get_info(dev, info);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}
