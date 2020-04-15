/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <devices/hal/gprs_impl.h>

#define GPRS_DRIVER(dev)  ((gprs_driver_t *)(((netdev_driver_t *)dev->drv)->link_ops))
#define GPRS_VALID(dev) do { \
        if (device_valid(dev, "gprs") != 0) \
            return -1; \
    } while(0)

int hal_gprs_set_mode(aos_dev_t *dev, gprs_mode_t mode)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->set_mode(dev, mode);
    device_unlock(dev);

    return ret;
}

int hal_gprs_get_mode(aos_dev_t *dev, gprs_mode_t *mode)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_mode(dev, mode);
    device_unlock(dev);

    return ret;
}

int hal_gprs_reset(aos_dev_t *dev)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->reset(dev);
    device_unlock(dev);

    return ret;
}

int hal_gprs_start(aos_dev_t *dev)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->start(dev);
    device_unlock(dev);

    return ret;
}

int hal_gprs_stop(aos_dev_t *dev)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->stop(dev);
    device_unlock(dev);

    return ret;
}

/*configuration*/
int hal_gprs_set_if_config(aos_dev_t *dev, uint32_t baud, uint8_t flow_control)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->set_if_config(dev, baud, flow_control);
    device_unlock(dev);

    return ret;
}

/*connection*/
int hal_gprs_module_init_check(aos_dev_t *dev)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->module_init_check(dev);
    device_unlock(dev);

    return ret;
}

int hal_gprs_connect_to_gprs(aos_dev_t *dev)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->connect_to_gprs(dev);
    device_unlock(dev);

    return ret;
}

int hal_gprs_disconnect_from_gprs(aos_dev_t *dev)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->disconnect_from_gprs(dev);
    device_unlock(dev);

    return ret;
}

int hal_gprs_sms_send(aos_dev_t *dev, char *sca, char *da, char *content)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->sms_send(dev, sca, da, content);
    device_unlock(dev);

    return ret;
}

int hal_gprs_sms_set_cb(aos_dev_t *dev, recv_sms_cb cb)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->sms_set_cb(dev, cb);
    device_unlock(dev);

    return ret;
}

int hal_gprs_get_imsi(aos_dev_t *dev, char *imsi)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_imsi(dev, imsi);
    device_unlock(dev);

    return ret;
}

int hal_gprs_get_imei(aos_dev_t *dev, char *imei)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_imei(dev, imei);
    device_unlock(dev);

    return ret;
}

int hal_gprs_get_link_status(aos_dev_t *dev, gprs_status_link_t *link_status)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_link_status(dev, link_status);
    device_unlock(dev);

    return ret;
}

int hal_gprs_get_ipaddr(aos_dev_t *dev, char ip[16])
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_ipaddr(dev, ip);
    device_unlock(dev);

    return ret;
}

int hal_gprs_get_csq(aos_dev_t *dev, int *csq)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_csq(dev, csq);
    device_unlock(dev);

    return ret;
}

int hal_gprs_get_simcard_info(aos_dev_t *dev, char ccid[21], int *insert)
{
    int ret;

    GPRS_VALID(dev);

    device_lock(dev);
    ret = GPRS_DRIVER(dev)->get_simcard_info(dev, ccid, insert);
    device_unlock(dev);

    return ret;
}