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

#if defined(CONFIG_SAL)
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/impl/gprs_impl.h>

#define GPRS_DRIVER(dev)  ((gprs_driver_t *)(((netdev_driver_t *)dev->drv)->link_ops))
#define GPRS_VALID(dev) do { \
        if (device_valid(dev, "gprs") != 0) \
            return -1; \
    } while(0)

int rvm_hal_gprs_set_mode(rvm_dev_t *dev, rvm_hal_gprs_mode_t mode)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->set_mode(dev, mode);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_get_mode(rvm_dev_t *dev, rvm_hal_gprs_mode_t *mode)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_mode(dev, mode);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_reset(rvm_dev_t *dev)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->reset(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_start(rvm_dev_t *dev)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->start(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_stop(rvm_dev_t *dev)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->stop(dev);
    device_unlock(dev);

    return ret;
}

/*configuration*/
int rvm_hal_gprs_set_if_config(rvm_dev_t *dev, uint32_t baud, uint8_t flow_control)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->set_if_config(dev, baud, flow_control);
    device_unlock(dev);

    return ret;
}

/*connection*/
int rvm_hal_gprs_module_init_check(rvm_dev_t *dev)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->module_init_check(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_connect_to_gprs(rvm_dev_t *dev)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->connect_to_gprs(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_disconnect_from_gprs(rvm_dev_t *dev)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->disconnect_from_gprs(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_sms_send(rvm_dev_t *dev, char *sca, char *da, char *content)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->sms_send(dev, sca, da, content);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_sms_set_cb(rvm_dev_t *dev, recv_sms_cb cb)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->sms_set_cb(dev, cb);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_get_imsi(rvm_dev_t *dev, char *imsi)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_imsi(dev, imsi);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_get_imei(rvm_dev_t *dev, char *imei)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_imei(dev, imei);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_get_link_status(rvm_dev_t *dev, rvm_hal_gprs_status_link_t *link_status)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_link_status(dev, link_status);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_get_ipaddr(rvm_dev_t *dev, char ip[16])
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_ipaddr(dev, ip);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_get_csq(rvm_dev_t *dev, int *csq)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_csq(dev, csq);
    device_unlock(dev);

    return ret;
}

int rvm_hal_gprs_get_simcard_info(rvm_dev_t *dev, char ccid[21], int *insert)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_simcard_info(dev, ccid, insert);
    device_unlock(dev);

    return ret;
}

#endif

