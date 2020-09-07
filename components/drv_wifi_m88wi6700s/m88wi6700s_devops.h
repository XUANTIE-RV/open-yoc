/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "sdio_lanqi_porting.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __M88WI6700S_H__
#define __M88WI6700S_H__


#ifndef BIT
#define BIT(s) (1 << (s))
#endif


enum nl80211_iftype {
    NL80211_IFTYPE_UNSPECIFIED,
    NL80211_IFTYPE_ADHOC,
    NL80211_IFTYPE_STATION,
    NL80211_IFTYPE_AP,
    NL80211_IFTYPE_AP_VLAN,
    NL80211_IFTYPE_WDS,
    NL80211_IFTYPE_MONITOR,
    NL80211_IFTYPE_MESH_POINT,
    NL80211_IFTYPE_P2P_CLIENT,
    NL80211_IFTYPE_P2P_GO,
    NL80211_IFTYPE_P2P_DEVICE,

    /* keep last */
    NUM_NL80211_IFTYPES,
    NL80211_IFTYPE_MAX = NUM_NL80211_IFTYPES - 1
};

enum {
    NBSS_SEC_NON = 0,
    NBSS_SEC_WEP = 1,
    NBSS_SEC_WPA = 2,
    NBSS_SEC_WPA2 = 4,
    NBSS_SEC_WPS = 8,
    NBSS_SEC_WAPI = 16,
};


enum BW_MODE {
    BW40MHZ_SCN = 0,    /* no secondary channel is present */
    BW40MHZ_SCA = 1,    /* secondary channel is above the primary channel */
    BW40MHZ_SCB = 3,    /* secondary channel is below the primary channel */
    BW40MHZ_AUTO = 4,   /* auto select secondary channel */
};

enum nl80211_channel_type {
    NL80211_CHAN_NO_HT,
    NL80211_CHAN_HT20,
    NL80211_CHAN_HT40MINUS,
    NL80211_CHAN_HT40PLUS
};

#ifndef WLAN_ADDR_LEN
#define WLAN_ADDR_LEN           6
#endif

#define NBSS_ENABLE             BIT(0)

typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;
typedef unsigned long long      u64;

typedef char                    s8;
typedef short                   s16;
typedef int                     s32;
typedef long long               s64;

struct ieee80211_radiotap_header {
        u8        it_version;     /* set to 0 */  
        u8        it_pad;  
        u16       it_len;         /* entire length */  
        u32       it_present;     /* fields present */  
} __attribute__((__packed__));  

struct lynx_rx_radiotap_hdr {
	struct ieee80211_radiotap_header hdr;
	u8 rate; /* unit 500Kbps */

	/* channel,mhz */
	u16 chnl_freq __attribute__((__aligned__(2)));
	u16 chnl_flags;

	s8 dbmsignal; /*dbM*/
} __attribute__((__packed__));


struct wlan_wme_ac_params {
    short qidx;
    short cwmin;
    short cwmax;
    short aifs;
    int txoplimit; /* in units of 32us */
    int admission_control_mandatory;
};


struct wme_ie_data {
    struct wlan_wme_ac_params wme_ac_params[4];
    u32 wme_acm;
};

struct wpa_ie_data {
    u32 proto;
    u32 pairwise_cipher;
    u32 group_cipher;
    u32 key_mgmt;
    u32 capabilities;
    u32 num_pmkid;
    u8 *pmkid;
    u32 mgmt_group_cipher;
};

struct neighbor_bss {
    unsigned int flag;
    unsigned char channel;
    unsigned char secondary_ch;
    unsigned char addr[WLAN_ADDR_LEN];
    signed char rssi[3];    /* avoid the rssi signal wrong case */
    unsigned char band;
    unsigned char dtim_period;
    unsigned char qinfo;
    unsigned short sec;
    unsigned short capability;
    unsigned short ht_capability;
    unsigned short beacon_interval;
    unsigned int expire;
    unsigned char ssid[33];

#ifdef CONFIG_LYNX_IBSS
    /* for IBSS */
    unsigned char   ampdu_params;
    unsigned char   bssid[WLAN_ADDR_LEN];
    unsigned short  atim_window;
    unsigned int    supp_rates;
    unsigned int    timestamp[2];
#endif // CONFIG_LYNX_IBSS

#ifdef CONFIG_WPS
    unsigned int wps_info;
#endif

    struct wpa_ie_data *wpa_info;
    struct wpa_ie_data *rsn_info;
    struct wme_ie_data *wme_info;
};


enum {
    LYNX_EVT_SCAN_DONE = 0,
} lynx_evt_t;

typedef int (*lynx_evt_callback)(int evt_id);
typedef void (*lynx_monitor_callback)(u8* data, int len);

void lynx_cli2_init(void);
int lynx_api_get_mac(uint8_t *mac);
int lynx_api_setiftype(enum nl80211_iftype iftype);
enum nl80211_iftype lynx_api_getiftype(void);
int lynx_api_connect(unsigned char *ssid, unsigned char *password, unsigned char *bssid);
void lynx_api_start_scan(bool block);
void lynx_api_stop_scan();
void lynx_api_reg_event_callback(lynx_evt_callback callback);
void lynx_api_switch_mode(enum nl80211_iftype type);
void lynx_api_set_iftype(enum nl80211_iftype iftype);
void lynx_api_disconnect();
int lynx_api_get_ap_info(uint8_t *bssid, uint8_t *ssid, uint8_t *channel, int8_t *rssi);
int lynx_api_get_scan_count();
int lynx_api_set_ps(bool enable_ps);
int lynx_api_get_ps();
struct neighbor_bss *lynx_api_get_scan_results();
void lynx_api_set_monitor_callback(lynx_monitor_callback callback);
int lynx_api_monitor_set_channel(int num);
int lynx_api_monitor_get_channel();
int lynx_api_sta_set_bssid(uint8_t *bssid);
int lynx_api_monitor_set_bandwidth(enum nl80211_channel_type superchan);
u8 lynx_api_monitor_get_bandwidth();


#ifdef __cplusplus
}
#endif

#endif
