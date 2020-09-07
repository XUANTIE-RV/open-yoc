/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/debug.h>

#include <sal.h>
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
#include <netif/ethernet.h>
#include <lwip/api.h>
#include <lwip/dns.h>

#include <devices/netdrv.h>

#include "hal/wifi_impl.h"
#include "m88wi6700s_devops.h"

#define WIFI_SSID_MAX_LEN 32
#define WIFI_PSK_MAX_LEN 64

#define WIFI_CHANNEL_MAX 14
#define WIFI_CHANNEL_ANY 255

enum wifi_security_type {
    WIFI_SECURITY_TYPE_NONE = 0,
    WIFI_SECURITY_TYPE_PSK,
};


#define IEEE80211_FCTL_TODS     0x0100
#define IEEE80211_FCTL_FROMDS   0x0200
#define IEEE80211_FTYPE_MGMT    0x0000
#define IEEE80211_FTYPE_CTL     0x0004
#define IEEE80211_FTYPE_DATA    0x0008
#define ETH_ALEN 6

struct ieee80211_hdr {
    u16 frame_control;
    u16 duration_id;
    u8 addr1[ETH_ALEN];
    u8 addr2[ETH_ALEN];
    u8 addr3[ETH_ALEN];
    u16 seq_ctrl;
} __attribute__((__packed__));

struct wifi_connect_req_params {
    uint8_t ssid[WIFI_SSID_MAX_LEN];
    uint8_t ssid_length; /* Max 32 */

    uint8_t psk[WIFI_PSK_MAX_LEN];
    uint8_t psk_length; /* Min 8 - Max 64 */

    uint8_t bssid[ETH_ALEN];

    uint8_t                 channel;
    enum wifi_security_type security;
};

typedef struct {
    aos_dev_t   device;
    uint8_t mode;

    struct wifi_connect_req_params conn_params;

    void (*write_event)(aos_dev_t *dev, int event_id, void *priv);
    void *priv;
} wifi_dev_t;



extern void dhcps_init(struct netif *pnetif);
extern void dhcp_run(void *, int, int, void *);
void dhcps_deinit(void);

static wifi_event_func *g_evt_func;
static aos_dev_t *wifi_evt_dev;
static wifi_promiscuous_cb_t g_monitor_cb;
static const char *TAG = "6700s OPS";

/*****************************************
* common netif driver interface
******************************************/




int dhcp_server_start()
{

    struct netif *netif = netif_find("en0");
    aos_check_return_einval(netif);
    ip4_addr_t ip_addr;
    ip4_addr_t net_mask;
    ip4_addr_t gw_addr;

    ip4addr_aton("192.168.1.1", (ip4_addr_t *)ip_2_ip4(&ip_addr));
    ip4addr_aton("255.255.255.0", (ip4_addr_t *)ip_2_ip4(&net_mask));
    ip4addr_aton("192.168.1.1", (ip4_addr_t *)ip_2_ip4(&gw_addr));

    netif_set_addr(netif, &ip_addr, &net_mask, &gw_addr);

    LOGD(TAG, "Init dhcp server!\n");

    dhcps_init(netif);
    return 0;
}



static int m88wi6700s_get_mac_addr(aos_dev_t *dev, uint8_t *mac)
{
    return lynx_api_get_mac(mac);
}


static int m88wi6700s_start_dhcp(aos_dev_t *dev)
{
    struct netif *netif = netif_find("en0");
    aos_check_return_einval(netif);

    if (!netif_is_link_up(netif)) {
        return -1;
    }

    netif_set_ipaddr(netif, NULL);
    netif_set_netmask(netif, NULL);
    netif_set_gw(netif, NULL);

    return netifapi_dhcp_start(netif);
}

static int m88wi6700s_stop_dhcp(aos_dev_t *dev)
{
    struct netif *netif = netif_find("en0");
    aos_check_return_einval(netif);

    netifapi_dhcp_stop(netif);

    return 0;
}

static int m88wi6700s_set_ipaddr(aos_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask,
                                 const ip_addr_t *gw)
{
    return -1;
}

static int m88wi6700s_get_ipaddr(aos_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw)
{
    struct netif *netif = netif_find("en0");
    aos_check_return_einval(netif && ipaddr && netmask && gw);

    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(ipaddr), *netif_ip_addr4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(gw), *netif_ip_gw4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(netmask), *netif_ip_netmask4(netif));

    return 0;
}

static int m88wi6700s_subscribe(aos_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    if (cb) {
        event_subscribe(event, cb, param);
    }

    return 0;
}
/*****************************************
* wifi driver interface
******************************************/

static void netif_link_cb(struct netif *netif)
{
    int is_link_up = netif_is_link_up(netif);
    event_publish(is_link_up ? EVENT_WIFI_LINK_UP : EVENT_WIFI_LINK_DOWN, NULL);
}


static int m88wi6700s_stop(aos_dev_t *dev)
{
    struct netif *netif = netif_find("en0");
    aos_check_return_einval(netif);
    netif_set_link_down(netif);
    netif_set_addr(netif, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
    return 0;
}

static int m88wi6700s_start(aos_dev_t *dev, wifi_config_t * config)
{
    int try = 5;
    
    struct netif *netif = netif_find("en0");
    aos_check_return_einval(netif);
    netif_set_link_down(netif);
    netif_set_addr(netif, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
    dhcps_deinit();

    switch (config->mode) {
    case WIFI_MODE_STA: {
            char bssid[6] = {0};
            if (config->sta_config.present)
                memcpy(bssid, config->sta_config.bssid, 6);
            /** scan before connect because AP channel
                may change when reconnect*/
            lynx_api_start_scan(0);
            lynx_api_switch_mode(NL80211_IFTYPE_STATION);
            while (-1 == lynx_api_connect((unsigned char*)config->ssid, (unsigned char*)config->password, (unsigned char*)bssid)) {
                aos_msleep(1000);

                try--;

                if (try <= 0)
                        return -1;
            }
        }
        break;
    case WIFI_MODE_AP:
        dhcp_server_start();
        lynx_api_switch_mode(NL80211_IFTYPE_AP);
        break;
    default:
        printf("Mode not handled");
    }


    return 0;
}

static int m88wi6700s_deinit(aos_dev_t *dev)
{
    return 0;
}

static int m88wi6700s_reset(aos_dev_t *dev)
{
    return 0;
}

static int m88wi6700s_init(aos_dev_t *dev)
{

    struct netif *netif = netif_find("en0");
    aos_check_return_einval(netif);
    netif_set_link_callback(netif, netif_link_cb);

    //lynx_cli2_init();

    return 0;
}


static int m88wi6700s_get_mode(aos_dev_t *dev, wifi_mode_t *mode)
{
    extern enum nl80211_iftype lynx_getiftype(void);

    switch (lynx_getiftype()) {
        case NL80211_IFTYPE_STATION:
            *mode = WIFI_MODE_STA;
            break;

        case NL80211_IFTYPE_UNSPECIFIED:
            *mode = WIFI_MODE_NULL;
            break;

        case NL80211_IFTYPE_AP:
            *mode = WIFI_MODE_AP;
            break;

        case NL80211_IFTYPE_AP_VLAN:
            *mode = WIFI_MODE_APSTA;
            break;

        default:
            *mode = WIFI_MODE_NULL;
    }

    return 0;
}

static int m88wi6700s_set_mode(aos_dev_t *dev, wifi_mode_t mode)
{
    /** before switch mode, net link is brought down */
    struct netif *netif = netif_find("en0");
    netif_set_link_down(netif);
    /** stop dhcp server */
    dhcps_deinit();

    switch (mode) {
        case WIFI_MODE_STA:
            lynx_api_switch_mode(NL80211_IFTYPE_STATION);
            break;

        case WIFI_MODE_AP:
            dhcp_server_start();
            lynx_api_switch_mode(NL80211_IFTYPE_AP);
            break;
        case WIFI_MODE_NULL:
        case WIFI_MODE_APSTA:
        case WIFI_MODE_P2P:
        default:
            LOGE(TAG, "Mode %d not handled!", mode);
            return -1;
    }

    return 0;
}


static int m88wi6700s_drv_get_link_status(aos_dev_t *dev, wifi_ap_record_t *ap_info)
{
    struct netif *netif = netif_find("en0");
    
    if (netif == NULL) {
        ap_info->link_status = WIFI_STATUS_LINK_DOWN;
        return 0;
    }

    /* Default cmd show net info*/
    int ret = netif_is_link_up(netif);

    if (ret) {
        //printf("\tnet link: LINK_UP\n");
        ap_info->link_status = WIFI_STATUS_LINK_UP;
        lynx_api_get_ap_info(ap_info->bssid, ap_info->ssid, &ap_info->channel, &ap_info->rssi);
    } else {
        //printf("\tnet link: LINK_DOWN\n");
        ap_info->link_status = WIFI_STATUS_LINK_DOWN;
    }
    return 0;
}

#if 0
static int m88wi6700s_get_link_info(aos_dev_t *dev, char ip[16], char gw[16], char mask[16])
{
    struct netif *netif = netif_find("en0");

    aos_check_return_einval(netif);

    /* Show the IPv4 address */
    printf("    \tinet addr:%s\n", inet_ntoa(netif->ip_addr));
    memcpy(ip, inet_ntoa(netif->ip_addr), 16);

    /* Show the IPv4 default router address */
    printf("\tGWaddr:%s\n", inet_ntoa(netif->gw));
    memcpy(gw, inet_ntoa(netif->gw), 16);
    /* Show the IPv4 network mask */
    printf("\tMask:%s\n", inet_ntoa(netif->netmask));
    memcpy(mask, inet_ntoa(netif->netmask), 16);
    return 0;
}
#endif

static int m88wi6700s_start_scan(aos_dev_t *dev, wifi_scan_config_t *config, bool block)
{
    lynx_api_start_scan(block);
    return 0;
}


int m88wi6700s_event_cb(int evt_id)
{
    if (!g_evt_func) {
        printf("Event func not registered\n");
        return -1;
    }

    switch (evt_id) {
        case LYNX_EVT_SCAN_DONE: {
            int scan_count = lynx_api_get_scan_count();

            if (scan_count == 0) {
                printf("scan done but no scan result\n");
                return -1;
            }

            int i = 0;
            wifi_ap_record_t *ap_records = aos_zalloc(sizeof(wifi_ap_record_t) * scan_count);
            struct neighbor_bss *bss_list = lynx_api_get_scan_results();
            struct neighbor_bss *nbss;

            for (i = 0; i < scan_count; i++) {
                nbss = &bss_list[i];

                if (nbss->flag & NBSS_ENABLE) {

                    memcpy(ap_records[i].ssid, nbss->ssid, 32);
                    memcpy(ap_records[i].bssid, nbss->addr, 6);
                    ap_records[i].channel = nbss->channel;
                    ap_records[i].rssi = nbss->rssi[0];

                    if (nbss->secondary_ch == BW40MHZ_SCN) {
                        ap_records[i].second = WIFI_SECOND_CHAN_NONE;
                    } else if (nbss->secondary_ch == BW40MHZ_SCA) {
                        ap_records[i].second = WIFI_SECOND_CHAN_ABOVE;
                    } else if (nbss->secondary_ch == BW40MHZ_SCB) {
                        ap_records[i].second = WIFI_SECOND_CHAN_BELOW;
                    }

                    if (nbss->sec & NBSS_SEC_NON) {
                        ap_records[i].encryptmode = WIFI_ENC_OPEN;
                        ap_records[i].authmode = WIFI_AUTH_OPEN;
                    }

                    if (nbss->sec & NBSS_SEC_WEP) {
                        ap_records[i].encryptmode = WIFI_ENC_WEP;
                        ap_records[i].authmode = WIFI_AUTH_WEP;
                    }

                    if (nbss->sec & NBSS_SEC_WPA2) {
                        ap_records[i].encryptmode = WIFI_ENC_AES;
                        ap_records[i].authmode = WIFI_AUTH_WPA2_PSK;
                    }

                    if (nbss->sec & NBSS_SEC_WPA) {
                        ap_records[i].encryptmode = WIFI_ENC_TKIP;
                        ap_records[i].authmode = WIFI_AUTH_WPA_PSK;
                    }

                    if ((nbss->sec & NBSS_SEC_WPA) && (nbss->sec & NBSS_SEC_WPA2)) {
                        ap_records[i].encryptmode = WIFI_ENC_TKIP_AES_MIX;
                        ap_records[i].authmode = WIFI_AUTH_WPA_WPA2_PSK;
                    }

                }
            }

            g_evt_func->scan_compeleted(wifi_evt_dev, scan_count, ap_records);
            aos_free(ap_records);
            break;
        }

        default:
            break;
    }

    return 0;
}


int m88wi6700s_install_event_cb(aos_dev_t *dev, wifi_event_func *evt_func)
{

    lynx_api_reg_event_callback(m88wi6700s_event_cb);

    g_evt_func = evt_func;
    wifi_evt_dev = dev;
    return 0;
}

int m88wi6700s_set_ps(aos_dev_t *dev, wifi_ps_type_t type)
{
    if (type == WIFI_PS_MODEM)
        return lynx_api_set_ps(1);
    else if (type == WIFI_PS_NONE)
        return lynx_api_set_ps(0);
    else
        return -1;
}

int m88wi6700s_get_ps(aos_dev_t *dev, wifi_ps_type_t *type)
{
    if (lynx_api_get_ps() == true)
        *type = WIFI_PS_MODEM;
    else
        *type = WIFI_PS_NONE;
    return 0;
}


void m88wi6700s_monitor_callback(u8 *data, int len)
{

    //tcpdump_hook_raw(data, len);
    //return;
    wifi_promiscuous_pkt_t *buf = aos_malloc(sizeof(wifi_promiscuous_pkt_t) + len);
    wifi_promiscuous_pkt_type_t type = WIFI_PKT_DATA;
    struct lynx_rx_radiotap_hdr *radiotap_hdr = (struct lynx_rx_radiotap_hdr *)data;
    struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)(data + sizeof(struct lynx_rx_radiotap_hdr));


    if ((hdr->frame_control & 0xc) == IEEE80211_FTYPE_DATA) {
        type = WIFI_PKT_DATA;
    } else if ((hdr->frame_control & 0xc) == IEEE80211_FTYPE_MGMT) {
        type = WIFI_PKT_MGMT;
    } else if ((hdr->frame_control & 0xc) == IEEE80211_FTYPE_CTL) {
        type = WIFI_PKT_CTRL;
    }

    buf->rx_ctrl.rssi = radiotap_hdr->dbmsignal;
    buf->rx_ctrl.channel = radiotap_hdr->chnl_freq;

    if (radiotap_hdr->chnl_freq <= 2472) {
        buf->rx_ctrl.channel = (radiotap_hdr->chnl_freq - 2407) / 5;
    } else if (buf->rx_ctrl.channel == 2484) {
        buf->rx_ctrl.channel = 14;
    } else {
        buf->rx_ctrl.channel = 0;
    }

    buf->rx_ctrl.sig_len = len - sizeof(struct lynx_rx_radiotap_hdr);
    buf->rx_ctrl.rate = (radiotap_hdr->rate);


    memcpy(buf->payload, data + sizeof(struct lynx_rx_radiotap_hdr), len - sizeof(struct lynx_rx_radiotap_hdr));

    g_monitor_cb(buf, type);

    aos_free(buf);
}

int m88wi6700s_start_monitor(aos_dev_t *dev, wifi_promiscuous_cb_t cb)
{
    /** before switch mode, net link is brought down */
    struct netif *netif = netif_find("en0");
    netif_set_link_down(netif);
    /** stop dhcp server */
    dhcps_deinit();

    g_monitor_cb = cb;
    lynx_api_set_monitor_callback(m88wi6700s_monitor_callback);

    lynx_api_switch_mode(NL80211_IFTYPE_MONITOR);
    return 0;

}


int m88wi6700s_stop_monitor(aos_dev_t *dev)
{
    lynx_api_switch_mode(NL80211_IFTYPE_STATION);
    return 0;
}



int m88wi6700s_set_channel(aos_dev_t *dev, uint8_t primary, wifi_second_chan_t second)
{
    return lynx_api_monitor_set_channel(primary);
}


int m88wi6700s_get_channel(aos_dev_t *dev, uint8_t *primary, wifi_second_chan_t *second)
{
    *primary = lynx_api_monitor_get_channel();
    *second = WIFI_SECOND_CHAN_NONE;
    return 0;
}


/*****************************************
* common driver interface
******************************************/
static aos_dev_t *m88wi6700s_dev_init(driver_t *drv, void *config, int id)
{
    aos_dev_t *dev = device_new(drv, sizeof(wifi_dev_t), id);

    return dev;
}

static void m88wi6700s_dev_uninit(aos_dev_t *dev)
{
    aos_check_param(dev);

    device_free(dev);
}

static int m88wi6700s_dev_open(aos_dev_t *dev)
{
    // power on device

    return 0;
}

static int m88wi6700s_dev_close(aos_dev_t *dev)
{
    //power off device
    return 0;
}


static net_ops_t m88wi6700s_net_driver = {
    .set_mac_addr = NULL,
    .get_mac_addr = m88wi6700s_get_mac_addr,
    .start_dhcp   = m88wi6700s_start_dhcp,
    .stop_dhcp    = m88wi6700s_stop_dhcp,
    .set_ipaddr   = m88wi6700s_set_ipaddr,
    .get_ipaddr   = m88wi6700s_get_ipaddr,
    .subscribe    = m88wi6700s_subscribe,
    .ping         = NULL,
};


static wifi_driver_t m88wi6700s_wifi_driver = {
    /** basic APIs */

    .init               = m88wi6700s_init,
    .deinit             = m88wi6700s_deinit,
    .reset              = m88wi6700s_reset,
    .set_mode           = m88wi6700s_set_mode,
    .get_mode           = m88wi6700s_get_mode,
    .install_event_cb   = m88wi6700s_install_event_cb,


    /** conf APIs */
    .set_protocol       = NULL,
    .get_protocol       = NULL,
    .set_country        = NULL,
    .get_country        = NULL,
    .set_mac_addr       = NULL,
    .get_mac_addr       = m88wi6700s_get_mac_addr,
    .set_auto_reconnect = NULL,
    .get_auto_reconnect = NULL,
    .set_ps             = m88wi6700s_set_ps,
    .get_ps             = m88wi6700s_get_ps,
    .power_on           = NULL,
    .power_off          = NULL,


    /** connection APIs */
    .start_scan             = m88wi6700s_start_scan,
    .start                  = m88wi6700s_start,
    .stop                   = m88wi6700s_stop,
    .sta_get_link_status    = m88wi6700s_drv_get_link_status,
    .ap_get_sta_list        = NULL,

    /** promiscuous APIs */
    .start_monitor          = m88wi6700s_start_monitor,
    .stop_monitor           = m88wi6700s_stop_monitor,
    .send_80211_raw_frame   = NULL,
    .set_channel    = m88wi6700s_set_channel,
    .get_channel    = m88wi6700s_get_channel,

    .set_smartcfg       = NULL,
};

static netdev_driver_t m88wi6700s_driver = {
    .drv =
    {
        .name   = "wifi",
        .init   = m88wi6700s_dev_init,
        .uninit = m88wi6700s_dev_uninit,
        .open   = m88wi6700s_dev_open,
        .close  = m88wi6700s_dev_close,
    },
    .link_type = NETDEV_TYPE_WIFI,
    .net_ops  = &m88wi6700s_net_driver,
    .link_ops  = &m88wi6700s_wifi_driver,
};

void wifi_m88wi6700s_register(void)
{
    //run eth_dev_init to create eth_dev_t and bind this driver
    driver_register(&m88wi6700s_driver.drv, NULL, 0);
    tcpip_init(NULL, NULL);

    extern void sdio_int_lanqi(void);
    sdio_int_lanqi();

}
