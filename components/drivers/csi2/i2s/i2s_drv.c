/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/kernel.h>
#include <drv/i2s.h>
#include <drv/clk.h>
#include <devices/impl/i2s_impl.h>

#define TAG "i2s_drv"

#ifndef CONFIG_I2S_BUF_SIZE
#define I2S_BUF_SIZE 2048
#else
#define I2S_BUF_SIZE CONFIG_I2S_BUF_SIZE
#endif

#ifndef CONFIG_I2S_CALLBACK_PERIOD
#define I2S_CALLBACK_PERIOD 512
#else
#define I2S_CALLBACK_PERIOD CONFIG_I2S_CALLBACK_PERIOD
#endif

typedef struct {
    rvm_dev_t device;
    csi_i2s_t handle;
    aos_sem_t rx_period_sem;
    aos_sem_t tx_period_sem;
    csi_ringbuf_t i2s_tx_ringbuffer;
    csi_ringbuf_t i2s_rx_ringbuffer;
    int rx_dma_en;
    int tx_dma_en;
    csi_dma_ch_t g_dma_ch_tx;
    csi_dma_ch_t g_dma_ch_rx;
    rvm_hal_i2s_config_t config;
} i2s_dev_t;

#define I2S(dev) ((i2s_dev_t *)dev)

static void i2s_cb(csi_i2s_t *handle, csi_i2s_event_t event, void *arg)
{
    rvm_dev_t *dev = (rvm_dev_t *)arg;

    if (event == I2S_EVENT_SEND_COMPLETE) {
        aos_sem_signal(&I2S(dev)->tx_period_sem);
    } else if (event == I2S_EVENT_RECEIVE_COMPLETE) {
        aos_sem_signal(&I2S(dev)->rx_period_sem);
    } else {
        LOGE(TAG, "i2s err event %d", event);
    }
}

static rvm_dev_t *_i2s_init(driver_t *drv, void *config, int id)
{
    i2s_dev_t *iis = (i2s_dev_t *)rvm_hal_device_new(drv, sizeof(i2s_dev_t), id);

    return (rvm_dev_t *)iis;
}

#define _i2s_uninit rvm_hal_device_free

static int _i2s_open(rvm_dev_t *dev)
{
    csi_error_t ret;

    if (aos_sem_new(&I2S(dev)->rx_period_sem, 1) != 0) {
        return -1;
    }

    if (aos_sem_new(&I2S(dev)->tx_period_sem, 1) != 0) {
        return -1;
    }

    ret = csi_i2s_init(&I2S(dev)->handle, dev->id);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_i2s_init error");
        return -1;
    }

    I2S(dev)->tx_dma_en = 0;
    I2S(dev)->rx_dma_en = 0;
    ret = csi_i2s_attach_callback(&I2S(dev)->handle, i2s_cb, NULL);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_i2s_attach_callback error");
        return -1;
    }
    return 0;
}

static int _i2s_close(rvm_dev_t *dev)
{
    if (I2S(dev)->tx_dma_en) {
        csi_i2s_tx_link_dma(&I2S(dev)->handle, NULL);
        I2S(dev)->tx_dma_en = 0;
    }
    if (I2S(dev)->rx_dma_en) {
        csi_i2s_rx_link_dma(&I2S(dev)->handle, NULL);
        I2S(dev)->rx_dma_en = 0;
    }
    aos_free(I2S(dev)->i2s_tx_ringbuffer.buffer);
    aos_free(I2S(dev)->i2s_rx_ringbuffer.buffer);
    aos_sem_free(&I2S(dev)->rx_period_sem);
    aos_sem_free(&I2S(dev)->tx_period_sem);
    csi_i2s_detach_callback(&I2S(dev)->handle);
    csi_i2s_uninit(&I2S(dev)->handle);
    return 0;
}

static int _i2s_clock(rvm_dev_t *dev, bool enable)
{
    if (enable) {
        csi_clk_enable(&I2S(dev)->handle.dev);
    } else {
        csi_clk_disable(&I2S(dev)->handle.dev);
    }
    return 0;
}

static int _i2s_lpm(rvm_dev_t *dev, int state)
{
#ifdef CONFIG_PM
    if (state == 0) {
        csi_i2s_disable_pm(&I2S(dev)->handle);
    } else {
        csi_i2s_enable_pm(&I2S(dev)->handle);
    }
#endif
    return 0;
}

static int _i2s_config(rvm_dev_t *dev, rvm_hal_i2s_config_t *config)
{
    csi_error_t ret;
    csi_i2s_format_t i2s_format;

    if (!config) {
        return -EINVAL;
    }

    if ((config->mode == RVM_HAL_I2S_MODE_MASTER_TX) || (config->mode == RVM_HAL_I2S_MODE_MASTER_RX)) {
        i2s_format.mode = I2S_MODE_MASTER;
    }

    if ((config->mode == RVM_HAL_I2S_MODE_SLAVE_TX) || (config->mode == RVM_HAL_I2S_MODE_SLAVE_RX)) {
        i2s_format.mode = I2S_MODE_SLAVE;
    }

    switch (config->standard) {
        case RVM_HAL_I2S_STANDARD_PHILIPS:
            i2s_format.width = I2S_SAMPLE_WIDTH_16BIT;
            i2s_format.protocol = I2S_PROTOCOL_I2S;
            break;

        case RVM_HAL_I2S_STANDARD_MSB:
            i2s_format.width = I2S_SAMPLE_WIDTH_16BIT;
            i2s_format.protocol = I2S_PROTOCOL_MSB_JUSTIFIED;
            break;

        case RVM_HAL_I2S_STANDARD_LSB:
            i2s_format.width = I2S_SAMPLE_WIDTH_24BIT;
            i2s_format.protocol = I2S_PROTOCOL_LSB_JUSTIFIED;
            break;

        case RVM_HAL_I2S_STANDARD_PCM_SHORT:
            i2s_format.width = I2S_SAMPLE_WIDTH_32BIT;
            i2s_format.protocol = I2S_PROTOCOL_PCM;
            break;

        case RVM_HAL_I2S_STANDARD_PCM_LONG:
            i2s_format.width = I2S_SAMPLE_WIDTH_32BIT;
            i2s_format.protocol = I2S_PROTOCOL_PCM;
            break;

        default:
            break;
    }

    i2s_format.rate = config->freq;
    i2s_format.sclk_nfs = I2S_SCLK_32FS;
    i2s_format.mclk_nfs = I2S_MCLK_256FS;
    i2s_format.polarity = 0;
    csi_i2s_format(&I2S(dev)->handle, &i2s_format);

    if ((config->mode == RVM_HAL_I2S_MODE_MASTER_TX) || (config->mode == RVM_HAL_I2S_MODE_SLAVE_TX)) {
        I2S(dev)->i2s_tx_ringbuffer.size = I2S_BUF_SIZE;
        I2S(dev)->i2s_tx_ringbuffer.buffer = (uint8_t *)aos_malloc(I2S_BUF_SIZE);
        csi_i2s_tx_set_buffer(&I2S(dev)->handle, &I2S(dev)->i2s_tx_ringbuffer);
        csi_i2s_tx_buffer_reset(&I2S(dev)->handle);
        ret = csi_i2s_tx_set_period(&I2S(dev)->handle, I2S_CALLBACK_PERIOD);
        csi_i2s_send_start(&I2S(dev)->handle);
    } else if ((config->mode == RVM_HAL_I2S_MODE_MASTER_RX) || (config->mode == RVM_HAL_I2S_MODE_SLAVE_RX)) {
        I2S(dev)->i2s_rx_ringbuffer.size = I2S_BUF_SIZE;
        I2S(dev)->i2s_rx_ringbuffer.buffer = (uint8_t *)aos_malloc(I2S_BUF_SIZE);
        csi_i2s_rx_set_buffer(&I2S(dev)->handle, &I2S(dev)->i2s_rx_ringbuffer);
        csi_i2s_rx_buffer_reset(&I2S(dev)->handle);
        ret = csi_i2s_rx_set_period(&I2S(dev)->handle, I2S_CALLBACK_PERIOD);
        csi_i2s_receive_start(&I2S(dev)->handle);
    } else {
        ret = -EINVAL;
    }

    if (!config->tx_dma_enable) {
        ret = csi_i2s_tx_link_dma(&I2S(dev)->handle, NULL);
        if (ret != CSI_OK) {
            return -1;
        }
    } else {
        ret = csi_i2s_tx_link_dma(&I2S(dev)->handle, &I2S(dev)->g_dma_ch_tx);
        if (ret != CSI_OK) {
            return -1;
        }
    }

    if (!config->rx_dma_enable) {
        ret = csi_i2s_rx_link_dma(&I2S(dev)->handle, NULL);
        if (ret != CSI_OK) {
            return -1;
        }
    } else {
        ret = csi_i2s_rx_link_dma(&I2S(dev)->handle, &I2S(dev)->g_dma_ch_rx);
        if (ret != CSI_OK) {
            return -1;
        }
    }

    memcpy(&I2S(dev)->config, config, sizeof(rvm_hal_i2s_config_t));
    return ret;
}

static int _i2s_config_get(rvm_dev_t *dev, rvm_hal_i2s_config_t *config)
{
    if (!config) {
        return -EINVAL;
    }
    memcpy(config, &I2S(dev)->config, sizeof(rvm_hal_i2s_config_t));
    return 0;
}

static int _i2s_send(rvm_dev_t *dev, const void *data, size_t size, uint32_t timeout)
{
    int num;
    num = csi_i2s_send_async(&I2S(dev)->handle, data, size);
    if (num != size) {
        printf("csi_i2s_send_async error\n");
        return -1;
    }
    return num;
}

static int _i2s_recv(rvm_dev_t *dev, void *data, size_t size, uint32_t timeout)
{
    int num;
    num = csi_i2s_receive_async(&I2S(dev)->handle, data, size);
    if (num != size) {
        printf("csi_i2s_receive_async error\n");
        return -1;
    }
    return num;
}

static int _i2s_pause(rvm_dev_t *dev)
{
    int ret = 0;
    if ((I2S(dev)->config.mode == RVM_HAL_I2S_MODE_MASTER_TX) || (I2S(dev)->config.mode == RVM_HAL_I2S_MODE_SLAVE_TX)) {
        ret = csi_i2s_send_pause(&I2S(dev)->handle);
    }

    if ((I2S(dev)->config.mode == RVM_HAL_I2S_MODE_MASTER_RX) || (I2S(dev)->config.mode == RVM_HAL_I2S_MODE_SLAVE_RX)) {
        csi_i2s_send_stop(&I2S(dev)->handle);
    }
    return ret;
}

static int _i2s_resume(rvm_dev_t *dev)
{
    int ret = 0;
    if ((I2S(dev)->config.mode == RVM_HAL_I2S_MODE_MASTER_TX) || (I2S(dev)->config.mode == RVM_HAL_I2S_MODE_SLAVE_TX)) {
        ret = csi_i2s_send_resume(&I2S(dev)->handle);
    }

    if ((I2S(dev)->config.mode == RVM_HAL_I2S_MODE_MASTER_RX) || (I2S(dev)->config.mode == RVM_HAL_I2S_MODE_SLAVE_RX)) {
        csi_i2s_receive_start(&I2S(dev)->handle);
    }
    return ret;
}

static int _i2s_stop(rvm_dev_t *dev)
{
    if ((I2S(dev)->config.mode == RVM_HAL_I2S_MODE_MASTER_TX) || (I2S(dev)->config.mode == RVM_HAL_I2S_MODE_SLAVE_TX)) {
        csi_i2s_send_stop(&I2S(dev)->handle);
    }

    if ((I2S(dev)->config.mode == RVM_HAL_I2S_MODE_MASTER_RX) || (I2S(dev)->config.mode == RVM_HAL_I2S_MODE_SLAVE_RX)) {
        csi_i2s_receive_stop(&I2S(dev)->handle);
    }
    return 0;
}

static i2s_driver_t i2s_driver = {
    .drv = {
        .name   = "iis",
        .init   = _i2s_init,
        .uninit = _i2s_uninit,
        .open   = _i2s_open,
        .close  = _i2s_close,
        .clk_en = _i2s_clock,
        .lpm    = _i2s_lpm
    },
    .config          = _i2s_config,
    .config_get      = _i2s_config_get,
    .recv            = _i2s_recv,
    .send            = _i2s_send,
    .resume          = _i2s_resume,
    .stop            = _i2s_stop,
    .pause           = _i2s_pause,

};

void rvm_i2s_drv_register(int idx)
{
    rvm_driver_register(&i2s_driver.drv, NULL, idx);
}
