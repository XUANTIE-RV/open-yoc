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

/**
 * @brief net manage init for eth
 * @return NULL on error
 */
netmgr_hdl_t netmgr_dev_eth_init();

/**
 * @brief net manage init for gprs
 * @return NULL on error
 */
netmgr_hdl_t netmgr_dev_gprs_init();

/**
 * @brief net manage init for wifi
 * @return NULL on error
 */
netmgr_hdl_t netmgr_dev_wifi_init();

/**
 * @brief net manage init for nbiot
 * @return NULL on error
 */
netmgr_hdl_t netmgr_dev_nbiot_init();

/**
 * @brief deinit the net manage handle
 * @param  [in] hdl
 * @return
 */
void netmgr_dev_wifi_deinit(netmgr_hdl_t hdl);

/**
 * @brief  get the handle of netmgr by network name
 * @param  [in] name : wifi/eth/gprs/nbiot
 * @return NULL on error
 */
netmgr_hdl_t netmgr_get_handle(const char *name);

/**
 * @brief  get aos_dev by the netmgr hdl
 * @param  [in] netmgr_hdl_t
 * @return NULL on error
 */
aos_dev_t *netmgr_get_dev(netmgr_hdl_t);

/**
 * @brief  net manage config for wifi device
 * @param  [in] hdl
 * @param  [in] ssid
 * @param  [in] ssid_length
 * @param  [in] psk
 * @param  [in] psk_length
 * @return 0 on success
 */
int netmgr_config_wifi(netmgr_hdl_t hdl, char *ssid, uint8_t ssid_length, char *psk, uint8_t psk_length);

/**
 * @brief  net manage config for gprs device(developed not yet, reserved)
 * @param  [in] hdl
 * @param  [in] mode
 * @return 0 on success
 */
int netmgr_config_gprs(netmgr_hdl_t hdl, int mode);

/**
 * @brief  net manage config for wifi device
 * @param  [in] hdl
 * @param  [in] duplex
 * @param  [in] speed
 * @return 0 on success
 */
int netmgr_config_eth(netmgr_hdl_t hdl, eth_mac_duplex_e duplex, eth_mac_speed_e speed);

/**
 * @brief  netmgr config the ip or enable dhcp
 * @param  [in] hdl
 * @param  [in] dhcp_en : if enable, dhcp priority. otherwise use the static ip
 * @param  [in] ipaddr
 * @param  [in] netmask
 * @param  [in] gw
 * @return 0 on success
 */
int netmgr_ipconfig(netmgr_hdl_t hdl, int dhcp_en, char *ipaddr, char *netmask, char *gw);

/**
 * @brief  netmgr start provision, async.
 * if provison ok, EVENT_NETMGR_GOT_IP post, otherwise EVENT_NETMGR_NET_DISCON
 * @param  [in] hdl
 * @return 0/-1
 */
int netmgr_start(netmgr_hdl_t hdl);

/**
 * @brief  reset the netmgr, and re-provision after sec
 * @param  [in] hdl
 * @param  [in] sec : 0s means re-provision immediatly
 * @return 0/-1
 */
int netmgr_reset(netmgr_hdl_t hdl, uint32_t sec);

/**
 * @brief  stop netmgr(sync), will call the unprovison callback
 * @param  [in] hdl
 * @return 0/-1
 */
int netmgr_stop(netmgr_hdl_t hdl);

/**
 * @brief  got ip whether
 * @param  [in] hdl
 * @return 0/1
 */
int netmgr_is_gotip(netmgr_hdl_t hdl);

/**
 * @brief  link layer is up whether
 * @param  [in] hdl
 * @return 0/1
 */
int netmgr_is_linkup(netmgr_hdl_t hdl);

/**
 * @brief  dump network info to stdout(ip/gateway/netmask, etc)
 * @param  [in] hdl
 * @return 0/-1
 */
int netmgr_get_info(netmgr_hdl_t hdl);

#ifdef __cplusplus
}
#endif

#endif

