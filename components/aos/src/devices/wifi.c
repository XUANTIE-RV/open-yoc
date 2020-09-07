/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <devices/wifi.h>
#include <devices/hal/wifi_impl.h>

#define WIFI_DRIVER(dev)  ((wifi_driver_t *)(((netdev_driver_t *)dev->drv)->link_ops))
#define WIFI_VALID(dev) do { \
        if (device_valid(dev, "wifi") != 0) \
            return -1; \
    } while(0)


int hal_wifi_init(aos_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->init(dev);
    device_unlock(dev);

    return ret;
}

int hal_wifi_deinit(aos_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->deinit(dev);
    device_unlock(dev);

    return ret;
}

int hal_wifi_reset(aos_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->reset(dev);
    device_unlock(dev);

    return ret;
}


int hal_wifi_set_mode(aos_dev_t *dev, wifi_mode_t mode)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_mode(dev, mode);
    device_unlock(dev);

    return ret;
}


int hal_wifi_get_mode(aos_dev_t *dev, wifi_mode_t *mode)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_mode(dev, mode);
    device_unlock(dev);

    return ret;
}


int hal_wifi_install_event_cb(aos_dev_t *dev, wifi_event_func *evt_cb)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->install_event_cb(dev, evt_cb);
    device_unlock(dev);

    return ret;
}


int hal_wifi_set_protocol(aos_dev_t *dev, uint8_t protocol_bitmap)
{
	int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_protocol(dev, protocol_bitmap);
    device_unlock(dev);

    return ret;
}

int hal_wifi_get_protocol(aos_dev_t *dev, uint8_t *protocol_bitmap)
{
	int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_protocol(dev, protocol_bitmap);
    device_unlock(dev);

    return ret;
}



int hal_wifi_set_country(aos_dev_t *dev, wifi_country_t country)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_country(dev, country);
    device_unlock(dev);

    return ret;
}


int hal_wifi_get_country(aos_dev_t *dev, wifi_country_t *country)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_country(dev, country);
    device_unlock(dev);

    return ret;
}



int hal_wifi_set_mac_addr(aos_dev_t *dev, const uint8_t *mac)
{

    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_mac_addr(dev, mac);
    device_unlock(dev);

    return ret;
}


int hal_wifi_get_mac_addr(aos_dev_t *dev, uint8_t *mac)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_mac_addr(dev, mac);
    device_unlock(dev);

    return ret;
}


int hal_wifi_set_auto_reconnect(aos_dev_t *dev, bool en)
{

    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_auto_reconnect(dev, en);
    device_unlock(dev);

    return ret;
}


int hal_wifi_get_auto_reconnect(aos_dev_t *dev, bool *en)
{

    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_auto_reconnect(dev, en);
    device_unlock(dev);

    return ret;
}

int hal_wifi_set_lpm(aos_dev_t *dev, wifi_lpm_mode_t mode)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_lpm(dev, mode);
    device_unlock(dev);

    return ret;
}


int hal_wifi_get_lpm(aos_dev_t *dev, wifi_lpm_mode_t *mode)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_lpm(dev, mode);
    device_unlock(dev);

    return ret;
}


int hal_wifi_power_on(aos_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->power_on(dev);
    device_unlock(dev);

    return ret;
}

int hal_wifi_power_off(aos_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->power_off(dev);
    device_unlock(dev);

    return ret;
}




int hal_wifi_start_scan(aos_dev_t *dev, wifi_scan_config_t *config, bool block)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->start_scan(dev, config, block);
    device_unlock(dev);

    return ret;
}



int hal_wifi_start(aos_dev_t *dev, wifi_config_t *config)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->start(dev, config);
    device_unlock(dev);

    return ret;
}



int hal_wifi_stop(aos_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->stop(dev);
    device_unlock(dev);

    return ret;
}


int hal_wifi_sta_get_link_status(aos_dev_t *dev, wifi_ap_record_t *ap_info)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->sta_get_link_status(dev, ap_info);
    device_unlock(dev);

    return ret;

}


int hal_wifi_ap_get_sta_list(aos_dev_t *dev, wifi_sta_list_t *sta)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->ap_get_sta_list(dev, sta);
    device_unlock(dev);

    return ret;

}

	
int hal_wifi_start_monitor(aos_dev_t *dev, wifi_promiscuous_cb_t cb)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->start_monitor(dev, cb);
    device_unlock(dev);

    return ret;

}


int hal_wifi_stop_monitor(aos_dev_t *dev)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->stop_monitor(dev);
    device_unlock(dev);

    return ret;

}


int hal_wifi_send_80211_raw_frame(aos_dev_t *dev, void *buffer, uint16_t len)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->send_80211_raw_frame(dev, buffer, len);
    device_unlock(dev);

    return ret;

}


int hal_wifi_set_channel(aos_dev_t *dev, uint8_t primary, wifi_second_chan_t second)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_channel(dev, primary, second);
    device_unlock(dev);

    return ret;
}


int hal_wifi_get_channel(aos_dev_t *dev, uint8_t *primary, wifi_second_chan_t *second)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->get_channel(dev, primary, second);
    device_unlock(dev);

    return ret;
}




int hal_wifi_set_smartcfg(aos_dev_t *dev, int enable)
{
    int ret;

    WIFI_VALID(dev);

    device_lock(dev);
    ret = WIFI_DRIVER(dev)->set_smartcfg(dev, enable);
    device_unlock(dev);

    return ret;
}




