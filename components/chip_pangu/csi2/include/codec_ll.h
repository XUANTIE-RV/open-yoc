/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     codec_ll.h
 * @brief    header file for wj codec ll driver
 * @version  V1.0
 * @date     13. July 2020
 ******************************************************************************/

#ifndef __CODEC_LL_H__
#define __CODEC_LL_H__

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * CODEC register bit definitions
 */
/* Offset: 0x00      (R)    SR                  Register */
#define SR_REG_PON_ACK_Pos                       (7U)
#define SR_REG_PON_ACK_Msk                       (0x1U << SR_REG_PON_ACK_Pos)
#define SR_REG_PON_ACK                           SR_REG_PON_ACK_Msk

#define SR_REG_IRQ_PENDING_Pos                   (6U)
#define SR_REG_IRQ_PENDING_Msk                   (0x1U << SR_REG_IRQ_PENDING_Pos)
#define SR_REG_IRQ_PENDING                       SR_REG_IRQ_PENDING_Msk

#define SR_REG_JACK_Pos                          (5U)
#define SR_REG_JACK_Msk                          (0x1U << SR_REG_JACK_Pos)
#define SR_REG_JACK                              SR_REG_JACK_Msk

#define SR_REG_DAC_LOCKED_Pos                    (4U)
#define SR_REG_DAC_LOCKED_Msk                    (0x1U << SR_REG_DAC_LOCKED_Pos)
#define SR_REG_DAC_LOCKED                        SR_REG_DAC_LOCKED_Msk

#define SR_REG_DAC_UNKNOW_FS_Pos                 (2U)
#define SR_REG_DAC_UNKNOW_FS_Msk                 (0x1U << SR_REG_DAC_UNKNOW_FS_Pos)
#define SR_REG_DAC_UNKNOW_FS                     SR_REG_DAC_UNKNOW_FS_Msk

#define SR_REG_DAC_RAMP_IN_PROGRESS_Pos          (0U)
#define SR_REG_DAC_RAMP_IN_PROGRESS_Msk          (0x1U << SR_REG_DAC_RAMP_IN_PROGRESS_Pos)
#define SR_REG_DAC_RAMP_IN_PROGRESS              SR_REG_DAC_RAMP_IN_PROGRESS_Msk
/* Offset: 0x04      (R)    SR2                 Register */
#define SR2_REG_DAC_MUTE_INPROGRESS_Pos          (7U)
#define SR2_REG_DAC_MUTE_INPROGRESS_Msk          (0x1U << SR2_REG_DAC_MUTE_INPROGRESS_Pos)
#define SR2_REG_DAC_MUTE_INPROGRESS              SR2_REG_DAC_MUTE_INPROGRESS_Msk

#define SR2_REG_ADC12_MUTE_INPROGRESS_Pos        (6U)
#define SR2_REG_ADC12_MUTE_INPROGRESS_Msk        (0x1U << SR2_REG_ADC12_MUTE_INPROGRESS_Pos)
#define SR2_REG_ADC12_MUTE_INPROGRESS            SR2_REG_ADC12_MUTE_INPROGRESS_Msk

/* Offset: 0x08      (R)    SR3                 Register */
#define SR3_REG_ADC34_MUTE_INPROGRESS_Pos        (0U)
#define SR3_REG_ADC34_MUTE_INPROGRESS_Msk        (0x1U << SR3_REG_ADC34_MUTE_INPROGRESS_Pos)
#define SR3_REG_ADC34_MUTE_INPROGRESS            SR3_REG_ADC34_MUTE_INPROGRESS_Msk

#define SR3_REG_ADC56_MUTE_INPROGRESS_Pos        (1U)
#define SR3_REG_ADC56_MUTE_INPROGRESS_Msk        (0x1U << SR3_REG_ADC56_MUTE_INPROGRESS_Pos)
#define SR3_REG_ADC56_MUTE_INPROGRESS            SR3_REG_ADC56_MUTE_INPROGRESS_Msk

#define SR3_REG_ADC78_MUTE_INPROGRESS_Pos        (2U)
#define SR3_REG_ADC78_MUTE_INPROGRESS_Msk        (0x1U << SR3_REG_ADC78_MUTE_INPROGRESS_Pos)
#define SR3_REG_ADC78_MUTE_INPROGRESS            SR3_REG_ADC78_MUTE_INPROGRESS_Msk
/* Offset: 0x0C      (R/W)  ICR                 Register */
#define ICR_REG_INT_FORM_Pos            (6U)
#define ICR_REG_INT_FORM_Msk            (0x3U << ICR_REG_INT_FORM_Pos)
#define ICR_REG_INT_FORM                ICR_REG_INT_FORM_Msk
#define ICR_REG_INT_FORM_IRQ_HIGH       (0U<<ICR_REG_INT_FORM_Pos)
#define ICR_REG_INT_FORM_IRQ_LOW        (1U<<ICR_REG_INT_FORM_Pos)
#define ICR_REG_INT_FORM_8MCCLK_HIGH    (2U<<ICR_REG_INT_FORM_Pos)
#define ICR_REG_INT_FORM_8MCCLK_LOW     (3U<<ICR_REG_INT_FORM_Pos)

/* Offset: 0x10      (R/W)  IMR                 Register */
#define IMR_REG_SC_MASK_Pos             (4U)
#define IMR_REG_SC_MASK_Msk             (0x1U << IMR_REG_SC_MASK_Pos)
#define IMR_REG_SC_MASK_EN               IMR_REG_SC_MASK_Msk

#define IMR_REG_JACK_MASK_Pos            (5U)
#define IMR_REG_JACK_MASK_Msk            (0x1U << IMR_REG_JACK_MASK_Pos)
#define IMR_REG_JACK_MASK_EN             IMR_REG_JACK_MASK_Msk

#define IMR_REG_ADAS_UNLOCK_MASK_Pos     (7U)
#define IMR_REG_ADAS_UNLOCK_MASK_Msk     (0x1U << IMR_REG_ADAS_UNLOCK_MASK_Pos)
#define IMR_REG_ADAS_UNLOCK_MASK_EN       IMR_REG_ADAS_UNLOCK_MASK_Msk
/* Offset: 0x14      (R/W)  IFR                 Register */
#define IFR_REG_SC_Pos                   (4U)
#define IFR_REG_SC_Msk                   (0x1U << IFR_REG_SC_Pos)
#define IFR_REG_SC_EN                       IFR_REG_SC_Msk

#define IFR_REG_JACK_EVENT_Pos            (5U)
#define IFR_REG_JACK_EVENT_Msk            (0x1U << IFR_REG_JACK_EVENT_Pos)
#define IFR_REG_JACK_EVENT_EN                IFR_REG_JACK_EVENT_Msk

#define IFR_REG_ADAS_UNLOCK_Pos          (7U)
#define IFR_REG_ADAS_UNLOCK_Msk          (0x1U << IFR_REG_ADAS_UNLOCK_Pos)
#define IFR_REG_ADAS_UNLOCK_EN               IFR_REG_ADAS_UNLOCK_Msk
/* Offset: 0x18      (R/W)  CR_VIC              Register */
#define CR_VIC_REG_SLEEP_ANALOG_Pos      (2U)
#define CR_VIC_REG_SLEEP_ANALOG_Msk      (0x1U << CR_VIC_REG_SLEEP_ANALOG_Pos)
#define CR_VIC_REG_SLEEP_ANALOG_EN           CR_VIC_REG_SLEEP_ANALOG_Msk

#define CR_VIC_REG_SB_ANALOG_Pos        (1U)
#define CR_VIC_REG_SB_ANALOG_Msk        (0x1U << CR_VIC_REG_SB_ANALOG_Pos)
#define CR_VIC_REG_SB_ANALOG_EN             CR_VIC_REG_SB_ANALOG_Msk

#define CR_VIC_REG_SB_Pos               (0U)
#define CR_VIC_REG_SB_Msk               (0x1U << CR_VIC_REG_SB_Pos)
#define CR_VIC_REG_SB_EN                   CR_VIC_REG_SB_Msk
/* Offset: 0x1C      (R/W)  CR_CK               Register */
#define CR_CK_REG_CRYSTAL_Pos               (0U)
#define CR_CK_REG_CRYSTAL_Msk               (0x3U << CR_CK_REG_CRYSTAL_Pos)
#define CR_CK_REG_CRYSTAL                   CR_CK_REG_CRYSTAL_Msk
#define CR_CK_REG_CRYSTAL_12MHZ              (0x0U << CR_CK_REG_CRYSTAL_Pos)
#define CR_CK_REG_CRYSTAL_13MHZ              (0x1U << CR_CK_REG_CRYSTAL_Pos)

/* Offset: 0x20      (R/W)  AICR_DAC            Register */
#define AICR_DAC_REG_DAC_AUDIOIF_Pos              (0U)
#define AICR_DAC_REG_DAC_AUDIOIF_Msk              (0x3U << AICR_DAC_REG_DAC_AUDIOIF_Pos)
#define AICR_DAC_REG_DAC_AUDIOIF_PARALLEL         (0x0U << CR_CK_REG_CRYSTAL_Pos)
#define AICR_DAC_REG_DAC_AUDIOIF_ALIGN_LEFT       (0x1U << CR_CK_REG_CRYSTAL_Pos)
#define AICR_DAC_REG_DAC_AUDIOIF_DSP              (0x2U << CR_CK_REG_CRYSTAL_Pos)
#define AICR_DAC_REG_DAC_AUDIOIF_I2S              (0x3U << CR_CK_REG_CRYSTAL_Pos)

#define AICR_DAC_REG_SB_AICR_DAC_Pos              (4U)
#define AICR_DAC_REG_SB_AICR_DAC_Msk              (0x1U << AICR_DAC_REG_SB_AICR_DAC_Pos)
#define AICR_DAC_REG_SB_AICR_DAC_EN               AICR_DAC_REG_SB_AICR_DAC_Msk

#define AICR_DAC_REG_DAC_SLAVE_Pos              (5U)
#define AICR_DAC_REG_DAC_SLAVE_Msk              (0x1U << AICR_DAC_REG_DAC_SLAVE_Pos)
#define AICR_DAC_REG_DAC_SLAVE_EN               AICR_DAC_REG_DAC_SLAVE_Msk

#define AICR_DAC_REG_DAC_ADWL_Pos              (6U)
#define AICR_DAC_REG_DAC_ADWL_Msk              (0x3U << AICR_DAC_REG_DAC_ADWL_Pos)
#define AICR_DAC_REG_DAC_ADWL_16               (0x0U << AICR_DAC_REG_DAC_ADWL_Pos)
#define AICR_DAC_REG_DAC_ADWL_18               (0x1U << AICR_DAC_REG_DAC_ADWL_Pos)
#define AICR_DAC_REG_DAC_ADWL_20               (0x2U << AICR_DAC_REG_DAC_ADWL_Pos)
#define AICR_DAC_REG_DAC_ADWL_24               (0x3U << AICR_DAC_REG_DAC_ADWL_Pos)

/* Offset: 0x24      (R/W)  AICR_ADC            Register */
#define AICR_ADC_REG_ADC_ADWL_Pos              (6U)
#define AICR_ADC_REG_ADC_ADWL_Msk              (0x3U << AICR_ADC_REG_ADC_ADWL_Pos)
#define AICR_ADC_REG_ADC_ADWL_16               (0x0U << AICR_ADC_REG_ADC_ADWL_Pos)
#define AICR_ADC_REG_ADC_ADWL_18               (0x1U << AICR_ADC_REG_ADC_ADWL_Pos)
#define AICR_ADC_REG_ADC_ADWL_20               (0x2U << AICR_ADC_REG_ADC_ADWL_Pos)
#define AICR_ADC_REG_ADC_ADWL_24               (0x3U << AICR_ADC_REG_ADC_ADWL_Pos)

#define AICR_ADC_REG_ADC_AUDIOIF_Pos           (0U)
#define AICR_ADC_REG_ADC_AUDIOIF_Msk           (0x3U << AICR_ADC_REG_ADC_AUDIOIF_Pos)
#define AICR_ADC_REG_ADC_AUDIOIF_PARALLEL      (0x0U << AICR_ADC_REG_ADC_AUDIOIF_Pos)
#define AICR_ADC_REG_ADC_AUDIOIF_RESERVED      (0x1U << AICR_ADC_REG_ADC_AUDIOIF_Pos)
#define AICR_ADC_REG_ADC_AUDIOIF_DSP           (0x2U << AICR_ADC_REG_ADC_AUDIOIF_Pos)
#define AICR_ADC_REG_ADC_AUDIOIF_I2S           (0x3U << AICR_ADC_REG_ADC_AUDIOIF_Pos)
/* Offset: 0x28      (R/W)  AICR_ADC_2          Register */
#define AICR_ADC_2_REG_SB_AICR_ADC12_Pos       (0U)
#define AICR_ADC_2_REG_SB_AICR_ADC12_Msk       (0x1U << AICR_ADC_2_REG_SB_AICR_ADC12_Pos)
#define AICR_ADC_2_REG_SB_AICR_ADC12_EN        AICR_ADC_2_REG_SB_AICR_ADC12_Msk

#define AICR_ADC_2_REG_SB_AICR_ADC34_Pos       (1U)
#define AICR_ADC_2_REG_SB_AICR_ADC34_Msk       (0x1U << AICR_ADC_2_REG_SB_AICR_ADC34_Pos)
#define AICR_ADC_2_REG_SB_AICR_ADC34_EN        AICR_ADC_2_REG_SB_AICR_ADC34_Msk

#define AICR_ADC_2_REG_SB_AICR_ADC56_Pos       (2U)
#define AICR_ADC_2_REG_SB_AICR_ADC56_Msk       (0x1U << AICR_ADC_2_REG_SB_AICR_ADC56_Pos)
#define AICR_ADC_2_REG_SB_AICR_ADC56_EN        AICR_ADC_2_REG_SB_AICR_ADC56_Msk

#define AICR_ADC_2_REG_SB_AICR_ADC78_Pos       (3U)
#define AICR_ADC_2_REG_SB_AICR_ADC78_Msk       (0x1U << AICR_ADC_2_REG_SB_AICR_ADC78_Pos)
#define AICR_ADC_2_REG_SB_AICR_ADC78_EN        AICR_ADC_2_REG_SB_AICR_ADC78_Msk
/* Offset: 0x2C      (R/W)  FCR_DAC             Register */
#define FCR_DAC_REG_DAC_FREQ_Pos           (0U)
#define FCR_DAC_REG_DAC_FREQ_Msk           (0xFU << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_8K            (0x0U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_11P025K       (0x1U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_12K           (0x2U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_16K           (0x3U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_22P05K        (0x4U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_24K           (0x5U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_32K           (0x6U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_44P1K         (0x7U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_48K           (0x8U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_96K           (0x10U << FCR_DAC_REG_DAC_FREQ_Pos)
/* Offset: 0X30      (R)     RESERVED */
/* Offset: 0x38      (R/W)  CR_WNF              Register */
#define CR_WNF_REG_ADC12_WNF_Pos        (0U)
#define CR_WNF_REG_ADC12_WNF_Msk        (0x3U <<CR_WNF_REG_ADC12_WNF_Pos)
#define CR_WNF_REG_ADC12_WNF_CLOSE      (0x0U <<CR_WNF_REG_ADC12_WNF_Pos)
#define CR_WNF_REG_ADC12_WNF_MODE1      (0x1U <<CR_WNF_REG_ADC12_WNF_Pos)
#define CR_WNF_REG_ADC12_WNF_MODE2      (0x2U <<CR_WNF_REG_ADC12_WNF_Pos)
#define CR_WNF_REG_ADC12_WNF_MODE3      (0x3U <<CR_WNF_REG_ADC12_WNF_Pos)

#define CR_WNF_REG_ADC34_WNF_Pos        (2U)
#define CR_WNF_REG_ADC34_WNF_Msk        (0x3U <<CR_WNF_REG_ADC34_WNF_Pos)
#define CR_WNF_REG_ADC34_WNF_CLOSE      (0x0U <<CR_WNF_REG_ADC34_WNF_Pos)
#define CR_WNF_REG_ADC34_WNF_MODE1      (0x1U <<CR_WNF_REG_ADC34_WNF_Pos)
#define CR_WNF_REG_ADC34_WNF_MODE2      (0x2U <<CR_WNF_REG_ADC34_WNF_Pos)
#define CR_WNF_REG_ADC34_WNF_MODE3      (0x3U <<CR_WNF_REG_ADC34_WNF_Pos)

#define CR_WNF_REG_ADC56_WNF_Pos        (4U)
#define CR_WNF_REG_ADC56_WNF_Msk        (0x3U <<CR_WNF_REG_ADC56_WNF_Pos)
#define CR_WNF_REG_ADC56_WNF_CLOSE      (0x0U <<CR_WNF_REG_ADC56_WNF_Pos)
#define CR_WNF_REG_ADC56_WNF_MODE1      (0x1U <<CR_WNF_REG_ADC56_WNF_Pos)
#define CR_WNF_REG_ADC56_WNF_MODE2      (0x2U <<CR_WNF_REG_ADC56_WNF_Pos)
#define CR_WNF_REG_ADC56_WNF_MODE3      (0x3U <<CR_WNF_REG_ADC56_WNF_Pos)

#define CR_WNF_REG_ADC78_WNF_Pos        (6U)
#define CR_WNF_REG_ADC78_WNF_Msk        (0x3U <<CR_WNF_REG_ADC78_WNF_Pos)
#define CR_WNF_REG_ADC78_WNF_CLOSE      (0x0U <<CR_WNF_REG_ADC78_WNF_Pos)
#define CR_WNF_REG_ADC78_WNF_MODE1      (0x1U <<CR_WNF_REG_ADC78_WNF_Pos)
#define CR_WNF_REG_ADC78_WNF_MODE2      (0x2U <<CR_WNF_REG_ADC78_WNF_Pos)
#define CR_WNF_REG_ADC78_WNF_MODE3      (0x3U <<CR_WNF_REG_ADC78_WNF_Pos)
/* Offset: 0x3C      (R/W)  FCR_ADC             Register */
#define FCR_ADC_REG_ADC_FREQ_Pos           (0U)
#define FCR_ADC_REG_ADC_FREQ_Msk           (0xFU << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_8K            (0x0U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_11P025K       (0x1U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_12K           (0x2U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_16K           (0x3U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_22P05K        (0x4U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_24K           (0x5U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_32K           (0x6U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_44P1K         (0x7U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_48K           (0x8U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_96K           (0x10U << FCR_ADC_REG_ADC_FREQ_Pos)

#define FCR_ADC_REG_ADC12_HPF_EN_Pos           (4U)
#define FCR_ADC_REG_ADC12_HPF_EN_Msk           (0x1U << FCR_ADC_REG_ADC12_HPF_EN_Pos)
#define FCR_ADC_REG_ADC12_HPF_EN               FCR_ADC_REG_ADC12_HPF_EN_Msk

#define FCR_ADC_REG_ADC34_HPF_EN_Pos           (5U)
#define FCR_ADC_REG_ADC34_HPF_EN_Msk           (0x1U << FCR_ADC_REG_ADC34_HPF_EN_Pos)
#define FCR_ADC_REG_ADC34_HPF_EN               FCR_ADC_REG_ADC34_HPF_EN_Msk

#define FCR_ADC_REG_ADC56_HPF_EN_Pos           (6U)
#define FCR_ADC_REG_ADC56_HPF_EN_Msk           (0x1U << FCR_ADC_REG_ADC56_HPF_EN_Pos)
#define FCR_ADC_REG_ADC56_HPF_EN               FCR_ADC_REG_ADC56_HPF_EN_Msk

#define FCR_ADC_REG_ADC78_HPF_EN_Pos            (7U)
#define FCR_ADC_REG_ADC78_HPF_EN_Msk            (0x1U << FCR_ADC_REG_ADC78_HPF_EN_Pos)
#define FCR_ADC_REG_ADC78_HPF_EN                FCR_ADC_REG_ADC78_HPF_EN_Msk

/* Offset: 0x40      (R/W)  CR_DMIC12           Register */
#define CR_DMIC12_REG_ADC_DMIC_SEL1_Pos          (2U)
#define CR_DMIC12_REG_ADC_DMIC_SEL1_Msk          (0x3U << CR_DMIC12_REG_ADC_DMIC_SEL1_Pos)
#define CR_DMIC12_REG_ADC_DMIC_SEL1_LOGIC        (0x0U << CR_DMIC12_REG_ADC_DMIC_SEL1_Msk)
#define CR_DMIC12_REG_ADC_DMIC_SEL1_DIGIT        (0x1U << CR_DMIC12_REG_ADC_DMIC_SEL1_Msk)
#define CR_DMIC12_REG_ADC_DMIC_SEL1_AVG_DIGIT    (0x2U << CR_DMIC12_REG_ADC_DMIC_SEL1_Msk)

#define CR_DMIC12_REG_ADC_DMIC_SEL2_Pos        (0U)
#define CR_DMIC12_REG_ADC_DMIC_SEL2_Msk        (0x3U << CR_DMIC12_REG_ADC_DMIC_SEL2_Pos)
#define CR_DMIC12_REG_ADC_DMIC_SEL2_LOGIC         (0x0U << CR_DMIC12_REG_ADC_DMIC_SEL2_Pos)
#define CR_DMIC12_REG_ADC_DMIC_SEL2_DIGIT         (0x1U << CR_DMIC12_REG_ADC_DMIC_SEL2_Pos)
#define CR_DMIC12_REG_ADC_DMIC_SEL2_AVG_DIGIT     (0x2U << CR_DMIC12_REG_ADC_DMIC_SEL2_Pos)

#define CR_DMIC12_REG_ADC_DMIC_RATE_Pos           (4U)
#define CR_DMIC12_REG_ADC_DMIC_RATE_Msk           (0x3U << CR_DMIC12_REG_ADC_DMIC_RATE_Pos)
#define CR_DMIC12_REG_ADC_DMIC_RATE_3P0_3P25      (0x0U << CR_DMIC12_REG_ADC_DMIC_RATE_Pos)
#define CR_DMIC12_REG_ADC_DMIC_RATE_2P4_2P6       (0x1U << CR_DMIC12_REG_ADC_DMIC_RATE_Pos)
#define CR_DMIC12_REG_ADC_DMIC_RATE_1P0_1P08      (0x1U << CR_DMIC12_REG_ADC_DMIC_RATE_Pos)

#define CR_DMIC12_REG_SB_DMIC2_Pos                (6U)
#define CR_DMIC12_REG_SB_DMIC2_Msk                (0x1U << CR_DMIC12_REG_SB_DMIC2_Pos)
#define CR_DMIC12_REG_SB_DMIC2_EN                 CR_DMIC12_REG_SB_DMIC2_Msk

#define CR_DMIC12_REG_SB_DMIC1_Pos               (7U)
#define CR_DMIC12_REG_SB_DMIC1_Msk               (0x1U << CR_DMIC12_REG_SB_DMIC2_Pos)
#define CR_DMIC12_REG_SB_DMIC1_EN                CR_DMIC12_REG_SB_DMIC2_Msk
/* Offset: 0x44      (R/W)  CR_DMIC34           Register */
#define CR_DMIC34_REG_ADC_DMIC_SEL4_Pos           (0U)
#define CR_DMIC34_REG_ADC_DMIC_SEL4_Msk           (0x3U << CR_DMIC34_REG_ADC_DMIC_SEL4_Pos)
#define CR_DMIC34_REG_ADC_DMIC_SEL4_LOGIC         (0x0U << CR_DMIC34_REG_ADC_DMIC_SEL4_Msk)
#define CR_DMIC34_REG_ADC_DMIC_SEL4_DIGIT         (0x1U << CR_DMIC34_REG_ADC_DMIC_SEL4_Msk)
#define CR_DMIC34_REG_ADC_DMIC_SEL4_AVG_DIGIT     (0x2U << CR_DMIC34_REG_ADC_DMIC_SEL4_Msk)

#define CR_DMIC34_REG_ADC_DMIC_SEL3_EN_Pos        (2U)
#define CR_DMIC34_REG_ADC_DMIC_SEL3_EN_Msk        (0x3U << CR_DMIC34_REG_ADC_DMIC_SEL3_EN_Pos)
#define CR_DMIC34_REG_ADC_DMIC_SEL3_LOGIC         (0x0U << CR_DMIC34_REG_ADC_DMIC_SEL3_EN_Pos)
#define CR_DMIC34_REG_ADC_DMIC_SEL3_DIGIT         (0x1U << CR_DMIC34_REG_ADC_DMIC_SEL3_EN_Pos)
#define CR_DMIC34_REG_ADC_DMIC_SEL3_AVG_DIGIT     (0x2U << CR_DMIC34_REG_ADC_DMIC_SEL3_EN_Pos)

#define CR_DMIC34_REG_SB_DMIC4_Pos                (6U)
#define CR_DMIC34_REG_SB_DMIC4_Msk                (0x1U << CR_DMIC34_REG_SB_DMIC4_Pos)
#define CR_DMIC34_REG_SB_DMIC4_EN                 CR_DMIC34_REG_SB_DMIC4_Msk

#define CR_DMIC34_REG_SB_DMIC3_Pos                (7U)
#define CR_DMIC34_REG_SB_DMIC3_Msk                (0x1U << CR_DMIC34_REG_SB_DMIC3_Pos)
#define CR_DMIC34_REG_SB_DMIC3_EN                 CR_DMIC34_REG_SB_DMIC3_Msk
/* Offset: 0x48      (R/W)  CR_DMIC56           Register */
#define CR_DMIC56_REG_ADC_DMIC_SEL6_Pos           (0U)
#define CR_DMIC56_REG_ADC_DMIC_SEL6_Msk           (0x3U << CR_DMIC56_REG_ADC_DMIC_SEL6_Pos)
#define CR_DMIC56_REG_ADC_DMIC_SEL6_LOGIC         (0x0U << CR_DMIC56_REG_ADC_DMIC_SEL6_Msk)
#define CR_DMIC56_REG_ADC_DMIC_SEL6_DIGIT         (0x1U << CR_DMIC56_REG_ADC_DMIC_SEL6_Msk)
#define CR_DMIC56_REG_ADC_DMIC_SEL6_AVG_DIGIT     (0x2U << CR_DMIC56_REG_ADC_DMIC_SEL6_Msk)

#define CR_DMIC56_REG_ADC_DMIC_SEL5_Pos           (2U)
#define CR_DMIC56_REG_ADC_DMIC_SEL5_Msk           (0x3U << CR_DMIC56_REG_ADC_DMIC_SEL5_Pos)
#define CR_DMIC56_REG_ADC_DMIC_SEL5_LOGIC         (0x0U << CR_DMIC56_REG_ADC_DMIC_SEL5_Pos)
#define CR_DMIC56_REG_ADC_DMIC_SEL5_DIGIT         (0x1U << CR_DMIC56_REG_ADC_DMIC_SEL5_Pos)
#define CR_DMIC56_REG_ADC_DMIC_SEL5_AVG_DIGIT     (0x2U << CR_DMIC56_REG_ADC_DMIC_SEL5_Pos)

#define CR_DMIC56_REG_SB_DMIC6_Pos                (6U)
#define CR_DMIC56_REG_SB_DMIC6_Msk                (0x1U << CR_DMIC56_REG_SB_DMIC6_Pos)
#define CR_DMIC56_REG_SB_DMIC6_EN                 CR_DMIC56_REG_SB_DMIC6_Msk

#define CR_DMIC56_REG_SB_DMIC5_Pos                (7U)
#define CR_DMIC56_REG_SB_DMIC5_Msk                (0x1U << CR_DMIC56_REG_SB_DMIC5_Pos)
#define CR_DMIC56_REG_SB_DMIC5_EN                 CR_DMIC56_REG_SB_DMIC5_Msk

/* Offset: 0x4C      (R/W)  CR_DMIC78           Register */
#define CR_DMIC78_REG_ADC_DMIC_SEL8_Pos           (0U)
#define CR_DMIC78_REG_ADC_DMIC_SEL8_Msk           (0x3U << CR_DMIC78_REG_ADC_DMIC_SEL8_Pos)
#define CR_DMIC78_REG_ADC_DMIC_SEL8_LOGIC         (0x0U << CR_DMIC78_REG_ADC_DMIC_SEL8_Msk)
#define CR_DMIC78_REG_ADC_DMIC_SEL8_DIGIT         (0x1U << CR_DMIC78_REG_ADC_DMIC_SEL8_Msk)
#define CR_DMIC78_REG_ADC_DMIC_SEL8_AVG_DIGIT     (0x2U << CR_DMIC78_REG_ADC_DMIC_SEL8_Msk)

#define CR_DMIC78_REG_ADC_DMIC_SEL7_Pos           (2U)
#define CR_DMIC78_REG_ADC_DMIC_SEL7_Msk           (0x3U << CR_DMIC78_REG_ADC_DMIC_SEL7_Pos)
#define CR_DMIC78_REG_ADC_DMIC_SEL7_LOGIC         (0x0U << CR_DMIC78_REG_ADC_DMIC_SEL7_Pos)
#define CR_DMIC78_REG_ADC_DMIC_SEL7_DIGIT         (0x1U << CR_DMIC78_REG_ADC_DMIC_SEL7_Pos)
#define CR_DMIC78_REG_ADC_DMIC_SEL7_AVG_DIGIT     (0x2U << CR_DMIC78_REG_ADC_DMIC_SEL7_Pos)

#define CR_DMIC78_REG_SB_DMIC8_Pos                (6U)
#define CR_DMIC78_REG_SB_DMIC8_Msk                (0x1U << CR_DMIC78_REG_SB_DMIC8_Pos)
#define CR_DMIC78_REG_SB_DMIC8_EN                 CR_DMIC78_REG_SB_DMIC8_Msk

#define CR_DMIC78_REG_SB_DMIC7_Pos                (7U)
#define CR_DMIC78_REG_SB_DMIC7_Msk                (0x1U << CR_DMIC78_REG_SB_DMIC7_Pos)
#define CR_DMIC78_REG_SB_DMIC7_EN                 CR_DMIC78_REG_SB_DMIC7_Msk

/* Offset: 0x50      (R/W)  CR_HP               Register */
#define CR_HP_REG_SB_HP_OPEN_Pos             (4U)
#define CR_HP_REG_SB_HP_OPEN_Msk             (0x1U << CR_HP_REG_SB_HP_OPEN_Pos)
#define CR_HP_REG_SB_HP_OPEN_EN              CR_HP_REG_SB_HP_OPEN_Msk

#define CR_HP_REG_SB_HPCM_Pos                (5U)
#define CR_HP_REG_SB_HPCM_Msk                (0x1U << CR_HP_REG_SB_HP_Pos)

#define CR_HP_REG_HP_MUTE_Pos                (7U)
#define CR_HP_REG_HP_MUTE_Msk                (0x1U << CR_HP_REG_HP_MUTE_Pos)
#define CR_HP_REG_HP_MUTE_EN                 CR_HP_REG_HP_MUTE_Msk

/* Offset: 0x54      (R/W)  GCR_HPL             Register */
#define GCR_HPL_REG_GOL_Pos                  (0U)
#define GCR_HPL_REG_GOL_Msk                  (0xFU << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_0DB                  (0x0U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_1DB                  (0x1U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_2DB                  (0x2U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_3DB                  (0x3U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_4DB                  (0x4U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_5DB                  (0x5U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_6DB                  (0x6U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_7DB                  (0x7U << GCR_HPL_REG_GOL_Pos)

#define GCR_HPL_REG_LRGO_Pos                 (6U)
#define GCR_HPL_REG_LRGO_Msk                 (0x1U << GCR_HPL_REG_LRGO_Pos)
#define GCR_HPL_REG_LRGO_INDEPENDENT         (0x0U << GCR_HPL_REG_LRGO_Pos)
#define GCR_HPL_REG_LRGO_TOGETHER            GCR_HPL_REG_LRGO_Msk
/* Offset: 0x58      (R/W)  GCR_HPR             Register */
#define GCR_HPR_REG_GOR_Pos                  (0U)
#define GCR_HPR_REG_GOR_Msk                  (0xFU << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_0DB                  (0x0U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_1DB                  (0x1U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_2DB                  (0x2U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_3DB                  (0x3U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_4DB                  (0x4U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_5DB                  (0x5U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_6DB                  (0x6U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_7DB                  (0x7U << GCR_HPR_REG_GOR_Pos)


/* Offset: 0x5C      (R/W)  CR_MIC1_REG              Register */
#define CR_MIC1_REG_SB_MICBIAS1_Pos           (5U)
#define CR_MIC1_REG_SB_MICBIAS1_Msk           (0x1U << CR_MIC1_REG_SB_MICBIAS1_Pos)
#define CR_MIC1_REG_SB_MICBIAS1_OPEN          (0x0U << CR_MIC1_REG_SB_MICBIAS1_Pos)
#define CR_MIC1_REG_SB_MICBIAS1_CLOSE         CR_MIC1_REG_SB_MICBIAS1_Msk

#define CR_MIC1_REG_SB_MICDIFF1_Pos           (6U)
#define CR_MIC1_REG_SB_MICDIFF1_Msk           (0x1U << CR_MIC1_REG_SB_MICBIAS1_Pos)
#define CR_MIC1_REG_SB_MICDIFF1_DIFF          (0x0U << CR_MIC1_REG_SB_MICBIAS1_Pos)
#define CR_MIC1_REG_SB_MICDIFF1_SINGLE        CR_MIC1_REG_SB_MICDIFF1_Msk

#define CR_MIC1_REG_SB_MICBIAS1_V_Pos         (7U)
#define CR_MIC1_REG_SB_MICBIAS1_V_Msk         (0x1U << CR_MIC1_REG_SB_MICBIAS1_Pos)
#define CR_MIC1_REG_SB_MICBIAS1_V_1P5         (0x0U << CR_MIC1_REG_SB_MICBIAS1_Pos)
#define CR_MIC1_REG_SB_MICBIAS1_V_1P8         (0x0U << CR_MIC1_REG_SB_MICBIAS1_Pos)


/* Offset: 0x8C      (R/W)  CR_DAC              Register */
#define GCR_MIC_REG_GIM1_Pos                  (0U)
#define GCR_MIC_REG_GIM1_Msk                  (0x7U << GCR_MIC_REG_GIM1_Pos)
#define GCR_MIC_REG_GIM_0DB                   (0x0U << GCR_MIC_REG_GIM1_Pos)
#define GCR_MIC_REG_GIM_4DB                   (0x1U << GCR_MIC_REG_GIM1_Pos)
#define GCR_MIC_REG_GIM_8DB                   (0x2U << GCR_MIC_REG_GIM1_Pos)
#define GCR_MIC_REG_GIM_12B                   (0x3U << GCR_MIC_REG_GIM1_Pos)
#define GCR_MIC_REG_GIM_16DB                  (0x4U << GCR_MIC_REG_GIM1_Pos)
#define GCR_MIC_REG_GIM_20DB                  (0x5U << GCR_MIC_REG_GIM1_Pos)

#define GCR_MIC_REG_GIM2_Pos                   (3U)
#define GCR_MIC_REG_GIM2_Msk                   (0x7U << GCR_MIC_REG_GIM2_Pos)
// #define GCR_MIC_REG_GIM2_0DB                   (0x0U << GCR_MIC_REG_GIM2_Pos)
// #define GCR_MIC_REG_GIM2_4DB                   (0x1U << GCR_MIC_REG_GIM2_Pos)
// #define GCR_MIC_REG_GIM2_8DB                   (0x2U << GCR_MIC_REG_GIM2_Pos)
// #define GCR_MIC_REG_GIM2_12B                   (0x3U << GCR_MIC_REG_GIM2_Pos)
// #define GCR_MIC_REG_GIM2_16DB                  (0x4U << GCR_MIC_REG_GIM2_Pos)
// #define GCR_MIC_REG_GIM2_20DB                  (0x5U << GCR_MIC_REG_GIM2_Pos)
/* Offset: 0x8C      (R/W)  CR_DAC              Register */
#define CR_DAC_REG_SB_DAC_Pos                   (4U)
#define CR_DAC_REG_SB_DAC_Msk                   (0x1U << CR_DAC_REG_SB_DAC_Pos)
#define CR_DAC_REG_SB_DAC_EN                     CR_DAC_REG_SB_DAC_Msk

#define CR_DAC_REG_DAC_LEFT_ONLY_Pos            (5U)
#define CR_DAC_REG_DAC_LEFT_ONLY_Msk            (0x1U << CR_DAC_REG_DAC_LEFT_ONLY_Pos)
#define CR_DAC_REG_DAC_LEFT_ONLY_EN             CR_DAC_REG_DAC_LEFT_ONLY_Msk

#define CR_DAC_REG_DAC_SOFT_MUTE_Pos            (6U)
#define CR_DAC_REG_DAC_SOFT_MUTE_Msk            (0x1U << CR_DAC_REG_DAC_SOFT_MUTE_Pos)
#define CR_DAC_REG_DAC_SOFT_MUTE_EN           CR_DAC_REG_DAC_SOFT_MUTE_Msk
/* Offset: 0x8C      (R/W)  CR_ADC              Register */
#define CR_ADC_REG_SB_ADC1_Pos                  (4U)
#define CR_ADC_REG_SB_ADC1_Msk                  (0x1U << CR_ADC_REG_SB_ADC1_Pos)
#define CR_ADC_REG_SB_ADC1_OPEN                 CR_ADC_REG_SB_ADC1_Msk

#define CR_ADC_REG_SB_ADC2_Pos                  (5U)
#define CR_ADC_REG_SB_ADC2_Msk                  (0x1U << CR_ADC_REG_SB_ADC1_Pos)
#define CR_ADC_REG_SB_ADC2_OPEN                 CR_ADC_REG_SB_ADC1_Msk

#define CR_ADC_REG_ADC12_SOFT_MUTE_Pos          (7U)
#define CR_ADC_REG_ADC12_SOFT_MUTE_Msk          (0x1U << CR_ADC_REG_ADC12_SOFT_MUTE_Pos)
#define CR_ADC_REG_ADC12_SOFT_MUTE_OPEN         CR_ADC_REG_ADC12_SOFT_MUTE_Msk
/* Offset: 0xA0      (R/W)  CR_MIX              Register */
#define CR_MIX_REG_MIX_ADD_Pos                  (0U)
#define CR_MIX_REG_MIX_ADD_Msk                  (0x3FU << CR_MIX_REG_MIX_ADD_Pos)

#define CR_MIX_REG_MIX_LOAD_Pos                 (6U)
#define CR_MIX_REG_MIX_LOAD_Msk                 (0x1U << CR_MIX_REG_MIX_LOAD_Pos)
#define CR_MIX_REG_MIX_LOAD_EN                  CR_MIX_REG_MIX_LOAD_Msk
/* Offset: 0xA4      (R/W)  DR_MIX              Register */
#define DR_MIX_REG_MIX_DATA_Pos                 (0U)
#define DR_MIX_REG_MIX_DATA_Msk                 (0xFFU << DR_MIX_REG_MIX_DATA_Pos)
/* Offset: 0xA8      (R/W)  GCR_DACL            Register */
#define GCR_DACL_REG_CODL_Pos                   (0U)
#define GCR_DACL_REG_CODL_Msk                   (0x3FU << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_0DB                   (0x0U << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_F1DB                  (0x1U << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_F2DB                  (0x2U << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_F31DB                 (0x1FU << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_1DB                   (0x3FU << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_2DB                   (0x3EU << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_32DB                  (0x20U << GCR_DACL_REG_CODL_Pos)

#define GCR_DACL_REG_LRGOD_Pos                   (7U)
#define GCR_DACL_REG_LRGOD_Msk                   (0x1U << GCR_DACL_REG_LRGOD_Pos)
#define GCR_DACL_REG_LRGOD_TOGETHER              GCR_DACL_REG_LRGOD_Msk
/* Offset: 0xAC      (R/W)  GCR_DACR            Register */
#define GCR_DACR_REG_CODR_Pos                   (0U)
#define GCR_DACR_REG_CODR_Msk                   (0x3FU << GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_0DB                   (0x0U <<  GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_F1DB                  (0x1U <<  GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_F2DB                  (0x2U <<  GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_F31DB                 (0x1FU << GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_1DB                   (0x3FU << GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_2DB                   (0x3EU << GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_32DB                  (0x20U << GCR_DACR_REG_CODR_Pos)

#define GCR_ADC_REG_GODR_Pos                    (0U)
#define GCR_ADC_REG_GODR_Msk                    (0x3FU << GCR_DACR_REG_CODR_Pos)
#define GCR_ADC_REG_LRGID_Pos                   (7U)
#define GCR_ADC_REG_LRGID_Msk                   (0x1U << GCR_ADC_REG_LRGID_Pos)

/* Offset: 0xB0      (R/W)  GCR_ADC1            Register */
#define GCR_ADC1_REG_GID1_Pos                   (0U)
#define GCR_ADC1_REG_GID1_Msk                   (0x3FU << GCR_ADC1_REG_GID1_Pos)

#define GCR_ADC1_REG_LRGID_Pos                   (7U)
#define GCR_ADC1_REG_LRGID_Msk                   (0x1U << GCR_ADC1_REG_LRGID_Pos)
#define GCR_ADC1_REG_LRGID                       GCR_ADC1_REG_LRGID_Msk
/* Offset: 0xD0      (R/W)  GCR_MIXDACL         Register */
#define GCR_MIXDACL_REG_GOMIXL_Pos              (0U)
#define GCR_MIXDACL_REG_GOMIXL_Msk              (0x1FU << GCR_DACR_REG_CODR_Pos)
#define GCR_MIXDACR_L_0DB                       (0U)
#define GCR_MIXDACR_L_F1DB                       (1U)
#define GCR_MIXDACR_L_F2DB                       (2U)
#define GCR_MIXDACR_L_F3DB                       (3U)
#define GCR_MIXDACR_L_F4DB                       (4U)
#define GCR_MIXDACR_L_F5DB                       (5U)
#define GCR_MIXDACR_L_F6DB                       (6U)
#define GCR_MIXDACR_L_F7DB                       (7U)
#define GCR_MIXDACR_L_F8DB                       (8U)
#define GCR_MIXDACR_L_F9DB                       (9U)
#define GCR_MIXDACR_L_F10DB                      (10U)
#define GCR_MIXDACR_L_F11DB                      (11U)
#define GCR_MIXDACR_L_F12DB                      (12U)
#define GCR_MIXDACR_L_F13DB                      (13U)
#define GCR_MIXDACR_L_F14DB                      (14U)
#define GCR_MIXDACR_L_F15DB                      (15U)
#define GCR_MIXDACR_L_F16DB                      (16U)
#define GCR_MIXDACR_L_F17DB                      (17U)
#define GCR_MIXDACR_L_F18DB                      (18U)
#define GCR_MIXDACR_L_F19DB                      (19U)
#define GCR_MIXDACR_L_F20DB                      (20U)
#define GCR_MIXDACR_L_F21DB                      (21U)
#define GCR_MIXDACR_L_F22DB                      (22U)
#define GCR_MIXDACR_L_F23DB                      (23U)
#define GCR_MIXDACR_L_F24DB                      (24U)
#define GCR_MIXDACR_L_F25DB                      (25U)
#define GCR_MIXDACR_L_F26DB                      (26U)
#define GCR_MIXDACR_L_F27DB                      (27U)
#define GCR_MIXDACR_L_F28DB                      (28U)
#define GCR_MIXDACR_L_F29DB                      (29U)
#define GCR_MIXDACR_L_F30DB                      (30U)
#define GCR_MIXDACR_L_F31DB                      (31U)
#define GCR_MIXDACL_REG_LRGOMIX_Pos             (7U)
#define GCR_MIXDACL_REG_LRGOMIX_Msk             (0x1U << GCR_MIXDACL_REG_LRGOMIX_Pos)
#define GCR_MIXDACL_REG_LRGOMIX_INDEPENDENT     (0x0U << GCR_MIXDACL_REG_LRGOMIX_Pos)
#define GCR_MIXDACL_REG_LRGOMIX_TOGETHER        GCR_MIXDACL_REG_LRGOMIX_Msk
/* Offset: 0xD4      (R/W)  GCR_MIXDACR         Register */
#define GCR_MIXDACR_REG_GOMIXR_Pos              (0U)
#define GCR_MIXDACR_REG_GOMIXR_Msk              (0x1FU << GCR_MIXDACR_REG_GOMIXR_Pos)
/* Offset: 0xD8      (R/W)  GCR_MIXADCL         Register */
#define GCR_MIXADCL_REG_GIMIXL_Pos              (0U)
#define GCR_MIXADCL_REG_GIMIXL_Msk              (0x1FU << GCR_MIXADCL_REG_GIMIXL_Pos)
#define GCR_MIXADCL_REG_LRGIMIX_Pos              (7U)
#define GCR_MIXADCL_REG_LRGIMIX_Msk              (0x1U << GCR_MIXADCL_REG_LRGIMIX_Pos)
/* Offset: 0xDC      (R/W)  GCR_MIXADCR         Register */
#define GCR_MIXADCR_REG_GIMIXR_Pos             (0U)
#define GCR_MIXADCR_REG_GIMIXR_Msk             (0x1FU << GCR_MIXADCR_REG_GIMIXR_Pos)
/* Offset: 0xE0      (R/W)  CR_DAC_AGC          Register */
/* Offset: 0xE4      (R/W)  DR_DAC_AGC          Register */
/* Offset: 0xE8      (R/W)  CR_ADC_AGC          Register */
#define CR_ADC_AGC_REG_ADC_AGC_ADD_Pos         (0U)
#define CR_ADC_AGC_REG_ADC_AGC_ADD_Msk         (0x3FU << CR_ADC_AGC_REG_ADC_AGC_ADD_Pos)

#define CR_ADC_AGC_REG_ADC_AGC_LOAD_Pos         (6U)
#define CR_ADC_AGC_REG_ADC_AGC_LOAD_Msk         (0x3FU << CR_ADC_AGC_REG_ADC_AGC_LOAD_Pos)
#define CR_ADC_AGC_REG_ADC_AGC_LOAD_EN          CR_ADC_AGC_REG_ADC_AGC_LOAD_Msk
/* Offset: 0xEC      (R/W)  DR_ADC_AGC          Register */
#define DR_ADC_AGC_REG_ADC_AGC_DATA_Pos        (0U)
#define DR_ADC_AGC_REG_ADC_AGC_DATA_Msk        (0xFFU << DR_ADC_AGC_REG_ADC_AGC_DATA_Pos)
/* Offset: 0xF0      (R/W)  CR_DAC_FI           Register */
/* Offset: 0xF4      (R/W)  DR_DAC_FI           Register */
/* Offset: 0XF8      (R)     RESERVED */
/* Offset: 0x100     (R/W)  SR_WT               Register */
#define SR_WT_REG_IRQ_WT_PENDING_Pos           (6U)
#define SR_WT_REG_IRQ_WT_PENDING_Msk           (0x1U << SR_WT_REG_IRQ_WT_PENDING_Pos)
#define SR_WT_REG_IRQ_WT_PENDING_EN            CR_ADC_AGC_REG_ADC_AGC_ADD_Msk
/* Offset: 0x104     (R/W)  ICR_WT              Register */
#define ICR_WT_REG_WT_INT_FORM_Pos             (6U)
#define ICR_WT_REG_WT_INT_FORM_Msk             (0x3U << ICR_WT_REG_WT_INT_FORM_Pos)
#define ICR_WT_REG_WT_INT_FORM_HIGH            (0U<<ICR_WT_REG_WT_INT_FORM_Pos)
#define ICR_WT_REG_WT_INT_FORM_LOW             (1U<<ICR_WT_REG_WT_INT_FORM_Pos)
#define ICR_WT_REG_WT_INT_FORM_8MCCLK_HIGH     (2U<<ICR_WT_REG_WT_INT_FORM_Pos)
#define ICR_WT_REG_WT_INT_FORM_8MCCLK_LOW      (3U<<ICR_WT_REG_WT_INT_FORM_Pos)
/* Offset: 0x108     (R/W)  IMR_WT              Register */
#define IMR_WT_REG_WT_DTC_MASK_Pos             (0U)
#define IMR_WT_REG_WT_DTC_MASK_Msk             (0x1U << IMR_WT_REG_WT_DTC_MASK_Pos)
#define IMR_WT_REG_WT_DTC_MASK_EN              IMR_WT_REG_WT_DTC_MASK_Msk
/* Offset: 0x10C     (R/W)  IFR_WT              Register */
#define IFR_WT_REG_WT_DTC_Pos                  (0U)
#define IFR_WT_REG_WT_DTC_Msk                  (0x1U << IFR_WT_REG_WT_DTC_Pos)
#define IFR_WT_REG_WT_DTC_EN                   IFR_WT_REG_WT_DTC_Msk
/* Offset: 0x110     (R/W)  CR_WT               Register */
#define CR_WT_REG_WT_RESTART_Pos               (0U)
#define CR_WT_REG_WT_RESTART_Msk               (0x1U << CR_WT_REG_WT_RESTART_Pos)
#define CR_WT_REG_WT_RESTART_EN                CR_WT_REG_WT_RESTART_Msk

#define CR_WT_REG_WT_ONLY_Pos                  (6U)
#define CR_WT_REG_WT_ONLY_Msk                  (0x1U << CR_WT_REG_WT_ONLY_Pos)
#define CR_WT_REG_WT_ONLY_EN                   CR_WT_REG_WT_ONLY_Msk

#define CR_WT_REG_WT_EN_Pos                    (7U)
#define CR_WT_REG_WT_EN_Msk                    (0x1U << CR_WT_REG_WT_EN_Pos)
#define CR_WT_REG_WT_EN                        CR_WT_REG_WT_EN_Msk
/* Offset: 0x114     (R/W)  CR_WT_2             Register */
#define CR_WT2_REG_WT_DMIC_FREQ_Pos            (0U)
#define CR_WT2_REG_WT_DMIC_FREQ_Msk            (0x7U << CR_WT2_REG_WT_DMIC_FREQ_Pos)
#define CR_WT2_REG_WT_DMIC_FREQ_3P0_3P25       (0x0U << CR_WT2_REG_WT_DMIC_FREQ_Pos)
#define CR_WT2_REG_WT_DMIC_FREQ_2P4_2P6        (0x1U << CR_WT2_REG_WT_DMIC_FREQ_Pos)
#define CR_WT2_REG_WT_DMIC_FREQ_1P0_1P08       (0x1U << CR_WT2_REG_WT_DMIC_FREQ_Pos)

#define CR_WT2_REG_ZCD_Pos           (4U)
#define CR_WT2_REG_ZCD_Msk           (0x1U << CR_WT2_REG_ZCD_Pos)
#define CR_WT2_REG_ZCD_EN               CR_WT2_REG_ZCD_Msk

#define CR_WT2_REG_WT_ADC_SEL_Pos       (7U)
#define CR_WT2_REG_WT_ADC_SEL_Msk       (0x1U << CR_WT2_REG_WT_ADC_SEL_Pos)
#define CR_WT2_REG_WT_ADC_SEL_ADC1      CR_WT2_REG_WT_ADC_SEL_Msk
#define CR_WT2_REG_WT_ADC_SEL_DIGIT1    (0U)

/* Offset: 0x118     (R/W)  CR_WT_3             Register */
#define CR_WT_3_REG_WT_POWER_SENS_Pos           (0U)
#define CR_WT_3_REG_WT_POWER_SENS_Msk           (0x1FU << CR_WT_3_REG_WT_POWER_SENS_Pos)
#define CR_WT_3_REG_WT_POWER_SENS_0DB           (0x0U << CR_WT_3_REG_WT_POWER_SENS_Pos)
/* Offset: 0x11C     (R/W)  CR_WT_4             Register */
/* Offset: 0X120     (R)     RESERVED */
/* Offset: 0x180     (R/W)  CR_TR               Register */
/* Offset: 0x184     (R/W)  DR_TR               Register */
/* Offset: 0x188     (R/W)  SR_TR1              Register */
/* Offset: 0x18C     (R/W)  SR_TR_SRCDAC        Register */
/* TRAN_SEL_REG, offset: 0x200 */
#define TRAN_SEL_REG_SI2C_Pos                       (0U)
#define TRAN_SEL_REG_SI2C_Msk                       (0x1U << TRAN_SEL_REG_SI2C_Pos)
#define TRAN_SEL_REG_I2C                            TRAN_SEL_REG_SI2C_Msk
#define TRAN_SEL_REG_uC                             (0U)
/* PARA_TX_FIFO_REG   ,offset：0x204 */
#define TRAN_TX_FIFOREG_LEFT_DATA_Pos                       (0U)
#define TRAN_TX_FIFOREG_LEFT_DATA_Msk                       (0xFFFFU << TRAN_TX_FIFOREG_LEFT_DATA_Pos)
#define TRAN_TX_FIFOREG_LEFT_DATA                           TRAN_TX_FIFOREG_LEFT_DATA_Msk

#define TRAN_TX_FIFOREG_RIGHT_DATA_Pos                      (16U)
#define TRAN_TX_FIFOREG_RIGHT_DATA_Msk                      (0xFFFFU << TRAN_TX_FIFOREG_RIGHT_DATA_Pos)
#define TRAN_TX_FIFOREG_RIGHT_DATA                          TRAN_TX_FIFOREG_RIGHT_DATA_Msk
/* RX_FIFO1_REG       ,offset：0x208 */
#define FIFO_TH_CTRL_TX_Pos                       (4U)
#define FIFO_TH_CTRL_TX_Msk                       (0x7U << FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_CTRL_TX                           FIFO_TH_CTRL_TX_Msk
#define FIFO_TH_TX_1                              (0U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_2                              (1U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_3                              (2U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_4                              (3U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_5                              (4U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_6                              (5U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_7                              (6U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_8                              (7U<< FIFO_TH_CTRL_TX_Pos)

#define FIFO_TH_CTRL_RX_Pos                       (0U)
#define FIFO_TH_CTRL_RX_Msk                       (0x7U << FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_CTRL_RX                           FIFO_TH_CTRL_RX_Msk
#define FIFO_TH_RX_1                              (0U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_2                              (1U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_3                              (2U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_4                              (3U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_5                              (4U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_6                              (5U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_7                              (6U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_8                              (7U<< FIFO_TH_CTRL_RX_Pos)
/* INTR_ERR_CTRL      ,offset：0x22C */
#define INTR_ERR_CTRL_REG_RX_FULL1_Pos                       (0U)
#define INTR_ERR_CTRL_REG_RX_FULL1_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL1_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL1                           INTR_ERR_CTRL_REG_RX_FULL1_Msk
#define INTR_ERR_CTRL_REG_RX_FULL2_Pos                       (1U)
#define INTR_ERR_CTRL_REG_RX_FULL2_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL2_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL2                           INTR_ERR_CTRL_REG_RX_FULL2_Msk
#define INTR_ERR_CTRL_REG_RX_FULL3_Pos                       (2U)
#define INTR_ERR_CTRL_REG_RX_FULL3_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL3_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL3                           INTR_ERR_CTRL_REG_RX_FULL3_Msk
#define INTR_ERR_CTRL_REG_RX_FULL4_Pos                       (3U)
#define INTR_ERR_CTRL_REG_RX_FULL4_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL4_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL4                           INTR_ERR_CTRL_REG_RX_FULL4_Msk
#define INTR_ERR_CTRL_REG_RX_FULL5_Pos                       (4U)
#define INTR_ERR_CTRL_REG_RX_FULL5_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL5_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL5                           INTR_ERR_CTRL_REG_RX_FULL5_Msk
#define INTR_ERR_CTRL_REG_RX_FULL6_Pos                       (5U)
#define INTR_ERR_CTRL_REG_RX_FULL6_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL6_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL6                           INTR_ERR_CTRL_REG_RX_FULL6_Msk
#define INTR_ERR_CTRL_REG_RX_FULL7_Pos                       (6U)
#define INTR_ERR_CTRL_REG_RX_FULL7_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL7_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL7                           INTR_ERR_CTRL_REG_RX_FULL7_Msk
#define INTR_ERR_CTRL_REG_RX_FULL8_Pos                       (7U)
#define INTR_ERR_CTRL_REG_RX_FULL8_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL8_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL8                           INTR_ERR_CTRL_REG_RX_FULL8_Msk
#define INTR_ERR_CTRL_REG_RX_TH1_Pos                         (8U)
#define INTR_ERR_CTRL_REG_RX_TH1_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH1_Pos)
#define INTR_ERR_CTRL_REG_RX_TH1                             INTR_ERR_CTRL_REG_RX_TH1_Msk
#define INTR_ERR_CTRL_REG_RX_TH2_Pos                         (9U)
#define INTR_ERR_CTRL_REG_RX_TH2_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH2_Pos)
#define INTR_ERR_CTRL_REG_RX_TH2                             INTR_ERR_CTRL_REG_RX_TH2_Msk
#define INTR_ERR_CTRL_REG_RX_TH3_Pos                         (10U)
#define INTR_ERR_CTRL_REG_RX_TH3_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH3_Pos)
#define INTR_ERR_CTRL_REG_RX_TH3                             INTR_ERR_CTRL_REG_RX_TH3_Msk
#define INTR_ERR_CTRL_REG_RX_TH4_Pos                         (11U)
#define INTR_ERR_CTRL_REG_RX_TH4_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH4_Pos)
#define INTR_ERR_CTRL_REG_RX_TH4                             INTR_ERR_CTRL_REG_RX_TH4_Msk
#define INTR_ERR_CTRL_REG_RX_TH5_Pos                         (12U)
#define INTR_ERR_CTRL_REG_RX_TH5_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH5_Pos)
#define INTR_ERR_CTRL_REG_RX_TH5                             INTR_ERR_CTRL_REG_RX_TH5_Msk
#define INTR_ERR_CTRL_REG_RX_TH6_Pos                         (13U)
#define INTR_ERR_CTRL_REG_RX_TH6_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH6_Pos)
#define INTR_ERR_CTRL_REG_RX_TH6                             INTR_ERR_CTRL_REG_RX_TH6_Msk
#define INTR_ERR_CTRL_REG_RX_TH7_Pos                         (14U)
#define INTR_ERR_CTRL_REG_RX_TH7_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH7_Pos)
#define INTR_ERR_CTRL_REG_RX_TH7                             INTR_ERR_CTRL_REG_RX_TH7_Msk
#define INTR_ERR_CTRL_REG_RX_TH8_Pos                         (15U)
#define INTR_ERR_CTRL_REG_RX_TH8_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH8_Pos)
#define INTR_ERR_CTRL_REG_RX_TH8                             INTR_ERR_CTRL_REG_RX_TH8_Msk
#define INTR_ERR_CTRL_REG_PARA_TX_EMPTY_Pos                  (16U)
#define INTR_ERR_CTRL_REG_PARA_TX_EMPTY_Msk                  (0x1U << INTR_ERR_CTRL_REG_PARA_TX_EMPTY_Pos)
#define INTR_ERR_CTRL_REG_PARA_TX_EMPTY                      INTR_ERR_CTRL_REG_PARA_TX_EMPTY_Msk
#define INTR_ERR_CTRL_REG_PARA_TX_TH_Pos                     (17U)
#define INTR_ERR_CTRL_REG_PARA_TX_TH_Msk                     (0x1U << INTR_ERR_CTRL_REG_PARA_TX_TH_Pos)
#define INTR_ERR_CTRL_REG_PARA_TX_TH                         INTR_ERR_CTRL_REG_PARA_TX_TH_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR1_Pos                       (18U)
#define INTR_ERR_CTRL_REG_RX_ERROR1_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR1_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR1                           INTR_ERR_CTRL_REG_RX_ERROR1_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR2_Pos                       (19U)
#define INTR_ERR_CTRL_REG_RX_ERROR2_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR2_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR2                           INTR_ERR_CTRL_REG_RX_ERROR2_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR3_Pos                       (20U)
#define INTR_ERR_CTRL_REG_RX_ERROR3_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR3_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR3                           INTR_ERR_CTRL_REG_RX_ERROR3_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR4_Pos                       (21U)
#define INTR_ERR_CTRL_REG_RX_ERROR4_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR4_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR4                           INTR_ERR_CTRL_REG_RX_ERROR4_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR5_Pos                       (22U)
#define INTR_ERR_CTRL_REG_RX_ERROR5_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR5_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR5                           INTR_ERR_CTRL_REG_RX_ERROR5_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR6_Pos                       (23U)
#define INTR_ERR_CTRL_REG_RX_ERROR6_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR6_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR6                           INTR_ERR_CTRL_REG_RX_ERROR6_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR7_Pos                       (24U)
#define INTR_ERR_CTRL_REG_RX_ERROR7_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR7_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR7                           INTR_ERR_CTRL_REG_RX_ERROR7_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR8_Pos                       (25U)
#define INTR_ERR_CTRL_REG_RX_ERROR8_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR8_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR8                           INTR_ERR_CTRL_REG_RX_ERROR8_Msk
#define INTR_ERR_CTRL_REG_TX_ERROR8_Pos                       (26U)
#define INTR_ERR_CTRL_REG_TX_ERROR8_Msk                       (0x1U << INTR_ERR_CTRL_REG_TX_ERROR8_Pos)
#define INTR_ERR_CTRL_REG_TX_ERROR8                           INTR_ERR_CTRL_REG_TX_ERROR8_Msk
/* INTR_ERR_STA _REG  ,offset：0x230 */
#define INTR_ERR_STA_REG_RX_FULL1_Pos                       (0U)
#define INTR_ERR_STA_REG_RX_FULL1_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL1_Pos)
#define INTR_ERR_STA_REG_RX_FULL1                           INTR_ERR_STA_REG_RX_FULL1_Msk
#define INTR_ERR_STA_REG_RX_FULL2_Pos                       (1U)
#define INTR_ERR_STA_REG_RX_FULL2_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL2_Pos)
#define INTR_ERR_STA_REG_RX_FULL2                           INTR_ERR_STA_REG_RX_FULL2_Msk
#define INTR_ERR_STA_REG_RX_FULL3_Pos                       (2U)
#define INTR_ERR_STA_REG_RX_FULL3_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL3_Pos)
#define INTR_ERR_STA_REG_RX_FULL3                           INTR_ERR_STA_REG_RX_FULL3_Msk
#define INTR_ERR_STA_REG_RX_FULL4_Pos                       (3U)
#define INTR_ERR_STA_REG_RX_FULL4_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL4_Pos)
#define INTR_ERR_STA_REG_RX_FULL4                           INTR_ERR_STA_REG_RX_FULL4_Msk
#define INTR_ERR_STA_REG_RX_FULL5_Pos                       (4U)
#define INTR_ERR_STA_REG_RX_FULL5_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL5_Pos)
#define INTR_ERR_STA_REG_RX_FULL5                           INTR_ERR_STA_REG_RX_FULL5_Msk
#define INTR_ERR_STA_REG_RX_FULL6_Pos                       (5U)
#define INTR_ERR_STA_REG_RX_FULL6_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL6_Pos)
#define INTR_ERR_STA_REG_RX_FULL6                           INTR_ERR_STA_REG_RX_FULL6_Msk
#define INTR_ERR_STA_REG_RX_FULL7_Pos                       (6U)
#define INTR_ERR_STA_REG_RX_FULL7_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL7_Pos)
#define INTR_ERR_STA_REG_RX_FULL7                           INTR_ERR_STA_REG_RX_FULL7_Msk
#define INTR_ERR_STA_REG_RX_FULL8_Pos                       (7U)
#define INTR_ERR_STA_REG_RX_FULL8_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL8_Pos)
#define INTR_ERR_STA_REG_RX_FULL8                           INTR_ERR_STA_REG_RX_FULL8_Msk
#define INTR_ERR_STA_REG_RX_TH1_Pos                         (8U)
#define INTR_ERR_STA_REG_RX_TH1_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH1_Pos)
#define INTR_ERR_STA_REG_RX_TH1                             INTR_ERR_STA_REG_RX_TH1_Msk
#define INTR_ERR_STA_REG_RX_TH2_Pos                         (9U)
#define INTR_ERR_STA_REG_RX_TH2_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH2_Pos)
#define INTR_ERR_STA_REG_RX_TH2                             INTR_ERR_STA_REG_RX_TH2_Msk
#define INTR_ERR_STA_REG_RX_TH3_Pos                         (10U)
#define INTR_ERR_STA_REG_RX_TH3_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH3_Pos)
#define INTR_ERR_STA_REG_RX_TH3                             INTR_ERR_STA_REG_RX_TH3_Msk
#define INTR_ERR_STA_REG_RX_TH4_Pos                         (11U)
#define INTR_ERR_STA_REG_RX_TH4_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH4_Pos)
#define INTR_ERR_STA_REG_RX_TH4                             INTR_ERR_STA_REG_RX_TH4_Msk
#define INTR_ERR_STA_REG_RX_TH5_Pos                         (12U)
#define INTR_ERR_STA_REG_RX_TH5_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH5_Pos)
#define INTR_ERR_STA_REG_RX_TH5                             INTR_ERR_STA_REG_RX_TH5_Msk
#define INTR_ERR_STA_REG_RX_TH6_Pos                         (13U)
#define INTR_ERR_STA_REG_RX_TH6_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH6_Pos)
#define INTR_ERR_STA_REG_RX_TH6                             INTR_ERR_STA_REG_RX_TH6_Msk
#define INTR_ERR_STA_REG_RX_TH7_Pos                         (14U)
#define INTR_ERR_STA_REG_RX_TH7_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH7_Pos)
#define INTR_ERR_STA_REG_RX_TH7                             INTR_ERR_STA_REG_RX_TH7_Msk
#define INTR_ERR_STA_REG_RX_TH8_Pos                         (15U)
#define INTR_ERR_STA_REG_RX_TH8_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH8_Pos)
#define INTR_ERR_STA_REG_RX_TH8                             INTR_ERR_STA_REG_RX_TH8_Msk
#define INTR_ERR_STA_REG_PARA_TX_EMPTY_Pos                  (16U)
#define INTR_ERR_STA_REG_PARA_TX_EMPTY_Msk                  (0x1U << INTR_ERR_STA_REG_PARA_TX_EMPTY_Pos)
#define INTR_ERR_STA_REG_PARA_TX_EMPTY                      INTR_ERR_STA_REG_PARA_TX_EMPTY_Msk
#define INTR_ERR_STA_REG_PARA_TX_TH_Pos                     (17U)
#define INTR_ERR_STA_REG_PARA_TX_TH_Msk                     (0x1U << INTR_ERR_STA_REG_PARA_TX_TH_Pos)
#define INTR_ERR_STA_REG_PARA_TX_TH                         INTR_ERR_STA_REG_PARA_TX_TH_Msk
#define INTR_ERR_STA_REG_RX_ERROR1_Pos                       (18U)
#define INTR_ERR_STA_REG_RX_ERROR1_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR1_Pos)
#define INTR_ERR_STA_REG_RX_ERROR1                           INTR_ERR_STA_REG_RX_ERROR1_Msk
#define INTR_ERR_STA_REG_RX_ERROR2_Pos                       (19U)
#define INTR_ERR_STA_REG_RX_ERROR2_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR2_Pos)
#define INTR_ERR_STA_REG_RX_ERROR2                           INTR_ERR_STA_REG_RX_ERROR2_Msk
#define INTR_ERR_STA_REG_RX_ERROR3_Pos                       (20U)
#define INTR_ERR_STA_REG_RX_ERROR3_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR3_Pos)
#define INTR_ERR_STA_REG_RX_ERROR3                           INTR_ERR_STA_REG_RX_ERROR3_Msk
#define INTR_ERR_STA_REG_RX_ERROR4_Pos                       (21U)
#define INTR_ERR_STA_REG_RX_ERROR4_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR4_Pos)
#define INTR_ERR_STA_REG_RX_ERROR4                           INTR_ERR_STA_REG_RX_ERROR4_Msk
#define INTR_ERR_STA_REG_RX_ERROR5_Pos                       (22U)
#define INTR_ERR_STA_REG_RX_ERROR5_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR5_Pos)
#define INTR_ERR_STA_REG_RX_ERROR5                           INTR_ERR_STA_REG_RX_ERROR5_Msk
#define INTR_ERR_STA_REG_RX_ERROR6_Pos                       (23U)
#define INTR_ERR_STA_REG_RX_ERROR6_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR6_Pos)
#define INTR_ERR_STA_REG_RX_ERROR6                           INTR_ERR_STA_REG_RX_ERROR6_Msk
#define INTR_ERR_STA_REG_RX_ERROR7_Pos                       (24U)
#define INTR_ERR_STA_REG_RX_ERROR7_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR7_Pos)
#define INTR_ERR_STA_REG_RX_ERROR7                           INTR_ERR_STA_REG_RX_ERROR7_Msk
#define INTR_ERR_STA_REG_RX_ERROR8_Pos                       (25U)
#define INTR_ERR_STA_REG_RX_ERROR8_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR8_Pos)
#define INTR_ERR_STA_REG_RX_ERROR8                           INTR_ERR_STA_REG_RX_ERROR8_Msk
#define INTR_ERR_STA_REG_TX_ERROR8_Pos                       (26U)
#define INTR_ERR_STA_REG_TX_ERROR8_Msk                       (0x1U << INTR_ERR_STA_REG_TX_ERROR8_Pos)
#define INTR_ERR_STA_REG_TX_ERROR8                           INTR_ERR_STA_REG_TX_ERROR8_Msk

/* PATH_EN_REG        ,offset：0x234 */
#define PATH_EN_REG_ADC1_Pos                       (0U)
#define PATH_EN_REG_ADC1_Msk                       (0x1U << PATH_EN_REG_ADC1_Pos)
#define PATH_EN_REG_ADC1                           PATH_EN_REG_ADC1_Msk
#define PATH_EN_REG_ADC2_Pos                       (1U)
#define PATH_EN_REG_ADC2_Msk                       (0x1U << PATH_EN_REG_ADC2_Pos)
#define PATH_EN_REG_ADC2                           PATH_EN_REG_ADC2_Msk
#define PATH_EN_REG_ADC3_Pos                       (2U)
#define PATH_EN_REG_ADC3_Msk                       (0x1U << PATH_EN_REG_ADC3_Pos)
#define PATH_EN_REG_ADC3                           PATH_EN_REG_ADC3_Msk
#define PATH_EN_REG_ADC4_Pos                       (3U)
#define PATH_EN_REG_ADC4_Msk                       (0x1U << PATH_EN_REG_ADC4_Pos)
#define PATH_EN_REG_ADC4                           PATH_EN_REG_ADC4_Msk
#define PATH_EN_REG_ADC5_Pos                       (4U)
#define PATH_EN_REG_ADC5_Msk                       (0x1U << PATH_EN_REG_ADC5_Pos)
#define PATH_EN_REG_ADC5                           PATH_EN_REG_ADC5_Msk
#define PATH_EN_REG_ADC6_Pos                       (5U)
#define PATH_EN_REG_ADC6_Msk                       (0x1U << PATH_EN_REG_ADC6_Pos)
#define PATH_EN_REG_ADC6                           PATH_EN_REG_ADC6_Msk
#define PATH_EN_REG_ADC7_Pos                       (6U)
#define PATH_EN_REG_ADC7_Msk                       (0x1U << PATH_EN_REG_ADC7_Pos)
#define PATH_EN_REG_ADC7                           PATH_EN_REG_ADC7_Msk
#define PATH_EN_REG_ADC8_Pos                       (7U)
#define PATH_EN_REG_ADC8_Msk                       (0x1U << PATH_EN_REG_ADC8_Pos)
#define PATH_EN_REG_ADC8                           PATH_EN_REG_ADC8_Msk
#define PATH_EN_REG_DAC_Pos                        (8U)
#define PATH_EN_REG_DAC_Msk                        (0x1U << PATH_EN_REG_DAC_Pos)
#define PATH_EN_REG_DAC                            PATH_EN_REG_DAC_Msk
/* INTR_ERR_CLR_REG   ,offset：0x238 */
#define INTR_ERR_CLR_REG_RX_FULL1_Pos                       (0U)
#define INTR_ERR_CLR_REG_RX_FULL1_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL1_Pos)
#define INTR_ERR_CLR_REG_RX_FULL1                           INTR_ERR_CLR_REG_RX_FULL1_Msk
#define INTR_ERR_CLR_REG_RX_FULL2_Pos                       (1U)
#define INTR_ERR_CLR_REG_RX_FULL2_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL2_Pos)
#define INTR_ERR_CLR_REG_RX_FULL2                           INTR_ERR_CLR_REG_RX_FULL2_Msk
#define INTR_ERR_CLR_REG_RX_FULL3_Pos                       (2U)
#define INTR_ERR_CLR_REG_RX_FULL3_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL3_Pos)
#define INTR_ERR_CLR_REG_RX_FULL3                           INTR_ERR_CLR_REG_RX_FULL3_Msk
#define INTR_ERR_CLR_REG_RX_FULL4_Pos                       (3U)
#define INTR_ERR_CLR_REG_RX_FULL4_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL4_Pos)
#define INTR_ERR_CLR_REG_RX_FULL4                           INTR_ERR_CLR_REG_RX_FULL4_Msk
#define INTR_ERR_CLR_REG_RX_FULL5_Pos                       (4U)
#define INTR_ERR_CLR_REG_RX_FULL5_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL5_Pos)
#define INTR_ERR_CLR_REG_RX_FULL5                           INTR_ERR_CLR_REG_RX_FULL5_Msk
#define INTR_ERR_CLR_REG_RX_FULL6_Pos                       (5U)
#define INTR_ERR_CLR_REG_RX_FULL6_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL6_Pos)
#define INTR_ERR_CLR_REG_RX_FULL6                           INTR_ERR_CLR_REG_RX_FULL6_Msk
#define INTR_ERR_CLR_REG_RX_FULL7_Pos                       (6U)
#define INTR_ERR_CLR_REG_RX_FULL7_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL7_Pos)
#define INTR_ERR_CLR_REG_RX_FULL7                           INTR_ERR_CLR_REG_RX_FULL7_Msk
#define INTR_ERR_CLR_REG_RX_FULL8_Pos                       (7U)
#define INTR_ERR_CLR_REG_RX_FULL8_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL8_Pos)
#define INTR_ERR_CLR_REG_RX_FULL8                           INTR_ERR_CLR_REG_RX_FULL8_Msk
#define INTR_ERR_CLR_REG_RX_TH1_Pos                         (8U)
#define INTR_ERR_CLR_REG_RX_TH1_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH1_Pos)
#define INTR_ERR_CLR_REG_RX_TH1                             INTR_ERR_CLR_REG_RX_TH1_Msk
#define INTR_ERR_CLR_REG_RX_TH2_Pos                         (9U)
#define INTR_ERR_CLR_REG_RX_TH2_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH2_Pos)
#define INTR_ERR_CLR_REG_RX_TH2                             INTR_ERR_CLR_REG_RX_TH2_Msk
#define INTR_ERR_CLR_REG_RX_TH3_Pos                         (10U)
#define INTR_ERR_CLR_REG_RX_TH3_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH3_Pos)
#define INTR_ERR_CLR_REG_RX_TH3                             INTR_ERR_CLR_REG_RX_TH3_Msk
#define INTR_ERR_CLR_REG_RX_TH4_Pos                         (11U)
#define INTR_ERR_CLR_REG_RX_TH4_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH4_Pos)
#define INTR_ERR_CLR_REG_RX_TH4                             INTR_ERR_CLR_REG_RX_TH4_Msk
#define INTR_ERR_CLR_REG_RX_TH5_Pos                         (12U)
#define INTR_ERR_CLR_REG_RX_TH5_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH5_Pos)
#define INTR_ERR_CLR_REG_RX_TH5                             INTR_ERR_CLR_REG_RX_TH5_Msk
#define INTR_ERR_CLR_REG_RX_TH6_Pos                         (13U)
#define INTR_ERR_CLR_REG_RX_TH6_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH6_Pos)
#define INTR_ERR_CLR_REG_RX_TH6                             INTR_ERR_CLR_REG_RX_TH6_Msk
#define INTR_ERR_CLR_REG_RX_TH7_Pos                         (14U)
#define INTR_ERR_CLR_REG_RX_TH7_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH7_Pos)
#define INTR_ERR_CLR_REG_RX_TH7                             INTR_ERR_CLR_REG_RX_TH7_Msk
#define INTR_ERR_CLR_REG_RX_TH8_Pos                         (15U)
#define INTR_ERR_CLR_REG_RX_TH8_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH8_Pos)
#define INTR_ERR_CLR_REG_RX_TH8                             INTR_ERR_CLR_REG_RX_TH8_Msk
#define INTR_ERR_CLR_REG_PARA_TX_EMPTY_Pos                  (16U)
#define INTR_ERR_CLR_REG_PARA_TX_EMPTY_Msk                  (0x1U << INTR_ERR_CLR_REG_PARA_TX_EMPTY_Pos)
#define INTR_ERR_CLR_REG_PARA_TX_EMPTY                      INTR_ERR_CLR_REG_PARA_TX_EMPTY_Msk
#define INTR_ERR_CLR_REG_PARA_TX_TH_Pos                     (17U)
#define INTR_ERR_CLR_REG_PARA_TX_TH_Msk                     (0x1U << INTR_ERR_CLR_REG_PARA_TX_TH_Pos)
#define INTR_ERR_CLR_REG_PARA_TX_TH                         INTR_ERR_CLR_REG_PARA_TX_TH_Msk
#define INTR_ERR_CLR_REG_RX_ERROR1_Pos                       (18U)
#define INTR_ERR_CLR_REG_RX_ERROR1_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR1_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR1                           INTR_ERR_CLR_REG_RX_ERROR1_Msk
#define INTR_ERR_CLR_REG_RX_ERROR2_Pos                       (19U)
#define INTR_ERR_CLR_REG_RX_ERROR2_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR2_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR2                           INTR_ERR_CLR_REG_RX_ERROR2_Msk
#define INTR_ERR_CLR_REG_RX_ERROR3_Pos                       (20U)
#define INTR_ERR_CLR_REG_RX_ERROR3_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR3_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR3                           INTR_ERR_CLR_REG_RX_ERROR3_Msk
#define INTR_ERR_CLR_REG_RX_ERROR4_Pos                       (21U)
#define INTR_ERR_CLR_REG_RX_ERROR4_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR4_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR4                           INTR_ERR_CLR_REG_RX_ERROR4_Msk
#define INTR_ERR_CLR_REG_RX_ERROR5_Pos                       (22U)
#define INTR_ERR_CLR_REG_RX_ERROR5_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR5_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR5                           INTR_ERR_CLR_REG_RX_ERROR5_Msk
#define INTR_ERR_CLR_REG_RX_ERROR6_Pos                       (23U)
#define INTR_ERR_CLR_REG_RX_ERROR6_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR6_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR6                           INTR_ERR_CLR_REG_RX_ERROR6_Msk
#define INTR_ERR_CLR_REG_RX_ERROR7_Pos                       (24U)
#define INTR_ERR_CLR_REG_RX_ERROR7_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR7_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR7                           INTR_ERR_CLR_REG_RX_ERROR7_Msk
#define INTR_ERR_CLR_REG_RX_ERROR8_Pos                       (25U)
#define INTR_ERR_CLR_REG_RX_ERROR8_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR8_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR8                           INTR_ERR_CLR_REG_RX_ERROR8_Msk
#define INTR_ERR_CLR_REG_TX_ERROR8_Pos                       (26U)
#define INTR_ERR_CLR_REG_TX_ERROR8_Msk                       (0x1U << INTR_ERR_CLR_REG_TX_ERROR8_Pos)
#define INTR_ERR_CLR_REG_TX_ERROR8                           INTR_ERR_CLR_REG_TX_ERROR8_Msk

/* COD_MASTER_REG     ,offset：0x23C */
#define CODE_MASTER_REG_MASTER_Pos                       (0U)
#define CODE_MASTER_REG_MASTER_Msk                       (0x1U << CODE_MASTER_REG_MASTER_Pos)
#define CODE_MASTER_REG_MASTER                           CODE_MASTER_REG_MASTER_Msk
/* I2C_ADDR_REG       ,offset：0x240 */
#define I2C_ADDR_REG_I2C_ADDR_Pos                       (0U)
#define I2C_ADDR_REG_I2C_ADDR_Msk                       (0x7FU << I2C_ADDR_REG_I2C_ADDR_Pos)
#define I2C_ADDR_REG_I2C_ADDR                           I2C_ADDR_REG_I2C_ADDR_Msk
/* TRAN_CTRL_REG      ,offset：0x244 */
#define TRAN_CTRL_REG_DMA_REQ_Pos                       (0U)
#define TRAN_CTRL_REG_DMA_REQ_Msk                       (0x1U << TRAN_CTRL_REG_DMA_REQ_Pos)
#define TRAN_CTRL_REG_DMA_REQ                           TRAN_CTRL_REG_DMA_REQ_Msk
/* SAMPLING_CTRL_REG  ,offset：0x248 */
#define SAMPLING_CTRL_REG_TX_BITS_Pos                   (0U)
#define SAMPLING_CTRL_REG_TX_BITS_Msk                   (0x1U << SAMPLING_CTRL_REG_TX_BITS_Pos)
#define SAMPLING_CTRL_REG_TX_24BITS                     SAMPLING_CTRL_REG_TX_BITS_Msk
#define SAMPLING_CTRL_REG_TX_16BITS                     (0U)
#define SAMPLING_CTRL_REG_RX_BITS_Pos                   (1U)
#define SAMPLING_CTRL_REG_RX_BITS_Msk                   (0x1U << SAMPLING_CTRL_REG_RX_BITS_Pos)
#define SAMPLING_CTRL_REG_RX_24BITS                     SAMPLING_CTRL_REG_RX_BITS_Msk
#define SAMPLING_CTRL_REG_RX_16BITS                     (0U)
/* TX_FIFO_CNT_REG    ,offset：0x24C */
#define TX_FIFO_CNT_REG_CNT_Pos                         (0U)
#define TX_FIFO_CNT_REG_CNT_Msk                         (0xFU << TX_FIFO_CNT_REG_CNT_Pos)
#define TX_FIFO_CNT_REG_CNT                             TX_FIFO_CNT_REG_CNT_Msk
#define TX_FIFO_CNT_REG_CNT_0                           (0U)
#define TX_FIFO_CNT_REG_CNT_1                           (1U)
#define TX_FIFO_CNT_REG_CNT_2                           (2U)
#define TX_FIFO_CNT_REG_CNT_3                           (3U)
#define TX_FIFO_CNT_REG_CNT_4                           (4U)
#define TX_FIFO_CNT_REG_CNT_5                           (5U)
#define TX_FIFO_CNT_REG_CNT_6                           (6U)
#define TX_FIFO_CNT_REG_CNT_7                           (7U)
#define TX_FIFO_CNT_REG_CNT_8                           (8U)

/* RX_FIFO_CNT_1_REG  ,offset：0x250 */
#define RX_FIFO_CNT1_REG_CNT1_Pos                         (0U)
#define RX_FIFO_CNT1_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT1_REG_CNT1_Pos)
#define RX_FIFO_CNT1_REG_CNT                             RX_FIFO_CNT1_REG_CNT_Msk
#define RX_FIFO_CNT1_REG_CNT_0                            (0U)
#define RX_FIFO_CNT1_REG_CNT_1                            (1U)
#define RX_FIFO_CNT1_REG_CNT_2                            (2U)
#define RX_FIFO_CNT1_REG_CNT_3                            (3U)
#define RX_FIFO_CNT1_REG_CNT_4                            (4U)
#define RX_FIFO_CNT1_REG_CNT_5                            (5U)
#define RX_FIFO_CNT1_REG_CNT_6                            (6U)
#define RX_FIFO_CNT1_REG_CNT_7                            (7U)
#define RX_FIFO_CNT1_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_2_REG  ,offset：0x254 */
#define RX_FIFO_CNT2_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT2_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT2_REG_CNT_Pos)
#define RX_FIFO_CNT2_REG_CNT                             RX_FIFO_CNT2_REG_CNT_Msk
#define RX_FIFO_CNT2_REG_CNT_0                            (0U)
#define RX_FIFO_CNT2_REG_CNT_1                            (1U)
#define RX_FIFO_CNT2_REG_CNT_2                            (2U)
#define RX_FIFO_CNT2_REG_CNT_3                            (3U)
#define RX_FIFO_CNT2_REG_CNT_4                            (4U)
#define RX_FIFO_CNT2_REG_CNT_5                            (5U)
#define RX_FIFO_CNT2_REG_CNT_6                            (6U)
#define RX_FIFO_CNT2_REG_CNT_7                            (7U)
#define RX_FIFO_CNT2_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_3_REG  ,offset：0x258 */
#define RX_FIFO_CNT3_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT3_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT3_REG_CNT_Pos)
#define RX_FIFO_CNT3_REG_CNT                             RX_FIFO_CNT3_REG_CNT_Msk
#define RX_FIFO_CNT3_REG_CNT_0                            (0U)
#define RX_FIFO_CNT3_REG_CNT_1                            (1U)
#define RX_FIFO_CNT3_REG_CNT_2                            (2U)
#define RX_FIFO_CNT3_REG_CNT_3                            (3U)
#define RX_FIFO_CNT3_REG_CNT_4                            (4U)
#define RX_FIFO_CNT3_REG_CNT_5                            (5U)
#define RX_FIFO_CNT3_REG_CNT_6                            (6U)
#define RX_FIFO_CNT3_REG_CNT_7                            (7U)
#define RX_FIFO_CNT3_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_4_REG  ,offset：0x25C */
#define RX_FIFO_CNT4_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT4_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT4_REG_CNT_Pos)
#define RX_FIFO_CNT4_REG_CNT                             RX_FIFO_CNT4_REG_CNT_Msk
#define RX_FIFO_CNT4_REG_CNT_0                            (0U)
#define RX_FIFO_CNT4_REG_CNT_1                            (1U)
#define RX_FIFO_CNT4_REG_CNT_2                            (2U)
#define RX_FIFO_CNT4_REG_CNT_3                            (3U)
#define RX_FIFO_CNT4_REG_CNT_4                            (4U)
#define RX_FIFO_CNT4_REG_CNT_5                            (5U)
#define RX_FIFO_CNT4_REG_CNT_6                            (6U)
#define RX_FIFO_CNT4_REG_CNT_7                            (7U)
#define RX_FIFO_CNT4_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_5_REG  ,offset：0x260 */
#define RX_FIFO_CNT5_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT5_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT5_REG_CNT_Pos)
#define RX_FIFO_CNT5_REG_CNT                             RX_FIFO_CNT5_REG_CNT_Msk
#define RX_FIFO_CNT5_REG_CNT_0                            (0U)
#define RX_FIFO_CNT5_REG_CNT_1                            (1U)
#define RX_FIFO_CNT5_REG_CNT_2                            (2U)
#define RX_FIFO_CNT5_REG_CNT_3                            (3U)
#define RX_FIFO_CNT5_REG_CNT_4                            (4U)
#define RX_FIFO_CNT5_REG_CNT_5                            (5U)
#define RX_FIFO_CNT5_REG_CNT_6                            (6U)
#define RX_FIFO_CNT5_REG_CNT_7                            (7U)
#define RX_FIFO_CNT5_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_6_REG  ,offset：0x264 */
#define RX_FIFO_CNT6_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT6_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT6_REG_CNT_Pos)
#define RX_FIFO_CNT6_REG_CNT                             RX_FIFO_CNT6_REG_CNT_Msk
#define RX_FIFO_CNT6_REG_CNT_0                            (0U)
#define RX_FIFO_CNT6_REG_CNT_1                            (1U)
#define RX_FIFO_CNT6_REG_CNT_2                            (2U)
#define RX_FIFO_CNT6_REG_CNT_3                            (3U)
#define RX_FIFO_CNT6_REG_CNT_4                            (4U)
#define RX_FIFO_CNT6_REG_CNT_5                            (5U)
#define RX_FIFO_CNT6_REG_CNT_6                            (6U)
#define RX_FIFO_CNT6_REG_CNT_7                            (7U)
#define RX_FIFO_CNT6_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_7_REG  ,offset：0x268 */
#define RX_FIFO_CNT7_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT7_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT7_REG_CNT_Pos)
#define RX_FIFO_CNT7_REG_CNT                             RX_FIFO_CNT7_REG_CNT_Msk
#define RX_FIFO_CNT7_REG_CNT_0                            (0U)
#define RX_FIFO_CNT7_REG_CNT_1                            (1U)
#define RX_FIFO_CNT7_REG_CNT_2                            (2U)
#define RX_FIFO_CNT7_REG_CNT_3                            (3U)
#define RX_FIFO_CNT7_REG_CNT_4                            (4U)
#define RX_FIFO_CNT7_REG_CNT_5                            (5U)
#define RX_FIFO_CNT7_REG_CNT_6                            (6U)
#define RX_FIFO_CNT7_REG_CNT_7                            (7U)
#define RX_FIFO_CNT7_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_8_REG  ,offset：0x26C */
#define RX_FIFO_CNT8_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT8_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT8_REG_CNT_Pos)
#define RX_FIFO_CNT8_REG_CNT                             RX_FIFO_CNT8_REG_CNT_Msk
#define RX_FIFO_CNT8_REG_CNT_0                            (0U)
#define RX_FIFO_CNT8_REG_CNT_1                            (1U)
#define RX_FIFO_CNT8_REG_CNT_2                            (2U)
#define RX_FIFO_CNT8_REG_CNT_3                            (3U)
#define RX_FIFO_CNT8_REG_CNT_4                            (4U)
#define RX_FIFO_CNT8_REG_CNT_5                            (5U)
#define RX_FIFO_CNT8_REG_CNT_6                            (6U)
#define RX_FIFO_CNT8_REG_CNT_7                            (7U)
#define RX_FIFO_CNT8_REG_CNT_8                            (8U)

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
    uint32_t RESERVED0[2]                        ; /* Offset: 0X30      (R)     RESERVED */
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
    uint32_t RESERVED1[2]                        ; /* Offset: 0XF8      (R)     RESERVED */
    __IOM uint32_t SR_WT                         ; /* Offset: 0x100     (R/W)  SR_WT               Register */
    __IOM uint32_t ICR_WT                        ; /* Offset: 0x104     (R/W)  ICR_WT              Register */
    __IOM uint32_t IMR_WT                        ; /* Offset: 0x108     (R/W)  IMR_WT              Register */
    __IOM uint32_t IFR_WT                        ; /* Offset: 0x10C     (R/W)  IFR_WT              Register */
    __IOM uint32_t CR_WT                         ; /* Offset: 0x110     (R/W)  CR_WT               Register */
    __IOM uint32_t CR_WT_2                       ; /* Offset: 0x114     (R/W)  CR_WT_2             Register */
    __IOM uint32_t CR_WT_3                       ; /* Offset: 0x118     (R/W)  CR_WT_3             Register */
    __IOM uint32_t CR_WT_4                       ; /* Offset: 0x11C     (R/W)  CR_WT_4             Register */
    uint32_t RESERVED2[24]                       ; /* Offset: 0X120     (R)     RESERVED */
    __IOM uint32_t CR_TR                         ; /* Offset: 0x180     (R/W)  CR_TR               Register */
    __IOM uint32_t DR_TR                         ; /* Offset: 0x184     (R/W)  DR_TR               Register */
    __IOM uint32_t SR_TR1                        ; /* Offset: 0x188     (R/W)  SR_TR1              Register */
    __IOM uint32_t SR_TR_SRCDAC                  ; /* Offset: 0x18C     (R/W)  SR_TR_SRCDAC        Register */
    uint32_t RESERVED3[28]                       ; /* Offset: 0X190     (R)     RESERVED */
    __IOM uint32_t  TRAN_SEL_REG;            /* 0x200        0x00000000        传输模式选择寄存器        */
    __IOM uint32_t  PARA_TX_FIFO_REG;        /* 0x204        0x00000000        DAC 路径数据发送寄存器    */
    __IOM uint32_t  RX_FIFO1_REG;            /* 0x208        0x00000000        第一路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO2_REG;            /* 0x20c        0x00000000        第二路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO3_REG;            /* 0x210        0x00000000        第三路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO4_REG;            /* 0x214        0x00000000        第四路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO5_REG;            /* 0x218        0x00000000        第五路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO6_REG;            /* 0x21C        0x00000000        第六路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO7_REG;            /* 0x220        0x00000000        第七路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO8_REG;            /* 0x224        0x00000000        第八路ADC 数据接收寄存器  */
    __IOM uint32_t  FIFO_TH_CTRL_REG;        /* 0x228        0x00000077        FIFO 门限控制寄存器       */
    __IOM uint32_t  INTR_ERR_CTRL_REG;       /* 0x22C        0x07FFFFFF        中断&错误控制寄存器       */
    __IOM uint32_t  INTR_ERR_STA_REG;        /* 0x230        0x00000000        中断&错误状态寄存器       */
    __IOM uint32_t  PATH_EN_REG;             /* 0x234        0x00000000        八路ADC 和DAC控制寄存器   */
    __IOM uint32_t  INTR_ERR_CLR_REG;        /* 0x238        0x00000000        中断&错误清除寄存器       */
    __IOM uint32_t  COD_MASTER_REG;          /* 0x23C        0x00000000        CODEC 主从控制寄存器      */
    __IOM uint32_t  I2C_ADDR_REG;            /* 0x240        0x00000014        CODEC I2C 地址寄存器      */
    __IOM uint32_t  TRAN_CTRL_REG;           /* 0x244        0x00000001        读取CODEC 数据方式选择寄存*/
    __IOM uint32_t  SAMPLING_CTRL_REG;       /* 0x248        0x00000000        采样率控制寄存器         */
    __IOM uint32_t  TX_FIFO_CNT_REG;         /* 0x24C        0x00000000        DAC FIFO 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT1_REG;        /* 0x250        0x00000000        ADC FIFO1 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT2_REG;        /* 0x254        0x00000000        ADC FIFO2 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT3_REG;        /* 0x258        0x00000000        ADC FIFO3 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT4_REG;        /* 0x25C        0x00000000        ADC FIFO4 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT5_REG;        /* 0x260        0x00000000        ADC FIFO5 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT6_REG;        /* 0x264        0x00000000        ADC FIFO6 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT7_REG;        /* 0x268        0x00000000        ADC FIFO7 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT8_REG;        /* 0x26C        0x00000000        ADC FIFO8 内部数据个数寄存*/
} wj_codec_regs_t;

#define CODES_BASE_OFFSET   0x200

static inline uint8_t  wj_codec_get_pon_ack_state(wj_codec_regs_t *addr)
{
    return (addr->SR & SR_REG_PON_ACK);
}

static inline uint8_t  wj_codec_get_vic_pending_state(wj_codec_regs_t *addr)
{
    return (addr->SR & SR_REG_IRQ_PENDING_Msk);
}

static inline uint8_t  wj_codec_get_jack_state(wj_codec_regs_t *addr)
{
    return (addr->SR & SR_REG_JACK_Msk);
}

static inline uint8_t  wj_codec_get_dac_locked_state(wj_codec_regs_t *addr)
{
    return (addr->SR & SR_REG_DAC_LOCKED_Msk);
}

static inline uint8_t  wj_codec_get_dac_unkonw_fs_state(wj_codec_regs_t *addr)
{
    return (addr->SR & SR_REG_DAC_UNKNOW_FS_Msk);
}

static inline uint8_t  wj_codec_get_dac_ramp_in_progress(wj_codec_regs_t *addr)
{
    return (addr->SR & SR2_REG_DAC_MUTE_INPROGRESS_Msk);
}

static inline uint8_t  wj_codec_get_dac_mute_state(wj_codec_regs_t *addr)
{
    return (addr->SR2 & SR2_REG_DAC_MUTE_INPROGRESS_Msk);
}

static inline uint8_t  wj_codec_get_adc78_mute_state(wj_codec_regs_t *addr)
{
    return (addr->SR3 & SR3_REG_ADC78_MUTE_INPROGRESS_Msk);
}

static inline uint8_t  wj_codec_get_adc56_mute_state(wj_codec_regs_t *addr)
{
    return (addr->SR3 & SR3_REG_ADC56_MUTE_INPROGRESS_Msk);
}

static inline uint8_t  wj_codec_get_adc34_mute_state(wj_codec_regs_t *addr)
{
    return (addr->SR3 & SR3_REG_ADC34_MUTE_INPROGRESS_Msk);
}

static inline void  wj_codec_set_int_form(wj_codec_regs_t *addr, uint8_t value)
{
    addr->ICR &= (~ICR_REG_INT_FORM_Msk);
    addr->ICR |= (value & ICR_REG_INT_FORM_Msk);
}

static inline uint8_t  wj_codec_get_int_form(wj_codec_regs_t *addr)
{
    return (addr->ICR & ICR_REG_INT_FORM_Msk);
}

static inline void  wj_codec_en_sc_mask(wj_codec_regs_t *addr)
{
    addr->IMR |= IMR_REG_SC_MASK_EN;
}

static inline void  wj_codec_dis_sc_mask(wj_codec_regs_t *addr)
{
    addr->IMR &= (~IMR_REG_SC_MASK_EN);
}

static inline uint8_t  wj_codec_get_sc_mask(wj_codec_regs_t *addr)
{
    return (addr->IMR & IMR_REG_SC_MASK_Msk);
}

static inline void  wj_codec_en_jack_mask(wj_codec_regs_t *addr)
{
    addr->IMR |= IMR_REG_JACK_MASK_EN;
}

static inline void  wj_codec_dis_jack_mask(wj_codec_regs_t *addr)
{
    addr->IMR &= (~IMR_REG_JACK_MASK_EN);
}

static inline uint8_t  wj_codec_get_jack_mask(wj_codec_regs_t *addr)
{
    return (addr->IMR & IMR_REG_JACK_MASK_Msk);
}


static inline void  wj_codec_en_adas_unloak_mask(wj_codec_regs_t *addr)
{
    addr->IMR |= IMR_REG_ADAS_UNLOCK_MASK_EN;
}

static inline void  wj_codec_dis_adas_unloak_mask(wj_codec_regs_t *addr)
{
    addr->IMR &= (~IMR_REG_ADAS_UNLOCK_MASK_EN);
}

static inline uint8_t  wj_codec_get_adas_unloak_mask(wj_codec_regs_t *addr)
{
    return (addr->IMR & IMR_REG_ADAS_UNLOCK_MASK_EN);
}

static inline void  wj_codec_dis_sc(wj_codec_regs_t *addr)
{
    addr->IFR &= (~IFR_REG_SC_EN);
}

static inline void  wj_codec_en_sc(wj_codec_regs_t *addr)
{
    addr->IFR |= IFR_REG_SC_EN;
}

static inline uint8_t  wj_codec_get_sc(wj_codec_regs_t *addr)
{
    return (addr->IFR & IFR_REG_SC_Msk);
}


static inline void  wj_codec_dis_jack_evnet(wj_codec_regs_t *addr)
{
    addr->IFR &= (~IFR_REG_JACK_EVENT_EN);
}

static inline void  wj_codec_en_jack_evnet(wj_codec_regs_t *addr)
{
    addr->IFR |= IFR_REG_JACK_EVENT_EN;
}

static inline uint8_t  wj_codec_get_jack_evnet(wj_codec_regs_t *addr)
{
    return (addr->IFR & IFR_REG_JACK_EVENT_Msk);
}

static inline void  wj_codec_dis_adas_unlock(wj_codec_regs_t *addr)
{
    addr->IFR &= (~IFR_REG_ADAS_UNLOCK_EN);
}

static inline void  wj_codec_en_adas_unlock(wj_codec_regs_t *addr)
{
    addr->IFR |= IFR_REG_ADAS_UNLOCK_EN;
}

static inline uint8_t  wj_codec_get_adas_unlock(wj_codec_regs_t *addr)
{
    return (addr->IFR & IFR_REG_ADAS_UNLOCK_Msk);
}

static inline void  wj_codec_dis_sb(wj_codec_regs_t *addr)
{
    addr->CR_VIC &= (~CR_VIC_REG_SB_EN);
}

static inline void  wj_codec_en_sb(wj_codec_regs_t *addr)
{
    addr->CR_VIC |= CR_VIC_REG_SB_EN;
}

static inline uint8_t  wj_codec_get_sb(wj_codec_regs_t *addr)
{
    return (addr->CR_VIC & CR_VIC_REG_SB_Msk);
}

static inline void  wj_codec_dis_sb_analog(wj_codec_regs_t *addr)
{
    addr->CR_VIC &= (~CR_VIC_REG_SB_EN);
}

static inline void  wj_codec_en_sb_analog(wj_codec_regs_t *addr)
{
    addr->CR_VIC |= CR_VIC_REG_SB_ANALOG_EN;
}

static inline uint8_t  wj_codec_get_sb_analog(wj_codec_regs_t *addr)
{
    return (addr->CR_VIC & CR_VIC_REG_SB_ANALOG_Msk);
}

static inline void  wj_codec_dis_sleep_analog(wj_codec_regs_t *addr)
{
    addr->CR_VIC &= (~CR_VIC_REG_SLEEP_ANALOG_EN);
}

static inline void  wj_codec_en_sleep_analog(wj_codec_regs_t *addr)
{
    addr->CR_VIC |= CR_VIC_REG_SLEEP_ANALOG_EN;
}

static inline uint8_t  wj_codec_get_sleep_analog(wj_codec_regs_t *addr)
{
    return (addr->CR_VIC & CR_VIC_REG_SLEEP_ANALOG_EN);
}

static inline void  wj_codec_set_crystall(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_CK &= ~CR_CK_REG_CRYSTAL_Msk;
    addr->CR_CK |= value & CR_CK_REG_CRYSTAL_Msk;
}

static inline uint8_t  wj_codec_get_crystall(wj_codec_regs_t *addr)
{
    return (addr->CR_CK & CR_CK_REG_CRYSTAL_Msk);
}

static inline void  wj_codec_set_dac_audioif(wj_codec_regs_t *addr, uint8_t value)
{
    addr->AICR_DAC &= ~AICR_DAC_REG_DAC_AUDIOIF_Msk;
    addr->AICR_DAC |= value & AICR_DAC_REG_DAC_AUDIOIF_Msk;
}

static inline uint8_t  wj_codec_get_dac_audioif(wj_codec_regs_t *addr)
{
    return (addr->AICR_DAC & AICR_DAC_REG_DAC_AUDIOIF_Msk);
}

static inline void  wj_codec_en_sb_aicr_dac(wj_codec_regs_t *addr, uint8_t value)
{
    addr->AICR_DAC &= ~AICR_DAC_REG_DAC_ADWL_Msk;
    addr->AICR_DAC |= value & AICR_DAC_REG_DAC_ADWL_Msk;
}

static inline uint8_t  wj_codec_get_dac_adwl(wj_codec_regs_t *addr)
{
    return (addr->AICR_DAC & AICR_DAC_REG_DAC_ADWL_Msk);
}

static inline void  wj_codec_set_dac_slave(wj_codec_regs_t *addr)
{
    addr->AICR_DAC |= AICR_DAC_REG_DAC_SLAVE_Msk;
}

static inline void  wj_codec_set_dac_master(wj_codec_regs_t *addr)
{
    addr->AICR_DAC &= ~AICR_DAC_REG_DAC_SLAVE_Msk;
}


static inline uint8_t  wj_codec_get_dac_slave(wj_codec_regs_t *addr)
{
    return (addr->AICR_DAC & AICR_DAC_REG_DAC_SLAVE_Msk);
}

static inline void  wj_codec_close_sb_aicr_dac(wj_codec_regs_t *addr)
{
    addr->AICR_DAC |= AICR_DAC_REG_SB_AICR_DAC_Msk;
}

static inline void  wj_codec_open_sb_aicr_dac(wj_codec_regs_t *addr)
{
    addr->AICR_DAC &= ~AICR_DAC_REG_SB_AICR_DAC_Msk;
}

static inline uint8_t  wj_codec_get_sb_aicr_dac(wj_codec_regs_t *addr)
{
    return (addr->AICR_DAC & AICR_DAC_REG_SB_AICR_DAC_Msk);
}

static inline void  wj_codec_set_adc_audioif(wj_codec_regs_t *addr, uint8_t value)
{
    addr->AICR_ADC &= ~AICR_ADC_REG_ADC_AUDIOIF_Msk;
    addr->AICR_ADC |= value & AICR_ADC_REG_ADC_AUDIOIF_Msk;
}

static inline uint8_t  wj_codec_get_adc_audioif(wj_codec_regs_t *addr)
{
    return (addr->AICR_ADC & AICR_ADC_REG_ADC_AUDIOIF_Msk);
}

static inline void  wj_codec_set_adc_adwl(wj_codec_regs_t *addr, uint8_t value)
{
    addr->AICR_ADC &= ~AICR_ADC_REG_ADC_ADWL_Msk;
    addr->AICR_ADC |= value & AICR_ADC_REG_ADC_ADWL_Msk;
}

static inline uint8_t  wj_codec_get_adc_adwl(wj_codec_regs_t *addr)
{
    return (addr->AICR_ADC & AICR_ADC_REG_ADC_ADWL_Msk);
}

static inline void  wj_codec_close_sb_aicr_adc12(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 |= AICR_ADC_2_REG_SB_AICR_ADC12_EN;
}

static inline void  wj_codec_open_sb_aicr_adc12(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 &= ~AICR_ADC_2_REG_SB_AICR_ADC12_EN;
}

static inline uint8_t  wj_codec_get_sb_aicr_adc12(wj_codec_regs_t *addr)
{
    return (addr->AICR_ADC_2 & AICR_ADC_2_REG_SB_AICR_ADC12_Msk);
}

static inline void  wj_codec_close_sb_aicr_adc34(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 |= AICR_ADC_2_REG_SB_AICR_ADC34_EN;
}

static inline void  wj_codec_open_sb_aicr_adc34(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 &= ~AICR_ADC_2_REG_SB_AICR_ADC34_EN;
}

static inline uint8_t  wj_codec_get_sb_aicr_adc34(wj_codec_regs_t *addr)
{
    return (addr->AICR_ADC_2 & AICR_ADC_2_REG_SB_AICR_ADC34_Msk);
}

static inline void  wj_codec_close_sb_aicr_adc56(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 |= AICR_ADC_2_REG_SB_AICR_ADC56_EN;
}

static inline void  wj_codec_open_sb_aicr_adc56(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 &= ~AICR_ADC_2_REG_SB_AICR_ADC56_EN;
}

static inline uint8_t  wj_codec_get_sb_aicr_adc56(wj_codec_regs_t *addr)
{
    return (addr->AICR_ADC_2 & AICR_ADC_2_REG_SB_AICR_ADC56_Msk);
}

static inline void  wj_codec_close_sb_aicr_adc78(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 |= AICR_ADC_2_REG_SB_AICR_ADC78_EN;
}

static inline void  wj_codec_open_sb_aicr_adc78(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 &= ~AICR_ADC_2_REG_SB_AICR_ADC78_EN;
}

static inline uint8_t  wj_codec_get_sb_aicr_adc78(wj_codec_regs_t *addr)
{
    return (addr->AICR_ADC_2 & AICR_ADC_2_REG_SB_AICR_ADC78_Msk);
}

static inline void  wj_codec_set_dac_freq(wj_codec_regs_t *addr, uint8_t value)
{
    addr->FCR_DAC &= ~FCR_DAC_REG_DAC_FREQ_Msk;
    addr->FCR_DAC |= value & FCR_DAC_REG_DAC_FREQ_Msk;
}

static inline uint8_t  wj_codec_get_dac_freq(wj_codec_regs_t *addr)
{
    return (addr->FCR_DAC & FCR_DAC_REG_DAC_FREQ_Msk);
}

static inline void  wj_codec_set_adc12_wnf(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_WNF &= ~CR_WNF_REG_ADC12_WNF_Msk;
    addr->CR_WNF |= value & CR_WNF_REG_ADC12_WNF_Msk;
}

static inline uint8_t  wj_codec_get_adc12_wnf(wj_codec_regs_t *addr)
{
    return (addr->CR_WNF & CR_WNF_REG_ADC12_WNF_Msk);
}

static inline void  wj_codec_set_adc34_wnf(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_WNF &= ~CR_WNF_REG_ADC34_WNF_Msk;
    addr->CR_WNF |= value & CR_WNF_REG_ADC34_WNF_Msk;
}

static inline uint8_t  wj_codec_get_adc34_wnf(wj_codec_regs_t *addr)
{
    return (addr->CR_WNF & CR_WNF_REG_ADC34_WNF_Msk);
}

static inline void  wj_codec_set_adc56_wnf(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_WNF &= ~CR_WNF_REG_ADC56_WNF_Msk;
    addr->CR_WNF |= value & CR_WNF_REG_ADC56_WNF_Msk;
}

static inline uint8_t  wj_codec_get_adc56_wnf(wj_codec_regs_t *addr)
{
    return (addr->CR_WNF & CR_WNF_REG_ADC56_WNF_Msk);
}

static inline void  wj_codec_set_adc78_wnf(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_WNF &= ~CR_WNF_REG_ADC78_WNF_Msk;
    addr->CR_WNF |= value & CR_WNF_REG_ADC78_WNF_Msk;
}

static inline uint8_t  wj_codec_get_adc78_wnf(wj_codec_regs_t *addr)
{
    return (addr->CR_WNF & CR_WNF_REG_ADC78_WNF_Msk);
}

static inline void  wj_codec_open_adc12_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC |= FCR_ADC_REG_ADC12_HPF_EN;
}

static inline void  wj_codec_close_adc12_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC &= ~FCR_ADC_REG_ADC12_HPF_EN;
}

static inline uint8_t  wj_codec_get_adc12_hpf(wj_codec_regs_t *addr)
{
    return (addr->FCR_ADC & FCR_ADC_REG_ADC12_HPF_EN_Msk);
}

static inline void  wj_codec_open_adc34_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC |= FCR_ADC_REG_ADC34_HPF_EN;
}

static inline void  wj_codec_close_adc34_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC &= ~FCR_ADC_REG_ADC34_HPF_EN;
}

static inline uint8_t  wj_codec_get_adc34_hpf(wj_codec_regs_t *addr)
{
    return (addr->FCR_ADC & FCR_ADC_REG_ADC34_HPF_EN_Msk);
}
static inline void  wj_codec_open_adc56_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC |= FCR_ADC_REG_ADC56_HPF_EN;
}

static inline void  wj_codec_close_adc56_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC &= ~FCR_ADC_REG_ADC56_HPF_EN;
}

static inline uint8_t  wj_codec_get_adc56_hpf(wj_codec_regs_t *addr)
{
    return (addr->FCR_ADC & FCR_ADC_REG_ADC56_HPF_EN_Msk);
}

static inline void  wj_codec_open_adc78_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC |= FCR_ADC_REG_ADC78_HPF_EN;
}

static inline void  wj_codec_close_adc78_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC &= ~FCR_ADC_REG_ADC78_HPF_EN;
}

static inline uint8_t  wj_codec_get_adc78_hpf(wj_codec_regs_t *addr)
{
    return (addr->FCR_ADC & FCR_ADC_REG_ADC78_HPF_EN_Msk);
}

static inline void  wj_codec_set_adc_freq(wj_codec_regs_t *addr, uint8_t value)
{
    addr->FCR_ADC &= ~FCR_ADC_REG_ADC_FREQ_Msk;
    addr->FCR_ADC |= value & FCR_ADC_REG_ADC_FREQ_Msk;
}

static inline uint8_t  wj_codec_get_adc_freq(wj_codec_regs_t *addr)
{
    return (addr->FCR_ADC & FCR_ADC_REG_ADC_FREQ_Msk);
}


static inline void  wj_codec_set_adc_dmic_sel2(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC12 &= ~CR_DMIC12_REG_ADC_DMIC_SEL2_Msk;
    addr->CR_DMIC12 |= value & CR_DMIC12_REG_ADC_DMIC_SEL2_Msk;
}

static inline uint8_t  wj_codec_get_adc_dmic_sel2(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC12 & CR_DMIC12_REG_ADC_DMIC_SEL2_Msk);
}

static inline void  wj_codec_set_adc_dmic_sel1(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC12 &= ~CR_DMIC12_REG_ADC_DMIC_SEL1_Msk;
    addr->CR_DMIC12 |= value & CR_DMIC12_REG_ADC_DMIC_SEL1_Msk;
}

static inline uint8_t  wj_codec_get_adc_dmic_sel1(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC12 & CR_DMIC12_REG_ADC_DMIC_SEL1_Msk);
}

static inline void  wj_codec_set_dmic_rate(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC12 &= ~CR_DMIC12_REG_ADC_DMIC_RATE_Msk;
    addr->CR_DMIC12 |= value & CR_DMIC12_REG_ADC_DMIC_RATE_Msk;
}

static inline uint8_t  wj_codec_get_dmic_rate(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC12 & CR_DMIC12_REG_ADC_DMIC_RATE_Msk);
}

static inline void  wj_codec_open_sb_dmic2(wj_codec_regs_t *addr)
{
    addr->CR_DMIC12 &= ~CR_DMIC12_REG_SB_DMIC2_EN;
}

static inline void  wj_codec_close_sb_dmic2(wj_codec_regs_t *addr)
{
    addr->CR_DMIC12 |= CR_DMIC12_REG_SB_DMIC2_EN;
}

static inline uint8_t  wj_codec_get_sb_dmic2(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC12 & CR_DMIC12_REG_SB_DMIC2_Msk);
}

static inline void  wj_codec_open_sb_dmic1(wj_codec_regs_t *addr)
{
    addr->CR_DMIC12 &= ~CR_DMIC12_REG_SB_DMIC1_EN;
}

static inline void  wj_codec_close_sb_dmic1(wj_codec_regs_t *addr)
{
    addr->CR_DMIC12 |= CR_DMIC12_REG_SB_DMIC1_EN;
}

static inline uint8_t  wj_codec_get_sb_dmic1(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC12 & CR_DMIC12_REG_SB_DMIC1_Msk);
}



static inline void  wj_codec_set_adc_dmic_sel6(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC56 &= ~CR_DMIC56_REG_ADC_DMIC_SEL6_Msk;
    addr->CR_DMIC56 |= value & CR_DMIC56_REG_ADC_DMIC_SEL6_Msk;
}

static inline uint8_t  wj_codec_get_adc_dmic_sel6(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC56 & CR_DMIC56_REG_ADC_DMIC_SEL6_Msk);
}

static inline void  wj_codec_set_adc_dmic_sel5(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC56 &= ~CR_DMIC56_REG_ADC_DMIC_SEL5_Msk;
    addr->CR_DMIC56 |= value & CR_DMIC56_REG_ADC_DMIC_SEL5_Msk;
}

static inline uint8_t  wj_codec_get_adc_dmic_sel5(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC56 & CR_DMIC56_REG_ADC_DMIC_SEL5_Msk);
}

static inline void  wj_codec_open_sb_dmic6(wj_codec_regs_t *addr)
{
    addr->CR_DMIC56 &= ~CR_DMIC56_REG_SB_DMIC6_EN;
}

static inline void  wj_codec_close_sb_dmic6(wj_codec_regs_t *addr)
{
    addr->CR_DMIC56 |= CR_DMIC56_REG_SB_DMIC6_EN;
}

static inline uint8_t  wj_codec_get_sb_dmic6(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC56 & CR_DMIC56_REG_SB_DMIC6_Msk);
}

static inline void  wj_codec_open_sb_dmic5(wj_codec_regs_t *addr)
{
    addr->CR_DMIC56 &= ~CR_DMIC56_REG_SB_DMIC5_EN;
}

static inline void  wj_codec_close_sb_dmic5(wj_codec_regs_t *addr)
{
    addr->CR_DMIC56 |= CR_DMIC56_REG_SB_DMIC5_EN;
}

static inline uint8_t  wj_codec_get_sb_dmic5(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC56 & CR_DMIC56_REG_SB_DMIC5_Msk);
}


static inline void  wj_codec_set_adc_dmic_sel8(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC78 &= ~CR_DMIC78_REG_ADC_DMIC_SEL8_Msk;
    addr->CR_DMIC78 |= value & CR_DMIC78_REG_ADC_DMIC_SEL8_Msk;
}

static inline uint8_t  wj_codec_get_adc_dmic_sel8(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC78 & CR_DMIC78_REG_ADC_DMIC_SEL8_Msk);
}

static inline void  wj_codec_set_adc_dmic_sel7(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC78 &= ~CR_DMIC78_REG_ADC_DMIC_SEL7_Msk;
    addr->CR_DMIC78 |= value & CR_DMIC78_REG_ADC_DMIC_SEL7_Msk;
}

static inline uint8_t  wj_codec_get_adc_dmic_sel7(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC78 & CR_DMIC78_REG_ADC_DMIC_SEL7_Msk);
}

static inline void  wj_codec_open_sb_dmic8(wj_codec_regs_t *addr)
{
    addr->CR_DMIC78 &= ~CR_DMIC78_REG_SB_DMIC8_EN;
}

static inline void  wj_codec_close_sb_dmic8(wj_codec_regs_t *addr)
{
    addr->CR_DMIC78 |= CR_DMIC78_REG_SB_DMIC8_EN;
}

static inline uint8_t  wj_codec_get_sb_dmic8(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC78 & CR_DMIC78_REG_SB_DMIC8_Msk);
}

static inline void  wj_codec_open_sb_dmic7(wj_codec_regs_t *addr)
{
    addr->CR_DMIC78 &= ~CR_DMIC78_REG_SB_DMIC7_EN;
}

static inline void  wj_codec_close_sb_dmic7(wj_codec_regs_t *addr)
{
    addr->CR_DMIC78 |= CR_DMIC78_REG_SB_DMIC7_EN;
}

static inline uint8_t  wj_codec_get_sb_dmic7(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC78 & CR_DMIC78_REG_SB_DMIC7_Msk);
}



static inline void  wj_codec_open_sb_hp(wj_codec_regs_t *addr)
{
    addr->CR_HP &= ~CR_HP_REG_SB_HP_OPEN_EN;
}

static inline void  wj_codec_close_sb_hp(wj_codec_regs_t *addr)
{
    addr->CR_HP |= CR_HP_REG_SB_HP_OPEN_EN;
}

static inline uint8_t  wj_codec_get_sb_hp(wj_codec_regs_t *addr)
{
    return (addr->CR_HP & CR_HP_REG_SB_HP_OPEN_Msk);
}

static inline void  wj_codec_open_sb_mute(wj_codec_regs_t *addr)
{
    addr->CR_HP &= ~CR_HP_REG_HP_MUTE_EN;
}

static inline void  wj_codec_close_sb_mute(wj_codec_regs_t *addr)
{
    addr->CR_HP |= CR_HP_REG_HP_MUTE_EN;
}

static inline uint8_t  wj_codec_get_sb_mute(wj_codec_regs_t *addr)
{
    return (addr->CR_HP & CR_HP_REG_HP_MUTE_Msk);
}


static inline void  wj_codec_set_gol(wj_codec_regs_t *addr, uint8_t value)
{
    addr->GCR_HPL &= ~GCR_HPL_REG_GOL_Msk;
    addr->GCR_HPL |= value & GCR_HPL_REG_GOL_Msk;
}

static inline uint8_t  wj_codec_get_gol(wj_codec_regs_t *addr)
{
    return (addr->GCR_HPL & GCR_HPL_REG_GOL_Msk);
}


static inline void  wj_codec_set_gor(wj_codec_regs_t *addr, uint8_t value)
{
    addr->GCR_HPR &= ~GCR_HPR_REG_GOR_Msk;
    addr->GCR_HPR |= value & GCR_HPR_REG_GOR_Msk;
}

static inline uint8_t  wj_codec_get_gor(wj_codec_regs_t *addr)
{
    return (addr->GCR_HPR & GCR_HPR_REG_GOR_Msk);
}

static inline void  wj_codec_open_sb_micbias1(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_MIC[index] &= ~CR_MIC1_REG_SB_MICBIAS1_CLOSE;
}

static inline void  wj_codec_close_sb_micbias1(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_MIC[index] |= CR_MIC1_REG_SB_MICBIAS1_CLOSE;
}

static inline uint8_t  wj_codec_get_sb_micbias1(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->CR_MIC[index] & CR_MIC1_REG_SB_MICBIAS1_CLOSE);
}


static inline void  wj_codec_set_differential(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_MIC[index] |= CR_MIC1_REG_SB_MICDIFF1_SINGLE;
}

static inline void  wj_codec_set_single_end(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_MIC[index] &= ~CR_MIC1_REG_SB_MICDIFF1_SINGLE;
}

static inline uint8_t  wj_codec_get_micdiff1(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->CR_MIC[index] & CR_MIC1_REG_SB_MICDIFF1_SINGLE);
}

static inline void  wj_codec_set_micbias1_1p8v(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_MIC[index] |= CR_MIC1_REG_SB_MICBIAS1_V_1P8;
}

static inline void  wj_codec_set_micbias1_1p5v(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_MIC[index] &= ~CR_MIC1_REG_SB_MICBIAS1_V_1P8;
}

static inline uint8_t  wj_codec_get_micbias1(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->CR_MIC[index] & CR_MIC1_REG_SB_MICBIAS1_V_Msk);
}


static inline void  wj_codec_set_analog_gain(wj_codec_regs_t *addr, uint8_t index, uint8_t value)
{
    addr->GCR_MIC[index << 1] &= ~(((index % 2 == 1) ? GCR_MIC_REG_GIM1_Msk : GCR_MIC_REG_GIM2_Msk));
    addr->GCR_MIC[index << 1] = (value << ((index % 2 == 1) ? GCR_MIC_REG_GIM1_Pos : GCR_MIC_REG_GIM2_Pos)) & (((index % 2 == 1) ? GCR_MIC_REG_GIM1_Msk : GCR_MIC_REG_GIM2_Msk));

}

static inline uint8_t  wj_codec_get_analog_gain(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->GCR_MIC[index << 1] & ((index % 2 == 1) ? GCR_MIC_REG_GIM1_Msk : GCR_MIC_REG_GIM2_Msk));
}


static inline void  wj_codec_open_dac(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_DAC &= ~CR_DAC_REG_SB_DAC_EN;
}

static inline void  wj_codec_close_dac(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_DAC |= CR_DAC_REG_SB_DAC_EN;
}

static inline uint8_t  wj_codec_get_dac(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->CR_DAC & CR_DAC_REG_SB_DAC_Msk);
}


static inline void  wj_codec_open_dac_left_right(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_DAC &= ~CR_DAC_REG_DAC_LEFT_ONLY_EN;
}

static inline void  wj_codec_close_dac_left(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_DAC |= CR_DAC_REG_DAC_LEFT_ONLY_EN;
}

static inline uint8_t  wj_codec_get_dac_state(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->CR_DAC & CR_DAC_REG_DAC_LEFT_ONLY_Msk);
}


static inline void  wj_codec_open_dac_soft_mute(wj_codec_regs_t *addr)
{
    addr->CR_DAC |= CR_DAC_REG_DAC_SOFT_MUTE_EN;
}

static inline void  wj_codec_close_dac_soft_mute(wj_codec_regs_t *addr)
{
    addr->CR_DAC &= ~CR_DAC_REG_DAC_SOFT_MUTE_EN;
}

static inline uint8_t  wj_codec_get_dac_soft_mute(wj_codec_regs_t *addr)
{
    return (addr->CR_DAC & CR_DAC_REG_DAC_SOFT_MUTE_Msk);
}

static inline void  wj_codec_open_adc_open(wj_codec_regs_t *addr, uint8_t index)
{
    if (index % 2 == 1) {
        addr->CR_ADC[index << 1] |= CR_ADC_REG_SB_ADC1_OPEN;
    } else {
        addr->CR_ADC[index << 1] |= CR_ADC_REG_SB_ADC2_OPEN;
    }
}

static inline void  wj_codec_close_adc_close(wj_codec_regs_t *addr, uint8_t index)
{

    if (index % 2 == 1) {
        addr->CR_ADC[index << 1] &= ~CR_ADC_REG_SB_ADC1_OPEN;
    } else {
        addr->CR_ADC[index << 1] |= CR_ADC_REG_SB_ADC2_OPEN;
    }
}

static inline uint8_t  wj_codec_get_adc_state(wj_codec_regs_t *addr, uint8_t index)
{

    if (index % 2 == 1) {
        addr->CR_ADC[index << 1] &= ~CR_ADC_REG_SB_ADC1_OPEN;
    } else {
        addr->CR_ADC[index << 1] |= CR_ADC_REG_SB_ADC2_OPEN;
    }

    return (addr->CR_ADC[index << 1] & ((index % 2 == 1) ? CR_ADC_REG_SB_ADC1_Msk : CR_ADC_REG_SB_ADC2_Msk));
}


static inline void  wj_codec_set_lrgod_dependence(wj_codec_regs_t *addr)
{
    addr->GCR_DACL &= ~GCR_DACL_REG_LRGOD_TOGETHER;
}

static inline void  wj_codec_set_lrgod_together(wj_codec_regs_t *addr)
{
    addr->GCR_DACL |= GCR_DACL_REG_LRGOD_TOGETHER;
}

static inline uint8_t  wj_codec_get_lrgod(wj_codec_regs_t *addr)
{
    return (addr->GCR_DACL & GCR_DACL_REG_LRGOD_Msk);
}

static inline void  wj_codec_set_godl(wj_codec_regs_t *addr, uint32_t value)
{
    addr->GCR_DACL &= ~ GCR_DACL_REG_CODL_Msk ;
    addr->GCR_DACL |= GCR_DACL_REG_CODL_Msk & value;
}

static inline uint8_t  wj_codec_get_godl(wj_codec_regs_t *addr)
{
    return (addr->GCR_DACL & GCR_DACL_REG_CODL_Msk);
}


static inline void  wj_codec_set_godr(wj_codec_regs_t *addr, uint8_t index, uint8_t value)
{
    addr->GCR_ADC[index] &= ~ GCR_ADC1_REG_GID1_Msk ;
    addr->GCR_ADC[index] |= GCR_ADC1_REG_GID1_Msk & value;
}

static inline uint8_t  wj_codec_get_godr(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->GCR_ADC[index] & GCR_ADC1_REG_GID1_Msk);
}

static inline void  wj_codec_set_lrgid_together(wj_codec_regs_t *addr, uint8_t index)
{
    addr->GCR_ADC[index] &= ~ GCR_ADC1_REG_LRGID_Msk ;

}

static inline void  wj_codec_set_lrgid_independence(wj_codec_regs_t *addr, uint8_t index)
{
    addr->GCR_ADC[index] &= ~ GCR_ADC1_REG_LRGID_Msk ;
}
static inline uint8_t  wj_codec_get_lrgid(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->GCR_ADC[index] & GCR_ADC1_REG_LRGID_Msk);
}

static inline void  wj_codec_set_gomixl(wj_codec_regs_t *addr, uint8_t value)
{
    addr->GCR_MIXDACL &= ~ GCR_MIXDACL_REG_GOMIXL_Msk ;
    addr->GCR_MIXDACL |= GCR_MIXDACL_REG_GOMIXL_Msk & value;
}

static inline uint8_t  wj_codec_get_gomixl(wj_codec_regs_t *addr)
{
    return (addr->GCR_MIXDACL & GCR_MIXDACL_REG_GOMIXL_Msk);
}

static inline void  wj_codec_set_lrgomix_together(wj_codec_regs_t *addr)
{
    addr->GCR_MIXDACL &= ~ GCR_MIXDACL_REG_LRGOMIX_Msk ;

}

static inline void  wj_codec_set_lrgomix_independence(wj_codec_regs_t *addr)
{
    addr->GCR_MIXDACL &= ~ GCR_MIXDACL_REG_LRGOMIX_Msk ;
}

static inline uint8_t  wj_codec_get_lrgomix(wj_codec_regs_t *addr)
{
    return (addr->GCR_MIXDACL & GCR_MIXDACL_REG_LRGOMIX_Msk);
}


static inline void  wj_codec_set_gomixr(wj_codec_regs_t *addr, uint8_t value)
{
    addr->GCR_MIXDACR &= ~ GCR_MIXDACR_REG_GOMIXR_Msk ;
    addr->GCR_MIXDACR |= GCR_MIXDACR_REG_GOMIXR_Msk & value;
}

static inline uint8_t  wj_codec_get_gomixr(wj_codec_regs_t *addr)
{
    return (addr->GCR_MIXDACR & GCR_MIXDACR_REG_GOMIXR_Msk);
}

static inline void  wj_codec_set_gimixl(wj_codec_regs_t *addr, uint8_t value)
{
    addr->GCR_MIXADCL &= ~ GCR_MIXADCL_REG_GIMIXL_Msk ;
    addr->GCR_MIXADCL |=  GCR_MIXADCL_REG_GIMIXL_Msk & value;
}

static inline uint8_t  wj_codec_get_gimixl(wj_codec_regs_t *addr)
{
    return (addr->GCR_MIXADCL & GCR_MIXADCL_REG_GIMIXL_Msk);
}

static inline void  wj_codec_set_lrgimix_together(wj_codec_regs_t *addr)
{
    addr->GCR_MIXADCL &= ~ GCR_MIXADCL_REG_LRGIMIX_Msk ;

}

static inline void  wj_codec_set_lrgimix_independence(wj_codec_regs_t *addr)
{
    addr->GCR_MIXADCL &= ~ GCR_MIXADCL_REG_LRGIMIX_Msk ;
}

static inline uint8_t  wj_codec_get_lrgimix(wj_codec_regs_t *addr)
{
    return (addr->GCR_MIXADCL & GCR_MIXADCL_REG_LRGIMIX_Msk);
}

static inline void  wj_codec_set_gimixr(wj_codec_regs_t *addr, uint8_t value)
{
    addr->GCR_MIXADCR &= ~ GCR_MIXADCR_REG_GIMIXR_Msk ;
    addr->GCR_MIXADCR |= GCR_MIXADCR_REG_GIMIXR_Msk & value;
}

static inline uint8_t  wj_codec_get_gimixr(wj_codec_regs_t *addr)
{
    return (addr->GCR_MIXADCR & GCR_MIXADCR_REG_GIMIXR_Msk);
}


static inline void  wj_codec_set_wt_int_form(wj_codec_regs_t *addr, uint8_t value)
{
    addr->ICR_WT &= ~ ICR_WT_REG_WT_INT_FORM_Msk ;
    addr->ICR_WT |= ICR_WT_REG_WT_INT_FORM_Msk & value;
}

static inline uint8_t  wj_codec_get_wt_int_form(wj_codec_regs_t *addr)
{
    return (addr->ICR_WT & ICR_WT_REG_WT_INT_FORM_Msk);
}


static inline void  wj_codec_en_whisper(wj_codec_regs_t *addr)
{
    addr->CR_WT |= CR_WT_REG_WT_EN;
}

static inline void  wj_codec_dis_whisper(wj_codec_regs_t *addr)
{
    addr->CR_WT &= ~CR_WT_REG_WT_EN;
}

static inline uint8_t  wj_codec_get_whisper(wj_codec_regs_t *addr)
{
    return (addr->CR_WT & CR_WT_REG_WT_EN_Msk);
}

static inline void  wj_codec_en_wt_only(wj_codec_regs_t *addr)
{
    addr->CR_WT |= CR_WT_REG_WT_ONLY_EN;
}

static inline void  wj_codec_dis_wt_only(wj_codec_regs_t *addr)
{
    addr->CR_WT &= ~CR_WT_REG_WT_ONLY_EN;
}

static inline uint8_t  wj_codec_get_wt_only(wj_codec_regs_t *addr)
{
    return (addr->CR_WT & CR_WT_REG_WT_ONLY_Msk);
}

static inline void  wj_codec_en_wt_restart(wj_codec_regs_t *addr)
{
    addr->CR_WT |= CR_WT_REG_WT_RESTART_EN;
}

static inline void  wj_codec_dis_wt_restart(wj_codec_regs_t *addr)
{
    addr->CR_WT &= ~CR_WT_REG_WT_RESTART_EN;
}

static inline uint8_t  wj_codec_get_wt_restart(wj_codec_regs_t *addr)
{
    return (addr->CR_WT & CR_WT_REG_WT_RESTART_Msk);
}


static inline void  wj_codec_en_wt_adc1(wj_codec_regs_t *addr)
{
    addr->CR_WT_2 |= CR_WT2_REG_WT_ADC_SEL_ADC1;
}

static inline void  wj_codec_dis_wt_digit(wj_codec_regs_t *addr)
{
    addr->CR_WT_2 &= ~CR_WT2_REG_WT_ADC_SEL_ADC1;
}

static inline uint8_t  wj_codec_get_wt(wj_codec_regs_t *addr)
{
    return (addr->CR_WT_2 & CR_WT2_REG_WT_ADC_SEL_Msk);
}

static inline void  wj_codec_en_wt_zcd(wj_codec_regs_t *addr)
{
    addr->CR_WT_2 |= CR_WT2_REG_ZCD_EN;
}

static inline void  wj_codec_dis_wt_zcd(wj_codec_regs_t *addr)
{
    addr->CR_WT_2 &= ~CR_WT2_REG_ZCD_EN;
}

static inline uint8_t  wj_codec_get_zcd(wj_codec_regs_t *addr)
{
    return (addr->CR_WT_2 & CR_WT2_REG_ZCD_Msk);
}

static inline void  wj_codec_set_dmic_freq(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_WT_2 &= ~ CR_WT2_REG_WT_DMIC_FREQ_Msk ;
    addr->CR_WT_2 |= CR_WT2_REG_WT_DMIC_FREQ_Msk & value;
}

static inline uint8_t  wj_codec_get_dmic_freq(wj_codec_regs_t *addr)
{
    return (addr->CR_WT_2 & CR_WT2_REG_WT_DMIC_FREQ_Msk);
}

static inline void  wj_codec_set_power_sens(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_WT_3 &= ~ CR_WT_3_REG_WT_POWER_SENS_Msk ;
    addr->CR_WT_3 |= CR_WT_3_REG_WT_POWER_SENS_Msk & value;
}

static inline uint8_t  wj_codec_get_power_sens(wj_codec_regs_t *addr)
{
    return (addr->CR_WT_3 & CR_WT_3_REG_WT_POWER_SENS_Msk);
}



static inline void  wj_codec_set_i2c_mode(wj_codec_regs_t *addr)
{
    addr->TRAN_SEL_REG &= (~TRAN_SEL_REG_SI2C_Msk);
    addr->TRAN_SEL_REG = TRAN_SEL_REG_I2C;
}

static inline void  wj_codec_set_uc_mode(wj_codec_regs_t *addr)
{
    addr->TRAN_SEL_REG &= (~TRAN_SEL_REG_SI2C_Msk);
    addr->TRAN_SEL_REG = TRAN_SEL_REG_uC;
}

static inline uint32_t  wj_codec_get_trans_mode(wj_codec_regs_t *addr)
{
    return (addr->TRAN_SEL_REG & TRAN_SEL_REG_SI2C_Msk);
}

static inline void  wj_codec_set_tx_fifo_ex(wj_codec_regs_t *addr, uint32_t left_value, uint32_t right_value)
{
    addr->PARA_TX_FIFO_REG = (right_value << TRAN_TX_FIFOREG_RIGHT_DATA_Pos) | (left_value << TRAN_TX_FIFOREG_LEFT_DATA_Pos);
}

static inline void  wj_codec_set_tx_fifo(wj_codec_regs_t *addr, uint32_t value)
{
    addr->PARA_TX_FIFO_REG = value;
}
static inline uint32_t  wj_codec_get_rx_fifo(wj_codec_regs_t *addr, uint32_t index)
{
    return *(&addr->RX_FIFO1_REG + index);
}
static inline uint32_t  wj_codec_get_rx_fifo1(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO1_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo2(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO2_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo3(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO3_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo4(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO4_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo5(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO5_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo6(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO6_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo7(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO7_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo8(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO8_REG);
}

static inline void  wj_codec_set_rx_threshold(wj_codec_regs_t *addr, uint32_t value)
{
    addr->FIFO_TH_CTRL_REG &= (~FIFO_TH_CTRL_RX_Msk);
    addr->FIFO_TH_CTRL_REG = value & FIFO_TH_CTRL_RX_Msk;
}
static inline void  wj_codec_set_tx_threshold(wj_codec_regs_t *addr, uint32_t value)
{
    addr->FIFO_TH_CTRL_REG &= (~FIFO_TH_CTRL_TX_Msk);
    addr->FIFO_TH_CTRL_REG = value & FIFO_TH_CTRL_TX_Msk;
}

static inline uint32_t  wj_codec_get_rx_threshold(wj_codec_regs_t *addr)
{
    return (addr->FIFO_TH_CTRL_REG & FIFO_TH_CTRL_RX_Msk);
}
static inline uint32_t  wj_codec_get_tx_threshold(wj_codec_regs_t *addr)
{
    return (addr->FIFO_TH_CTRL_REG & FIFO_TH_CTRL_TX_Msk);
}

static inline uint32_t  wj_codec_get_error_mask(wj_codec_regs_t *addr)
{
    return addr->INTR_ERR_CTRL_REG;
}
static inline void  wj_codec_set_error_mask(wj_codec_regs_t *addr, uint32_t value)
{
    addr->INTR_ERR_CTRL_REG = value;
}

static inline uint32_t  wj_codec_get_error_state(wj_codec_regs_t *addr)
{
    return addr->INTR_ERR_STA_REG;
}

static inline void  wj_codec_set_channel_en(wj_codec_regs_t *addr, uint32_t value)
{
    addr->PATH_EN_REG = value;
}
static inline void  wj_codec_enable_adc(wj_codec_regs_t *addr, uint32_t value)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC1 + value;
}
static inline void  wj_codec_enable_adc1(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC1;
}
static inline void  wj_codec_enable_adc2(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC2;
}
static inline void  wj_codec_enable_adc3(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC3;
}
static inline void  wj_codec_enable_adc4(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC4;
}
static inline void  wj_codec_enable_adc5(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC5;
}
static inline void  wj_codec_enable_adc6(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC6;
}
static inline void  wj_codec_enable_adc7(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC7;
}
static inline void  wj_codec_enable_adc8(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC8;
}
static inline void  wj_codec_enable_dac(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_DAC;
}

static inline void  wj_codec_disable_adc1(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC1;
}
static inline void  wj_codec_disable_adc2(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC2;
}
static inline void  wj_codec_disable_adc3(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC3;
}
static inline void  wj_codec_disable_adc4(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC4;
}
static inline void  wj_codec_disable_adc5(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC5;
}
static inline void  wj_codec_disable_adc6(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC6;
}
static inline void  wj_codec_disable_adc7(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC7;
}
static inline void  wj_codec_disable_adc8(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC8;
}
static inline void  wj_codec_disable_dac(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_DAC;
}

static inline void  wj_codec_clr_intr(wj_codec_regs_t *addr, uint32_t value)
{
    addr->INTR_ERR_CLR_REG = value;
}

// static inline void  wj_codec_set_cod_master(wj_codec_regs_t *addr)
// {
//     addr->COD_MASTER_REG |= CODE_MASTER_REG_MASTER;
// }

// static inline void  wj_codec_set_cod_slave(wj_codec_regs_t *addr)
// {
//     addr->COD_MASTER_REG &= ~CODE_MASTER_REG_MASTER;
// }

static inline void  wj_codec_set_i2c_addr(wj_codec_regs_t *addr, uint32_t value)
{
    addr->I2C_ADDR_REG = value & I2C_ADDR_REG_I2C_ADDR_Msk;
}

static inline void  wj_codec_enable_dma_req(wj_codec_regs_t *addr)
{
    addr->TRAN_CTRL_REG |= TRAN_CTRL_REG_DMA_REQ;
}

static inline void  wj_codec_disable_dma_req(wj_codec_regs_t *addr)
{
    addr->TRAN_CTRL_REG &= ~TRAN_CTRL_REG_DMA_REQ;
}

static inline void  wj_codec_set_adc_24bits(wj_codec_regs_t *addr)
{
    addr->SAMPLING_CTRL_REG |= SAMPLING_CTRL_REG_RX_24BITS;
}

static inline void  wj_codec_set_adc_16bits(wj_codec_regs_t *addr)
{
    addr->SAMPLING_CTRL_REG &= ~SAMPLING_CTRL_REG_RX_24BITS;
}

static inline void  wj_codec_set_dac_24bits(wj_codec_regs_t *addr)
{
    addr->SAMPLING_CTRL_REG |= SAMPLING_CTRL_REG_TX_24BITS;
}

static inline void  wj_codec_set_dac_16bits(wj_codec_regs_t *addr)
{
    addr->SAMPLING_CTRL_REG &= ~SAMPLING_CTRL_REG_TX_24BITS;
}


static inline uint32_t  wj_codec_get_tx_fifo_cnt(wj_codec_regs_t *addr)
{
    return addr->TX_FIFO_CNT_REG;
}


static inline uint32_t  wj_codec_get_rx_fifo_cnt1(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT1_REG;
}



static inline uint32_t  wj_codec_get_rx_fifo_cnt2(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT2_REG;
}

static inline uint32_t  wj_codec_get_rx_fifo_cnt3(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT3_REG;
}

static inline uint32_t  wj_codec_get_rx_fifo_cnt4(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT4_REG;
}

static inline uint32_t  wj_codec_get_rx_fifo_cnt5(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT5_REG;
}

static inline uint32_t  wj_codec_get_rx_fifo_cnt6(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT6_REG;
}

static inline uint32_t  wj_codec_get_rx_fifo_cnt7(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT7_REG;
}

static inline uint32_t  wj_codec_get_rx_fifo_cnt8(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT8_REG;
}
#ifdef __cplusplus
}
#endif

#endif /* _IPGOAL_CODEDC_LL_H_ */
/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     codec_ll.h
 * @brief    header file for wj codec ll driver
 * @version  V1.0
 * @date     13. July 2020
 ******************************************************************************/

#ifndef __CODEC_LL_H__
#define __CODEC_LL_H__

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * CODEC register bit definitions
 */
/* Offset: 0x00      (R)    SR                  Register */
#define SR_REG_PON_ACK_Pos                       (7U)
#define SR_REG_PON_ACK_Msk                       (0x1U << SR_REG_PON_ACK_Pos)
#define SR_REG_PON_ACK                           SR_REG_PON_ACK_Msk

#define SR_REG_IRQ_PENDING_Pos                   (6U)
#define SR_REG_IRQ_PENDING_Msk                   (0x1U << SR_REG_IRQ_PENDING_Pos)
#define SR_REG_IRQ_PENDING                       SR_REG_IRQ_PENDING_Msk

#define SR_REG_JACK_Pos                          (5U)
#define SR_REG_JACK_Msk                          (0x1U << SR_REG_JACK_Pos)
#define SR_REG_JACK                              SR_REG_JACK_Msk

#define SR_REG_DAC_LOCKED_Pos                    (4U)
#define SR_REG_DAC_LOCKED_Msk                    (0x1U << SR_REG_DAC_LOCKED_Pos)
#define SR_REG_DAC_LOCKED                        SR_REG_DAC_LOCKED_Msk

#define SR_REG_DAC_UNKNOW_FS_Pos                 (2U)
#define SR_REG_DAC_UNKNOW_FS_Msk                 (0x1U << SR_REG_DAC_UNKNOW_FS_Pos)
#define SR_REG_DAC_UNKNOW_FS                     SR_REG_DAC_UNKNOW_FS_Msk

#define SR_REG_DAC_RAMP_IN_PROGRESS_Pos          (0U)
#define SR_REG_DAC_RAMP_IN_PROGRESS_Msk          (0x1U << SR_REG_DAC_RAMP_IN_PROGRESS_Pos)
#define SR_REG_DAC_RAMP_IN_PROGRESS              SR_REG_DAC_RAMP_IN_PROGRESS_Msk
/* Offset: 0x04      (R)    SR2                 Register */
#define SR2_REG_DAC_MUTE_INPROGRESS_Pos          (7U)
#define SR2_REG_DAC_MUTE_INPROGRESS_Msk          (0x1U << SR2_REG_DAC_MUTE_INPROGRESS_Pos)
#define SR2_REG_DAC_MUTE_INPROGRESS              SR2_REG_DAC_MUTE_INPROGRESS_Msk

#define SR2_REG_ADC12_MUTE_INPROGRESS_Pos        (6U)
#define SR2_REG_ADC12_MUTE_INPROGRESS_Msk        (0x1U << SR2_REG_ADC12_MUTE_INPROGRESS_Pos)
#define SR2_REG_ADC12_MUTE_INPROGRESS            SR2_REG_ADC12_MUTE_INPROGRESS_Msk

/* Offset: 0x08      (R)    SR3                 Register */
#define SR3_REG_ADC34_MUTE_INPROGRESS_Pos        (0U)
#define SR3_REG_ADC34_MUTE_INPROGRESS_Msk        (0x1U << SR3_REG_ADC34_MUTE_INPROGRESS_Pos)
#define SR3_REG_ADC34_MUTE_INPROGRESS            SR3_REG_ADC34_MUTE_INPROGRESS_Msk

#define SR3_REG_ADC56_MUTE_INPROGRESS_Pos        (1U)
#define SR3_REG_ADC56_MUTE_INPROGRESS_Msk        (0x1U << SR3_REG_ADC56_MUTE_INPROGRESS_Pos)
#define SR3_REG_ADC56_MUTE_INPROGRESS            SR3_REG_ADC56_MUTE_INPROGRESS_Msk

#define SR3_REG_ADC78_MUTE_INPROGRESS_Pos        (2U)
#define SR3_REG_ADC78_MUTE_INPROGRESS_Msk        (0x1U << SR3_REG_ADC78_MUTE_INPROGRESS_Pos)
#define SR3_REG_ADC78_MUTE_INPROGRESS            SR3_REG_ADC78_MUTE_INPROGRESS_Msk
/* Offset: 0x0C      (R/W)  ICR                 Register */
#define ICR_REG_INT_FORM_Pos            (6U)
#define ICR_REG_INT_FORM_Msk            (0x3U << ICR_REG_INT_FORM_Pos)
#define ICR_REG_INT_FORM                ICR_REG_INT_FORM_Msk
#define ICR_REG_INT_FORM_IRQ_HIGH       (0U<<ICR_REG_INT_FORM_Pos)
#define ICR_REG_INT_FORM_IRQ_LOW        (1U<<ICR_REG_INT_FORM_Pos)
#define ICR_REG_INT_FORM_8MCCLK_HIGH    (2U<<ICR_REG_INT_FORM_Pos)
#define ICR_REG_INT_FORM_8MCCLK_LOW     (3U<<ICR_REG_INT_FORM_Pos)

/* Offset: 0x10      (R/W)  IMR                 Register */
#define IMR_REG_SC_MASK_Pos             (4U)
#define IMR_REG_SC_MASK_Msk             (0x1U << IMR_REG_SC_MASK_Pos)
#define IMR_REG_SC_MASK_EN               IMR_REG_SC_MASK_Msk

#define IMR_REG_JACK_MASK_Pos            (5U)
#define IMR_REG_JACK_MASK_Msk            (0x1U << IMR_REG_JACK_MASK_Pos)
#define IMR_REG_JACK_MASK_EN             IMR_REG_JACK_MASK_Msk

#define IMR_REG_ADAS_UNLOCK_MASK_Pos     (7U)
#define IMR_REG_ADAS_UNLOCK_MASK_Msk     (0x1U << IMR_REG_ADAS_UNLOCK_MASK_Pos)
#define IMR_REG_ADAS_UNLOCK_MASK_EN       IMR_REG_ADAS_UNLOCK_MASK_Msk
/* Offset: 0x14      (R/W)  IFR                 Register */
#define IFR_REG_SC_Pos                   (4U)
#define IFR_REG_SC_Msk                   (0x1U << IFR_REG_SC_Pos)
#define IFR_REG_SC_EN                       IFR_REG_SC_Msk

#define IFR_REG_JACK_EVENT_Pos            (5U)
#define IFR_REG_JACK_EVENT_Msk            (0x1U << IFR_REG_JACK_EVENT_Pos)
#define IFR_REG_JACK_EVENT_EN                IFR_REG_JACK_EVENT_Msk

#define IFR_REG_ADAS_UNLOCK_Pos          (7U)
#define IFR_REG_ADAS_UNLOCK_Msk          (0x1U << IFR_REG_ADAS_UNLOCK_Pos)
#define IFR_REG_ADAS_UNLOCK_EN               IFR_REG_ADAS_UNLOCK_Msk
/* Offset: 0x18      (R/W)  CR_VIC              Register */
#define CR_VIC_REG_SLEEP_ANALOG_Pos      (2U)
#define CR_VIC_REG_SLEEP_ANALOG_Msk      (0x1U << CR_VIC_REG_SLEEP_ANALOG_Pos)
#define CR_VIC_REG_SLEEP_ANALOG_EN           CR_VIC_REG_SLEEP_ANALOG_Msk

#define CR_VIC_REG_SB_ANALOG_Pos        (1U)
#define CR_VIC_REG_SB_ANALOG_Msk        (0x1U << CR_VIC_REG_SB_ANALOG_Pos)
#define CR_VIC_REG_SB_ANALOG_EN             CR_VIC_REG_SB_ANALOG_Msk

#define CR_VIC_REG_SB_Pos               (0U)
#define CR_VIC_REG_SB_Msk               (0x1U << CR_VIC_REG_SB_Pos)
#define CR_VIC_REG_SB_EN                   CR_VIC_REG_SB_Msk
/* Offset: 0x1C      (R/W)  CR_CK               Register */
#define CR_CK_REG_CRYSTAL_Pos               (0U)
#define CR_CK_REG_CRYSTAL_Msk               (0x3U << CR_CK_REG_CRYSTAL_Pos)
#define CR_CK_REG_CRYSTAL                   CR_CK_REG_CRYSTAL_Msk
#define CR_CK_REG_CRYSTAL_12MHZ              (0x0U << CR_CK_REG_CRYSTAL_Pos)
#define CR_CK_REG_CRYSTAL_13MHZ              (0x1U << CR_CK_REG_CRYSTAL_Pos)

/* Offset: 0x20      (R/W)  AICR_DAC            Register */
#define AICR_DAC_REG_DAC_AUDIOIF_Pos              (0U)
#define AICR_DAC_REG_DAC_AUDIOIF_Msk              (0x3U << AICR_DAC_REG_DAC_AUDIOIF_Pos)
#define AICR_DAC_REG_DAC_AUDIOIF_PARALLEL         (0x0U << CR_CK_REG_CRYSTAL_Pos)
#define AICR_DAC_REG_DAC_AUDIOIF_ALIGN_LEFT       (0x1U << CR_CK_REG_CRYSTAL_Pos)
#define AICR_DAC_REG_DAC_AUDIOIF_DSP              (0x2U << CR_CK_REG_CRYSTAL_Pos)
#define AICR_DAC_REG_DAC_AUDIOIF_I2S              (0x3U << CR_CK_REG_CRYSTAL_Pos)

#define AICR_DAC_REG_SB_AICR_DAC_Pos              (4U)
#define AICR_DAC_REG_SB_AICR_DAC_Msk              (0x1U << AICR_DAC_REG_SB_AICR_DAC_Pos)
#define AICR_DAC_REG_SB_AICR_DAC_EN               AICR_DAC_REG_SB_AICR_DAC_Msk

#define AICR_DAC_REG_DAC_SLAVE_Pos              (5U)
#define AICR_DAC_REG_DAC_SLAVE_Msk              (0x1U << AICR_DAC_REG_DAC_SLAVE_Pos)
#define AICR_DAC_REG_DAC_SLAVE_EN               AICR_DAC_REG_DAC_SLAVE_Msk

#define AICR_DAC_REG_DAC_ADWL_Pos              (6U)
#define AICR_DAC_REG_DAC_ADWL_Msk              (0x3U << AICR_DAC_REG_DAC_ADWL_Pos)
#define AICR_DAC_REG_DAC_ADWL_16               (0x0U << AICR_DAC_REG_DAC_ADWL_Pos)
#define AICR_DAC_REG_DAC_ADWL_18               (0x1U << AICR_DAC_REG_DAC_ADWL_Pos)
#define AICR_DAC_REG_DAC_ADWL_20               (0x2U << AICR_DAC_REG_DAC_ADWL_Pos)
#define AICR_DAC_REG_DAC_ADWL_24               (0x3U << AICR_DAC_REG_DAC_ADWL_Pos)

/* Offset: 0x24      (R/W)  AICR_ADC            Register */
#define AICR_ADC_REG_ADC_ADWL_Pos              (6U)
#define AICR_ADC_REG_ADC_ADWL_Msk              (0x3U << AICR_ADC_REG_ADC_ADWL_Pos)
#define AICR_ADC_REG_ADC_ADWL_16               (0x0U << AICR_ADC_REG_ADC_ADWL_Pos)
#define AICR_ADC_REG_ADC_ADWL_18               (0x1U << AICR_ADC_REG_ADC_ADWL_Pos)
#define AICR_ADC_REG_ADC_ADWL_20               (0x2U << AICR_ADC_REG_ADC_ADWL_Pos)
#define AICR_ADC_REG_ADC_ADWL_24               (0x3U << AICR_ADC_REG_ADC_ADWL_Pos)

#define AICR_ADC_REG_ADC_AUDIOIF_Pos           (0U)
#define AICR_ADC_REG_ADC_AUDIOIF_Msk           (0x3U << AICR_ADC_REG_ADC_AUDIOIF_Pos)
#define AICR_ADC_REG_ADC_AUDIOIF_PARALLEL      (0x0U << AICR_ADC_REG_ADC_AUDIOIF_Pos)
#define AICR_ADC_REG_ADC_AUDIOIF_RESERVED      (0x1U << AICR_ADC_REG_ADC_AUDIOIF_Pos)
#define AICR_ADC_REG_ADC_AUDIOIF_DSP           (0x2U << AICR_ADC_REG_ADC_AUDIOIF_Pos)
#define AICR_ADC_REG_ADC_AUDIOIF_I2S           (0x3U << AICR_ADC_REG_ADC_AUDIOIF_Pos)
/* Offset: 0x28      (R/W)  AICR_ADC_2          Register */
#define AICR_ADC_2_REG_SB_AICR_ADC12_Pos       (0U)
#define AICR_ADC_2_REG_SB_AICR_ADC12_Msk       (0x1U << AICR_ADC_2_REG_SB_AICR_ADC12_Pos)
#define AICR_ADC_2_REG_SB_AICR_ADC12_EN        AICR_ADC_2_REG_SB_AICR_ADC12_Msk

#define AICR_ADC_2_REG_SB_AICR_ADC34_Pos       (1U)
#define AICR_ADC_2_REG_SB_AICR_ADC34_Msk       (0x1U << AICR_ADC_2_REG_SB_AICR_ADC34_Pos)
#define AICR_ADC_2_REG_SB_AICR_ADC34_EN        AICR_ADC_2_REG_SB_AICR_ADC34_Msk

#define AICR_ADC_2_REG_SB_AICR_ADC56_Pos       (2U)
#define AICR_ADC_2_REG_SB_AICR_ADC56_Msk       (0x1U << AICR_ADC_2_REG_SB_AICR_ADC56_Pos)
#define AICR_ADC_2_REG_SB_AICR_ADC56_EN        AICR_ADC_2_REG_SB_AICR_ADC56_Msk

#define AICR_ADC_2_REG_SB_AICR_ADC78_Pos       (3U)
#define AICR_ADC_2_REG_SB_AICR_ADC78_Msk       (0x1U << AICR_ADC_2_REG_SB_AICR_ADC78_Pos)
#define AICR_ADC_2_REG_SB_AICR_ADC78_EN        AICR_ADC_2_REG_SB_AICR_ADC78_Msk
/* Offset: 0x2C      (R/W)  FCR_DAC             Register */
#define FCR_DAC_REG_DAC_FREQ_Pos           (0U)
#define FCR_DAC_REG_DAC_FREQ_Msk           (0xFU << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_8K            (0x0U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_11P025K       (0x1U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_12K           (0x2U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_16K           (0x3U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_22P05K        (0x4U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_24K           (0x5U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_32K           (0x6U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_44P1K         (0x7U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_48K           (0x8U << FCR_DAC_REG_DAC_FREQ_Pos)
#define FCR_DAC_REG_DAC_FREQ_96K           (0x10U << FCR_DAC_REG_DAC_FREQ_Pos)
/* Offset: 0X30      (R)     RESERVED */
/* Offset: 0x38      (R/W)  CR_WNF              Register */
#define CR_WNF_REG_ADC12_WNF_Pos        (0U)
#define CR_WNF_REG_ADC12_WNF_Msk        (0x3U <<CR_WNF_REG_ADC12_WNF_Pos)
#define CR_WNF_REG_ADC12_WNF_CLOSE      (0x0U <<CR_WNF_REG_ADC12_WNF_Pos)
#define CR_WNF_REG_ADC12_WNF_MODE1      (0x1U <<CR_WNF_REG_ADC12_WNF_Pos)
#define CR_WNF_REG_ADC12_WNF_MODE2      (0x2U <<CR_WNF_REG_ADC12_WNF_Pos)
#define CR_WNF_REG_ADC12_WNF_MODE3      (0x3U <<CR_WNF_REG_ADC12_WNF_Pos)

#define CR_WNF_REG_ADC34_WNF_Pos        (2U)
#define CR_WNF_REG_ADC34_WNF_Msk        (0x3U <<CR_WNF_REG_ADC34_WNF_Pos)
#define CR_WNF_REG_ADC34_WNF_CLOSE      (0x0U <<CR_WNF_REG_ADC34_WNF_Pos)
#define CR_WNF_REG_ADC34_WNF_MODE1      (0x1U <<CR_WNF_REG_ADC34_WNF_Pos)
#define CR_WNF_REG_ADC34_WNF_MODE2      (0x2U <<CR_WNF_REG_ADC34_WNF_Pos)
#define CR_WNF_REG_ADC34_WNF_MODE3      (0x3U <<CR_WNF_REG_ADC34_WNF_Pos)

#define CR_WNF_REG_ADC56_WNF_Pos        (4U)
#define CR_WNF_REG_ADC56_WNF_Msk        (0x3U <<CR_WNF_REG_ADC56_WNF_Pos)
#define CR_WNF_REG_ADC56_WNF_CLOSE      (0x0U <<CR_WNF_REG_ADC56_WNF_Pos)
#define CR_WNF_REG_ADC56_WNF_MODE1      (0x1U <<CR_WNF_REG_ADC56_WNF_Pos)
#define CR_WNF_REG_ADC56_WNF_MODE2      (0x2U <<CR_WNF_REG_ADC56_WNF_Pos)
#define CR_WNF_REG_ADC56_WNF_MODE3      (0x3U <<CR_WNF_REG_ADC56_WNF_Pos)

#define CR_WNF_REG_ADC78_WNF_Pos        (6U)
#define CR_WNF_REG_ADC78_WNF_Msk        (0x3U <<CR_WNF_REG_ADC78_WNF_Pos)
#define CR_WNF_REG_ADC78_WNF_CLOSE      (0x0U <<CR_WNF_REG_ADC78_WNF_Pos)
#define CR_WNF_REG_ADC78_WNF_MODE1      (0x1U <<CR_WNF_REG_ADC78_WNF_Pos)
#define CR_WNF_REG_ADC78_WNF_MODE2      (0x2U <<CR_WNF_REG_ADC78_WNF_Pos)
#define CR_WNF_REG_ADC78_WNF_MODE3      (0x3U <<CR_WNF_REG_ADC78_WNF_Pos)
/* Offset: 0x3C      (R/W)  FCR_ADC             Register */
#define FCR_ADC_REG_ADC_FREQ_Pos           (0U)
#define FCR_ADC_REG_ADC_FREQ_Msk           (0xFU << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_8K            (0x0U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_11P025K       (0x1U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_12K           (0x2U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_16K           (0x3U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_22P05K        (0x4U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_24K           (0x5U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_32K           (0x6U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_44P1K         (0x7U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_48K           (0x8U << FCR_ADC_REG_ADC_FREQ_Pos)
#define FCR_ADC_REG_ADC_FREQ_96K           (0x10U << FCR_ADC_REG_ADC_FREQ_Pos)

#define FCR_ADC_REG_ADC12_HPF_EN_Pos           (4U)
#define FCR_ADC_REG_ADC12_HPF_EN_Msk           (0x1U << FCR_ADC_REG_ADC12_HPF_EN_Pos)
#define FCR_ADC_REG_ADC12_HPF_EN               FCR_ADC_REG_ADC12_HPF_EN_Msk

#define FCR_ADC_REG_ADC34_HPF_EN_Pos           (5U)
#define FCR_ADC_REG_ADC34_HPF_EN_Msk           (0x1U << FCR_ADC_REG_ADC34_HPF_EN_Pos)
#define FCR_ADC_REG_ADC34_HPF_EN               FCR_ADC_REG_ADC34_HPF_EN_Msk

#define FCR_ADC_REG_ADC56_HPF_EN_Pos           (6U)
#define FCR_ADC_REG_ADC56_HPF_EN_Msk           (0x1U << FCR_ADC_REG_ADC56_HPF_EN_Pos)
#define FCR_ADC_REG_ADC56_HPF_EN               FCR_ADC_REG_ADC56_HPF_EN_Msk

#define FCR_ADC_REG_ADC78_HPF_EN_Pos            (7U)
#define FCR_ADC_REG_ADC78_HPF_EN_Msk            (0x1U << FCR_ADC_REG_ADC78_HPF_EN_Pos)
#define FCR_ADC_REG_ADC78_HPF_EN                FCR_ADC_REG_ADC78_HPF_EN_Msk

/* Offset: 0x40      (R/W)  CR_DMIC12           Register */
#define CR_DMIC12_REG_ADC_DMIC_SEL1_Pos          (2U)
#define CR_DMIC12_REG_ADC_DMIC_SEL1_Msk          (0x3U << CR_DMIC12_REG_ADC_DMIC_SEL1_Pos)
#define CR_DMIC12_REG_ADC_DMIC_SEL1_LOGIC        (0x0U << CR_DMIC12_REG_ADC_DMIC_SEL1_Msk)
#define CR_DMIC12_REG_ADC_DMIC_SEL1_DIGIT        (0x1U << CR_DMIC12_REG_ADC_DMIC_SEL1_Msk)
#define CR_DMIC12_REG_ADC_DMIC_SEL1_AVG_DIGIT    (0x2U << CR_DMIC12_REG_ADC_DMIC_SEL1_Msk)

#define CR_DMIC12_REG_ADC_DMIC_SEL2_Pos        (0U)
#define CR_DMIC12_REG_ADC_DMIC_SEL2_Msk        (0x3U << CR_DMIC12_REG_ADC_DMIC_SEL2_Pos)
#define CR_DMIC12_REG_ADC_DMIC_SEL2_LOGIC         (0x0U << CR_DMIC12_REG_ADC_DMIC_SEL2_Pos)
#define CR_DMIC12_REG_ADC_DMIC_SEL2_DIGIT         (0x1U << CR_DMIC12_REG_ADC_DMIC_SEL2_Pos)
#define CR_DMIC12_REG_ADC_DMIC_SEL2_AVG_DIGIT     (0x2U << CR_DMIC12_REG_ADC_DMIC_SEL2_Pos)

#define CR_DMIC12_REG_ADC_DMIC_RATE_Pos           (4U)
#define CR_DMIC12_REG_ADC_DMIC_RATE_Msk           (0x3U << CR_DMIC12_REG_ADC_DMIC_RATE_Pos)
#define CR_DMIC12_REG_ADC_DMIC_RATE_3P0_3P25      (0x0U << CR_DMIC12_REG_ADC_DMIC_RATE_Pos)
#define CR_DMIC12_REG_ADC_DMIC_RATE_2P4_2P6       (0x1U << CR_DMIC12_REG_ADC_DMIC_RATE_Pos)
#define CR_DMIC12_REG_ADC_DMIC_RATE_1P0_1P08      (0x1U << CR_DMIC12_REG_ADC_DMIC_RATE_Pos)

#define CR_DMIC12_REG_SB_DMIC2_Pos                (6U)
#define CR_DMIC12_REG_SB_DMIC2_Msk                (0x1U << CR_DMIC12_REG_SB_DMIC2_Pos)
#define CR_DMIC12_REG_SB_DMIC2_EN                 CR_DMIC12_REG_SB_DMIC2_Msk

#define CR_DMIC12_REG_SB_DMIC1_Pos               (7U)
#define CR_DMIC12_REG_SB_DMIC1_Msk               (0x1U << CR_DMIC12_REG_SB_DMIC2_Pos)
#define CR_DMIC12_REG_SB_DMIC1_EN                CR_DMIC12_REG_SB_DMIC2_Msk
/* Offset: 0x44      (R/W)  CR_DMIC34           Register */
#define CR_DMIC34_REG_ADC_DMIC_SEL4_Pos           (0U)
#define CR_DMIC34_REG_ADC_DMIC_SEL4_Msk           (0x3U << CR_DMIC34_REG_ADC_DMIC_SEL4_Pos)
#define CR_DMIC34_REG_ADC_DMIC_SEL4_LOGIC         (0x0U << CR_DMIC34_REG_ADC_DMIC_SEL4_Msk)
#define CR_DMIC34_REG_ADC_DMIC_SEL4_DIGIT         (0x1U << CR_DMIC34_REG_ADC_DMIC_SEL4_Msk)
#define CR_DMIC34_REG_ADC_DMIC_SEL4_AVG_DIGIT     (0x2U << CR_DMIC34_REG_ADC_DMIC_SEL4_Msk)

#define CR_DMIC34_REG_ADC_DMIC_SEL3_EN_Pos        (2U)
#define CR_DMIC34_REG_ADC_DMIC_SEL3_EN_Msk        (0x3U << CR_DMIC34_REG_ADC_DMIC_SEL3_EN_Pos)
#define CR_DMIC34_REG_ADC_DMIC_SEL3_LOGIC         (0x0U << CR_DMIC34_REG_ADC_DMIC_SEL3_EN_Pos)
#define CR_DMIC34_REG_ADC_DMIC_SEL3_DIGIT         (0x1U << CR_DMIC34_REG_ADC_DMIC_SEL3_EN_Pos)
#define CR_DMIC34_REG_ADC_DMIC_SEL3_AVG_DIGIT     (0x2U << CR_DMIC34_REG_ADC_DMIC_SEL3_EN_Pos)

#define CR_DMIC34_REG_SB_DMIC4_Pos                (6U)
#define CR_DMIC34_REG_SB_DMIC4_Msk                (0x1U << CR_DMIC34_REG_SB_DMIC4_Pos)
#define CR_DMIC34_REG_SB_DMIC4_EN                 CR_DMIC34_REG_SB_DMIC4_Msk

#define CR_DMIC34_REG_SB_DMIC3_Pos                (7U)
#define CR_DMIC34_REG_SB_DMIC3_Msk                (0x1U << CR_DMIC34_REG_SB_DMIC3_Pos)
#define CR_DMIC34_REG_SB_DMIC3_EN                 CR_DMIC34_REG_SB_DMIC3_Msk
/* Offset: 0x48      (R/W)  CR_DMIC56           Register */
#define CR_DMIC56_REG_ADC_DMIC_SEL6_Pos           (0U)
#define CR_DMIC56_REG_ADC_DMIC_SEL6_Msk           (0x3U << CR_DMIC56_REG_ADC_DMIC_SEL6_Pos)
#define CR_DMIC56_REG_ADC_DMIC_SEL6_LOGIC         (0x0U << CR_DMIC56_REG_ADC_DMIC_SEL6_Msk)
#define CR_DMIC56_REG_ADC_DMIC_SEL6_DIGIT         (0x1U << CR_DMIC56_REG_ADC_DMIC_SEL6_Msk)
#define CR_DMIC56_REG_ADC_DMIC_SEL6_AVG_DIGIT     (0x2U << CR_DMIC56_REG_ADC_DMIC_SEL6_Msk)

#define CR_DMIC56_REG_ADC_DMIC_SEL5_Pos           (2U)
#define CR_DMIC56_REG_ADC_DMIC_SEL5_Msk           (0x3U << CR_DMIC56_REG_ADC_DMIC_SEL5_Pos)
#define CR_DMIC56_REG_ADC_DMIC_SEL5_LOGIC         (0x0U << CR_DMIC56_REG_ADC_DMIC_SEL5_Pos)
#define CR_DMIC56_REG_ADC_DMIC_SEL5_DIGIT         (0x1U << CR_DMIC56_REG_ADC_DMIC_SEL5_Pos)
#define CR_DMIC56_REG_ADC_DMIC_SEL5_AVG_DIGIT     (0x2U << CR_DMIC56_REG_ADC_DMIC_SEL5_Pos)

#define CR_DMIC56_REG_SB_DMIC6_Pos                (6U)
#define CR_DMIC56_REG_SB_DMIC6_Msk                (0x1U << CR_DMIC56_REG_SB_DMIC6_Pos)
#define CR_DMIC56_REG_SB_DMIC6_EN                 CR_DMIC56_REG_SB_DMIC6_Msk

#define CR_DMIC56_REG_SB_DMIC5_Pos                (7U)
#define CR_DMIC56_REG_SB_DMIC5_Msk                (0x1U << CR_DMIC56_REG_SB_DMIC5_Pos)
#define CR_DMIC56_REG_SB_DMIC5_EN                 CR_DMIC56_REG_SB_DMIC5_Msk

/* Offset: 0x4C      (R/W)  CR_DMIC78           Register */
#define CR_DMIC78_REG_ADC_DMIC_SEL8_Pos           (0U)
#define CR_DMIC78_REG_ADC_DMIC_SEL8_Msk           (0x3U << CR_DMIC78_REG_ADC_DMIC_SEL8_Pos)
#define CR_DMIC78_REG_ADC_DMIC_SEL8_LOGIC         (0x0U << CR_DMIC78_REG_ADC_DMIC_SEL8_Msk)
#define CR_DMIC78_REG_ADC_DMIC_SEL8_DIGIT         (0x1U << CR_DMIC78_REG_ADC_DMIC_SEL8_Msk)
#define CR_DMIC78_REG_ADC_DMIC_SEL8_AVG_DIGIT     (0x2U << CR_DMIC78_REG_ADC_DMIC_SEL8_Msk)

#define CR_DMIC78_REG_ADC_DMIC_SEL7_Pos           (2U)
#define CR_DMIC78_REG_ADC_DMIC_SEL7_Msk           (0x3U << CR_DMIC78_REG_ADC_DMIC_SEL7_Pos)
#define CR_DMIC78_REG_ADC_DMIC_SEL7_LOGIC         (0x0U << CR_DMIC78_REG_ADC_DMIC_SEL7_Pos)
#define CR_DMIC78_REG_ADC_DMIC_SEL7_DIGIT         (0x1U << CR_DMIC78_REG_ADC_DMIC_SEL7_Pos)
#define CR_DMIC78_REG_ADC_DMIC_SEL7_AVG_DIGIT     (0x2U << CR_DMIC78_REG_ADC_DMIC_SEL7_Pos)

#define CR_DMIC78_REG_SB_DMIC8_Pos                (6U)
#define CR_DMIC78_REG_SB_DMIC8_Msk                (0x1U << CR_DMIC78_REG_SB_DMIC8_Pos)
#define CR_DMIC78_REG_SB_DMIC8_EN                 CR_DMIC78_REG_SB_DMIC8_Msk

#define CR_DMIC78_REG_SB_DMIC7_Pos                (7U)
#define CR_DMIC78_REG_SB_DMIC7_Msk                (0x1U << CR_DMIC78_REG_SB_DMIC7_Pos)
#define CR_DMIC78_REG_SB_DMIC7_EN                 CR_DMIC78_REG_SB_DMIC7_Msk

/* Offset: 0x50      (R/W)  CR_HP               Register */
#define CR_HP_REG_SB_HP_OPEN_Pos             (4U)
#define CR_HP_REG_SB_HP_OPEN_Msk             (0x1U << CR_HP_REG_SB_HP_OPEN_Pos)
#define CR_HP_REG_SB_HP_OPEN_EN              CR_HP_REG_SB_HP_OPEN_Msk

#define CR_HP_REG_SB_HPCM_Pos                (5U)
#define CR_HP_REG_SB_HPCM_Msk                (0x1U << CR_HP_REG_SB_HP_Pos)

#define CR_HP_REG_HP_MUTE_Pos                (7U)
#define CR_HP_REG_HP_MUTE_Msk                (0x1U << CR_HP_REG_HP_MUTE_Pos)
#define CR_HP_REG_HP_MUTE_EN                 CR_HP_REG_HP_MUTE_Msk

/* Offset: 0x54      (R/W)  GCR_HPL             Register */
#define GCR_HPL_REG_GOL_Pos                  (0U)
#define GCR_HPL_REG_GOL_Msk                  (0xFU << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_0DB                  (0x0U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_1DB                  (0x1U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_2DB                  (0x2U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_3DB                  (0x3U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_4DB                  (0x4U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_5DB                  (0x5U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_6DB                  (0x6U << GCR_HPL_REG_GOL_Pos)
#define GCR_HPL_REG_GOL_7DB                  (0x7U << GCR_HPL_REG_GOL_Pos)

#define GCR_HPL_REG_LRGO_Pos                 (6U)
#define GCR_HPL_REG_LRGO_Msk                 (0x1U << GCR_HPL_REG_LRGO_Pos)
#define GCR_HPL_REG_LRGO_INDEPENDENT         (0x0U << GCR_HPL_REG_LRGO_Pos)
#define GCR_HPL_REG_LRGO_TOGETHER            GCR_HPL_REG_LRGO_Msk
/* Offset: 0x58      (R/W)  GCR_HPR             Register */
#define GCR_HPR_REG_GOR_Pos                  (0U)
#define GCR_HPR_REG_GOR_Msk                  (0xFU << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_0DB                  (0x0U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_1DB                  (0x1U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_2DB                  (0x2U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_3DB                  (0x3U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_4DB                  (0x4U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_5DB                  (0x5U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_6DB                  (0x6U << GCR_HPR_REG_GOR_Pos)
#define GCR_HPR_REG_GOR_7DB                  (0x7U << GCR_HPR_REG_GOR_Pos)


/* Offset: 0x5C      (R/W)  CR_MIC1_REG              Register */
#define CR_MIC1_REG_SB_MICBIAS1_Pos           (5U)
#define CR_MIC1_REG_SB_MICBIAS1_Msk           (0x1U << CR_MIC1_REG_SB_MICBIAS1_Pos)
#define CR_MIC1_REG_SB_MICBIAS1_OPEN          (0x0U << CR_MIC1_REG_SB_MICBIAS1_Pos)
#define CR_MIC1_REG_SB_MICBIAS1_CLOSE         CR_MIC1_REG_SB_MICBIAS1_Msk

#define CR_MIC1_REG_SB_MICDIFF1_Pos           (6U)
#define CR_MIC1_REG_SB_MICDIFF1_Msk           (0x1U << CR_MIC1_REG_SB_MICBIAS1_Pos)
#define CR_MIC1_REG_SB_MICDIFF1_DIFF          (0x0U << CR_MIC1_REG_SB_MICBIAS1_Pos)
#define CR_MIC1_REG_SB_MICDIFF1_SINGLE        CR_MIC1_REG_SB_MICDIFF1_Msk

#define CR_MIC1_REG_SB_MICBIAS1_V_Pos         (7U)
#define CR_MIC1_REG_SB_MICBIAS1_V_Msk         (0x1U << CR_MIC1_REG_SB_MICBIAS1_Pos)
#define CR_MIC1_REG_SB_MICBIAS1_V_1P5         (0x0U << CR_MIC1_REG_SB_MICBIAS1_Pos)
#define CR_MIC1_REG_SB_MICBIAS1_V_1P8         (0x0U << CR_MIC1_REG_SB_MICBIAS1_Pos)


/* Offset: 0x8C      (R/W)  CR_DAC              Register */
#define GCR_MIC_REG_GIM1_Pos                  (0U)
#define GCR_MIC_REG_GIM1_Msk                  (0x7U << GCR_MIC_REG_GIM1_Pos)
#define GCR_MIC_REG_GIM_0DB                   (0x0U << GCR_MIC_REG_GIM1_Pos)
#define GCR_MIC_REG_GIM_4DB                   (0x1U << GCR_MIC_REG_GIM1_Pos)
#define GCR_MIC_REG_GIM_8DB                   (0x2U << GCR_MIC_REG_GIM1_Pos)
#define GCR_MIC_REG_GIM_12B                   (0x3U << GCR_MIC_REG_GIM1_Pos)
#define GCR_MIC_REG_GIM_16DB                  (0x4U << GCR_MIC_REG_GIM1_Pos)
#define GCR_MIC_REG_GIM_20DB                  (0x5U << GCR_MIC_REG_GIM1_Pos)

#define GCR_MIC_REG_GIM2_Pos                   (3U)
#define GCR_MIC_REG_GIM2_Msk                   (0x7U << GCR_MIC_REG_GIM2_Pos)
// #define GCR_MIC_REG_GIM2_0DB                   (0x0U << GCR_MIC_REG_GIM2_Pos)
// #define GCR_MIC_REG_GIM2_4DB                   (0x1U << GCR_MIC_REG_GIM2_Pos)
// #define GCR_MIC_REG_GIM2_8DB                   (0x2U << GCR_MIC_REG_GIM2_Pos)
// #define GCR_MIC_REG_GIM2_12B                   (0x3U << GCR_MIC_REG_GIM2_Pos)
// #define GCR_MIC_REG_GIM2_16DB                  (0x4U << GCR_MIC_REG_GIM2_Pos)
// #define GCR_MIC_REG_GIM2_20DB                  (0x5U << GCR_MIC_REG_GIM2_Pos)
/* Offset: 0x8C      (R/W)  CR_DAC              Register */
#define CR_DAC_REG_SB_DAC_Pos                   (4U)
#define CR_DAC_REG_SB_DAC_Msk                   (0x1U << CR_DAC_REG_SB_DAC_Pos)
#define CR_DAC_REG_SB_DAC_EN                     CR_DAC_REG_SB_DAC_Msk

#define CR_DAC_REG_DAC_LEFT_ONLY_Pos            (5U)
#define CR_DAC_REG_DAC_LEFT_ONLY_Msk            (0x1U << CR_DAC_REG_DAC_LEFT_ONLY_Pos)
#define CR_DAC_REG_DAC_LEFT_ONLY_EN             CR_DAC_REG_DAC_LEFT_ONLY_Msk

#define CR_DAC_REG_DAC_SOFT_MUTE_Pos            (6U)
#define CR_DAC_REG_DAC_SOFT_MUTE_Msk            (0x1U << CR_DAC_REG_DAC_SOFT_MUTE_Pos)
#define CR_DAC_REG_DAC_SOFT_MUTE_EN           CR_DAC_REG_DAC_SOFT_MUTE_Msk
/* Offset: 0x8C      (R/W)  CR_ADC              Register */
#define CR_ADC_REG_SB_ADC1_Pos                  (4U)
#define CR_ADC_REG_SB_ADC1_Msk                  (0x1U << CR_ADC_REG_SB_ADC1_Pos)
#define CR_ADC_REG_SB_ADC1_OPEN                 CR_ADC_REG_SB_ADC1_Msk

#define CR_ADC_REG_SB_ADC2_Pos                  (5U)
#define CR_ADC_REG_SB_ADC2_Msk                  (0x1U << CR_ADC_REG_SB_ADC1_Pos)
#define CR_ADC_REG_SB_ADC2_OPEN                 CR_ADC_REG_SB_ADC1_Msk

#define CR_ADC_REG_ADC12_SOFT_MUTE_Pos          (7U)
#define CR_ADC_REG_ADC12_SOFT_MUTE_Msk          (0x1U << CR_ADC_REG_ADC12_SOFT_MUTE_Pos)
#define CR_ADC_REG_ADC12_SOFT_MUTE_OPEN         CR_ADC_REG_ADC12_SOFT_MUTE_Msk
/* Offset: 0xA0      (R/W)  CR_MIX              Register */
#define CR_MIX_REG_MIX_ADD_Pos                  (0U)
#define CR_MIX_REG_MIX_ADD_Msk                  (0x3FU << CR_MIX_REG_MIX_ADD_Pos)

#define CR_MIX_REG_MIX_LOAD_Pos                 (6U)
#define CR_MIX_REG_MIX_LOAD_Msk                 (0x1U << CR_MIX_REG_MIX_LOAD_Pos)
#define CR_MIX_REG_MIX_LOAD_EN                  CR_MIX_REG_MIX_LOAD_Msk
/* Offset: 0xA4      (R/W)  DR_MIX              Register */
#define DR_MIX_REG_MIX_DATA_Pos                 (0U)
#define DR_MIX_REG_MIX_DATA_Msk                 (0xFFU << DR_MIX_REG_MIX_DATA_Pos)
/* Offset: 0xA8      (R/W)  GCR_DACL            Register */
#define GCR_DACL_REG_CODL_Pos                   (0U)
#define GCR_DACL_REG_CODL_Msk                   (0x3FU << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_0DB                   (0x0U << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_F1DB                  (0x1U << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_F2DB                  (0x2U << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_F31DB                 (0x1FU << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_1DB                   (0x3FU << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_2DB                   (0x3EU << GCR_DACL_REG_CODL_Pos)
#define GCR_DACL_REG_CODL_32DB                  (0x20U << GCR_DACL_REG_CODL_Pos)

#define GCR_DACL_REG_LRGOD_Pos                   (7U)
#define GCR_DACL_REG_LRGOD_Msk                   (0x1U << GCR_DACL_REG_LRGOD_Pos)
#define GCR_DACL_REG_LRGOD_TOGETHER              GCR_DACL_REG_LRGOD_Msk
/* Offset: 0xAC      (R/W)  GCR_DACR            Register */
#define GCR_DACR_REG_CODR_Pos                   (0U)
#define GCR_DACR_REG_CODR_Msk                   (0x3FU << GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_0DB                   (0x0U <<  GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_F1DB                  (0x1U <<  GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_F2DB                  (0x2U <<  GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_F31DB                 (0x1FU << GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_1DB                   (0x3FU << GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_2DB                   (0x3EU << GCR_DACR_REG_CODR_Pos)
#define GCR_DACR_REG_CODR_32DB                  (0x20U << GCR_DACR_REG_CODR_Pos)

#define GCR_ADC_REG_GODR_Pos                    (0U)
#define GCR_ADC_REG_GODR_Msk                    (0x3FU << GCR_DACR_REG_CODR_Pos)
#define GCR_ADC_REG_LRGID_Pos                   (7U)
#define GCR_ADC_REG_LRGID_Msk                   (0x1U << GCR_ADC_REG_LRGID_Pos)

/* Offset: 0xB0      (R/W)  GCR_ADC1            Register */
#define GCR_ADC1_REG_GID1_Pos                   (0U)
#define GCR_ADC1_REG_GID1_Msk                   (0x3FU << GCR_ADC1_REG_GID1_Pos)

#define GCR_ADC1_REG_LRGID_Pos                   (7U)
#define GCR_ADC1_REG_LRGID_Msk                   (0x1U << GCR_ADC1_REG_LRGID_Pos)
#define GCR_ADC1_REG_LRGID                       GCR_ADC1_REG_LRGID_Msk
/* Offset: 0xD0      (R/W)  GCR_MIXDACL         Register */
#define GCR_MIXDACL_REG_GOMIXL_Pos              (0U)
#define GCR_MIXDACL_REG_GOMIXL_Msk              (0x1FU << GCR_DACR_REG_CODR_Pos)
#define GCR_MIXDACR_L_0DB                       (0U)
#define GCR_MIXDACR_L_F1DB                       (1U)
#define GCR_MIXDACR_L_F2DB                       (2U)
#define GCR_MIXDACR_L_F3DB                       (3U)
#define GCR_MIXDACR_L_F4DB                       (4U)
#define GCR_MIXDACR_L_F5DB                       (5U)
#define GCR_MIXDACR_L_F6DB                       (6U)
#define GCR_MIXDACR_L_F7DB                       (7U)
#define GCR_MIXDACR_L_F8DB                       (8U)
#define GCR_MIXDACR_L_F9DB                       (9U)
#define GCR_MIXDACR_L_F10DB                      (10U)
#define GCR_MIXDACR_L_F11DB                      (11U)
#define GCR_MIXDACR_L_F12DB                      (12U)
#define GCR_MIXDACR_L_F13DB                      (13U)
#define GCR_MIXDACR_L_F14DB                      (14U)
#define GCR_MIXDACR_L_F15DB                      (15U)
#define GCR_MIXDACR_L_F16DB                      (16U)
#define GCR_MIXDACR_L_F17DB                      (17U)
#define GCR_MIXDACR_L_F18DB                      (18U)
#define GCR_MIXDACR_L_F19DB                      (19U)
#define GCR_MIXDACR_L_F20DB                      (20U)
#define GCR_MIXDACR_L_F21DB                      (21U)
#define GCR_MIXDACR_L_F22DB                      (22U)
#define GCR_MIXDACR_L_F23DB                      (23U)
#define GCR_MIXDACR_L_F24DB                      (24U)
#define GCR_MIXDACR_L_F25DB                      (25U)
#define GCR_MIXDACR_L_F26DB                      (26U)
#define GCR_MIXDACR_L_F27DB                      (27U)
#define GCR_MIXDACR_L_F28DB                      (28U)
#define GCR_MIXDACR_L_F29DB                      (29U)
#define GCR_MIXDACR_L_F30DB                      (30U)
#define GCR_MIXDACR_L_F31DB                      (31U)
#define GCR_MIXDACL_REG_LRGOMIX_Pos             (7U)
#define GCR_MIXDACL_REG_LRGOMIX_Msk             (0x1U << GCR_MIXDACL_REG_LRGOMIX_Pos)
#define GCR_MIXDACL_REG_LRGOMIX_INDEPENDENT     (0x0U << GCR_MIXDACL_REG_LRGOMIX_Pos)
#define GCR_MIXDACL_REG_LRGOMIX_TOGETHER        GCR_MIXDACL_REG_LRGOMIX_Msk
/* Offset: 0xD4      (R/W)  GCR_MIXDACR         Register */
#define GCR_MIXDACR_REG_GOMIXR_Pos              (0U)
#define GCR_MIXDACR_REG_GOMIXR_Msk              (0x1FU << GCR_MIXDACR_REG_GOMIXR_Pos)
/* Offset: 0xD8      (R/W)  GCR_MIXADCL         Register */
#define GCR_MIXADCL_REG_GIMIXL_Pos              (0U)
#define GCR_MIXADCL_REG_GIMIXL_Msk              (0x1FU << GCR_MIXADCL_REG_GIMIXL_Pos)
#define GCR_MIXADCL_REG_LRGIMIX_Pos              (7U)
#define GCR_MIXADCL_REG_LRGIMIX_Msk              (0x1U << GCR_MIXADCL_REG_LRGIMIX_Pos)
/* Offset: 0xDC      (R/W)  GCR_MIXADCR         Register */
#define GCR_MIXADCR_REG_GIMIXR_Pos             (0U)
#define GCR_MIXADCR_REG_GIMIXR_Msk             (0x1FU << GCR_MIXADCR_REG_GIMIXR_Pos)
/* Offset: 0xE0      (R/W)  CR_DAC_AGC          Register */
/* Offset: 0xE4      (R/W)  DR_DAC_AGC          Register */
/* Offset: 0xE8      (R/W)  CR_ADC_AGC          Register */
#define CR_ADC_AGC_REG_ADC_AGC_ADD_Pos         (0U)
#define CR_ADC_AGC_REG_ADC_AGC_ADD_Msk         (0x3FU << CR_ADC_AGC_REG_ADC_AGC_ADD_Pos)

#define CR_ADC_AGC_REG_ADC_AGC_LOAD_Pos         (6U)
#define CR_ADC_AGC_REG_ADC_AGC_LOAD_Msk         (0x3FU << CR_ADC_AGC_REG_ADC_AGC_LOAD_Pos)
#define CR_ADC_AGC_REG_ADC_AGC_LOAD_EN          CR_ADC_AGC_REG_ADC_AGC_LOAD_Msk
/* Offset: 0xEC      (R/W)  DR_ADC_AGC          Register */
#define DR_ADC_AGC_REG_ADC_AGC_DATA_Pos        (0U)
#define DR_ADC_AGC_REG_ADC_AGC_DATA_Msk        (0xFFU << DR_ADC_AGC_REG_ADC_AGC_DATA_Pos)
/* Offset: 0xF0      (R/W)  CR_DAC_FI           Register */
/* Offset: 0xF4      (R/W)  DR_DAC_FI           Register */
/* Offset: 0XF8      (R)     RESERVED */
/* Offset: 0x100     (R/W)  SR_WT               Register */
#define SR_WT_REG_IRQ_WT_PENDING_Pos           (6U)
#define SR_WT_REG_IRQ_WT_PENDING_Msk           (0x1U << SR_WT_REG_IRQ_WT_PENDING_Pos)
#define SR_WT_REG_IRQ_WT_PENDING_EN            CR_ADC_AGC_REG_ADC_AGC_ADD_Msk
/* Offset: 0x104     (R/W)  ICR_WT              Register */
#define ICR_WT_REG_WT_INT_FORM_Pos             (6U)
#define ICR_WT_REG_WT_INT_FORM_Msk             (0x3U << ICR_WT_REG_WT_INT_FORM_Pos)
#define ICR_WT_REG_WT_INT_FORM_HIGH            (0U<<ICR_WT_REG_WT_INT_FORM_Pos)
#define ICR_WT_REG_WT_INT_FORM_LOW             (1U<<ICR_WT_REG_WT_INT_FORM_Pos)
#define ICR_WT_REG_WT_INT_FORM_8MCCLK_HIGH     (2U<<ICR_WT_REG_WT_INT_FORM_Pos)
#define ICR_WT_REG_WT_INT_FORM_8MCCLK_LOW      (3U<<ICR_WT_REG_WT_INT_FORM_Pos)
/* Offset: 0x108     (R/W)  IMR_WT              Register */
#define IMR_WT_REG_WT_DTC_MASK_Pos             (0U)
#define IMR_WT_REG_WT_DTC_MASK_Msk             (0x1U << IMR_WT_REG_WT_DTC_MASK_Pos)
#define IMR_WT_REG_WT_DTC_MASK_EN              IMR_WT_REG_WT_DTC_MASK_Msk
/* Offset: 0x10C     (R/W)  IFR_WT              Register */
#define IFR_WT_REG_WT_DTC_Pos                  (0U)
#define IFR_WT_REG_WT_DTC_Msk                  (0x1U << IFR_WT_REG_WT_DTC_Pos)
#define IFR_WT_REG_WT_DTC_EN                   IFR_WT_REG_WT_DTC_Msk
/* Offset: 0x110     (R/W)  CR_WT               Register */
#define CR_WT_REG_WT_RESTART_Pos               (0U)
#define CR_WT_REG_WT_RESTART_Msk               (0x1U << CR_WT_REG_WT_RESTART_Pos)
#define CR_WT_REG_WT_RESTART_EN                CR_WT_REG_WT_RESTART_Msk

#define CR_WT_REG_WT_ONLY_Pos                  (6U)
#define CR_WT_REG_WT_ONLY_Msk                  (0x1U << CR_WT_REG_WT_ONLY_Pos)
#define CR_WT_REG_WT_ONLY_EN                   CR_WT_REG_WT_ONLY_Msk

#define CR_WT_REG_WT_EN_Pos                    (7U)
#define CR_WT_REG_WT_EN_Msk                    (0x1U << CR_WT_REG_WT_EN_Pos)
#define CR_WT_REG_WT_EN                        CR_WT_REG_WT_EN_Msk
/* Offset: 0x114     (R/W)  CR_WT_2             Register */
#define CR_WT2_REG_WT_DMIC_FREQ_Pos            (0U)
#define CR_WT2_REG_WT_DMIC_FREQ_Msk            (0x7U << CR_WT2_REG_WT_DMIC_FREQ_Pos)
#define CR_WT2_REG_WT_DMIC_FREQ_3P0_3P25       (0x0U << CR_WT2_REG_WT_DMIC_FREQ_Pos)
#define CR_WT2_REG_WT_DMIC_FREQ_2P4_2P6        (0x1U << CR_WT2_REG_WT_DMIC_FREQ_Pos)
#define CR_WT2_REG_WT_DMIC_FREQ_1P0_1P08       (0x1U << CR_WT2_REG_WT_DMIC_FREQ_Pos)

#define CR_WT2_REG_ZCD_Pos           (4U)
#define CR_WT2_REG_ZCD_Msk           (0x1U << CR_WT2_REG_ZCD_Pos)
#define CR_WT2_REG_ZCD_EN               CR_WT2_REG_ZCD_Msk

#define CR_WT2_REG_WT_ADC_SEL_Pos       (7U)
#define CR_WT2_REG_WT_ADC_SEL_Msk       (0x1U << CR_WT2_REG_WT_ADC_SEL_Pos)
#define CR_WT2_REG_WT_ADC_SEL_ADC1      CR_WT2_REG_WT_ADC_SEL_Msk
#define CR_WT2_REG_WT_ADC_SEL_DIGIT1    (0U)

/* Offset: 0x118     (R/W)  CR_WT_3             Register */
#define CR_WT_3_REG_WT_POWER_SENS_Pos           (0U)
#define CR_WT_3_REG_WT_POWER_SENS_Msk           (0x1FU << CR_WT_3_REG_WT_POWER_SENS_Pos)
#define CR_WT_3_REG_WT_POWER_SENS_0DB           (0x0U << CR_WT_3_REG_WT_POWER_SENS_Pos)
/* Offset: 0x11C     (R/W)  CR_WT_4             Register */
/* Offset: 0X120     (R)     RESERVED */
/* Offset: 0x180     (R/W)  CR_TR               Register */
/* Offset: 0x184     (R/W)  DR_TR               Register */
/* Offset: 0x188     (R/W)  SR_TR1              Register */
/* Offset: 0x18C     (R/W)  SR_TR_SRCDAC        Register */
/* TRAN_SEL_REG, offset: 0x200 */
#define TRAN_SEL_REG_SI2C_Pos                       (0U)
#define TRAN_SEL_REG_SI2C_Msk                       (0x1U << TRAN_SEL_REG_SI2C_Pos)
#define TRAN_SEL_REG_I2C                            TRAN_SEL_REG_SI2C_Msk
#define TRAN_SEL_REG_uC                             (0U)
/* PARA_TX_FIFO_REG   ,offset：0x204 */
#define TRAN_TX_FIFOREG_LEFT_DATA_Pos                       (0U)
#define TRAN_TX_FIFOREG_LEFT_DATA_Msk                       (0xFFFFU << TRAN_TX_FIFOREG_LEFT_DATA_Pos)
#define TRAN_TX_FIFOREG_LEFT_DATA                           TRAN_TX_FIFOREG_LEFT_DATA_Msk

#define TRAN_TX_FIFOREG_RIGHT_DATA_Pos                      (16U)
#define TRAN_TX_FIFOREG_RIGHT_DATA_Msk                      (0xFFFFU << TRAN_TX_FIFOREG_RIGHT_DATA_Pos)
#define TRAN_TX_FIFOREG_RIGHT_DATA                          TRAN_TX_FIFOREG_RIGHT_DATA_Msk
/* RX_FIFO1_REG       ,offset：0x208 */
#define FIFO_TH_CTRL_TX_Pos                       (4U)
#define FIFO_TH_CTRL_TX_Msk                       (0x7U << FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_CTRL_TX                           FIFO_TH_CTRL_TX_Msk
#define FIFO_TH_TX_1                              (0U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_2                              (1U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_3                              (2U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_4                              (3U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_5                              (4U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_6                              (5U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_7                              (6U<< FIFO_TH_CTRL_TX_Pos)
#define FIFO_TH_TX_8                              (7U<< FIFO_TH_CTRL_TX_Pos)

#define FIFO_TH_CTRL_RX_Pos                       (0U)
#define FIFO_TH_CTRL_RX_Msk                       (0x7U << FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_CTRL_RX                           FIFO_TH_CTRL_RX_Msk
#define FIFO_TH_RX_1                              (0U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_2                              (1U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_3                              (2U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_4                              (3U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_5                              (4U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_6                              (5U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_7                              (6U<< FIFO_TH_CTRL_RX_Pos)
#define FIFO_TH_RX_8                              (7U<< FIFO_TH_CTRL_RX_Pos)
/* INTR_ERR_CTRL      ,offset：0x22C */
#define INTR_ERR_CTRL_REG_RX_FULL1_Pos                       (0U)
#define INTR_ERR_CTRL_REG_RX_FULL1_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL1_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL1                           INTR_ERR_CTRL_REG_RX_FULL1_Msk
#define INTR_ERR_CTRL_REG_RX_FULL2_Pos                       (1U)
#define INTR_ERR_CTRL_REG_RX_FULL2_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL2_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL2                           INTR_ERR_CTRL_REG_RX_FULL2_Msk
#define INTR_ERR_CTRL_REG_RX_FULL3_Pos                       (2U)
#define INTR_ERR_CTRL_REG_RX_FULL3_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL3_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL3                           INTR_ERR_CTRL_REG_RX_FULL3_Msk
#define INTR_ERR_CTRL_REG_RX_FULL4_Pos                       (3U)
#define INTR_ERR_CTRL_REG_RX_FULL4_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL4_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL4                           INTR_ERR_CTRL_REG_RX_FULL4_Msk
#define INTR_ERR_CTRL_REG_RX_FULL5_Pos                       (4U)
#define INTR_ERR_CTRL_REG_RX_FULL5_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL5_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL5                           INTR_ERR_CTRL_REG_RX_FULL5_Msk
#define INTR_ERR_CTRL_REG_RX_FULL6_Pos                       (5U)
#define INTR_ERR_CTRL_REG_RX_FULL6_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL6_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL6                           INTR_ERR_CTRL_REG_RX_FULL6_Msk
#define INTR_ERR_CTRL_REG_RX_FULL7_Pos                       (6U)
#define INTR_ERR_CTRL_REG_RX_FULL7_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL7_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL7                           INTR_ERR_CTRL_REG_RX_FULL7_Msk
#define INTR_ERR_CTRL_REG_RX_FULL8_Pos                       (7U)
#define INTR_ERR_CTRL_REG_RX_FULL8_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_FULL8_Pos)
#define INTR_ERR_CTRL_REG_RX_FULL8                           INTR_ERR_CTRL_REG_RX_FULL8_Msk
#define INTR_ERR_CTRL_REG_RX_TH1_Pos                         (8U)
#define INTR_ERR_CTRL_REG_RX_TH1_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH1_Pos)
#define INTR_ERR_CTRL_REG_RX_TH1                             INTR_ERR_CTRL_REG_RX_TH1_Msk
#define INTR_ERR_CTRL_REG_RX_TH2_Pos                         (9U)
#define INTR_ERR_CTRL_REG_RX_TH2_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH2_Pos)
#define INTR_ERR_CTRL_REG_RX_TH2                             INTR_ERR_CTRL_REG_RX_TH2_Msk
#define INTR_ERR_CTRL_REG_RX_TH3_Pos                         (10U)
#define INTR_ERR_CTRL_REG_RX_TH3_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH3_Pos)
#define INTR_ERR_CTRL_REG_RX_TH3                             INTR_ERR_CTRL_REG_RX_TH3_Msk
#define INTR_ERR_CTRL_REG_RX_TH4_Pos                         (11U)
#define INTR_ERR_CTRL_REG_RX_TH4_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH4_Pos)
#define INTR_ERR_CTRL_REG_RX_TH4                             INTR_ERR_CTRL_REG_RX_TH4_Msk
#define INTR_ERR_CTRL_REG_RX_TH5_Pos                         (12U)
#define INTR_ERR_CTRL_REG_RX_TH5_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH5_Pos)
#define INTR_ERR_CTRL_REG_RX_TH5                             INTR_ERR_CTRL_REG_RX_TH5_Msk
#define INTR_ERR_CTRL_REG_RX_TH6_Pos                         (13U)
#define INTR_ERR_CTRL_REG_RX_TH6_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH6_Pos)
#define INTR_ERR_CTRL_REG_RX_TH6                             INTR_ERR_CTRL_REG_RX_TH6_Msk
#define INTR_ERR_CTRL_REG_RX_TH7_Pos                         (14U)
#define INTR_ERR_CTRL_REG_RX_TH7_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH7_Pos)
#define INTR_ERR_CTRL_REG_RX_TH7                             INTR_ERR_CTRL_REG_RX_TH7_Msk
#define INTR_ERR_CTRL_REG_RX_TH8_Pos                         (15U)
#define INTR_ERR_CTRL_REG_RX_TH8_Msk                         (0x1U << INTR_ERR_CTRL_REG_RX_TH8_Pos)
#define INTR_ERR_CTRL_REG_RX_TH8                             INTR_ERR_CTRL_REG_RX_TH8_Msk
#define INTR_ERR_CTRL_REG_PARA_TX_EMPTY_Pos                  (16U)
#define INTR_ERR_CTRL_REG_PARA_TX_EMPTY_Msk                  (0x1U << INTR_ERR_CTRL_REG_PARA_TX_EMPTY_Pos)
#define INTR_ERR_CTRL_REG_PARA_TX_EMPTY                      INTR_ERR_CTRL_REG_PARA_TX_EMPTY_Msk
#define INTR_ERR_CTRL_REG_PARA_TX_TH_Pos                     (17U)
#define INTR_ERR_CTRL_REG_PARA_TX_TH_Msk                     (0x1U << INTR_ERR_CTRL_REG_PARA_TX_TH_Pos)
#define INTR_ERR_CTRL_REG_PARA_TX_TH                         INTR_ERR_CTRL_REG_PARA_TX_TH_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR1_Pos                       (18U)
#define INTR_ERR_CTRL_REG_RX_ERROR1_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR1_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR1                           INTR_ERR_CTRL_REG_RX_ERROR1_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR2_Pos                       (19U)
#define INTR_ERR_CTRL_REG_RX_ERROR2_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR2_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR2                           INTR_ERR_CTRL_REG_RX_ERROR2_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR3_Pos                       (20U)
#define INTR_ERR_CTRL_REG_RX_ERROR3_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR3_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR3                           INTR_ERR_CTRL_REG_RX_ERROR3_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR4_Pos                       (21U)
#define INTR_ERR_CTRL_REG_RX_ERROR4_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR4_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR4                           INTR_ERR_CTRL_REG_RX_ERROR4_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR5_Pos                       (22U)
#define INTR_ERR_CTRL_REG_RX_ERROR5_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR5_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR5                           INTR_ERR_CTRL_REG_RX_ERROR5_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR6_Pos                       (23U)
#define INTR_ERR_CTRL_REG_RX_ERROR6_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR6_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR6                           INTR_ERR_CTRL_REG_RX_ERROR6_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR7_Pos                       (24U)
#define INTR_ERR_CTRL_REG_RX_ERROR7_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR7_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR7                           INTR_ERR_CTRL_REG_RX_ERROR7_Msk
#define INTR_ERR_CTRL_REG_RX_ERROR8_Pos                       (25U)
#define INTR_ERR_CTRL_REG_RX_ERROR8_Msk                       (0x1U << INTR_ERR_CTRL_REG_RX_ERROR8_Pos)
#define INTR_ERR_CTRL_REG_RX_ERROR8                           INTR_ERR_CTRL_REG_RX_ERROR8_Msk
#define INTR_ERR_CTRL_REG_TX_ERROR8_Pos                       (26U)
#define INTR_ERR_CTRL_REG_TX_ERROR8_Msk                       (0x1U << INTR_ERR_CTRL_REG_TX_ERROR8_Pos)
#define INTR_ERR_CTRL_REG_TX_ERROR8                           INTR_ERR_CTRL_REG_TX_ERROR8_Msk
/* INTR_ERR_STA _REG  ,offset：0x230 */
#define INTR_ERR_STA_REG_RX_FULL1_Pos                       (0U)
#define INTR_ERR_STA_REG_RX_FULL1_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL1_Pos)
#define INTR_ERR_STA_REG_RX_FULL1                           INTR_ERR_STA_REG_RX_FULL1_Msk
#define INTR_ERR_STA_REG_RX_FULL2_Pos                       (1U)
#define INTR_ERR_STA_REG_RX_FULL2_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL2_Pos)
#define INTR_ERR_STA_REG_RX_FULL2                           INTR_ERR_STA_REG_RX_FULL2_Msk
#define INTR_ERR_STA_REG_RX_FULL3_Pos                       (2U)
#define INTR_ERR_STA_REG_RX_FULL3_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL3_Pos)
#define INTR_ERR_STA_REG_RX_FULL3                           INTR_ERR_STA_REG_RX_FULL3_Msk
#define INTR_ERR_STA_REG_RX_FULL4_Pos                       (3U)
#define INTR_ERR_STA_REG_RX_FULL4_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL4_Pos)
#define INTR_ERR_STA_REG_RX_FULL4                           INTR_ERR_STA_REG_RX_FULL4_Msk
#define INTR_ERR_STA_REG_RX_FULL5_Pos                       (4U)
#define INTR_ERR_STA_REG_RX_FULL5_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL5_Pos)
#define INTR_ERR_STA_REG_RX_FULL5                           INTR_ERR_STA_REG_RX_FULL5_Msk
#define INTR_ERR_STA_REG_RX_FULL6_Pos                       (5U)
#define INTR_ERR_STA_REG_RX_FULL6_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL6_Pos)
#define INTR_ERR_STA_REG_RX_FULL6                           INTR_ERR_STA_REG_RX_FULL6_Msk
#define INTR_ERR_STA_REG_RX_FULL7_Pos                       (6U)
#define INTR_ERR_STA_REG_RX_FULL7_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL7_Pos)
#define INTR_ERR_STA_REG_RX_FULL7                           INTR_ERR_STA_REG_RX_FULL7_Msk
#define INTR_ERR_STA_REG_RX_FULL8_Pos                       (7U)
#define INTR_ERR_STA_REG_RX_FULL8_Msk                       (0x1U << INTR_ERR_STA_REG_RX_FULL8_Pos)
#define INTR_ERR_STA_REG_RX_FULL8                           INTR_ERR_STA_REG_RX_FULL8_Msk
#define INTR_ERR_STA_REG_RX_TH1_Pos                         (8U)
#define INTR_ERR_STA_REG_RX_TH1_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH1_Pos)
#define INTR_ERR_STA_REG_RX_TH1                             INTR_ERR_STA_REG_RX_TH1_Msk
#define INTR_ERR_STA_REG_RX_TH2_Pos                         (9U)
#define INTR_ERR_STA_REG_RX_TH2_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH2_Pos)
#define INTR_ERR_STA_REG_RX_TH2                             INTR_ERR_STA_REG_RX_TH2_Msk
#define INTR_ERR_STA_REG_RX_TH3_Pos                         (10U)
#define INTR_ERR_STA_REG_RX_TH3_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH3_Pos)
#define INTR_ERR_STA_REG_RX_TH3                             INTR_ERR_STA_REG_RX_TH3_Msk
#define INTR_ERR_STA_REG_RX_TH4_Pos                         (11U)
#define INTR_ERR_STA_REG_RX_TH4_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH4_Pos)
#define INTR_ERR_STA_REG_RX_TH4                             INTR_ERR_STA_REG_RX_TH4_Msk
#define INTR_ERR_STA_REG_RX_TH5_Pos                         (12U)
#define INTR_ERR_STA_REG_RX_TH5_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH5_Pos)
#define INTR_ERR_STA_REG_RX_TH5                             INTR_ERR_STA_REG_RX_TH5_Msk
#define INTR_ERR_STA_REG_RX_TH6_Pos                         (13U)
#define INTR_ERR_STA_REG_RX_TH6_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH6_Pos)
#define INTR_ERR_STA_REG_RX_TH6                             INTR_ERR_STA_REG_RX_TH6_Msk
#define INTR_ERR_STA_REG_RX_TH7_Pos                         (14U)
#define INTR_ERR_STA_REG_RX_TH7_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH7_Pos)
#define INTR_ERR_STA_REG_RX_TH7                             INTR_ERR_STA_REG_RX_TH7_Msk
#define INTR_ERR_STA_REG_RX_TH8_Pos                         (15U)
#define INTR_ERR_STA_REG_RX_TH8_Msk                         (0x1U << INTR_ERR_STA_REG_RX_TH8_Pos)
#define INTR_ERR_STA_REG_RX_TH8                             INTR_ERR_STA_REG_RX_TH8_Msk
#define INTR_ERR_STA_REG_PARA_TX_EMPTY_Pos                  (16U)
#define INTR_ERR_STA_REG_PARA_TX_EMPTY_Msk                  (0x1U << INTR_ERR_STA_REG_PARA_TX_EMPTY_Pos)
#define INTR_ERR_STA_REG_PARA_TX_EMPTY                      INTR_ERR_STA_REG_PARA_TX_EMPTY_Msk
#define INTR_ERR_STA_REG_PARA_TX_TH_Pos                     (17U)
#define INTR_ERR_STA_REG_PARA_TX_TH_Msk                     (0x1U << INTR_ERR_STA_REG_PARA_TX_TH_Pos)
#define INTR_ERR_STA_REG_PARA_TX_TH                         INTR_ERR_STA_REG_PARA_TX_TH_Msk
#define INTR_ERR_STA_REG_RX_ERROR1_Pos                       (18U)
#define INTR_ERR_STA_REG_RX_ERROR1_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR1_Pos)
#define INTR_ERR_STA_REG_RX_ERROR1                           INTR_ERR_STA_REG_RX_ERROR1_Msk
#define INTR_ERR_STA_REG_RX_ERROR2_Pos                       (19U)
#define INTR_ERR_STA_REG_RX_ERROR2_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR2_Pos)
#define INTR_ERR_STA_REG_RX_ERROR2                           INTR_ERR_STA_REG_RX_ERROR2_Msk
#define INTR_ERR_STA_REG_RX_ERROR3_Pos                       (20U)
#define INTR_ERR_STA_REG_RX_ERROR3_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR3_Pos)
#define INTR_ERR_STA_REG_RX_ERROR3                           INTR_ERR_STA_REG_RX_ERROR3_Msk
#define INTR_ERR_STA_REG_RX_ERROR4_Pos                       (21U)
#define INTR_ERR_STA_REG_RX_ERROR4_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR4_Pos)
#define INTR_ERR_STA_REG_RX_ERROR4                           INTR_ERR_STA_REG_RX_ERROR4_Msk
#define INTR_ERR_STA_REG_RX_ERROR5_Pos                       (22U)
#define INTR_ERR_STA_REG_RX_ERROR5_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR5_Pos)
#define INTR_ERR_STA_REG_RX_ERROR5                           INTR_ERR_STA_REG_RX_ERROR5_Msk
#define INTR_ERR_STA_REG_RX_ERROR6_Pos                       (23U)
#define INTR_ERR_STA_REG_RX_ERROR6_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR6_Pos)
#define INTR_ERR_STA_REG_RX_ERROR6                           INTR_ERR_STA_REG_RX_ERROR6_Msk
#define INTR_ERR_STA_REG_RX_ERROR7_Pos                       (24U)
#define INTR_ERR_STA_REG_RX_ERROR7_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR7_Pos)
#define INTR_ERR_STA_REG_RX_ERROR7                           INTR_ERR_STA_REG_RX_ERROR7_Msk
#define INTR_ERR_STA_REG_RX_ERROR8_Pos                       (25U)
#define INTR_ERR_STA_REG_RX_ERROR8_Msk                       (0x1U << INTR_ERR_STA_REG_RX_ERROR8_Pos)
#define INTR_ERR_STA_REG_RX_ERROR8                           INTR_ERR_STA_REG_RX_ERROR8_Msk
#define INTR_ERR_STA_REG_TX_ERROR8_Pos                       (26U)
#define INTR_ERR_STA_REG_TX_ERROR8_Msk                       (0x1U << INTR_ERR_STA_REG_TX_ERROR8_Pos)
#define INTR_ERR_STA_REG_TX_ERROR8                           INTR_ERR_STA_REG_TX_ERROR8_Msk

/* PATH_EN_REG        ,offset：0x234 */
#define PATH_EN_REG_ADC1_Pos                       (0U)
#define PATH_EN_REG_ADC1_Msk                       (0x1U << PATH_EN_REG_ADC1_Pos)
#define PATH_EN_REG_ADC1                           PATH_EN_REG_ADC1_Msk
#define PATH_EN_REG_ADC2_Pos                       (1U)
#define PATH_EN_REG_ADC2_Msk                       (0x1U << PATH_EN_REG_ADC2_Pos)
#define PATH_EN_REG_ADC2                           PATH_EN_REG_ADC2_Msk
#define PATH_EN_REG_ADC3_Pos                       (2U)
#define PATH_EN_REG_ADC3_Msk                       (0x1U << PATH_EN_REG_ADC3_Pos)
#define PATH_EN_REG_ADC3                           PATH_EN_REG_ADC3_Msk
#define PATH_EN_REG_ADC4_Pos                       (3U)
#define PATH_EN_REG_ADC4_Msk                       (0x1U << PATH_EN_REG_ADC4_Pos)
#define PATH_EN_REG_ADC4                           PATH_EN_REG_ADC4_Msk
#define PATH_EN_REG_ADC5_Pos                       (4U)
#define PATH_EN_REG_ADC5_Msk                       (0x1U << PATH_EN_REG_ADC5_Pos)
#define PATH_EN_REG_ADC5                           PATH_EN_REG_ADC5_Msk
#define PATH_EN_REG_ADC6_Pos                       (5U)
#define PATH_EN_REG_ADC6_Msk                       (0x1U << PATH_EN_REG_ADC6_Pos)
#define PATH_EN_REG_ADC6                           PATH_EN_REG_ADC6_Msk
#define PATH_EN_REG_ADC7_Pos                       (6U)
#define PATH_EN_REG_ADC7_Msk                       (0x1U << PATH_EN_REG_ADC7_Pos)
#define PATH_EN_REG_ADC7                           PATH_EN_REG_ADC7_Msk
#define PATH_EN_REG_ADC8_Pos                       (7U)
#define PATH_EN_REG_ADC8_Msk                       (0x1U << PATH_EN_REG_ADC8_Pos)
#define PATH_EN_REG_ADC8                           PATH_EN_REG_ADC8_Msk
#define PATH_EN_REG_DAC_Pos                        (8U)
#define PATH_EN_REG_DAC_Msk                        (0x1U << PATH_EN_REG_DAC_Pos)
#define PATH_EN_REG_DAC                            PATH_EN_REG_DAC_Msk
/* INTR_ERR_CLR_REG   ,offset：0x238 */
#define INTR_ERR_CLR_REG_RX_FULL1_Pos                       (0U)
#define INTR_ERR_CLR_REG_RX_FULL1_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL1_Pos)
#define INTR_ERR_CLR_REG_RX_FULL1                           INTR_ERR_CLR_REG_RX_FULL1_Msk
#define INTR_ERR_CLR_REG_RX_FULL2_Pos                       (1U)
#define INTR_ERR_CLR_REG_RX_FULL2_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL2_Pos)
#define INTR_ERR_CLR_REG_RX_FULL2                           INTR_ERR_CLR_REG_RX_FULL2_Msk
#define INTR_ERR_CLR_REG_RX_FULL3_Pos                       (2U)
#define INTR_ERR_CLR_REG_RX_FULL3_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL3_Pos)
#define INTR_ERR_CLR_REG_RX_FULL3                           INTR_ERR_CLR_REG_RX_FULL3_Msk
#define INTR_ERR_CLR_REG_RX_FULL4_Pos                       (3U)
#define INTR_ERR_CLR_REG_RX_FULL4_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL4_Pos)
#define INTR_ERR_CLR_REG_RX_FULL4                           INTR_ERR_CLR_REG_RX_FULL4_Msk
#define INTR_ERR_CLR_REG_RX_FULL5_Pos                       (4U)
#define INTR_ERR_CLR_REG_RX_FULL5_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL5_Pos)
#define INTR_ERR_CLR_REG_RX_FULL5                           INTR_ERR_CLR_REG_RX_FULL5_Msk
#define INTR_ERR_CLR_REG_RX_FULL6_Pos                       (5U)
#define INTR_ERR_CLR_REG_RX_FULL6_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL6_Pos)
#define INTR_ERR_CLR_REG_RX_FULL6                           INTR_ERR_CLR_REG_RX_FULL6_Msk
#define INTR_ERR_CLR_REG_RX_FULL7_Pos                       (6U)
#define INTR_ERR_CLR_REG_RX_FULL7_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL7_Pos)
#define INTR_ERR_CLR_REG_RX_FULL7                           INTR_ERR_CLR_REG_RX_FULL7_Msk
#define INTR_ERR_CLR_REG_RX_FULL8_Pos                       (7U)
#define INTR_ERR_CLR_REG_RX_FULL8_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_FULL8_Pos)
#define INTR_ERR_CLR_REG_RX_FULL8                           INTR_ERR_CLR_REG_RX_FULL8_Msk
#define INTR_ERR_CLR_REG_RX_TH1_Pos                         (8U)
#define INTR_ERR_CLR_REG_RX_TH1_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH1_Pos)
#define INTR_ERR_CLR_REG_RX_TH1                             INTR_ERR_CLR_REG_RX_TH1_Msk
#define INTR_ERR_CLR_REG_RX_TH2_Pos                         (9U)
#define INTR_ERR_CLR_REG_RX_TH2_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH2_Pos)
#define INTR_ERR_CLR_REG_RX_TH2                             INTR_ERR_CLR_REG_RX_TH2_Msk
#define INTR_ERR_CLR_REG_RX_TH3_Pos                         (10U)
#define INTR_ERR_CLR_REG_RX_TH3_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH3_Pos)
#define INTR_ERR_CLR_REG_RX_TH3                             INTR_ERR_CLR_REG_RX_TH3_Msk
#define INTR_ERR_CLR_REG_RX_TH4_Pos                         (11U)
#define INTR_ERR_CLR_REG_RX_TH4_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH4_Pos)
#define INTR_ERR_CLR_REG_RX_TH4                             INTR_ERR_CLR_REG_RX_TH4_Msk
#define INTR_ERR_CLR_REG_RX_TH5_Pos                         (12U)
#define INTR_ERR_CLR_REG_RX_TH5_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH5_Pos)
#define INTR_ERR_CLR_REG_RX_TH5                             INTR_ERR_CLR_REG_RX_TH5_Msk
#define INTR_ERR_CLR_REG_RX_TH6_Pos                         (13U)
#define INTR_ERR_CLR_REG_RX_TH6_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH6_Pos)
#define INTR_ERR_CLR_REG_RX_TH6                             INTR_ERR_CLR_REG_RX_TH6_Msk
#define INTR_ERR_CLR_REG_RX_TH7_Pos                         (14U)
#define INTR_ERR_CLR_REG_RX_TH7_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH7_Pos)
#define INTR_ERR_CLR_REG_RX_TH7                             INTR_ERR_CLR_REG_RX_TH7_Msk
#define INTR_ERR_CLR_REG_RX_TH8_Pos                         (15U)
#define INTR_ERR_CLR_REG_RX_TH8_Msk                         (0x1U << INTR_ERR_CLR_REG_RX_TH8_Pos)
#define INTR_ERR_CLR_REG_RX_TH8                             INTR_ERR_CLR_REG_RX_TH8_Msk
#define INTR_ERR_CLR_REG_PARA_TX_EMPTY_Pos                  (16U)
#define INTR_ERR_CLR_REG_PARA_TX_EMPTY_Msk                  (0x1U << INTR_ERR_CLR_REG_PARA_TX_EMPTY_Pos)
#define INTR_ERR_CLR_REG_PARA_TX_EMPTY                      INTR_ERR_CLR_REG_PARA_TX_EMPTY_Msk
#define INTR_ERR_CLR_REG_PARA_TX_TH_Pos                     (17U)
#define INTR_ERR_CLR_REG_PARA_TX_TH_Msk                     (0x1U << INTR_ERR_CLR_REG_PARA_TX_TH_Pos)
#define INTR_ERR_CLR_REG_PARA_TX_TH                         INTR_ERR_CLR_REG_PARA_TX_TH_Msk
#define INTR_ERR_CLR_REG_RX_ERROR1_Pos                       (18U)
#define INTR_ERR_CLR_REG_RX_ERROR1_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR1_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR1                           INTR_ERR_CLR_REG_RX_ERROR1_Msk
#define INTR_ERR_CLR_REG_RX_ERROR2_Pos                       (19U)
#define INTR_ERR_CLR_REG_RX_ERROR2_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR2_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR2                           INTR_ERR_CLR_REG_RX_ERROR2_Msk
#define INTR_ERR_CLR_REG_RX_ERROR3_Pos                       (20U)
#define INTR_ERR_CLR_REG_RX_ERROR3_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR3_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR3                           INTR_ERR_CLR_REG_RX_ERROR3_Msk
#define INTR_ERR_CLR_REG_RX_ERROR4_Pos                       (21U)
#define INTR_ERR_CLR_REG_RX_ERROR4_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR4_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR4                           INTR_ERR_CLR_REG_RX_ERROR4_Msk
#define INTR_ERR_CLR_REG_RX_ERROR5_Pos                       (22U)
#define INTR_ERR_CLR_REG_RX_ERROR5_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR5_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR5                           INTR_ERR_CLR_REG_RX_ERROR5_Msk
#define INTR_ERR_CLR_REG_RX_ERROR6_Pos                       (23U)
#define INTR_ERR_CLR_REG_RX_ERROR6_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR6_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR6                           INTR_ERR_CLR_REG_RX_ERROR6_Msk
#define INTR_ERR_CLR_REG_RX_ERROR7_Pos                       (24U)
#define INTR_ERR_CLR_REG_RX_ERROR7_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR7_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR7                           INTR_ERR_CLR_REG_RX_ERROR7_Msk
#define INTR_ERR_CLR_REG_RX_ERROR8_Pos                       (25U)
#define INTR_ERR_CLR_REG_RX_ERROR8_Msk                       (0x1U << INTR_ERR_CLR_REG_RX_ERROR8_Pos)
#define INTR_ERR_CLR_REG_RX_ERROR8                           INTR_ERR_CLR_REG_RX_ERROR8_Msk
#define INTR_ERR_CLR_REG_TX_ERROR8_Pos                       (26U)
#define INTR_ERR_CLR_REG_TX_ERROR8_Msk                       (0x1U << INTR_ERR_CLR_REG_TX_ERROR8_Pos)
#define INTR_ERR_CLR_REG_TX_ERROR8                           INTR_ERR_CLR_REG_TX_ERROR8_Msk

/* COD_MASTER_REG     ,offset：0x23C */
#define CODE_MASTER_REG_MASTER_Pos                       (0U)
#define CODE_MASTER_REG_MASTER_Msk                       (0x1U << CODE_MASTER_REG_MASTER_Pos)
#define CODE_MASTER_REG_MASTER                           CODE_MASTER_REG_MASTER_Msk
/* I2C_ADDR_REG       ,offset：0x240 */
#define I2C_ADDR_REG_I2C_ADDR_Pos                       (0U)
#define I2C_ADDR_REG_I2C_ADDR_Msk                       (0x7FU << I2C_ADDR_REG_I2C_ADDR_Pos)
#define I2C_ADDR_REG_I2C_ADDR                           I2C_ADDR_REG_I2C_ADDR_Msk
/* TRAN_CTRL_REG      ,offset：0x244 */
#define TRAN_CTRL_REG_DMA_REQ_Pos                       (0U)
#define TRAN_CTRL_REG_DMA_REQ_Msk                       (0x1U << TRAN_CTRL_REG_DMA_REQ_Pos)
#define TRAN_CTRL_REG_DMA_REQ                           TRAN_CTRL_REG_DMA_REQ_Msk
/* SAMPLING_CTRL_REG  ,offset：0x248 */
#define SAMPLING_CTRL_REG_TX_BITS_Pos                   (0U)
#define SAMPLING_CTRL_REG_TX_BITS_Msk                   (0x1U << SAMPLING_CTRL_REG_TX_BITS_Pos)
#define SAMPLING_CTRL_REG_TX_24BITS                     SAMPLING_CTRL_REG_TX_BITS_Msk
#define SAMPLING_CTRL_REG_TX_16BITS                     (0U)
#define SAMPLING_CTRL_REG_RX_BITS_Pos                   (1U)
#define SAMPLING_CTRL_REG_RX_BITS_Msk                   (0x1U << SAMPLING_CTRL_REG_RX_BITS_Pos)
#define SAMPLING_CTRL_REG_RX_24BITS                     SAMPLING_CTRL_REG_RX_BITS_Msk
#define SAMPLING_CTRL_REG_RX_16BITS                     (0U)
/* TX_FIFO_CNT_REG    ,offset：0x24C */
#define TX_FIFO_CNT_REG_CNT_Pos                         (0U)
#define TX_FIFO_CNT_REG_CNT_Msk                         (0xFU << TX_FIFO_CNT_REG_CNT_Pos)
#define TX_FIFO_CNT_REG_CNT                             TX_FIFO_CNT_REG_CNT_Msk
#define TX_FIFO_CNT_REG_CNT_0                           (0U)
#define TX_FIFO_CNT_REG_CNT_1                           (1U)
#define TX_FIFO_CNT_REG_CNT_2                           (2U)
#define TX_FIFO_CNT_REG_CNT_3                           (3U)
#define TX_FIFO_CNT_REG_CNT_4                           (4U)
#define TX_FIFO_CNT_REG_CNT_5                           (5U)
#define TX_FIFO_CNT_REG_CNT_6                           (6U)
#define TX_FIFO_CNT_REG_CNT_7                           (7U)
#define TX_FIFO_CNT_REG_CNT_8                           (8U)

/* RX_FIFO_CNT_1_REG  ,offset：0x250 */
#define RX_FIFO_CNT1_REG_CNT1_Pos                         (0U)
#define RX_FIFO_CNT1_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT1_REG_CNT1_Pos)
#define RX_FIFO_CNT1_REG_CNT                             RX_FIFO_CNT1_REG_CNT_Msk
#define RX_FIFO_CNT1_REG_CNT_0                            (0U)
#define RX_FIFO_CNT1_REG_CNT_1                            (1U)
#define RX_FIFO_CNT1_REG_CNT_2                            (2U)
#define RX_FIFO_CNT1_REG_CNT_3                            (3U)
#define RX_FIFO_CNT1_REG_CNT_4                            (4U)
#define RX_FIFO_CNT1_REG_CNT_5                            (5U)
#define RX_FIFO_CNT1_REG_CNT_6                            (6U)
#define RX_FIFO_CNT1_REG_CNT_7                            (7U)
#define RX_FIFO_CNT1_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_2_REG  ,offset：0x254 */
#define RX_FIFO_CNT2_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT2_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT2_REG_CNT_Pos)
#define RX_FIFO_CNT2_REG_CNT                             RX_FIFO_CNT2_REG_CNT_Msk
#define RX_FIFO_CNT2_REG_CNT_0                            (0U)
#define RX_FIFO_CNT2_REG_CNT_1                            (1U)
#define RX_FIFO_CNT2_REG_CNT_2                            (2U)
#define RX_FIFO_CNT2_REG_CNT_3                            (3U)
#define RX_FIFO_CNT2_REG_CNT_4                            (4U)
#define RX_FIFO_CNT2_REG_CNT_5                            (5U)
#define RX_FIFO_CNT2_REG_CNT_6                            (6U)
#define RX_FIFO_CNT2_REG_CNT_7                            (7U)
#define RX_FIFO_CNT2_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_3_REG  ,offset：0x258 */
#define RX_FIFO_CNT3_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT3_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT3_REG_CNT_Pos)
#define RX_FIFO_CNT3_REG_CNT                             RX_FIFO_CNT3_REG_CNT_Msk
#define RX_FIFO_CNT3_REG_CNT_0                            (0U)
#define RX_FIFO_CNT3_REG_CNT_1                            (1U)
#define RX_FIFO_CNT3_REG_CNT_2                            (2U)
#define RX_FIFO_CNT3_REG_CNT_3                            (3U)
#define RX_FIFO_CNT3_REG_CNT_4                            (4U)
#define RX_FIFO_CNT3_REG_CNT_5                            (5U)
#define RX_FIFO_CNT3_REG_CNT_6                            (6U)
#define RX_FIFO_CNT3_REG_CNT_7                            (7U)
#define RX_FIFO_CNT3_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_4_REG  ,offset：0x25C */
#define RX_FIFO_CNT4_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT4_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT4_REG_CNT_Pos)
#define RX_FIFO_CNT4_REG_CNT                             RX_FIFO_CNT4_REG_CNT_Msk
#define RX_FIFO_CNT4_REG_CNT_0                            (0U)
#define RX_FIFO_CNT4_REG_CNT_1                            (1U)
#define RX_FIFO_CNT4_REG_CNT_2                            (2U)
#define RX_FIFO_CNT4_REG_CNT_3                            (3U)
#define RX_FIFO_CNT4_REG_CNT_4                            (4U)
#define RX_FIFO_CNT4_REG_CNT_5                            (5U)
#define RX_FIFO_CNT4_REG_CNT_6                            (6U)
#define RX_FIFO_CNT4_REG_CNT_7                            (7U)
#define RX_FIFO_CNT4_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_5_REG  ,offset：0x260 */
#define RX_FIFO_CNT5_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT5_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT5_REG_CNT_Pos)
#define RX_FIFO_CNT5_REG_CNT                             RX_FIFO_CNT5_REG_CNT_Msk
#define RX_FIFO_CNT5_REG_CNT_0                            (0U)
#define RX_FIFO_CNT5_REG_CNT_1                            (1U)
#define RX_FIFO_CNT5_REG_CNT_2                            (2U)
#define RX_FIFO_CNT5_REG_CNT_3                            (3U)
#define RX_FIFO_CNT5_REG_CNT_4                            (4U)
#define RX_FIFO_CNT5_REG_CNT_5                            (5U)
#define RX_FIFO_CNT5_REG_CNT_6                            (6U)
#define RX_FIFO_CNT5_REG_CNT_7                            (7U)
#define RX_FIFO_CNT5_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_6_REG  ,offset：0x264 */
#define RX_FIFO_CNT6_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT6_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT6_REG_CNT_Pos)
#define RX_FIFO_CNT6_REG_CNT                             RX_FIFO_CNT6_REG_CNT_Msk
#define RX_FIFO_CNT6_REG_CNT_0                            (0U)
#define RX_FIFO_CNT6_REG_CNT_1                            (1U)
#define RX_FIFO_CNT6_REG_CNT_2                            (2U)
#define RX_FIFO_CNT6_REG_CNT_3                            (3U)
#define RX_FIFO_CNT6_REG_CNT_4                            (4U)
#define RX_FIFO_CNT6_REG_CNT_5                            (5U)
#define RX_FIFO_CNT6_REG_CNT_6                            (6U)
#define RX_FIFO_CNT6_REG_CNT_7                            (7U)
#define RX_FIFO_CNT6_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_7_REG  ,offset：0x268 */
#define RX_FIFO_CNT7_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT7_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT7_REG_CNT_Pos)
#define RX_FIFO_CNT7_REG_CNT                             RX_FIFO_CNT7_REG_CNT_Msk
#define RX_FIFO_CNT7_REG_CNT_0                            (0U)
#define RX_FIFO_CNT7_REG_CNT_1                            (1U)
#define RX_FIFO_CNT7_REG_CNT_2                            (2U)
#define RX_FIFO_CNT7_REG_CNT_3                            (3U)
#define RX_FIFO_CNT7_REG_CNT_4                            (4U)
#define RX_FIFO_CNT7_REG_CNT_5                            (5U)
#define RX_FIFO_CNT7_REG_CNT_6                            (6U)
#define RX_FIFO_CNT7_REG_CNT_7                            (7U)
#define RX_FIFO_CNT7_REG_CNT_8                            (8U)
/* RX_FIFO_CNT_8_REG  ,offset：0x26C */
#define RX_FIFO_CNT8_REG_CNT_Pos                         (0U)
#define RX_FIFO_CNT8_REG_CNT_Msk                         (0xFU << RX_FIFO_CNT8_REG_CNT_Pos)
#define RX_FIFO_CNT8_REG_CNT                             RX_FIFO_CNT8_REG_CNT_Msk
#define RX_FIFO_CNT8_REG_CNT_0                            (0U)
#define RX_FIFO_CNT8_REG_CNT_1                            (1U)
#define RX_FIFO_CNT8_REG_CNT_2                            (2U)
#define RX_FIFO_CNT8_REG_CNT_3                            (3U)
#define RX_FIFO_CNT8_REG_CNT_4                            (4U)
#define RX_FIFO_CNT8_REG_CNT_5                            (5U)
#define RX_FIFO_CNT8_REG_CNT_6                            (6U)
#define RX_FIFO_CNT8_REG_CNT_7                            (7U)
#define RX_FIFO_CNT8_REG_CNT_8                            (8U)

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
    uint32_t RESERVED0[2]                        ; /* Offset: 0X30      (R)     RESERVED */
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
    uint32_t RESERVED1[2]                        ; /* Offset: 0XF8      (R)     RESERVED */
    __IOM uint32_t SR_WT                         ; /* Offset: 0x100     (R/W)  SR_WT               Register */
    __IOM uint32_t ICR_WT                        ; /* Offset: 0x104     (R/W)  ICR_WT              Register */
    __IOM uint32_t IMR_WT                        ; /* Offset: 0x108     (R/W)  IMR_WT              Register */
    __IOM uint32_t IFR_WT                        ; /* Offset: 0x10C     (R/W)  IFR_WT              Register */
    __IOM uint32_t CR_WT                         ; /* Offset: 0x110     (R/W)  CR_WT               Register */
    __IOM uint32_t CR_WT_2                       ; /* Offset: 0x114     (R/W)  CR_WT_2             Register */
    __IOM uint32_t CR_WT_3                       ; /* Offset: 0x118     (R/W)  CR_WT_3             Register */
    __IOM uint32_t CR_WT_4                       ; /* Offset: 0x11C     (R/W)  CR_WT_4             Register */
    uint32_t RESERVED2[24]                       ; /* Offset: 0X120     (R)     RESERVED */
    __IOM uint32_t CR_TR                         ; /* Offset: 0x180     (R/W)  CR_TR               Register */
    __IOM uint32_t DR_TR                         ; /* Offset: 0x184     (R/W)  DR_TR               Register */
    __IOM uint32_t SR_TR1                        ; /* Offset: 0x188     (R/W)  SR_TR1              Register */
    __IOM uint32_t SR_TR_SRCDAC                  ; /* Offset: 0x18C     (R/W)  SR_TR_SRCDAC        Register */
    uint32_t RESERVED3[28]                       ; /* Offset: 0X190     (R)     RESERVED */
    __IOM uint32_t  TRAN_SEL_REG;            /* 0x200        0x00000000        传输模式选择寄存器        */
    __IOM uint32_t  PARA_TX_FIFO_REG;        /* 0x204        0x00000000        DAC 路径数据发送寄存器    */
    __IOM uint32_t  RX_FIFO1_REG;            /* 0x208        0x00000000        第一路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO2_REG;            /* 0x20c        0x00000000        第二路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO3_REG;            /* 0x210        0x00000000        第三路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO4_REG;            /* 0x214        0x00000000        第四路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO5_REG;            /* 0x218        0x00000000        第五路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO6_REG;            /* 0x21C        0x00000000        第六路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO7_REG;            /* 0x220        0x00000000        第七路ADC 数据接收寄存器  */
    __IOM uint32_t  RX_FIFO8_REG;            /* 0x224        0x00000000        第八路ADC 数据接收寄存器  */
    __IOM uint32_t  FIFO_TH_CTRL_REG;        /* 0x228        0x00000077        FIFO 门限控制寄存器       */
    __IOM uint32_t  INTR_ERR_CTRL_REG;       /* 0x22C        0x07FFFFFF        中断&错误控制寄存器       */
    __IOM uint32_t  INTR_ERR_STA_REG;        /* 0x230        0x00000000        中断&错误状态寄存器       */
    __IOM uint32_t  PATH_EN_REG;             /* 0x234        0x00000000        八路ADC 和DAC控制寄存器   */
    __IOM uint32_t  INTR_ERR_CLR_REG;        /* 0x238        0x00000000        中断&错误清除寄存器       */
    __IOM uint32_t  COD_MASTER_REG;          /* 0x23C        0x00000000        CODEC 主从控制寄存器      */
    __IOM uint32_t  I2C_ADDR_REG;            /* 0x240        0x00000014        CODEC I2C 地址寄存器      */
    __IOM uint32_t  TRAN_CTRL_REG;           /* 0x244        0x00000001        读取CODEC 数据方式选择寄存*/
    __IOM uint32_t  SAMPLING_CTRL_REG;       /* 0x248        0x00000000        采样率控制寄存器         */
    __IOM uint32_t  TX_FIFO_CNT_REG;         /* 0x24C        0x00000000        DAC FIFO 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT1_REG;        /* 0x250        0x00000000        ADC FIFO1 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT2_REG;        /* 0x254        0x00000000        ADC FIFO2 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT3_REG;        /* 0x258        0x00000000        ADC FIFO3 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT4_REG;        /* 0x25C        0x00000000        ADC FIFO4 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT5_REG;        /* 0x260        0x00000000        ADC FIFO5 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT6_REG;        /* 0x264        0x00000000        ADC FIFO6 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT7_REG;        /* 0x268        0x00000000        ADC FIFO7 内部数据个数寄存*/
    __IOM uint32_t  RX_FIFO_CNT8_REG;        /* 0x26C        0x00000000        ADC FIFO8 内部数据个数寄存*/
} wj_codec_regs_t;

#define CODES_BASE_OFFSET   0x200

static inline uint8_t  wj_codec_get_pon_ack_state(wj_codec_regs_t *addr)
{
    return (addr->SR & SR_REG_PON_ACK);
}

static inline uint8_t  wj_codec_get_vic_pending_state(wj_codec_regs_t *addr)
{
    return (addr->SR & SR_REG_IRQ_PENDING_Msk);
}

static inline uint8_t  wj_codec_get_jack_state(wj_codec_regs_t *addr)
{
    return (addr->SR & SR_REG_JACK_Msk);
}

static inline uint8_t  wj_codec_get_dac_locked_state(wj_codec_regs_t *addr)
{
    return (addr->SR & SR_REG_DAC_LOCKED_Msk);
}

static inline uint8_t  wj_codec_get_dac_unkonw_fs_state(wj_codec_regs_t *addr)
{
    return (addr->SR & SR_REG_DAC_UNKNOW_FS_Msk);
}

static inline uint8_t  wj_codec_get_dac_ramp_in_progress(wj_codec_regs_t *addr)
{
    return (addr->SR & SR2_REG_DAC_MUTE_INPROGRESS_Msk);
}

static inline uint8_t  wj_codec_get_dac_mute_state(wj_codec_regs_t *addr)
{
    return (addr->SR2 & SR2_REG_DAC_MUTE_INPROGRESS_Msk);
}

static inline uint8_t  wj_codec_get_adc78_mute_state(wj_codec_regs_t *addr)
{
    return (addr->SR3 & SR3_REG_ADC78_MUTE_INPROGRESS_Msk);
}

static inline uint8_t  wj_codec_get_adc56_mute_state(wj_codec_regs_t *addr)
{
    return (addr->SR3 & SR3_REG_ADC56_MUTE_INPROGRESS_Msk);
}

static inline uint8_t  wj_codec_get_adc34_mute_state(wj_codec_regs_t *addr)
{
    return (addr->SR3 & SR3_REG_ADC34_MUTE_INPROGRESS_Msk);
}

static inline void  wj_codec_set_int_form(wj_codec_regs_t *addr, uint8_t value)
{
    addr->ICR &= (~ICR_REG_INT_FORM_Msk);
    addr->ICR |= (value & ICR_REG_INT_FORM_Msk);
}

static inline uint8_t  wj_codec_get_int_form(wj_codec_regs_t *addr)
{
    return (addr->ICR & ICR_REG_INT_FORM_Msk);
}

static inline void  wj_codec_en_sc_mask(wj_codec_regs_t *addr)
{
    addr->IMR |= IMR_REG_SC_MASK_EN;
}

static inline void  wj_codec_dis_sc_mask(wj_codec_regs_t *addr)
{
    addr->IMR &= (~IMR_REG_SC_MASK_EN);
}

static inline uint8_t  wj_codec_get_sc_mask(wj_codec_regs_t *addr)
{
    return (addr->IMR & IMR_REG_SC_MASK_Msk);
}

static inline void  wj_codec_en_jack_mask(wj_codec_regs_t *addr)
{
    addr->IMR |= IMR_REG_JACK_MASK_EN;
}

static inline void  wj_codec_dis_jack_mask(wj_codec_regs_t *addr)
{
    addr->IMR &= (~IMR_REG_JACK_MASK_EN);
}

static inline uint8_t  wj_codec_get_jack_mask(wj_codec_regs_t *addr)
{
    return (addr->IMR & IMR_REG_JACK_MASK_Msk);
}


static inline void  wj_codec_en_adas_unloak_mask(wj_codec_regs_t *addr)
{
    addr->IMR |= IMR_REG_ADAS_UNLOCK_MASK_EN;
}

static inline void  wj_codec_dis_adas_unloak_mask(wj_codec_regs_t *addr)
{
    addr->IMR &= (~IMR_REG_ADAS_UNLOCK_MASK_EN);
}

static inline uint8_t  wj_codec_get_adas_unloak_mask(wj_codec_regs_t *addr)
{
    return (addr->IMR & IMR_REG_ADAS_UNLOCK_MASK_EN);
}

static inline void  wj_codec_dis_sc(wj_codec_regs_t *addr)
{
    addr->IFR &= (~IFR_REG_SC_EN);
}

static inline void  wj_codec_en_sc(wj_codec_regs_t *addr)
{
    addr->IFR |= IFR_REG_SC_EN;
}

static inline uint8_t  wj_codec_get_sc(wj_codec_regs_t *addr)
{
    return (addr->IFR & IFR_REG_SC_Msk);
}


static inline void  wj_codec_dis_jack_evnet(wj_codec_regs_t *addr)
{
    addr->IFR &= (~IFR_REG_JACK_EVENT_EN);
}

static inline void  wj_codec_en_jack_evnet(wj_codec_regs_t *addr)
{
    addr->IFR |= IFR_REG_JACK_EVENT_EN;
}

static inline uint8_t  wj_codec_get_jack_evnet(wj_codec_regs_t *addr)
{
    return (addr->IFR & IFR_REG_JACK_EVENT_Msk);
}

static inline void  wj_codec_dis_adas_unlock(wj_codec_regs_t *addr)
{
    addr->IFR &= (~IFR_REG_ADAS_UNLOCK_EN);
}

static inline void  wj_codec_en_adas_unlock(wj_codec_regs_t *addr)
{
    addr->IFR |= IFR_REG_ADAS_UNLOCK_EN;
}

static inline uint8_t  wj_codec_get_adas_unlock(wj_codec_regs_t *addr)
{
    return (addr->IFR & IFR_REG_ADAS_UNLOCK_Msk);
}

static inline void  wj_codec_dis_sb(wj_codec_regs_t *addr)
{
    addr->CR_VIC &= (~CR_VIC_REG_SB_EN);
}

static inline void  wj_codec_en_sb(wj_codec_regs_t *addr)
{
    addr->CR_VIC |= CR_VIC_REG_SB_EN;
}

static inline uint8_t  wj_codec_get_sb(wj_codec_regs_t *addr)
{
    return (addr->CR_VIC & CR_VIC_REG_SB_Msk);
}

static inline void  wj_codec_dis_sb_analog(wj_codec_regs_t *addr)
{
    addr->CR_VIC &= (~CR_VIC_REG_SB_EN);
}

static inline void  wj_codec_en_sb_analog(wj_codec_regs_t *addr)
{
    addr->CR_VIC |= CR_VIC_REG_SB_ANALOG_EN;
}

static inline uint8_t  wj_codec_get_sb_analog(wj_codec_regs_t *addr)
{
    return (addr->CR_VIC & CR_VIC_REG_SB_ANALOG_Msk);
}

static inline void  wj_codec_dis_sleep_analog(wj_codec_regs_t *addr)
{
    addr->CR_VIC &= (~CR_VIC_REG_SLEEP_ANALOG_EN);
}

static inline void  wj_codec_en_sleep_analog(wj_codec_regs_t *addr)
{
    addr->CR_VIC |= CR_VIC_REG_SLEEP_ANALOG_EN;
}

static inline uint8_t  wj_codec_get_sleep_analog(wj_codec_regs_t *addr)
{
    return (addr->CR_VIC & CR_VIC_REG_SLEEP_ANALOG_EN);
}

static inline void  wj_codec_set_crystall(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_CK &= ~CR_CK_REG_CRYSTAL_Msk;
    addr->CR_CK |= value & CR_CK_REG_CRYSTAL_Msk;
}

static inline uint8_t  wj_codec_get_crystall(wj_codec_regs_t *addr)
{
    return (addr->CR_CK & CR_CK_REG_CRYSTAL_Msk);
}

static inline void  wj_codec_set_dac_audioif(wj_codec_regs_t *addr, uint8_t value)
{
    addr->AICR_DAC &= ~AICR_DAC_REG_DAC_AUDIOIF_Msk;
    addr->AICR_DAC |= value & AICR_DAC_REG_DAC_AUDIOIF_Msk;
}

static inline uint8_t  wj_codec_get_dac_audioif(wj_codec_regs_t *addr)
{
    return (addr->AICR_DAC & AICR_DAC_REG_DAC_AUDIOIF_Msk);
}

static inline void  wj_codec_en_sb_aicr_dac(wj_codec_regs_t *addr, uint8_t value)
{
    addr->AICR_DAC &= ~AICR_DAC_REG_DAC_ADWL_Msk;
    addr->AICR_DAC |= value & AICR_DAC_REG_DAC_ADWL_Msk;
}

static inline uint8_t  wj_codec_get_dac_adwl(wj_codec_regs_t *addr)
{
    return (addr->AICR_DAC & AICR_DAC_REG_DAC_ADWL_Msk);
}

static inline void  wj_codec_set_dac_slave(wj_codec_regs_t *addr)
{
    addr->AICR_DAC |= AICR_DAC_REG_DAC_SLAVE_Msk;
}

static inline void  wj_codec_set_dac_master(wj_codec_regs_t *addr)
{
    addr->AICR_DAC &= ~AICR_DAC_REG_DAC_SLAVE_Msk;
}


static inline uint8_t  wj_codec_get_dac_slave(wj_codec_regs_t *addr)
{
    return (addr->AICR_DAC & AICR_DAC_REG_DAC_SLAVE_Msk);
}

static inline void  wj_codec_close_sb_aicr_dac(wj_codec_regs_t *addr)
{
    addr->AICR_DAC |= AICR_DAC_REG_SB_AICR_DAC_Msk;
}

static inline void  wj_codec_open_sb_aicr_dac(wj_codec_regs_t *addr)
{
    addr->AICR_DAC &= ~AICR_DAC_REG_SB_AICR_DAC_Msk;
}

static inline uint8_t  wj_codec_get_sb_aicr_dac(wj_codec_regs_t *addr)
{
    return (addr->AICR_DAC & AICR_DAC_REG_SB_AICR_DAC_Msk);
}

static inline void  wj_codec_set_adc_audioif(wj_codec_regs_t *addr, uint8_t value)
{
    addr->AICR_ADC &= ~AICR_ADC_REG_ADC_AUDIOIF_Msk;
    addr->AICR_ADC |= value & AICR_ADC_REG_ADC_AUDIOIF_Msk;
}

static inline uint8_t  wj_codec_get_adc_audioif(wj_codec_regs_t *addr)
{
    return (addr->AICR_ADC & AICR_ADC_REG_ADC_AUDIOIF_Msk);
}

static inline void  wj_codec_set_adc_adwl(wj_codec_regs_t *addr, uint8_t value)
{
    addr->AICR_ADC &= ~AICR_ADC_REG_ADC_ADWL_Msk;
    addr->AICR_ADC |= value & AICR_ADC_REG_ADC_ADWL_Msk;
}

static inline uint8_t  wj_codec_get_adc_adwl(wj_codec_regs_t *addr)
{
    return (addr->AICR_ADC & AICR_ADC_REG_ADC_ADWL_Msk);
}

static inline void  wj_codec_close_sb_aicr_adc12(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 |= AICR_ADC_2_REG_SB_AICR_ADC12_EN;
}

static inline void  wj_codec_open_sb_aicr_adc12(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 &= ~AICR_ADC_2_REG_SB_AICR_ADC12_EN;
}

static inline uint8_t  wj_codec_get_sb_aicr_adc12(wj_codec_regs_t *addr)
{
    return (addr->AICR_ADC_2 & AICR_ADC_2_REG_SB_AICR_ADC12_Msk);
}

static inline void  wj_codec_close_sb_aicr_adc34(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 |= AICR_ADC_2_REG_SB_AICR_ADC34_EN;
}

static inline void  wj_codec_open_sb_aicr_adc34(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 &= ~AICR_ADC_2_REG_SB_AICR_ADC34_EN;
}

static inline uint8_t  wj_codec_get_sb_aicr_adc34(wj_codec_regs_t *addr)
{
    return (addr->AICR_ADC_2 & AICR_ADC_2_REG_SB_AICR_ADC34_Msk);
}

static inline void  wj_codec_close_sb_aicr_adc56(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 |= AICR_ADC_2_REG_SB_AICR_ADC56_EN;
}

static inline void  wj_codec_open_sb_aicr_adc56(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 &= ~AICR_ADC_2_REG_SB_AICR_ADC56_EN;
}

static inline uint8_t  wj_codec_get_sb_aicr_adc56(wj_codec_regs_t *addr)
{
    return (addr->AICR_ADC_2 & AICR_ADC_2_REG_SB_AICR_ADC56_Msk);
}

static inline void  wj_codec_close_sb_aicr_adc78(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 |= AICR_ADC_2_REG_SB_AICR_ADC78_EN;
}

static inline void  wj_codec_open_sb_aicr_adc78(wj_codec_regs_t *addr)
{
    addr->AICR_ADC_2 &= ~AICR_ADC_2_REG_SB_AICR_ADC78_EN;
}

static inline uint8_t  wj_codec_get_sb_aicr_adc78(wj_codec_regs_t *addr)
{
    return (addr->AICR_ADC_2 & AICR_ADC_2_REG_SB_AICR_ADC78_Msk);
}

static inline void  wj_codec_set_dac_freq(wj_codec_regs_t *addr, uint8_t value)
{
    addr->FCR_DAC &= ~FCR_DAC_REG_DAC_FREQ_Msk;
    addr->FCR_DAC |= value & FCR_DAC_REG_DAC_FREQ_Msk;
}

static inline uint8_t  wj_codec_get_dac_freq(wj_codec_regs_t *addr)
{
    return (addr->FCR_DAC & FCR_DAC_REG_DAC_FREQ_Msk);
}

static inline void  wj_codec_set_adc12_wnf(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_WNF &= ~CR_WNF_REG_ADC12_WNF_Msk;
    addr->CR_WNF |= value & CR_WNF_REG_ADC12_WNF_Msk;
}

static inline uint8_t  wj_codec_get_adc12_wnf(wj_codec_regs_t *addr)
{
    return (addr->CR_WNF & CR_WNF_REG_ADC12_WNF_Msk);
}

static inline void  wj_codec_set_adc34_wnf(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_WNF &= ~CR_WNF_REG_ADC34_WNF_Msk;
    addr->CR_WNF |= value & CR_WNF_REG_ADC34_WNF_Msk;
}

static inline uint8_t  wj_codec_get_adc34_wnf(wj_codec_regs_t *addr)
{
    return (addr->CR_WNF & CR_WNF_REG_ADC34_WNF_Msk);
}

static inline void  wj_codec_set_adc56_wnf(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_WNF &= ~CR_WNF_REG_ADC56_WNF_Msk;
    addr->CR_WNF |= value & CR_WNF_REG_ADC56_WNF_Msk;
}

static inline uint8_t  wj_codec_get_adc56_wnf(wj_codec_regs_t *addr)
{
    return (addr->CR_WNF & CR_WNF_REG_ADC56_WNF_Msk);
}

static inline void  wj_codec_set_adc78_wnf(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_WNF &= ~CR_WNF_REG_ADC78_WNF_Msk;
    addr->CR_WNF |= value & CR_WNF_REG_ADC78_WNF_Msk;
}

static inline uint8_t  wj_codec_get_adc78_wnf(wj_codec_regs_t *addr)
{
    return (addr->CR_WNF & CR_WNF_REG_ADC78_WNF_Msk);
}

static inline void  wj_codec_open_adc12_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC |= FCR_ADC_REG_ADC12_HPF_EN;
}

static inline void  wj_codec_close_adc12_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC &= ~FCR_ADC_REG_ADC12_HPF_EN;
}

static inline uint8_t  wj_codec_get_adc12_hpf(wj_codec_regs_t *addr)
{
    return (addr->FCR_ADC & FCR_ADC_REG_ADC12_HPF_EN_Msk);
}

static inline void  wj_codec_open_adc34_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC |= FCR_ADC_REG_ADC34_HPF_EN;
}

static inline void  wj_codec_close_adc34_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC &= ~FCR_ADC_REG_ADC34_HPF_EN;
}

static inline uint8_t  wj_codec_get_adc34_hpf(wj_codec_regs_t *addr)
{
    return (addr->FCR_ADC & FCR_ADC_REG_ADC34_HPF_EN_Msk);
}
static inline void  wj_codec_open_adc56_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC |= FCR_ADC_REG_ADC56_HPF_EN;
}

static inline void  wj_codec_close_adc56_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC &= ~FCR_ADC_REG_ADC56_HPF_EN;
}

static inline uint8_t  wj_codec_get_adc56_hpf(wj_codec_regs_t *addr)
{
    return (addr->FCR_ADC & FCR_ADC_REG_ADC56_HPF_EN_Msk);
}

static inline void  wj_codec_open_adc78_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC |= FCR_ADC_REG_ADC78_HPF_EN;
}

static inline void  wj_codec_close_adc78_hpf(wj_codec_regs_t *addr)
{
    addr->FCR_ADC &= ~FCR_ADC_REG_ADC78_HPF_EN;
}

static inline uint8_t  wj_codec_get_adc78_hpf(wj_codec_regs_t *addr)
{
    return (addr->FCR_ADC & FCR_ADC_REG_ADC78_HPF_EN_Msk);
}

static inline void  wj_codec_set_adc_freq(wj_codec_regs_t *addr, uint8_t value)
{
    addr->FCR_ADC &= ~FCR_ADC_REG_ADC_FREQ_Msk;
    addr->FCR_ADC |= value & FCR_ADC_REG_ADC_FREQ_Msk;
}

static inline uint8_t  wj_codec_get_adc_freq(wj_codec_regs_t *addr)
{
    return (addr->FCR_ADC & FCR_ADC_REG_ADC_FREQ_Msk);
}


static inline void  wj_codec_set_adc_dmic_sel2(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC12 &= ~CR_DMIC12_REG_ADC_DMIC_SEL2_Msk;
    addr->CR_DMIC12 |= value & CR_DMIC12_REG_ADC_DMIC_SEL2_Msk;
}

static inline uint8_t  wj_codec_get_adc_dmic_sel2(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC12 & CR_DMIC12_REG_ADC_DMIC_SEL2_Msk);
}

static inline void  wj_codec_set_adc_dmic_sel1(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC12 &= ~CR_DMIC12_REG_ADC_DMIC_SEL1_Msk;
    addr->CR_DMIC12 |= value & CR_DMIC12_REG_ADC_DMIC_SEL1_Msk;
}

static inline uint8_t  wj_codec_get_adc_dmic_sel1(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC12 & CR_DMIC12_REG_ADC_DMIC_SEL1_Msk);
}

static inline void  wj_codec_set_dmic_rate(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC12 &= ~CR_DMIC12_REG_ADC_DMIC_RATE_Msk;
    addr->CR_DMIC12 |= value & CR_DMIC12_REG_ADC_DMIC_RATE_Msk;
}

static inline uint8_t  wj_codec_get_dmic_rate(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC12 & CR_DMIC12_REG_ADC_DMIC_RATE_Msk);
}

static inline void  wj_codec_open_sb_dmic2(wj_codec_regs_t *addr)
{
    addr->CR_DMIC12 &= ~CR_DMIC12_REG_SB_DMIC2_EN;
}

static inline void  wj_codec_close_sb_dmic2(wj_codec_regs_t *addr)
{
    addr->CR_DMIC12 |= CR_DMIC12_REG_SB_DMIC2_EN;
}

static inline uint8_t  wj_codec_get_sb_dmic2(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC12 & CR_DMIC12_REG_SB_DMIC2_Msk);
}

static inline void  wj_codec_open_sb_dmic1(wj_codec_regs_t *addr)
{
    addr->CR_DMIC12 &= ~CR_DMIC12_REG_SB_DMIC1_EN;
}

static inline void  wj_codec_close_sb_dmic1(wj_codec_regs_t *addr)
{
    addr->CR_DMIC12 |= CR_DMIC12_REG_SB_DMIC1_EN;
}

static inline uint8_t  wj_codec_get_sb_dmic1(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC12 & CR_DMIC12_REG_SB_DMIC1_Msk);
}



static inline void  wj_codec_set_adc_dmic_sel6(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC56 &= ~CR_DMIC56_REG_ADC_DMIC_SEL6_Msk;
    addr->CR_DMIC56 |= value & CR_DMIC56_REG_ADC_DMIC_SEL6_Msk;
}

static inline uint8_t  wj_codec_get_adc_dmic_sel6(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC56 & CR_DMIC56_REG_ADC_DMIC_SEL6_Msk);
}

static inline void  wj_codec_set_adc_dmic_sel5(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC56 &= ~CR_DMIC56_REG_ADC_DMIC_SEL5_Msk;
    addr->CR_DMIC56 |= value & CR_DMIC56_REG_ADC_DMIC_SEL5_Msk;
}

static inline uint8_t  wj_codec_get_adc_dmic_sel5(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC56 & CR_DMIC56_REG_ADC_DMIC_SEL5_Msk);
}

static inline void  wj_codec_open_sb_dmic6(wj_codec_regs_t *addr)
{
    addr->CR_DMIC56 &= ~CR_DMIC56_REG_SB_DMIC6_EN;
}

static inline void  wj_codec_close_sb_dmic6(wj_codec_regs_t *addr)
{
    addr->CR_DMIC56 |= CR_DMIC56_REG_SB_DMIC6_EN;
}

static inline uint8_t  wj_codec_get_sb_dmic6(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC56 & CR_DMIC56_REG_SB_DMIC6_Msk);
}

static inline void  wj_codec_open_sb_dmic5(wj_codec_regs_t *addr)
{
    addr->CR_DMIC56 &= ~CR_DMIC56_REG_SB_DMIC5_EN;
}

static inline void  wj_codec_close_sb_dmic5(wj_codec_regs_t *addr)
{
    addr->CR_DMIC56 |= CR_DMIC56_REG_SB_DMIC5_EN;
}

static inline uint8_t  wj_codec_get_sb_dmic5(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC56 & CR_DMIC56_REG_SB_DMIC5_Msk);
}


static inline void  wj_codec_set_adc_dmic_sel8(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC78 &= ~CR_DMIC78_REG_ADC_DMIC_SEL8_Msk;
    addr->CR_DMIC78 |= value & CR_DMIC78_REG_ADC_DMIC_SEL8_Msk;
}

static inline uint8_t  wj_codec_get_adc_dmic_sel8(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC78 & CR_DMIC78_REG_ADC_DMIC_SEL8_Msk);
}

static inline void  wj_codec_set_adc_dmic_sel7(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_DMIC78 &= ~CR_DMIC78_REG_ADC_DMIC_SEL7_Msk;
    addr->CR_DMIC78 |= value & CR_DMIC78_REG_ADC_DMIC_SEL7_Msk;
}

static inline uint8_t  wj_codec_get_adc_dmic_sel7(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC78 & CR_DMIC78_REG_ADC_DMIC_SEL7_Msk);
}

static inline void  wj_codec_open_sb_dmic8(wj_codec_regs_t *addr)
{
    addr->CR_DMIC78 &= ~CR_DMIC78_REG_SB_DMIC8_EN;
}

static inline void  wj_codec_close_sb_dmic8(wj_codec_regs_t *addr)
{
    addr->CR_DMIC78 |= CR_DMIC78_REG_SB_DMIC8_EN;
}

static inline uint8_t  wj_codec_get_sb_dmic8(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC78 & CR_DMIC78_REG_SB_DMIC8_Msk);
}

static inline void  wj_codec_open_sb_dmic7(wj_codec_regs_t *addr)
{
    addr->CR_DMIC78 &= ~CR_DMIC78_REG_SB_DMIC7_EN;
}

static inline void  wj_codec_close_sb_dmic7(wj_codec_regs_t *addr)
{
    addr->CR_DMIC78 |= CR_DMIC78_REG_SB_DMIC7_EN;
}

static inline uint8_t  wj_codec_get_sb_dmic7(wj_codec_regs_t *addr)
{
    return (addr->CR_DMIC78 & CR_DMIC78_REG_SB_DMIC7_Msk);
}



static inline void  wj_codec_open_sb_hp(wj_codec_regs_t *addr)
{
    addr->CR_HP &= ~CR_HP_REG_SB_HP_OPEN_EN;
}

static inline void  wj_codec_close_sb_hp(wj_codec_regs_t *addr)
{
    addr->CR_HP |= CR_HP_REG_SB_HP_OPEN_EN;
}

static inline uint8_t  wj_codec_get_sb_hp(wj_codec_regs_t *addr)
{
    return (addr->CR_HP & CR_HP_REG_SB_HP_OPEN_Msk);
}

static inline void  wj_codec_open_sb_mute(wj_codec_regs_t *addr)
{
    addr->CR_HP &= ~CR_HP_REG_HP_MUTE_EN;
}

static inline void  wj_codec_close_sb_mute(wj_codec_regs_t *addr)
{
    addr->CR_HP |= CR_HP_REG_HP_MUTE_EN;
}

static inline uint8_t  wj_codec_get_sb_mute(wj_codec_regs_t *addr)
{
    return (addr->CR_HP & CR_HP_REG_HP_MUTE_Msk);
}


static inline void  wj_codec_set_gol(wj_codec_regs_t *addr, uint8_t value)
{
    addr->GCR_HPL &= ~GCR_HPL_REG_GOL_Msk;
    addr->GCR_HPL |= value & GCR_HPL_REG_GOL_Msk;
}

static inline uint8_t  wj_codec_get_gol(wj_codec_regs_t *addr)
{
    return (addr->GCR_HPL & GCR_HPL_REG_GOL_Msk);
}


static inline void  wj_codec_set_gor(wj_codec_regs_t *addr, uint8_t value)
{
    addr->GCR_HPR &= ~GCR_HPR_REG_GOR_Msk;
    addr->GCR_HPR |= value & GCR_HPR_REG_GOR_Msk;
}

static inline uint8_t  wj_codec_get_gor(wj_codec_regs_t *addr)
{
    return (addr->GCR_HPR & GCR_HPR_REG_GOR_Msk);
}

static inline void  wj_codec_open_sb_micbias1(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_MIC[index] &= ~CR_MIC1_REG_SB_MICBIAS1_CLOSE;
}

static inline void  wj_codec_close_sb_micbias1(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_MIC[index] |= CR_MIC1_REG_SB_MICBIAS1_CLOSE;
}

static inline uint8_t  wj_codec_get_sb_micbias1(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->CR_MIC[index] & CR_MIC1_REG_SB_MICBIAS1_CLOSE);
}


static inline void  wj_codec_set_differential(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_MIC[index] |= CR_MIC1_REG_SB_MICDIFF1_SINGLE;
}

static inline void  wj_codec_set_single_end(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_MIC[index] &= ~CR_MIC1_REG_SB_MICDIFF1_SINGLE;
}

static inline uint8_t  wj_codec_get_micdiff1(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->CR_MIC[index] & CR_MIC1_REG_SB_MICDIFF1_SINGLE);
}

static inline void  wj_codec_set_micbias1_1p8v(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_MIC[index] |= CR_MIC1_REG_SB_MICBIAS1_V_1P8;
}

static inline void  wj_codec_set_micbias1_1p5v(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_MIC[index] &= ~CR_MIC1_REG_SB_MICBIAS1_V_1P8;
}

static inline uint8_t  wj_codec_get_micbias1(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->CR_MIC[index] & CR_MIC1_REG_SB_MICBIAS1_V_Msk);
}


static inline void  wj_codec_set_analog_gain(wj_codec_regs_t *addr, uint8_t index, uint8_t value)
{
    addr->GCR_MIC[index << 1] &= ~(((index % 2 == 1) ? GCR_MIC_REG_GIM1_Msk : GCR_MIC_REG_GIM2_Msk));
    addr->GCR_MIC[index << 1] = (value << ((index % 2 == 1) ? GCR_MIC_REG_GIM1_Pos : GCR_MIC_REG_GIM2_Pos)) & (((index % 2 == 1) ? GCR_MIC_REG_GIM1_Msk : GCR_MIC_REG_GIM2_Msk));

}

static inline uint8_t  wj_codec_get_analog_gain(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->GCR_MIC[index << 1] & ((index % 2 == 1) ? GCR_MIC_REG_GIM1_Msk : GCR_MIC_REG_GIM2_Msk));
}


static inline void  wj_codec_open_dac(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_DAC &= ~CR_DAC_REG_SB_DAC_EN;
}

static inline void  wj_codec_close_dac(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_DAC |= CR_DAC_REG_SB_DAC_EN;
}

static inline uint8_t  wj_codec_get_dac(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->CR_DAC & CR_DAC_REG_SB_DAC_Msk);
}


static inline void  wj_codec_open_dac_left_right(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_DAC &= ~CR_DAC_REG_DAC_LEFT_ONLY_EN;
}

static inline void  wj_codec_close_dac_left(wj_codec_regs_t *addr, uint8_t index)
{
    addr->CR_DAC |= CR_DAC_REG_DAC_LEFT_ONLY_EN;
}

static inline uint8_t  wj_codec_get_dac_state(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->CR_DAC & CR_DAC_REG_DAC_LEFT_ONLY_Msk);
}


static inline void  wj_codec_open_dac_soft_mute(wj_codec_regs_t *addr)
{
    addr->CR_DAC |= CR_DAC_REG_DAC_SOFT_MUTE_EN;
}

static inline void  wj_codec_close_dac_soft_mute(wj_codec_regs_t *addr)
{
    addr->CR_DAC &= ~CR_DAC_REG_DAC_SOFT_MUTE_EN;
}

static inline uint8_t  wj_codec_get_dac_soft_mute(wj_codec_regs_t *addr)
{
    return (addr->CR_DAC & CR_DAC_REG_DAC_SOFT_MUTE_Msk);
}

static inline void  wj_codec_open_adc_open(wj_codec_regs_t *addr, uint8_t index)
{
    if (index % 2 == 1) {
        addr->CR_ADC[index << 1] |= CR_ADC_REG_SB_ADC1_OPEN;
    } else {
        addr->CR_ADC[index << 1] |= CR_ADC_REG_SB_ADC2_OPEN;
    }
}

static inline void  wj_codec_close_adc_close(wj_codec_regs_t *addr, uint8_t index)
{

    if (index % 2 == 1) {
        addr->CR_ADC[index << 1] &= ~CR_ADC_REG_SB_ADC1_OPEN;
    } else {
        addr->CR_ADC[index << 1] |= CR_ADC_REG_SB_ADC2_OPEN;
    }
}

static inline uint8_t  wj_codec_get_adc_state(wj_codec_regs_t *addr, uint8_t index)
{

    if (index % 2 == 1) {
        addr->CR_ADC[index << 1] &= ~CR_ADC_REG_SB_ADC1_OPEN;
    } else {
        addr->CR_ADC[index << 1] |= CR_ADC_REG_SB_ADC2_OPEN;
    }

    return (addr->CR_ADC[index << 1] & ((index % 2 == 1) ? CR_ADC_REG_SB_ADC1_Msk : CR_ADC_REG_SB_ADC2_Msk));
}


static inline void  wj_codec_set_lrgod_dependence(wj_codec_regs_t *addr)
{
    addr->GCR_DACL &= ~GCR_DACL_REG_LRGOD_TOGETHER;
}

static inline void  wj_codec_set_lrgod_together(wj_codec_regs_t *addr)
{
    addr->GCR_DACL |= GCR_DACL_REG_LRGOD_TOGETHER;
}

static inline uint8_t  wj_codec_get_lrgod(wj_codec_regs_t *addr)
{
    return (addr->GCR_DACL & GCR_DACL_REG_LRGOD_Msk);
}

static inline void  wj_codec_set_godl(wj_codec_regs_t *addr, uint32_t value)
{
    addr->GCR_DACL &= ~ GCR_DACL_REG_CODL_Msk ;
    addr->GCR_DACL |= GCR_DACL_REG_CODL_Msk & value;
}

static inline uint8_t  wj_codec_get_godl(wj_codec_regs_t *addr)
{
    return (addr->GCR_DACL & GCR_DACL_REG_CODL_Msk);
}


static inline void  wj_codec_set_godr(wj_codec_regs_t *addr, uint8_t index, uint8_t value)
{
    addr->GCR_ADC[index] &= ~ GCR_ADC1_REG_GID1_Msk ;
    addr->GCR_ADC[index] |= GCR_ADC1_REG_GID1_Msk & value;
}

static inline uint8_t  wj_codec_get_godr(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->GCR_ADC[index] & GCR_ADC1_REG_GID1_Msk);
}

static inline void  wj_codec_set_lrgid_together(wj_codec_regs_t *addr, uint8_t index)
{
    addr->GCR_ADC[index] &= ~ GCR_ADC1_REG_LRGID_Msk ;

}

static inline void  wj_codec_set_lrgid_independence(wj_codec_regs_t *addr, uint8_t index)
{
    addr->GCR_ADC[index] &= ~ GCR_ADC1_REG_LRGID_Msk ;
}
static inline uint8_t  wj_codec_get_lrgid(wj_codec_regs_t *addr, uint8_t index)
{
    return (addr->GCR_ADC[index] & GCR_ADC1_REG_LRGID_Msk);
}

static inline void  wj_codec_set_gomixl(wj_codec_regs_t *addr, uint8_t value)
{
    addr->GCR_MIXDACL &= ~ GCR_MIXDACL_REG_GOMIXL_Msk ;
    addr->GCR_MIXDACL |= GCR_MIXDACL_REG_GOMIXL_Msk & value;
}

static inline uint8_t  wj_codec_get_gomixl(wj_codec_regs_t *addr)
{
    return (addr->GCR_MIXDACL & GCR_MIXDACL_REG_GOMIXL_Msk);
}

static inline void  wj_codec_set_lrgomix_together(wj_codec_regs_t *addr)
{
    addr->GCR_MIXDACL &= ~ GCR_MIXDACL_REG_LRGOMIX_Msk ;

}

static inline void  wj_codec_set_lrgomix_independence(wj_codec_regs_t *addr)
{
    addr->GCR_MIXDACL &= ~ GCR_MIXDACL_REG_LRGOMIX_Msk ;
}

static inline uint8_t  wj_codec_get_lrgomix(wj_codec_regs_t *addr)
{
    return (addr->GCR_MIXDACL & GCR_MIXDACL_REG_LRGOMIX_Msk);
}


static inline void  wj_codec_set_gomixr(wj_codec_regs_t *addr, uint8_t value)
{
    addr->GCR_MIXDACR &= ~ GCR_MIXDACR_REG_GOMIXR_Msk ;
    addr->GCR_MIXDACR |= GCR_MIXDACR_REG_GOMIXR_Msk & value;
}

static inline uint8_t  wj_codec_get_gomixr(wj_codec_regs_t *addr)
{
    return (addr->GCR_MIXDACR & GCR_MIXDACR_REG_GOMIXR_Msk);
}

static inline void  wj_codec_set_gimixl(wj_codec_regs_t *addr, uint8_t value)
{
    addr->GCR_MIXADCL &= ~ GCR_MIXADCL_REG_GIMIXL_Msk ;
    addr->GCR_MIXADCL |=  GCR_MIXADCL_REG_GIMIXL_Msk & value;
}

static inline uint8_t  wj_codec_get_gimixl(wj_codec_regs_t *addr)
{
    return (addr->GCR_MIXADCL & GCR_MIXADCL_REG_GIMIXL_Msk);
}

static inline void  wj_codec_set_lrgimix_together(wj_codec_regs_t *addr)
{
    addr->GCR_MIXADCL &= ~ GCR_MIXADCL_REG_LRGIMIX_Msk ;

}

static inline void  wj_codec_set_lrgimix_independence(wj_codec_regs_t *addr)
{
    addr->GCR_MIXADCL &= ~ GCR_MIXADCL_REG_LRGIMIX_Msk ;
}

static inline uint8_t  wj_codec_get_lrgimix(wj_codec_regs_t *addr)
{
    return (addr->GCR_MIXADCL & GCR_MIXADCL_REG_LRGIMIX_Msk);
}

static inline void  wj_codec_set_gimixr(wj_codec_regs_t *addr, uint8_t value)
{
    addr->GCR_MIXADCR &= ~ GCR_MIXADCR_REG_GIMIXR_Msk ;
    addr->GCR_MIXADCR |= GCR_MIXADCR_REG_GIMIXR_Msk & value;
}

static inline uint8_t  wj_codec_get_gimixr(wj_codec_regs_t *addr)
{
    return (addr->GCR_MIXADCR & GCR_MIXADCR_REG_GIMIXR_Msk);
}


static inline void  wj_codec_set_wt_int_form(wj_codec_regs_t *addr, uint8_t value)
{
    addr->ICR_WT &= ~ ICR_WT_REG_WT_INT_FORM_Msk ;
    addr->ICR_WT |= ICR_WT_REG_WT_INT_FORM_Msk & value;
}

static inline uint8_t  wj_codec_get_wt_int_form(wj_codec_regs_t *addr)
{
    return (addr->ICR_WT & ICR_WT_REG_WT_INT_FORM_Msk);
}


static inline void  wj_codec_en_whisper(wj_codec_regs_t *addr)
{
    addr->CR_WT |= CR_WT_REG_WT_EN;
}

static inline void  wj_codec_dis_whisper(wj_codec_regs_t *addr)
{
    addr->CR_WT &= ~CR_WT_REG_WT_EN;
}

static inline uint8_t  wj_codec_get_whisper(wj_codec_regs_t *addr)
{
    return (addr->CR_WT & CR_WT_REG_WT_EN_Msk);
}

static inline void  wj_codec_en_wt_only(wj_codec_regs_t *addr)
{
    addr->CR_WT |= CR_WT_REG_WT_ONLY_EN;
}

static inline void  wj_codec_dis_wt_only(wj_codec_regs_t *addr)
{
    addr->CR_WT &= ~CR_WT_REG_WT_ONLY_EN;
}

static inline uint8_t  wj_codec_get_wt_only(wj_codec_regs_t *addr)
{
    return (addr->CR_WT & CR_WT_REG_WT_ONLY_Msk);
}

static inline void  wj_codec_en_wt_restart(wj_codec_regs_t *addr)
{
    addr->CR_WT |= CR_WT_REG_WT_RESTART_EN;
}

static inline void  wj_codec_dis_wt_restart(wj_codec_regs_t *addr)
{
    addr->CR_WT &= ~CR_WT_REG_WT_RESTART_EN;
}

static inline uint8_t  wj_codec_get_wt_restart(wj_codec_regs_t *addr)
{
    return (addr->CR_WT & CR_WT_REG_WT_RESTART_Msk);
}


static inline void  wj_codec_en_wt_adc1(wj_codec_regs_t *addr)
{
    addr->CR_WT_2 |= CR_WT2_REG_WT_ADC_SEL_ADC1;
}

static inline void  wj_codec_dis_wt_digit(wj_codec_regs_t *addr)
{
    addr->CR_WT_2 &= ~CR_WT2_REG_WT_ADC_SEL_ADC1;
}

static inline uint8_t  wj_codec_get_wt(wj_codec_regs_t *addr)
{
    return (addr->CR_WT_2 & CR_WT2_REG_WT_ADC_SEL_Msk);
}

static inline void  wj_codec_en_wt_zcd(wj_codec_regs_t *addr)
{
    addr->CR_WT_2 |= CR_WT2_REG_ZCD_EN;
}

static inline void  wj_codec_dis_wt_zcd(wj_codec_regs_t *addr)
{
    addr->CR_WT_2 &= ~CR_WT2_REG_ZCD_EN;
}

static inline uint8_t  wj_codec_get_zcd(wj_codec_regs_t *addr)
{
    return (addr->CR_WT_2 & CR_WT2_REG_ZCD_Msk);
}

static inline void  wj_codec_set_dmic_freq(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_WT_2 &= ~ CR_WT2_REG_WT_DMIC_FREQ_Msk ;
    addr->CR_WT_2 |= CR_WT2_REG_WT_DMIC_FREQ_Msk & value;
}

static inline uint8_t  wj_codec_get_dmic_freq(wj_codec_regs_t *addr)
{
    return (addr->CR_WT_2 & CR_WT2_REG_WT_DMIC_FREQ_Msk);
}

static inline void  wj_codec_set_power_sens(wj_codec_regs_t *addr, uint8_t value)
{
    addr->CR_WT_3 &= ~ CR_WT_3_REG_WT_POWER_SENS_Msk ;
    addr->CR_WT_3 |= CR_WT_3_REG_WT_POWER_SENS_Msk & value;
}

static inline uint8_t  wj_codec_get_power_sens(wj_codec_regs_t *addr)
{
    return (addr->CR_WT_3 & CR_WT_3_REG_WT_POWER_SENS_Msk);
}



static inline void  wj_codec_set_i2c_mode(wj_codec_regs_t *addr)
{
    addr->TRAN_SEL_REG &= (~TRAN_SEL_REG_SI2C_Msk);
    addr->TRAN_SEL_REG = TRAN_SEL_REG_I2C;
}

static inline void  wj_codec_set_uc_mode(wj_codec_regs_t *addr)
{
    addr->TRAN_SEL_REG &= (~TRAN_SEL_REG_SI2C_Msk);
    addr->TRAN_SEL_REG = TRAN_SEL_REG_uC;
}

static inline uint32_t  wj_codec_get_trans_mode(wj_codec_regs_t *addr)
{
    return (addr->TRAN_SEL_REG & TRAN_SEL_REG_SI2C_Msk);
}

static inline void  wj_codec_set_tx_fifo_ex(wj_codec_regs_t *addr, uint32_t left_value, uint32_t right_value)
{
    addr->PARA_TX_FIFO_REG = (right_value << TRAN_TX_FIFOREG_RIGHT_DATA_Pos) | (left_value << TRAN_TX_FIFOREG_LEFT_DATA_Pos);
}

static inline void  wj_codec_set_tx_fifo(wj_codec_regs_t *addr, uint32_t value)
{
    addr->PARA_TX_FIFO_REG = value;
}
static inline uint32_t  wj_codec_get_rx_fifo(wj_codec_regs_t *addr, uint32_t index)
{
    return *(&addr->RX_FIFO1_REG + index);
}
static inline uint32_t  wj_codec_get_rx_fifo1(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO1_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo2(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO2_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo3(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO3_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo4(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO4_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo5(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO5_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo6(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO6_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo7(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO7_REG);
}

static inline uint32_t  wj_codec_get_rx_fifo8(wj_codec_regs_t *addr)
{
    return (addr->RX_FIFO8_REG);
}

static inline void  wj_codec_set_rx_threshold(wj_codec_regs_t *addr, uint32_t value)
{
    addr->FIFO_TH_CTRL_REG &= (~FIFO_TH_CTRL_RX_Msk);
    addr->FIFO_TH_CTRL_REG = value & FIFO_TH_CTRL_RX_Msk;
}
static inline void  wj_codec_set_tx_threshold(wj_codec_regs_t *addr, uint32_t value)
{
    addr->FIFO_TH_CTRL_REG &= (~FIFO_TH_CTRL_TX_Msk);
    addr->FIFO_TH_CTRL_REG = value & FIFO_TH_CTRL_TX_Msk;
}

static inline uint32_t  wj_codec_get_rx_threshold(wj_codec_regs_t *addr)
{
    return (addr->FIFO_TH_CTRL_REG & FIFO_TH_CTRL_RX_Msk);
}
static inline uint32_t  wj_codec_get_tx_threshold(wj_codec_regs_t *addr)
{
    return (addr->FIFO_TH_CTRL_REG & FIFO_TH_CTRL_TX_Msk);
}

static inline uint32_t  wj_codec_get_error_mask(wj_codec_regs_t *addr)
{
    return addr->INTR_ERR_CTRL_REG;
}
static inline void  wj_codec_set_error_mask(wj_codec_regs_t *addr, uint32_t value)
{
    addr->INTR_ERR_CTRL_REG = value;
}

static inline uint32_t  wj_codec_get_error_state(wj_codec_regs_t *addr)
{
    return addr->INTR_ERR_STA_REG;
}

static inline void  wj_codec_set_channel_en(wj_codec_regs_t *addr, uint32_t value)
{
    addr->PATH_EN_REG = value;
}
static inline void  wj_codec_enable_adc(wj_codec_regs_t *addr, uint32_t value)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC1 + value;
}
static inline void  wj_codec_enable_adc1(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC1;
}
static inline void  wj_codec_enable_adc2(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC2;
}
static inline void  wj_codec_enable_adc3(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC3;
}
static inline void  wj_codec_enable_adc4(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC4;
}
static inline void  wj_codec_enable_adc5(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC5;
}
static inline void  wj_codec_enable_adc6(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC6;
}
static inline void  wj_codec_enable_adc7(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC7;
}
static inline void  wj_codec_enable_adc8(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_ADC8;
}
static inline void  wj_codec_enable_dac(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG |= PATH_EN_REG_DAC;
}

static inline void  wj_codec_disable_adc1(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC1;
}
static inline void  wj_codec_disable_adc2(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC2;
}
static inline void  wj_codec_disable_adc3(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC3;
}
static inline void  wj_codec_disable_adc4(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC4;
}
static inline void  wj_codec_disable_adc5(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC5;
}
static inline void  wj_codec_disable_adc6(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC6;
}
static inline void  wj_codec_disable_adc7(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC7;
}
static inline void  wj_codec_disable_adc8(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_ADC8;
}
static inline void  wj_codec_disable_dac(wj_codec_regs_t *addr)
{
    addr->PATH_EN_REG &= ~PATH_EN_REG_DAC;
}

static inline void  wj_codec_clr_intr(wj_codec_regs_t *addr, uint32_t value)
{
    addr->INTR_ERR_CLR_REG = value;
}

// static inline void  wj_codec_set_cod_master(wj_codec_regs_t *addr)
// {
//     addr->COD_MASTER_REG |= CODE_MASTER_REG_MASTER;
// }

// static inline void  wj_codec_set_cod_slave(wj_codec_regs_t *addr)
// {
//     addr->COD_MASTER_REG &= ~CODE_MASTER_REG_MASTER;
// }

static inline void  wj_codec_set_i2c_addr(wj_codec_regs_t *addr, uint32_t value)
{
    addr->I2C_ADDR_REG = value & I2C_ADDR_REG_I2C_ADDR_Msk;
}

static inline void  wj_codec_enable_dma_req(wj_codec_regs_t *addr)
{
    addr->TRAN_CTRL_REG |= TRAN_CTRL_REG_DMA_REQ;
}

static inline void  wj_codec_disable_dma_req(wj_codec_regs_t *addr)
{
    addr->TRAN_CTRL_REG &= ~TRAN_CTRL_REG_DMA_REQ;
}

static inline void  wj_codec_set_adc_24bits(wj_codec_regs_t *addr)
{
    addr->SAMPLING_CTRL_REG |= SAMPLING_CTRL_REG_RX_24BITS;
}

static inline void  wj_codec_set_adc_16bits(wj_codec_regs_t *addr)
{
    addr->SAMPLING_CTRL_REG &= ~SAMPLING_CTRL_REG_RX_24BITS;
}

static inline void  wj_codec_set_dac_24bits(wj_codec_regs_t *addr)
{
    addr->SAMPLING_CTRL_REG |= SAMPLING_CTRL_REG_TX_24BITS;
}

static inline void  wj_codec_set_dac_16bits(wj_codec_regs_t *addr)
{
    addr->SAMPLING_CTRL_REG &= ~SAMPLING_CTRL_REG_TX_24BITS;
}


static inline uint32_t  wj_codec_get_tx_fifo_cnt(wj_codec_regs_t *addr)
{
    return addr->TX_FIFO_CNT_REG;
}


static inline uint32_t  wj_codec_get_rx_fifo_cnt1(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT1_REG;
}



static inline uint32_t  wj_codec_get_rx_fifo_cnt2(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT2_REG;
}

static inline uint32_t  wj_codec_get_rx_fifo_cnt3(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT3_REG;
}

static inline uint32_t  wj_codec_get_rx_fifo_cnt4(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT4_REG;
}

static inline uint32_t  wj_codec_get_rx_fifo_cnt5(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT5_REG;
}

static inline uint32_t  wj_codec_get_rx_fifo_cnt6(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT6_REG;
}

static inline uint32_t  wj_codec_get_rx_fifo_cnt7(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT7_REG;
}

static inline uint32_t  wj_codec_get_rx_fifo_cnt8(wj_codec_regs_t *addr)
{
    return addr->RX_FIFO_CNT8_REG;
}
#ifdef __cplusplus
}
#endif

#endif /* _IPGOAL_CODEDC_LL_H_ */
