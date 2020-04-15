/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_NET_MGR_H
#define YOC_NET_MGR_H

#include <yoc/event.h>
#include <sys/socket.h>
#include <devices/netdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ETH_MAC_SPEED_10M,
    ETH_MAC_SPEED_100M,
    ETH_MAC_SPEED_1000M,
} eth_mac_speed_e;

typedef enum {
    ETH_MAC_DUPLEX_HALF,
    ETH_MAC_DUPLEX_FULL,
} eth_mac_duplex_e;

typedef void *netmgr_hdl_t;

/* netmgr init */
netmgr_hdl_t netmgr_dev_eth_init();
netmgr_hdl_t netmgr_dev_gprs_init();
netmgr_hdl_t netmgr_dev_wifi_init();
netmgr_hdl_t netmgr_dev_nbiot_init();
void netmgr_dev_wifi_deinit(netmgr_hdl_t hdl);

netmgr_hdl_t netmgr_get_handle(const char *name);
aos_dev_t *netmgr_get_dev(netmgr_hdl_t);

/* netmgr config */
int netmgr_config_wifi(netmgr_hdl_t hdl, char *ssid, uint8_t ssid_length, char *psk, uint8_t psk_length);
int netmgr_config_gprs(netmgr_hdl_t hdl, int mode);
int netmgr_config_eth(netmgr_hdl_t hdl, eth_mac_duplex_e duplex, eth_mac_speed_e speed);
int netmgr_ipconfig(netmgr_hdl_t hdl, int dhcp_en, char *ipaddr, char *netmask, char *gw);

int netmgr_start(netmgr_hdl_t hdl);
int netmgr_reset(netmgr_hdl_t hdl, int sec);
int netmgr_stop(netmgr_hdl_t hdl);
int netmgr_is_gotip(netmgr_hdl_t hdl);
int netmgr_is_linkup(netmgr_hdl_t hdl);
int netmgr_get_info(netmgr_hdl_t hdl);

#ifdef __cplusplus
}
#endif

#endif

