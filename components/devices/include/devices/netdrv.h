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

#ifndef DRIVER_NETDRV_H
#define DRIVER_NETDRV_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>

#include <uservice/event.h>
#include <devices/device.h>

#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif


#define EVENT_NET_GOT_IP        0x100

#define EVENT_WIFI_LINK_DOWN    0x101
#define EVENT_WIFI_LINK_UP      0x102
#define EVENT_WIFI_EXCEPTION    0x103
#define EVENT_WIFI_SMARTCFG     0x105

#define EVENT_ETH_LINK_UP       0x111
#define EVENT_ETH_LINK_DOWN     0x112
#define EVENT_ETH_EXCEPTION     0x113

//nbiot linkup&gotip todo
#define EVENT_NBIOT_LINK_UP     0x122
#define EVENT_NBIOT_LINK_DOWN   0x123

#define EVENT_GPRS_LINK_UP      0x132
#define EVENT_GPRS_LINK_DOWN    0x133

/* EVENT_NETMGR_NET_DISCON(LINK DOWN) REASON */
#define NET_DISCON_REASON_NORMAL         255
#define NET_DISCON_REASON_WIFI_TIMEOUT   1
#define NET_DISCON_REASON_WIFI_PSK_ERR   2
#define NET_DISCON_REASON_WIFI_NOEXIST   3
#define NET_DISCON_REASON_ERROR          4
#define NET_DISCON_REASON_DHCP_ERROR     5

#define EVENT_NETWORK_RESTART (140)


enum {
    NETDEV_TYPE_ETH,
    NETDEV_TYPE_WIFI,
    NETDEV_TYPE_GPRS,
    NETDEV_TYPE_NBIOT,
};

enum {
    NETDEV_EVENT_RECV_DATA = 0,
};

/** @brief net recv event callback */
typedef void (*rvm_hal_net_event)(rvm_dev_t *dev, int event_id, void *priv);

/**
    These APIs define Ethernet level operation
*/
int rvm_hal_net_get_mac_addr(rvm_dev_t *dev, uint8_t *mac);
int rvm_hal_net_set_mac_addr(rvm_dev_t *dev, const uint8_t *mac);
int rvm_hal_net_get_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num);
int rvm_hal_net_set_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num);
int rvm_hal_net_set_link_up(rvm_dev_t *dev);
int rvm_hal_net_set_link_down(rvm_dev_t *dev);
int rvm_hal_net_set_hostname(rvm_dev_t *dev, const char *name);
const char *rvm_hal_net_get_hostname(rvm_dev_t *dev);
int rvm_hal_net_start_dhcp(rvm_dev_t *dev);
int rvm_hal_net_stop_dhcp(rvm_dev_t *dev);
int rvm_hal_net_set_ipaddr(rvm_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw);
int rvm_hal_net_get_ipaddr(rvm_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw);
int rvm_hal_net_ping(rvm_dev_t *dev, int type, char *remote_ip);
int rvm_hal_net_subscribe(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param);
int rvm_hal_net_unsubscribe(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param);


/**
  \brief       alloc a buffer for send or recv data
  \param[in]   dev        Pointer to device object.
  \param[in]   size       buffer size
  \return      NULL for failed or buffer pointer
*/
void* rvm_hal_net_alloc_buf(rvm_dev_t *dev, size_t size);

/**
  \brief       send data, should use buff alloced from rvm_hal_net_alloc_buf
  \param[in]   dev        Pointer to device object.
  \param[in]   buff       data buff
  \param[in]   len        buffer size
  \return      0 on success, else on fail.
*/
int rvm_hal_net_send_data(rvm_dev_t *dev, void* buff, size_t len);

/**
  \brief       recv data, should use buff alloced from rvm_hal_net_alloc_buf
  \param[in]   dev        Pointer to device object.
  \param[in]   buff       data buff
  \param[in]   len        buffer size
  \param[in]   timeout_ms timeout in ms
  \return      0 on success, else on fail.
*/
int rvm_hal_net_recv_data(rvm_dev_t *dev, void* buff, size_t len, int timeout_ms);

/**
  \brief       set event callback
  \param[in]   dev      Pointer to device object.
  \param[in]   event_cb event callback
  \param[in]   priv     private data for user
  \return      0 on success, else on fail.
*/
int rvm_hal_net_set_event(rvm_dev_t *dev, rvm_hal_net_event event_cb, void *priv);

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_netdrv.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
