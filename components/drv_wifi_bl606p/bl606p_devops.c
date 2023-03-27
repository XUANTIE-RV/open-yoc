/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/debug.h>
#include <uservice/uservice.h>
#include <devices/netdrv.h>

#include <lwip/apps/dhcps.h>
#include <lwip/netifapi.h>
#include <lwip/dns.h>

#include <netif/ethernet.h>
#include <devices/wifi.h>
#include <devices/impl/wifi_impl.h>
#include "wifi_mgmr_ext.h"
#include <ulog/ulog.h>
#include <aos/yloop.h>
#include <bl606p_yloop.h>

#include <bl60x_fw_api.h>
#include <bl_efuse.h>
#include <bl_wifi.h>
#ifdef CONFIG_KV_SMART
#include <aos/kv.h>
#endif
#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (1)
#include "k_api.h"

#define TAG "wifi"

#if !defined(MIN)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define IEEE80211_FTYPE_MGMT 0x0000
#define IEEE80211_FTYPE_CTL  0x0004
#define IEEE80211_FTYPE_DATA 0x0008

#define WIFI_AP_DATA_RATE_1Mbps   0x00
#define WIFI_AP_DATA_RATE_2Mbps   0x01
#define WIFI_AP_DATA_RATE_5_5Mbps 0x02
#define WIFI_AP_DATA_RATE_11Mbps  0x03
#define WIFI_AP_DATA_RATE_6Mbps   0x0b
#define WIFI_AP_DATA_RATE_9Mbps   0x0f
#define WIFI_AP_DATA_RATE_12Mbps  0x0a
#define WIFI_AP_DATA_RATE_18Mbps  0x0e
#define WIFI_AP_DATA_RATE_24Mbps  0x09
#define WIFI_AP_DATA_RATE_36Mbps  0x0d
#define WIFI_AP_DATA_RATE_48Mbps  0x08
#define WIFI_AP_DATA_RATE_54Mbps  0x0c

typedef struct {
    rvm_dev_t device;
    uint8_t   mode;
    void (*write_event)(rvm_dev_t *dev, int event_id, void *priv);
    void *priv;
} wifi_dev_t;

typedef struct ieee80211_frame_info {
    unsigned short i_fc;
    unsigned short i_dur;
    unsigned char  i_addr1[6];
    unsigned char  i_addr2[6];
    unsigned char  i_addr3[6];
    unsigned short i_seq;
    unsigned char  bssid[6];
    unsigned char  encrypt;
    signed char    rssi;
} ieee80211_frame_info_t;

struct wifi_ap_data_rate {
    uint8_t     data_rate;
    const char *val;
};

static const struct wifi_ap_data_rate data_rate_list[] = {
    { WIFI_AP_DATA_RATE_1Mbps, "1.0 Mbit/s, 20Mhz" },
    { WIFI_AP_DATA_RATE_2Mbps, "2.0 Mbit/s, 20Mhz" },
    { WIFI_AP_DATA_RATE_5_5Mbps, "5.5 Mbit/s, 20Mhz" },
    { WIFI_AP_DATA_RATE_11Mbps, "11.0 Mbit/s, 20Mhz" },
    { WIFI_AP_DATA_RATE_6Mbps, "6.0 Mbit/s, 20Mhz" },
    { WIFI_AP_DATA_RATE_9Mbps, "9.0 Mbit/s, 20Mhz" },
    { WIFI_AP_DATA_RATE_12Mbps, "12.0 Mbit/s, 20Mhz" },
    { WIFI_AP_DATA_RATE_18Mbps, "18.0 Mbit/s, 20Mhz" },
    { WIFI_AP_DATA_RATE_24Mbps, "24.0 Mbit/s, 20Mhz" },
    { WIFI_AP_DATA_RATE_36Mbps, "36.0 Mbit/s, 20Mhz" },
    { WIFI_AP_DATA_RATE_48Mbps, "48.0 Mbit/s, 20Mhz" },
    { WIFI_AP_DATA_RATE_54Mbps, "54.0 Mbit/s, 20Mhz" },
};

static void wifi_sta_stop(void);
static void wifi_ap_stop(void);
// advise: hal functions using global variable here may be deleted
static rvm_hal_wifi_lpm_mode_t       g_wifi_lpm_mode;
static rvm_hal_wifi_config_t         g_config;
static int                   sta_task_state;
static int                   wifi_crashed;
static wifi_interface_t      sta_wifi_interface;
static wifi_interface_t      ap_wifi_interface;
static wifi_mgmr_ap_item_t  *ap_ary_p;
static uint32_t              scan_cnt;
static rvm_hal_wifi_promiscuous_cb_t sinnfer_fn;
static uint8_t               mgmt_is_enable;
static rvm_hal_wifi_event_func      *g_evt_func;
static rvm_hal_wifi_mgnt_cb_t        mgmt_fn;
static rvm_dev_t            *wifi_evt_dev;

__attribute__((section(".wifi_ram"))) static cpu_stack_t wifi_stack[WIFI_STACK_SIZE];

static ktask_t wifi_fw_task;

#define DRIVER_INVALID_RETURN_VAL                             \
    do {                                                      \
        if (wifi_crashed)                                     \
            return -1;                                        \
    } while (0)

#define DRIVER_INVALID_RETURN                                 \
    do {                                                      \
        if (wifi_crashed)                                     \
            return;                                           \
    } while (0)

#define PTA_REG_BASE_ADDR               (0x24920000)
#define PTA_REG_REVISION                (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x00))
#define PTA_REG_CONFIG                  (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x04))
uint32_t btc = 0;
uint32_t wific = 0;
uint8_t c = 0;
float btc_wifi = 0.0;

static void cal_pta(void)                                                                                                                                                                                                                               
{
    uint32_t config = PTA_REG_CONFIG;
    if ((config & 0x000c0000) && (!(config & 0x00030000))) {
        btc ++;
    } else if ((config & 0x00030000) && (!(config & 0x000c0000))) {
        wific ++;
    }
    if (0 == (c%100)) {
        btc_wifi = (float)btc/(btc+wific);
        LOGD(TAG, "[%lld] cal_pta %lx, %lx, %.3f\r\n", aos_now_ms(), btc, wific, (float)btc/(btc+wific));
        //int bl_wifi_ps_dump(void);
        //bl_wifi_ps_dump();
        btc = 0;
        wific = 0;
        c = 0;
    }
    c ++;
}

static BL_Timer_t timer = NULL;

static void _wifi_ps_mode(void *arg)                                                                                                                                                                                                                    
{
    int mode = (int)arg;
    if (mode) {

        if (timer == NULL) {
            timer = bl_os_timer_create(cal_pta, NULL);
            bl_os_timer_start_periodic(timer, 0, 10 *1e6);
        }

        wifi_mgmr_sta_ps_enter(WIFI_COEX_PM_STA_MESH);
        wifi_mgmr_set_wifi_active_time(50);
    } else {
         if (timer) {
            bl_os_timer_delete(timer, 0);
            timer = NULL;
         }
        wifi_mgmr_sta_ps_exit();
    }
}

bool tg_wifi_connected(void)
{
    int state;
    bool connected = false;
    
    wifi_mgmr_state_get(&state);
    if (WIFI_STATE_CONNECTED_IP_GOT == state || 
        WIFI_STATE_CONNECTED_IP_GETTING ==state) {
        connected = true;
    } 
    return connected;
}

int tg_wifi_ps_mode_entry(void)
{
    if (tg_wifi_connected()) {
        aos_post_delayed_action(1000, _wifi_ps_mode, (void *)1);
    }
    return 0;
}

int tg_wifi_ps_mode_exit(void)
{   
    aos_post_delayed_action(1000, _wifi_ps_mode, (void *)0);
    return 0;
}

static int bl606p_set_mac_addr(rvm_dev_t *dev, const uint8_t *mac)
{
    /** write this mac address will write to efuse, forbidden this */
    return -1;
}

static int bl606p_get_mac_addr(rvm_dev_t *dev, uint8_t *mac)
{
    DRIVER_INVALID_RETURN_VAL;

    LOGD(TAG, "%s\r\n", __func__);
    int ret = wifi_mgmr_sta_mac_get((uint8_t *)mac);
    return ret;
}

static int bl606p_set_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    return -1;
}

static int bl606p_get_dns_server(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    int n = MIN(num, 2);

    if (n == 2) {
        wifi_mgmr_sta_dns_get(&ipaddr[0].addr, &ipaddr[1].addr);
    } else {
        wifi_mgmr_sta_dns_get(&ipaddr[0].addr, NULL);
    }
    return n;
}

static int bl606p_set_hostname(rvm_dev_t *dev, const char *name)
{

    return -1;
}

static const char *bl606p_get_hostname(rvm_dev_t *dev)
{
    return NULL;
}

static int bl606p_start_dhcp(rvm_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;

    return 0;
}

static int bl606p_stop_dhcp(rvm_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;

    return 0;
}

static int bl606p_set_ipaddr(rvm_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw)
{
    return -1;
}

static int bl606p_get_ipaddr(rvm_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw)
{
    DRIVER_INVALID_RETURN_VAL;

    LOGD(TAG, "%s\r\n", __func__);
    uint32_t ip, mask, gateway;

    if (sta_wifi_interface)
        wifi_mgmr_sta_ip_get(&ip, &mask, &gateway);
    else
        wifi_mgmr_ap_ip_get(&ip, &mask, &gateway);

    if (ipaddr)
        ipaddr->addr = ip;
    if (netmask)
        netmask->addr = mask;
    if (gw)
        gw->addr = gateway;

    return 0;
}

static int bl606p_subscribe(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    return -1;
}

static int bl606p_ping_remote(rvm_dev_t *dev, int type, char *remote_ip)
{
    return -1;
}

static net_ops_t bl606p_net_driver = {
    .set_mac_addr   = bl606p_set_mac_addr,
    .get_mac_addr   = bl606p_get_mac_addr,
    .set_dns_server = bl606p_set_dns_server,
    .get_dns_server = bl606p_get_dns_server,
    .set_hostname   = bl606p_set_hostname,
    .get_hostname   = bl606p_get_hostname,
    .start_dhcp     = bl606p_start_dhcp,
    .stop_dhcp      = bl606p_stop_dhcp,
    .set_ipaddr     = bl606p_set_ipaddr,
    .get_ipaddr     = bl606p_get_ipaddr,
    .subscribe      = bl606p_subscribe,
    .ping           = bl606p_ping_remote,
};

/**
    The wifi mode is controlled by the global variable 'wifi_mode'
*/
static int bl606p_set_mode(rvm_dev_t *dev, rvm_hal_wifi_mode_t mode)
{
    DRIVER_INVALID_RETURN_VAL;

    return 0;
}

static int bl606p_get_mode(rvm_dev_t *dev, rvm_hal_wifi_mode_t *mode)
{
    DRIVER_INVALID_RETURN_VAL;

    return 0;
}

static int bl606p_init(rvm_dev_t *dev)
{
    // wifi_on();
    return 0;
}

static int bl606p_deinit(rvm_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;

    wifi_sta_stop();
    wifi_ap_stop();
    return -1;
}

static void wifi_ap_start(rvm_hal_wifi_config_t *config)
{
    uint8_t hidden_ssid = config->ap_config.hide_ssid ? 1 : 0;
    int     channel     = config->ap_config.channel;

    ap_wifi_interface = wifi_mgmr_ap_enable();
    if (strlen(config->password) == 0) {
        /*no password when only one param*/
        wifi_mgmr_ap_start(ap_wifi_interface, config->ssid, hidden_ssid, NULL, 1);
    } else {
        /*hardcode password*/
        if (channel <= 0 || channel > 11) {
            channel = 1;
        }
        wifi_mgmr_ap_start(ap_wifi_interface, config->ssid, hidden_ssid, config->password, channel);
    }
}

static void wifi_ap_stop(void)
{
    if (ap_wifi_interface) {
        wifi_mgmr_ap_stop(NULL);
        ap_wifi_interface = NULL;
    }
}

static void wifi_sta_start_task(void *arg)
{
    sta_task_state        = 1;
    sta_wifi_interface    = wifi_mgmr_sta_enable();
    rvm_hal_wifi_config_t *config = (rvm_hal_wifi_config_t *)arg;
    wifi_mgmr_sta_autoconnect_enable();

    if (0 != config->ssid[0]) {
        wifi_mgmr_sta_connect(sta_wifi_interface, config->ssid, config->password, NULL, NULL, 0, 0);
    }
    LOGD(TAG, "%s\r\n", __func__);
    event_publish(EVENT_WIFI_LINK_UP, NULL);

    sta_task_state = 2;
}

static void wifi_sta_stop(void)
{
    if (sta_task_state) {
        wifi_mgmr_sta_autoconnect_disable();
        wifi_mgmr_sta_disconnect();
        aos_msleep(1000);
        wifi_mgmr_sta_disable(NULL);
        aos_msleep(1000);
        sta_task_state     = 0;
        sta_wifi_interface = NULL;
    }
}

static int bl606p_start(rvm_dev_t *dev, rvm_hal_wifi_config_t *config)
{
    DRIVER_INVALID_RETURN_VAL;

    if (config->mode == WIFI_MODE_STA) {
        wifi_sta_stop();
        wifi_ap_stop();
        memcpy(&g_config, config, sizeof(rvm_hal_wifi_config_t));
        aos_task_t task_handle;
        aos_task_new_ext(&task_handle, "wifi_sta_start_task", wifi_sta_start_task, &g_config, 1024 * 6, 35);
    } else if (config->mode == WIFI_MODE_AP) {
        LOGD(TAG, "start softap!\n");
        wifi_sta_stop();
        wifi_ap_start(config);
    } else {
        LOGD(TAG, "Not supported wifi mode!\n");
    }

    return 0;
}

static int bl606p_stop(rvm_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;
    wifi_sta_stop();
    wifi_ap_stop();
    return 0;
}

static int bl606p_reset(rvm_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;
    wifi_sta_stop();
    wifi_ap_stop();
    return 0;
}

/** conf APIs */
static int bl606p_set_lpm(rvm_dev_t *dev, rvm_hal_wifi_lpm_mode_t mode)
{
    DRIVER_INVALID_RETURN_VAL;

    g_wifi_lpm_mode = mode;

    return 0;
}

static int bl606p_get_lpm(rvm_dev_t *dev, rvm_hal_wifi_lpm_mode_t *mode)
{
    *mode = g_wifi_lpm_mode;
    return 0;
}

static int bl606p_set_protocol(rvm_dev_t *dev, uint8_t protocol_bitmap)
{
    DRIVER_INVALID_RETURN_VAL;

    return 0;
}

static int bl606p_get_protocol(rvm_dev_t *dev, uint8_t *protocol_bitmap)
{
    DRIVER_INVALID_RETURN_VAL;

    return 0;
}

static int bl606p_set_country(rvm_dev_t *dev, rvm_hal_wifi_country_t country)
{
    DRIVER_INVALID_RETURN_VAL;

    return -1;
}

static int bl606p_get_country(rvm_dev_t *dev, rvm_hal_wifi_country_t *country)
{
    DRIVER_INVALID_RETURN_VAL;

    return 0;
}

/**
    When wifi is disconnected, judge if reconnect wifi
*/
static int bl606p_set_auto_reconnect(rvm_dev_t *dev, bool en)
{
    DRIVER_INVALID_RETURN_VAL;

    return -1;
}

static int bl606p_get_auto_reconnect(rvm_dev_t *dev, bool *en)
{
    DRIVER_INVALID_RETURN_VAL;

    return -1;
}

static int bl606p_power_on(rvm_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;

    return 0;
}

static int bl606p_power_off(rvm_dev_t *dev)
{
    return 0;
}

/**
    TBD, scan with scan config
*/

static int bl606p_start_scan(rvm_dev_t *dev, wifi_scan_config_t *config, bool block)
{
    int ret = -1;
    LOGD(TAG, "%s\r\n", __func__);
    if (block == 0) {
        return -1;
    }

    if (!g_evt_func->scan_compeleted) {
        return -1;
    }
    ap_ary_p = NULL;
    scan_cnt = 0;

    wifi_mgmr_all_ap_scan(&ap_ary_p, &scan_cnt);
    if (scan_cnt != 0) {
        rvm_hal_wifi_ap_record_t *ap_records = aos_calloc(sizeof(rvm_hal_wifi_ap_record_t), scan_cnt);

        for (int i = 0; i < scan_cnt; i++) {
            memcpy(ap_records[i].ssid, ap_ary_p[i].ssid, ap_ary_p[i].ssid_len);
            memcpy(ap_records[i].bssid, ap_ary_p[i].bssid, 6);
            ap_records[i].channel  = ap_ary_p[i].channel;
            ap_records[i].rssi     = ap_ary_p[i].rssi;
            ap_records[i].authmode = ap_ary_p[i].auth < WIFI_AUTH_MAX ? ap_ary_p[i].auth : WIFI_AUTH_MAX;
        }

        g_evt_func->scan_compeleted(wifi_evt_dev, scan_cnt, ap_records);
        aos_free(ap_records);
        aos_free(ap_ary_p);
        ret = 0;
    }
    return ret;
}

static int bl606p_sta_get_link_status(rvm_dev_t *dev, rvm_hal_wifi_ap_record_t *ap_info)
{
    DRIVER_INVALID_RETURN_VAL;

    wifi_mgmr_sta_connect_ind_stat_info_t info = { 0 };

    int state;
    int rssi, channel;

    LOGD(TAG, "%s\r\n", __func__);
    wifi_mgmr_state_get(&state);
    if ((WIFI_STATE_CONNECTED_IP_GOT != state) && (WIFI_STATE_CONNECTED_IP_GETTING != state)) {
        ap_info->link_status = WIFI_STATUS_LINK_DOWN;
    } else {
        ap_info->link_status = WIFI_STATUS_LINK_UP;
        wifi_mgmr_sta_connect_ind_stat_get(&info);
        wifi_mgmr_rssi_get(&rssi);
        memcpy(ap_info->ssid, info.ssid, sizeof(ap_info->ssid));
        memcpy(ap_info->bssid, info.bssid, sizeof(ap_info->bssid));
        wifi_mgmr_channel_get(&channel);
        ap_info->channel = (uint8_t)channel;
        ap_info->rssi    = rssi;
    }

    return 0;
}

static int bl606p_ap_get_sta_list(rvm_dev_t *dev, rvm_hal_wifi_sta_list_t *sta)
{
    DRIVER_INVALID_RETURN_VAL;

    int        state   = WIFI_STATE_UNKNOWN;
    int        cnt     = 0;
    uint8_t    sta_cnt = 0, i, j;
    uint8_t    index   = 0;
    long long sta_time;

    struct wifi_sta_basic_info sta_info;

    wifi_mgmr_state_get(&state);
    if (!WIFI_STATE_AP_IS_ENABLED(state)) {
        LOGD(TAG, "wifi AP is not enabled, state = %d\r\n", state);
        return -1;
    }

    wifi_mgmr_ap_sta_cnt_get(&sta_cnt);
    if (!sta_cnt) {
        LOGD(TAG, "no sta connect current AP, sta_cnt = %d\r\n", sta_cnt);
        return -1;
    }

    memset(&sta_info, 0, sizeof(struct wifi_sta_basic_info));
    //LOGD(TAG, "sta list:\r\n");
    //LOGD(TAG, "-----------------------------------------------------------------------------------\r\n");
    //LOGD(TAG, "No.      StaIndex      Mac-Address       Signal      DateRate            TimeStamp\r\n");
    //LOGD(TAG, "-----------------------------------------------------------------------------------\r\n");
    for (i = 0; i < sta_cnt; i++) {
        wifi_mgmr_ap_sta_info_get(&sta_info, i);
        if (!sta_info.is_used || (sta_info.sta_idx == 0xef)) {
            continue;
        }

        cnt++;
        sta_time = (long long)sta_info.tsfhi;
        sta_time = (sta_time << 32) | sta_info.tsflo;

        for (j = 0; j < sizeof(data_rate_list) / sizeof(data_rate_list[0]); j++) {
            if (data_rate_list[j].data_rate == sta_info.data_rate) {
                index = j;
                break;
            }
        }

        if (i >= HAL_WIFI_MAX_CONN_NUM) {
            LOGD(TAG, "WiFi connection exceeds max number of HAL\n");
            break;
        }

        LOGD(TAG,
             " %u       "
             "   %u        "
             "%02X:%02X:%02X:%02X:%02X:%02X    "
             "%d      "
             "%s      "
             "0x%llx"
             "\r\n",
             i,
             sta_info.sta_idx,
             sta_info.sta_mac[0],
             sta_info.sta_mac[1],
             sta_info.sta_mac[2],
             sta_info.sta_mac[3],
             sta_info.sta_mac[4],
             sta_info.sta_mac[5],
             sta_info.rssi,
             data_rate_list[index].val,
             sta_time);
        memcpy(sta->sta[i].mac, sta_info.sta_mac, 6);
    }
    sta->num = cnt;
    return 0;
}

static int bl606p_start_mgnt_monitor(rvm_dev_t *dev, rvm_hal_wifi_mgnt_cb_t cb)
{
    DRIVER_INVALID_RETURN_VAL;

    mgmt_is_enable = 1;
    mgmt_fn        = cb;

    return 0;
}

static int bl606p_stop_mgnt_monitor(rvm_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;

    mgmt_is_enable = 0;
    mgmt_fn        = NULL;

    return 0;
}

static int bl606p_start_monitor(rvm_dev_t *dev, rvm_hal_wifi_promiscuous_cb_t cb)
{
    DRIVER_INVALID_RETURN_VAL;

    LOGD(TAG, "%s\r\n", __func__);
    sinnfer_fn = cb;
    wifi_mgmr_sniffer_enable();

    return 0;
}

static int bl606p_stop_monitor(rvm_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;

    LOGD(TAG, "%s\r\n", __func__);
    wifi_mgmr_sniffer_disable();
    wifi_mgmr_sniffer_unregister(NULL);
    sinnfer_fn = NULL;

    return 0;
}

static int bl606p_set_channel(rvm_dev_t *dev, uint8_t primary, rvm_hal_wifi_second_chan_t second)
{
    DRIVER_INVALID_RETURN_VAL;

    LOGD(TAG, "%s\r\n", __func__);
    return wifi_mgmr_channel_set(primary, 0);
}

static int bl606p_get_channel(rvm_dev_t *dev, uint8_t *primary, rvm_hal_wifi_second_chan_t *second)
{
    DRIVER_INVALID_RETURN_VAL;

    LOGD(TAG, "%s\r\n", __func__);
    int channel;
    if (wifi_mgmr_channel_get(&channel) == 0) {
        *primary = channel;
        return 0;
    } else {
        return -1;
    }
}

static int bl606p_send_80211_raw_frame(rvm_dev_t *dev, void *buffer, uint16_t len)
{
    DRIVER_INVALID_RETURN_VAL;

    LOGD(TAG, "%s\r\n", __func__);
    int ret = wifi_mgmr_raw_80211_send((uint8_t *)buffer, len);
    return ret;
}

static int bl606p_install_event_cb(rvm_dev_t *dev, rvm_hal_wifi_event_func *evt_func)
{
    DRIVER_INVALID_RETURN_VAL;

    g_evt_func   = evt_func;
    wifi_evt_dev = dev;

    return 0;
}

static wifi_driver_t bl606p_wifi_driver = {
    /** basic APIs */

    .init             = bl606p_init,
    .deinit           = bl606p_deinit,
    .reset            = bl606p_reset,
    .set_mode         = bl606p_set_mode,
    .get_mode         = bl606p_get_mode,
    .install_event_cb = bl606p_install_event_cb,

    .set_protocol        = bl606p_set_protocol,
    .get_protocol        = bl606p_get_protocol,
    .set_country         = bl606p_set_country,
    .get_country         = bl606p_get_country,
    .set_mac_addr        = bl606p_set_mac_addr,
    .get_mac_addr        = bl606p_get_mac_addr,
    .set_auto_reconnect  = bl606p_set_auto_reconnect,
    .get_auto_reconnect  = bl606p_get_auto_reconnect,
    .set_lpm             = bl606p_set_lpm,
    .get_lpm             = bl606p_get_lpm,
    .power_on            = bl606p_power_on,
    .power_off           = bl606p_power_off,
    .start_scan          = bl606p_start_scan,
    .start               = bl606p_start,
    .stop                = bl606p_stop,
    .sta_get_link_status = bl606p_sta_get_link_status,
    .ap_get_sta_list     = bl606p_ap_get_sta_list,

    /** promiscuous APIs */
    .start_monitor = bl606p_start_monitor,
    .stop_monitor  = bl606p_stop_monitor,

    .start_mgnt_monitor   = bl606p_start_mgnt_monitor,
    .stop_mgnt_monitor    = bl606p_stop_mgnt_monitor,
    .send_80211_raw_frame = bl606p_send_80211_raw_frame,
    .set_channel          = bl606p_set_channel,
    .get_channel          = bl606p_get_channel,

    .set_smartcfg = NULL,
};

static rvm_dev_t *bl606p_dev_init(driver_t *drv, void *config, int id)
{
    rvm_dev_t *dev = rvm_hal_device_new(drv, sizeof(wifi_dev_t), id);

    return dev;
}

static void bl606p_dev_uninit(rvm_dev_t *dev)
{
    aos_check_param(dev);

    rvm_hal_device_free(dev);
}

static int bl606p_dev_open(rvm_dev_t *dev)
{
    return 0;
}

static int bl606p_dev_close(rvm_dev_t *dev)
{
    return 0;
}

static netdev_driver_t bl606p_driver = {
    .drv =
        {
            .name   = "wifi",
            .init   = bl606p_dev_init,
            .uninit = bl606p_dev_uninit,
            .open   = bl606p_dev_open,
            .close  = bl606p_dev_close,
        },
    .link_type = NETDEV_TYPE_WIFI,
    .net_ops   = &bl606p_net_driver,
    .link_ops  = &bl606p_wifi_driver,
};

#define WIFI_MAC_ADDR_KEY "wifi_key_mac"

static int wifi_get_debug_mac_address(uint8_t mac[6])
{
    mac[0] = 0x18;
    mac[1] = 0xb9;
    mac[2] = 0x05;
    mac[3] = 0xde;
    mac[4] = 0x4a;
    mac[5] = 0xee;

#ifdef CONFIG_KV_SMART
    char mac_str[32];

    if (aos_kv_getstring(WIFI_MAC_ADDR_KEY, mac_str, sizeof(mac_str)) > 0) {
        sscanf(mac_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
    }
#endif

    return 0;
}

static void _sniffer_callback(void *env, uint8_t *pkt, int len)
{
    if (len < sizeof(ieee80211_frame_info_t)) {
        // while(1);
        return;
    }
    ieee80211_frame_info_t     *info = (ieee80211_frame_info_t *)pkt;
    rvm_hal_wifi_promiscuous_pkt_t     *buf  = aos_malloc(sizeof(rvm_hal_wifi_promiscuous_pkt_t) + len);
    rvm_hal_wifi_promiscuous_pkt_type_t type = WIFI_PKT_MISC;

    // buf->rx_ctrl.rssi    = info->rssi;
    buf->rx_ctrl.sig_len = len - sizeof(ieee80211_frame_info_t);
    memcpy(buf->payload, pkt + sizeof(ieee80211_frame_info_t), len - sizeof(ieee80211_frame_info_t));

    if ((info->i_fc & 0xc) == IEEE80211_FTYPE_DATA) {
        type = WIFI_PKT_DATA;
    } else if ((info->i_fc & 0xc) == IEEE80211_FTYPE_MGMT) {
        type = WIFI_PKT_MGMT;
    } else if ((info->i_fc & 0xc) == IEEE80211_FTYPE_CTL) {
        type = WIFI_PKT_CTRL;
    }

    if (mgmt_is_enable && mgmt_fn) {
        if (type == WIFI_PKT_MGMT) {
            mgmt_fn(pkt, len);
        }
    }
    if (sinnfer_fn) {
        sinnfer_fn(buf, type);
    }
    aos_free(buf);
}

static void event_cb_wifi_event(input_event_t *event, void *private_data)
{
    static wifi_conf_t conf = {
        .country_code = "CN",
    };

    switch (event->code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOGD(TAG, "[APP] [EVT] INIT DONE %ld\r\n", aos_now_ms());
            wifi_mgmr_start_background(&conf);
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOGD(TAG, "[APP] [EVT] MGMR DONE %ld\r\n", aos_now_ms());
        } break;
        case CODE_WIFI_ON_MGMR_DENOISE: {
            LOGD(TAG, "[APP] [EVT] Microwave Denoise is ON %lld\r\n", aos_now_ms());
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            wifi_mgmr_cli_scanlist();
            LOGD(TAG, "[APP] [EVT] SCAN Done %ld\r\n", aos_now_ms());
        } break;
        case CODE_WIFI_ON_SCAN_DONE_ONJOIN: {
            LOGD(TAG, "[APP] [EVT] SCAN On Join %ld\r\n", aos_now_ms());
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
            LOGD(TAG,
                 "[APP] [EVT] disconnect %ld, Reason: %s\r\n",
                 (long)aos_now_ms(),
                 wifi_mgmr_status_code_str(event->value));
        } break;
        case CODE_WIFI_ON_CONNECTING: {
            LOGD(TAG, "[APP] [EVT] Connecting %ld\r\n", aos_now_ms());
        } break;
        case CODE_WIFI_CMD_RECONNECT: {
            LOGD(TAG, "[APP] [EVT] Reconnect %ld\r\n", aos_now_ms());
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOGD(TAG, "[APP] [EVT] connected %ld\r\n", aos_now_ms());
        } break;
        case CODE_WIFI_ON_PRE_GOT_IP: {
            LOGD(TAG, "[APP] [EVT] connected %ld\r\n", aos_now_ms());
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            LOGD(TAG, "[APP] [EVT] GOT IP %ld\r\n", aos_now_ms());
            event_publish(EVENT_NET_GOT_IP, NULL);
            extern bool hal_bt_is_connected(void);
            if (hal_bt_is_connected()) {
                tg_wifi_ps_mode_entry();
            }

        } break;
        case CODE_WIFI_ON_EMERGENCY_MAC: {
            LOGD(TAG, "[APP] [EVT] EMERGENCY MAC %ld\r\n", aos_now_ms());
        } break;
            break;
        case CODE_WIFI_ON_AP_STA_ADD: {
            LOGD(TAG, "[APP] [EVT] [AP] [ADD] %ld, sta idx is %lu\r\n", aos_now_ms(), (uint32_t)event->value);
        } break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            LOGD(TAG, "[APP] [EVT] [AP] [DEL] %ld, sta idx is %lu\r\n", aos_now_ms(), (uint32_t)event->value);
        } break;
        default: {
            LOGD(TAG, "[APP] [EVT] Unknown code %u, %ld\r\n", event->code, aos_now_ms());
            /*nothing*/
        }
    }
}

static int _wifi_start_firmware_task(void)
{
    bl_pm_init();
    krhino_task_create(&wifi_fw_task,
                       "fw",
                       NULL,
                       TASK_PRIORITY_FW,
                       0,
                       (cpu_stack_t *)((uint32_t)wifi_stack | 0x40000000),
                       WIFI_STACK_SIZE,
                       wifi_main,
                       1);
    return 0;
}

/**
 * @brief  register wifi driver of rtl8723
 * @param  [in] config
 * @return
 */
void wifi_bl606p_register(void *config)
{
    int ret = 0;

    uint8_t mac_addr[6] = { 0 };

    tcpip_init(NULL, NULL);

    /* get mac from efuse */
    ret = bl_efuse_read_mac_smart(1, mac_addr, 0);

    if (ret < 0) {
        wifi_get_debug_mac_address(mac_addr);
    }

    /* set mac addr to wifi mgr sys */
    wifi_mgmr_sta_mac_set(mac_addr);
    bl_wifi_sta_mac_addr_set(mac_addr);

    _wifi_start_firmware_task();

    aos_register_event_filter(EV_WIFI, event_cb_wifi_event, NULL);

    /*Trigger to start Wi-Fi*/
    aos_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0);

    wifi_mgmr_sniffer_register(NULL, _sniffer_callback);

    rvm_driver_register(&bl606p_driver.drv, NULL, 0);
}
