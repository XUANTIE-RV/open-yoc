/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     ck_codec.c
 * @brief    CSI Source File for codec Driver
 * @version  V1.0
 * @date     25. September 2019
 * @vendor   csky
 * @name     csky-codec
 * @ip_id    0x111000010
 * @model    codec
 * @tag      DRV_CK_CODEC_TAG
 ******************************************************************************/
#include <drv/irq.h>
#include "drv/ringbuffer.h"
#include "ck_codec.h"
#include "string.h"

#define CONFIG_CODEC_NUM    1
#define CSI_DRV_ERRNO_CODEC_BASE            0x811D0000
#define ERR_CODEC(errno) (CSI_DRV_ERRNO_CODEC_BASE | errno)
#define CODEC_BUSY_TIMEOUT    0x1000000
#define CODEC_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_CODEC(DRV_ERROR_PARAMETER))

typedef enum {
    DRV_ERROR = CSI_UNSUPPORTED,   ///< Unspecified error
    DRV_ERROR_BUSY,                ///< Driver is busy
    DRV_ERROR_TIMEOUT,             ///< Timeout occurred
    DRV_ERROR_UNSUPPORTED,         ///< Operation not supported
    DRV_ERROR_PARAMETER,           ///< Parameter error
    DRV_ERROR_SPECIFIC             ///< Start of driver specific errors
} drv_err_e;

extern void mdelay(uint32_t ms);


ck_codec_ch_priv_t ck_codec_input_instance[CK_CODEC_ADC_CHANNEL_NUM];
ck_codec_ch_priv_t ck_codec_output_instance[CK_CODEC_DAC_CHANNEL_NUM];
csi_dev_t ck_codec_instance_v2[CONFIG_CODEC_NUM];

static int32_t ck_codec_sample_rate_mask(int32_t sample_rate)
{
    switch (sample_rate) {
        case 8000:
            return 0;

        case 11025:
            return 1;

        case 12000:
            return 2;

        case 16000:
            return 3;

        case 22050:
            return 4;

        case 24000:
            return 5;

        case 32000:
            return 6;

        case 44100:
            return 7;

        case 48000:
            return 8;

        case 96000:
            return 10;
    }

    return -1;
}

extern void mdelay(uint32_t ms);

extern void ck_codec_irqhandler(csi_dev_t *pt);

void dw_codec_handler(void *arg)
{
    csi_dev_t *pt = (csi_dev_t *)arg;

    ck_codec_irqhandler(pt);
}

int32_t codec_init(uint32_t idx)
{
    /*to do reset the codec module*/

    if (idx >= CONFIG_CODEC_NUM) {
        return -1;
    }

    csi_dev_t *priv = &ck_codec_instance_v2[idx];
    priv->idx = idx;
    priv->reg_base = WJ_CODEC_BASE;
    priv->irq_num = WJ_GSK_INTC_IRQn;
    priv->dev_tag = DEV_WJ_CODEC_TAG;

    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->reg_base;
    /*IRQ High level*/
    addr->ICR = 0;
    addr->IMR = 0x30;

    ck_codec_sb_power_ctrl(priv, 1);
    mdelay(100);

    ck_codec_sb_power_ctrl(priv, 0);
    ck_codec_mclk_freq_select(priv, CK_CODEC_MCLK_12M);
    ck_codec_tx_threshold_val(priv, 4);
    ck_codec_rx_threshold_val(priv, 4);

    ck_codec_inerrupt_disable_all(priv);
    ck_codec_tx_feed(priv);
    /*Don't change me, enable fifo tx empty and error interrupt*/
    uint32_t mask = (1 << 16) | (0x1ff << 18);
    ck_codec_inerrupt_ctrl(priv, mask, 1);

    csi_irq_attach((uint32_t)priv->irq_num, dw_codec_handler, priv);
    csi_irq_enable((uint32_t)priv->irq_num);

    /*wait codec power ready*/
    uint32_t time_out = CODEC_BUSY_TIMEOUT;

    while (!(addr->SR & (1 << 7)) && time_out--);

    if (time_out == 0) {
        return -1;
    }

    time_out = CODEC_BUSY_TIMEOUT;

    /*wati codec ready*/
    while ((addr->SR & 0x01) && time_out--);

    if (time_out == 0) {
        return -1;
    }

    mdelay(100);

    return 0;
}

void codec_uninit(uint32_t idx)
{
    csi_dev_t *priv = &ck_codec_instance_v2[idx];
    ck_codec_inerrupt_disable_all(priv);
    ck_codec_sb_power_ctrl(priv, 1);

    csi_irq_disable((uint32_t)priv->irq_num);
    csi_irq_detach((uint32_t)priv->irq_num);
    /*to do reset the codec module*/
}


int32_t codec_input_open(codec_input_t *handle)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int ret = -1;
    int ch = handle->ch_idx;
    int codec_idx = handle->codec_idx;

    if (ch < 0 || ch >= CK_CODEC_ADC_CHANNEL_NUM || codec_idx >= CONFIG_CODEC_NUM) {
        return -1;
    }

    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    ck_codec_ch_priv_t *input_priv = &ck_codec_input_instance[ch];

    input_priv->codec_idx = handle->codec_idx;
    input_priv->idx = ch;

    ck_codec_disable_dmic(codec_priv);
    ck_codec_adc_channel_enable(codec_priv, ch, 1);
    ck_codec_adc_wnf_ctrl(codec_priv, ch, 0);
    ck_codec_mic_set_bias(codec_priv, ch, CK_CODEC_MIC_BIAS_1500MV);
    ck_codec_adc_mode_set(codec_priv, ch, CK_CODEC_ADC_SINGLE_MODE);
    ck_codec_mic_bias_en(codec_priv, ch, 1);
    /*disable sleep*/
    ck_codec_adc_sb_enable(codec_priv, ch, 0);
    ck_codec_adc_mute_en(codec_priv, ch, 0);
    ck_codec_adc_mixer_set_gain(codec_priv, 0);
    ck_codec_adc_set_boost_gain(codec_priv, ch, 0);
    ck_codec_adc_set_digital_gain(codec_priv, ch, 0);

    input_priv->dma.ch_id = -1;
    ret = ck_codec_input_dma_config(input_priv);

    if (ret == -1) {
        return -1;
    }

    input_priv->cb = handle->cb;
    input_priv->cb_arg = handle->cb_arg;
    input_priv->period = handle->period;
    input_priv->fifo.buffer = handle->buf;
    input_priv->fifo.size = handle->buf_size;
    input_priv->fifo.read = input_priv->fifo.write = input_priv->fifo.data_len = 0;
    input_priv->priv_sta |= CODEC_INIT_MASK;

    return 0;
}

int32_t codec_input_close(codec_input_t *handle)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int ch = handle->ch_idx;
    int codec_idx = handle->codec_idx;

    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    ck_codec_ch_priv_t *input_priv = &ck_codec_input_instance[ch];

    if (input_priv->priv_sta & CODEC_RUNING_MASK) {
        return -1;
    }

    ck_codec_adc_channel_enable(codec_priv, ch, 0);
    ck_codec_mic_bias_en(codec_priv, ch, 0);
    /*enable sleep*/
    ck_codec_adc_sb_enable(codec_priv, ch, 1);
    ck_codec_adc_mute_en(codec_priv, ch, 1);
    ck_codec_path_adc_en(codec_priv, ch, 0);

    csi_dma_ch_free(&input_priv->dma);
    memset(input_priv, 0, sizeof(ck_codec_ch_priv_t));

    return 0;
}

extern uint32_t g_ad_timer_period;
int32_t codec_input_config(codec_input_t *handle, codec_input_config_t *config)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int ch = handle->ch_idx;
    int codec_idx = handle->codec_idx;

    ck_codec_ch_priv_t *input_priv = &ck_codec_input_instance[ch];
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    if ((input_priv->priv_sta & CODEC_INIT_MASK) == 0) {
        return -1;
    }

    int32_t sample_rate_mask = ck_codec_sample_rate_mask(config->sample_rate);

    if (sample_rate_mask == -1) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    ck_codec_adc_sample_rate_config(codec_priv, sample_rate_mask);

    if (config->bit_width > 16) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    /*only support 16bit*/
    ck_codec_adc_bit_width_and_mode_set(codec_priv);

    uint32_t period_ms = handle->period * 1000 / 2 / config->sample_rate;
    uint32_t i = 2000 * 1000 / 2 / 2 / config->sample_rate;
    while((period_ms % i) != 0) {
        i--;
    }
    g_ad_timer_period = i * 1000;

    return 0;
}


static int32_t ck_codec_input_stream_start(codec_input_t *handle)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int ch = handle->ch_idx;
    int codec_idx = handle->codec_idx;

    ck_codec_ch_priv_t *input_priv = &ck_codec_input_instance[ch];
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    ck_codec_reg_t *addr = (ck_codec_reg_t *)codec_priv->reg_base;

    if ((input_priv->priv_sta & CODEC_INIT_MASK) == 0) {
        return -1;
    }

    if (input_priv->priv_sta & CODEC_RUNING_MASK) {
        return 0;
    }

    int ret = ck_codec_dma_input_start(input_priv, addr);

    if (ret == 0) {
        input_priv->priv_sta |= CODEC_RUNING_MASK;
    }

    return ret;
}

static int32_t ck_codec_input_stream_stop(codec_input_t *handle)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int ch = handle->ch_idx;
    int codec_idx = handle->codec_idx;

    ck_codec_ch_priv_t *input_priv = &ck_codec_input_instance[ch];
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    if (input_priv->dma.ch_id == -1) {
        return -1;
    }

    ck_codec_path_adc_en(codec_priv, ch, 0);
    ck_codec_input_dma_stop(input_priv);
    input_priv->priv_sta &= ~CODEC_RUNING_MASK;
    return 0;
}

int32_t codec_input_start(codec_input_t *handle)
{
    return ck_codec_input_stream_start(handle);
}

int32_t codec_input_stop(codec_input_t *handle)
{
    int ret = ck_codec_input_stream_stop(handle);
    int ch = handle->ch_idx;
    ck_codec_ch_priv_t *input_priv = &ck_codec_input_instance[ch];

    ringbuffer_reset(&input_priv->fifo);
    return ret;
}

uint32_t codec_input_read(codec_input_t *handle, uint8_t *buf, uint32_t length)
{
    if (handle == NULL) {
        return 0;
    }

    int ch = handle->ch_idx;
    ck_codec_ch_priv_t *input_priv = &ck_codec_input_instance[ch];
    return ringbuffer_out(&input_priv->fifo, buf, length);
}

uint32_t codec_input_buf_avail(codec_input_t *handle)
{
    if (handle == NULL) {
        return 0;
    }

    int ch = handle->ch_idx;
    ck_codec_ch_priv_t *input_priv = &ck_codec_input_instance[ch];
    return ringbuffer_len(&input_priv->fifo);
}

int32_t codec_input_buf_reset(codec_input_t *handle)
{
    if (handle == NULL) {
        return -1;
    }

    int ch = handle->ch_idx;
    ck_codec_ch_priv_t *input_priv = &ck_codec_input_instance[ch];

    ringbuffer_reset(&input_priv->fifo);

    return 0;
}

int32_t codec_input_pause(codec_input_t *handle)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int ch = handle->ch_idx;
    ck_codec_ch_priv_t *input_priv = &ck_codec_input_instance[ch];

    ck_codec_input_dma_stop(input_priv);
    return 0;
}

int32_t codec_input_resume(codec_input_t *handle)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int ch = handle->ch_idx;
    int codec_idx = handle->codec_idx;

    ck_codec_ch_priv_t *input_priv = &ck_codec_input_instance[ch];
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    ck_codec_reg_t *addr = (ck_codec_reg_t *)codec_priv->reg_base;

    ck_codec_dma_input_start(input_priv, addr);
    return 0;
}

int32_t codec_output_open(codec_output_t *handle)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int ret = -1;
    int ch = handle->ch_idx;
    int codec_idx = handle->codec_idx;

    if (ch < 0 || ch >= CK_CODEC_DAC_CHANNEL_NUM || codec_idx >= CONFIG_CODEC_NUM) {
        return -1;
    }

    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    ck_codec_ch_priv_t *output_priv = &ck_codec_output_instance[ch];

    ck_codec_dac_mono_enable(codec_priv, 0);
    ck_codec_dac_mute_en(codec_priv, 0);
    ck_codec_dac_sb_en(codec_priv, 0);
    ck_codec_dac_headphone_mute(codec_priv, 0);
    ck_codec_dac_headphone_sb_enable(codec_priv, 0);

    //del fix output noise:Fix me
    //ck_codec_dac_headphone_set_left_gain(codec_priv, 0);
    //ck_codec_dac_headphone_set_right_gain(codec_priv, 0);

    output_priv->dma.ch_id = -1;
    ret = ck_codec_output_dma_config(output_priv);

    if (ret == -1) {
        return -1;
    }

    output_priv->codec_idx = handle->codec_idx;
    output_priv->cb = handle->cb;
    output_priv->cb_arg = handle->cb_arg;
    output_priv->priv_sta |= CODEC_INIT_MASK;
    output_priv->period = handle->period;
    output_priv->fifo.buffer = handle->buf;
    output_priv->fifo.size = handle->buf_size;
    output_priv->fifo.read = output_priv->fifo.write = output_priv->fifo.data_len = 0;

    return 0;
}

int32_t codec_output_close(codec_output_t *handle)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int ch = handle->ch_idx;
    int codec_idx = handle->codec_idx;

    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    ck_codec_ch_priv_t *output_priv = &ck_codec_output_instance[ch];


    if (output_priv->priv_sta & CODEC_RUNING_MASK) {
        return -1;
    }

    ck_codec_dac_mute_en(codec_priv, 1);

    if (output_priv->dma.ch_id != -1) {
        // ck_codec_dma_ch_release(output_priv->dma_ch);
        csi_dma_ch_detach_callback(&output_priv->dma);
        csi_dma_ch_free(&output_priv->dma);
    }

    memset(output_priv, 0, sizeof(ck_codec_ch_priv_t));

    return 0;
}

static int32_t ck_codec_output_bitwidth_to_mask(uint32_t bit_width)
{
    int i = 16;
    int j = 0;

    while (i <= 24) {
        if (bit_width == i) {
            return j;
        }

        i += 2;
        j++;
    }

    return -1;
}

int32_t codec_output_config(codec_output_t *handle, codec_output_config_t *config)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int ch = handle->ch_idx;
    int codec_idx = handle->codec_idx;

    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    ck_codec_ch_priv_t *output_priv = &ck_codec_output_instance[ch];

    ck_codec_reg_t *addr = (ck_codec_reg_t *)codec_priv->reg_base;

    if ((output_priv->priv_sta & CODEC_INIT_MASK) == 0) {
        return -1;
    }

    int bit_width_mask = ck_codec_output_bitwidth_to_mask(config->bit_width);

    if (bit_width_mask == -1) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    ck_codec_dac_bit_width_and_mode_set(codec_priv, bit_width_mask);

    int32_t sample_rate_mask = ck_codec_sample_rate_mask(config->sample_rate);

    if (sample_rate_mask == -1) {
        return -1;
    }

    ck_codec_dac_sample_rate_config(codec_priv, sample_rate_mask);

    if (config->mono_mode_en) {
        ck_codec_dac_mono_enable(codec_priv, 1);
    }

    output_priv->bit_width = config->bit_width;
    uint32_t time_out = CODEC_BUSY_TIMEOUT;

    /*wati codec ready*/
    while ((addr->SR & 0x01) && time_out--);

    if (time_out == 0) {
        return -1;
    }

    return 0;
}

int32_t codec_output_start(codec_output_t *handle)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    ck_codec_ch_priv_t *output_priv = &ck_codec_output_instance[handle->ch_idx];

    if ((output_priv->priv_sta & CODEC_INIT_MASK) == 0) {
        return -1;
    }

    if (output_priv->priv_sta & CODEC_RUNING_MASK) {
        return 0;
    }

    output_priv->priv_sta |= CODEC_READT_TO_START_MASK;

    int ret_len = ringbuffer_len(&output_priv->fifo);

    if (ret_len != 0) {
        if (ck_codec_output_dma_start(handle->ch_idx) == 0) {
            output_priv->priv_sta |= CODEC_RUNING_MASK;
        }
    }

    return 0;
}

int32_t codec_output_stop(codec_output_t *handle)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    ck_codec_ch_priv_t *output_priv = &ck_codec_output_instance[handle->ch_idx];

    output_priv->priv_sta &= ~CODEC_READT_TO_START_MASK;

    if ((output_priv->priv_sta & CODEC_RUNING_MASK) == 0) {
        return 0;
    }

    output_priv->priv_sta &= ~CODEC_RUNING_MASK;
    ck_codec_output_dma_stop(handle->ch_idx);
    ringbuffer_reset(&output_priv->fifo);

    return 0;
}

uint32_t codec_output_write(codec_output_t *handle, uint8_t *buf, uint32_t length)
{
    if (handle == NULL || buf == NULL) {
        return 0;
    }

    int ch = handle->ch_idx;
    ck_codec_ch_priv_t *output_priv = &ck_codec_output_instance[ch];

    if (output_priv->bit_width >= 16) {
        length -= length % 4;
    } else {
        length -= length % 2;
    }

    int ret_len = ringbuffer_in(&output_priv->fifo, buf, length);

    if ((output_priv->priv_sta & CODEC_RUNING_MASK) == 0
        && (output_priv->priv_sta & CODEC_READT_TO_START_MASK)) {
        if (ck_codec_output_dma_start(ch) == 0) {
            output_priv->priv_sta |= CODEC_RUNING_MASK;
        }
    }

    return ret_len;
}

int32_t codec_output_buf_reset(codec_output_t *handle)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int ch = handle->ch_idx;
    ck_codec_ch_priv_t *output_priv = &ck_codec_output_instance[ch];

    ringbuffer_reset(&output_priv->fifo);

    return 0;
}

uint32_t codec_output_buf_avail(codec_output_t *handle)
{
    if (handle == NULL) {
        return 0;
    }

    int ch = handle->ch_idx;
    ck_codec_ch_priv_t *input_priv = &ck_codec_output_instance[ch];
    return ringbuffer_avail(&input_priv->fifo);
}

int32_t codec_output_pause(codec_output_t *handle)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    //ck_codec_output_dma_stop(handle->ch_idx);
    //FIXME: patch for baoyin, ugly. Slight popping now yet
    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    ck_codec_dac_mute_en(codec_priv, 1);
    return 0;
}

int32_t codec_output_resume(codec_output_t *handle)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    //ck_codec_output_dma_start(handle->ch_idx);
    //FIXME: patch for baoyin, ugly. Slight popping now yet
    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    ck_codec_dac_mute_en(codec_priv, 0);
    return 0;
}

int32_t codec_input_set_digital_gain(codec_input_t *handle, int32_t gain)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int ch = handle->ch_idx;
    int codec_idx = handle->codec_idx;

    if (gain > 31 || gain < 0) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    ck_codec_adc_set_digital_gain(codec_priv, ch, gain);

    return 0;
}

int32_t codec_input_get_digital_gain(codec_input_t *handle, int32_t *gain)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (gain == NULL) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    int ch = handle->ch_idx;
    int codec_idx = handle->codec_idx;

    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    *gain = ck_codec_adc_get_digital_gain(codec_priv, ch);

    return 0;
}

int32_t codec_input_set_analog_gain(codec_input_t *handle, int32_t gain)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int ch = handle->ch_idx;
    int codec_idx = handle->codec_idx;

    if (gain > 20 || gain < 0) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    ck_codec_adc_set_boost_gain(codec_priv, ch, gain);

    return 0;
}

int32_t codec_input_get_analog_gain(codec_input_t *handle, int32_t *gain)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (gain == NULL) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    int ch = handle->ch_idx;
    int codec_idx = handle->codec_idx;

    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    *gain = ck_codec_adc_get_boost_gain(codec_priv, ch);

    return 0;
}

int32_t codec_input_set_mixer_gain(codec_input_t *handle, int32_t gain)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (gain > 0 || gain < -31) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    gain = -gain;
    ck_codec_adc_mixer_set_gain(codec_priv, gain);

    return 0;
}

int32_t codec_input_get_mixer_gain(codec_input_t *handle, int32_t *gain)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (gain == NULL) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    int codec_idx = handle->codec_idx;

    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    *gain = -ck_codec_adc_mixer_get_gain(codec_priv);

    return 0;
}

int32_t codec_input_mute(codec_input_t *handle, int en)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int codec_idx = handle->codec_idx;
    int ch = handle->ch_idx;

    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    ck_codec_adc_mute_en(codec_priv, ch, en);

    return 0;
}

int32_t codec_output_set_digital_left_gain(codec_output_t *handle, int32_t val)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (val < -31 || val > 32) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    if (val <= 0) {
        val = -val;
    } else {
        val = 64 - val;
    }

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    ck_codec_dac_set_digital_left_gan(codec_priv, val);

    return 0;
}

int32_t codec_output_set_digital_right_gain(codec_output_t *handle, int32_t val)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (val < -31 || val > 32) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    if (val <= 0) {
        val = -val;
    } else {
        val = 64 - val;
    }

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];
    ck_codec_dac_set_digital_right_gan(codec_priv, val);

    return 0;
}

int32_t codec_output_set_analog_left_gain(codec_output_t *handle, int32_t val)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (val < -31 || val > 0) {
        return -1;
    }

    val = -val;

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    ck_codec_dac_set_analog_left_gan(codec_priv, (uint32_t)val);

    return 0;
}

int32_t codec_output_set_analog_right_gain(codec_output_t *handle, int32_t val)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (val < -31 || val > 0) {
        return -1;
    }

    val = -val;

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    ck_codec_dac_set_analog_right_gan(codec_priv, (uint32_t)val);

    return 0;
}

int32_t codec_output_set_mixer_left_gain(codec_output_t *handle, int32_t val)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (val < -31 || val > 0) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    val = -val;
    ck_codec_dac_set_mix_left_gain(codec_priv, val);

    return 0;
}
int32_t codec_output_set_mixer_right_gain(codec_output_t *handle, int32_t val)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (val < -31 || val > 0) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    val = -val;
    ck_codec_dac_set_mix_right_gain(codec_priv, val);

    return 0;
}

int32_t codec_output_get_mixer_left_gain(codec_output_t *handle, int32_t *val)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (val == NULL) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    int32_t gan = ck_codec_dac_get_mix_left_gain(codec_priv);
    *val = -gan;
    return 0;
}

int32_t codec_output_get_mixer_right_gain(codec_output_t *handle, int32_t *val)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (val == NULL) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    int32_t gan = ck_codec_dac_get_mix_right_gain(codec_priv);
    *val = -gan;
    return 0;
}

int32_t codec_output_get_digital_left_gain(codec_output_t *handle, int32_t *val)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (val == NULL) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    int32_t gan = ck_codec_dac_get_digital_left_gan(codec_priv);

    if (gan <= 31) {
        gan = -gan;
    } else {
        gan = 64 - gan;
    }

    *val = gan;

    return 0;
}

int32_t codec_output_get_digital_right_gain(codec_output_t *handle, int32_t *val)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (val == NULL) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    int32_t gan = ck_codec_dac_get_digital_right_gan(codec_priv);

    if (gan <= 31) {
        gan = -gan;
    } else {
        gan = 64 - gan;
    }

    *val = gan;
    return 0;
}

int32_t codec_output_get_analog_left_gain(codec_output_t *handle, int32_t *val)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (val == NULL) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    int32_t gan = ck_codec_dac_get_analog_left_gan(codec_priv);
    *val = -gan;
    return 0;
}
int32_t codec_output_get_analog_right_gain(codec_output_t *handle, int32_t *val)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    if (val == NULL) {
        return ERR_CODEC(DRV_ERROR_PARAMETER);
    }

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    int32_t gan = ck_codec_dac_get_analog_right_gan(codec_priv);
    *val = -gan;
    return 0;
}

int32_t codec_output_mute(codec_output_t *handle, int en)
{
    CSI_PARAM_CHK(handle, CSI_ERROR);

    int codec_idx = handle->codec_idx;
    csi_dev_t *codec_priv = &ck_codec_instance_v2[codec_idx];

    ck_codec_dac_mute_en(codec_priv, en);

    return 0;
}

__attribute__((section(".tcm1"))) static void ck_codec_whisper_iqrhandle()
{
    csi_dev_t *codec_priv = &ck_codec_instance_v2[0];
    ck_codec_reg_t *addr = (ck_codec_reg_t *)codec_priv->reg_base;

    addr->TRAN_SEL = 0x0;
    addr->IFR_WT = 0x1;

    ck_codec_ch_priv_t *ch = &ck_codec_input_instance[0];

    if (ch->cb != NULL) {
        ///<CODEC_EVENT_VAD_TRIGGER
        ch->cb(0, 7, ch->cb_arg);
    }
}

static void delay(int ms)
{
    volatile int i, j;

    for (i = 0; i < ms; i++) {
        for (j = 0; j < i; j++) {
            ;
        }
    }
}

int32_t codec_vad_enable(codec_input_t *handle, int en)
{
#define PREG32(addr) *((volatile unsigned int *)addr)

    if (handle->codec_idx >= CONFIG_CODEC_NUM || handle->codec_idx < 0) {
        return -1;
    }

    if (handle->ch_idx != 0) {
        return -1;
    }

    csi_dev_t *codec_priv = &ck_codec_instance_v2[handle->codec_idx];
    ck_codec_reg_t *addr = (ck_codec_reg_t *)codec_priv->reg_base;

    if (en) {
        PREG32(0x8b00006c) = 0x1;
        PREG32(0x8b000068) = 0x3FFE;
        PREG32(0x8b000068) = 0x3FFF;

        addr->TRAN_SEL = 0x0;
        addr->PATH_EN |= 0x1;
        addr->CR_VIC = 0x0;  //digital detection mode
        addr->ICR = 0x0;  //interrupt form: high level
        addr->CR_ADC[0] = 0x20;

        addr->IMR_WT = 0x0;
        addr->CR_WT_2 = 0x80;
        delay(1000);
        addr->CR_WT_3 = handle->vad_threshold;
        delay(1000);
        addr->IFR_WT = 0x1;
        addr->ICR_WT = 0x0;
        delay(1000);
        addr->CR_WT = 0x80;

        csi_irq_enable(WJ_CODEC_WHISPER_IRQn);
        csi_irq_attach(WJ_CODEC_WHISPER_IRQn, ck_codec_whisper_iqrhandle, codec_priv);
        PREG32(0xe000e280) = 0x10000;
        PREG32(0xE000E140) = 0x40;
    } else {
        addr->TRAN_SEL = 0x0;
        addr->CR_WT = 0x0;
        addr->PATH_EN = 0;
        addr->CR_ADC[0] = 0;
        addr->CR_WT_2 = 0;
        csi_irq_disable(WJ_CODEC_WHISPER_IRQn);
        csi_irq_detach(WJ_CODEC_WHISPER_IRQn);
    }

    return 0;
}

void ck_codec_lpm(uint32_t idx, uint32_t state)
{
    if (idx >= CONFIG_CODEC_NUM) {
        return;
    }

    if (state == 1) {
        csi_dev_t *priv = &ck_codec_instance_v2[idx];
        ck_codec_inerrupt_disable_all(priv);
        // ck_codec_sb_power_ctrl(priv, 1);
        csi_irq_disable(WJ_CODEC_WHISPER_IRQn);
        csi_irq_detach(WJ_CODEC_WHISPER_IRQn);
    } else if (state == 0) {
        csi_dev_t *priv = &ck_codec_instance_v2[idx];
        priv->idx = idx;
        priv->reg_base = WJ_CODEC_BASE;
        priv->irq_num = WJ_GSK_INTC_IRQn;
        priv->dev_tag = DEV_WJ_CODEC_TAG;

        ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->reg_base;
        /*IRQ High level*/
        addr->ICR = 0;
        addr->IMR = 0x30;

        ck_codec_sb_power_ctrl(priv, 0);
        ck_codec_mclk_freq_select(priv, CK_CODEC_MCLK_12M);
        ck_codec_tx_threshold_val(priv, 4);
        ck_codec_rx_threshold_val(priv, 4);

        ck_codec_inerrupt_disable_all(priv);
        uint32_t mask = 0xff | 0x3ff << 18;
        ck_codec_inerrupt_ctrl(priv, mask, 1);

        csi_irq_attach((uint32_t)priv->irq_num, dw_codec_handler, priv);
        csi_irq_enable((uint32_t)priv->irq_num);

        /*wait codec power ready*/
        uint32_t time_out = CODEC_BUSY_TIMEOUT;

        while (!(addr->SR & (1 << 7)) && time_out--);

        if (time_out == 0) {
            return;
        }

        time_out = CODEC_BUSY_TIMEOUT;

        /*wati codec ready*/
        while ((addr->SR & 0x01) && time_out--);

        if (time_out == 0) {
            return;
        }
    }
}
