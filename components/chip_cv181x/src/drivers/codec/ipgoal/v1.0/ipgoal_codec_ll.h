/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ipgoal_codec_ll.h
 * @brief    header file for wj codec ll driver
 * @version  V1.0
 * @date     8. May 2020
 ******************************************************************************/

#ifndef _IPGOAL_CODEC_LL_H_
#define _IPGOAL_CODEC_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * CODEC register bit definitions
 */

/* FC_REG00, offset: 0x00 */
#define IPGOAL_FC_REG00_HP_MUTE_Pos                         (0U)
#define IPGOAL_FC_REG00_HP_MUTE_Msk                         (0x1U << IPGOAL_FC_REG00_HP_MUTE_Pos)
#define IPGOAL_FC_REG00_HP_MUTE                             IPGOAL_FC_REG00_HP_MUTE_Msk

#define IPGOAL_FC_REG00_DAC_EN_Pos                          (1U)
#define IPGOAL_FC_REG00_DAC_EN_Msk                          (0x1U << IPGOAL_FC_REG00_DAC_EN_Pos)
#define IPGOAL_FC_REG00_DAC_EN                              IPGOAL_FC_REG00_DAC_EN_Msk

#define IPGOAL_FC_REG00_DEPOP_EN_Pos                        (2U)
#define IPGOAL_FC_REG00_DEPOP_EN_Msk                        (0x1U << IPGOAL_FC_REG00_DEPOP_EN_Pos)
#define IPGOAL_FC_REG00_DEPOP_EN                            IPGOAL_FC_REG00_DEPOP_EN_Msk

#define IPGOAL_FC_REG00_PD_HP_Pos                           (4U)
#define IPGOAL_FC_REG00_PD_HP_Msk                           (0x1U << IPGOAL_FC_REG00_PD_HP_Pos)
#define IPGOAL_FC_REG00_PD_HP                               IPGOAL_FC_REG00_PD_HP_Msk

#define IPGOAL_FC_REG00_PD_BIAS_Pos                         (5U)
#define IPGOAL_FC_REG00_PD_BIAS_Msk                         (0x1U << IPGOAL_FC_REG00_PD_BIAS_Pos)
#define IPGOAL_FC_REG00_PD_BIAS                             IPGOAL_FC_REG00_PD_BIAS_Msk

#define IPGOAL_FC_REG00_PD_BUFFER_Pos                       (6U)
#define IPGOAL_FC_REG00_PD_BUFFER_Msk                       (0x1U << IPGOAL_FC_REG00_PD_BUFFER_Pos)
#define IPGOAL_FC_REG00_PD_BUFFER                           IPGOAL_FC_REG00_PD_BUFFER_Msk

#define IPGOAL_FC_REG00_SUSPEND_Pos                         (7U)
#define IPGOAL_FC_REG00_SUSPEND_Msk                         (0x1U << IPGOAL_FC_REG00_SUSPEND_Pos)
#define IPGOAL_FC_REG00_SUSPEND                             IPGOAL_FC_REG00_SUSPEND_Msk

/* FC_REG01, offset: 0x01 */
#define IPGOAL_FC_REG01_I_DEPOP_Pos                         (0U)
#define IPGOAL_FC_REG01_I_DEPOP_Msk                         (0x7U << IPGOAL_FC_REG00_I_DEPOP_Pos)

#define IPGOAL_FC_REG01_LINEIN_EN_Pos                       (3U)
#define IPGOAL_FC_REG01_LINEIN_EN_Msk                       (0x1U << IPGOAL_FC_REG00_LINEIN_EN_Pos)
#define IPGOAL_FC_REG01_LINEIN_EN                           IPGOAL_FC_REG00_LINEIN_EN_Msk

#define IPGOAL_FC_REG01_CHANNEL_SET_Pos                     (4U)
#define IPGOAL_FC_REG01_CHANNEL_SET_Msk                     (0x3U << IPGOAL_FC_REG00_CHANNEL_SET_Pos)

/* FC_REG02, offset: 0x02 */
#define IPGOAL_FC_REG02_HP_TIME_SET_Pos                     (0U)
#define IPGOAL_FC_REG02_HP_TIME_SET_Msk                     (0x3U << IPGOAL_FC_REG02_HP_TIME_SET_Pos)

#define IPGOAL_FC_REG02_LINEIN1_EN_Pos                      (2U)
#define IPGOAL_FC_REG02_LINEIN1_EN_Msk                      (0x1U << IPGOAL_FC_REG00_LINEIN_EN_Pos)
#define IPGOAL_FC_REG02_LINEIN1_EN                          IPGOAL_FC_REG00_LINEIN_EN_Msk

#define IPGOAL_FC_REG02_LINEIN2_EN_Pos                      (3U)
#define IPGOAL_FC_REG02_LINEIN2_EN_Msk                      (0x1U << IPGOAL_FC_REG02_LINEIN2_EN_Pos)
#define IPGOAL_FC_REG02_LINEIN2_EN                          IPGOAL_FC_REG02_LINEIN2_EN_Msk

#define IPGOAL_FC_REG02_ADC_MUTE_SEL_Pos                    (4U)
#define IPGOAL_FC_REG02_ADC_MUTE_SEL_Msk                    (0x3U << IPGOAL_FC_REG02_ADC_MUTE_SEL_Pos)

#define IPGOAL_FC_REG02_ADC_MUTE_EN_Pos                     (6U)
#define IPGOAL_FC_REG02_ADC_MUTE_EN_Msk                     (0x1U << IPGOAL_FC_REG02_ADC_MUTE_EN_Pos)
#define IPGOAL_FC_REG02_ADC_MUTE_EN                         IPGOAL_FC_REG02_ADC_MUTE_EN_Msk

#define IPGOAL_FC_REG02_ADC_MODE_SEL_Pos                    (7U)
#define IPGOAL_FC_REG02_ADC_MODE_SEL_Msk                    (0x1U << IPGOAL_FC_REG02_ADC_MODE_SEL_Pos)
#define IPGOAL_FC_REG02_ADC_MODE_SEL                        IPGOAL_FC_REG02_ADC_MODE_SEL_Msk

/* FC_REG03, offset: 0x03 */
#define IPGOAL_FC_REG03_PM_ADC_Pos                          (0U)
#define IPGOAL_FC_REG03_PM_ADC_Msk                          (0x3U << IPGOAL_FC_REG03_PM_ADC_Pos)

#define IPGOAL_FC_REG03_CIC_MUX_EN_Pos                      (2U)
#define IPGOAL_FC_REG03_CIC_MUX_EN_Msk                      (0x1U << IPGOAL_FC_REG03_CIC_MUX_EN_Pos)
#define IPGOAL_FC_REG03_CIC_MUX_EN                          IPGOAL_FC_REG03_CIC_MUX_EN_Msk

#define IPGOAL_FC_REG03_HPF_MUX_EN_Pos                      (3U)
#define IPGOAL_FC_REG03_HPF_MUX_EN_Msk                      (0x1U << IPGOAL_FC_REG03_HPF_MUX_EN_Pos)
#define IPGOAL_FC_REG03_HPF_MUX_EN                          IPGOAL_FC_REG03_HPF_MUX_EN_Msk

#define IPGOAL_FC_REG03_CIC_DOWN_MUX_Pos                    (4U)
#define IPGOAL_FC_REG03_CIC_DOWN_MUX_Msk                    (0x1U << IPGOAL_FC_REG03_CIC_DOWN_MUX_Pos)
#define IPGOAL_FC_REG03_CIC_DOWN_MUX                        IPGOAL_FC_REG03_CIC_DOWN_MUX_Msk

#define IPGOAL_FC_REG03_HPF_EN_Pos                          (5U)
#define IPGOAL_FC_REG03_HPF_EN_Msk                          (0x1U << IPGOAL_FC_REG03_HPF_EN_Pos)
#define IPGOAL_FC_REG03_HPF_EN                              IPGOAL_FC_REG03_HPF_EN_Msk

#define IPGOAL_FC_REG03_HP_PREV_EN_Pos                      (6U)
#define IPGOAL_FC_REG03_HP_PREV_EN_Msk                      (0x1U << IPGOAL_FC_REG03_HP_PREV_EN_Pos)
#define IPGOAL_FC_REG03_HP_PREV_EN                          IPGOAL_FC_REG03_HP_PREV_EN_Msk

#define IPGOAL_FC_REG03_PD_SDM_Pos                          (7U)
#define IPGOAL_FC_REG03_PD_SDM_Msk                          (0x1U << IPGOAL_FC_REG03_PD_SDM_Pos)
#define IPGOAL_FC_REG03_PD_SDM                              IPGOAL_FC_REG03_PD_SDM_Msk

/* FC_REG04, offset: 0x04 */
#define IPGOAL_FC_REG04_MIC_PGA_Pos                         (0U)
#define IPGOAL_FC_REG04_MIC_PGA_Msk                         (0xFU << IPGOAL_FC_REG04_MIC_PGA_Pos)

#define IPGOAL_FC_REG04_MIC_PFL_Pos                         (4U)
#define IPGOAL_FC_REG04_MIC_PFL_Msk                         (0xFU << IPGOAL_FC_REG04_MIC_PFL_Pos)

/* FC_REG05, offset: 0x05 */
#define IPGOAL_FC_REG05_CHOP_CLK_Pos                        (0U)
#define IPGOAL_FC_REG05_CHOP_CLK_Msk                        (0x3U << IPGOAL_FC_REG05_CHOP_CLK_Pos)

#define IPGOAL_FC_REG05_PD_CHOP_Pos                         (2U)
#define IPGOAL_FC_REG05_PD_CHOP_Msk                         (0x1U << IPGOAL_FC_REG05_PD_CHOP_Pos)
#define IPGOAL_FC_REG05_PD_CHOP                             IPGOAL_FC_REG05_PD_CHOP_Msk

#define IPGOAL_FC_REG05_MIC_PGA_RIN_Pos                     (4U)
#define IPGOAL_FC_REG05_MIC_PGA_RIN_Msk                     (0x3U << IPGOAL_FC_REG05_MIC_PGA_RIN_Pos)

#define IPGOAL_FC_REG05_MIC_N_EN_Pos                        (6U)
#define IPGOAL_FC_REG05_MIC_N_EN_Msk                        (0x1U << IPGOAL_FC_REG05_MIC_N_EN_Pos)
#define IPGOAL_FC_REG05_MIC_N_EN                            IPGOAL_FC_REG05_MIC_N_EN_Msk

#define IPGOAL_FC_REG05_MIC_P_EN_Pos                        (7U)
#define IPGOAL_FC_REG05_MIC_P_EN_Msk                        (0x1U << IPGOAL_FC_REG05_MIC_P_EN_Pos)
#define IPGOAL_FC_REG05_MIC_P_EN                            IPGOAL_FC_REG05_MIC_P_EN_Msk

/* FC_REG06, offset: 0x06 */
#define IPGOAL_FC_REG06_VMID_EN_Pos                         (0U)
#define IPGOAL_FC_REG06_VMID_EN_Msk                         (0x1U << IPGOAL_FC_REG06_VMID_EN_Pos)
#define IPGOAL_FC_REG06_VMID_EN                             IPGOAL_FC_REG06_VMID_EN_Msk

#define IPGOAL_FC_REG06_VMID_RIN_Pos                        (1U)
#define IPGOAL_FC_REG06_VMID_RIN_Msk                        (0x1U << IPGOAL_FC_REG06_VMID_RIN_Pos)
#define IPGOAL_FC_REG06_VMID_RIN                            IPGOAL_FC_REG06_VMID_RIN_Msk

#define IPGOAL_FC_REG06_PD_MICBIAS_Pos                      (2U)
#define IPGOAL_FC_REG06_PD_MICBIAS_Msk                      (0x1U << IPGOAL_FC_REG06_PD_MICBIAS_Pos)
#define IPGOAL_FC_REG06_PD_MICBIAS                          IPGOAL_FC_REG06_PD_MICBIAS_Msk

#define IPGOAL_FC_REG06_PD_MIC_PGA_Pos                      (3U)
#define IPGOAL_FC_REG06_PD_MIC_PGA_Msk                      (0x1U << IPGOAL_FC_REG06_PD_MIC_PGA_Pos)
#define IPGOAL_FC_REG06_PD_MIC_PGA                          IPGOAL_FC_REG06_PD_MIC_PGA_Msk

#define IPGOAL_FC_REG06_SDM_GAIN_Pos                        (4U)
#define IPGOAL_FC_REG06_SDM_GAIN_Msk                        (0x3U << IPGOAL_FC_REG06_SDM_GAIN_Pos)

/* FC_REG07, offset: 0x07 */
#define IPGOAL_FC_REG07_PM_DAC_Pos                          (0U)
#define IPGOAL_FC_REG07_PM_DAC_Msk                          (0x3U << IPGOAL_FC_REG07_PM_DAC_Pos)

#define IPGOAL_FC_REG07_DATA_MUTE_Pos                       (2U)
#define IPGOAL_FC_REG07_DATA_MUTE_Msk                       (0x1U << IPGOAL_FC_REG07_DATA_MUTE_Pos)
#define IPGOAL_FC_REG07_DATA_MUTE                           IPGOAL_FC_REG07_DATA_MUTE_Msk

#define IPGOAL_FC_REG07_DATA_CLEAR_Pos                      (3U)
#define IPGOAL_FC_REG07_DATA_CLEAR_Msk                      (0x1U << IPGOAL_FC_REG07_DATA_CLEAR_Pos)
#define IPGOAL_FC_REG07_DATA_CLEAR                          IPGOAL_FC_REG07_DATA_CLEAR_Msk

#define IPGOAL_FC_REG07_RAM_CLEAR_Pos                       (4U)
#define IPGOAL_FC_REG07_RAM_CLEAR_Msk                       (0x1U << IPGOAL_FC_REG07_RAM_CLEAR_Pos)
#define IPGOAL_FC_REG07_RAM_CLEAR                           IPGOAL_FC_REG07_RAM_CLEAR_Msk

#define IPGOAL_FC_REG07_SDM_FPS_Pos                         (5U)
#define IPGOAL_FC_REG07_SDM_FPS_Msk                         (0x1U << IPGOAL_FC_REG07_SDM_FPS_Pos)
#define IPGOAL_FC_REG07_SDM_FPS                             IPGOAL_FC_REG07_SDM_FPS_Msk

#define IPGOAL_FC_REG07_DAC_MODE_SEL_Pos                    (6U)
#define IPGOAL_FC_REG07_DAC_MODE_SEL_Msk                    (0x1U << IPGOAL_FC_REG07_DAC_MODE_SEL_Pos)
#define IPGOAL_FC_REG07_DAC_MODE_SEL                        IPGOAL_FC_REG07_DAC_MODE_SEL_Msk

/* FC_REG08, offset: 0x08 */
#define IPGOAL_FC_REG08_GAIN_SET_Pos                        (0U)
#define IPGOAL_FC_REG08_GAIN_SET_Msk                        (0x1FU << IPGOAL_FC_REG08_GAIN_SET_Pos)

#define IPGOAL_FC_REG08_OFFSET_EN_Pos                       (5U)
#define IPGOAL_FC_REG08_OFFSET_EN_Msk                       (0x1U << IPGOAL_FC_REG08_OFFSET_EN_Pos)
#define IPGOAL_FC_REG08_OFFSET_EN                           IPGOAL_FC_REG08_OFFSET_EN_Msk

#define IPGOAL_FC_REG08_DWA_EN_Pos                          (6U)
#define IPGOAL_FC_REG08_DWA_EN_Msk                          (0x1U << IPGOAL_FC_REG08_DWA_EN_Pos)
#define IPGOAL_FC_REG08_DWA_EN                              IPGOAL_FC_REG08_DWA_EN_Msk

#define IPGOAL_FC_REG08_PD_DAC_Pos                          (7U)
#define IPGOAL_FC_REG08_PD_DAC_Msk                          (0x1U << IPGOAL_FC_REG08_PD_DAC_Pos)
#define IPGOAL_FC_REG08_PD_DAC                              IPGOAL_FC_REG08_PD_DAC_Msk

/* FC_REG09, offset: 0x09 */
#define IPGOAL_FC_REG09_HPF_SET_AVER_Pos                    (0U)
#define IPGOAL_FC_REG09_HPF_SET_AVER_Msk                    (0x1FU << IPGOAL_FC_REG09_HPF_SET_AVER_Pos)

#define IPGOAL_FC_REG09_HPF_SET_AVER21_16_Pos               (7U)
#define IPGOAL_FC_REG09_HPF_SET_AVER21_16_Msk               (0x1U << IPGOAL_FC_REG09_HPF_SET_AVER21_16_Pos)

/* FC_REG0A, offset: 0x0A */
#define IPGOAL_FC_REG0A_HPF_SET_AVER15_8_Pos                (0U)
#define IPGOAL_FC_REG0A_HPF_SET_AVER15_8_Msk                (0xFFU << IPGOAL_FC_REG0A_HPF_SET_AVER15_8_Pos)

/* FC_REG0B, offset: 0x0B */
#define IPGOAL_FC_REG0B_HPF_SET_AVER7_0_Pos                 (0U)
#define IPGOAL_FC_REG0B_HPF_SET_AVER7_0_Msk                 (0xFFU << IPGOAL_FC_REG09_HPF_SET_AVER7_0_Pos)


typedef struct {
    __IOM uint8_t CODEC_FC_REG00;           /* Offset: 0x00h (R/W)  CODEC REG00 */
    __IOM uint8_t CODEC_FC_REG01;           /* Offset: 0x01h (R/W)  CODEC REG01 */
    __IOM uint8_t CODEC_FC_REG02;           /* Offset: 0x02h (R/W)  CODEC REG02 */
    __IOM uint8_t CODEC_FC_REG03;           /* Offset: 0x03h (R/W)  CODEC REG03 */
    __IOM uint8_t CODEC_FC_REG04;           /* Offset: 0x04h (R/W)  CODEC REG04 */
    __IOM uint8_t CODEC_FC_REG05;           /* Offset: 0x05h (R/W)  CODEC REG05 */
    __IOM uint8_t CODEC_FC_REG06;           /* Offset: 0x06h (R/W)  CODEC REG06 */
    __IOM uint8_t CODEC_FC_REG07;           /* Offset: 0x07h (R/W)  CODEC REG07 */
    __IOM uint8_t CODEC_FC_REG08;           /* Offset: 0x08h (R/W)  CODEC REG08 */
    __IOM uint8_t CODEC_FC_REG09;           /* Offset: 0x09h (R/W)  CODEC REG09 */
    __IOM uint8_t CODEC_FC_REG0A;           /* Offset: 0x0Ah (R/W)  CODEC REG0A */
    __IOM uint8_t CODEC_FC_REG0B;           /* Offset: 0x0Bh (R/W)  CODEC REG0B */
} wj_codec_regs_t;

static inline void ipgoal_set_fc_reg00(wj_codec_regs_t *codec_base, uint8_t data)
{
    codec_base->CODEC_FC_REG00 = data;
}

static inline void ipgoal_set_fc_reg01(wj_codec_regs_t *codec_base, uint8_t data)
{
    codec_base->CODEC_FC_REG01 = data;
}

static inline void ipgoal_set_fc_reg02(wj_codec_regs_t *codec_base, uint8_t data)
{
    codec_base->CODEC_FC_REG02 = data;
}

static inline void ipgoal_set_fc_reg03(wj_codec_regs_t *codec_base, uint8_t data)
{
    codec_base->CODEC_FC_REG03 = data;
}

static inline void ipgoal_set_fc_reg04(wj_codec_regs_t *codec_base, uint8_t data)
{
    codec_base->CODEC_FC_REG04 = data;
}

static inline void ipgoal_set_fc_reg05(wj_codec_regs_t *codec_base, uint8_t data)
{
    codec_base->CODEC_FC_REG05 = data;
}

static inline void ipgoal_set_fc_reg06(wj_codec_regs_t *codec_base, uint8_t data)
{
    codec_base->CODEC_FC_REG06 = data;
}

static inline void ipgoal_set_fc_reg07(wj_codec_regs_t *codec_base, uint8_t data)
{
    codec_base->CODEC_FC_REG07 = data;
}

static inline void ipgoal_set_fc_reg08(wj_codec_regs_t *codec_base, uint8_t data)
{
    codec_base->CODEC_FC_REG08 = data;
}

static inline void ipgoal_set_fc_reg09(wj_codec_regs_t *codec_base, uint8_t data)
{
    codec_base->CODEC_FC_REG09 = data;
}

static inline void ipgoal_set_fc_reg0a(wj_codec_regs_t *codec_base, uint8_t data)
{
    codec_base->CODEC_FC_REG0A = data;
}

static inline void ipgoal_set_fc_reg0b(wj_codec_regs_t *codec_base, uint8_t data)
{
    codec_base->CODEC_FC_REG0B = data;
}

#ifdef __cplusplus
}
#endif

#endif /* _IPGOAL_CODEDC_LL_H_ */
