/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_i2s_v2.c
 * @brief    CSI Source File for I2S Driver
 * @version  V1.2
 * @date     8.15. August 2019
 * @vendor   csky
 * @name     csky-i2s
 * @ip_id    0x111000010
 * @model    i2s
 * @tag      DRV_CK_I2S_TAG
******************************************************************************/

#include <string.h>
#include <drv/irq.h>
#include <drv/i2s.h>
#include <soc.h>

#include "wm_i2s.h"

typedef struct {
    __IO uint32_t CTRL;
    __IO uint32_t INT_MASK;
    __IO uint32_t INT_SRC;
    __I  uint32_t INT_STATUS;
    __O  uint32_t TX;
    __I	 uint32_t RX;
} ck_i2s_v2_reg_t;

typedef struct  {
    uint32_t base;
    uint32_t irq_num;
    void *irq_handle;

    i2s_event_cb_t cb;
    void *cb_arg;

    uint32_t tx_runing;
    uint32_t rx_runing;
} ck_i2s_priv_v2_t;

#define ERR_I2S(errno) (CSI_DRV_ERRNO_I2S_BASE | errno)
#define I2S_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_I2S(DRV_ERROR_PARAMETER))

extern void i2s_I2S_IRQHandler(void);
extern int32_t target_i2s_init(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);
ck_i2s_priv_v2_t i2s_instance[CONFIG_I2S_NUM];

i2s_handle_t csi_i2s_initialize(int32_t idx, i2s_event_cb_t cb_event, void *cb_arg)
{
    if (idx >= CONFIG_I2S_NUM) {
        return NULL;
    }

    ck_i2s_priv_v2_t *handle = &i2s_instance[idx];

    int32_t ret = target_i2s_init(idx, &handle->base, &handle->irq_num, &handle->irq_handle);

    if (ret != idx) {
        return NULL;
    }

    handle->cb = cb_event;
    handle->cb_arg = cb_arg;

    drv_irq_register(handle->irq_num, handle->irq_handle);
    drv_irq_enable(handle->irq_num);

    return handle;
}

int32_t csi_i2s_uninitialize(i2s_handle_t handle)
{
    I2S_NULL_PARAM_CHK(handle);
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;

    drv_irq_unregister(priv->irq_num);
    drv_irq_disable(priv->irq_num);

    memset(handle, 0, sizeof(ck_i2s_priv_v2_t));

    return 0;
}

int32_t csi_i2s_config(i2s_handle_t handle, i2s_config_t *config)
{
    I2S_NULL_PARAM_CHK(handle);

    uint32_t value;
    //ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;
    //ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;

    if (I2S_PROTOCOL_LSB_JUSTIFIED == config->cfg.protocol) {
        return ERR_I2S(DRV_ERROR_UNSUPPORTED);
    }

    if (config->cfg.mode == I2S_MODE_TX_MASTER ||
        config->cfg.mode == I2S_MODE_RX_MASTER ||
        config->cfg.mode == I2S_MODE_FULL_DUPLEX_MASTER)
    {
        wm_i2s_set_mode(0);
    }
    else
    {
        wm_i2s_set_mode(1);
    }

    if (config->cfg.protocol == I2S_PROTOCOL_I2S)
    {
    	wm_i2s_set_format(I2S_Standard);
    }
    else if (config->cfg.protocol == I2S_PROTOCOL_MSB_JUSTIFIED)
    {
    	wm_i2s_set_format(I2S_Standard_MSB);
    }
    else if (config->cfg.protocol == I2S_PROTOCOL_PCM)
    {
    	wm_i2s_set_format(I2S_Standard_PCMA);//I2S_Standard_PCMB
    }

    if (config->cfg.width == I2S_SAMPLE_16BIT)
    {
        wm_i2s_set_word_len(I2S_DataFormat_16);
    }
    else if (config->cfg.width == I2S_SAMPLE_24BIT)
    {
        wm_i2s_set_word_len(I2S_DataFormat_24);
    }
    else if (config->cfg.width == I2S_SAMPLE_32BIT)
    {
        wm_i2s_set_word_len(I2S_DataFormat_32);
    }

    //config->cfg.left_polarity

    if (config->cfg.sclk_freq == I2S_SCLK_32FS)
        value = 32 * config->rate;
    else if (config->cfg.sclk_freq == I2S_SCLK_48FS)
        value = 48 * config->rate;
    else if (config->cfg.sclk_freq == I2S_SCLK_64FS)
        value = 64 * config->rate;
    else if (config->cfg.sclk_freq == I2S_SCLK_16FS)
        value = 16 * config->rate;

    wm_i2s_set_freq(value, config->cfg.mclk_freq);

    if (config->cfg.tx_mono_enable || config->cfg.rx_mono_enable)
        wm_i2s_mono_select(1);
    else
        wm_i2s_mono_select(0);

    if (config->cfg.rx_mono_select_ch == I2S_RX_RIGHT_CHANNEL)
        wm_i2s_left_channel_sel(I2S_RIGHT_CHANNEL);
    else
        wm_i2s_left_channel_sel(I2S_LEFT_CHANNEL);

    return 0;
}

uint32_t csi_i2s_send(i2s_handle_t handle, const uint8_t *data, uint32_t length)
{
    I2S_NULL_PARAM_CHK(handle);

    int ret;
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;

    priv->tx_runing = 1;
    ret = wm_i2s_tx_int((int16_t *)data, length / 4, NULL);
    priv->tx_runing = 0;

    if (ret)
    {
        return 0;
    }
    else
    {
        if (priv->cb)
            priv->cb(0, I2S_EVENT_SEND_COMPLETE, priv->cb_arg);
        return length;
    }
}

uint32_t csi_i2s_receive(i2s_handle_t handle, uint8_t *buf, uint32_t length)
{
    I2S_NULL_PARAM_CHK(handle);

    int ret;
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;

    priv->rx_runing = 1;
    ret = wm_i2s_rx_int((int16_t *)buf, length / 4);
    priv->rx_runing = 0;

    if (ret)
    {
        return 0;
    }
    else
    {
        if (priv->cb)
            priv->cb(0, I2S_EVENT_RECEIVE_COMPLETE, priv->cb_arg);
        return length;
    }
}

int32_t csi_i2s_send_ctrl(i2s_handle_t handle, i2s_ctrl_e cmd)
{
    I2S_NULL_PARAM_CHK(handle);

    return ERR_I2S(DRV_ERROR_UNSUPPORTED);
}

int32_t csi_i2s_receive_ctrl(i2s_handle_t handle, i2s_ctrl_e cmd)
{
    I2S_NULL_PARAM_CHK(handle);

    return ERR_I2S(DRV_ERROR_UNSUPPORTED);
}

void csi_i2s_enable(i2s_handle_t handle, int en)
{
    if (!handle) return;

    wm_i2s_enable(en);
}

void i2s_irqhandler(int32_t idx)
{
    ck_i2s_priv_v2_t *priv = &i2s_instance[0];
    ck_i2s_v2_reg_t *addr = (ck_i2s_v2_reg_t *)priv->base;

    if (addr->INT_SRC & (1 << 1))
    {
        if (priv->cb)
            priv->cb(0, I2S_EVENT_RX_OVERFLOW, priv->cb_arg);
    }
    if (addr->INT_SRC & (1 << 4))
    {
        if (priv->cb)
            priv->cb(0, I2S_EVENT_TX_UNDERFLOW, priv->cb_arg);
    }

    i2s_I2S_IRQHandler();
}

i2s_status_t csi_i2s_get_status(i2s_handle_t handle)
{
    i2s_status_t i2s_status;
    ck_i2s_priv_v2_t *priv = (ck_i2s_priv_v2_t *)handle;

    memset(&i2s_status, 0, sizeof(i2s_status));
    if (!handle) return i2s_status;

    i2s_status.tx_runing = priv->tx_runing;
    i2s_status.rx_runing = priv->rx_runing;

    return i2s_status;
}

i2s_capabilities_t csi_i2s_get_capabilities(int32_t idx)
{
    i2s_capabilities_t val;
    val.protocol_user = 0;
    val.protocol_i2s = 1;
    val.protocol_justified = 1;
    val.protocol_pcm = 1;
    val.mono_mode = 1;
    val.event_frame_error = 0;
    val.mclk_pin  = 1;
    val.full_duplex = 1;

    return val;
}

int32_t csi_i2s_power_control(i2s_handle_t handle, csi_power_stat_e state)
{
    return ERR_I2S(DRV_ERROR_UNSUPPORTED);
}
