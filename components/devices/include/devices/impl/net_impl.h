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

#ifndef HAL_NETMGR_IMPL_H
#define HAL_NETMGR_IMPL_H

#if defined(CONFIG_SAL) || defined(CONFIG_TCPIP)
#include <stdint.h>

#include <uservice/uservice.h>
#include <devices/netdrv.h>
#include <lwip/ip_addr.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct net_ops {
    int (*get_mac_addr)(rvm_dev_t *dev, uint8_t *mac);
    int (*set_mac_addr)(rvm_dev_t *dev, const uint8_t *mac);

    int (*get_dns_server)(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num);
    int (*set_dns_server)(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num);

    int (*set_hostname)(rvm_dev_t *dev, const char *name);
    const char* (*get_hostname)(rvm_dev_t *dev);

    int (*set_link_up)(rvm_dev_t *dev);
    int (*set_link_down)(rvm_dev_t *dev);

    int (*start_dhcp)(rvm_dev_t *dev);
    int (*stop_dhcp)(rvm_dev_t *dev);
    int (*set_ipaddr)(rvm_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw);
    int (*get_ipaddr)(rvm_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw);
    int (*ping)(rvm_dev_t *dev, int type, char *remote_ip);

    int (*subscribe)(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param);
    int (*unsubscribe)(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param);

    void* (*alloc_buf)(rvm_dev_t *dev, size_t size);
    int (*send_data)(rvm_dev_t *dev, void* buff, size_t len);
    int (*recv_data)(rvm_dev_t *dev, void* buff, size_t len, int timeout_ms);
    int (*set_event)(rvm_dev_t *dev, rvm_hal_net_event event_cb, void *priv);
} net_ops_t;

typedef struct netdev_driver {
    driver_t drv;
    net_ops_t *net_ops;
    int link_type;
    void *link_ops;
} netdev_driver_t;

#ifdef __cplusplus
}
#endif

#endif

#endif
