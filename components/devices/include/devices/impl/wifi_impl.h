 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HAL_WIFI_IMPL_H
#define HAL_WIFI_IMPL_H


#include <aos/aos.h>

#include <devices/impl/net_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

//30个HAL接口
typedef struct wifi_driver {

    /** common APIs */
    int (*init)(rvm_dev_t *dev);
    int (*deinit)(rvm_dev_t *dev);
    int (*reset)(rvm_dev_t *dev);
    int (*set_mode)(rvm_dev_t *dev, rvm_hal_wifi_mode_t mode);
    int (*get_mode)(rvm_dev_t *dev, rvm_hal_wifi_mode_t *mode);
    int (*install_event_cb)(rvm_dev_t *dev, rvm_hal_wifi_event_func *evt_cb);

    /** conf APIs */
    int (*set_protocol)(rvm_dev_t *dev, uint8_t protocol_bitmap); //11bgn
    int (*get_protocol)(rvm_dev_t *dev, uint8_t *protocol_bitmap);
    int (*set_country)(rvm_dev_t *dev, rvm_hal_wifi_country_t country);
    int (*get_country)(rvm_dev_t *dev, rvm_hal_wifi_country_t *country);
    int (*set_mac_addr)(rvm_dev_t *dev, const uint8_t *mac);
    int (*get_mac_addr)(rvm_dev_t *dev, uint8_t *mac);
    int (*set_auto_reconnect)(rvm_dev_t *dev, bool en);
    int (*get_auto_reconnect)(rvm_dev_t *dev, bool *en);
    int (*set_lpm)(rvm_dev_t *dev, rvm_hal_wifi_lpm_mode_t mode); //ps on/pff
    int (*get_lpm)(rvm_dev_t *dev, rvm_hal_wifi_lpm_mode_t *mode);
    int (*power_on)(rvm_dev_t *dev); //the wifi module power on/off
    int (*power_off)(rvm_dev_t *dev);

    /** connection APIs */
    int (*start_scan)(rvm_dev_t *dev, wifi_scan_config_t *config, bool block);
    int (*start)(rvm_dev_t *dev, rvm_hal_wifi_config_t * config); //start ap or sta
    int (*stop)(rvm_dev_t *dev);//stop ap or sta
    int (*sta_get_link_status)(rvm_dev_t *dev, rvm_hal_wifi_ap_record_t *ap_info);
    int (*ap_get_sta_list)(rvm_dev_t *dev, rvm_hal_wifi_sta_list_t *sta);


    /** promiscuous APIs */
    int (*start_monitor)(rvm_dev_t *dev, rvm_hal_wifi_promiscuous_cb_t cb);
    int (*stop_monitor)(rvm_dev_t *dev);
    int (*start_mgnt_monitor)(rvm_dev_t *dev, rvm_hal_wifi_mgnt_cb_t cb);
    int (*stop_mgnt_monitor)(rvm_dev_t *dev);
    int (*send_80211_raw_frame)(rvm_dev_t *dev, void *buffer, uint16_t len);
    int (*set_channel)(rvm_dev_t *dev, uint8_t primary, rvm_hal_wifi_second_chan_t second);
    int (*get_channel)(rvm_dev_t *dev, uint8_t *primary, rvm_hal_wifi_second_chan_t *second);


    /* esp8266 related API */
    int (*set_smartcfg)(rvm_dev_t *dev, int enable);

} wifi_driver_t;

#ifdef __cplusplus
}
#endif

#endif


