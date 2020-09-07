/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_NETMGR_IMPL_H
#define HAL_NETMGR_IMPL_H

#include <stdint.h>
#include <sys/socket.h>

#include <devices/driver.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct netif_driver {
    driver_t drv;
    void (*get_mac_addr)(netif_driver_t *netif, uint8_t *mac);
    void (*set_mac_addr)(netif_driver_t *netif, const uint8_t *mac);

    void (*set_link_up)(netif_driver_t *netif);
    void (*set_link_down)(netif_driver_t *netif);

    void (*set_ipaddr)(netif_driver_t *netif, const ip4_addr_t *ipaddr);
    void (*set_netmask)(netif_driver_t *netif, const ip4_addr_t *netmask);
    void (*set_gw)(netif_driver_t *netif, const ip4_addr_t *gw);
} netif_driver_t;


typedef struct netif_at_driver {
    netif_driver_t netif;
    sal_op_t sal_op;

} netif_at_driver_t;

#ifdef __cplusplus
}
#endif

#endif