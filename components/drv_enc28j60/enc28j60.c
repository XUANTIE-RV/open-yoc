/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

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

#include "csi_core.h"
#include "drv/tee.h"
#include "drv/spi.h"
#include "drv/gpio.h"
#include "drv/eth.h"
#include "drv/eth_phy.h"
#include "drv/eth_mac.h"

#include <devices/netdrv.h>
#include <devices/hal/ethernet_impl.h>

#include "csi_eth_enc28j60.h"
#include "enc28j60.h"

//#define GPIO_PIN_FOR_DEBUG
#define DEBUG_ETH_IN_EXCEPTION      0
#define DEBUG_ETH_OUT_EXCEPTION     0

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

enum nic_event {
    NIC_EVENT_INT,
    NIC_EVENT_API_RESET,
    NIC_EVENT_WAKE_UP
};

typedef struct {
    aos_dev_t device;
    struct netif netif;
    uint8_t mac[NETIF_MAX_HWADDR_LEN];
    enc28j60_pin_t pins;
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
//static eth_config_t eth_configs;
//static enc28j60_pin_t eth_pins;
static gpio_pin_handle_t pin_int = NULL;
static eth_phy_handle_t eth_phy_handle;
static eth_mac_handle_t eth_mac_handle;

static aos_timer_t timer_send_handle;
static int g_bTimerStopped = 1;
static int g_flag_exception = 1;

static void eth_gpio_config_int(bool en);
static err_t eth_handle_event(struct netif *netif, uint32_t event);
static void  ethernetif_input(struct netif *netif);
extern int32_t drv_get_sys_freq(void);
extern uint32_t cmd_ping4_func(ip_addr_t *ping_target);
extern int cmd_ifconfig(int item_count, char **argv);

static void event_publish_exception(void)
{
    if (g_flag_exception == 0 || eth_mac_handle == NULL) {
        g_flag_exception = 1;
        event_publish(EVENT_ETH_EXCEPTION, NULL);
    }
}

#ifdef GPIO_PIN_FOR_DEBUG
static gpio_pin_handle_t *pin = NULL;

void pin_set_level(int level)
{
    if (level) {
        csi_gpio_pin_write(pin, 1);
    } else {
        csi_gpio_pin_write(pin, 0);
    }
}

void pin_toggle(void)
{
    static int level = 0;

    level ^= 0x01;
    csi_gpio_pin_write(pin, level);
}

void pin_gpio_init(void)
{
    drv_pinmux_config(PA18, PIN_FUNC_GPIO);

    pin = csi_gpio_pin_initialize(PA18, NULL);

    csi_gpio_pin_config_mode(pin, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pin, GPIO_DIRECTION_OUTPUT);
}
#else
void pin_toggle(void)
{
    // do nothing;
}
#endif

/**
 int line:
 -----------            ----------------------
            |___________|   |_________________
            1           2 3 4
 1. a nic event arised, enc28j60 pulls int line down
    run ISR to tell tcpip thread to handle the event
 2. switch to tcpip thread, clear INTIE, save and clear all pending event
 3. handle the event one by one
 4. set INTIE, if new event is valid while handling event, enc28j60 pulls line down
    otherwise, keep the line high as usually

 If a falling edge of GPIO is missing, the line is always kept low,
 no gpio interrupt will be triggered any more.
 As consider of reliability, trigger interrupt by low level
 */
static void eth_int_handle(int32_t idx)
{
    struct netif *netif = &eth_dev->netif;

    pin_toggle();

    eth_gpio_config_int(0);

    tcpip_signal_netif_event(netif, NIC_EVENT_INT, eth_handle_event);
}

#if 0
static bool eth_check_card_is_existed(void)
{
    bool val;

    csi_gpio_pin_read(pin_int, &val);

    if (val == 1) {
        return true;
    } else {
        return false;
    }
}
#endif

static void eth_gpio_config_int(bool en)
{
    if (en) {
        csi_gpio_pin_set_irq(pin_int, GPIO_IRQ_MODE_LOW_LEVEL/*GPIO_IRQ_MODE_LOW_LEVEL*/, 1);
    } else {
        csi_gpio_pin_set_irq(pin_int, GPIO_IRQ_MODE_LOW_LEVEL/*GPIO_IRQ_MODE_LOW_LEVEL*/, 0);
    }
}

bool eth_gpio_get_level(void)
{
    bool pin_level;
    csi_gpio_pin_read(pin_int, &pin_level);

    return pin_level;
}

static int32_t eth_gpio_init(int32_t pin, gpio_event_cb_t event_cb)
{
    uint32_t ret;

    pin_int = csi_gpio_pin_initialize(pin, event_cb);

    ret = csi_gpio_pin_config_mode(pin_int, GPIO_MODE_PULLUP);
    ret = csi_gpio_pin_config_direction(pin_int, GPIO_DIRECTION_INPUT);

    ret = csi_gpio_pin_set_irq(pin_int, GPIO_IRQ_MODE_LOW_LEVEL/*GPIO_IRQ_MODE_LOW_LEVEL*/, 0);

    return ret;
}

static int32_t g_fn_phy_read(uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{
    return 0;
}

static int32_t g_fn_phy_write(uint8_t phy_addr, uint8_t reg_addr, uint16_t  data)
{
    return 0;
}

void eth_spi_transfer_callback(int32_t idx, spi_event_e event)
{
    if (event == SPI_EVENT_TRANSFER_COMPLETE) {

    } else if (event == SPI_EVENT_TX_COMPLETE) {

    } else if (event == SPI_EVENT_RX_COMPLETE) {

    }/* else if (event == SPI_EVENT_DATA_LOST) {

        printf("TRANSFER_DATA_LOST\n");
    } */else {
        LOGE(TAG, "spi event %d\n", event);
    }
}

int32_t eth_spi_init(int32_t idx, spi_handle_t *spi_hdl, gpio_pin_handle_t *gpio_hdl)
{
    enc28j60_pin_t *eth_pin = (enc28j60_pin_t *)&eth_dev->pins;
    *spi_hdl = csi_spi_initialize(idx, (spi_event_cb_t)eth_spi_transfer_callback);
    //ETH_LOG_DBG("spi %p\n", *spi_hdl);

    //spi_handle success goto config spi
    if (*spi_hdl != NULL) {
        csi_spi_config(*spi_hdl, drv_get_sys_freq() / 10, SPI_MODE_MASTER, SPI_FORMAT_CPOL0_CPHA0,
                       SPI_ORDER_MSB2LSB, SPI_SS_MASTER_SW, 8);

        *gpio_hdl = csi_gpio_pin_initialize(eth_pin->enc28j60_spi_cs, NULL);
        //ETH_LOG_DBG("gpio %p\n", *gpio_hdl);

        csi_gpio_pin_config_mode(*gpio_hdl, GPIO_MODE_PULLNONE);
        csi_gpio_pin_config_direction(*gpio_hdl, GPIO_DIRECTION_OUTPUT);
    }

    return 0;
}

/**
 *  Hard reset enc28j60
 * @param void
 *
 */
static void eth_hard_reset(void)
{
    gpio_pin_handle_t pin = NULL;
    enc28j60_pin_t *eth_pin = (enc28j60_pin_t *)&eth_dev->pins;
    pin = csi_gpio_pin_initialize(eth_pin->enc28j60_spi_rst, NULL);

    csi_gpio_pin_config_mode(pin, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pin, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(pin, 0);  /* LOW */
    aos_msleep(10);
    csi_gpio_pin_write(pin, 1);
    aos_msleep(50);
}

static void eth_send_timeout_cb(void *arg)
{
    //struct netif *netif = (struct netif *)arg;
    LOGE(TAG, "eth send");
    aos_timer_stop(&timer_send_handle);

    event_publish_exception();
}

static int nic_link_stat = 0;
static err_t eth_handle_event(struct netif *netif, uint32_t event)
{
    int link_status;
    uint32_t eth_event;
    aos_dev_t *dev = (aos_dev_t *)eth_dev;
    static int g_event_error_count = 0;
    pin_toggle();

    eth_event = eth_get_event(0);

    if (eth_event) {
        aos_timer_stop(&timer_send_handle);
        g_bTimerStopped = 1;
        g_event_error_count = 0;
    } else {
        g_event_error_count ++;
        if (g_event_error_count > 1000) {
            LOGE(TAG, "intr ovfl");
            g_event_error_count = 0;
            eth_event = 0x80;
        }
    }

    if (eth_event & 0x80) {
        event_publish_exception();
    } else {
        if (eth_event & CSI_ETH_MAC_EVENT_LINK_CHANGE) {
            link_status = csi_eth_phy_get_linkstate(eth_phy_handle);

            if ((link_status != -1)) {
                if (nic_link_stat != link_status) {
                    nic_link_stat = link_status;
                    LOGI(TAG, "Net:link %s", (nic_link_stat == 1) ? "up" : "down");

                    if (nic_link_stat == 1) {
                        netif_set_link_up(netif);
                        event_publish(EVENT_ETH_LINK_UP, eth_dev);

                        g_flag_exception = 0;
                    } else {
                        netif_set_link_down(netif);
                        event_publish(EVENT_ETH_LINK_DOWN, NULL);
                    }
                }
            } else {
                /* if error, post connect, app error will reset driver */
                event_publish(EVENT_ETH_LINK_UP, eth_dev);
            }
        }

        if (eth_event == CSI_ETH_MAC_EVENT_RX_FRAME) {

            ethernetif_input(netif);

            /*if there are packets in rx buffer, interrupt will be triggered again*/
        }

        if (eth_event == CSI_ETH_MAC_EVENT_TX_FRAME) {
            if (dev) {
                dev->busy = 0;
            }
        }

        pin_toggle();
    }

    //Enable interrupt
    csi_eth_mac_control(eth_mac_handle, CSI_ETH_MAC_CONTROL_RX, 1);
    eth_gpio_config_int(1);

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

    enc28j60_pin_t *eth_pin = (enc28j60_pin_t *)&eth_dev->pins;

    /* set MAC hardware address length */
    netif->hwaddr_len = NETIF_MAX_HWADDR_LEN; //ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    memcpy(netif->hwaddr, eth_dev->mac, NETIF_MAX_HWADDR_LEN);

    /* maximum transfer unit */
    netif->mtu = TCP_MSS + 54 + 10;  /* net head + redundance */

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;// | NETIF_FLAG_LINK_UP;

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
    if (timer_send_handle.hdl){
        aos_timer_stop(&timer_send_handle);
        aos_timer_free(&timer_send_handle);
        g_bTimerStopped = 1;
    }
    
    aos_timer_new_ext(&timer_send_handle, (aos_timer_cb_t)eth_send_timeout_cb, netif, 100, 0, 0);

    if (eth_pin->enc28j60_spi_interrupt) {
        eth_gpio_init(eth_pin->enc28j60_spi_interrupt, eth_int_handle);
    } else {
        LOGE(TAG, "spi intr");
        event_publish_exception();
        return;
    }

    csi_eth_mac_set_macaddr(eth_mac_handle, (const eth_mac_addr_t *)netif->hwaddr);

    int retry = 0;

    do {
        eth_hard_reset();
        eth_phy_handle = csi_eth_phy_initialize(g_fn_phy_read, g_fn_phy_write);
        eth_mac_handle = csi_eth_mac_initialize(eth_pin->enc28j60_spi_idx, NULL);
        retry++;
        // TODO: power off and power on ethernet module
    } while ((eth_mac_handle == NULL) && (retry < 3));

    if (eth_mac_handle == NULL) {
        LOGE(TAG, "eth init");
        event_publish_exception();
        return;
    }

    LOGI(TAG, "MAC: %02x:%02x:%02x:%02x:%02x:%02x", netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2], netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);

    csi_eth_mac_control(eth_mac_handle, CSI_ETH_MAC_CONTROL_RX, 1);
    eth_gpio_config_int(1);

#ifdef GPIO_PIN_FOR_DEBUG
    pin_gpio_init();
#endif
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
#ifdef CONFIG_TCPIP_ENC28J60_USE_PBUF
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q = NULL;
    int l = 0;
    aos_dev_t *dev = (aos_dev_t *)eth_dev;
#if DEBUG_ETH_OUT_EXCEPTION
    static int cnt;
#endif

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
    pin_toggle();

    if (g_bTimerStopped) {
        aos_timer_stop(&timer_send_handle);
        aos_timer_change(&timer_send_handle, 10000);
        aos_timer_start(&timer_send_handle);
        g_bTimerStopped = 0;
    }

    if (dev) {
        dev->busy = 1;
    }

#if DEBUG_ETH_OUT_EXCEPTION
    if (cnt++ > 50) {
        cnt = 0;
        event_publish_exception();
    }
#endif
    csi_eth_mac_ex_send_frame_begin(eth_mac_handle, p->tot_len);

    for (q = p; q != NULL; q = q->next) {
        csi_eth_mac_ex_send_frame(eth_mac_handle, q->payload, q->len, 0);
        l = l + q->len;
    }

    csi_eth_mac_ex_send_frame_end(eth_mac_handle);
    pin_toggle();

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
    int32_t len;
    int l = 0;
#if DEBUG_ETH_IN_EXCEPTION
    static int cnt;
#endif

    /* Obtain the size of the packet and put it into the "len"
       variable. */
    len = csi_eth_mac_ex_read_frame_begin(eth_mac_handle);

#if DEBUG_ETH_IN_EXCEPTION
    if (cnt++ > 50) {
        cnt = 0;
        len = -1;
    }
#endif

    if (len < 0) {
        /* errors in rx buffer, reset enc28j60 */
        LOGE(TAG, "rxb");

        /* reset, user link will down */
        low_level_init(netif);

        // Let netmgr recover eth exception
        event_publish_exception();

        return NULL;
    } else if (len == 0) {
        return NULL;
    }

    //g_continue_rx_error = 0;

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
            /* read data into(q->payload, q->len); */
            csi_eth_mac_ex_read_frame(eth_mac_handle, (u8_t *)q->payload, q->len);
            l = l + q->len;
        }

        csi_eth_mac_ex_read_frame_end(eth_mac_handle);

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
        LOGE(TAG, "alloc pbuf");
        /* drop packet(); */
        csi_eth_mac_ex_read_frame_end(eth_mac_handle);
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
        MIB2_STATS_NETIF_INC(netif, ifindiscards);
    }

    return p;
}
#else

uint8_t Data_Buf[1528];
uint8_t Tx_Data_Buf[1528];

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q = NULL;
    int l = 0;

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
    aos_timer_stop(&timer_send_handle);
    aos_timer_change(&timer_send_handle, 10000);
    aos_timer_start(&timer_send_handle);

    for (q = p; q != NULL; q = q->next) {
        memcpy((u8_t *)&Tx_Data_Buf[l], q->payload, q->len);
        l = l + q->len;
    }

    csi_eth_mac_send_frame(eth_mac_handle, Tx_Data_Buf, p->tot_len, 0);

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
    int32_t len;
    int l = 0;

    /* Obtain the size of the packet and put it into the "len"
       variable. */
    len = csi_eth_mac_read_frame(eth_mac_handle, Data_Buf, MAX_FRAMELEN);

    if (len < 0) {
        /* errors in rx buffer, reset enc28j60 */
        LOGE(TAG, "rxb");
        event_publish_exception();

        return NULL;
    } else if (len == 0) {
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
            /* read data into(q->payload, q->len); */
            memcpy((u8_t *)q->payload, (u8_t *)&Data_Buf[l], q->len);
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
        LOGE(TAG, "alloc pbuf");
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
        MIB2_STATS_NETIF_INC(netif, ifindiscards);
    }

    return p;
}
#endif

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
static void ethernetif_input(struct netif *netif)
{
    //struct ethernetif *ethernetif;
    struct pbuf *p;

    //ethernetif = netif->state;

    /* move received packet into a new pbuf */
    p = low_level_input(netif);

    /* if no packet could be read, silently ignore this */
    if (p != NULL) {
        /* pass all packets to ethernet_input, which decides what packets it supports */
        if (netif->input(p, netif) != ERR_OK) {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\r\n"));
            pbuf_free(p);
            p = NULL;
        }
    }
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
    netif->hostname = "cksy-lwip";
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

/********************************************************************************
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
 ********************************************************************************/
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


/********************************************************************************
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
 ********************************************************************************/
static int lwip_tcpip_init(void)
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
    memcpy(&((eth_dev_t *)dev)->pins, ((eth_config_t *)config)->net_pin, sizeof(enc28j60_pin_t));

    return dev;
}

#define eth_dev_uninit device_free

static int eth_dev_open(aos_dev_t *dev)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;
    // power on eth module

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
    csi_eth_mac_power_control(eth_mac_handle, eth_pm_state);

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
    if (g_flag_exception || nic_link_stat) {
        struct netif *netif = &((eth_dev_t *)dev)->netif;

        /* set down stat, after init will post link up event */
        nic_link_stat = 0;

        netifapi_netif_common(netif, low_level_init, NULL);

        netifapi_netif_set_default(netif);
        netifapi_netif_set_up(netif);
    }

    return 0;
}

int eth_restart(aos_dev_t *dev)
{
    return eth_start(dev);
}

static int eth_start_dhcp(aos_dev_t *dev)
{
    int ret;
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    if (!netif_is_link_up(netif)) {
        return -1;
    }

    netif_set_ipaddr(netif, NULL);
    netif_set_netmask(netif, NULL);
    netif_set_gw(netif, NULL);
    //ret = netifapi_dhcp_release(netif);
    ret = netifapi_dhcp_start(netif);

    return ret;
}
static int eth_stop_dhcp(aos_dev_t *dev)
{
    struct netif *netif = &((eth_dev_t *)dev)->netif;

    netifapi_dhcp_stop(netif);

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
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2], netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);

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
    .start = eth_start, //start L2 link up, can be put to net driver for all device
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

void eth_enc28j60_register(eth_config_t *eth_config)
{
    net_driver.net_ops = &eth_net_driver;
    net_driver.link_ops = &eth_driver;

    //run eth_dev_init to create eth_dev_t and bind this driver
    driver_register(&net_driver.drv, eth_config, 0);
    lwip_tcpip_init();
}

