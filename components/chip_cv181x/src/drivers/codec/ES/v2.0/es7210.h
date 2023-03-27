/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     es7210.h
 * @brief
 * @version
 * @date     2020-07-09
 ******************************************************************************/

#ifndef _ES7210_H_
#define _ES7210_H_

#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ES7210_RESET_CTL_REG00      0x00
#define ES7210_CLK_ON_OFF_REG01     0x01
#define ES7210_MCLK_CTL_REG02       0x02
#define ES7210_MST_CLK_CTL_REG03    0x03
#define ES7210_MST_LRCDIVH_REG04    0x04
#define ES7210_MST_LRCDIVL_REG05    0x05
#define ES7210_DIGITAL_PDN_REG06    0x06
#define ES7210_ADC_OSR_REG07        0x07
#define ES7210_MODE_CFG_REG08       0x08

#define ES7210_TCT0_CHPINI_REG09    0x09
#define ES7210_TCT1_CHPINI_REG0A    0x0A
#define ES7210_CHIP_STA_REG0B       0x0B

#define ES7210_IRQ_CTL_REG0C        0x0C
#define ES7210_MISC_CTL_REG0D       0x0D
#define ES7210_DMIC_CTL_REG10       0x10

#define ES7210_SDP_CFG1_REG11       0x11
#define ES7210_SDP_CFG2_REG12       0x12

#define ES7210_ADC_AUTOMUTE_REG13   0x13
#define ES7210_ADC34_MUTE_REG14     0x14
#define ES7210_ADC12_MUTE_REG15     0x15

#define ES7210_ALC_SEL_REG16        0x16
#define ES7210_ALC_COM_CFG1_REG17   0x17
#define ES7210_ALC34_LVL_REG18      0x18
#define ES7210_ALC12_LVL_REG19      0x19
#define ES7210_ALC_COM_CFG2_REG1A   0x1A
#define ES7210_ADC4_MAX_GAIN_REG1B  0x1B
#define ES7210_ADC3_MAX_GAIN_REG1C  0x1C
#define ES7210_ADC2_MAX_GAIN_REG1D  0x1D
#define ES7210_ADC1_MAX_GAIN_REG1E  0x1E

#define ES7210_ADC34_HPF2_REG20     0x20
#define ES7210_ADC34_HPF1_REG21     0x21
#define ES7210_ADC12_HPF2_REG22     0x22
#define ES7210_ADC12_HPF1_REG23     0x23

#define ES7210_CHP_ID1_REG3D        0x3D
#define ES7210_CHP_ID0_REG3E        0x3E
#define ES7210_CHP_VER_REG3F        0x3F

#define ES7210_ANALOG_SYS_REG40     0x40

#define ES7210_MICBIAS12_REG41      0x41
#define ES7210_MICBIAS34_REG42      0x42
#define ES7210_MIC1_GAIN_REG43      0x43
#define ES7210_MIC2_GAIN_REG44      0x44
#define ES7210_MIC3_GAIN_REG45      0x45
#define ES7210_MIC4_GAIN_REG46      0x46
#define ES7210_MIC1_LP_REG47        0x47
#define ES7210_MIC2_LP_REG48        0x48
#define ES7210_MIC3_LP_REG49        0x49
#define ES7210_MIC4_LP_REG4A        0x4A
#define ES7210_MIC12_PDN_REG4B      0x4B
#define ES7210_MIC34_PDN_REG4C      0x4C

typedef enum {
    ES7210_SAMPLE_RATE_8000              = 8000U,
    ES7210_SAMPLE_RATE_11025             = 11025U,
    ES7210_SAMPLE_RATE_12000             = 12000U,
    ES7210_SAMPLE_RATE_16000             = 16000U,
    ES7210_SAMPLE_RATE_22050             = 22050U,
    ES7210_SAMPLE_RATE_24000             = 24000U,
    ES7210_SAMPLE_RATE_32000             = 32000U,
    ES7210_SAMPLE_RATE_44100             = 44100U,
    ES7210_SAMPLE_RATE_48000             = 48000U,
    ES7210_SAMPLE_RATE_96000             = 96000U,
    ES7210_SAMPLE_RATE_192000            = 192000U,
    ES7210_SAMPLE_RATE_256000            = 256000U,
} es7210_sample_rate_t;

typedef enum {
    ES7210_MODE_MASTER,
    ES7210_MODE_SLAVE,
} es7210_mode_t;

typedef enum {
    ES7210_I2S,
    ES7210_LSB_JUSTIFIED,
    ES7210_DSPA,
    ES7210_DSPB,
    ES7210_I2S_TDM,
    ES7210_LEFT_JUSTIFIED_TDM,
    ES7210_DSPA_TDM,
    ES7210_DSPB_TDM,
} es7210_protocol_t;

typedef enum {
    ES7210_16BIT_LENGTH = 16U,
    ES7210_18BIT_LENGTH = 18U,
    ES7210_20BIT_LENGTH = 20U,
    ES7210_24BIT_LENGTH = 24U,
    ES7210_32BIT_LENGTH = 32U,
} es7210_data_len_t;

typedef enum {
    ES7210_SCLK_16FS   = 16U,
    ES7210_SCLK_32FS   = 32U,
    ES7210_SCLK_48FS   = 48U,
    ES7210_SCLK_64FS   = 64U,
    ES7210_SCLK_128FS  = 128U,
    ES7210_SCLK_256FS  = 256U,
    ES7210_SCLK_1024FS = 1024U,
} es7210_sclk_freq_t;

typedef enum {
    ES7210_MCLK_256FS = 256U,
    ES7210_MCLK_384FS = 384U,
} es7210_mclk_freq_t;

typedef struct {
    es7210_sample_rate_t rate;                      ///< es7210 sample rate
    es7210_mode_t  mode;                            ///< es7210 work modes
    es7210_protocol_t  protocol;                    ///< es7210 protocols
    es7210_data_len_t  data_len;                    ///< es7210 data len
    es7210_sclk_freq_t sclk_freq;                   ///< es7210 lrck divider
    es7210_mclk_freq_t mclk_freq;                   ///< input mclk freq
} es7210_config_t;

typedef struct {
    int8_t                    es7210_addr;          ///< es7210 iic addr
    int32_t (*es7210_iic_write)(uint8_t addr, uint8_t *data, uint32_t date_len);
    int32_t (*es7210_iic_read)(uint8_t addr, uint8_t *data, uint32_t date_len);
} es7210_dev_t;


int32_t es7210_init(es7210_dev_t *dev, es7210_config_t *es7210_config);
int32_t es7210_uninit(es7210_dev_t *dev);
int32_t es7210_mic1_set_gain(es7210_dev_t *dev, unsigned char gain);
int32_t es7210_mic2_set_gain(es7210_dev_t *dev, unsigned char gain);
int32_t es7210_mic3_set_gain(es7210_dev_t *dev, unsigned char gain);
int32_t es7210_mic4_set_gain(es7210_dev_t *dev, unsigned char gain);
int32_t es7210_mic1_set_mute(es7210_dev_t *dev, bool en);
int32_t es7210_mic2_set_mute(es7210_dev_t *dev, bool en);
int32_t es7210_mic3_set_mute(es7210_dev_t *dev, bool en);
int32_t es7210_mic4_set_mute(es7210_dev_t *dev, bool en);
int32_t es7210_adc1_set_gain(es7210_dev_t *dev, unsigned char gain);
int32_t es7210_adc2_set_gain(es7210_dev_t *dev, unsigned char gain);
int32_t es7210_adc3_set_gain(es7210_dev_t *dev, unsigned char gain);
int32_t es7210_adc4_set_gain(es7210_dev_t *dev, unsigned char gain);
void es7210_read_all_register(es7210_dev_t *dev, uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif  /* _ES7210_H_*/
