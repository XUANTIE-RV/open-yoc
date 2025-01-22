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
#include <dev_internal.h>
#include <devices/impl/hci_impl.h>
#include <devices/hci.h>

int rvm_hal_hci_send(rvm_dev_t *dev, void *data, uint32_t size)
{
    hci_driver_t *hci = (hci_driver_t *)dev->drv;
    int ret;

    device_lock(dev);
    ret = hci->send(dev, data, size);
    device_unlock(dev);

    return ret;
}

int rvm_hal_hci_set_event(rvm_dev_t *dev, hci_event_cb_t event, void *pirv)
{
    hci_driver_t *hci = (hci_driver_t *)dev->drv;
    int ret;

    device_lock(dev);
    ret = hci->set_event(dev, event, pirv);
    device_unlock(dev);

    return ret;
}

int rvm_hal_hci_recv(rvm_dev_t *dev, void* data, uint32_t size)
{
    hci_driver_t *hci = (hci_driver_t *)dev->drv;
    int ret;

    device_lock(dev);
    ret = hci->recv(dev, data, size);
    device_unlock(dev);

    return ret;
}

int rvm_hal_hci_start(rvm_dev_t *dev, hci_driver_send_cmd_t send_cmd)
{
    hci_driver_t *hci = (hci_driver_t *)dev->drv;
    int ret;

    // device_lock(dev);
    ret = hci->start(dev, send_cmd);
    // device_unlock(dev);

    return ret;
}
