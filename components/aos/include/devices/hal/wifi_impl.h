/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_WIFI_IMPL_H
#define HAL_WIFI_IMPL_H


#include <aos/aos.h>

#include <devices/hal/net_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

//30个HAL接口
typedef struct wifi_driver {

    /** common APIs */
    int (*init)(aos_dev_t *dev);
    int (*deinit)(aos_dev_t *dev);
    int (*reset)(aos_dev_t *dev);
    int (*set_mode)(aos_dev_t *dev, wifi_mode_t mode);
    int (*get_mode)(aos_dev_t *dev, wifi_mode_t *mode);
    int (*install_event_cb)(aos_dev_t *dev, wifi_event_func *evt_cb);

    /** conf APIs */
    int (*set_protocol)(aos_dev_t *dev, uint8_t protocol_bitmap); //11bgn
    int (*get_protocol)(aos_dev_t *dev, uint8_t *protocol_bitmap);
    int (*set_country)(aos_dev_t *dev, wifi_country_t country);
    int (*get_country)(aos_dev_t *dev, wifi_country_t *country);
    int (*set_mac_addr)(aos_dev_t *dev, const uint8_t *mac);
    int (*get_mac_addr)(aos_dev_t *dev, uint8_t *mac);
    int (*set_auto_reconnect)(aos_dev_t *dev, bool en);
    int (*get_auto_reconnect)(aos_dev_t *dev, bool *en);
    int (*set_lpm)(aos_dev_t *dev, wifi_lpm_mode_t mode); //ps on/pff
    int (*get_lpm)(aos_dev_t *dev, wifi_lpm_mode_t *mode);
    int (*power_on)(aos_dev_t *dev); //the wifi module power on/off
    int (*power_off)(aos_dev_t *dev); 

    /** connection APIs */
    int (*start_scan)(aos_dev_t *dev, wifi_scan_config_t *config, bool block);
    int (*start)(aos_dev_t *dev, wifi_config_t * config); //start ap or sta
    int (*stop)(aos_dev_t *dev);//stop ap or sta
    int (*sta_get_link_status)(aos_dev_t *dev, wifi_ap_record_t *ap_info);
    int (*ap_get_sta_list)(aos_dev_t *dev, wifi_sta_list_t *sta);


    /** promiscuous APIs */
    int (*start_monitor)(aos_dev_t *dev, wifi_promiscuous_cb_t cb);
    int (*stop_monitor)(aos_dev_t *dev);
    int (*send_80211_raw_frame)(aos_dev_t *dev, void *buffer, uint16_t len);
    int (*set_channel)(aos_dev_t *dev, uint8_t primary, wifi_second_chan_t second);
    int (*get_channel)(aos_dev_t *dev, uint8_t *primary, wifi_second_chan_t *second);


    /* esp8266 related API */
    int (*set_smartcfg)(aos_dev_t *dev, int enable);

} wifi_driver_t;

#ifdef __cplusplus
}
#endif

#endif


