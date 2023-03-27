/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lwip/apps/dhcps.h>
#include <lwip/netifapi.h>
#include <lwip/dns.h>

#include <devices/netdrv.h>
#include <devices/impl/net_impl.h>
#include <devices/impl/gprs_impl.h>

#include <usbh_core.h>

#include "ec200a_api.h"

#define TAG "ec200a_dev"

typedef struct {
    rvm_dev_t device;
    uint8_t mode;

    void *priv;
} ec200a_gprs_dev_t;

extern struct netif rndis_netif;

static rvm_dev_t *ec200a_dev_init(driver_t *drv, void *config, int id)
{
    rvm_dev_t *dev = rvm_hal_device_new(drv, sizeof(ec200a_gprs_dev_t), id);

    return dev;
}

#define ec200a_dev_uninit rvm_hal_device_free

static int ec200a_dev_open(rvm_dev_t *dev)
{

    return 0;
}

static int ec200a_dev_close(rvm_dev_t *dev)
{

    return 0;
}

/*****************************************
* common netif driver interface
******************************************/
static int ec200a_set_mac_addr(rvm_dev_t *dev, const uint8_t *mac)
{
    LOGE(TAG, "%s not support", __func__);
    return -1;
}

static void net_status_callback(struct netif *netif)
{
    if (!ip_addr_isany(ip_2_ip4(&netif->ip_addr))) {
        event_publish(EVENT_NET_GOT_IP, NULL);
    }
}

static int ec200a_start_dhcp(rvm_dev_t *dev)
{
    struct netif *netif = &rndis_netif; //netif_find("en0");
    aos_check_return_einval(netif);

    if (!netif_is_link_up(netif)) {
        return -1;
    }

    netif_set_ipaddr(netif, NULL);
    netif_set_netmask(netif, NULL);
    netif_set_gw(netif, NULL);

    netif_set_status_callback(netif, net_status_callback);

    return netifapi_dhcp_start(netif);
}

static int ec200a_stop_dhcp(rvm_dev_t *dev)
{
    struct netif *netif = &rndis_netif;
    aos_check_return_einval(netif);

    netifapi_dhcp_stop(netif);

    return 0;
}

static int ec200a_set_ipaddr(rvm_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw)
{
    return -1;
}

static int ec200a_get_ipaddr(rvm_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask_addr, ip_addr_t *gw_addr)
{
    struct netif *netif = &rndis_netif; //netif_find("en0");
    aos_check_return_einval(netif && ipaddr && netmask_addr && gw_addr);

    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(ipaddr), *netif_ip_addr4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(gw_addr), *netif_ip_gw4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(netmask_addr), *netif_ip_netmask4(netif));

    return 0;
}

static int ec200a_subscribe(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    if (cb) {
        event_subscribe(event, cb, param);
    }

    return 0;
}

/*****************************************
* gprs driver interface
******************************************/
static int ec200a_set_mode(rvm_dev_t *dev, rvm_hal_gprs_mode_t mode)
{

    return 0;
}

static int ec200a_reset(rvm_dev_t *dev)
{
    // int ret;

    // LOGE(TAG, "ec200a reset ok");
    return 0;
}

static int ec200a_init_check(rvm_dev_t *dev)
{
    rvm_hal_sig_qual_resp_t  signal_quality;
    rvm_hal_regs_stat_resp_t reg_stat;
    int retry = 0;
    int ret;
    int net_type;

#if 0
    ret = ec200a_get_state_pin();
    if(ret <= 0) {
        ec200a_hard_reset();
    }
#endif

    retry = 3;
    do {
        ret = ec200a_close_echo();
    } while(ret < 0 && retry--);

    if (ret < 0) {
        LOGE(TAG, "ec200a close echo failed");
        goto check_err;
    }


    retry = 3;
    do {
        ret = ec200a_check_net_type(&net_type);
    } while(ret < 0 && retry--);

    if (net_type != 3) {
        LOGE(TAG, "net_type error set type use \"AT+QCFG=\"usbnet\",3\" and reboot");
        ec200a_set_net_type(3);
        goto check_err;
    }

    retry = 5;

    do {
        ret = ec200a_check_simcard_is_insert(0);
        sleep(1);
    } while(ret < 0 && retry--);


    if (ret < 0) {
        LOGE(TAG, "simcard is not inside");
        goto check_err;
    }

    retry = 10;
    do {
        ret = ec200a_check_signal_quality(&signal_quality, 0);

        if (ret < 0 || signal_quality.rssi <= 0 || signal_quality.rssi == 99) {
            sleep(1);
        }
    } while (signal_quality.rssi <= 0 && retry--);

    if(retry == 0) {
        LOGI(TAG, "gprs sig_qua err(%d)", signal_quality.rssi);
        goto check_err;
    }

    retry = 10;
    do {
        ret = ec200a_check_register_status(&reg_stat, 0);

        if (reg_stat.stat != 1) {
            sleep(1);
        }
    } while (reg_stat.stat != 1 && retry--);

    if(retry == 0) {
        LOGI(TAG, "gprs reg status err");
        goto check_err;
    }

    if(retry == 0) {
        LOGI(TAG, "gprs server state err");
        goto check_err;
    }

    return 0;
check_err:
    event_publish(EVENT_GPRS_LINK_DOWN, NULL);
    return -1;
}

static void connect_entry(void *argv)
{
    struct usbh_rndis *rndis_class;

    while (1) {
        rndis_class = (struct usbh_rndis *)usbh_find_class_instance("/dev/rndis");

        if (rndis_class) {
                ec200a_start_network();
                netif_set_link_up(&rndis_netif);
                event_publish(EVENT_GPRS_LINK_UP, NULL);
                return;
        }

        aos_msleep(100);
    }
}

static int ec200a_connect_to_gprs(rvm_dev_t *dev)
{
    aos_task_t task_handle;

    ec200a_connect_to_network(0);

    aos_task_new_ext(&task_handle, "ec200a", connect_entry, dev, 2048, AOS_DEFAULT_APP_PRI);
    return 0;
}

static int ec200a_disconnect_gprs(rvm_dev_t *dev)
{

    return 0;
}

static int ec200a_ping_remote(rvm_dev_t *dev, int type, char *remote_ip)
{

    return 0;
}

static int ec200a_get_link_status(rvm_dev_t *dev, rvm_hal_gprs_status_link_t *link_status)
{
    return 0;
}

static int ec200a_get_csq(rvm_dev_t *dev, int *csq)
{
    int ret;
    rvm_hal_sig_qual_resp_t signal_quality;

    ret = ec200a_check_signal_quality(&signal_quality, 0);

    if(ret == 0) {
        *csq = signal_quality.rssi;
    }

    return ret;
}

static int ec200a_get_simcard_info(rvm_dev_t *dev, char ccid[21], int *insert)
{
    int ret = ec200a_check_simcard_is_insert(0);

    if(ret == 0) {
        *insert = 0;
        ret = ec200a_get_ccid(ccid);
    } else {
        *insert = -1;
    }
    return ret;
}

static net_ops_t ec200a_net_driver = {
    .set_mac_addr = ec200a_set_mac_addr,
    .start_dhcp = ec200a_start_dhcp,
    .stop_dhcp = ec200a_stop_dhcp,
    .set_ipaddr = ec200a_set_ipaddr,
    .get_ipaddr = ec200a_get_ipaddr,
    .subscribe  = ec200a_subscribe,
    .ping       = ec200a_ping_remote,
};

static gprs_driver_t ec200a_gprs_driver = {
    .reset = ec200a_reset,
    .set_mode = ec200a_set_mode,
    .module_init_check = ec200a_init_check,
    .connect_to_gprs = ec200a_connect_to_gprs,
    .disconnect_from_gprs = ec200a_disconnect_gprs,
    .get_link_status = ec200a_get_link_status,
    .get_csq        = ec200a_get_csq,
    .get_simcard_info = ec200a_get_simcard_info,
};

static netdev_driver_t ec200a_driver = {
    .drv = {
        .name   = "gprs",
        .init   = ec200a_dev_init,
        .uninit = ec200a_dev_uninit,
        .open   = ec200a_dev_open,
        .close  = ec200a_dev_close,
    },
    .link_type = NETDEV_TYPE_GPRS,
    .net_ops =  &ec200a_net_driver,
    .link_ops = &ec200a_gprs_driver,
};

void drv_ec200a_rndis_register()
{
    int ret = ec200a_module_init("usb_serial0");

    if (ret < 0) {
        LOGI(TAG, "ec200a driver init error");
        return;
    }

    ec200a_lwip_init();

    //run ec200a_dev_init to create gprs_dev_t and bind this driver
    ret = rvm_driver_register(&ec200a_driver.drv, NULL, 0);

    if (ret < 0) {
        LOGI(TAG, "ec200a device register error");
    }
}