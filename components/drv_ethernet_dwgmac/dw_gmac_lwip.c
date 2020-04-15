/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc_config.h>

#include <aos/log.h>

#include <string.h>
#include <lwip/opt.h>
#include <lwip/def.h>
#include <lwip/mem.h>
#include <lwip/pbuf.h>
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include <lwip/ethip6.h>
#include <lwip/etharp.h>
#include <lwip/sys.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>
#include <lwip/netifapi.h>
#include <lwip/api.h>
#include <lwip/dns.h>
#include <netif/ethernet.h>
#include <yoc/lpm.h>
#include <yoc/uservice.h>

#include <csi_core.h>
#include <drv/eth.h>
#include <drv/eth_phy.h>
#include <drv/eth_mac.h>

#include <netdrv.h>
#include <hal/ethernet_impl.h>

#include <dw_gmac_lwip.h>

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

#define DW_GMAC_RX_MAX_CNT  8

typedef struct {
    aos_dev_t device;
    struct netif netif;
    uint8_t mac[NETIF_MAX_HWADDR_LEN];
    void *priv;
} eth_dev_t;

static const char *TAG = "netif";

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
    struct eth_addr *ethaddr;
    /* Add whatever per-interface state that is needed here. */
};

static eth_dev_t *eth_dev;
static eth_phy_handle_t eth_phy_handle;
static eth_mac_handle_t eth_mac_handle;

static err_t eth_handle_event(struct netif *netif, uint32_t event);
static int ethernetif_input(struct netif *netif);

static aos_timer_t link_state_timer;

static void event_publish_exception(void)
{
    event_publish(EVENT_ETH_EXCEPTION, NULL);
}

static int32_t eth_phy_read(uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{
    return csi_eth_mac_phy_read(eth_mac_handle, phy_addr, reg_addr, data);
}

static int32_t eth_phy_write(uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
{
    return csi_eth_mac_phy_write(eth_mac_handle, phy_addr, reg_addr, data);
}

static void eth_intr_cb(uint32_t idx, uint32_t event)
{
    struct netif *netif = &eth_dev->netif;

    /* disable RX interrupt */
    csi_eth_mac_control(eth_mac_handle, DRV_ETH_MAC_CONTROL_IRQ, 0);

    tcpip_signal_netif_event(netif, event, eth_handle_event);
}

static eth_link_state_t cur_link_state = ETH_LINK_DOWN;

static void eth_detect_linkstate(void)
{
    eth_link_state_t new_link_state;
    struct netif *netif = &eth_dev->netif;

    new_link_state = csi_eth_phy_get_linkstate(eth_phy_handle);

    if (cur_link_state != new_link_state) {

        cur_link_state = new_link_state;

        LOGI(TAG, "Link %s",
                (cur_link_state == ETH_LINK_UP) ? "Up" : "Down");

        /* adjust mac link parameter */
        csi_eth_mac_control(eth_mac_handle, DRV_ETH_MAC_ADJUST_LINK,
                            (uint32_t)eth_phy_handle);

        if (cur_link_state == ETH_LINK_UP) {
            netif_set_link_up(netif);
            event_publish(EVENT_ETH_LINK_UP, eth_dev);
        } else {
            netif_set_link_down(netif);
            event_publish(EVENT_ETH_LINK_DOWN, NULL);
        }
    }
}

static void eth_linkstate_timeout_cb(void *arg)
{
    eth_detect_linkstate();
}

static err_t eth_handle_event(struct netif *netif, uint32_t event)
{
    uint32_t eth_event;
    static int g_event_error_count = 0;
    int rx_cnt;

    eth_event = event;

    if (eth_event) {
        g_event_error_count = 0;
    } else {
        g_event_error_count++;
        if (g_event_error_count > 1000) {
            LOGE(TAG, "intr overflow");
            g_event_error_count = 0;
            eth_event = 0xFFFF;
        }
    }

    if (eth_event == 0xFFFF) {
        event_publish_exception();
    } else {
        if (eth_event & CSI_ETH_MAC_EVENT_RX_FRAME) {

            for (rx_cnt = 0; rx_cnt <= DW_GMAC_RX_MAX_CNT; rx_cnt++) {
                if (ethernetif_input(netif) != 0) {
                    break;
                }
            }
        }

        if (eth_event & CSI_ETH_MAC_EVENT_TX_FRAME) {
            // nothing to do
        }
    }

    /* enable RX interrupt */
    csi_eth_mac_control(eth_mac_handle, DRV_ETH_MAC_CONTROL_IRQ, 1);
    return 0;
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
    int ret;

    /* set MAC hardware address length */
    netif->hwaddr_len = NETIF_MAX_HWADDR_LEN; /* ETHARP_HWADDR_LEN */

    /* set MAC hardware address */
    memcpy(netif->hwaddr, eth_dev->mac, NETIF_MAX_HWADDR_LEN);

    /* maximum transfer unit */
    netif->mtu = 1500; //TCP_MSS + 54 + 10;  /* net head + redundance */

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP; // | NETIF_FLAG_LINK_UP;

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

    /* initialize MAC & PHY */
    eth_mac_handle = csi_eth_mac_initialize(0, (eth_event_cb_t)eth_intr_cb);
    if (eth_mac_handle == NULL) {
        LOGE(TAG, "Failed to init mac");
        event_publish_exception();
        return;
    }
    eth_phy_handle = csi_eth_phy_initialize(eth_phy_read, eth_phy_write);
    if (eth_phy_handle == NULL) {
        LOGE(TAG, "Failed to init phy");
        event_publish_exception();
        return;
    }

    /* set mac address */
    ret = csi_eth_mac_set_macaddr(eth_mac_handle,
                (const eth_mac_addr_t *)netif->hwaddr);
    if (ret != 0) {
        LOGE(TAG, "Failed to set mac address");
        event_publish_exception();
        return;
    }
    LOGI(TAG, "MAC: %02x:%02x:%02x:%02x:%02x:%02x",
            netif->hwaddr[0],
            netif->hwaddr[1],
            netif->hwaddr[2],
            netif->hwaddr[3],
            netif->hwaddr[4],
            netif->hwaddr[5]);

    /* Start up the PHY */
    ret = csi_eth_phy_power_control(eth_phy_handle, CSI_ETH_POWER_FULL);
    if (ret != 0) {
        LOGE(TAG, "Failed to start phy");
        event_publish_exception();
        return;
    }
#if 0 /* link state is changed in eth_detect_linkstate() */
    /* adjust mac link parameter */
    ret = csi_eth_mac_control(eth_mac_handle, DRV_ETH_MAC_ADJUST_LINK,
                              (uint32_t)eth_phy_handle);
    if (ret != 0) {
        LOGE(TAG, "Failed to adjust link");
        event_publish_exception();
        return;
    }
#endif
    /* start TX */
    ret = csi_eth_mac_control(eth_mac_handle, CSI_ETH_MAC_CONTROL_TX, 1);
    if (ret != 0) {
        LOGE(TAG, "Failed to start TX\n");
        event_publish_exception();
        return;
    }

    /* start RX */
    ret = csi_eth_mac_control(eth_mac_handle, CSI_ETH_MAC_CONTROL_RX, 1);
    if (ret != 0) {
        LOGE(TAG, "Failed to start RX\n");
        event_publish_exception();
        return;
    }

    aos_timer_new_ext(&link_state_timer,
                      (aos_timer_cb_t)eth_linkstate_timeout_cb, netif, 1000,
                      1, 1); /* repeat and autorun */

}

#if !LWIP_NETIF_TX_SINGLE_PBUF
uint8_t tx_data_buf[2048];
#endif

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
#if !LWIP_NETIF_TX_SINGLE_PBUF
    struct pbuf *q = NULL;
    int l = 0;
#endif

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

#if LWIP_NETIF_TX_SINGLE_PBUF
    if (p->tot_len != p->len) {
        LOGE(TAG, "NOT a single pbuf");
        // error ignored
    }
    csi_eth_mac_send_frame(eth_mac_handle, p->payload, p->len, 0);
#else
    for (q = p; q != NULL; q = q->next) {
        memcpy((u8_t *)&tx_data_buf[l], q->payload, q->len);
        l = l + q->len;
    }
    csi_eth_mac_send_frame(eth_mac_handle, tx_data_buf, p->tot_len, 0);
#endif

    MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);

    if (((u8_t *)p->payload)[0] & 1) {
        /* broadcast or multicast packet */
        MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
    } else {
        /* unicast packet */
        MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
    }

    /* increase ifoutdiscards or ifouterrors on error */

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.xmit);

    return ERR_OK;
}

static struct pbuf *low_level_input(struct netif *netif)
{
    struct pbuf *p, *q;
    int32_t len;
    int l = 0;
    uint8_t *frame = NULL;

    /* Obtain the size of the packet and put it into the "len"
       variable. */
    len = drv_eth_mac_request_frame(eth_mac_handle, &frame);

    if (len < 0) {
        /* no packet received, just retry */
        return NULL;
    } else if (len == 0) {
        drv_eth_mac_release_frame(eth_mac_handle);
        return NULL;
    }

#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* allocate a pbuf chain of pbufs from the pool. */
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
            /* read data into(q->payload, q->len); */
            memcpy((u8_t *)q->payload, (u8_t *)&frame[l], q->len);
            l = l + q->len;
        }

        MIB2_STATS_NETIF_ADD(netif, ifinoctets, p->tot_len);

        if (((u8_t *)p->payload)[0] & 1) {
            /* broadcast or multicast packet */
            MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
        } else {
            /* unicast packet */
            MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
        }

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    } else {
        /* drop packet(); */
        /* error to allocate a pbuf chain of pbufs from the pool */
        LOGE(TAG, "Failed to alloc pbuf");
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
        MIB2_STATS_NETIF_INC(netif, ifindiscards);
    }

    drv_eth_mac_release_frame(eth_mac_handle);
    return p;
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
    if (p == NULL) {
        /* if no packet could be read, silently ignore this */
        return -1;
    }

    /* pass all packets to ethernet_input, which decides what packets it supports */
    if (netif->input(p, netif) != ERR_OK) {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\r\n"));
        pbuf_free(p);
        p = NULL;
    }

    return 0;
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
err_t ethernetif_init(struct netif *netif)
{
    struct ethernetif *ethernetif;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    ethernetif = aos_malloc_check(sizeof(struct ethernetif));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "csky-lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

    netif->state = ethernetif;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
#if LWIP_IPV4
    netif->output = etharp_output;
#endif

#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput = low_level_output;

    ethernetif->ethaddr = (struct eth_addr *) & (netif->hwaddr[0]);

    /* initialize the hardware */
    //low_level_init(netif);

    return ERR_OK;
}

/*******************************************************************************
 * Name:
 *          netif_proto_init
 *
 * Description:
 *          the network card init call low_level_init()
 *
 * Parameter:
 *          netif:struct for network card
 *
 * Return:
 *          the status of init
 ******************************************************************************/
static void tcpip_init_done(struct netif *netif)
{
#if LWIP_IPV4
    ip4_addr_t ipaddr, netmask, gw;
    memset(&ipaddr, 0, sizeof(ipaddr));
    memset(&netmask, 0, sizeof(netmask));
    memset(&gw, 0, sizeof(gw));

    netif_add(netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);
#endif

#if LWIP_IPV6
#if !LWIP_IPV4
    netif_add(netif, NULL, ethernetif_init, tcpip_input);
#endif
    netif_create_ip6_linklocal_address(netif, 1);
    netif->ip6_autoconfig_enabled = 1;
#endif

    //netif_set_default(netif);
    //netif_set_up(netif);
}

/*******************************************************************************
 * Name:
 *          netif_proto_init
 *
 * Description:
 *          the network card init call low_level_init()
 *
 * Parameter:
 *          netif:struct for network card
 *
 * Return:
 *          the status of init
 ******************************************************************************/
int lwip_tcpip_init(void)
{
#ifdef PERF
    perf_init("/tmp/minimal.perf");
#endif /* PERF */

    tcpip_init(NULL, NULL);
    LOGI(TAG, "TCP/IP initialized.");
    return 0;
}

/**
 * netif_set_rs_count.
 *
 * @param NULL
 *
 */
void netif_set_rs_count(struct netif *netif, uint8_t value)
{
#if LWIP_IPV6
    netif->rs_count = value;
#endif
}

void netif_power_down(struct netif *netif)
{

}

/*****************************************
* common driver interface
******************************************/
static aos_dev_t *eth_dev_init(driver_t *drv, void *config, int id)
{
    aos_dev_t *dev = device_new(drv, sizeof(eth_dev_t), id);
    eth_dev = (eth_dev_t *)dev;

    memcpy(((eth_dev_t *)dev)->mac, ((eth_config_t *)config)->mac, NETIF_MAX_HWADDR_LEN);

    return dev;
}

#define eth_dev_uninit device_free

static int eth_dev_open(aos_dev_t *dev)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    tcpip_init_done(netif);
    return 0;
}

static int eth_dev_close(aos_dev_t *dev)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    netifapi_netif_set_down(netif);
    netifapi_netif_common(netif, netif_power_down, NULL);
    return 0;
}

//run in idle task, can't use schedule functions
static int eth_dev_lpm(aos_dev_t *dev, int pm_state)
{
    //struct netif *netif = &((eth_dev_t *)dev)->netif;
    //netif->pm_state = pm_state;
    //netifapi_netif_common(netif, netif_manage_power, NULL);
    int eth_pm_state;

    switch (pm_state) {
        case DEVICE_PM_ACTIVE_STATE:
            eth_pm_state = CSI_ETH_POWER_FULL;
            break;

        case DEVICE_PM_LOW_POWER_STATE:
        case DEVICE_PM_SUSPEND_STATE:
            eth_pm_state = CSI_ETH_POWER_LOW;
            break;

        case DEVICE_PM_OFF_STATE:
            eth_pm_state = CSI_ETH_POWER_OFF;
            break;

        default:
            eth_pm_state = -1;
            break;
    }

    LOGD(TAG, "power state(%d)", eth_pm_state);
    //csi_eth_mac_power_control(eth_mac_handle, eth_pm_state);
    return 0;
}

int eth_get_mac_addr(aos_dev_t *dev, uint8_t *mac)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    memcpy((void*)mac, netif->hwaddr, NETIF_MAX_HWADDR_LEN);

    return 0;
}

int eth_set_mac_addr(aos_dev_t *dev, const uint8_t *mac)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    memcpy(netif->hwaddr, mac, NETIF_MAX_HWADDR_LEN);

    return 0;
}

int eth_start(aos_dev_t *dev)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    netifapi_netif_common(netif, low_level_init, NULL);

    netifapi_netif_set_default(netif);
    netifapi_netif_set_up(netif);
    return 0;
}

int eth_restart(aos_dev_t *dev)
{
    return eth_start(dev);
}

static void net_status_callback(struct netif *netif)
{
    if (!ip_addr_isany(ip_2_ip4(&netif->ip_addr))) {
        event_publish(EVENT_NET_GOT_IP, NULL);
    }
}

static int eth_start_dhcp(aos_dev_t *dev)
{
    int ret;
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    LOGI(TAG, "eth start dhcp");

    if (!netif_is_link_up(netif)) {
        LOGE(TAG, "link is down");
        return -1;
    }

    netif_set_ipaddr(netif, NULL);
    netif_set_netmask(netif, NULL);
    netif_set_gw(netif, NULL);

    netif_set_status_callback(netif, net_status_callback);

    //ret = netifapi_dhcp_release(netif);
    ret = netifapi_dhcp_start(netif);
    return ret;
}
static int eth_stop_dhcp(aos_dev_t *dev)
{
#if 0
    struct netif *netif = &((eth_dev_t *)dev)->netif;
    netifapi_dhcp_stop(netif);
#endif
    return 0;
}

static int eth_ifconfig(aos_dev_t *dev)
{
#if LWIP_IPV4
    ip4_addr_t  ipv4;
#endif
    struct netif *netif = &eth_dev->netif;

    /* Default cmd show net info*/
    int ret = netif_is_link_up(netif);

    if (ret) {
        printf("\tnet link: LINK_UP\n");
    } else {
        printf("\tnet link: LINK_DOWN\n");
    }

    printf("\neth0\tLink encap:Ethernet  HWaddr ");
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
            netif->hwaddr[0], netif->hwaddr[1],
            netif->hwaddr[2], netif->hwaddr[3],
            netif->hwaddr[4], netif->hwaddr[5]);

#if LWIP_IPV4 && LWIP_IPV6
    /* Show the IPv4 address */
    printf("    \tinet addr:%s\n", inet_ntoa(netif->ip_addr.u_addr.ip4.addr));

    /* Show the IPv4 default router address */
    printf("\tGWaddr:%s\n", inet_ntoa(netif->gw));

    /* Show the IPv4 network mask */
    printf("\tMask:%s\n", inet_ntoa(netif->netmask));

    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i))) {
            printf("\tinet6 addr: %s Scope:Link\n", ip6addr_ntoa(netif_ip6_addr(netif, i)));
        } else if (ip6_addr_istentative(netif_ip6_addr_state(netif, i))) {
            printf("\tinet6 addr: %s tentative\n", ip6addr_ntoa(netif_ip6_addr(netif, i)));
        } else if (ip6_addr_isinvalid(netif_ip6_addr_state(netif, i))) {
            printf("\tinet6 addr: %s invalid\n", ip6addr_ntoa(netif_ip6_addr(netif, i)));
        }
    }

    memcpy((void *)&ipv4, (const void *)dns_getserver(0), sizeof(ip4_addr_t));
    printf("     \tDNSaddr:%s\n", inet_ntoa(ipv4));

#else /* LWIP_IPV4 && LWIP_IPV6 */

#if LWIP_IPV4
    /* Show the IPv4 address */
    printf("    \tinet addr:%s\n", inet_ntoa(netif->ip_addr));

    /* Show the IPv4 default router address */
    printf("\tGWaddr:%s\n", inet_ntoa(netif->gw));

    /* Show the IPv4 network mask */
    printf("\tMask:%s\n", inet_ntoa(netif->netmask));

    memcpy((void *)&ipv4, (const void *)dns_getserver(0), sizeof(ip_addr_t));
    printf("     \tDNSaddr:%s\n", inet_ntoa(ipv4));
#endif

#if LWIP_IPV6

    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i))) {
            printf("\tinet6 addr: %s Scope:Link\n", ip6addr_ntoa(netif_ip6_addr(netif, i)));
        } else if (ip6_addr_istentative(netif_ip6_addr_state(netif, i))) {
            printf("\tinet6 addr: %s tentative\n", ip6addr_ntoa(netif_ip6_addr(netif, i)));
        } else if (ip6_addr_isinvalid(netif_ip6_addr_state(netif, i))) {
            printf("\tinet6 addr: %s invalid\n", ip6addr_ntoa(netif_ip6_addr(netif, i)));
        }
    }

    //printf("     \tDNSaddr:%s\n", inet6_ntoa(addr));
#endif

#endif
    return 0;
}

static int eth_set_ipaddr(aos_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    netifapi_netif_set_addr(netif, ip_2_ip4(ipaddr), ip_2_ip4(netmask), ip_2_ip4(gw));
    return 0;
}

static int eth_get_ipaddr(aos_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(ipaddr), *netif_ip_addr4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(gw), *netif_ip_gw4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(netmask), *netif_ip_netmask4(netif));

    return 0;
}

static int eth_subscribe(aos_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    if (cb) {
        event_subscribe(event, cb, param);
    }

    return 0;
}

// for L3 operations
static net_ops_t eth_net_driver = {
    .get_mac_addr = eth_get_mac_addr,
    .set_mac_addr = eth_set_mac_addr,
    //.set_link_up = eth_set_link_up,
    .start_dhcp = eth_start_dhcp,
    .stop_dhcp = eth_stop_dhcp,
    .set_ipaddr = eth_set_ipaddr,
    .get_ipaddr = eth_get_ipaddr,
    .subscribe = eth_subscribe,
};

// for L2 configurations
static int eth_set_packet_filter(aos_dev_t *dev, int type)
{
    return 0;
}

// for L2
static eth_driver_t eth_driver = {
    .set_packet_filter = eth_set_packet_filter,
    .start = eth_start,
    .restart = eth_restart,
    .ifconfig = eth_ifconfig,
};

static netdev_driver_t net_driver = {
    .drv = {
        .name   = "eth",
        .init   = eth_dev_init,
        .uninit = eth_dev_uninit,
        .open   = eth_dev_open,
        .close  = eth_dev_close,
        .lpm    = eth_dev_lpm,
    },
    .link_type = NETDEV_TYPE_ETH,
};

void eth_dw_gmac_register(eth_config_t *eth_config)
{
    net_driver.net_ops = &eth_net_driver;
    net_driver.link_ops = &eth_driver;

    /* run eth_dev_init to create eth_dev_t and bind this driver */
    driver_register(&net_driver.drv, eth_config, 0);
    lwip_tcpip_init();
}
