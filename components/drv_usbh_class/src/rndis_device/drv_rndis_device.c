/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/cli.h>
#include <ulog/ulog.h>

#include "netif/etharp.h"
#include <lwip/netif.h>
#include <lwip/netifapi.h>

#include "usbd_core.h"
#include "usbd_rndis.h"
#include "rndis_protocol.h"

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

#define RNDIS_MSG_QUEUE_NUM 100

#define RNDIS_MSG_TPYE_INPUT  0
#define RNDIS_MSG_TPYE_OUTPUT 1

/*!< endpoint address */
#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x83

#define USBD_VID           0xEFFF
#define USBD_PID           0xEFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + CDC_RNDIS_DESCRIPTOR_LEN)

#define TAG "rndis"

typedef struct rndis_msg {
    int      type;
    uint8_t *buf;
    uint32_t len;
} rndis_msg_t;

static void rndis_data_input(void *data, int size);

struct netif           rndis_device_netif;
static aos_queue_t     s_rndis_msg_queue;
static rndis_msg_t     s_queue_msg[RNDIS_MSG_QUEUE_NUM];

static uint8_t mac[6] = { 0x00, 0x15, 0x5D, 0xC7, 0xE6, 0x89};
static struct usbd_interface intf0;
static struct usbd_interface intf1;

/*!< global descriptor */
static const uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_RNDIS_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, 0x02),
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x2A,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'R', 0x00,                  /* wcChar10 */
    'N', 0x00,                  /* wcChar11 */
    'D', 0x00,                  /* wcChar12 */
    'I', 0x00,                  /* wcChar13 */
    'S', 0x00,                  /* wcChar14 */
    ' ', 0x00,                  /* wcChar15 */
    'D', 0x00,                  /* wcChar16 */
    'E', 0x00,                  /* wcChar17 */
    'M', 0x00,                  /* wcChar18 */
    'O', 0x00,                  /* wcChar19 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

static void low_level_init(struct netif *netif)
{
    uint8_t mac_addr[6] = { 0x00, 0x15, 0x5D, 0xC7, 0xE6, 0x79};

    netif->hwaddr_len = ETH_ALEN;

    memcpy(&netif->hwaddr[0], mac_addr, ETH_ALEN);

    /* Maximum transfer unit */
    netif->mtu = 1500;

#if LWIP_IPV6_AUTOCONFIG
    netif_set_ip6_autoconfig_enabled(netif, 1);
#endif
    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP
#if defined(DHCP_USED)
                    | NETIF_FLAG_DHCP
#endif
        ;
}

static struct pbuf *low_level_input(struct netif *netif, uint8_t *buf, uint32_t buf_len)
{
    struct pbuf *p = NULL, *q = NULL;

#if ETH_PAD_SIZE
    s_len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, buf_len, PBUF_RAM);

    if (p != NULL) {
#if ETH_PAD_SIZE
        pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

        /* Iterate over the pbuf chain until we have read the entire
         * packet into the pbuf. */
        for (q = p; q != NULL; q = q->next) {
            /* Read enough bytes to fill this pbuf in the chain. The
             * available data in the pbuf is given by the q->len
             * variable. */
            /* read data into(q->payload, q->len); */
            memcpy(q->payload, buf, q->len);
            buf += q->len;
        }
        /* Acknowledge that packet has been read(); */

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* Reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    } else {
        /* Drop packet(); */
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
    }

    return p;
}

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *        q       = NULL;
    int                  datalen = 0;
    uint8_t *            buf;
    rndis_msg_t          msg;

    int ret;

    buf = malloc(p->tot_len);

    if (buf == NULL)
        return ERR_MEM;

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* Drop the padding word */
#endif

    for (q = p; q != NULL; q = q->next) {
        /* Send data from(q->payload, q->len); */
        memcpy(buf + datalen, q->payload, q->len);
        datalen += q->len;
    }

    msg.buf  = buf;
    msg.len  = p->tot_len;
    msg.type = RNDIS_MSG_TPYE_OUTPUT;

    ret = aos_queue_send(&s_rndis_msg_queue, &msg, sizeof(rndis_msg_t));

    if (ret) {
        LOGE(TAG, "queue send error");
        return ERR_ABRT;
    }

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* Reclaim the padding word */
#endif

    LINK_STATS_INC(link.xmit);

    return ERR_OK;
}

static err_t rndis_netif_init(struct netif *netif)
{
    netif->output = etharp_output;
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

static int rndis_data_rx(void *buf, int nbytes)
{
    struct netif *netif = &rndis_device_netif;
    struct pbuf * p;

    if (nbytes > 0) {
        p = low_level_input(&rndis_device_netif, buf, nbytes);
        if (p) {
            if (ERR_OK != netif->input(p, netif)) {
                LOGE(TAG, "ethernetif_input error");
                pbuf_free(p);
                p = NULL;
            }
            return 0;
        } else {
            return -1;
        }
    }

    return -1;
}

static int rndis_data_tx(void *data, uint32_t len)
{
    return usbd_rndis_eth_write(data, len);
}

static void rndis_data_input(void *data, int size)
{
    rndis_msg_t        msg;

    void *buf = malloc(size);

    if (buf == NULL) {
        return;
    }

    memcpy(buf, data, size);

    msg.buf  = buf;
    msg.len  = size;
    msg.type = RNDIS_MSG_TPYE_INPUT;

    aos_queue_send(&s_rndis_msg_queue, &msg, sizeof(rndis_msg_t));
}

static void rndis_task_entry(void *argv)
{
    rndis_msg_t        msg;
    size_t             size;

    while (1) {
        aos_queue_recv(&s_rndis_msg_queue, AOS_WAIT_FOREVER, &msg, &size);

        if (msg.type == RNDIS_MSG_TPYE_INPUT) {
            // LOGD(TAG, "rndis_data_rx, len: %d", msg.len);

            rndis_data_rx(msg.buf, msg.len);

            free(msg.buf);
        } else if (msg.type == RNDIS_MSG_TPYE_OUTPUT) {

            // LOGD(TAG, "rndis_data_tx, len: %d", msg.len);

            rndis_data_tx(msg.buf, msg.len);

            free(msg.buf);
        } else {
            LOGE(TAG, "Wrong Type");
        }
    }
}

void rndis_device_start_network()
{
    static int init_flag;

    if (init_flag == 1) {
        return;
    }

    init_flag = 1;
    aos_task_t task_handle;

    aos_queue_new(&s_rndis_msg_queue, s_queue_msg, sizeof(s_queue_msg), sizeof(rndis_msg_t));

    aos_task_new_ext(&task_handle, "rndis_trx", rndis_task_entry, NULL, 2048, AOS_DEFAULT_APP_PRI);
}

static void _read_cb(void *data, uint32_t len)
{
    usbd_rndis_eth_start_read();

    rndis_data_input(data, len);
}

static void cdc_rndis_init(void)
{
    usbd_desc_register(cdc_descriptor);
    usbd_add_interface(usbd_rndis_init_intf(&intf0, CDC_OUT_EP, CDC_IN_EP, CDC_INT_EP, mac));
    usbd_add_interface(usbd_rndis_init_intf(&intf1, CDC_OUT_EP, CDC_IN_EP, CDC_INT_EP, mac));
    usbd_initialize();

    usbd_rndis_set_read_cb(_read_cb);
}

void rndis_device_lwip_init(void)
{
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;

    IP4_ADDR(&ipaddr, 192, 168, 11, 10);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 11, 1);

    cdc_rndis_init();

    tcpip_init(NULL, NULL);

    rndis_device_netif.name[0] = 'e';
    rndis_device_netif.name[1] = '0';
    netifapi_netif_add(&rndis_device_netif, &ipaddr, &netmask, &gw, NULL, rndis_netif_init, tcpip_input);

    netifapi_netif_set_default(&rndis_device_netif);

    netifapi_netif_set_up(&rndis_device_netif);
    netifapi_netif_set_link_up(&rndis_device_netif);

    rndis_device_start_network();
}
