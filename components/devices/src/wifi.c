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

#if defined(CONFIG_SAL) || defined(CONFIG_TCPIP)
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/wifi.h>
#include <devices/impl/wifi_impl.h>

#define WIFI_DRIVER(dev)  ((wifi_driver_t *)(((netdev_driver_t *)dev->drv)->link_ops))
#define WIFI_VALID(dev) do { \
        if (device_valid(dev, "wifi") != 0) \
            return -1; \
    } while(0)


int rvm_hal_wifi_init(rvm_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->init(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_wifi_deinit(rvm_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->deinit(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_wifi_reset(rvm_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->reset(dev);
    device_unlock(dev);

    return ret;
}


int rvm_hal_wifi_set_mode(rvm_dev_t *dev, rvm_hal_wifi_mode_t mode)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_mode(dev, mode);
    device_unlock(dev);

    return ret;
}


int rvm_hal_wifi_get_mode(rvm_dev_t *dev, rvm_hal_wifi_mode_t *mode)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_mode(dev, mode);
    device_unlock(dev);

    return ret;
}


int rvm_hal_wifi_install_event_cb(rvm_dev_t *dev, rvm_hal_wifi_event_func *evt_cb)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->install_event_cb(dev, evt_cb);
    device_unlock(dev);

    return ret;
}


int rvm_hal_wifi_set_protocol(rvm_dev_t *dev, uint8_t protocol_bitmap)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_protocol(dev, protocol_bitmap);
    device_unlock(dev);

    return ret;
}

int rvm_hal_wifi_get_protocol(rvm_dev_t *dev, uint8_t *protocol_bitmap)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_protocol(dev, protocol_bitmap);
    device_unlock(dev);

    return ret;
}



int rvm_hal_wifi_set_country(rvm_dev_t *dev, rvm_hal_wifi_country_t country)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_country(dev, country);
    device_unlock(dev);

    return ret;
}


int rvm_hal_wifi_get_country(rvm_dev_t *dev, rvm_hal_wifi_country_t *country)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_country(dev, country);
    device_unlock(dev);

    return ret;
}



int rvm_hal_wifi_set_mac_addr(rvm_dev_t *dev, const uint8_t *mac)
{

    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_mac_addr(dev, mac);
    device_unlock(dev);

    return ret;
}


int rvm_hal_wifi_get_mac_addr(rvm_dev_t *dev, uint8_t *mac)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_mac_addr(dev, mac);
    device_unlock(dev);

    return ret;
}
// For some Libraries
int hal_wifi_get_mac_addr(rvm_dev_t *dev, uint8_t *mac)
{
    return rvm_hal_wifi_get_mac_addr(dev, mac);
}


int rvm_hal_wifi_set_auto_reconnect(rvm_dev_t *dev, bool en)
{

    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_auto_reconnect(dev, en);
    device_unlock(dev);

    return ret;
}


int rvm_hal_wifi_get_auto_reconnect(rvm_dev_t *dev, bool *en)
{

    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_auto_reconnect(dev, en);
    device_unlock(dev);

    return ret;
}

int rvm_hal_wifi_set_lpm(rvm_dev_t *dev, rvm_hal_wifi_lpm_mode_t mode)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_lpm(dev, mode);
    device_unlock(dev);

    return ret;
}


int rvm_hal_wifi_get_lpm(rvm_dev_t *dev, rvm_hal_wifi_lpm_mode_t *mode)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_lpm(dev, mode);
    device_unlock(dev);

    return ret;
}


int rvm_hal_wifi_power_on(rvm_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->power_on(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_wifi_power_off(rvm_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->power_off(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_wifi_start_scan(rvm_dev_t *dev, wifi_scan_config_t *config, bool block)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->start_scan(dev, config, block);
    device_unlock(dev);

    return ret;
}



int rvm_hal_wifi_start(rvm_dev_t *dev, rvm_hal_wifi_config_t *config)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->start(dev, config);
    device_unlock(dev);

    return ret;
}



int rvm_hal_wifi_stop(rvm_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->stop(dev);
    device_unlock(dev);

    return ret;
}


int rvm_hal_wifi_sta_get_link_status(rvm_dev_t *dev, rvm_hal_wifi_ap_record_t *ap_info)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->sta_get_link_status(dev, ap_info);
    device_unlock(dev);

    return ret;

}


int rvm_hal_wifi_ap_get_sta_list(rvm_dev_t *dev, rvm_hal_wifi_sta_list_t *sta)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->ap_get_sta_list(dev, sta);
    device_unlock(dev);

    return ret;

}


int rvm_hal_wifi_start_monitor(rvm_dev_t *dev, rvm_hal_wifi_promiscuous_cb_t cb)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->start_monitor(dev, cb);
    device_unlock(dev);

    return ret;

}


int rvm_hal_wifi_stop_monitor(rvm_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->stop_monitor(dev);
    device_unlock(dev);

    return ret;

}

int rvm_hal_wifi_start_mgnt_monitor(rvm_dev_t *dev, rvm_hal_wifi_mgnt_cb_t cb)
{
    int ret;
    WIFI_VALID(dev);
    device_lock(dev);
    ret = WIFI_DRIVER(dev)->start_mgnt_monitor(dev, cb);
    device_unlock(dev);
    return ret;
}

int rvm_hal_wifi_stop_mgnt_monitor(rvm_dev_t *dev)
{
    int ret;
    WIFI_VALID(dev);
    device_lock(dev);
    ret = WIFI_DRIVER(dev)->stop_mgnt_monitor(dev);
    device_unlock(dev);
    return ret;
}

int rvm_hal_wifi_send_80211_raw_frame(rvm_dev_t *dev, void *buffer, uint16_t len)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->send_80211_raw_frame(dev, buffer, len);
    device_unlock(dev);

    return ret;

}


int rvm_hal_wifi_set_channel(rvm_dev_t *dev, uint8_t primary, rvm_hal_wifi_second_chan_t second)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_channel(dev, primary, second);
    device_unlock(dev);

    return ret;
}


int rvm_hal_wifi_get_channel(rvm_dev_t *dev, uint8_t *primary, rvm_hal_wifi_second_chan_t *second)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_channel(dev, primary, second);
    device_unlock(dev);

    return ret;
}




int rvm_hal_wifi_set_smartcfg(rvm_dev_t *dev, int enable)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_smartcfg(dev, enable);
    device_unlock(dev);

    return ret;
}

#endif

