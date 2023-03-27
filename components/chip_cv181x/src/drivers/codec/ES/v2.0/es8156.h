/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     es8156.h
 * @brief
 * @version
 * @date     2020-07-09
 ******************************************************************************/

#ifndef _ES8156_H_
#define _ES8156_H_

#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ES8156_RESET_CTL_REG00      0x00
#define ES8156_MAIN_CLK_REG01       0x01
#define ES8156_MODE_CONFIG_REG02    0x02
#define ES8156_M_LRCK_DIVH_REG03    0x03
#define ES8156_M_LRCK_DIVL_REG04    0x04
#define ES8156_M_CLK_CTL_REG05      0x05
#define ES8156_NFS_CONFIG_REG06     0x06
#define ES8156_MISC_CTL1_REG07      0x07
#define ES8156_CLOCK_OFF_REG08      0x08

#define ES8156_MISC_CTL2_REG09      0x09
#define ES8156_TIME_CTL1_REG0A      0x0A
#define ES8156_TIME_CTL2_REG0B      0x0B

#define ES8156_CSM_STATE_REG0C      0x0C
#define ES8156_P2S_CTL_REG0D        0x0D
#define ES8156_DAC_NS_REG10         0x10

#define ES8156_SDP_CFG1_REG11       0x11
#define ES8156_AUTOMUTE_CTL_REG12   0x12

#define ES8156_MUTE_CTL_REG13       0x13
#define ES8156_VOL_CTL_REG14        0x14
#define ES8156_ALC_CONFIG1_REG15    0x15

#define ES8156_ALC_CONFIG2_REG16    0x16
#define ES8156_ALC_LEVEL_REG17      0x17
#define ES8156_MISC_CTL3_REG18      0x18
#define ES8156_EQ_CTL1_REG19        0x19
#define ES8156_EQ_CTL2_REG1A        0x1A

#define ES8156_ANALOG_SYS_REG20     0x20
#define ES8156_ANALOG_SYS_REG21     0x21
#define ES8156_ANALOG_SYS_REG22     0x22
#define ES8156_ANALOG_SYS_REG23     0x23
#define ES8156_ANALOG_SYS_REG24     0x24
#define ES8156_ANALOG_SYS_REG25     0x25

#define ES8156_PAGE_SEL_REGFC       0xFC

#define ES8156_CHIP_ID1_REGFD       0xFD
#define ES8156_CHIP_ID0_REGFE       0xFE
#define ES8156_CHIP_VER_REGFF       0xFF

typedef enum {
    ES8156_I2S_SAMPLE_RATE_8000              = 8000U,
    ES8156_I2S_SAMPLE_RATE_16000             = 16000U,
    ES8156_I2S_SAMPLE_RATE_24000             = 24000U,
    ES8156_I2S_SAMPLE_RATE_32000             = 32000U,
    ES8156_I2S_SAMPLE_RATE_44100             = 44100U,
    ES8156_I2S_SAMPLE_RATE_48000             = 48000U,
    ES8156_I2S_SAMPLE_RATE_96000             = 96000U,
    ES8156_I2S_SAMPLE_RATE_128000            = 128000U,
} es8156_i2s_sample_rate_t;

typedef enum {
    ES8156_I2S_MODE_MASTER,
    ES8156_I2S_MODE_SLAVE,
} es8156_i2s_mode_t;

typedef enum {
    ES8156_NORMAL_I2S,
    ES8156_NORMAL_LSB_JUSTIFIED,
} es8156_protocol_t;

typedef enum {
    ES8156_16BIT_LENGTH,
    ES8156_18BIT_LENGTH,
    ES8156_20BIT_LENGTH,
    ES8156_24BIT_LENGTH,
    ES8156_32BIT_LENGTH,
} es8156_data_len_t;

typedef enum {
    ES8156_I2S_SCLK_16FS = 16U,
    ES8156_I2S_SCLK_32FS = 32U,
    ES8156_I2S_SCLK_48FS = 48U,
    ES8156_I2S_SCLK_64FS = 64U,
} es8156_i2s_sclk_freq_t;

typedef enum {
    ES8156_MCLK_256FS = 256U,
    ES8156_MCLK_384FS = 384U,
} es8156_mclk_freq_t;

typedef struct {
    es8156_i2s_sample_rate_t i2s_rate;              ///< es8156 i2s rate
    es8156_i2s_mode_t  i2s_mode;                    ///< es8156 i2s work mode
    es8156_protocol_t  i2s_protocol;                ///< es8156 i2s protocol
    es8156_data_len_t  data_len;                    ///< es8156 data len
    es8156_i2s_sclk_freq_t i2s_sclk_freq;           ///< es8156 lrck divider
    es8156_mclk_freq_t mclk_freq;                   ///< input mclk freq
} es8156_config_t;

typedef struct {
    int8_t                    es8156_addr;          ///< es8156 iic addr
    int32_t (*es8156_iic_write)(uint8_t addr, uint8_t *data, uint32_t date_len);
    int32_t (*es8156_iic_read)(uint8_t addr, uint8_t *data, uint32_t date_len);
} es8156_dev_t;


int32_t es8156_init(es8156_dev_t *dev, es8156_config_t *es8156_config);
int32_t es8156_uninit(es8156_dev_t *dev);
int32_t es8156_set_left_channel_mute(es8156_dev_t *dev, bool en);
int32_t es8156_set_right_channel_mute(es8156_dev_t *dev, bool en);
int32_t es8156_set_software_mute(es8156_dev_t *dev, bool en);
int32_t es8156_set_volume(es8156_dev_t *dev, unsigned char gain);
void es8156_read_all_register(es8156_dev_t *dev, uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif  /* _ES8156_H_*/
