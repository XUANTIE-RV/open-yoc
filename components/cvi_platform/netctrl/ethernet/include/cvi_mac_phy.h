/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __CVI_MAC_PHY_H__
#define __CVI_MAC_PHY_H__


int csi_eth_mac_phy_init(void);
void csi_eth_phy_update_link_task(void *arg);
void csi_send_packet(uint8_t *buf, uint32_t len, uint32_t flags);
int32_t csi_receive_packet(uint8_t *buf, uint32_t len);
int32_t csi_get_macaddr(uint8_t *mac);
int32_t csi_set_macaddr(uint8_t *mac);
/* loopack test Ethernet code */
void loopback_test(void);

#endif
