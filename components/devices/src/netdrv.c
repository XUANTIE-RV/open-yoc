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

#if defined(CONFIG_SAL) || defined(CONFIG_TCPIP)
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/impl/net_impl.h>

#define NET_DRIVER(dev)  ((net_ops_t *)(((netdev_driver_t *)dev->drv)->net_ops))


int rvm_hal_net_get_mac_addr(rvm_dev_t *dev, uint8_t *mac)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->get_mac_addr(dev, mac);
    device_unlock(dev);

    return ret;
}

int rvm_hal_net_set_mac_addr(rvm_dev_t *dev, const uint8_t *mac)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->set_mac_addr(dev, mac);
    device_unlock(dev);

    return ret;
}

int rvm_hal_net_get_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->get_dns_server(dev, ipaddr, num);
    device_unlock(dev);

    return ret;
}

int rvm_hal_net_set_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->set_dns_server(dev, ipaddr, num);
    device_unlock(dev);

    return ret;
}

int rvm_hal_net_set_hostname(rvm_dev_t *dev, const char *name)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->set_hostname(dev, name);
    device_unlock(dev);

    return ret;
}

const char *rvm_hal_net_get_hostname(rvm_dev_t *dev)
{
    const char* ret;

    if (dev == NULL) {
        return NULL;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->get_hostname(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_net_set_link_up(rvm_dev_t *dev)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->set_link_up(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_net_set_link_down(rvm_dev_t *dev)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    if (NET_DRIVER(dev)->set_link_down)
        ret = NET_DRIVER(dev)->set_link_down(dev);
    else
        ret = -1;
    device_unlock(dev);

    return ret;
}

int rvm_hal_net_start_dhcp(rvm_dev_t *dev)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    if (NET_DRIVER(dev)->start_dhcp)
        ret = NET_DRIVER(dev)->start_dhcp(dev);
    else
        ret = -1;
    device_unlock(dev);

    return ret;

}
int rvm_hal_net_stop_dhcp(rvm_dev_t *dev)
{
    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->stop_dhcp(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_net_set_ipaddr(rvm_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw)
{

    int ret;

    if (dev == NULL) {
        return -1;
    }

    device_lock(dev);
    ret = NET_DRIVER(dev)->set_ipaddr(dev, ipaddr, netmask, gw);
    device_unlock(dev);

    return ret;

}


int rvm_hal_net_get_ipaddr(rvm_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw)
{
    int ret;

    device_lock(dev);
    ret = NET_DRIVER(dev)->get_ipaddr(dev, ipaddr, netmask, gw);
    device_unlock(dev);

    return ret;
}



int rvm_hal_net_ping(rvm_dev_t *dev, int type, char *remote_ip)
{
    int ret;

    device_lock(dev);
    ret = NET_DRIVER(dev)->ping(dev, type, remote_ip);
    device_unlock(dev);

    return ret;
}


int rvm_hal_net_subscribe(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    int ret;

    device_lock(dev);
    ret = NET_DRIVER(dev)->subscribe(dev, event, cb, param);
    device_unlock(dev);

    return ret;
}

int rvm_hal_net_unsubscribe(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    int ret;

    device_lock(dev);
    ret = NET_DRIVER(dev)->unsubscribe(dev, event, cb, param);
    device_unlock(dev);

    return ret;
}

void* rvm_hal_net_alloc_buf(rvm_dev_t *dev, size_t size)
{
    void* ret;

    device_lock(dev);
    ret = NET_DRIVER(dev)->alloc_buf(dev, size);
    device_unlock(dev);

    return ret;
}

int rvm_hal_net_send_data(rvm_dev_t *dev, void* buff, size_t len)
{
    int ret;

    device_lock(dev);
    ret = NET_DRIVER(dev)->send_data(dev, buff, len);
    device_unlock(dev);

    return ret;
}

int rvm_hal_net_recv_data(rvm_dev_t *dev, void* buff, size_t len, int timeout_ms)
{
    int ret;

    device_lock(dev);
    ret = NET_DRIVER(dev)->recv_data(dev, buff, len, timeout_ms);
    device_unlock(dev);

    return ret;
}

int rvm_hal_net_set_event(rvm_dev_t *dev, rvm_hal_net_event event_cb, void *priv)
{
    int ret;

    device_lock(dev);
    ret = NET_DRIVER(dev)->set_event(dev, event_cb, priv);
    device_unlock(dev);

    return ret;
}

#endif
