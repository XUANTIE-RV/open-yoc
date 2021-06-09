/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_codec.h
 * @brief    header File for codec driver
 * @version  V1.0
 * @date     2019-08-26 14:23:53:23
  ******************************************************************************/
#ifndef _CK_CODEC_V1_H_
#define _CK_CODEC_V1_H_

#include "soc.h"
#include "stdint.h"
#include "drv/ringbuffer.h"
#include "drv_codec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))

/* SR, Offset: 0x00 */
#define PON_ACK_Pos 7U
#define PON_ACK_Msk (0x1U << PON_ACK_Pos)
#define IRQ_PENDING_Pos 6U
#define IRQ_PENDING_Msk (0x1U << IRQ_PENDING_Pos)
#define JACK_Pos 5U
#define JACK_Msk (0x1U << JACK_Pos)
#define DAC_LOCKED_Pos 4U
#define DAC_LOCKED_Msk (0x1U << DAC_LOCKED_Pos)
#define DAC_UNKNOWN_FS_Pos 2U
#define DAC_UNKNOWN_FS_Msk (0x1U << DAC_UNKNOWN_FS_Pos)
#define DAC_RAMP_IN_PROGRESS_Pos 0U
#define DAC_RAMP_IN_PROGRESS_Msk (0x1U << DAC_RAMP_IN_PROGRESS_Pos)

/* SR2, Offset: 0x04 */
#define DAC_SMUTE_IN_PROGRESS_Pos 7U
#define DAC_SMUTE_IN_PROGRESS_Msk (0x1U << DAC_SMUTE_IN_PROGRESS_Pos)
#define ADC12_SMUTE_IN_PROGRESS_Pos 6U
#define ADC12_SMUTE_IN_PROGRESS_Msk (0x1U << ADC12_SMUTE_IN_PROGRESS_Pos)

/* SR3, Offset: 0x08 */
#define ADC78_SMUTE_IN_PROGRESS_Pos 2U
#define ADC78_SMUTE_IN_PROGRESS_Msk (0x1U << ADC78_SMUTE_IN_PROGRESS_Pos)
#define ADC56_SMUTE_IN_PROGRESS_Pos 1U
#define ADC56_SMUTE_IN_PROGRESS_Msk (0x1U << ADC56_SMUTE_IN_PROGRESS_Pos)
#define ADC34_SMUTE_IN_PROGRESS_Pos 0U
#define ADC34_SMUTE_IN_PROGRESS_Msk (0x1U << ADC34_SMUTE_IN_PROGRESS_Pos)

/* ICR, Offset: 0x0C */
#define INT_FORM_Pos 6U
#define INT_FORM_Msk (0x3U << INT_FORM_Pos)

/* IMR, Offset: 0x10 */
#define ADAS_UNLOCK_MASK_Pos 7U
#define ADAS_UNLOCK_MASK_Msk (0x1U << ADAS_UNLOCK_MASK_Pos)
#define JACK_MASK_Pos 5U
#define JACK_MASK_Msk (0x1U << JACK_MASK_Pos)
#define SC_MASK_Pos 4U
#define SC_MASK_Msk (0x1U << SC_MASK_Pos)

/* IFR, Offset: 0x14 */
#define ADAS_UNLOCK_Pos 7U
#define ADAS_UNLOCK_Msk (0x1U << ADAS_UNLOCK_Pos)
#define JACK_EVENT_Pos 5U
#define JACK_EVENT_Msk (0x1U << JACK_EVENT_Pos)
#define SC_Pos 4U
#define SC_Msk (0x1U << SC_Pos)

/* CR_VIC, Offset: 0x18 */
#define SLEEP_ANALOG_Pos 2U
#define SLEEP_ANALOG_Msk (0x1U << SLEEP_ANALOG_Pos)
#define SB_ANALOG_Pos 1U
#define SB_ANALOG_Msk (0x1U << SB_ANALOG_Pos)
#define SB_Pos 0U
#define SB_Msk (0x1U << SB_Pos)

/* CR_CK, Offset: 0x1C */
#define CRYSTAL_Pos 0U
#define CRYSTAL_Msk (0x3U << CRYSTAL_Pos)

/* AICR_DAC, Offset: 0x20 */
#define DAC_ADWL_Pos 6U
#define DAC_ADWL_Msk (0x3U << DAC_ADWL_Pos)
#define DAC_SLAVE_Pos 5U
#define DAC_SLAVE_Msk (0x1U << DAC_SLAVE_Pos)
#define SB_AICR_DAC_Pos 4U
#define SB_AICR_DAC_Msk (0x1U << SB_AICR_DAC_Pos)
#define DAC_AUDIOIF_Pos 0U
#define DAC_AUDIOIF_Msk (0x3U << DAC_AUDIOIF_Pos)

/* AICR_ADC, Offset: 0x24 */
#define ADC_ADWL_Pos 6U
#define ADC_ADWL_Msk (0x3U << ADC_ADWL_Pos)
#define ADC_AUDIOIF_Pos 0U
#define ADC_AUDIOIF_Msk (0x3U << ADC_AUDIOIF_Pos)

/* AICR_ADC_2, Offset: 0x28 */
#define SB_AICR_ADC78_Pos 3U
#define SB_AICR_ADC78_Msk (0x1U << SB_AICR_ADC78_Pos)
#define SB_AICR_ADC56_Pos 2U
#define SB_AICR_ADC56_Msk (0x1U << SB_AICR_ADC56_Pos)
#define SB_AICR_ADC34_Pos 1U
#define SB_AICR_ADC34_Msk (0x1U << SB_AICR_ADC34_Pos)
#define SB_AICR_ADC12_Pos 0U
#define SB_AICR_ADC12_Msk (0x1U << SB_AICR_ADC12_Pos)

/* FCR_DAC, Offset: 0x2C */
#define DAC_FREQ_Pos 0U
#define DAC_FREQ_Msk (0xfU << DAC_FREQ_Pos)

/* CR_WNF, Offset: 0x38 */
#define ADC78_WNF_Pos 6U
#define ADC78_WNF_Msk (0x3U << ADC78_WNF_Pos)
#define ADC56_WNF_Pos 4U
#define ADC56_WNF_Msk (0x3U << ADC56_WNF_Pos)
#define ADC34_WNF_Pos 2U
#define ADC34_WNF_Msk (0x3U << ADC34_WNF_Pos)
#define ADC12_WNF_Pos 0U
#define ADC12_WNF_Msk (0x3U << ADC12_WNF_Pos)

/* FCR_ADC, Offset: 0x3C */
#define ADC78_HPF_EN_Pos 7U
#define ADC78_HPF_EN_Msk (0x1U << ADC78_HPF_EN_Pos)
#define ADC56_HPF_EN_Pos 6U
#define ADC56_HPF_EN_Msk (0x1U << ADC56_HPF_EN_Pos)
#define ADC34_HPF_EN_Pos 5U
#define ADC34_HPF_EN_Msk (0x1U << ADC34_HPF_EN_Pos)
#define ADC12_HPF_EN_Pos 4U
#define ADC12_HPF_EN_Msk (0x1U << ADC12_HPF_EN_Pos)
#define ADC_FREQ_Pos 0U
#define ADC_FREQ_Msk (0xfU << ADC_FREQ_Pos)

/* CR_DMIC12, Offset: 0x40 */
#define SB_DMIC1_Pos 7U
#define SB_DMIC1_Msk (0x1U << SB_DMIC1_Pos)
#define SB_DMIC2_Pos 6U
#define SB_DMIC2_Msk (0x1U << SB_DMIC2_Pos)
#define DMIC_RATE_Pos 4U
#define DMIC_RATE_Msk (0x3U << DMIC_RATE_Pos)
#define ADC_DMIC_SEL1_Pos 2U
#define ADC_DMIC_SEL1_Msk (0x3U << ADC_DMIC_SEL1_Pos)
#define ADC_DMIC_SEL2_Pos 0U
#define ADC_DMIC_SEL2_Msk (0x3U << ADC_DMIC_SEL2_Pos)

/* CR_DMIC34, Offset: 0x44 */
#define SB_DMIC3_Pos 7U
#define SB_DMIC3_Msk (0x1U << SB_DMIC3_Pos)
#define SB_DMIC4_Pos 6U
#define SB_DMIC4_Msk (0x1U << SB_DMIC4_Pos)
#define ADC_DMIC_SEL3_Pos 2U
#define ADC_DMIC_SEL3_Msk (0x3U << ADC_DMIC_SEL3_Pos)
#define ADC_DMIC_SEL4_Pos 0U
#define ADC_DMIC_SEL4_Msk (0x3U << ADC_DMIC_SEL4_Pos)

/* CR_DMIC56, Offset: 0x48 */
#define SB_DMIC5_Pos 7U
#define SB_DMIC5_Msk (0x1U << SB_DMIC5_Pos)
#define SB_DMIC6_Pos 6U
#define SB_DMIC6_Msk (0x1U << SB_DMIC6_Pos)
#define ADC_DMIC_SEL5_Pos 2U
#define ADC_DMIC_SEL5_Msk (0x3U << ADC_DMIC_SEL5_Pos)
#define ADC_DMIC_SEL6_Pos 0U
#define ADC_DMIC_SEL6_Msk (0x3U << ADC_DMIC_SEL6_Pos)

/* CR_DMIC78, Offset: 0x4C */
#define SB_DMIC7_Pos 7U
#define SB_DMIC7_Msk (0x1U << SB_DMIC7_Pos)
#define SB_DMIC8_Pos 6U
#define SB_DMIC8_Msk (0x1U << SB_DMIC8_Pos)
#define ADC_DMIC_SEL7_Pos 2U
#define ADC_DMIC_SEL7_Msk (0x3U << ADC_DMIC_SEL7_Pos)
#define ADC_DMIC_SEL8_Pos 0U
#define ADC_DMIC_SEL8_Msk (0x3U << ADC_DMIC_SEL8_Pos)

/* CR_HP, Offset: 0x50 */
#define HP_MUTE_Pos 7U
#define HP_MUTE_Msk (0x1U << HP_MUTE_Pos)
#define SB_HPCM_Pos 5U
#define SB_HPCM_Msk (0x1U << SB_HPCM_Pos)
#define SB_HP_Pos 4U
#define SB_HP_Msk (0x1U << SB_HP_Pos)

/* GCR_HPL, Offset: 0x54 */
#define LRGO_Pos 6U
#define LRGO_Msk (0x1U << LRGO_Pos)
#define GOL_Pos 0U
#define GOL_Msk (0xfU << GOL_Pos)

/* GCR_HPR, Offset: 0x58 */
#define GOR_Pos 0U
#define GOR_Msk (0x1fU << GOR_Pos)

/* CR_MIC1, Offset: 0x5C */
#define MICBIAS1_V_Pos 7U
#define MICBIAS1_V_Msk (0x1U << MICBIAS1_V_Pos)
#define MICDIFF1_Pos 6U
#define MICDIFF1_Msk (0x1U << MICDIFF1_Pos)
#define SB_MICBIAS1_Pos 5U
#define SB_MICBIAS1_Msk (0x1U << SB_MICBIAS1_Pos)

/* CR_MIC2, Offset: 0x60 */
#define MICBIAS2_V_Pos 7U
#define MICBIAS2_V_Msk (0x1U << MICBIAS2_V_Pos)
#define MICDIFF2_Pos 6U
#define MICDIFF2_Msk (0x1U << MICDIFF2_Pos)
#define SB_MICBIAS2_Pos 5U
#define SB_MICBIAS2_Msk (0x1U << SB_MICBIAS2_Pos)

/* CR_MIC3, Offset: 0x64 */
#define MICBIAS3_V_Pos 7U
#define MICBIAS3_V_Msk (0x1U << MICBIAS3_V_Pos)
#define MICDIFF3_Pos 6U
#define MICDIFF3_Msk (0x1U << MICDIFF3_Pos)
#define SB_MICBIAS3_Pos 5U
#define SB_MICBIAS3_Msk (0x1U << SB_MICBIAS3_Pos)

/* CR_MIC4, Offset: 0x68 */
#define MICBIAS4_V_Pos 7U
#define MICBIAS4_V_Msk (0x1U << MICBIAS4_V_Pos)
#define MICDIFF4_Pos 6U
#define MICDIFF4_Msk (0x1U << MICDIFF4_Pos)
#define SB_MICBIAS4_Pos 5U
#define SB_MICBIAS4_Msk (0x1U << SB_MICBIAS4_Pos)

/* CR_MIC5, Offset: 0x6C */
#define MICDIFF5_Pos 6U
#define MICDIFF5_Msk (0x1U << MICDIFF5_Pos)

/* CR_MIC6, Offset: 0x70 */
#define MICDIFF6_Pos 6U
#define MICDIFF6_Msk (0x1U << MICDIFF6_Pos)

/* CR_MIC7, Offset: 0x74 */
#define MICDIFF7_Pos 6U
#define MICDIFF7_Msk (0x1U << MICDIFF7_Pos)

/* CR_MIC8, Offset: 0x78 */
#define MICDIFF8_Pos 6U
#define MICDIFF8_Msk (0x1U << MICDIFF8_Pos)

/* GCR_MIC12, Offset: 0x7C */
#define GIM2_Pos 3U
#define GIM2_Msk (0x7U << GIM2_Pos)
#define GIM1_Pos 0U
#define GIM1_Msk (0x7U << GIM1_Pos)

/* GCR_MIC34, Offset: 0x80 */
#define GIM4_Pos 3U
#define GIM4_Msk (0x7U << GIM4_Pos)
#define GIM3_Pos 0U
#define GIM3_Msk (0x7U << GIM3_Pos)

/* GCR_MIC56, Offset: 0x84 */
#define GIM6_Pos 3U
#define GIM6_Msk (0x7U << GIM6_Pos)
#define GIM5_Pos 0U
#define GIM5_Msk (0x7U << GIM5_Pos)

/* GCR_MIC78, Offset: 0x88 */
#define GIM8_Pos 3U
#define GIM8_Msk (0x7U << GIM8_Pos)
#define GIM7_Pos 0U
#define GIM7_Msk (0x7U << GIM7_Pos)

/* CR_DAC, Offset: 0x8C */
#define DAC_SOFT_MUTE_Pos 7U
#define DAC_SOFT_MUTE_Msk (0x1U << DAC_SOFT_MUTE_Pos)
#define DAC_LEFT_ONLY_Pos 5U
#define DAC_LEFT_ONLY_Msk (0x1U << DAC_LEFT_ONLY_Pos)
#define SB_DAC_Pos 4U
#define SB_DAC_Msk (0x1U << SB_DAC_Pos)

/* CR_ADC12, Offset: 0x90 */
#define ADC12_SOFT_MUTE_Pos 7U
#define ADC12_SOFT_MUTE_Msk (0x1U << ADC12_SOFT_MUTE_Pos)
#define SB_ADC2_Pos 5U
#define SB_ADC2_Msk (0x1U << SB_ADC2_Pos)
#define SB_ADC1_Pos 4U
#define SB_ADC1_Msk (0x1U << SB_ADC1_Pos)

/* CR_ADC34, Offset: 0x94 */
#define ADC34_SOFT_MUTE_Pos 7U
#define ADC34_SOFT_MUTE_Msk (0x1U << ADC34_SOFT_MUTE_Pos)
#define SB_ADC4_Pos 5U
#define SB_ADC4_Msk (0x1U << SB_ADC4_Pos)
#define SB_ADC3_Pos 4U
#define SB_ADC3_Msk (0x1U << SB_ADC3_Pos)

/* CR_ADC56, Offset: 0x98 */
#define ADC56_SOFT_MUTE_Pos 7U
#define ADC56_SOFT_MUTE_Msk (0x1U << ADC56_SOFT_MUTE_Pos)
#define SB_ADC6_Pos 5U
#define SB_ADC6_Msk (0x1U << SB_ADC6_Pos)
#define SB_ADC5_Pos 4U
#define SB_ADC5_Msk (0x1U << SB_ADC5_Pos)

/* CR_ADC78, Offset: 0x9C */
#define ADC78_SOFT_MUTE_Pos 7U
#define ADC78_SOFT_MUTE_Msk (0x1U << ADC78_SOFT_MUTE_Pos)
#define SB_ADC8_Pos 5U
#define SB_ADC8_Msk (0x1U << SB_ADC8_Pos)
#define SB_ADC7_Pos 4U
#define SB_ADC7_Msk (0x1U << SB_ADC7_Pos)

/* CR_MIX, Offset: 0xA0 */
#define MIX_LOAD_Pos 6U
#define MIX_LOAD_Msk (0x1U << MIX_LOAD_Pos)
#define MIX_ADD_Pos 0U
#define MIX_ADD_Msk (0x3fU << MIX_ADD_Pos)

/* DR_MIX, Offset: 0xA4 */
#define MIX_DATA_Pos 0U
#define MIX_DATA_Msk (0xffU << MIX_DATA_Pos)

/* GCR_DACL, Offset: 0xA8 */
#define LRGOD_Pos 7U
#define LRGOD_Msk (0x1U << LRGOD_Pos)
#define GODL_Pos 0U
#define GODL_Msk (0x3fU << GODL_Pos)

/* GCR_DACR, Offset: 0xAC */
#define GODR_Pos 0U
#define GODR_Msk (0x3fU << GODR_Pos)

/* GCR_ADC1, Offset: 0xB0 */
#define LRGID1_Pos 7U
#define LRGID1_Msk (0x1U << LRGID1_Pos)
#define GID1_Pos 0U
#define GID1_Msk (0x3fU << GID1_Pos)

/* GCR_ADC2, Offset: 0xB4 */
#define GID2_Pos 0U
#define GID2_Msk (0x3fU << GID2_Pos)

/* GCR_ADC3, Offset: 0xB8 */
#define LRGID3_Pos 7U
#define LRGID3_Msk (0x1U << LRGID3_Pos)
#define GID3_Pos 0U
#define GID3_Msk (0x3fU << GID3_Pos)

/* GCR_ADC4, Offset: 0xBC */
#define GID4_Pos 0U
#define GID4_Msk (0x3fU << GID4_Pos)

/* GCR_ADC5, Offset: 0xC0 */
#define LRGID5_Pos 7U
#define LRGID5_Msk (0x1U << LRGID5_Pos)
#define GID5_Pos 0U
#define GID5_Msk (0x3fU << GID5_Pos)

/* GCR_ADC6, Offset: 0xC4 */
#define GID6_Pos 0U
#define GID6_Msk (0x3fU << GID6_Pos)

/* GCR_ADC7, Offset: 0xC8 */
#define LRGID7_Pos 7U
#define LRGID7_Msk (0x1U << LRGID7_Pos)
#define GID7_Pos 0U
#define GID7_Msk (0x3fU << GID7_Pos)

/* GCR_ADC8, Offset: 0xCC */
#define GID8_Pos 0U
#define GID8_Msk (0x3fU << GID8_Pos)

/* GCR_MIXDACL, Offset: 0xD0 */
#define LRGOMIX_Pos 7U
#define LRGOMIX_Msk (0x1U << LRGOMIX_Pos)
#define GOMIXL_Pos 0U
#define GOMIXL_Msk (0x1fU << GOMIXL_Pos)

/* GCR_MIXDACR, Offset: 0xD4 */
#define GOMIXR_Pos 0U
#define GOMIXR_Msk (0x1fU << GOMIXR_Pos)

/* GCR_MIXADCL, Offset: 0xD8 */
#define LRGIMIX_Pos 7U
#define LRGIMIX_Msk (0x1U << LRGIMIX_Pos)
#define GIMIXL_Pos 0U
#define GIMIXL_Msk (0x1fU << GIMIXL_Pos)

/* GCR_MIXADCR, Offset: 0xDC */
#define GIMIXR_Pos 0U
#define GIMIXR_Msk (0x1fU << GIMIXR_Pos)

/* CR_DAC_AGC, Offset: 0xE0 */

/* DR_DAC_AGC, Offset: 0xE4 */

/* CR_ADC_AGC, Offset: 0xE8 */
#define ADC_AGC_LOAD_Pos 6U
#define ADC_AGC_LOAD_Msk (0x1U << ADC_AGC_LOAD_Pos)
#define ADC_AGC_ADD_Pos 0U
#define ADC_AGC_ADD_Msk (0x3fU << ADC_AGC_ADD_Pos)

/* DR_ADC_AGC, Offset: 0xEC */
#define ADC_AGC_DATA_Pos 0U
#define ADC_AGC_DATA_Msk (0xffU << ADC_AGC_DATA_Pos)

/* CR_DAC_FI, Offset: 0xF0 */

/* DR_DAC_FI, Offset: 0xF4 */

/* SR_WT, Offset: 0x100 */
#define IRQ_WT_PENDING_Pos 6U
#define IRQ_WT_PENDING_Msk (0x1U << IRQ_WT_PENDING_Pos)

/* ICR_WT, Offset: 0x104 */
#define WT_INT_FORM_Pos 6U
#define WT_INT_FORM_Msk (0x3U << WT_INT_FORM_Pos)

/* IMR_WT, Offset: 0x108 */
#define WT_DTC_MASK_Pos 0U
#define WT_DTC_MASK_Msk (0x1U << WT_DTC_MASK_Pos)

/* IFR_WT, Offset: 0x10C */
#define WT_DTC_Pos 0U
#define WT_DTC_Msk (0x1U << WT_DTC_Pos)

/* CR_WT, Offset: 0x110 */
#define WT_EN_Pos 7U
#define WT_EN_Msk (0x1U << WT_EN_Pos)
#define WT_ONLY_Pos 6U
#define WT_ONLY_Msk (0x1U << WT_ONLY_Pos)
#define WT_RESTART_Pos 0U
#define WT_RESTART_Msk (0x1U << WT_RESTART_Pos)

/* CR_WT_2, Offset: 0x114 */
#define WT_ADC_SEL_Pos 7U
#define WT_ADC_SEL_Msk (0x1U << WT_ADC_SEL_Pos)
#define ZCD_EN_Pos 4U
#define ZCD_EN_Msk (0x1U << ZCD_EN_Pos)
#define WT_DMIC_FREQ_Pos 0U
#define WT_DMIC_FREQ_Msk (0x7U << WT_DMIC_FREQ_Pos)

/* CR_WT_3, Offset: 0x118 */
#define WT_POWER_SENS_Pos 0U
#define WT_POWER_SENS_Msk (0x1fU << WT_POWER_SENS_Pos)

/* CR_WT_4, Offset: 0x11C */

/* CR_TR, Offset: 0x180 */

/* DR_TR, Offset: 0x184 */

/* SR_TR1, Offset: 0x188 */

/* SR_TR_SRCDAC, Offset: 0x18C */

/* TRAN_SEL, Offset: 0x200 */
#define SI2C_Pos 0U
#define SI2C_Msk (0x1U << SI2C_Pos)

/* PARA_TX_FIFO, Offset: 0x204 */
#define TX_A_DATA_Pos 0U
#define TX_A_DATA_Msk (0xffffffffU << TX_A_DATA_Pos)

/* RX_FIFO1, Offset: 0x208 */
#define RX_A_DATA1_Pos 0U
#define RX_A_DATA1_Msk (0xffffU << RX_A_DATA1_Pos)

/* RX_FIFO2, Offset: 0x20C */
#define RX_A_DATA2_Pos 0U
#define RX_A_DATA2_Msk (0xffffU << RX_A_DATA2_Pos)

/* RX_FIFO3, Offset: 0x210 */
#define RX_A_DATA3_Pos 0U
#define RX_A_DATA3_Msk (0xffffU << RX_A_DATA3_Pos)

/* RX_FIFO4, Offset: 0x214 */
#define RX_A_DATA4_Pos 0U
#define RX_A_DATA4_Msk (0xffffU << RX_A_DATA4_Pos)

/* RX_FIFO5, Offset: 0x218 */
#define RX_A_DATA5_Pos 0U
#define RX_A_DATA5_Msk (0xffffU << RX_A_DATA5_Pos)

/* RX_FIFO6, Offset: 0x21C */

/* RX_FIFO7, Offset: 0x220 */
#define RX_A_DATA7_Pos 0U
#define RX_A_DATA7_Msk (0xffffU << RX_A_DATA7_Pos)

/* RX_FIFO8, Offset: 0x224 */
#define RX_A_DATA8_Pos 0U
#define RX_A_DATA8_Msk (0xffffU << RX_A_DATA8_Pos)

/* FIFO_TH_CTRL, Offset: 0x228 */
#define PARA_TX_TH_VALUE_Pos 4U
#define PARA_TX_TH_VALUE_Msk (0x7U << PARA_TX_TH_VALUE_Pos)
#define PARA_RX_TH_VALUE_Pos 0U
#define PARA_RX_TH_VALUE_Msk (0x7U << PARA_RX_TH_VALUE_Pos)

/* INTR_ERR_CTRL, Offset: 0x22C */
#define TX_ERROR_MASK_EN_Pos 26U
#define TX_ERROR_MASK_EN_Msk (0x1U << TX_ERROR_MASK_EN_Pos)
#define RX_ERROR_MASK_EN8_Pos 25U
#define RX_ERROR_MASK_EN8_Msk (0x1U << RX_ERROR_MASK_EN8_Pos)
#define RX_ERROR_MASK_EN7_Pos 24U
#define RX_ERROR_MASK_EN7_Msk (0x1U << RX_ERROR_MASK_EN7_Pos)
#define RX_ERROR_MASK_EN6_Pos 23U
#define RX_ERROR_MASK_EN6_Msk (0x1U << RX_ERROR_MASK_EN6_Pos)
#define RX_ERROR_MASK_EN5_Pos 22U
#define RX_ERROR_MASK_EN5_Msk (0x1U << RX_ERROR_MASK_EN5_Pos)
#define RX_ERROR_MASK_EN4_Pos 21U
#define RX_ERROR_MASK_EN4_Msk (0x1U << RX_ERROR_MASK_EN4_Pos)
#define RX_ERROR_MASK_EN3_Pos 20U
#define RX_ERROR_MASK_EN3_Msk (0x1U << RX_ERROR_MASK_EN3_Pos)
#define RX_ERROR_MASK_EN2_Pos 19U
#define RX_ERROR_MASK_EN2_Msk (0x1U << RX_ERROR_MASK_EN2_Pos)
#define RX_ERROR_MASK_EN1_Pos 18U
#define RX_ERROR_MASK_EN1_Msk (0x1U << RX_ERROR_MASK_EN1_Pos)
#define TX_TH_MASK_EN_Pos 17U
#define TX_TH_MASK_EN_Msk (0x1U << TX_TH_MASK_EN_Pos)
#define TX_EMPTY_MASK_EN_Pos 16U
#define TX_EMPTY_MASK_EN_Msk (0x1U << TX_EMPTY_MASK_EN_Pos)
#define RX_TH_MASK_EN8_Pos 15U
#define RX_TH_MASK_EN8_Msk (0x1U << RX_TH_MASK_EN8_Pos)
#define RX_TH_MASK_EN7_Pos 14U
#define RX_TH_MASK_EN7_Msk (0x1U << RX_TH_MASK_EN7_Pos)
#define RX_TH_MASK_EN6_Pos 13U
#define RX_TH_MASK_EN6_Msk (0x1U << RX_TH_MASK_EN6_Pos)
#define RX_TH_MASK_EN5_Pos 12U
#define RX_TH_MASK_EN5_Msk (0x1U << RX_TH_MASK_EN5_Pos)
#define RX_TH_MASK_EN4_Pos 11U
#define RX_TH_MASK_EN4_Msk (0x1U << RX_TH_MASK_EN4_Pos)
#define RX_TH_MASK_EN3_Pos 10U
#define RX_TH_MASK_EN3_Msk (0x1U << RX_TH_MASK_EN3_Pos)
#define RX_TH_MASK_EN2_Pos 9U
#define RX_TH_MASK_EN2_Msk (0x1U << RX_TH_MASK_EN2_Pos)
#define RX_TH_MASK_EN1_Pos 8U
#define RX_TH_MASK_EN1_Msk (0x1U << RX_TH_MASK_EN1_Pos)
#define RX_FULL_MASK_EN8_Pos 7U
#define RX_FULL_MASK_EN8_Msk (0x1U << RX_FULL_MASK_EN8_Pos)
#define RX_FULL_MASK_EN7_Pos 6U
#define RX_FULL_MASK_EN7_Msk (0x1U << RX_FULL_MASK_EN7_Pos)
#define RX_FULL_MASK_EN6_Pos 5U
#define RX_FULL_MASK_EN6_Msk (0x1U << RX_FULL_MASK_EN6_Pos)
#define RX_FULL_MASK_EN5_Pos 4U
#define RX_FULL_MASK_EN5_Msk (0x1U << RX_FULL_MASK_EN5_Pos)
#define RX_FULL_MASK_EN4_Pos 3U
#define RX_FULL_MASK_EN4_Msk (0x1U << RX_FULL_MASK_EN4_Pos)
#define RX_FULL_MASK_EN3_Pos 2U
#define RX_FULL_MASK_EN3_Msk (0x1U << RX_FULL_MASK_EN3_Pos)
#define RX_FULL_MASK_EN2_Pos 1U
#define RX_FULL_MASK_EN2_Msk (0x1U << RX_FULL_MASK_EN2_Pos)
#define RX_FULL_MASK_EN1_Pos 0U
#define RX_FULL_MASK_EN1_Msk (0x1U << RX_FULL_MASK_EN1_Pos)

/* INTR_ERR_STA, Offset: 0x230 */
#define TX_ERR_MASK_STA_Pos 26U
#define TX_ERR_MASK_STA_Msk (0x1U << TX_ERR_MASK_STA_Pos)
#define RX_ERR_MASK_STA8_Pos 25U
#define RX_ERR_MASK_STA8_Msk (0x1U << RX_ERR_MASK_STA8_Pos)
#define RX_ERR_MASK_STA7_Pos 24U
#define RX_ERR_MASK_STA7_Msk (0x1U << RX_ERR_MASK_STA7_Pos)
#define RX_ERR_MASK_STA6_Pos 23U
#define RX_ERR_MASK_STA6_Msk (0x1U << RX_ERR_MASK_STA6_Pos)
#define RX_ERR_MASK_STA5_Pos 22U
#define RX_ERR_MASK_STA5_Msk (0x1U << RX_ERR_MASK_STA5_Pos)
#define RX_ERR_MASK_STA4_Pos 21U
#define RX_ERR_MASK_STA4_Msk (0x1U << RX_ERR_MASK_STA4_Pos)
#define RX_ERR_MASK_STA3_Pos 20U
#define RX_ERR_MASK_STA3_Msk (0x1U << RX_ERR_MASK_STA3_Pos)
#define RX_ERR_MASK_STA2_Pos 19U
#define RX_ERR_MASK_STA2_Msk (0x1U << RX_ERR_MASK_STA2_Pos)
#define RX_ERR_MASK_STA1_Pos 18U
#define RX_ERR_MASK_STA1_Msk (0x1U << RX_ERR_MASK_STA1_Pos)
#define TX_TH_MASK_STA_Pos 17U
#define TX_TH_MASK_STA_Msk (0x1U << TX_TH_MASK_STA_Pos)
#define TX_EMPTY_MASK_STA_Pos 16U
#define TX_EMPTY_MASK_STA_Msk (0x1U << TX_EMPTY_MASK_STA_Pos)
#define RX_TH_MASK_STA8_Pos 15U
#define RX_TH_MASK_STA8_Msk (0x1U << RX_TH_MASK_STA8_Pos)
#define RX_TH_MASK_STA7_Pos 14U
#define RX_TH_MASK_STA7_Msk (0x1U << RX_TH_MASK_STA7_Pos)
#define RX_TH_MASK_STA6_Pos 13U
#define RX_TH_MASK_STA6_Msk (0x1U << RX_TH_MASK_STA6_Pos)
#define RX_TH_MASK_STA5_Pos 12U
#define RX_TH_MASK_STA5_Msk (0x1U << RX_TH_MASK_STA5_Pos)
#define RX_TH_MASK_STA4_Pos 11U
#define RX_TH_MASK_STA4_Msk (0x1U << RX_TH_MASK_STA4_Pos)
#define RX_TH_MASK_STA3_Pos 10U
#define RX_TH_MASK_STA3_Msk (0x1U << RX_TH_MASK_STA3_Pos)
#define RX_TH_MASK_STA2_Pos 9U
#define RX_TH_MASK_STA2_Msk (0x1U << RX_TH_MASK_STA2_Pos)
#define RX_TH_MASK_STA1_Pos 8U
#define RX_TH_MASK_STA1_Msk (0x1U << RX_TH_MASK_STA1_Pos)
#define RX_FULL_MASK_STA8_Pos 7U
#define RX_FULL_MASK_STA8_Msk (0x1U << RX_FULL_MASK_STA8_Pos)
#define RX_FULL_MASK_STA7_Pos 6U
#define RX_FULL_MASK_STA7_Msk (0x1U << RX_FULL_MASK_STA7_Pos)
#define RX_FULL_MASK_STA6_Pos 5U
#define RX_FULL_MASK_STA6_Msk (0x1U << RX_FULL_MASK_STA6_Pos)
#define RX_FULL_MASK_STA5_Pos 4U
#define RX_FULL_MASK_STA5_Msk (0x1U << RX_FULL_MASK_STA5_Pos)
#define RX_FULL_MASK_STA4_Pos 3U
#define RX_FULL_MASK_STA4_Msk (0x1U << RX_FULL_MASK_STA4_Pos)
#define RX_FULL_MASK_STA3_Pos 2U
#define RX_FULL_MASK_STA3_Msk (0x1U << RX_FULL_MASK_STA3_Pos)
#define RX_FULL_MASK_STA2_Pos 1U
#define RX_FULL_MASK_STA2_Msk (0x1U << RX_FULL_MASK_STA2_Pos)
#define RX_FULL_MASK_STA1_Pos 0U
#define RX_FULL_MASK_STA1_Msk (0x1U << RX_FULL_MASK_STA1_Pos)

/* PATH_EN, Offset: 0x234 */
#define PARA_DAC_EN_Pos 8U
#define PARA_DAC_EN_Msk (0x1U << PARA_DAC_EN_Pos)
#define PARA_ADC_EN8_Pos 7U
#define PARA_ADC_EN8_Msk (0x1U << PARA_ADC_EN8_Pos)
#define PARA_ADC_EN7_Pos 6U
#define PARA_ADC_EN7_Msk (0x1U << PARA_ADC_EN7_Pos)
#define PARA_ADC_EN6_Pos 5U
#define PARA_ADC_EN6_Msk (0x1U << PARA_ADC_EN6_Pos)
#define PARA_ADC_EN5_Pos 4U
#define PARA_ADC_EN5_Msk (0x1U << PARA_ADC_EN5_Pos)
#define PARA_ADC_EN4_Pos 3U
#define PARA_ADC_EN4_Msk (0x1U << PARA_ADC_EN4_Pos)
#define PARA_ADC_EN3_Pos 2U
#define PARA_ADC_EN3_Msk (0x1U << PARA_ADC_EN3_Pos)
#define PARA_ADC_EN2_Pos 1U
#define PARA_ADC_EN2_Msk (0x1U << PARA_ADC_EN2_Pos)
#define PARA_ADC_EN1_Pos 0U
#define PARA_ADC_EN1_Msk (0x1U << PARA_ADC_EN1_Pos)

/* INTR_ERR_CLR, Offset: 0x238 */
#define TX_ERR_CLR_Pos 26U
#define TX_ERR_CLR_Msk (0x1U << TX_ERR_CLR_Pos)
#define RX_ERR_CLR8_Pos 25U
#define RX_ERR_CLR8_Msk (0x1U << RX_ERR_CLR8_Pos)
#define RX_ERR_CLR7_Pos 24U
#define RX_ERR_CLR7_Msk (0x1U << RX_ERR_CLR7_Pos)
#define RX_ERR_CLR6_Pos 23U
#define RX_ERR_CLR6_Msk (0x1U << RX_ERR_CLR6_Pos)
#define RX_ERR_CLR5_Pos 22U
#define RX_ERR_CLR5_Msk (0x1U << RX_ERR_CLR5_Pos)
#define RX_ERR_CLR4_Pos 21U
#define RX_ERR_CLR4_Msk (0x1U << RX_ERR_CLR4_Pos)
#define RX_ERR_CLR3_Pos 20U
#define RX_ERR_CLR3_Msk (0x1U << RX_ERR_CLR3_Pos)
#define RX_ERR_CLR2_Pos 19U
#define RX_ERR_CLR2_Msk (0x1U << RX_ERR_CLR2_Pos)
#define RX_ERR_CLR1_Pos 18U
#define RX_ERR_CLR1_Msk (0x1U << RX_ERR_CLR1_Pos)
#define PARA_TX_TH_CLR_Pos 17U
#define PARA_TX_TH_CLR_Msk (0x1U << PARA_TX_TH_CLR_Pos)
#define PARA_TX_EMPTY_CLR_Pos 16U
#define PARA_TX_EMPTY_CLR_Msk (0x1U << PARA_TX_EMPTY_CLR_Pos)
#define RX_TH_CLR8_Pos 15U
#define RX_TH_CLR8_Msk (0x1U << RX_TH_CLR8_Pos)
#define RX_TH_CLR7_Pos 14U
#define RX_TH_CLR7_Msk (0x1U << RX_TH_CLR7_Pos)
#define RX_TH_CLR6_Pos 13U
#define RX_TH_CLR6_Msk (0x1U << RX_TH_CLR6_Pos)
#define RX_TH_CLR5_Pos 12U
#define RX_TH_CLR5_Msk (0x1U << RX_TH_CLR5_Pos)
#define RX_TH_CLR4_Pos 11U
#define RX_TH_CLR4_Msk (0x1U << RX_TH_CLR4_Pos)
#define RX_TH_CLR3_Pos 10U
#define RX_TH_CLR3_Msk (0x1U << RX_TH_CLR3_Pos)
#define RX_TH_CLR2_Pos 9U
#define RX_TH_CLR2_Msk (0x1U << RX_TH_CLR2_Pos)
#define RX_TH_CLR1_Pos 8U
#define RX_TH_CLR1_Msk (0x1U << RX_TH_CLR1_Pos)
#define RX_FULL_CLR8_Pos 7U
#define RX_FULL_CLR8_Msk (0x1U << RX_FULL_CLR8_Pos)
#define RX_FULL_CLR7_Pos 6U
#define RX_FULL_CLR7_Msk (0x1U << RX_FULL_CLR7_Pos)
#define RX_FULL_CLR6_Pos 5U
#define RX_FULL_CLR6_Msk (0x1U << RX_FULL_CLR6_Pos)
#define RX_FULL_CLR5_Pos 4U
#define RX_FULL_CLR5_Msk (0x1U << RX_FULL_CLR5_Pos)
#define RX_FULL_CLR4_Pos 3U
#define RX_FULL_CLR4_Msk (0x1U << RX_FULL_CLR4_Pos)
#define RX_FULL_CLR3_Pos 2U
#define RX_FULL_CLR3_Msk (0x1U << RX_FULL_CLR3_Pos)
#define RX_FULL_CLR2_Pos 1U
#define RX_FULL_CLR2_Msk (0x1U << RX_FULL_CLR2_Pos)
#define RX_FULL_CLR1_Pos 0U
#define RX_FULL_CLR1_Msk (0x1U << RX_FULL_CLR1_Pos)

/* COD_MASTER, Offset: 0x23C */
#define DAC_MASTER_Pos 0U
#define DAC_MASTER_Msk (0x1U << DAC_MASTER_Pos)

/* I2C_ADDR, Offset: 0x240 */
#define I2C_ADDR_Pos 0U
#define I2C_ADDR_Msk (0x7fU << I2C_ADDR_Pos)

/* TRAN_CTRL, Offset: 0x244 */
#define DMA_REG_EN_Pos 0U
#define DMA_REG_EN_Msk (0x1U << DMA_REG_EN_Pos)

/* SAMPLING_CTRL, Offset: 0x248 */
#define RX_BITS_Pos 1U
#define RX_BITS_Msk (0x1U << RX_BITS_Pos)
#define TX_BITS_Pos 0U
#define TX_BITS_Msk (0x1U << TX_BITS_Pos)

/* TX_FIFO_CNT, Offset: 0x24C */
#define TX_FIFO_CNT_Pos 0U
#define TX_FIFO_CNT_Msk (0xfU << TX_FIFO_CNT_Pos)

/* RX_FIFO_CNT_1, Offset: 0x250 */
#define RX_FIFO_CNT1_Pos 0U
#define RX_FIFO_CNT1_Msk (0xfU << RX_FIFO_CNT1_Pos)

/* RX_FIFO_CNT_2, Offset: 0x254 */
#define RX_FIFO_CNT2_Pos 0U
#define RX_FIFO_CNT2_Msk (0xfU << RX_FIFO_CNT2_Pos)

/* RX_FIFO_CNT_3, Offset: 0x258 */
#define RX_FIFO_CNT3_Pos 0U
#define RX_FIFO_CNT3_Msk (0xfU << RX_FIFO_CNT3_Pos)

/* RX_FIFO_CNT_4, Offset: 0x25C */
#define RX_FIFO_CNT4_Pos 0U
#define RX_FIFO_CNT4_Msk (0xfU << RX_FIFO_CNT4_Pos)

/* RX_FIFO_CNT_5, Offset: 0x260 */
#define RX_FIFO_CNT5_Pos 0U
#define RX_FIFO_CNT5_Msk (0xfU << RX_FIFO_CNT5_Pos)

/* RX_FIFO_CNT_6, Offset: 0x264 */
#define RX_FIFO_CNT6_Pos 0U
#define RX_FIFO_CNT6_Msk (0xfU << RX_FIFO_CNT6_Pos)

/* RX_FIFO_CNT_7, Offset: 0x268 */
#define RX_FIFO_CNT7_Pos 0U
#define RX_FIFO_CNT7_Msk (0xfU << RX_FIFO_CNT7_Pos)

/* RX_FIFO_CNT_8, Offset: 0x26C */
#define RX_FIFO_CNT8_Pos 0U
#define RX_FIFO_CNT8_Msk (0xfU << RX_FIFO_CNT8_Pos)

typedef struct {
    __IM  uint32_t SR                            ; /* Offset: 0x00      (R)    SR                  Register */
    __IM  uint32_t SR2                           ; /* Offset: 0x04      (R)    SR2                 Register */
    __IM  uint32_t SR3                           ; /* Offset: 0x08      (R)    SR3                 Register */
    __IOM uint32_t ICR                           ; /* Offset: 0x0C      (R/W)  ICR                 Register */
    __IOM uint32_t IMR                           ; /* Offset: 0x10      (R/W)  IMR                 Register */
    __IOM uint32_t IFR                           ; /* Offset: 0x14      (R/W)  IFR                 Register */
    __IOM uint32_t CR_VIC                        ; /* Offset: 0x18      (R/W)  CR_VIC              Register */
    __IOM uint32_t CR_CK                         ; /* Offset: 0x1C      (R/W)  CR_CK               Register */
    __IOM uint32_t AICR_DAC                      ; /* Offset: 0x20      (R/W)  AICR_DAC            Register */
    __IOM uint32_t AICR_ADC                      ; /* Offset: 0x24      (R/W)  AICR_ADC            Register */
    __IOM uint32_t AICR_ADC_2                    ; /* Offset: 0x28      (R/W)  AICR_ADC_2          Register */
    __IOM uint32_t FCR_DAC                       ; /* Offset: 0x2C      (R/W)  FCR_DAC             Register */
    uint32_t RESERVED0[2]                  ; /* Offset: 0X30      (R)     RESERVED */
    __IOM uint32_t CR_WNF                        ; /* Offset: 0x38      (R/W)  CR_WNF              Register */
    __IOM uint32_t FCR_ADC                       ; /* Offset: 0x3C      (R/W)  FCR_ADC             Register */
    __IOM uint32_t CR_DMIC12                     ; /* Offset: 0x40      (R/W)  CR_DMIC12           Register */
    __IOM uint32_t CR_DMIC34                     ; /* Offset: 0x44      (R/W)  CR_DMIC34           Register */
    __IOM uint32_t CR_DMIC56                     ; /* Offset: 0x48      (R/W)  CR_DMIC56           Register */
    __IOM uint32_t CR_DMIC78                     ; /* Offset: 0x4C      (R/W)  CR_DMIC78           Register */
    __IOM uint32_t CR_HP                         ; /* Offset: 0x50      (R/W)  CR_HP               Register */
    __IOM uint32_t GCR_HPL                       ; /* Offset: 0x54      (R/W)  GCR_HPL             Register */
    __IOM uint32_t GCR_HPR                       ; /* Offset: 0x58      (R/W)  GCR_HPR             Register */
    __IOM uint32_t CR_MIC[8];
    __IOM uint32_t GCR_MIC[4];
    __IOM uint32_t CR_DAC                        ; /* Offset: 0x8C      (R/W)  CR_DAC              Register */
    __IOM uint32_t CR_ADC[4];
    __IOM uint32_t CR_MIX                        ; /* Offset: 0xA0      (R/W)  CR_MIX              Register */
    __IOM uint32_t DR_MIX                        ; /* Offset: 0xA4      (R/W)  DR_MIX              Register */
    __IOM uint32_t GCR_DACL                      ; /* Offset: 0xA8      (R/W)  GCR_DACL            Register */
    __IOM uint32_t GCR_DACR                      ; /* Offset: 0xAC      (R/W)  GCR_DACR            Register */
    __IOM uint32_t GCR_ADC[8];
    __IOM uint32_t GCR_MIXDACL                   ; /* Offset: 0xD0      (R/W)  GCR_MIXDACL         Register */
    __IOM uint32_t GCR_MIXDACR                   ; /* Offset: 0xD4      (R/W)  GCR_MIXDACR         Register */
    __IOM uint32_t GCR_MIXADCL                   ; /* Offset: 0xD8      (R/W)  GCR_MIXADCL         Register */
    __IOM uint32_t GCR_MIXADCR                   ; /* Offset: 0xDC      (R/W)  GCR_MIXADCR         Register */
    __IOM uint32_t CR_DAC_AGC                    ; /* Offset: 0xE0      (R/W)  CR_DAC_AGC          Register */
    __IOM uint32_t DR_DAC_AGC                    ; /* Offset: 0xE4      (R/W)  DR_DAC_AGC          Register */
    __IOM uint32_t CR_ADC_AGC                    ; /* Offset: 0xE8      (R/W)  CR_ADC_AGC          Register */
    __IOM uint32_t DR_ADC_AGC                    ; /* Offset: 0xEC      (R/W)  DR_ADC_AGC          Register */
    __IOM uint32_t CR_DAC_FI                     ; /* Offset: 0xF0      (R/W)  CR_DAC_FI           Register */
    __IOM uint32_t DR_DAC_FI                     ; /* Offset: 0xF4      (R/W)  DR_DAC_FI           Register */
    uint32_t RESERVED1[2]                  ; /* Offset: 0XF8      (R)     RESERVED */
    __IOM uint32_t SR_WT                         ; /* Offset: 0x100     (R/W)  SR_WT               Register */
    __IOM uint32_t ICR_WT                        ; /* Offset: 0x104     (R/W)  ICR_WT              Register */
    __IOM uint32_t IMR_WT                        ; /* Offset: 0x108     (R/W)  IMR_WT              Register */
    __IOM uint32_t IFR_WT                        ; /* Offset: 0x10C     (R/W)  IFR_WT              Register */
    __IOM uint32_t CR_WT                         ; /* Offset: 0x110     (R/W)  CR_WT               Register */
    __IOM uint32_t CR_WT_2                       ; /* Offset: 0x114     (R/W)  CR_WT_2             Register */
    __IOM uint32_t CR_WT_3                       ; /* Offset: 0x118     (R/W)  CR_WT_3             Register */
    __IOM uint32_t CR_WT_4                       ; /* Offset: 0x11C     (R/W)  CR_WT_4             Register */
    uint32_t RESERVED2[24]                 ; /* Offset: 0X120     (R)     RESERVED */
    __IOM uint32_t CR_TR                         ; /* Offset: 0x180     (R/W)  CR_TR               Register */
    __IOM uint32_t DR_TR                         ; /* Offset: 0x184     (R/W)  DR_TR               Register */
    __IOM uint32_t SR_TR1                        ; /* Offset: 0x188     (R/W)  SR_TR1              Register */
    __IOM uint32_t SR_TR_SRCDAC                  ; /* Offset: 0x18C     (R/W)  SR_TR_SRCDAC        Register */
    uint32_t RESERVED3[28]                 ; /* Offset: 0X190     (R)     RESERVED */
    __IOM uint32_t TRAN_SEL                      ; /* Offset: 0x200     (R/W)  TRAN_SEL            Register */
    __OM  uint32_t PARA_TX_FIFO                  ; /* Offset: 0x204     (W)    PARA_TX_FIFO        Register */
    __IM  uint32_t RX_FIFO[8];
    __IOM uint32_t FIFO_TH_CTRL                  ; /* Offset: 0x228     (R/W)  FIFO_TH_CTRL        Register */
    __IOM uint32_t INTR_ERR_CTRL                 ; /* Offset: 0x22C     (R/W)  INTR_ERR_CTRL       Register */
    __IM  uint32_t INTR_ERR_STA                  ; /* Offset: 0x230     (R)    INTR_ERR_STA        Register */
    __IOM uint32_t PATH_EN                       ; /* Offset: 0x234     (R/W)  PATH_EN             Register */
    __OM  uint32_t INTR_ERR_CLR                  ; /* Offset: 0x238     (W)    INTR_ERR_CLR        Register */
    __IM  uint32_t COD_MASTER                    ; /* Offset: 0x23C     (R)    COD_MASTER          Register */
    __IOM uint32_t I2C_ADDR                      ; /* Offset: 0x240     (R/W)  I2C_ADDR            Register */
    __IOM uint32_t TRAN_CTRL                     ; /* Offset: 0x244     (R/W)  TRAN_CTRL           Register */
    __IOM uint32_t SAMPLING_CTRL                 ; /* Offset: 0x248     (R/W)  SAMPLING_CTRL       Register */
    __IM  uint32_t TX_FIFO_CNT                   ; /* Offset: 0x24C     (R)    TX_FIFO_CNT         Register */
    __IM  uint32_t RX_FIFO_CNT[8];
} ck_codec_reg_t;

/*****************************************************************/
typedef enum {
    CK_CODEC_MCLK_12M,
    CK_CODEC_MCLK_13M = 2,
} ck_codec_mclk_freq_t;


#define CK_CODEC_ADC_CHANNEL_NUM 8
#define CK_CODEC_DAC_CHANNEL_NUM 1

typedef struct {
    int idx;
    int codec_idx;
    codec_event_cb_t cb;
    void *cb_arg;
    volatile uint32_t priv_sta;
    csi_dma_ch_t dma;
    ringbuffer_t fifo;
    uint32_t period;
    int32_t mv_size;
    uint32_t bit_width;
} ck_codec_ch_priv_t;


/*****codec priv sta*****/
#define CODEC_INIT_MASK             (1 << 0)
#define CODEC_READT_TO_START_MASK   (1 << 1)
#define CODEC_RUNING_MASK           (1 << 2)
#define CODEC_PAUSE_MASK            (1 << 3)

typedef enum {
    CK_CODEC_MIC_BIAS_1500MV,
    CK_CODEC_MIC_BIAS_1800MV,
} ck_codec_mic_bias_t;

typedef enum {
    CK_CODEC_ADC_SINGLE_MODE,
    CK_CODEC_ADC_DIFFERENTIAL_MODE,
} ck_codec_adc_mode_t;

void ck_codec_tx_feed(csi_dev_t *priv);
void ck_codec_sb_power_ctrl(csi_dev_t *priv, int en);
void ck_codec_inerrupt_ctrl(csi_dev_t *priv, uint32_t mask, int en);
void ck_codec_inerrupt_disable_all(csi_dev_t *priv);
void ck_codec_mclk_freq_select(csi_dev_t *priv, ck_codec_mclk_freq_t freq);
void ck_codec_adc_bit_width_and_mode_set(csi_dev_t *priv);
void ck_codec_tx_threshold_val(csi_dev_t *priv, uint32_t val);
void ck_codec_rx_threshold_val(csi_dev_t *priv, uint32_t val);
void ck_codec_adc_channel_enable(csi_dev_t *priv, int ad_idx, int en);
void ck_codec_adc_sample_rate_config(csi_dev_t *priv, uint32_t rate);
void ck_codec_adc_wnf_ctrl(csi_dev_t *priv, int ad_idx, int mode);
void ck_codec_mic_set_bias(csi_dev_t *priv, int ad_idx, ck_codec_mic_bias_t bias);
void ck_codec_disable_dmic(csi_dev_t *priv);
void ck_codec_adc_mixer_set_gain(csi_dev_t *priv, uint32_t gain);
uint32_t ck_codec_adc_mixer_get_gain(csi_dev_t *priv);
void ck_codec_adc_mode_set(csi_dev_t *priv, int ad_idx, ck_codec_adc_mode_t mode);
void ck_codec_mic_bias_en(csi_dev_t *priv, int ad_idx, int en);
void ck_codec_adc_sb_enable(csi_dev_t *priv, int ad_idx, int en);
void ck_codec_adc_mute_en(csi_dev_t *priv, int ad_idx, int en);
void ck_codec_path_adc_en(csi_dev_t *priv, int adc_idx, int en);
void ck_codec_adc_set_boost_gain(csi_dev_t *priv, int32_t adc_idx, uint32_t gain);
uint32_t ck_codec_adc_get_boost_gain(csi_dev_t *priv, int32_t adc_idx);
void ck_codec_adc_set_digital_gain(csi_dev_t *priv, int32_t adc_idx, uint32_t gain);
uint32_t ck_codec_adc_get_digital_gain(csi_dev_t *priv, int32_t adc_idx);
void ck_codec_dac_set_analog_left_gan(csi_dev_t *priv, uint32_t val);
uint32_t ck_codec_dac_get_analog_left_gan(csi_dev_t *priv);
void ck_codec_dac_set_analog_right_gan(csi_dev_t *priv, uint32_t val);
uint32_t ck_codec_dac_get_analog_right_gan(csi_dev_t *priv);
uint32_t ck_codec_tx_fifo_cnt(csi_dev_t *priv);
void ck_codec_dac_bit_width_and_mode_set(csi_dev_t *priv, uint32_t bit_width);
void ck_codec_dac_power_down_ctrl(csi_dev_t *priv, int en);
void ck_codec_dac_sample_rate_config(csi_dev_t *priv, uint32_t rate);
void ck_codec_dac_mute_en(csi_dev_t *priv, int32_t en);
void ck_codec_dac_mono_enable(csi_dev_t *priv, int32_t en);
void ck_codec_dac_sb_en(csi_dev_t *priv, int32_t en);
void ck_codec_dac_set_mix_left_gain(csi_dev_t *priv, uint32_t val);
uint32_t ck_codec_dac_get_mix_left_gain(csi_dev_t *priv);
void ck_codec_dac_set_mix_right_gain(csi_dev_t *priv, uint32_t val);
uint32_t ck_codec_dac_get_mix_right_gain(csi_dev_t *priv);
void ck_codec_dac_set_digital_left_gan(csi_dev_t *priv, uint32_t val);
uint32_t ck_codec_dac_get_digital_left_gan(csi_dev_t *priv);
void ck_codec_dac_set_digital_right_gan(csi_dev_t *priv, uint32_t val);
uint32_t ck_codec_dac_get_digital_right_gan(csi_dev_t *priv);

void ck_codec_path_dac_en(csi_dev_t *priv, int en);
void ck_codec_dac_headphone_mute(csi_dev_t *priv, int en);
void ck_codec_dac_headphone_sb_enable(csi_dev_t *priv, int en);
void ck_codec_dac_headphone_set_right_gain(csi_dev_t *priv, uint32_t val);
void ck_codec_dac_headphone_set_left_gain(csi_dev_t *priv, uint32_t val);
void ck_codec_dac_headphone_get_right_gain(csi_dev_t *priv, uint32_t *val);
void ck_codec_dac_headphone_get_left_gain(csi_dev_t *priv, uint32_t *val);
void ck_codec_gasket_reset();
void ck_codec_gasket_error_irqhandler(int32_t idx);

/*********dma**********/
int32_t ck_codec_input_dma_config(ck_codec_ch_priv_t *input_priv);
int32_t ck_codec_dma_input_start(ck_codec_ch_priv_t *input_priv, ck_codec_reg_t *addr);
void ck_codec_input_dma_stop(ck_codec_ch_priv_t *input_priv);
int32_t ck_codec_output_dma_config(ck_codec_ch_priv_t *output_priv);
int32_t ck_codec_output_dma_start(int idx);
void ck_codec_output_dma_stop(int idx);
void ck_codec_dma_ch_release(int ch);
void ck_codec_dma_all_restart(void);

void ck_codec_lpm(uint32_t idx, uint32_t state);

#ifdef __cplusplus
}
#endif

#endif /* _CK_CODEC_H_ */
