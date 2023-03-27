/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <aos/kernel.h>
#include <yoc/netmgr.h>
#include <drv/eth_mac.h>
#include <ulog/ulog.h>
#include <uservice/uservice.h>

#include "lwip/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/netifapi.h"

#include "eth_internal.h"

#define TAG         "eth_drv"
#define PHY_OFFLINE 0
#define PHY_ONLINE  1

#ifndef IP_ADDR0
#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 1
#define IP_ADDR3 3
#endif
/*NETMASK*/
#ifndef NETMASK_ADDR0
#define NETMASK_ADDR0 255
#define NETMASK_ADDR1 255
#define NETMASK_ADDR2 255
#define NETMASK_ADDR3 0
#endif
/*Gateway Address*/
#ifndef GW_ADDR0
#define GW_ADDR0 192
#define GW_ADDR1 168
#define GW_ADDR2 1
#define GW_ADDR3 1
#endif

#define GMAC_BUF_LEN (1500 + 20)

static struct pbuf *low_level_input(struct netif *netif);

static uint8_t          SendDataBuf[GMAC_BUF_LEN];
static uint8_t          RecvDataBuf[GMAC_BUF_LEN];
static eth_mac_handle_t g_mac_handle;
static eth_phy_handle_t g_phy_handle;
static uint8_t          g_phy_online_stat = PHY_OFFLINE;
static uint8_t          g_mac_addr[6]     = { 0xf2, 0x42, 0x9f, 0xa5, 0x0a, 0x72 };
static aos_sem_t        eth_rx_sem;
struct netif            eth_netif;

static void eth_mac_signal_event(uint32_t idx, uint32_t event)
{
    if (idx == 0) {
        if (event & CSI_ETH_MAC_EVENT_RX_FRAME) {
            aos_sem_signal(&eth_rx_sem);
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

static void eth_send_packet(uint8_t *buf, uint32_t len, uint32_t flags)
{
    int32_t ret = 0;

    if ((buf == NULL) || (g_phy_online_stat == PHY_OFFLINE))
        return;
    ret = csi_eth_mac_send_frame(g_mac_handle, buf, len, flags);
    if (ret != 0) {
        LOGE(TAG, "eth_send_packet send failed");
    }

    return;
}

static int32_t eth_receive_packet(uint8_t *buf, uint32_t len)
{
    int32_t ac_len = 0;

    if ((buf == NULL) || (g_phy_online_stat == PHY_OFFLINE))
        return -1;
    ac_len = csi_eth_mac_read_frame(g_mac_handle, buf, len);

    return ac_len;
}

static int eth_mac_phy_enable(uint32_t enable)
{
    eth_mac_addr_t addr;
    int32_t        ret;
    static int     init_flag;

    if ((init_flag == 0) && enable) {
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
        init_flag = 1;
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

static void _eth_phy_update_link_task(void *arg)
{
    eth_link_state_t curr_link_state = ETH_LINK_DOWN;
    eth_link_state_t pre_link_state  = ETH_LINK_DOWN;

    if (!g_phy_handle)
        return;

    while (1) {
        curr_link_state = csi_eth_phy_get_linkstate(g_phy_handle);
        if (pre_link_state != curr_link_state) {
            if (curr_link_state == ETH_LINK_UP) {
                /* enable mac and phy */
                eth_mac_phy_enable(1);
                g_phy_online_stat = PHY_ONLINE;
                netif_set_link_up(&eth_netif);
                event_publish(EVENT_ETH_LINK_UP, NULL);
            } else {
                /* disable mac and phy */
                eth_mac_phy_enable(0);
                g_phy_online_stat = PHY_OFFLINE;
                netif_set_link_down(&eth_netif);
                event_publish(EVENT_ETH_LINK_DOWN, NULL);
            }
            pre_link_state = curr_link_state;
        }
        aos_msleep(400);
    }
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */

static int ethernetif_input(struct netif *netif)
{
    struct pbuf *p;

    /* move received packet into a new pbuf */
    p = low_level_input(netif);
    /* if no packet could be read, silently ignore this */
    if (p != NULL) {
        /* pass all packets to ethernet_input, which decides what packets it supports */
        if (netif->input(p, netif) != ERR_OK) {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
        return 1;
    }
    return 0;
}

static void eth_rx(void *arg)
{
    int ret;

    while (1) {
        aos_sem_wait(&eth_rx_sem, AOS_WAIT_FOREVER);
        while (1) {
            ret = ethernetif_input(&eth_netif);
            if (ret == 0) {
                break;
            }
        }
    }
    aos_task_exit(0);
}

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
    int     ret    = 0;
    uint8_t mac[6] = { 0xf2, 0x42, 0x9f, 0xa5, 0x0a, 0x72 };

    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* get MAC hardware address */
    eth_get_macaddr(mac);
    memcpy(netif->hwaddr, mac, ETHARP_HWADDR_LEN);

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

#if LWIP_IPV6 && LWIP_IPV6_MLD
    /*
     * For hardware/netifs that implement MAC filtering.
     * All-nodes link-local is handled by default, so we must let the hardware know
     * to allow multicast packets in.
     * Should set mld_mac_filter previously. */
    if (netif->mld_mac_filter != NULL) {
        ip6_addr_t ip6_allnodes_ll;
        ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
        netif->mld_mac_filter(netif, &ip6_allnodes_ll, NETIF_ADD_MAC_FILTER);
    }
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

    /* Do whatever else is needed to initialize interface. */
    aos_task_t eth_rec_handle;
    aos_task_t eth_uplink_handle;

    aos_sem_new(&eth_rx_sem, 0);

    ret = aos_task_new_ext(&eth_rec_handle, "eth_recv_task", eth_rx, NULL, 8192, 37);
    if (0 != ret) {
        LOGE(TAG, "eth_recv_task create fail!\n");
    }

    ret = aos_task_new_ext(&eth_uplink_handle, "eth_uplink_task", _eth_phy_update_link_task, NULL, 4096, 32);
    if (0 != ret) {
        LOGE(TAG, "eth_uplink_task create fail!\n");
    }
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    uint32_t     i = 0;

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    for (q = p; q != NULL; q = q->next) {
        /* Send the data from the pbuf to the interface, one pbuf at a
           time. The size of the data in each pbuf is kept in the ->len
           variable. */
        // send data from(q->payload, q->len);
        MEMCPY((uint8_t *)&SendDataBuf[i], (uint8_t *)q->payload, q->len);
        i = i + q->len;
        if ((i > GMAC_BUF_LEN) || (i > p->tot_len)) {
            LWIP_DEBUGF(NETIF_DEBUG, ("low_level_output: error, len=%" U32_F ",tot_len=%" U32_F "\n\t", i, p->tot_len));
            return ERR_BUF;
        }
    }
    if (i == p->tot_len) {

        eth_send_packet(SendDataBuf, i, 0);

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.xmit);

        return ERR_OK;
    } else {
        LWIP_PLATFORM_DIAG(("send_packet: length mismatch, slen=%" U32_F ",tot_len=%" U32_F "\n\t", i, p->tot_len));
        return ERR_BUF;
    }
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *low_level_input(struct netif *netif)
{
    struct pbuf *p, *q;
    int32_t      len;
    uint32_t     i = 0;

    /* Obtain the size of the packet and put it into the "len"
       variable. */
    len = eth_receive_packet(RecvDataBuf, GMAC_BUF_LEN);

    if ((len <= 0) || (len > GMAC_BUF_LEN)) {
        // LWIP_DEBUGF(NETIF_DEBUG, ("low_level_input: len error. len: %d\n", len));
        return NULL;
    }
#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

    if (p != NULL) {

#if ETH_PAD_SIZE
        pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

        /* We iterate over the pbuf chain until we have read the entire
         * packet into the pbuf. */
        for (q = p; q != NULL; q = q->next) {
            /* Read enough bytes to fill this pbuf in the chain. The
             * available data in the pbuf is given by the q->len
             * variable.
             * This does not necessarily have to be a memcpy, you can also preallocate
             * pbufs for a DMA-enabled MAC and after receiving truncate it to the
             * actually received size. In this case, ensure the tot_len member of the
             * pbuf is the sum of the chained pbuf len members.
             */
            memcpy((u8_t *)q->payload, (u8_t *)&RecvDataBuf[i], q->len);
            i = i + q->len;
            // read data into(q->payload, q->len);
        }

        if ((i != p->tot_len) || (i > len)) {
            return NULL;
        }
        // acknowledge that packet has been read();

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    } else {
        // drop packet();
        LWIP_DEBUGF(NETIF_DEBUG, ("drop packet.\n"));
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
    }

    return p;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "T-head";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->output = etharp_output;
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    etharp_init();
    netif_set_flags(netif, NETIF_FLAG_IGMP);

    return ERR_OK;
}

static void eth_lwip_init()
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    int8_t    idx = 0;
    /* Create tcp_ip stack thread */
    tcpip_init(NULL, NULL);

    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    eth_netif.name[0] = 'e';
    eth_netif.name[1] = '0' + idx;
    netif_add(&eth_netif, &ipaddr, &netmask, &gw, NULL, ethif_init, tcpip_input);

    /*  Registers the default network interface. */
    netif_set_default(&eth_netif);
    netif_set_up(&eth_netif);
}

int eth_init(void)
{
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

    eth_set_macaddr(g_mac_addr);

    eth_lwip_init();

    LOGI(TAG, "PHY MAC init done");

    return 0;
}

int32_t eth_get_macaddr(uint8_t *mac)
{
    csi_eth_mac_get_macaddr(g_mac_handle, (eth_mac_addr_t *)mac);
    return 0;
}

int32_t eth_set_macaddr(const uint8_t *mac)
{
    eth_mac_addr_t addr;
    memcpy(addr.b, mac, 6);
    csi_eth_mac_set_macaddr(g_mac_handle, &addr);
    return 0;
}
