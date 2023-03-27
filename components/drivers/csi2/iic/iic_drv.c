/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/kernel.h>
#include <drv/iic.h>
#include <drv/clk.h>
#include <devices/impl/iic_impl.h>

#define TAG "iic_drv"

typedef struct {
    rvm_dev_t      device;
    csi_iic_t      handle;
    aos_event_t    event;
    rvm_hal_iic_config_t   config;
} iic_dev_t;

#define EVT_SEND_DONE       1
#define EVT_RECV_DONE       1

#define iic(dev) ((iic_dev_t *)dev)

static void iic_csky_event_cb_fun(csi_iic_t *iic_handler, csi_iic_event_t event, void *arg)
{
    rvm_dev_t *dev = (rvm_dev_t *)arg;

    if (event == IIC_EVENT_SEND_COMPLETE) {
        if (aos_event_is_valid(&iic(dev)->event)) {
            aos_event_set(&iic(dev)->event, EVT_SEND_DONE, AOS_EVENT_OR);
        }
    } else if (event == IIC_EVENT_RECEIVE_COMPLETE) {
        if (aos_event_is_valid(&iic(dev)->event)) {
            aos_event_set(&iic(dev)->event, EVT_RECV_DONE, AOS_EVENT_OR);
        }
    } else {
        printf("i2c err event %d\n", event);
    }
}

static rvm_dev_t *iic_csky_init(driver_t *drv, void *config, int id)
{
    iic_dev_t *iic = (iic_dev_t *)rvm_hal_device_new(drv, sizeof(iic_dev_t), id);

    return (rvm_dev_t *)iic;
}

#define iic_csky_uninit rvm_hal_device_free

static int iic_csky_open(rvm_dev_t *dev)
{
    csi_error_t ret = csi_iic_init(&iic(dev)->handle, dev->id);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_iic_init error");
        return -1;
    }
    aos_event_new(&iic(dev)->event, 0);
    return 0;
}

static int iic_csky_close(rvm_dev_t *dev)
{
    csi_iic_uninit(&iic(dev)->handle);
    csi_iic_detach_callback(&iic(dev)->handle);
    aos_event_free(&iic(dev)->event);
    return 0;
}

static int iic_csky_clock(rvm_dev_t *dev, bool enable)
{
    if (enable) {
        csi_clk_enable(&iic(dev)->handle.dev);
    } else {
        csi_clk_disable(&iic(dev)->handle.dev);
    }
    return 0;
}

static int iic_csky_config(rvm_dev_t *dev, rvm_hal_iic_config_t *config)
{
    csi_error_t ret = csi_iic_mode(&iic(dev)->handle, config->mode);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_iic set mode error");
        return -1;
    }

    ret = csi_iic_addr_mode(&iic(dev)->handle, config->addr_mode);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_iic_addr_mode error");
        return -1;
    }

    ret = csi_iic_speed(&iic(dev)->handle, config->speed);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_iic set speed error");
        return -1;
    }

    if (config->mode == MODE_SLAVE) {
        ret = csi_iic_own_addr(&iic(dev)->handle, config->slave_addr);
        if (ret != CSI_OK) {
            LOGE(TAG, "set csi_iic_own_addr error");
            return -1;
        }
    }

    ret = csi_iic_attach_callback(&iic(dev)->handle, iic_csky_event_cb_fun, dev);
    if (ret != CSI_OK) {
        return -EIO;
    }

    memcpy(&iic(dev)->config, config, sizeof(rvm_hal_iic_config_t));

    return 0;
}

static int iic_csky_master_send(rvm_dev_t *dev, uint16_t dev_addr, const void *data, uint32_t size, uint32_t timeout)
{
    unsigned int flags = 0;

    csi_error_t ret = csi_iic_master_send_async(&iic(dev)->handle, dev_addr, data, size);
    if (ret < 0) {
        LOGE(TAG, "iic send fail");
        return -1;
    }

    aos_event_get(&iic(dev)->event, EVT_SEND_DONE, AOS_EVENT_OR_CLEAR, &flags, timeout);

    if (flags & EVT_SEND_DONE) {
        return 0;
    } else {
        return -1;
    }

    return -ETIMEDOUT;
}

static int iic_csky_master_recv(rvm_dev_t *dev, uint16_t dev_addr, void *data, uint32_t size, uint32_t timeout)
{
    int ret;
    unsigned int flags = 0;

    ret = csi_iic_master_receive_async(&iic(dev)->handle, dev_addr, data, size);

    if (ret < 0) {
        LOGE(TAG, "iic recv fail");
    }

    aos_event_get(&iic(dev)->event, EVT_RECV_DONE, AOS_EVENT_OR_CLEAR, &flags, timeout);

    if (flags & EVT_RECV_DONE) {
        return 0;
    } else {
        return -1;
    }

    return -ETIMEDOUT;
}

static int iic_csky_slave_send(rvm_dev_t *dev, const void *data, uint32_t size, uint32_t timeout)
{
    int ret;
    unsigned int flags = 0;

    ret = csi_iic_slave_send_async(&iic(dev)->handle, data, size);

    if (ret < 0) {
        LOGE(TAG, "iic slave send fail");
    }

    aos_event_get(&iic(dev)->event, EVT_SEND_DONE, AOS_EVENT_OR_CLEAR, &flags, timeout);

    if (flags & EVT_SEND_DONE) {
        return 0;
    } else {
        return -1;
    }

    return -ETIMEDOUT;
}

static int iic_csky_slave_recv(rvm_dev_t *dev, void *data, uint32_t size, uint32_t timeout)
{
    int ret;
    unsigned int flags = 0;

    ret = csi_iic_slave_receive_async(&iic(dev)->handle, data, size);

    if (ret < 0) {
        LOGE(TAG, "iic slave recv fail");
    }

    aos_event_get(&iic(dev)->event, EVT_RECV_DONE, AOS_EVENT_OR_CLEAR, &flags, timeout);

    if (flags & EVT_RECV_DONE) {
        return 0;
    } else {
        return -1;
    }

    return -ETIMEDOUT;
}

static int iic_csky_mem_write(rvm_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,
                                const void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret_len = csi_iic_mem_send(&iic(dev)->handle, dev_addr, mem_addr, mem_addr_size, data, size, timeout);
    if (ret_len != size) {
        return -ETIMEDOUT;
    }

    return 0;
}

static int iic_csky_mem_read(rvm_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,
                                void *data, uint32_t size, uint32_t timeout)
{
    int32_t ret_len = csi_iic_mem_receive(&iic(dev)->handle, dev_addr, mem_addr, mem_addr_size, data, size, timeout);
    if (ret_len != size) {
        return -ETIMEDOUT;
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
        .clk_en = iic_csky_clock
    },
    .config          = iic_csky_config,
    .master_send     = iic_csky_master_send,
    .master_recv     = iic_csky_master_recv,
    .slave_send      = iic_csky_slave_send,
    .slave_recv      = iic_csky_slave_recv,
    .mem_write       = iic_csky_mem_write,
    .mem_read        = iic_csky_mem_read
};

void rvm_iic_drv_register(int idx)
{
    rvm_driver_register(&iic_driver.drv, NULL, idx);
}
