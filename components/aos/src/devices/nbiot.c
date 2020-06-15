/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <devices/hal/nbiot_impl.h>

#define NBIOT_DRIVER(dev)  ((nbiot_driver_t *)(((netdev_driver_t *)dev->drv)->link_ops))
#define NBIOT_VALID(dev) do { \
        if (device_valid(dev, "nbiot") != 0) \
            return -1; \
    } while(0)


int hal_nbiot_start(aos_dev_t *dev)
{
    int ret;

    NBIOT_VALID(dev);

    device_lock(dev);
    ret = NBIOT_DRIVER(dev)->start_nbiot(dev);
    device_unlock(dev);

    return ret;
}

int hal_nbiot_stop(aos_dev_t *dev)
{
    int ret;

    NBIOT_VALID(dev);

    device_lock(dev);
    ret = NBIOT_DRIVER(dev)->stop_nbiot(dev);
    device_unlock(dev);

    return ret;
}

int hal_nbiot_get_imsi(aos_dev_t *dev, nbiot_imsi_t *imsi)
{
    int ret;

    NBIOT_VALID(dev);

    device_lock(dev);
    ret = NBIOT_DRIVER(dev)->get_imsi(dev, imsi);
    device_unlock(dev);

    return ret;
}

int hal_nbiot_get_imei(aos_dev_t *dev, nbiot_imei_t *imei)
{
    int ret;

    NBIOT_VALID(dev);

    device_lock(dev);
    ret = NBIOT_DRIVER(dev)->get_imei(dev, imei);
    device_unlock(dev);

    return ret;
}

int hal_nbiot_get_status(aos_dev_t *dev, nbiot_status_t *link_status)
{
    int ret;

    NBIOT_VALID(dev);

    device_lock(dev);
    ret = NBIOT_DRIVER(dev)->get_nbiot_status(dev, link_status);
    device_unlock(dev);

    return ret;
}

int hal_nbiot_get_csq(aos_dev_t *dev, int *csq)
{
    int ret;

    NBIOT_VALID(dev);

    device_lock(dev);
    ret = NBIOT_DRIVER(dev)->get_csq(dev, csq);
    device_unlock(dev);

    return ret;
}

int hal_nbiot_get_simcard_info(aos_dev_t *dev, nbiot_iccid_t *iccid, int *insert)
{
    int ret;

    NBIOT_VALID(dev);

    device_lock(dev);
    ret = NBIOT_DRIVER(dev)->get_simcard_info(dev, iccid, insert);
    device_unlock(dev);

    return ret;
}

int hal_nbiot_get_cell_info(aos_dev_t *dev, nbiot_cell_info_t *cellinfo)
{
    int ret;

    NBIOT_VALID(dev);

    device_lock(dev);
    ret = NBIOT_DRIVER(dev)->get_cell_info(dev, cellinfo);
    device_unlock(dev);

    return ret;
}


int hal_nbiot_set_status_ind(aos_dev_t *dev, int status)
{
    int ret;

    NBIOT_VALID(dev);

    device_lock(dev);
    ret = NBIOT_DRIVER(dev)->set_nbiot_status_ind(dev, status);
    device_unlock(dev);

    return ret;
}

int hal_nbiot_set_signal_strength_ind(aos_dev_t *dev, int status)
{
    int ret;

    NBIOT_VALID(dev);

    device_lock(dev);
    ret = NBIOT_DRIVER(dev)->set_nbiot_signal_strength_ind(dev, status);
    device_unlock(dev);

    return ret;
}
