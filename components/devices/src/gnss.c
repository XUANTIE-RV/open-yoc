/*
 * Copyright (C) 2019 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <dev_internal.h>
#include <devices/impl/gnss_impl.h>

#define GNSS_DRIVER(dev)  ((gnss_driver_t*)(dev->drv))
#define GNSS_VAILD(dev) do { \
    if (device_valid(dev, "gnss") != 0) \
        return -1; \
} while(0)

int rvm_hal_gnss_getinfo(rvm_dev_t *dev, rvm_hal_gnss_info_t *info)
{
    int ret;

    GNSS_VAILD(dev);

    device_lock(dev);
    ret = GNSS_DRIVER(dev)->get_info(dev, info);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}
