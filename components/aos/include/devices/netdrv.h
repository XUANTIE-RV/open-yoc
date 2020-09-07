/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DRIVER_NETDRV_H
#define DRIVER_NETDRV_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>

#include <yoc/event.h>
#include <devices/device.h>
#include <devices/driver.h>

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



/**
    These APIs define Ethernet level operation
*/
int hal_net_get_mac_addr(aos_dev_t *dev, uint8_t *mac);
int hal_net_set_mac_addr(aos_dev_t *dev, const uint8_t *mac);
int hal_net_get_dns_server(aos_dev_t *dev, ip_addr_t ipaddr[], uint32_t num);
int hal_net_set_dns_server(aos_dev_t *dev, ip_addr_t ipaddr[], uint32_t num);
int hal_net_set_link_up(aos_dev_t *dev);
int hal_net_set_link_down(aos_dev_t *dev);
int hal_net_set_hostname(aos_dev_t *dev, const char *name);
const char *hal_net_get_hostname(aos_dev_t *dev);
int hal_net_start_dhcp(aos_dev_t *dev);
int hal_net_stop_dhcp(aos_dev_t *dev);
int hal_net_set_ipaddr(aos_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw);
int hal_net_get_ipaddr(aos_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw);
int hal_net_ping(aos_dev_t *dev, int type, char *remote_ip);
int hal_net_subscribe(aos_dev_t *dev, uint32_t event, event_callback_t cb, void *param);
int hal_net_unsubscribe(aos_dev_t *dev, uint32_t event, event_callback_t cb, void *param);


#ifdef __cplusplus
}
#endif

#endif
