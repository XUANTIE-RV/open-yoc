/*
* Copyright (C) Cvitek Co., Ltd. 2019-2022. All rights reserved.
*/

/******************************************************************************
 * @file     cvi_mac_phy.c
 * @brief    ethernet init setting
 * @version  V1.0
 * @date     21 July 2022
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <drv/eth_mac.h>
#include <ulog/ulog.h>
#include <mmio.h>

#include "aos/kernel.h"
#include "cvi_mac_phy.h"

#define TAG "ethernet"
#define PHY_OFFLINE 0
#define PHY_ONLINE 1

static eth_mac_handle_t g_mac_handle;
static eth_phy_handle_t g_phy_handle;
static uint8_t g_phy_online_stat = PHY_OFFLINE;
static uint8_t g_mac_phy_init_finish = 0;

static uint8_t g_mac_addr[6] = {0xf2, 0x42, 0x9f, 0xa5, 0x0a, 0x72};
/*
#define CSI_ETH_MAC_EVENT_RX_FRAME      (1UL << 0)  ///< Frame Received
#define CSI_ETH_MAC_EVENT_TX_FRAME      (1UL << 1)  ///< Frame Transmitted
#define CSI_ETH_MAC_EVENT_WAKEUP        (1UL << 2)  ///< Wake-up (on Magic Packet)
#define CSI_ETH_MAC_EVENT_TIMER_ALCSI   (1UL << 3)  ///< Timer Alarm
#define CSI_ETH_MAC_EVENT_LINK_CHANGE
*/
extern aos_sem_t eth_rx_sem;
static void eth_mac_signal_event(uint32_t idx, uint32_t event)
{
    if(idx == 0) {
        if(event & CSI_ETH_MAC_EVENT_RX_FRAME) {
            if (eth_rx_sem) {
                aos_sem_signal(&eth_rx_sem);
            }
        }
    }
}

static int32_t fn_phy_read(uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{
    return csi_eth_mac_phy_read(g_mac_handle, phy_addr, reg_addr, data);
}

static int32_t fn_phy_write(uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
{
    return csi_eth_mac_phy_write(g_mac_handle, phy_addr, reg_addr, data);
}

// int32_t csi_eth_mac_start(void)
// {
//     int32_t ret;

//     /* adjust mac link parameter */
//     ret = csi_eth_mac_control(g_mac_handle, DRV_ETH_MAC_ADJUST_LINK, (uint32_t)g_phy_handle);

//     if (ret != 0) {
//         LOG_E("Failed to adjust link");
//         return -1;
//     }

//     return 0;
// }

static int csi_eth_mac_phy_enable(uint32_t enable)
{
    eth_mac_addr_t addr;
    int32_t ret;

    if ((g_mac_phy_init_finish == 0) && enable) {
        /* startup mac */
        ret = csi_eth_mac_control(g_mac_handle, CSI_ETH_MAC_CONFIGURE, 1);

        if (ret != 0) {
            LOGE(TAG, "Failed to control mac");
            return -1;
        }

        /* Start up the PHY */
        ret = csi_eth_phy_power_control(g_phy_handle, CSI_ETH_POWER_FULL);

        if (ret != 0) {
            LOGE(TAG, "Failed to control phy, ret:0x%d", ret);
            return -1;
        }
    }

    /* enable mac TX/RX */
    ret = csi_eth_mac_control(g_mac_handle, CSI_ETH_MAC_CONTROL_TX, enable ? 1 : 0);
    if (ret != 0) {
        LOGE(TAG, "Failed to enable mac TX");
    }
    ret = csi_eth_mac_control(g_mac_handle, CSI_ETH_MAC_CONTROL_RX, enable ? 1 : 0);
    if (ret != 0) {
        LOGE(TAG, "Failed to enable mac RX");
    }

    /* set mac address */
    memcpy(addr.b, g_mac_addr, sizeof(g_mac_addr));
    ret = csi_eth_mac_set_macaddr(g_mac_handle, &addr);
    if (ret != 0) {
        LOGE(TAG, "Failed to set mac address");
        return -1;
    }

    /* adjust mac link parameter */
    ret = csi_eth_mac_control(g_mac_handle, DRV_ETH_MAC_ADJUST_LINK, 1);
    if (ret != 0) {
        LOGE(TAG, "Failed to adjust link");
        return -1;
    }

    return 0;
}

static void cvi_ephy_id_init(void)
{
	// set rg_ephy_apb_rw_sel 0x0804@[0]=1/APB by using APB interface
	mmio_write_32(0x03009804, 0x0001);

	// Release 0x0800[0]=0/shutdown
	mmio_write_32(0x03009800, 0x0900);

	// Release 0x0800[2]=1/dig_rst_n, Let mii_reg can be accessabile
	mmio_write_32(0x03009800, 0x0904);

	// PHY_ID
	mmio_write_32(0x03009008, 0x0043);
	mmio_write_32(0x0300900c, 0x5649);

	// switch to MDIO control by ETH_MAC
	mmio_write_32(0x03009804, 0x0000);
}


int csi_eth_mac_phy_init(void)
{
    /* init phy id */
    cvi_ephy_id_init();

    /* initialize MAC & PHY */
    g_mac_handle = csi_eth_mac_initialize(0, (eth_event_cb_t)eth_mac_signal_event);
    if (g_mac_handle == NULL) {
        LOGE(TAG, "Failed to init mac");
        return -1;
    }
    g_phy_handle = csi_eth_phy_initialize(fn_phy_read, fn_phy_write);
    if (g_phy_handle == NULL) {
        LOGE(TAG, "Failed to init phy");
        return -1;
    }

    csi_eth_mac_connect_phy(g_mac_handle, g_phy_handle);

    if (csi_eth_mac_phy_enable(1)) {
        LOGE(TAG, "PHY MAC init fail");
        return -1;
    }

    g_phy_online_stat = PHY_ONLINE;
    g_mac_phy_init_finish = 1;
    LOGI(TAG, "PHY MAC init done");
    return 0;
}

void csi_eth_phy_update_link_task(void *arg)
{
    eth_link_state_t curr_link_state;
    eth_link_state_t pre_link_state;

    if(!g_phy_handle)
        return;

    pre_link_state = csi_eth_phy_get_linkstate(g_phy_handle);

    while(1) {
        curr_link_state = csi_eth_phy_get_linkstate(g_phy_handle);
        if (pre_link_state != curr_link_state) {
            if (curr_link_state == ETH_LINK_UP) {
                /* enable mac and phy */
                csi_eth_mac_phy_enable(1);
                g_phy_online_stat = PHY_ONLINE;
            } else {
                /* disable mac and phy */
                csi_eth_mac_phy_enable(0);
                g_phy_online_stat = PHY_OFFLINE;
            }
            pre_link_state = curr_link_state;
        }
        aos_msleep(400);
    }
}

void csi_send_packet(uint8_t *buf, uint32_t len, uint32_t flags)
{
    int32_t ret =0;

    if((buf == NULL) || (g_phy_online_stat == PHY_OFFLINE)) return;
    ret = csi_eth_mac_send_frame(g_mac_handle, buf, len, flags);
    if(ret != 0) {
        LOGE(TAG, "csi_send_packet send failed");
    }

    return;
}
int32_t csi_receive_packet(uint8_t *buf, uint32_t len)
{
    int32_t ac_len = 0;

    if((buf == NULL) || (g_phy_online_stat == PHY_OFFLINE)) return -1;
    ac_len = csi_eth_mac_read_frame(g_mac_handle, buf, len);

    return ac_len;
}

int32_t csi_get_macaddr(uint8_t *mac)
{
    csi_eth_mac_get_macaddr(g_mac_handle, (eth_mac_addr_t *)mac);
    return 0;
}

int32_t csi_set_macaddr(uint8_t *mac)
{
    eth_mac_addr_t addr;
    memcpy(addr.b, mac, 6);
    csi_eth_mac_set_macaddr(g_mac_handle, &addr);
    return 0;
}

/* loopack test Ethernet code */
#if 1
static uint8_t eth_tx_test_data[1512] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x11, 0x22, 0x60, 0x94, 0x2d, 0x08, 0x06,
    0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01,
    0x00, 0x11, 0x22, 0x60, 0x94, 0x2d, 0xc0, 0xa8,
    0x3b, 0xc8, 0x50, 0x10, 0xfa, 0xf0, 0xc0, 0xa8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static uint8_t eth_rx_test_data[1512] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static void rcv_task(void* arg)
{
    int32_t recv_len;
    while(1) {
        recv_len = csi_receive_packet(eth_rx_test_data, sizeof(eth_rx_test_data));
        if(recv_len > 0) {
            LOGI(TAG, "rcv %d data: ", recv_len);
        }
        aos_msleep(1);
    }

}
#include "aos/kernel.h"
void loopback_test(void)
{
    aos_task_t rcv_handle;
	csi_eth_mac_phy_init();
    aos_task_new_ext(&rcv_handle, "test_rcv", rcv_task, NULL, 8192, 26);
    uint8_t count = 0;
	while (1) {
        count++;
        memset(eth_tx_test_data, count, sizeof(eth_tx_test_data));
		csi_send_packet(eth_tx_test_data, sizeof(eth_tx_test_data), 0);
		aos_msleep(1000);
	}
}
#endif
