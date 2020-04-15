/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_NETMGR_IMPL_H
#define HAL_NETMGR_IMPL_H

#include <stdint.h>

#include <yoc/uservice.h>
#include <devices/driver.h>
#include <lwip/ip_addr.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct net_ops {
    int (*get_mac_addr)(aos_dev_t *dev, uint8_t *mac);
    int (*set_mac_addr)(aos_dev_t *dev, const uint8_t *mac);

    int (*get_dns_server)(aos_dev_t *dev, ip_addr_t ipaddr[], uint32_t num);
    int (*set_dns_server)(aos_dev_t *dev, ip_addr_t ipaddr[], uint32_t num);

    int (*set_hostname)(aos_dev_t *dev, const char *name);
    const char* (*get_hostname)(aos_dev_t *dev);

    int (*set_link_up)(aos_dev_t *dev);
    int (*set_link_down)(aos_dev_t *dev);

    int (*start_dhcp)(aos_dev_t *dev);
    int (*stop_dhcp)(aos_dev_t *dev);
    int (*set_ipaddr)(aos_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw);
    int (*get_ipaddr)(aos_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw);
    int (*ping)(aos_dev_t *dev, int type, char *remote_ip);

    int (*subscribe)(aos_dev_t *dev, uint32_t event, event_callback_t cb, void *param);
    int (*unsubscribe)(aos_dev_t *dev, uint32_t event, event_callback_t cb, void *param);
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
