/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <yoc/uservice.h>

#include <devices/netdrv.h>
#include <devices/hal/nbiot_impl.h>
#include <devices/u1.h>

#include "u1_api.h"

#define TAG "u1_dev"

typedef struct {
    aos_dev_t device;
    void *priv;
} u1_dev_t;

/*****************************************
* common driver interface
******************************************/

static aos_dev_t *u1_dev_init(driver_t *drv, void *config, int id)
{
    aos_dev_t *dev = device_new(drv, sizeof(u1_dev_t), id);

    return dev;
}

static void u1_dev_uninit(aos_dev_t *dev)
{
    device_free(dev);
}

static int u1_dev_open(aos_dev_t *dev)
{
    return 0;
}

static int u1_dev_close(aos_dev_t *dev)
{
    return 0;
}

/*****************************************
* common netif driver interface
******************************************/
static int u1_set_mac_addr(aos_dev_t *dev, const uint8_t *mac)
{
    return 0;
}

static int u1_start_dhcp(aos_dev_t *dev)
{
    return 0;
}

static int u1_stop_dhcp(aos_dev_t *dev)
{
    return 0;
}

static int u1_set_ipaddr(aos_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw)
{
    return 0;
}

static int u1_get_ipaddr(aos_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask_addr, ip_addr_t *gw_addr)
{
    int ret ;
    char ip[16];

    ret = u1_get_local_ip(ip);
    if (ret < 0) {
        LOGE(TAG, "nbiot get_local_ip fail");
        return -1;
    }

    ipaddr_aton(ip, (ip4_addr_t *)ipaddr);

    return ret;
}

static int u1_subscribe(aos_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    if (cb) {
        event_subscribe(event, cb, param);
    }

    return 0;
}

int u1_ping_remote(aos_dev_t *dev, int type, char *remote_ip)
{
    return 0;
}

/*****************************************
* nbiot driver interface
******************************************/

static int u1_drv_start_nbiot(aos_dev_t *dev)
{
    return u1_start_nbiot();
}

static int u1_drv_stop_nbiot(aos_dev_t *dev)
{
    return u1_stop_nbiot();
}

static int u1_drv_reset_nbiot(aos_dev_t *dev)
{
    return u1_reset_nbiot();
}

static int u1_drv_get_nbiot_status(aos_dev_t *dev, nbiot_status_t *status)
{
    nbiot_regs_stat_resp_t status_resp;
    int ret;

    ret = u1_check_register_status(&status_resp);
    if (ret == 0) {
        if (status_resp.stat == NBIOT_STATUS_REGISTERED) {
            *status = NBIOT_STATUS_CONNECTED;
        } else {
            *status = NBIOT_STATUS_DISCONNECTED;
        }
    }

    return ret;
}

static int u1_drv_get_imsi(aos_dev_t *dev, nbiot_imsi_t *imsi)
{
    return u1_get_imsi(imsi);
}

static int u1_drv_get_imei(aos_dev_t *dev, nbiot_imei_t *imei)
{
    return u1_get_imei(imei);
}

static int u1_drv_get_csq(aos_dev_t *dev, int *csq)
{
    int ret;
    nbiot_sig_qual_resp_t signal_quality;

    ret = u1_check_signal_quality(&signal_quality);

    if(ret == 0) {
        *csq = signal_quality.rssi;
    }

    return ret;
}

static int u1_drv_get_simcard_info(aos_dev_t *dev, nbiot_iccid_t *iccid, int *insert)
{
    int card_status;
    int ret;

    ret = u1_check_simcard_is_insert(&card_status);

    if(ret == 0) {
        *insert = card_status;
        if (card_status == 1)
            ret = u1_get_iccid(iccid);
    } else {
        *insert = -1;
    }
    return ret;
}

static int u1_drv_get_cell_info(aos_dev_t *dev, nbiot_cell_info_t *cellinfo)
{
    return u1_get_cell_info(cellinfo);
}

static int u1_drv_set_nbiot_status_ind(aos_dev_t *dev, int status)
{
    return u1_set_register_status_ind(status);
}

static int u1_drv_set_nbiot_sig_ind(aos_dev_t *dev, int status)
{
    return u1_set_signal_strength_ind(status);
}

/*****************************************
* nbiot driver register
******************************************/

static net_ops_t u1_net_driver = {
    .set_mac_addr = u1_set_mac_addr,
    .start_dhcp = u1_start_dhcp,
    .stop_dhcp = u1_stop_dhcp,
    .set_ipaddr = u1_set_ipaddr,
    .get_ipaddr = u1_get_ipaddr,
    .subscribe  = u1_subscribe,
    .ping       = u1_ping_remote,
};

static nbiot_driver_t u1_nbiot_driver = {
    .start_nbiot = u1_drv_start_nbiot,
    .stop_nbiot = u1_drv_stop_nbiot,
    .reset_nbiot = u1_drv_reset_nbiot,
    .get_nbiot_status = u1_drv_get_nbiot_status,
    .get_imsi = u1_drv_get_imsi,
    .get_imei = u1_drv_get_imei,
    .get_csq        = u1_drv_get_csq,
    .get_simcard_info = u1_drv_get_simcard_info,
    .get_cell_info = u1_drv_get_cell_info,
    .set_nbiot_status_ind = u1_drv_set_nbiot_status_ind,
    .set_nbiot_signal_strength_ind = u1_drv_set_nbiot_sig_ind
};

static netdev_driver_t u1_driver = {
    .drv = {
        .name   = "nbiot",
        .init   = u1_dev_init,
        .uninit = u1_dev_uninit,
        .open   = u1_dev_open,
        .close  = u1_dev_close,
    },
    .net_ops = &u1_net_driver,
    .link_type = NETDEV_TYPE_NBIOT,
    .link_ops = &u1_nbiot_driver,
};

void nbiot_u1_register(utask_t *task, u1_nbiot_param_t *param)
{
    int ret = u1_module_init(task, param);

    if (ret < 0) {
        LOGI(TAG, "u1 driver init error");
        return;
    }

    ret = driver_register(&u1_driver.drv, param, 0);

    if (ret < 0) {
        LOGI(TAG, "u1 device register error");
    }
}

