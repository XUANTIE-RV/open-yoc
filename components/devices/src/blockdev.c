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
#include <dev_internal.h>
#include <devices/impl/blockdev_impl.h>

#define BLOCKDEV_DRIVER(dev)  ((blockdev_driver_t*)(dev->drv))
#define BLOCKDEV_VAILD(dev) do { \
    if (device_valid(dev, "mmc") != 0 && device_valid(dev, "sd") != 0 && device_valid(dev, "usb_mass") != 0) \
        return -1; \
} while(0)

int rvm_hal_blockdev_read_blks(rvm_dev_t *dev, void *buffer, uint32_t start_block, uint32_t block_cnt)
{
    int ret;

    BLOCKDEV_VAILD(dev);

    device_lock(dev);
    ret = BLOCKDEV_DRIVER(dev)->read_blks(dev, buffer, start_block, block_cnt);
    device_unlock(dev);

    return ret;
}

int rvm_hal_blockdev_write_blks(rvm_dev_t *dev, void *buffer, uint32_t start_block, uint32_t block_cnt)
{
    int ret;

    BLOCKDEV_VAILD(dev);

    device_lock(dev);
    ret = BLOCKDEV_DRIVER(dev)->write_blks(dev, buffer, start_block, block_cnt);
    device_unlock(dev);

    return ret;
}

int rvm_hal_blockdev_erase_blks(rvm_dev_t *dev, uint32_t start_block, uint32_t block_cnt)
{
    int ret;

    BLOCKDEV_VAILD(dev);

    device_lock(dev);
    ret = BLOCKDEV_DRIVER(dev)->erase_blks(dev, start_block, block_cnt);
    device_unlock(dev);

    return ret;
}

#if defined(CONFIG_COMP_SDMMC)
int rvm_hal_blockdev_mmc_select_area(rvm_dev_t *dev, rvm_hal_mmc_access_area_t area)
{
    int ret = 0;
    rvm_hal_mmc_access_area_t def_area;

    BLOCKDEV_VAILD(dev);

    if (!BLOCKDEV_DRIVER(dev)->current_area || !BLOCKDEV_DRIVER(dev)->select_area)
        return 0;

    device_lock(dev);
    ret = BLOCKDEV_DRIVER(dev)->current_area(dev, &def_area);
    if (ret == 0) {
        if (def_area != area) {
            ret = BLOCKDEV_DRIVER(dev)->select_area(dev, area);
        }
    }
    device_unlock(dev);
    return ret;
}
#endif

int rvm_hal_blockdev_get_info(rvm_dev_t *dev, rvm_hal_blockdev_info_t *info)
{
    int ret;

    BLOCKDEV_VAILD(dev);

    device_lock(dev);
    ret = BLOCKDEV_DRIVER(dev)->get_info(dev, info);
    device_unlock(dev);

    return ret;
}
