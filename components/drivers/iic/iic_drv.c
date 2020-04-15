/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/log.h>
#include <aos/kernel.h>
#include <drv/iic.h>

#include "hal/iic_impl.h"

#define TAG "iic_drv"

typedef struct {
    aos_dev_t          device;
    iic_handle_t   handle;
    char          *recv_buf;
    volatile int   event_flag;
    iic_config_t   config;
} iic_dev_t;

#define iic(dev) ((iic_dev_t *)dev)

static aos_dev_t *iic_csky_init(driver_t *drv, void *config, int id)
{
    iic_dev_t *iic = (iic_dev_t *)device_new(drv, sizeof(iic_dev_t), id);

    return (aos_dev_t *)iic;
}

#define iic_csky_uninit device_free

static void iic_event_cb_fun(int32_t idx, iic_event_e event)
{
    static iic_dev_t *iic_idx[4];
    iic_dev_t *iic;

    if (iic_idx[idx] == NULL) {
        iic_idx[idx] = (iic_dev_t *)device_find("iic", idx);
    }

    iic = iic_idx[idx];

    // iic->evt_id = event;

    switch (event) {
        case IIC_EVENT_TRANSFER_DONE:
            iic->event_flag = 1 << IIC_EVENT_TRANSFER_DONE;
            break;

        case IIC_EVENT_BUS_ERROR:
            csi_iic_abort_transfer(iic->handle);
            iic->event_flag = 1 << IIC_EVENT_BUS_ERROR;
            break;
        default: break;
    }
}

static int iic_csky_open(aos_dev_t *dev)
{
    iic_handle_t iic_handle = csi_iic_initialize(dev->id, iic_event_cb_fun);

    if (iic_handle == NULL) {
        printf("csi_iic_initialize error\n");
        return -1;
    }
    iic(dev)->handle = iic_handle;
    return 0;
}

static int iic_csky_close(aos_dev_t *dev)
{
    csi_iic_uninitialize(iic(dev)->handle);
    return 0;
}

static int iic_csky_config(aos_dev_t *dev, iic_config_t *config)
{
    int32_t ret = csi_iic_config(iic(dev)->handle, config->mode, config->speed,
                                 config->addr_mode, config->slave_addr);

    if (ret < 0) {
        return -EIO;
    }

    memcpy(&iic(dev)->config, config, sizeof(iic_config_t));

    return 0;
}

static int iic_busy_stat(aos_dev_t *dev)
{
    iic_status_t stat = csi_iic_get_status(iic(dev)->handle);

    if (stat.busy != 0) {
        LOGD(TAG, "iic(%d) read busy", dev->id);
        csi_iic_abort_transfer(iic(dev)->handle);
        return -EBUSY;
    }

    return 0;
}

static int iic_csky_send(aos_dev_t *dev, uint8_t dev_addr, const void *data, uint32_t size)
{
    int ret;

    if (iic_busy_stat(dev) < 0) {
        return -1;
    }

    iic(dev)->event_flag = 0;
    ret = csi_iic_master_send(iic(dev)->handle, dev_addr/*iic(dev)->config.slave_addr*/, data, size, 0);

    if (ret < 0) {
        LOGD(TAG, "iic send fail");
    }

    while(!iic(dev)->event_flag){}

    if (iic(dev)->event_flag & (1 << IIC_EVENT_BUS_ERROR)) {
        return -1;
    }

    return size;
}

static int iic_csky_recv(aos_dev_t *dev, uint8_t dev_addr, void *data, uint32_t size)
{
    int ret;

    if (iic_busy_stat(dev) < 0) {
        return -1;
    }

    iic(dev)->event_flag = 0;
    ret = csi_iic_master_receive(iic(dev)->handle, dev_addr, data, size, 0);

    if (ret < 0) {
        LOGD(TAG, "iic recv fail");
    }

    while(!iic(dev)->event_flag){}

    if (iic(dev)->event_flag & (1 << IIC_EVENT_BUS_ERROR)) {
        return -1;
    }

    return 0;
}

static iic_driver_t iic_driver = {
    .drv = {
        .name   = "iic",
        .init   = iic_csky_init,
        .uninit = iic_csky_uninit,
        .open   = iic_csky_open,
        .close  = iic_csky_close,
    },
    .config          = iic_csky_config,
    .send            = iic_csky_send,
    .recv            = iic_csky_recv,
};

void iic_csky_register(int idx)
{
    driver_register(&iic_driver.drv, NULL, idx);
}
