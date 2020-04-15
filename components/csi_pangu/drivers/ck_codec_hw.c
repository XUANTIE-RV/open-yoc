/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     ck_codec_register.c
 * @brief    CSI Source File for codec Driver
 * @version  V1.0
 * @date     25. September 2019
 ******************************************************************************/

#include "stdlib.h"
#include <drv/irq.h>
#include <drv/dmac.h>
#include <stdbool.h>
#include <csi_core.h>
#include "ck_codec.h"
#include "dev_ringbuf.h"

#define ERR_CODEC(errno) (CSI_DRV_ERRNO_CODEC_BASE | errno)
#define CODEC_BUSY_TIMEOUT    0xffffffff
#define CODEC_NULL_PARAM_CHK(para) HANDLE_PARAM_CHK(para, ERR_CODEC(DRV_ERROR_PARAMETER))

#define REG_READ(val, addr) do{val = addr; val = addr;}while(0);

/*if sb en, some power is off*/
void ck_codec_sb_power_ctrl(ck_codec_priv_v2_t *priv, int en)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;

    if (en) {
        val = 0x07;
    }

    addr->CR_VIC = val;
}

void ck_codec_inerrupt_ctrl(ck_codec_priv_v2_t *priv, uint32_t mask, int en)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->INTR_ERR_CTRL);

    val &= ~mask;

    if (en) {
        val |= mask;
    }

    addr->INTR_ERR_CTRL = val;
}

void ck_codec_inerrupt_disable_all(ck_codec_priv_v2_t *priv)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;

    addr->INTR_ERR_CTRL = 0;
}

void ck_codec_mclk_freq_select(ck_codec_priv_v2_t *priv, ck_codec_mclk_freq_t freq)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = freq;

    addr->CR_CK = val;
}

void ck_codec_adc_bit_width_and_mode_set(ck_codec_priv_v2_t *priv)
{
    /*must set 0, 16bit and Parallel mode*/
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    addr->AICR_ADC = 0;
}

void ck_codec_tx_threshold_val(ck_codec_priv_v2_t *priv, uint32_t val)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t reg = 0;
    REG_READ(reg, addr->FIFO_TH_CTRL);
    reg &= ~(0x7);
    val -= 1;
    reg |= val & 0x7;

    addr->FIFO_TH_CTRL = 0x33;//val;
}

void ck_codec_rx_threshold_val(ck_codec_priv_v2_t *priv, uint32_t val)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t reg = 0;
    REG_READ(reg, addr->FIFO_TH_CTRL);
    reg &= ~(0x7 << 4);
    val -= 1;
    reg |= (val & 0x7) << 4;

    addr->FIFO_TH_CTRL = val;
}

void ck_codec_adc_channel_enable(ck_codec_priv_v2_t *priv, int ad_idx, int en)
{
    ad_idx /= 2;
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->AICR_ADC_2);
    val |= (0x01 << ad_idx);

    if (en) {
        val &= ~(0x01 << ad_idx);
    }

    addr->AICR_ADC_2 = val;
    //addr->AICR_ADC_2 = 0;
}

void ck_codec_adc_sample_rate_config(ck_codec_priv_v2_t *priv, uint32_t rate)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->FCR_ADC);
    val = rate;

    addr->FCR_ADC = val;
    // FIXME: 修复直流偏移问题
    /*
    FCR_ADC寄存器
    ADC12：bit4
    ADC34：bit5
    ADC56：bit6
    ADC78：bit7
    */
    addr->FCR_ADC |= (0xF << 4);
}

/*mode：0 disable, 1~3 enable, func mode*/
void ck_codec_adc_wnf_ctrl(ck_codec_priv_v2_t *priv, int ad_idx, int mode)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->CR_WNF);
    ad_idx /= 2;
    ad_idx *= 2;
    val &= ~(0x03 << ad_idx);

    if (mode) {
        val |= (mode << ad_idx);
    }

    addr->CR_WNF = val;
}

/*ad idx 0~7*/
void ck_codec_mic_set_bias(ck_codec_priv_v2_t *priv, int ad_idx, ck_codec_mic_bias_t bias)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->CR_MIC[ad_idx]);
    val &= ~(MICBIAS1_V_Msk);

    if (bias == CK_CODEC_MIC_BIAS_1800MV) {
        val |= MICBIAS1_V_Msk;
    }

    addr->CR_MIC[ad_idx] = val;
}

void ck_codec_disable_dmic(ck_codec_priv_v2_t *priv)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    addr->CR_DMIC12 = 0xC0;  //05: DMIC12 is active, c0:inactive
    addr->CR_DMIC34 = 0xC0;  //05: DMIC12 is active, c0:inactive
    addr->CR_DMIC56 = 0xC0;  //05: DMIC12 is active, c0:inactive
    addr->CR_DMIC78 = 0xC0;  //05: DMIC12 is active, c0:inactive
}

/*val 0~31 -> 0~ -31db*/
void ck_codec_adc_mixer_set_gain(ck_codec_priv_v2_t *priv, uint32_t gain)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;;
    addr->GCR_MIXADCL = gain;
    addr->GCR_MIXADCR = gain;
}

uint32_t ck_codec_adc_mixer_get_gain(ck_codec_priv_v2_t *priv)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;;
    return addr->GCR_MIXADCL;
}

/*ad_idx 0~7*/
void ck_codec_adc_mode_set(ck_codec_priv_v2_t *priv, int ad_idx, ck_codec_adc_mode_t mode)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->CR_MIC[ad_idx]);

    val &= ~(MICDIFF1_Msk);
    val |= mode << MICDIFF1_Pos;

    addr->CR_MIC[ad_idx] = val;
}

void ck_codec_mic_bias_en(ck_codec_priv_v2_t *priv, int ad_idx, int en)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->CR_MIC[ad_idx]);

    val &= ~(1 << SB_MICBIAS1_Pos);

    if (en == 0) {
        val |= 1 << SB_MICBIAS1_Pos;
    }

    addr->CR_MIC[ad_idx] = val;
}

void ck_codec_adc_sb_enable(ck_codec_priv_v2_t *priv, int ad_idx, int en)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    int offset = ad_idx % 2;
    ad_idx /= 2;
    uint32_t val = 0;
    REG_READ(val, addr->CR_ADC[ad_idx]);

    val &= ~(0x01 << (offset + 4));

    if (en) {
        val |= (0x01 << (offset + 4));
    }

    addr->CR_ADC[ad_idx] = val;
}

void ck_codec_adc_mute_en(ck_codec_priv_v2_t *priv, int ad_idx, int en)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    int reg_idx = ad_idx / 2;
    uint32_t val = 0;
    REG_READ(val, addr->CR_ADC[reg_idx]);

    val &= ~(0x01 << 7);

    if (en) {
        val |= (0x01 << 7);
    }

    addr->CR_ADC[reg_idx] = val;
}

void ck_codec_path_adc_en(ck_codec_priv_v2_t *priv, int adc_idx, int en)
{
    if (adc_idx > 7) {
        return;
    }

    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->PATH_EN);

    val &= ~(0x01 << adc_idx);

    if (en) {
        val |= (0x01 << adc_idx);
    }

    addr->PATH_EN = val;
}

/*Microphone boost stage gain programming value, val 0~7, analog gain*/
void ck_codec_adc_set_boost_gain(ck_codec_priv_v2_t *priv, int32_t adc_idx, uint32_t gain)
{
    if (gain > 0x07) {
        gain = 0x7;
    }

    uint32_t reg_idx = adc_idx / 2;
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->GCR_MIC[reg_idx]);

    if (adc_idx % 2) {
        val &= ~(0x07 << 3);
        gain = gain << 3;
    } else {
        val &= ~0x07;
    }

    val |= gain;

    addr->GCR_MIC[reg_idx] = val;
}

uint32_t ck_codec_adc_get_boost_gain(ck_codec_priv_v2_t *priv, int32_t adc_idx)
{
    uint32_t reg_idx = adc_idx / 2;
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->GCR_MIC[reg_idx]);

    if (adc_idx % 2) {
        val &= 0x07 << 3;
    } else {
        val &= 0x07;
    }

    return val;
}

/*adc digital filters digital gain programming value for channel*/
void ck_codec_adc_set_digital_gain(ck_codec_priv_v2_t *priv, int32_t adc_idx, uint32_t gain)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;

    if (gain > 31) {
        gain = 31;
    }

    addr->GCR_ADC[adc_idx] = gain;
}

uint32_t ck_codec_adc_get_digital_gain(ck_codec_priv_v2_t *priv, int32_t adc_idx)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    return addr->GCR_ADC[adc_idx];
}

/*********************DAC Registers********************/

void ck_codec_dac_bit_width_and_mode_set(ck_codec_priv_v2_t *priv, uint32_t bit_width)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->AICR_DAC);
    val &= ~(0x03 << 6);
    val |= bit_width;
    val &= ~(0x03);
    val &= !(0x01 << 4);

    addr->AICR_DAC = val;
}

/*
DAC audio interface activation
0: DAC audio interface active
1: DAC audio interface in power-down mode
*/
void ck_codec_dac_power_down_ctrl(ck_codec_priv_v2_t *priv, int en)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->AICR_DAC);

    val &= ~(0x01 << 4);

    if (en) {
        val |= 0x01 << 4;
    }

    addr->AICR_DAC = val;
}

void ck_codec_dac_sample_rate_config(ck_codec_priv_v2_t *priv, uint32_t rate)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->FCR_DAC);
    val = rate;

    addr->FCR_DAC = val;
}

void ck_codec_dac_mute_en(ck_codec_priv_v2_t *priv, int32_t en)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->CR_DAC);

    val &= ~(1 << 7);

    if (en) {
        val |= (1 << 7);
    }

    addr->CR_DAC = val;
}

void ck_codec_dac_mono_enable(ck_codec_priv_v2_t *priv, int32_t en)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->CR_DAC);

    val &= ~(1 << 5);

    if (en) {
        val |= (1 << 5);
    }

    addr->CR_DAC = val;
}

/*if en dac power off*/
void ck_codec_dac_sb_en(ck_codec_priv_v2_t *priv, int32_t en)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->CR_DAC);

    val &= ~(1 << 4);

    if (en) {
        val |= (1 << 4);
    }

    addr->CR_DAC = val;
}

/*val 0~31 -> 0~ -31db*/
void ck_codec_dac_set_mix_left_gain(ck_codec_priv_v2_t *priv, uint32_t val)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    addr->GCR_MIXDACL = val;
}

uint32_t ck_codec_dac_get_mix_left_gain(ck_codec_priv_v2_t *priv)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    return addr->GCR_MIXDACL;
}

/*val 0~31 -> 0~ -31db*/
void ck_codec_dac_set_mix_right_gain(ck_codec_priv_v2_t *priv, uint32_t val)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    addr->GCR_MIXDACR = val;
}

uint32_t ck_codec_dac_get_mix_right_gain(ck_codec_priv_v2_t *priv)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    return addr->GCR_MIXDACR;
}

/*val 0~31 = 0~-31db, 63~32 = 1~32db*/
void ck_codec_dac_set_digital_left_gan(ck_codec_priv_v2_t *priv, uint32_t val)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    addr->GCR_DACL = val;

}

uint32_t ck_codec_dac_get_digital_left_gan(ck_codec_priv_v2_t *priv)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    return addr->GCR_DACL;
}

/*val 0~31,seemingly no effect*/
void ck_codec_dac_set_digital_right_gan(ck_codec_priv_v2_t *priv, uint32_t val)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    addr->GCR_DACR = val;
}

uint32_t ck_codec_dac_get_digital_right_gan(ck_codec_priv_v2_t *priv)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    return addr->GCR_DACR;
}

/*val 0~31 = 0~-31db*/
void ck_codec_dac_set_analog_left_gan(ck_codec_priv_v2_t *priv, uint32_t val)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    addr->GCR_HPL = val;
}

uint32_t ck_codec_dac_get_analog_left_gan(ck_codec_priv_v2_t *priv)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    return addr->GCR_HPL;
}

/*val 0~31 = 0~-31db*/
void ck_codec_dac_set_analog_right_gan(ck_codec_priv_v2_t *priv, uint32_t val)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    addr->GCR_HPR = val;
}

uint32_t ck_codec_dac_get_analog_right_gan(ck_codec_priv_v2_t *priv)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    return addr->GCR_HPR;
}

void ck_codec_path_dac_en(ck_codec_priv_v2_t *priv, int en)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->PATH_EN);

    val &= ~(0x01 << 8);

    if (en) {
        val |= (0x01 << 8);
    }

    addr->PATH_EN = val;
}

void ck_codec_dac_headphone_mute(ck_codec_priv_v2_t *priv, int en)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->CR_HP);
    val &= ~(0x01 << 7);

    if (en) {
        val |= 0x01 << 7;
    }

    addr->CR_HP = val;
}

void ck_codec_dac_headphone_sb_enable(ck_codec_priv_v2_t *priv, int en)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;
    uint32_t val = 0;
    REG_READ(val, addr->CR_HP);
    val &= ~((0x01 << 5) | (0x01 << 4));

    if (en) {
        val |= 0x01 << 4 ;
    }

    addr->CR_HP = val;
}

/*val 0~15, The larger the number, the smaller the gain*/
void ck_codec_dac_headphone_set_right_gain(ck_codec_priv_v2_t *priv, uint32_t val)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;

    addr->GCR_HPR = val;
}

/*val 0~15, The larger the number, the smaller the gain*/
void ck_codec_dac_headphone_set_left_gain(ck_codec_priv_v2_t *priv, uint32_t val)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;

    addr->GCR_HPL = val;
}

/*val 0~15, The larger the number, the smaller the gain*/
void ck_codec_dac_headphone_get_right_gain(ck_codec_priv_v2_t *priv, uint32_t *val)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;

    *val = addr->GCR_HPR;
}

/*val 0~15, The larger the number, the smaller the gain*/
void ck_codec_dac_headphone_get_left_gain(ck_codec_priv_v2_t *priv, uint32_t *val)
{
    ck_codec_reg_t *addr = (ck_codec_reg_t *)priv->base;

    *val = addr->GCR_HPL;
}

void ck_codec_gasket_reset()
{
    *(volatile uint32_t *)0x8b000068 = 0x3FFE;
    *(volatile uint32_t *)0x8b000068 = 0x3FFF;
}

extern int32_t target_codec_init(int32_t idx, uint32_t *base, uint32_t *gsk_irq, void **gsk_handler);

#define CODEC_ERROR_IRQ_MSAK (0xff | 0x3ff << 18)
void ck_codec_irqhandler(int32_t idx)
{
    uint32_t base;
    uint32_t gsk_irq;
    void *gsk_handler;

    int32_t ret = target_codec_init(idx, &base, &gsk_irq, &gsk_handler);

    if (ret != idx) {
        return;
    }

    ck_codec_reg_t *addr = (ck_codec_reg_t *)base;
    uint32_t error_mask = 0;
    REG_READ(error_mask, addr->INTR_ERR_STA);
    addr->INTR_ERR_CLR = error_mask;

    if (error_mask & CODEC_ERROR_IRQ_MSAK) {
        ck_codec_dma_all_restart();
    }
}
