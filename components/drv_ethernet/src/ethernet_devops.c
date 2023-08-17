/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lwip/apps/dhcps.h>
#include <lwip/netifapi.h>
#include <lwip/dns.h>

#include <devices/netdrv.h>
#include <devices/ethernet.h>
#include <devices/impl/net_impl.h>
#include <devices/impl/ethernet_impl.h>

#include "eth_internal.h"

#if !defined(MIN)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define TAG "eth_drv"

typedef struct {
    rvm_dev_t device;

    void *priv;
} eth_dev_t;

extern struct netif eth_netif;

static rvm_dev_t *eth_dev_init(driver_t *drv, void *config, int id)
{
    rvm_dev_t *dev = rvm_hal_device_new(drv, sizeof(eth_dev_t), id);

    return dev;
}

#define eth_dev_uninit rvm_hal_device_free

static int eth_dev_open(rvm_dev_t *dev)
{

    return 0;
}

static int eth_dev_close(rvm_dev_t *dev)
{

    return 0;
}

/*****************************************
 * common netif driver interface
 ******************************************/
static int eth_set_mac_addr(rvm_dev_t *dev, const uint8_t *mac)
{
    eth_set_macaddr(mac);
    return 0;
}

static int eth_get_mac_addr(rvm_dev_t *dev, uint8_t *mac)
{
    eth_get_macaddr(mac);
    return 0;
}

static void net_status_callback(struct netif *netif)
{
    if (!ip_addr_isany(ip_2_ip4(&netif->ip_addr))) {
        event_publish(EVENT_NET_GOT_IP, NULL);
    }
}

static int eth_start_dhcp(rvm_dev_t *dev)
{
    struct netif *netif = &eth_netif;
    aos_check_return_einval(netif);

    if (!netif_is_link_up(netif)) {
        return -1;
    }

    netif_set_ipaddr(netif, NULL);
    netif_set_netmask(netif, NULL);
    netif_set_gw(netif, NULL);

    netif_set_status_callback(netif, net_status_callback);

    return netifapi_dhcp_start(netif);
}

static int eth_stop_dhcp(rvm_dev_t *dev)
{
    struct netif *netif = &eth_netif;
    aos_check_return_einval(netif);

    netifapi_dhcp_stop(netif);

    return 0;
}

static int eth_set_ipaddr(rvm_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw)
{
    return -1;
}

static int eth_get_ipaddr(rvm_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask_addr, ip_addr_t *gw_addr)
{
    struct netif *netif = &eth_netif;
    aos_check_return_einval(netif && ipaddr && netmask_addr && gw_addr);

    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(ipaddr), *netif_ip_addr4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(gw_addr), *netif_ip_gw4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(netmask_addr), *netif_ip_netmask4(netif));

    return 0;
}

static int eth_subscribe(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    if (cb) {
        event_subscribe(event, cb, param);
    }

    return 0;
}

static int eth_set_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    int n, i;

    n = MIN(num, DNS_MAX_SERVERS);

    for (i = 0; i < n; i++) {
        dns_setserver(i, &ipaddr[i]);
    }

    return n;
}

static int eth_get_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    int n, i;

    n = MIN(num, DNS_MAX_SERVERS);

    for (i = 0; i < n; i++) {
        if (!ip_addr_isany(dns_getserver(i))) {
            memcpy(&ipaddr[i], dns_getserver(i), sizeof(ip_addr_t));
        } else {
            return i;
        }
    }

    return n;
}

static int eth_set_hostname(rvm_dev_t *dev, const char *name)
{
#if LWIP_NETIF_HOSTNAME
    struct netif *netif = &eth_netif;
    netif_set_hostname(netif, name);
    return 0;
#else
    return -1;
#endif
}

static const char *eth_get_hostname(rvm_dev_t *dev)
{
#if LWIP_NETIF_HOSTNAME
    struct netif *netif = &eth_netif;
    return netif_get_hostname(netif);
#else
    return NULL;
#endif
}

/*****************************************
 * eth driver interface
 ******************************************/

static int eth_mac_control(rvm_dev_t *dev, eth_config_t *config)
{

    return 0;
}

static int eth_set_packet_filter(rvm_dev_t *dev, int type)
{

    return 0;
}

static int eth_start(rvm_dev_t *dev)
{

    return 0;
}

static int eth_stop(rvm_dev_t *dev)
{

    return 0;
}

static int eth_reset(rvm_dev_t *dev)
{

    return 0;
}

static net_ops_t eth_net_driver = {
    .get_mac_addr   = eth_get_mac_addr,
    .set_mac_addr   = eth_set_mac_addr,
    .set_dns_server = eth_set_dns_server,
    .get_dns_server = eth_get_dns_server,
    .set_hostname   = eth_set_hostname,
    .get_hostname   = eth_get_hostname,
    .start_dhcp     = eth_start_dhcp,
    .stop_dhcp      = eth_stop_dhcp,
    .set_ipaddr     = eth_set_ipaddr,
    .get_ipaddr     = eth_get_ipaddr,
    .subscribe      = eth_subscribe,
    .ping           = NULL,
};

static eth_driver_t eth_driver = {
    .mac_control       = eth_mac_control,
    .set_packet_filter = eth_set_packet_filter,
    .start             = eth_start,
    .stop              = eth_stop,
    .reset             = eth_reset,
};

static netdev_driver_t neteth_driver = {
    .drv = {
        .name   = "eth",
        .init   = eth_dev_init,
        .uninit = eth_dev_uninit,
        .open   = eth_dev_open,
        .close  = eth_dev_close,
    },
    .link_type = NETDEV_TYPE_ETH,
    .net_ops =  &eth_net_driver,
    .link_ops = &eth_driver,
};

void drv_eth_register()
{
    int ret;

    eth_init();

    // run eth_dev_init to create gprs_dev_t and bind this driver
    ret = rvm_driver_register(&neteth_driver.drv, NULL, 0);

    if (ret < 0) {
        LOGI(TAG, "ether device register error");
    }
}