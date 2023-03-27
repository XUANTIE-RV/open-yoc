#include "wm_include.h"
#include "tls_ieee80211.h"
#include "tls_wireless.h"

#ifdef bool
#undef bool
#endif

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "pinmux.h"
#include <drv/irq.h>
#include <aos/aos.h>
#include <aos/debug.h>
//#include <uservice/uservice.h>
#include <devices/netdrv.h>

#include <lwip/apps/dhcps.h>
#include <lwip/netifapi.h>
#include <lwip/netif.h>
#include <lwip/dns.h>
#include "netif/etharp.h"
#include <netif/ethernet.h>
#include <devices/wifi.h>
#include <devices/impl/wifi_impl.h>

#include "drv/gpio.h"

#include "w800_devops.h"

typedef struct {
    rvm_dev_t   device;
    uint8_t mode;
    void (*write_event)(rvm_dev_t *dev, int event_id, void *priv);
    void *priv;
} wifi_dev_t;

#define TAG "WIFI"

static uint8_t g_wifi_auto_reconnect = 0;
static int g_is_wifi_user_disconnect = 0;
static gpio_pin_handle_t    *smartcfg_pin = NULL;
static rvm_hal_wifi_lpm_mode_t       g_wifi_lpm_mode = WIFI_LPM_NONE;
static rvm_hal_wifi_promiscuous_cb_t g_monitor_cb;
static rvm_hal_wifi_mgnt_cb_t        g_monitor_mgnt_cb;
static rvm_hal_wifi_event_func *     g_evt_func;
static rvm_dev_t *           wifi_evt_dev;

struct netif w800_netif[2];
static uint8_t g_wifi_got_ip = 0;
static tls_os_sem_t *g_wifi_scan_sem = NULL;
static wifi_scan_config_t *g_wifi_scan_config;

#define FW_MAJOR_VER           0x03
#define FW_MINOR_VER           0x04
#define FW_PATCH_VER           0x05

const char FirmWareVer[4] = {
	'G',
	FW_MAJOR_VER,  /* Main version */
	FW_MINOR_VER, /* Subversion */
	FW_PATCH_VER  /* Internal version */
	};
const char HwVer[6] = {
	'H',
	0x1,
	0x0,
	0x0,
	0x0,
	0x0
};

extern void w800_board_init(void);
extern uint8_t tx_gain_group[];
extern int hed_rf_current_channel;
extern void *tls_wl_init(u8 *tx_gain, u8* mac_addr, u8 *hwver);
extern int wpa_supplicant_init(u8 *mac_addr);
extern void wpa_supplicant_set_mac(u8 *mac);
extern u8 *wpa_supplicant_get_mac(void);
extern u8 *hostapd_get_mac(void);
extern u8* tls_wifi_buffer_acquire(int total_len);
extern void tls_wifi_buffer_release(bool is_apsta, u8* buffer);
extern void tls_wifi_set_tempcomp_flag(int flag);
extern void tls_wl_plcp_cb_register(tls_wifi_data_recv_callback callback);
extern void tls_wl_plcp_stop(void);
extern void tls_wl_plcp_start(void);
extern ATTRIBUTE_ISR void MAC_IRQHandler(void);
extern ATTRIBUTE_ISR void SEC_IRQHandler(void);
extern ATTRIBUTE_ISR void PMU_RTC_IRQHandler(void);

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

static void low_level_init(struct netif *netif)
{
    u8 mac_addr[6];

    netif->hwaddr_len = ETH_ALEN;
    tls_get_mac_addr(&mac_addr[0]);

    if (netif == &w800_netif[0])
    {
        memcpy(&netif->hwaddr[0], mac_addr, ETH_ALEN);
    }
    else
    {
        mac_addr[0] = mac_addr[0] + 2;
        memcpy(&netif->hwaddr[0], mac_addr, ETH_ALEN);
    }

    /* Maximum transfer unit */
    netif->mtu = 1500;
 	
#if  LWIP_IPV6_AUTOCONFIG  	
		netif_set_ip6_autoconfig_enabled(netif, 1);
#endif
    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
        NETIF_FLAG_IGMP  
#if defined(DHCP_USED)
        | NETIF_FLAG_DHCP
#endif
        ;

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
}

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
	struct pbuf *q = NULL;
	int datalen = 0;
	u8* buf = tls_wifi_buffer_acquire(p->tot_len);
	if(buf == NULL)
		return ERR_MEM;

#if ETH_PAD_SIZE
	pbuf_header(p, -ETH_PAD_SIZE);    /* Drop the padding word */
#endif

	/* Check the buffer boundary */
	//if (p->tot_len > NET_RW_BUFF_SIZE) {
	//	return ERR_BUF;
	//}
	
	for (q = p; q != NULL; q = q->next) {
		/* Send data from(q->payload, q->len); */
		memcpy(buf + datalen, q->payload, q->len);
		datalen += q->len;
	}

    if (netif != &w800_netif[0])
	    tls_wifi_buffer_release(TRUE, buf);
	else
	    tls_wifi_buffer_release(FALSE, buf);

#if ETH_PAD_SIZE
	pbuf_header(p, ETH_PAD_SIZE);    /* Reclaim the padding word */
#endif

	LINK_STATS_INC(link.xmit);

	return ERR_OK;
}

static struct pbuf *low_level_input(struct netif *netif, u8 *buf, u32 buf_len)
{
    struct pbuf *p = NULL, *q = NULL;
    u16_t s_len;
    u8_t *bufptr;

    /* Obtain the size of the packet and put it into the "len"
     * variable. */

    s_len = buf_len; 
    bufptr = buf;

#if ETH_PAD_SIZE
    s_len += ETH_PAD_SIZE;    /* allow room for Ethernet padding */
#endif

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, s_len, PBUF_RAM);

    if (p != NULL) {
#if ETH_PAD_SIZE
        pbuf_header(p, -ETH_PAD_SIZE);  /* drop the padding word */
#endif

        /* Iterate over the pbuf chain until we have read the entire
         * packet into the pbuf. */
        for (q = p; q != NULL; q = q->next) {
            /* Read enough bytes to fill this pbuf in the chain. The
             * available data in the pbuf is given by the q->len
             * variable. */
            /* read data into(q->payload, q->len); */
            memcpy(q->payload, bufptr, q->len);
            bufptr += q->len;
        }
        /* Acknowledge that packet has been read(); */

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE);    /* Reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    } else {
        /* Drop packet(); */
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
    }

    return p;
}

static err_t w800_netif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "w800-wifi";
#endif /* LWIP_NETIF_HOSTNAME */

  //MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, NET_LINK_SPEED);

  netif->output = etharp_output;
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
  netif->linkoutput = low_level_output;
  
  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

static void w800_netif_status_changed(struct netif *netif)
{
    //LOGD(TAG, "netif status %d, %d", netif_is_link_up(netif), netif_is_up(netif));

	if (netif_is_link_up(netif))
	{
		if (!ip_addr_isany(netif_ip4_addr(netif)))
        {
            g_wifi_got_ip = 1;
            event_publish(EVENT_NET_GOT_IP, NULL);

            bool enable = FALSE;
            tls_param_get(TLS_PARAM_ID_PSM, &enable, TRUE);		
            tls_wifi_set_psflag(enable, FALSE);
        }
	}
	else
	{
        g_wifi_got_ip = 0;
	}
}

static int w800_netif_input(const u8 *bssid, u8 *buf, u32 buf_len)
{
    struct netif    *netif = &w800_netif[0];
    struct pbuf       *p;

    u8* mac_addr = hostapd_get_mac();
    if (0 == compare_ether_addr(bssid, mac_addr))
    {
        netif = &w800_netif[1];
    }

    /* move received packet into a new pbuf */
    p = low_level_input(netif, buf, buf_len);
    if (p) {
        if (ERR_OK != netif->input(p, netif)) {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
        return 0;
    } else {
        return -1;
    }
}

static void w800_lwip_init(void)
{
    tcpip_init(NULL, NULL);

    w800_netif[0].name[0] = 'w';
    w800_netif[0].name[1] = '0';
    netifapi_netif_add(&w800_netif[0], IPADDR_ANY, IPADDR_ANY, IPADDR_ANY, NULL, w800_netif_init, tcpip_input);
    netif_set_status_callback(&w800_netif[0], w800_netif_status_changed);

    w800_netif[1].name[0] = 'w';
    w800_netif[1].name[1] = '1';
    netifapi_netif_add(&w800_netif[1], IPADDR_ANY, IPADDR_ANY, IPADDR_ANY, NULL, w800_netif_init, tcpip_input);
    //netif_set_status_callback(&w800_netif[1], w800_netif_status_changed);

    netifapi_netif_set_default(&w800_netif[0]);

    netifapi_netif_set_up(&w800_netif[0]);
    netifapi_netif_set_up(&w800_netif[1]);
}

static void wm_wlan_auto_reconnect(void)
{
    struct tls_param_ssid ssid;
    struct tls_param_original_key origin_key;
    struct tls_param_bssid bssid;
    //u8 auto_reconnect = 0xff;
    u8 wireless_protocol = 0;

    //tls_wifi_auto_connect_flag(WIFI_AUTO_CNT_FLAG_GET, &auto_reconnect);
    //if (auto_reconnect != WIFI_AUTO_CNT_ON)
    if (!g_wifi_auto_reconnect)
    {
        return;
    }
    else
    {
        if (g_is_wifi_user_disconnect)
    	    return;
    }

    tls_param_get(TLS_PARAM_ID_WPROTOCOL, (void*) &wireless_protocol, TRUE);
    switch (wireless_protocol)
    {
        case TLS_PARAM_IEEE80211_INFRA:
#if TLS_CONFIG_AP
            case (TLS_PARAM_IEEE80211_INFRA | TLS_PARAM_IEEE80211_SOFTAP):
#endif
            {
                LOGI(TAG, "wifi auto connect");

                tls_param_get(TLS_PARAM_ID_BSSID, (void*) &bssid, TRUE);
                tls_param_get(TLS_PARAM_ID_SSID, (void*) &ssid, TRUE);				
                tls_param_get(TLS_PARAM_ID_ORIGIN_KEY, (void*) &origin_key, TRUE);

                if (bssid.bssid_enable)
                {
                    tls_wifi_connect_by_ssid_bssid(ssid.ssid, ssid.ssid_len, 
                                                   bssid.bssid, origin_key.psk,
                                                   origin_key.key_length);
                }
                else
                {
                    tls_wifi_connect(ssid.ssid, ssid.ssid_len, 
                                     origin_key.psk, origin_key.key_length);
                }
            }
            break;
        default:
            break;
    }

    return ;
}

static void wm_wlan_client_event(u8 *mac, enum tls_wifi_client_event_type event)
{
    LOGD(TAG, "sta "MACSTR" %s", MAC2STR(mac), event ? "offline" : "online");
}

static void wm_wlan_status_changed(uint8_t status)
{
    //LOGD(TAG, "status:%d", status);

    switch (status)
    {
        case WIFI_JOIN_SUCCESS:
            LOGD(TAG, "wifi connected");
            netif_set_link_up(&w800_netif[0]);
            event_publish(EVENT_WIFI_LINK_UP, NULL);
            break;

        case WIFI_JOIN_FAILED:
            LOGD(TAG, "wifi failed");
            netif_set_link_down(&w800_netif[0]);
            event_publish(EVENT_WIFI_LINK_DOWN, NULL);
            wm_wlan_auto_reconnect();
            break;

        case WIFI_DISCONNECTED:
            LOGD(TAG, "wifi disconnected");
            netif_set_link_down(&w800_netif[0]);
            event_publish(EVENT_WIFI_LINK_DOWN, NULL);
            wm_wlan_auto_reconnect();
            break;

        case WIFI_SOFTAP_SUCCESS:
            LOGD(TAG, "softap created");
            ip_addr_t     ipaddr;
            ip_addr_t     netmask;
            ip_addr_t     gw;
            ip_addr_t     ipaddr_pool_start;
            ip_addr_t     ipaddr_pool_end;
            IP4_ADDR(&ipaddr, 192, 168, 1, 1);
            IP4_ADDR(&netmask, 255, 255, 255, 0);
            IP4_ADDR(&gw, 192, 168, 1, 1);
            netif_set_addr(&w800_netif[1], &ipaddr, &netmask, &gw);
            dhcps_deinit();
            IP4_ADDR(&ipaddr_pool_start, 192, 168, 1, 100);
            IP4_ADDR(&ipaddr_pool_end, 192, 168, 1, 107); /** limited to only 8 client */
            dhcps_set_addr_pool(1, &ipaddr_pool_start, &ipaddr_pool_end);
            dhcps_init(&w800_netif[1]);
            netif_set_link_up(&w800_netif[1]);
            break;

        case WIFI_SOFTAP_FAILED:
            LOGD(TAG, "softap failed");
            break;

        case WIFI_SOFTAP_CLOSED:
            LOGD(TAG, "softap closed");
            netif_set_link_down(&w800_netif[1]);
            dhcps_deinit();
            break;

        default:
            break;
    }
}

static void wm_wlan_scan_callback(void)
{
    int err;
    int buflen = 2000;
    char *buf = NULL;
    uint16_t number = 0;
    u8 *buf2;
    int i;
    rvm_hal_wifi_ap_record_t *ap_records;

    if (g_wifi_scan_sem)
        tls_os_sem_release(g_wifi_scan_sem);

    if (g_evt_func)
    {
        if (g_evt_func->scan_compeleted)
        {
            buf = malloc(buflen);
            if (NULL == buf)
            {
                LOGE(TAG, "scan malloc failed...");
                return;
            }

            err = tls_wifi_get_scan_rslt((u8 *)buf, buflen);
            if (err != WM_SUCCESS)
            {
                free(buf);
                return;
            }

            struct tls_scan_bss_t *scan_res = (struct tls_scan_bss_t *)buf;
            struct tls_bss_info_t *bss_info = (struct tls_bss_info_t *)scan_res->bss;

            buf2 = malloc(scan_res->count * sizeof(rvm_hal_wifi_ap_record_t));
            if (!buf2)
            {
                LOGE(TAG, "scan malloc failed...");
                free(buf);
                return;
            }

            memset(buf2, 0, scan_res->count * sizeof(rvm_hal_wifi_ap_record_t));

            ap_records = (rvm_hal_wifi_ap_record_t *)buf2;

            for (i = 0; i < scan_res->count; i ++)
            {
                if (g_wifi_scan_config)
                {
                    if (strncmp((char *)g_wifi_scan_config->ssid, (char *)bss_info->ssid, bss_info->ssid_len) ||
                        strlen((char *)g_wifi_scan_config->ssid) != bss_info->ssid_len)
                        continue;
                    if (memcmp(g_wifi_scan_config->bssid, bss_info->bssid, 6))
                        continue;
                    if (g_wifi_scan_config->channel != bss_info->channel)
                        continue;
                    if (!g_wifi_scan_config->show_hidden && !bss_info->ssid_len)
                        continue;
                }

                ap_records->link_status = WIFI_STATUS_LINK_DOWN;

                if (WM_WIFI_DISCONNECTED != tls_wifi_get_state())
                {
                    struct tls_curr_bss_t currbss;
                    tls_wifi_get_current_bss(&currbss);
                    if (!compare_ether_addr(currbss.bssid, bss_info->bssid))
                    {
                        if (g_wifi_got_ip)
                            ap_records->link_status = WIFI_STATUS_GOTIP;
                        else
                            ap_records->link_status = WIFI_STATUS_LINK_UP;
                    }
                }

                memcpy(ap_records->bssid, bss_info->bssid, 6);

                memcpy(ap_records->ssid, bss_info->ssid, bss_info->ssid_len);
                ap_records->ssid[bss_info->ssid_len] = '\0';

                ap_records->channel = bss_info->channel;
                ap_records->rssi = -(char)(0x100 - bss_info->rssi);
                ap_records->second = WIFI_SECOND_CHAN_NONE;

                if (((WM_WIFI_AUTH_MODE_WPA_PSK_TKIP & bss_info->privacy) && (WM_WIFI_AUTH_MODE_WPA_PSK_CCMP & bss_info->privacy)) ||
                    ((WM_WIFI_AUTH_MODE_WPA2_PSK_TKIP & bss_info->privacy) && (WM_WIFI_AUTH_MODE_WPA2_PSK_CCMP & bss_info->privacy)))
                {
                    ap_records->encryptmode = WIFI_ENC_TKIP_AES_MIX;
                }
                else if ((WM_WIFI_AUTH_MODE_WPA_PSK_CCMP & bss_info->privacy) || (WM_WIFI_AUTH_MODE_WPA2_PSK_CCMP & bss_info->privacy))
                {
                    ap_records->encryptmode = WIFI_ENC_AES;
                }
                else if ((WM_WIFI_AUTH_MODE_WPA_PSK_TKIP & bss_info->privacy) || (WM_WIFI_AUTH_MODE_WPA2_PSK_TKIP & bss_info->privacy))
                {
                    ap_records->encryptmode = WIFI_ENC_TKIP;
                }
                if (WM_WIFI_AUTH_MODE_WEP_AUTO & bss_info->privacy)
                {
                    ap_records->encryptmode = WIFI_ENC_WEP;
                }

                if ((WM_WIFI_AUTH_MODE_WPA_PSK_AUTO & bss_info->privacy) && 
                    (WM_WIFI_AUTH_MODE_WPA2_PSK_AUTO & bss_info->privacy))
                {
                    ap_records->authmode = WIFI_AUTH_WPA_WPA2_PSK;
                }
                else if (WM_WIFI_AUTH_MODE_WPA2_PSK_AUTO & bss_info->privacy)
                {
                    ap_records->authmode = WIFI_AUTH_WPA2_PSK;
                }
                else if (WM_WIFI_AUTH_MODE_WPA_PSK_AUTO & bss_info->privacy)
                {
                    ap_records->authmode = WIFI_AUTH_WPA_PSK;
                }

                if (WM_WIFI_AUTH_MODE_OPEN == bss_info->privacy)
                {
                    ap_records->encryptmode = WIFI_ENC_OPEN;
                    ap_records->authmode = WIFI_AUTH_OPEN;
                }
                if (WM_WIFI_AUTH_MODE_UNKNOWN == bss_info->privacy)
                {
                    ap_records->encryptmode = WIFI_ENC_MAX;
                    ap_records->authmode = WIFI_AUTH_MAX;
                }

                bss_info++;
                ap_records++;
                number++;
            }

            free(buf);

            g_evt_func->scan_compeleted(wifi_evt_dev, number, (rvm_hal_wifi_ap_record_t *)buf2);

            free(buf2);
        }
    }
}

static void wm_wlan_data_recv_callback(u8 *data, u32 data_len)
{
    struct ieee80211_hdr *hdr;
    rvm_hal_wifi_promiscuous_pkt_type_t type = WIFI_PKT_MISC;
    rvm_hal_wifi_promiscuous_pkt_t *buf;

    buf = malloc(sizeof(rvm_hal_wifi_promiscuous_pkt_t) + data_len);
    if (!buf)
        return;

    hdr = (struct ieee80211_hdr *)data;

    if (ieee80211_is_ctl(hdr->frame_control))
        type = WIFI_PKT_CTRL;
    else if (ieee80211_is_mgmt(hdr->frame_control))
        type = WIFI_PKT_MGMT;
    else if (ieee80211_is_data(hdr->frame_control))
        type = WIFI_PKT_DATA;

    memset(buf, 0, sizeof(rvm_hal_wifi_promiscuous_pkt_t) + data_len);
    buf->rx_ctrl.rssi = -30; //-(char)(0x100 - ext->rssi);
    buf->rx_ctrl.sig_len = data_len;
    memcpy(buf->payload, data, data_len);

    if (g_monitor_cb)
        g_monitor_cb(buf, type);

    free(buf);
}

static void wm_wlan_mgmt_recv_callback(u8 *data, u32 data_len, struct tls_wifi_ext_t *ext)
{
#define CRC_LEN (4)
    struct ieee80211_hdr *hdr;
    rvm_hal_wifi_promiscuous_pkt_type_t type = WIFI_PKT_MISC;
    rvm_hal_wifi_promiscuous_pkt_t *buf;

    buf = malloc(sizeof(rvm_hal_wifi_promiscuous_pkt_t) + data_len + CRC_LEN);
    if (!buf)
        return;

    hdr = (struct ieee80211_hdr *)data;

    if (ieee80211_is_ctl(hdr->frame_control))
        type = WIFI_PKT_CTRL;
    else if (ieee80211_is_mgmt(hdr->frame_control))
        type = WIFI_PKT_MGMT;
    else if (ieee80211_is_data(hdr->frame_control))
        type = WIFI_PKT_DATA;

    memset(buf, 0, sizeof(rvm_hal_wifi_promiscuous_pkt_t) + data_len + CRC_LEN);
    buf->rx_ctrl.rssi = -30; //-(char)(0x100 - ext->rssi);
    buf->rx_ctrl.sig_len = data_len + CRC_LEN;
    memcpy(buf->payload, data, data_len + CRC_LEN);

    if (g_monitor_cb)
        g_monitor_cb(buf, type);

    free(buf);
}

static int w800_set_mac_addr(rvm_dev_t *dev, const uint8_t *mac)
{
    wpa_supplicant_set_mac((u8 *)mac);
    tls_set_mac_addr((u8 *)mac);

    return WIFI_ERR_OK;
}

static int w800_get_mac_addr(rvm_dev_t *dev, uint8_t *mac)
{
    memcpy(mac, wpa_supplicant_get_mac(), 6);

    return WIFI_ERR_OK;
}

static int w800_set_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    int n, i;

    n = MIN(num, DNS_MAX_SERVERS);

    for (i = 0; i < n; i ++) {
        dns_setserver(i, &ipaddr[i]);
    }

    return n;
}

static int w800_get_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    int n, i;

    n = MIN(num, DNS_MAX_SERVERS);

    for (i = 0; i < n; i ++) {
        if (!ip_addr_isany(dns_getserver(i))) {
            memcpy(&ipaddr[i], dns_getserver(i), sizeof(ip_addr_t));
        } else {
            return i;
        }
    }

    return n;
}

static int w800_set_hostname(rvm_dev_t *dev, const char *name)
{
#if LWIP_NETIF_HOSTNAME
    struct netif *netif = &w800_netif[0];
    netif_set_hostname(netif, name);
    return 0;
#else
    return -1;
#endif
}

static const char *w800_get_hostname(rvm_dev_t *dev)
{
#if LWIP_NETIF_HOSTNAME
    struct netif *netif = &w800_netif[0];
    return netif_get_hostname(netif);
#else
    return NULL;
#endif
}

static int w800_set_link_up(rvm_dev_t *dev)
{
    //netif_set_link_up(&w800_netif[0]);
    return 0;
}

static int w800_set_link_down(rvm_dev_t *dev)
{
    //netif_set_link_down(&w800_netif[0]);
    return 0;
}

static int w800_start_dhcp(rvm_dev_t *dev)
{
    struct netif *netif = &w800_netif[0];
    aos_check_return_einval(netif);

    if (!netif_is_link_up(netif)) {
        return -1;
    }

    netif_set_ipaddr(netif, NULL);
    netif_set_netmask(netif, NULL);
    netif_set_gw(netif, NULL);

    return netifapi_dhcp_start(netif);
}

static int w800_stop_dhcp(rvm_dev_t *dev)
{
    struct netif *netif = &w800_netif[0];
    aos_check_return_einval(netif);

    netifapi_dhcp_stop(netif);

    return 0;
}

static int w800_set_ipaddr(rvm_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask,
                           const ip_addr_t *gw)
{
    struct netif *netif = &w800_netif[0];

    netifapi_dhcp_stop(netif);

    netif_set_ipaddr(netif, ipaddr);
    netif_set_netmask(netif, netmask);
    netif_set_gw(netif, gw);

    w800_netif_status_changed(netif);

    return 0;
}

static int w800_get_ipaddr(rvm_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw)
{
    struct netif *netif = &w800_netif[0];
    aos_check_return_einval(netif && ipaddr && netmask && gw);

    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(ipaddr), *netif_ip_addr4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(gw), *netif_ip_gw4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(netmask), *netif_ip_netmask4(netif));

    return 0;
}

static int w800_ping_remote(rvm_dev_t *dev, int type, char *remote_ip)
{
    return -1;
}

static int w800_subscribe(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    return -1;
}

static int w800_unsubscribe(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    return -1;
}

static net_ops_t w800_net_driver = {
    .set_mac_addr   = w800_set_mac_addr,
    .get_mac_addr   = w800_get_mac_addr,

    .set_dns_server = w800_set_dns_server,
    .get_dns_server = w800_get_dns_server,

    .set_hostname   = w800_set_hostname,
    .get_hostname   = w800_get_hostname,

    .set_link_up    = w800_set_link_up,
    .set_link_down  = w800_set_link_down,

    .start_dhcp     = w800_start_dhcp,
    .stop_dhcp      = w800_stop_dhcp,
    .set_ipaddr     = w800_set_ipaddr,
    .get_ipaddr     = w800_get_ipaddr,
    .ping           = w800_ping_remote,

    .subscribe      = w800_subscribe,
    .unsubscribe    = w800_unsubscribe,
};

static int w800_init(rvm_dev_t *dev)
{
    static uint8_t wifi_inited = 0;
    uint8_t enable = 0;
    uint8_t mac_addr[6] = {0x00, 0x25, 0x08, 0x09, 0x01, 0x0F};

    if (!wifi_inited)
    {
        tls_get_tx_gain(&tx_gain_group[0]);

        if (tls_wifi_mem_cfg(WIFI_MEM_START_ADDR, 7, 7))
        {
            LOGE(TAG, "wl mem initial failured");
            return WIFI_ERR_FAIL;
        }

        tls_get_mac_addr(&mac_addr[0]);

        drv_irq_register(MAC_IRQn, MAC_IRQHandler);
        drv_irq_register(SEC_IRQn, SEC_IRQHandler);
        drv_irq_register(PMU_IRQn, PMU_RTC_IRQHandler);

        if (tls_wl_init(NULL, &mac_addr[0], NULL) == NULL)
        {
            LOGI(TAG, "wl driver initial failured");
            return WIFI_ERR_FAIL;
        }

        if (wpa_supplicant_init(mac_addr))
        {
            LOGI(TAG, "supplicant initial failured");
            return WIFI_ERR_FAIL;
        }

        tls_wifi_set_tempcomp_flag(0);

        tls_wifi_enable_log(false);

        tls_param_get(TLS_PARAM_ID_PSM, &enable, TRUE);	
    	if (enable != FALSE)
    	{
    	    enable = FALSE;
    	    tls_param_set(TLS_PARAM_ID_PSM, &enable, TRUE);
    	    g_wifi_lpm_mode = WIFI_LPM_KEEP_LINK;
    	}

        tls_wifi_status_change_cb_register(wm_wlan_status_changed);
        tls_wifi_softap_client_event_register(wm_wlan_client_event);
        tls_ethernet_data_rx_callback(w800_netif_input);

        wifi_inited = 1;
    }

    return WIFI_ERR_OK;
}

static int w800_deinit(rvm_dev_t *dev)
{
    tls_wifi_disconnect();
    tls_wifi_softap_destroy();

    return WIFI_ERR_OK;
}

static int w800_reset(rvm_dev_t *dev)
{
    tls_wifi_disconnect();
    tls_wifi_softap_destroy();

    return WIFI_ERR_OK;
}

static int w800_set_mode(rvm_dev_t *dev, rvm_hal_wifi_mode_t mode)
{
    u8 wmode = IEEE80211_MODE_INFRA;
    if (mode == WIFI_MODE_STA)
        wmode = IEEE80211_MODE_INFRA;
    else if (mode == WIFI_MODE_AP)
        wmode = IEEE80211_MODE_AP;
    else if (mode == WIFI_MODE_APSTA)
        wmode = IEEE80211_MODE_INFRA | IEEE80211_MODE_AP;
    else
        return WIFI_ERR_ARG;

    tls_param_set(TLS_PARAM_ID_WPROTOCOL, (void *)&wmode, TRUE);

    w800_init(dev);

    return WIFI_ERR_OK;
}

static int w800_get_mode(rvm_dev_t *dev, rvm_hal_wifi_mode_t *mode)
{
    u8 wmode = IEEE80211_MODE_INFRA;

    tls_param_get(TLS_PARAM_ID_WPROTOCOL, (void *)&wmode, TRUE);

    if (wmode == IEEE80211_MODE_INFRA)
        *mode = WIFI_MODE_STA;
    else if (wmode == IEEE80211_MODE_AP)
        *mode = WIFI_MODE_AP;
    else if (wmode == (IEEE80211_MODE_INFRA | IEEE80211_MODE_AP))
        *mode = WIFI_MODE_APSTA;
    else
        *mode = WIFI_MODE_NULL;

    return WIFI_ERR_OK;
}

static int w800_install_event_cb(rvm_dev_t *dev, rvm_hal_wifi_event_func *evt_func)
{
    g_evt_func   = evt_func;
    wifi_evt_dev = dev;

    return WIFI_ERR_OK;
}

static int w800_set_protocol(rvm_dev_t *dev, uint8_t protocol_bitmap)
{
    struct tls_param_bgr bgr;

    tls_param_get(TLS_PARAM_ID_WBGR, (void *)&bgr, TRUE);

    if (protocol_bitmap & (WIFI_PROTOCOL_11N)) {
        bgr.bg = 2;
        bgr.max_rate = 28;
    } else if (protocol_bitmap & (WIFI_PROTOCOL_11G)) {
        bgr.bg = 0;
        bgr.max_rate = 11;
    } else if (protocol_bitmap & (WIFI_PROTOCOL_11B)) {
        bgr.bg = 1;
        bgr.max_rate = 3;
    }

    tls_param_set(TLS_PARAM_ID_WBGR, (void *)&bgr, TRUE);

    return WIFI_ERR_OK;
}

static int w800_get_protocol(rvm_dev_t *dev, uint8_t *protocol_bitmap)
{
    struct tls_param_bgr bgr;

    tls_param_get(TLS_PARAM_ID_WBGR, (void *)&bgr, TRUE);

    switch (bgr.bg) {
        case 1:
            *protocol_bitmap = (WIFI_PROTOCOL_11B);
            break;

        case 0:
            *protocol_bitmap = (WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G);
            break;

        case 2:
            *protocol_bitmap = (WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
            break;
    }

    return WIFI_ERR_OK;
}

static int w800_set_country(rvm_dev_t *dev, rvm_hal_wifi_country_t country)
{
    u8 region = (u8)country;

    tls_param_set(TLS_PARAM_ID_COUNTRY_REGION, (void *)&region, TRUE);

    return WIFI_ERR_OK;
}

static int w800_get_country(rvm_dev_t *dev, rvm_hal_wifi_country_t *country)
{
    u8 region = WIFI_COUNTRY_CN;

    tls_param_get(TLS_PARAM_ID_COUNTRY_REGION, (void *)&region, TRUE);

    *country = region;

    return WIFI_ERR_OK;
}

static int w800_set_auto_reconnect(rvm_dev_t *dev, bool en)
{
#if 0
    u8 mode;

    if (en)
    {
        mode = WIFI_AUTO_CNT_ON;
    }
    else
    {
        mode = WIFI_AUTO_CNT_OFF;
    }

    tls_wifi_auto_connect_flag(WIFI_AUTO_CNT_FLAG_SET, &mode);
#endif

    g_wifi_auto_reconnect = en;

    return WIFI_ERR_OK;
}

static int w800_get_auto_reconnect(rvm_dev_t *dev, bool *en)
{
#if 0
    u8 mode;

    tls_wifi_auto_connect_flag(WIFI_AUTO_CNT_FLAG_GET, &mode);

    if (WIFI_AUTO_CNT_ON == mode)
        *en = 1;
    else
        *en = 0;
#endif

    *en = g_wifi_auto_reconnect;

    return WIFI_ERR_OK;
}

static int w800_set_lpm(rvm_dev_t *dev, rvm_hal_wifi_lpm_mode_t mode)
{
    tls_wifi_set_psflag(mode != WIFI_LPM_NONE ? TRUE : FALSE, FALSE);

    g_wifi_lpm_mode = mode;

    return WIFI_ERR_OK;
}

static int w800_get_lpm(rvm_dev_t *dev, rvm_hal_wifi_lpm_mode_t *mode)
{
    *mode = g_wifi_lpm_mode;
    return WIFI_ERR_OK;
}

static int w800_power_on(rvm_dev_t *dev)
{
    tls_wl_if_ps(1);
    return WIFI_ERR_OK;
}

static int w800_power_off(rvm_dev_t *dev)
{
    tls_wl_if_ps(0);
    return WIFI_ERR_OK;
}

static int w800_start_scan(rvm_dev_t *dev, wifi_scan_config_t *config, bool block)
{
    int ret;

    if (g_wifi_scan_sem)
        return WIFI_ERR_FAIL;

    w800_init(dev);

    if (block)
    {
        ret = tls_os_sem_create(&g_wifi_scan_sem, 0);
        if (TLS_OS_SUCCESS != ret)
            return WIFI_ERR_NO_MEM;
    }

    if (config)
    {
        if (g_wifi_scan_config)
            free(g_wifi_scan_config);
        g_wifi_scan_config = malloc(sizeof(wifi_scan_config_t));
        if (!g_wifi_scan_config)
        {
            if (g_wifi_scan_sem)
            {
                tls_os_sem_delete(g_wifi_scan_sem);
                g_wifi_scan_sem = NULL;
            }
            return WIFI_ERR_NO_MEM;
        }
        memcpy(g_wifi_scan_config, config, sizeof(wifi_scan_config_t));
    }

    tls_wifi_scan_result_cb_register(wm_wlan_scan_callback);

    while (WM_SUCCESS != tls_wifi_scan())
	{
	    LOGD(TAG, "wscan failed, retrying...");
        tls_os_time_delay(HZ / 10);
    }

    if (block)
    {
        ret = tls_os_sem_acquire(g_wifi_scan_sem, 0);
        tls_wifi_scan_result_cb_register(NULL);
        tls_os_sem_delete(g_wifi_scan_sem);
        g_wifi_scan_sem = NULL;
        if (TLS_OS_SUCCESS != ret)
        {
    	    if (g_wifi_scan_config)
    	    {
    	        free(g_wifi_scan_config);
    	        g_wifi_scan_config = NULL;
    	    }
            return WIFI_ERR_FAIL;
        }
    }

    return WIFI_ERR_OK;
}

static int w800_start_sta(rvm_hal_wifi_config_t *config)
{
    int ret;
    u8 channel_en;
    u8 channel;

    tls_param_get(TLS_PARAM_ID_CHANNEL_EN, (void *)&channel_en, TRUE);

    if (config->sta_config.present)
    {
        if (config->sta_config.channel)
        {
            tls_param_get(TLS_PARAM_ID_CHANNEL, (void *)&channel, TRUE);
            if (!channel_en || (channel != config->sta_config.channel))
            {
                channel_en = 1;
                channel = config->sta_config.channel;
                tls_param_set(TLS_PARAM_ID_CHANNEL_EN, (void *)&channel_en, TRUE);
                tls_param_set(TLS_PARAM_ID_CHANNEL, (void *)&channel, TRUE);
            }
        }

        if (!is_zero_ether_addr((u8 *)config->sta_config.bssid))
        {
            ret = tls_wifi_connect_by_bssid((u8 *)config->sta_config.bssid, (u8 *)config->password, strlen(config->password));
        }
        else
        {
            ret = tls_wifi_connect((u8 *)config->ssid, strlen(config->ssid), (u8 *)config->password, strlen(config->password));
        }
    }
    else
    {
        if (channel_en)
        {
            channel_en = 0;
            tls_param_set(TLS_PARAM_ID_CHANNEL_EN, (void *)&channel_en, TRUE);
        }

        ret = tls_wifi_connect((u8 *)config->ssid, strlen(config->ssid), (u8 *)config->password, strlen(config->password));
    }

    g_is_wifi_user_disconnect = 0;

    return ret;
}

static int w800_start_softap(rvm_hal_wifi_config_t *config)
{
    int ret;
    struct tls_softap_info_t *apinfo;
    struct tls_ip_info_t *ipinfo;
    u8 ssid_set = 0;
    u8 ssid_set2 = 1;

    ipinfo = malloc(sizeof(struct tls_ip_info_t));
    if (!ipinfo)
    {
        return WIFI_ERR_NO_MEM;
    }
    apinfo = malloc(sizeof(struct tls_softap_info_t));
    if (!apinfo)
    {
        free(ipinfo);
        return WIFI_ERR_NO_MEM;
    }

    memset(apinfo, 0, sizeof(struct tls_softap_info_t));
    strcpy((char *)apinfo->ssid, config->ssid);

    apinfo->channel = 6;

    apinfo->encrypt = 0;  /*0:open, 1:wep64, 2:wep128,3:TKIP WPA ,4: CCMP WPA, 5:TKIP WPA2 ,6: CCMP WPA2*/

    if (config->ap_config.present)
    {
        if (config->ap_config.channel)
            apinfo->channel = config->ap_config.channel;
        if (config->ap_config.hide_ssid)
            ssid_set2 = 0;
        if (config->sec_config.present)
        {
            if (config->sec_config.encrypt_type == WIFI_ENC_WEP)
            {
                if (5  == strlen(config->password))
                    apinfo->encrypt = 1;
                else if (13  == strlen(config->password))
                    apinfo->encrypt = 2;
                else
                {
                    LOGE(TAG, "WEP error.");
                }
            }
            if (config->sec_config.encrypt_type == WIFI_ENC_TKIP)
                apinfo->encrypt = 5;
            if (config->sec_config.encrypt_type == WIFI_ENC_AES)
                apinfo->encrypt = 6;
            if (config->sec_config.encrypt_type == WIFI_ENC_TKIP_AES_MIX)
                apinfo->encrypt = 6;
        }
    }

    tls_param_get(TLS_PARAM_ID_BRDSSID, (void *)&ssid_set, TRUE);
    if (ssid_set2 != ssid_set)
    {
        ssid_set = ssid_set2;//1
        tls_param_set(TLS_PARAM_ID_BRDSSID, (void *)&ssid_set, FALSE); /*set 1 bssid broadcast flag*/
    }

    apinfo->keyinfo.format = 1; /*key's format:0-HEX, 1-ASCII*/
    apinfo->keyinfo.index = 1;  /*wep key index*/
    apinfo->keyinfo.key_len = strlen(config->password); /*key length*/
    memcpy(apinfo->keyinfo.key, config->password, apinfo->keyinfo.key_len);

    /*ip info:ipaddress, netmask, dns, doesn't actually work*/
    ipinfo->ip_addr[0] = 192;
    ipinfo->ip_addr[1] = 168;
    ipinfo->ip_addr[2] = 1;
    ipinfo->ip_addr[3] = 1;
    ipinfo->netmask[0] = 255;
    ipinfo->netmask[1] = 255;
    ipinfo->netmask[2] = 255;
    ipinfo->netmask[3] = 0;
    memcpy(ipinfo->dnsname, "local.wm", sizeof("local.wm"));

    ret = tls_wifi_softap_create(apinfo, ipinfo);

    return ret;
}

static int w800_start(rvm_dev_t *dev, rvm_hal_wifi_config_t *config)
{
    int ret;
    u8 wireless_protocol;

    w800_init(dev);

    if (config->mode == WIFI_MODE_STA)
    {
        tls_wifi_disconnect();
    
        tls_param_get(TLS_PARAM_ID_WPROTOCOL, (void *) &wireless_protocol, TRUE);
        if (TLS_PARAM_IEEE80211_INFRA != wireless_protocol)
        {
            tls_wifi_softap_destroy();
            wireless_protocol = TLS_PARAM_IEEE80211_INFRA;
            tls_param_set(TLS_PARAM_ID_WPROTOCOL, (void *) &wireless_protocol, FALSE);
        }

        tls_wifi_set_oneshot_flag(0);

        ret = w800_start_sta(config);
    }
    else if (config->mode == WIFI_MODE_AP)
    {
        tls_wifi_disconnect();

        tls_param_get(TLS_PARAM_ID_WPROTOCOL, (void *) &wireless_protocol, TRUE);
        if (TLS_PARAM_IEEE80211_SOFTAP != wireless_protocol)
        {
            wireless_protocol = TLS_PARAM_IEEE80211_SOFTAP;
            tls_param_set(TLS_PARAM_ID_WPROTOCOL, (void *) &wireless_protocol, FALSE);
        }

        tls_wifi_set_oneshot_flag(0);
    
        ret = w800_start_softap(config);
    }
    else if (config->mode == WIFI_MODE_APSTA)
    {
        ret  = w800_start_sta(config);
        ret |= w800_start_softap(config);
    }
    else
    {
        LOGD(TAG, "invalid wifi mode");
        return WIFI_ERR_ARG;
    }

    return WIFI_ERR_OK;
}

static int w800_stop(rvm_dev_t *dev)
{
    u8 wmode = IEEE80211_MODE_INFRA;

    tls_param_get(TLS_PARAM_ID_WPROTOCOL, (void *)&wmode, TRUE);

    if (wmode == IEEE80211_MODE_INFRA)
    {
        g_is_wifi_user_disconnect = 1;
        tls_wifi_disconnect();
    }
    else if (wmode == IEEE80211_MODE_AP)
    {
        tls_wifi_softap_destroy();
    }
    else if (wmode == (IEEE80211_MODE_INFRA | IEEE80211_MODE_AP))
    {
        g_is_wifi_user_disconnect = 1;
        tls_wifi_disconnect();
        tls_wifi_softap_destroy();
    }

    return WIFI_ERR_OK;
}

static int w800_sta_get_link_status(rvm_dev_t *dev, rvm_hal_wifi_ap_record_t *ap_info)
{
    struct tls_curr_bss_t currbss;

    if (WM_WIFI_JOINED != tls_wifi_get_state())
    {
        ap_info->link_status = WIFI_STATUS_LINK_DOWN;
        return WIFI_ERR_OK;
    }

    tls_wifi_get_current_bss(&currbss);

    if (g_wifi_got_ip)
        ap_info->link_status = WIFI_STATUS_GOTIP;
    else
        ap_info->link_status = WIFI_STATUS_LINK_UP;

    memcpy(ap_info->bssid, currbss.bssid, 6);

    memcpy(ap_info->ssid, currbss.ssid, currbss.ssid_len);
    ap_info->ssid[currbss.ssid_len] = '\0';

    ap_info->channel = currbss.channel;
    ap_info->rssi = -currbss.rssi - 1;
    ap_info->second = WIFI_SECOND_CHAN_NONE;

    ap_info->encryptmode = WIFI_ENC_MAX;
    ap_info->authmode = WIFI_AUTH_MAX;

    if ((IEEE80211_ENCRYT_AUTO_WPA == currbss.encryptype) || (IEEE80211_ENCRYT_AUTO_WPA2 == currbss.encryptype))
    {
        ap_info->encryptmode = WIFI_ENC_TKIP_AES_MIX;
    }
    else if ((IEEE80211_ENCRYT_CCMP_WPA == currbss.encryptype) || (IEEE80211_ENCRYT_CCMP_WPA2 == currbss.encryptype))
    {
        ap_info->encryptmode = WIFI_ENC_AES;
    }
    else if ((IEEE80211_ENCRYT_TKIP_WPA == currbss.encryptype) || (IEEE80211_ENCRYT_CCMP_WPA == currbss.encryptype))
    {
        ap_info->encryptmode = WIFI_ENC_TKIP;
    }
    if ((IEEE80211_ENCRYT_WEP40 == currbss.encryptype) || (IEEE80211_ENCRYT_WEP104 == currbss.encryptype))
    {
        ap_info->encryptmode = WIFI_ENC_WEP;
    }

    if ((IEEE80211_ENCRYT_AUTO_WPA == currbss.encryptype) || (IEEE80211_ENCRYT_AUTO_WPA2 == currbss.encryptype))
    {
        ap_info->authmode = WIFI_AUTH_WPA_WPA2_PSK;
    }
    else if (IEEE80211_ENCRYT_AUTO_WPA2 == currbss.encryptype)
    {
        ap_info->authmode = WIFI_AUTH_WPA2_PSK;
    }
    else if (IEEE80211_ENCRYT_AUTO_WPA == currbss.encryptype)
    {
        ap_info->authmode = WIFI_AUTH_WPA_PSK;
    }

    if (IEEE80211_ENCRYT_NONE == currbss.encryptype)
    {
        ap_info->encryptmode = WIFI_ENC_OPEN;
        ap_info->authmode = WIFI_AUTH_OPEN;
    }

    return WIFI_ERR_OK;
}

static int w800_ap_get_sta_list(rvm_dev_t *dev, rvm_hal_wifi_sta_list_t *sta)
{
    tls_wifi_get_authed_sta_info((u32 *)&sta->num, (u8 *)&sta->sta[0], sizeof(sta->sta));

    return WIFI_ERR_OK;
}

static void wm_wlan_mgmt_ext_recv_callback(u8* data, u32 data_len, struct tls_wifi_ext_t *ext)
{
    if (g_monitor_mgnt_cb) {
        g_monitor_mgnt_cb(data, data_len);
    }
}

int w800_start_mgnt_monitor(rvm_dev_t *dev, rvm_hal_wifi_mgnt_cb_t cb)
{
    g_monitor_mgnt_cb = cb;

    tls_wifi_mgmt_ext_recv_cb_register(wm_wlan_mgmt_ext_recv_callback);

    return 0;
}

int w800_stop_mgnt_monitor(rvm_dev_t *dev)
{
    return 0;
}

static int w800_start_monitor(rvm_dev_t *dev, rvm_hal_wifi_promiscuous_cb_t cb)
{
    if (!cb)
        return WIFI_ERR_ARG;

    w800_init(dev);

    g_monitor_cb = cb;

    tls_wifi_data_recv_cb_register(wm_wlan_data_recv_callback);
    tls_wl_plcp_cb_register(wm_wlan_data_recv_callback);

    tls_wifi_mgmt_ext_recv_cb_register(wm_wlan_mgmt_recv_callback);

    tls_wifi_set_listen_mode(1);
    tls_wl_plcp_start();

    return WIFI_ERR_OK;
}

static int w800_stop_monitor(rvm_dev_t *dev)
{
    tls_wifi_set_listen_mode(0);
    tls_wl_plcp_stop();

    tls_wifi_data_ext_recv_cb_register(NULL);
    tls_wl_plcp_cb_register(NULL);

    g_monitor_cb = NULL;

    return WIFI_ERR_OK;
}

static int w800_send_80211_raw_frame(rvm_dev_t *dev, void *buffer, uint16_t len)
{
    struct tls_wifi_tx_rate_t tx;

    if (!buffer || len > 1600 || len < 28)
        return WIFI_ERR_ARG;

    tx.tx_rate = WM_WIFI_TX_RATEIDX_36M;
    tx.tx_gain = tls_wifi_get_tx_gain_max(tx.tx_rate);
    tls_wifi_send_data(NULL, buffer, len - 4, &tx); /* -fcs len */

    return WIFI_ERR_OK;
}

static int w800_set_channel(rvm_dev_t *dev, uint8_t primary, rvm_hal_wifi_second_chan_t second)
{
    if ((primary < 1) || (primary > 14)) {
        return WIFI_ERR_ARG;
    }
    
    tls_wifi_change_chanel(primary - 1);

    return WIFI_ERR_OK;
}

static int w800_get_channel(rvm_dev_t *dev, uint8_t *primary, rvm_hal_wifi_second_chan_t *second)
{
    *primary = hed_rf_current_channel;
    return WIFI_ERR_OK;
}

static int w800_set_smartcfg(rvm_dev_t *dev, int enable)
{
    tls_wifi_set_oneshot_flag(enable);

    return WIFI_ERR_OK;
}

static wifi_driver_t w800_wifi_driver = {
    .init             = w800_init,
    .deinit           = w800_deinit,
    .reset            = w800_reset,
    .set_mode         = w800_set_mode,
    .get_mode         = w800_get_mode,
    .install_event_cb = w800_install_event_cb,

    .set_protocol        = w800_set_protocol,
    .get_protocol        = w800_get_protocol,
    .set_country         = w800_set_country,
    .get_country         = w800_get_country,
    .set_mac_addr        = w800_set_mac_addr,
    .get_mac_addr        = w800_get_mac_addr,
    .set_auto_reconnect  = w800_set_auto_reconnect,
    .get_auto_reconnect  = w800_get_auto_reconnect,
    .set_lpm             = w800_set_lpm,
    .get_lpm             = w800_get_lpm,
    .power_on            = w800_power_on,
    .power_off           = w800_power_off,

    .start_scan          = w800_start_scan,
    .start               = w800_start,
    .stop                = w800_stop,
    .sta_get_link_status = w800_sta_get_link_status,
    .ap_get_sta_list     = w800_ap_get_sta_list,

    .start_monitor        = w800_start_monitor,
    .stop_monitor         = w800_stop_monitor,
    .start_mgnt_monitor   = w800_start_mgnt_monitor,
    .stop_mgnt_monitor    = w800_stop_mgnt_monitor,
    .send_80211_raw_frame = w800_send_80211_raw_frame,
    .set_channel          = w800_set_channel,
    .get_channel          = w800_get_channel,

    .set_smartcfg         = w800_set_smartcfg,
};

static rvm_dev_t *w800_dev_init(driver_t *drv, void *config, int id)
{
    rvm_dev_t *dev = rvm_hal_device_new(drv, sizeof(wifi_dev_t), id);

    return dev;
}

static void w800_dev_uninit(rvm_dev_t *dev)
{
    aos_check_param(dev);

    rvm_hal_device_free(dev);
}

static int w800_dev_open(rvm_dev_t *dev)
{
    return 0;
}

static int w800_dev_close(rvm_dev_t *dev)
{
    return 0;
}

static netdev_driver_t w800_driver = {
    .drv =
        {
            .name   = "wifi",
            .init   = w800_dev_init,
            .uninit = w800_dev_uninit,
            .open   = w800_dev_open,
            .close  = w800_dev_close,
        },
    .link_type = NETDEV_TYPE_WIFI,
    .net_ops   = &w800_net_driver,
    .link_ops  = &w800_wifi_driver,
};

#if 1
static void smartcfg_int_callback(int32_t idx)
{
    event_publish(EVENT_WIFI_SMARTCFG, NULL);
}

static void w800_pin_init(w800_wifi_param_t *config)
{

    drv_pinmux_config(config->smartcfg_pin, PIN_FUNC_GPIO);

    smartcfg_pin = csi_gpio_pin_initialize(config->smartcfg_pin, smartcfg_int_callback);
    csi_gpio_pin_config_mode(smartcfg_pin, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(smartcfg_pin, GPIO_DIRECTION_INPUT);

    csi_gpio_pin_set_irq(smartcfg_pin, GPIO_IRQ_MODE_RISING_EDGE, 1);

}
#endif

int wifi_is_connected_to_ap(void)
{
    if (WM_WIFI_JOINED == tls_wifi_get_state()) {
        return 1;
    } else {
        return 0;
    }
}

void wifi_w800_register(w800_wifi_param_t *config)
{
    w800_board_init();

    if (config)
        w800_pin_init(config);

    w800_lwip_init();

    rvm_driver_register(&w800_driver.drv, NULL, 0);
}

