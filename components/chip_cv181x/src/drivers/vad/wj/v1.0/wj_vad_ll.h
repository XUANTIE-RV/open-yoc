/*                                                                              
 *       Copyright (C) 2017-2020 Alibaba Group Holding Limited                  
 */                                                                             
/******************************************************************************
 * @file     wj_vad_ll.h
 * @brief    header file for vad ll driver
 * @version  V1.0
 * @date     8. Dec 2020
  ******************************************************************************/

#ifndef _WJ_VAD_LL_H_
#define _WJ_VAD_LL_H_

#include <csi_core.h>
#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * VAD register bit definitions
 */

/* VAD_CTRL Offset: 0x0 */
#define WJ_VAD_DENO_Pos                   (24U)
#define WJ_VAD_DENO_Msk                   (0x7U << WJ_VAD_DENO_Pos)

#define WJ_VAD_NUME_Pos                   (16U)
#define WJ_VAD_NUME_Msk                   (0xFU << WJ_VAD_NUME_Pos)

#define WJ_VAD_BUF_SEL_Pos                (8U)
#define WJ_VAD_BUF_SEL_Msk                (0xFFU << WJ_VAD_BUF_SEL_Pos)

#define WJ_VAD_CTRL_VAD_CH3_EN_Pos        (7U)
#define WJ_VAD_CTRL_VAD_CH3_EN_Msk        (0x1U << WJ_VAD_CTRL_VAD_CH3_EN_Pos)
#define WJ_VAD_CTRL_VAD_CH3_EN            WJ_VAD_CTRL_VAD_CH3_EN_Msk

#define WJ_VAD_CTRL_VAD_CH2_EN_Pos        (6U)
#define WJ_VAD_CTRL_VAD_CH2_EN_Msk        (0x1U << WJ_VAD_CTRL_VAD_CH2_EN_Pos)
#define WJ_VAD_CTRL_VAD_CH2_EN            WJ_VAD_CTRL_VAD_CH2_EN_Msk

#define WJ_VAD_CTRL_VAD_CH1_EN_Pos        (5U)
#define WJ_VAD_CTRL_VAD_CH1_EN_Msk        (0x1U << WJ_VAD_CTRL_VAD_CH1_EN_Pos)
#define WJ_VAD_CTRL_VAD_CH1_EN            WJ_VAD_CTRL_VAD_CH1_EN_Msk

#define WJ_VAD_CTRL_VAD_CH0_EN_Pos        (4U)
#define WJ_VAD_CTRL_VAD_CH0_EN_Msk        (0x1U << WJ_VAD_CTRL_VAD_CH0_EN_Pos)
#define WJ_VAD_CTRL_VAD_CH0_EN            WJ_VAD_CTRL_VAD_CH0_EN_Msk

#define WJ_VAD_CTRL_BUS_READY_Pos         (2U)
#define WJ_VAD_CTRL_BUS_READY_Msk         (0x1U << WJ_VAD_CTRL_BUS_READY_Pos)
#define WJ_VAD_CTRL_BUS_READY_EN          WJ_VAD_CTRL_BUS_READY_Msk

#define WJ_VAD_CTRL_DATA_TRANS_EN_Pos     (1U)
#define WJ_VAD_CTRL_DATA_TRANS_EN_Msk     (0x1U << WJ_VAD_CTRL_DATA_TRANS_EN_Pos)
#define WJ_VAD_CTRL_DATA_TRANS_EN         WJ_VAD_CTRL_DATA_TRANS_EN_Msk

#define WJ_VAD_CTRL_VAD_EN_Pos            (0U)
#define WJ_VAD_CTRL_VAD_EN_Msk            (0x1U << WJ_VAD_CTRL_VAD_EN_Pos)
#define WJ_VAD_CTRL_VAD_EN                WJ_VAD_CTRL_VAD_EN_Msk

/* ZCR_TH_CTRL Offset: 0x4 */
#define WJ_VAD_ZCR_HTH_Pos                (8U)
#define WJ_VAD_ZCR_HTH_Msk                (0xFFU << WJ_VAD_ZCR_HTH_Pos)

#define WJ_VAD_ZCR_LTH_Pos                (0U)
#define WJ_VAD_ZCR_LTH_Msk                (0xFFU << WJ_VAD_ZCR_LTH_Pos)

/* STE_HTH_CTRL Offset: 0x8 */
#define WJ_VAD_STE_HTH_Pos                (0U)
#define WJ_VAD_STE_HTH_Msk                (0xFFFFU << WJ_VAD_STE_HTH_Pos)

/* STE_LTH_CTRL Offset: 0xC */
#define WJ_VAD_STE_LTH_Pos                (0U)
#define WJ_VAD_STE_LTH_Msk                (0xFFFFU << WJ_VAD_STE_LTH_Pos)

/* VAD_INTR_MFLAG Offset: 0x10 */
#define WJ_VAD_INTR_MFLAG_Pos             (0U)
#define WJ_VAD_INTR_MFLAG_Msk             (0x1U << WJ_VAD_INTR_MFLAG_Pos)
#define WJ_VAD_INTR_MFLAG_EN              WJ_VAD_INTR_MFLAG_Msk

/* VAD_INTR_CLR Offset: 0x14 */
#define WJ_VAD_CLR_Pos                    (0U)
#define WJ_VAD_CLR_Msk                    (0x1U << WJ_VAD_CLR_Pos)
#define WJ_VAD_CLR_EN                     WJ_VAD_CLR_Msk

/* VAD_INTR_RAW Offset: 0x18 */
#define WJ_VAD_INTR_RAW_Pos               (0U)
#define WJ_VAD_INTR_RAW_Msk               (0x1U << WJ_VAD_INTR_RAW_Pos)
#define WJ_VAD_INTR_RAW_EN                WJ_VAD_INTR_RAW_Msk

/* VAD_INTR_MASK Offset: 0x1C */
#define WJ_VAD_INTR_UNMASK_Pos            (0U)
#define WJ_VAD_INTR_UNMASK_Msk            (0x1U << WJ_VAD_INTR_UNMASK_Pos)
#define WJ_VAD_INTR_UNMASK                WJ_VAD_INTR_UNMASK_Msk

/* DAI_EN Offset: 0x20 */
#define WJ_VAD_DAI_EN_Pos                 (0U)
#define WJ_VAD_DAI_EN_Msk                 (0x1U << WJ_VAD_DAI_EN_Pos)
#define WJ_VAD_DAI_EN_EN                  WJ_VAD_DAI_EN_Msk

/* DAI_CTRL Offset: 0x24 */
#define WJ_VAD_DAI_CTRL_I2S_DIV_SEL_Pos   (27U)
#define WJ_VAD_DAI_CTRL_I2S_DIV_SEL_Msk   (0x7U << WJ_VAD_DAI_CTRL_I2S_DIV_SEL_Pos)

#define WJ_VAD_DAI_CTRL_ST_GAIN_Pos       (24U)
#define WJ_VAD_DAI_CTRL_ST_GAIN_Msk       (0x7U << WJ_VAD_DAI_CTRL_ST_GAIN_Pos)

#define WJ_VAD_DAI_CTRL_LP_GAIN_Pos       (20U)
#define WJ_VAD_DAI_CTRL_LP_GAIN_Msk       (0x7U << WJ_VAD_DAI_CTRL_LP_GAIN_Pos)

#define WJ_VAD_DAI_CTRL_I2S_MCLK_SEL_Pos  (18U)
#define WJ_VAD_DAI_CTRL_I2S_MCLK_SEL_Msk  (0x1U << WJ_VAD_DAI_CTRL_I2S_DIV_SEL_Pos)
#define WJ_VAD_DAI_CTRL_I2S_MCLK_SEL_384FS WJ_VAD_DAI_CTRL_I2S_MCLK_SEL_Msk

#define WJ_VAD_DAI_CTRL_I2S_MODE_Pos      (17U)
#define WJ_VAD_DAI_CTRL_I2S_MODE_Msk      (0x1U << WJ_VAD_DAI_CTRL_I2S_MODE_Pos)
#define WJ_VAD_DAI_CTRL_I2S_MODE_MASTER   WJ_VAD_DAI_CTRL_I2S_MODE_Msk

#define WJ_VAD_DAI_CTRL_PDM_MODE_Pos      (16U)
#define WJ_VAD_DAI_CTRL_PDM_MODE_Msk      (0x1U << WJ_VAD_DAI_CTRL_PDM_MODE_Pos)
#define WJ_VAD_DAI_CTRL_PDM_MODE_STANDARD WJ_VAD_DAI_CTRL_PDM_MODE_Msk

#define WJ_VAD_DAI_CTRL_I2S_WDSEL_Pos     (14U)
#define WJ_VAD_DAI_CTRL_I2S_WDSEL_Msk     (0x3U << WJ_VAD_DAI_CTRL_I2S_WDSEL_Pos)
#define WJ_VAD_DAI_CTRL_I2S_WDSEL_8BITS   (0x3U << WJ_VAD_DAI_CTRL_I2S_WDSEL_Pos)
#define WJ_VAD_DAI_CTRL_I2S_WDSEL_16BITS  (0x0U << WJ_VAD_DAI_CTRL_I2S_WDSEL_Pos)
#define WJ_VAD_DAI_CTRL_I2S_WDSEL_24BITS  (0x1U << WJ_VAD_DAI_CTRL_I2S_WDSEL_Pos)
#define WJ_VAD_DAI_CTRL_I2S_WDSEL_32BITS  (0x2U << WJ_VAD_DAI_CTRL_I2S_WDSEL_Pos)

#define WJ_VAD_DAI_CTRL_I2S_MODESEL_Pos   (12U)
#define WJ_VAD_DAI_CTRL_I2S_MODESEL_Msk   (0x3U << WJ_VAD_DAI_CTRL_I2S_MODESEL_Pos)
#define WJ_VAD_DAI_CTRL_I2S_MODESEL_I2S   (0x0U << WJ_VAD_DAI_CTRL_I2S_MODESEL_Pos)
#define WJ_VAD_DAI_CTRL_I2S_MODESEL_PCM   (0x3U << WJ_VAD_DAI_CTRL_I2S_MODESEL_Pos)
#define WJ_VAD_DAI_CTRL_I2S_MODESEL_RIGHT_JUSTIFIED   (0x1U << WJ_VAD_DAI_CTRL_I2S_MODESEL_Pos)
#define WJ_VAD_DAI_CTRL_I2S_MODESEL_LEFT_JUSTIFIED    (0x2U << WJ_VAD_DAI_CTRL_I2S_MODESEL_Pos)

#define WJ_VAD_DAI_CTRL_CH_SEL_Pos        (8U)
#define WJ_VAD_DAI_CTRL_CH_SEL_Msk        (0x1U << WJ_VAD_DAI_CTRL_CH_SEL_Pos)
#define WJ_VAD_DAI_CTRL_VAD_CH_SEL_RIGHT  WJ_VAD_DAI_CTRL_CH_SEL_Msk

#define WJ_VAD_DAI_CTRL_MONO_EN_Pos       (4U)
#define WJ_VAD_DAI_CTRL_MONO_EN_Msk       (0x1U << WJ_VAD_DAI_CTRL_MONO_EN_Pos)
#define WJ_VAD_DAI_CTRL_MONO_EN           WJ_VAD_DAI_CTRL_MONO_EN_Msk

#define WJ_VAD_DAI_CTRL_FUNCMODE_Pos      (0U)
#define WJ_VAD_DAI_CTRL_FUNCMODE_Msk      (0x1U << WJ_VAD_DAI_CTRL_FUNCMODE_Pos)
#define WJ_VAD_DAI_CTRL_FUNCMODE_I2S      WJ_VAD_DAI_CTRL_FUNCMODE_Msk

/* FIFO_CNT Offset: 0x28 */
#define WJ_VAD_FIFO_CNT_FIFO0_L_CNT_Pos   (4U)
#define WJ_VAD_FIFO_CNT_FIFO0_L_CNT_Msk   (0xFU << WJ_VAD_FIFO_CNT_FIFO0_L_CNT_Pos)
#define WJ_VAD_FIFO_CNT_FIFO0_R_CNT_Pos   (0U)
#define WJ_VAD_FIFO_CNT_FIFO0_R_CNT_Msk   (0xFU << WJ_VAD_FIFO_CNT_FIFO0_R_CNT_Pos)

#define WJ_VAD_FIFO_CNT_FIFO1_L_CNT_Pos   (8U)
#define WJ_VAD_FIFO_CNT_FIFO1_L_CNT_Msk   (0xFU << WJ_VAD_FIFO_CNT_FIFO1_L_CNT_Pos)
#define WJ_VAD_FIFO_CNT_FIFO1_R_CNT_Pos   (12U)
#define WJ_VAD_FIFO_CNT_FIFO1_R_CNT_Msk   (0xFU << WJ_VAD_FIFO_CNT_FIFO1_R_CNT_Pos)

#define WJ_VAD_FIFO_CNT_FIFO2_L_CNT_Pos   (16U)
#define WJ_VAD_FIFO_CNT_FIFO2_L_CNT_Msk   (0xFU << WJ_VAD_FIFO_CNT_FIFO2_L_CNT_Pos)
#define WJ_VAD_FIFO_CNT_FIFO2_R_CNT_Pos   (20U)
#define WJ_VAD_FIFO_CNT_FIFO2_R_CNT_Msk   (0xFU << WJ_VAD_FIFO_CNT_FIFO2_R_CNT_Pos)

#define WJ_VAD_FIFO_CNT_FIFO3_L_CNT_Pos   (24U)
#define WJ_VAD_FIFO_CNT_FIFO3_L_CNT_Msk   (0xFU << WJ_VAD_FIFO_CNT_FIFO3_L_CNT_Pos)
#define WJ_VAD_FIFO_CNT_FIFO3_R_CNT_Pos   (28U)
#define WJ_VAD_FIFO_CNT_FIFO3_R_CNT_Msk   (0xFU << WJ_VAD_FIFO_CNT_FIFO3_R_CNT_Pos)

/* FIFO_TH_CTRL Offset: 0x2C */
#define WJ_VAD_FIFO_TH_CTRL_Pos           (0U)
#define WJ_VAD_FIFO_TH_CTRL_Msk           (0x7U << WJ_VAD_FIFO_TH_CTRL_Pos)

/* FIFO_INTR_MFLAG Offset: 0x30 */
#define WJ_VAD_INTR_MFLAG_FIFO_L_EMPTY_CLR_Pos      (7U)
#define WJ_VAD_INTR_MFLAG_FIFO_L_EMPTY_CLR_Msk      (0x1U << WJ_VAD_INTR_MFLAG_FIFO_L_EMPTY_CLR_Pos)
#define WJ_VAD_INTR_MFLAG_FIFO_L_EMPTY_CLR          WJ_VAD_INTR_MFLAG_FIFO_L_EMPTY_CLR_Msk

#define WJ_VAD_INTR_MFLAG_FIFO_L_FULL_CLR_Pos       (6U)
#define WJ_VAD_INTR_MFLAG_FIFO_L_FULL_CLR_Msk       (0x1U << WJ_VAD_INTR_MFLAG_FIFO_L_FULL_CLR_Pos)
#define WJ_VAD_INTR_MFLAG_FIFO_L_FULL_CLR           WJ_VAD_INTR_MFLAG_FIFO_L_FULL_CLR_Msk

#define WJ_VAD_INTR_MFLAG_FIFO_L_EMPTY_ERR_CLR_Pos  (4U)
#define WJ_VAD_INTR_MFLAG_FIFO_L_EMPTY_ERR_CLR_Msk  (0x1U << WJ_VAD_INTR_MFLAG_FIFO_L_EMPTY_ERR_CLR_Pos)
#define WJ_VAD_INTR_MFLAG_FIFO_L_EMPTY_ERR_CLR      WJ_VAD_INTR_MFLAG_FIFO_L_EMPTY_ERR_CLR_Msk

#define WJ_VAD_INTR_MFLAG_FIFO_R_EMPTY_CLR_Pos      (3U)
#define WJ_VAD_INTR_MFLAG_FIFO_R_EMPTY_CLR_Msk      (0x1U << WJ_VAD_INTR_MFLAG_FIFO_R_EMPTY_CLR_Pos)
#define WJ_VAD_INTR_MFLAG_FIFO_R_EMPTY_CLR          WJ_VAD_INTR_MFLAG_FIFO_R_EMPTY_CLR_Msk

#define WJ_VAD_INTR_MFLAG_FIFO_R_FULL_CLR_Pos       (2U)
#define WJ_VAD_INTR_MFLAG_FIFO_R_FULL_CLR_Msk       (0x1U << WJ_VAD_INTR_MFLAG_FIFO_R_FULL_CLR_Pos)
#define WJ_VAD_INTR_MFLAG_FIFO_R_FULL_CLR           WJ_VAD_INTR_MFLAG_FIFO_R_FULL_CLR_Msk

#define WJ_VAD_INTR_MFLAG_DATA_OVERFLOW_ERR_CLR_Pos (1U)
#define WJ_VAD_INTR_MFLAG_DATA_OVERFLOW_ERR_CLR_Msk (0x1U << WJ_VAD_INTR_MFLAG_DATA_OVERFLOW_ERR_CLR_Pos)
#define WJ_VAD_INTR_MFLAG_DATA_OVERFLOW_ERR_CLR     WJ_VAD_INTR_MFLAG_DATA_OVERFLOW_ERR_CLR_Msk

#define WJ_VAD_INTR_MFLAG_FIFO_R_EMPTY_ERR_CLR_Pos  (0U)
#define WJ_VAD_INTR_MFLAG_FIFO_R_EMPTY_ERR_CLR_Msk  (0x1U << WJ_VAD_INTR_MFLAG_FIFO_R_EMPTY_ERR_CLR_Pos)
#define WJ_VAD_INTR_MFLAG_FIFO_R_EMPTY_ERR_CLR      WJ_VAD_INTR_MFLAG_FIFO_R_EMPTY_ERR_CLR_Msk

/* FIFO_INTR_CLR Offset: 0x34 */
#define WJ_VAD_INTR_CLR_FIFO_L_EMPTY_CLR_Pos      (7U)
#define WJ_VAD_INTR_CLR_FIFO_L_EMPTY_CLR_Msk      (0x1U << WJ_VAD_INTR_CLR_FIFO_L_EMPTY_CLR_Pos)
#define WJ_VAD_INTR_CLR_FIFO_L_EMPTY_CLR          WJ_VAD_INTR_CLR_FIFO_L_EMPTY_CLR_Msk

#define WJ_VAD_INTR_CLR_FIFO_L_FULL_CLR_Pos       (6U)
#define WJ_VAD_INTR_CLR_FIFO_L_FULL_CLR_Msk       (0x1U << WJ_VAD_INTR_CLR_FIFO_L_FULL_CLR_Pos)
#define WJ_VAD_INTR_CLR_FIFO_L_FULL_CLR           WJ_VAD_INTR_CLR_FIFO_L_FULL_CLR_Msk

#define WJ_VAD_INTR_CLR_FIFO_L_EMPTY_ERR_CLR_Pos  (4U)
#define WJ_VAD_INTR_CLR_FIFO_L_EMPTY_ERR_CLR_Msk  (0x1U << WJ_VAD_INTR_CLR_FIFO_L_EMPTY_ERR_CLR_Pos)
#define WJ_VAD_INTR_CLR_FIFO_L_EMPTY_ERR_CLR      WJ_VAD_INTR_CLR_FIFO_L_EMPTY_ERR_CLR_Msk

#define WJ_VAD_INTR_CLR_FIFO_R_EMPTY_CLR_Pos      (3U)
#define WJ_VAD_INTR_CLR_FIFO_R_EMPTY_CLR_Msk      (0x1U << WJ_VAD_INTR_CLR_FIFO_R_EMPTY_CLR_Pos)
#define WJ_VAD_INTR_CLR_FIFO_R_EMPTY_CLR          WJ_VAD_INTR_CLR_FIFO_R_EMPTY_CLR_Msk

#define WJ_VAD_INTR_CLR_FIFO_R_FULL_CLR_Pos       (2U)
#define WJ_VAD_INTR_CLR_FIFO_R_FULL_CLR_Msk       (0x1U << WJ_VAD_INTR_CLR_FIFO_R_FULL_CLR_Pos)
#define WJ_VAD_INTR_CLR_FIFO_R_FULL_CLR           WJ_VAD_INTR_CLR_FIFO_R_FULL_CLR_Msk

#define WJ_VAD_INTR_CLR_DATA_OVERFLOW_ERR_CLR_Pos (1U)
#define WJ_VAD_INTR_CLR_DATA_OVERFLOW_ERR_CLR_Msk (0x1U << WJ_VAD_INTR_CLR_DATA_OVERFLOW_ERR_CLR_Pos)
#define WJ_VAD_INTR_CLR_DATA_OVERFLOW_ERR_CLR     WJ_VAD_INTR_CLR_DATA_OVERFLOW_ERR_CLR_Msk

#define WJ_VAD_INTR_CLR_FIFO_R_EMPTY_ERR_CLR_Pos  (0U)
#define WJ_VAD_INTR_CLR_FIFO_R_EMPTY_ERR_CLR_Msk  (0x1U << WJ_VAD_INTR_CLR_FIFO_R_EMPTY_ERR_CLR_Pos)
#define WJ_VAD_INTR_CLR_FIFO_R_EMPTY_ERR_CLR      WJ_VAD_INTR_CLR_FIFO_R_EMPTY_ERR_CLR_Msk

/* FIFO_INTR_FLAG Offset: 0x38 */
#define WJ_VAD_INTR_FLAG_FIFO_L_EMPTY_CLR_Pos      (7U)
#define WJ_VAD_INTR_FLAG_FIFO_L_EMPTY_CLR_Msk      (0x1U << WJ_VAD_INTR_FLAG_FIFO_L_EMPTY_CLR_Pos)
#define WJ_VAD_INTR_FLAG_FIFO_L_EMPTY_CLR          WJ_VAD_INTR_FLAG_FIFO_L_EMPTY_CLR_Msk

#define WJ_VAD_INTR_FLAG_FIFO_L_FULL_CLR_Pos       (6U)
#define WJ_VAD_INTR_FLAG_FIFO_L_FULL_CLR_Msk       (0x1U << WJ_VAD_INTR_FLAG_FIFO_L_FULL_CLR_Pos)
#define WJ_VAD_INTR_FLAG_FIFO_L_FULL_CLR           WJ_VAD_INTR_FLAG_FIFO_L_FULL_CLR_Msk

#define WJ_VAD_INTR_FLAG_FIFO_L_EMPTY_ERR_CLR_Pos  (4U)
#define WJ_VAD_INTR_FLAG_FIFO_L_EMPTY_ERR_CLR_Msk  (0x1U << WJ_VAD_INTR_FLAG_FIFO_L_EMPTY_ERR_CLR_Pos)
#define WJ_VAD_INTR_FLAG_FIFO_L_EMPTY_ERR_CLR      WJ_VAD_INTR_FLAG_FIFO_L_EMPTY_ERR_CLR_Msk

#define WJ_VAD_INTR_FLAG_FIFO_R_EMPTY_CLR_Pos      (3U)
#define WJ_VAD_INTR_FLAG_FIFO_R_EMPTY_CLR_Msk      (0x1U << WJ_VAD_INTR_FLAG_FIFO_R_EMPTY_CLR_Pos)
#define WJ_VAD_INTR_FLAG_FIFO_R_EMPTY_CLR          WJ_VAD_INTR_FLAG_FIFO_R_EMPTY_CLR_Msk

#define WJ_VAD_INTR_FLAG_FIFO_R_FULL_CLR_Pos       (2U)
#define WJ_VAD_INTR_FLAG_FIFO_R_FULL_CLR_Msk       (0x1U << WJ_VAD_INTR_FLAG_FIFO_R_FULL_CLR_Pos)
#define WJ_VAD_INTR_FLAG_FIFO_R_FULL_CLR           WJ_VAD_INTR_FLAG_FIFO_R_FULL_CLR_Msk

#define WJ_VAD_INTR_FLAG_DATA_OVERFLOW_ERR_CLR_Pos (1U)
#define WJ_VAD_INTR_FLAG_DATA_OVERFLOW_ERR_CLR_Msk (0x1U << WJ_VAD_INTR_FLAG_DATA_OVERFLOW_ERR_CLR_Pos)
#define WJ_VAD_INTR_FLAG_DATA_OVERFLOW_ERR_CLR     WJ_VAD_INTR_FLAG_DATA_OVERFLOW_ERR_CLR_Msk

#define WJ_VAD_INTR_FLAG_FIFO_R_EMPTY_ERR_CLR_Pos  (0U)
#define WJ_VAD_INTR_FLAG_FIFO_R_EMPTY_ERR_CLR_Msk  (0x1U << WJ_VAD_INTR_FLAG_FIFO_R_EMPTY_ERR_CLR_Pos)
#define WJ_VAD_INTR_FLAG_FIFO_R_EMPTY_ERR_CLR      WJ_VAD_INTR_FLAG_FIFO_R_EMPTY_ERR_CLR_Msk

/* FIFO_INTR_MASK Offset: 0x3C */
#define WJ_VAD_INTR_MASK_FIFO_L_EMPTY_CLR_Pos      (7U)
#define WJ_VAD_INTR_MASK_FIFO_L_EMPTY_CLR_Msk      (0x1U << WJ_VAD_INTR_MASK_FIFO_L_EMPTY_CLR_Pos)
#define WJ_VAD_INTR_MASK_FIFO_L_EMPTY_CLR          WJ_VAD_INTR_MASK_FIFO_L_EMPTY_CLR_Msk

#define WJ_VAD_INTR_MASK_FIFO_L_FULL_CLR_Pos       (6U)
#define WJ_VAD_INTR_MASK_FIFO_L_FULL_CLR_Msk       (0x1U << WJ_VAD_INTR_MASK_FIFO_L_FULL_CLR_Pos)
#define WJ_VAD_INTR_MASK_FIFO_L_FULL_CLR           WJ_VAD_INTR_MASK_FIFO_L_FULL_CLR_Msk

#define WJ_VAD_INTR_MASK_FIFO_L_EMPTY_ERR_CLR_Pos  (4U)
#define WJ_VAD_INTR_MASK_FIFO_L_EMPTY_ERR_CLR_Msk  (0x1U << WJ_VAD_INTR_MASK_FIFO_L_EMPTY_ERR_CLR_Pos)
#define WJ_VAD_INTR_MASK_FIFO_L_EMPTY_ERR_CLR      WJ_VAD_INTR_MASK_FIFO_L_EMPTY_ERR_CLR_Msk

#define WJ_VAD_INTR_MASK_FIFO_R_EMPTY_CLR_Pos      (3U)
#define WJ_VAD_INTR_MASK_FIFO_R_EMPTY_CLR_Msk      (0x1U << WJ_VAD_INTR_MASK_FIFO_R_EMPTY_CLR_Pos)
#define WJ_VAD_INTR_MASK_FIFO_R_EMPTY_CLR          WJ_VAD_INTR_MASK_FIFO_R_EMPTY_CLR_Msk

#define WJ_VAD_INTR_MASK_FIFO_R_FULL_CLR_Pos       (2U)
#define WJ_VAD_INTR_MASK_FIFO_R_FULL_CLR_Msk       (0x1U << WJ_VAD_INTR_MASK_FIFO_R_FULL_CLR_Pos)
#define WJ_VAD_INTR_MASK_FIFO_R_FULL_CLR           WJ_VAD_INTR_MASK_FIFO_R_FULL_CLR_Msk

#define WJ_VAD_INTR_MASK_DATA_OVERFLOW_ERR_CLR_Pos (1U)
#define WJ_VAD_INTR_MASK_DATA_OVERFLOW_ERR_CLR_Msk (0x1U << WJ_VAD_INTR_MASK_DATA_OVERFLOW_ERR_CLR_Pos)
#define WJ_VAD_INTR_MASK_DATA_OVERFLOW_ERR_CLR     WJ_VAD_INTR_MASK_DATA_OVERFLOW_ERR_CLR_Msk

#define WJ_VAD_INTR_MASK_FIFO_R_EMPTY_ERR_CLR_Pos  (0U)
#define WJ_VAD_INTR_MASK_FIFO_R_EMPTY_ERR_CLR_Msk  (0x1U << WJ_VAD_INTR_MASK_FIFO_R_EMPTY_ERR_CLR_Pos)
#define WJ_VAD_INTR_MASK_FIFO_R_EMPTY_ERR_CLR      WJ_VAD_INTR_MASK_FIFO_R_EMPTY_ERR_CLR_Msk

/* CIC_L_ST_OFFSET Offset: 0x40 */
#define WJ_VAD_CIC_L_ST_OFFSET_Pos                 (0U)
#define WJ_VAD_CIC_L_ST_OFFSET_Msk                 (0x7FFFFFU << WJ_VAD_CIC_L_ST_OFFSET_Pos)

/* CIC_L_LP_OFFSET Offset: 0x4C */
#define WJ_VAD_CIC_L_LP_OFFSET_Pos                 (0U)
#define WJ_VAD_CIC_L_LP_OFFSET_Msk                 (0xFFFFU << WJ_VAD_CIC_L_LP_OFFSET_Pos)

/* CIC_R_ST_OFFSET Offset: 0x48 */
#define WJ_VAD_CIC_R_ST_OFFSET_Pos                 (0U)
#define WJ_VAD_CIC_R_ST_OFFSET_Msk                 (0x7FFFFFU << WJ_VAD_CIC_R_ST_OFFSET_Pos)

/* CIC_R_LP_OFFSET Offset: 0x4C */
#define WJ_VAD_CIC_R_LP_OFFSET_Pos                 (0U)
#define WJ_VAD_CIC_R_LP_OFFSET_Msk                 (0xFFFFU << WJ_VAD_CIC_R_LP_OFFSET_Pos)

/* FIFO_L_READ Offset: 0x50 */
#define WJ_VAD_CIC_FIFO_L_READ_Pos                 (0U)
#define WJ_VAD_CIC_FIFO_L_READ_Msk                 (0xFFFFFFFFU << WJ_VAD_CIC_FIFO_L_READ_Pos)

/* FIFO_R_READ Offset: 0x54 */
#define WJ_VAD_CIC_FIFO_R_READ_Pos                 (0U)
#define WJ_VAD_CIC_FIFO_R_READ_Msk                 (0xFFFFFFFFU << WJ_VAD_CIC_FIFO_R_READ_Pos)

/* IP_ID Offset: 0x7C */
#define WJ_VAD_IP_ID_Pos                           (0U)
#define WJ_VAD_IP_ID_Msk                           (0xFFFFFFFFU << WJ_VAD_IP_ID_Pos)

typedef struct {
    __IOM uint32_t VAD_CTRL;                     /* Offset: 0x0       (R/W)  VAD_CTRL;           Register */
    __IOM uint32_t ZCR_TH_CTRL;                  /* Offset: 0x4       (R/W)  ZCR_TH_CTRL;        Register */
    __IOM uint32_t STE_HTH_CTRL;                 /* Offset: 0x8       (R/W)  STE_HTH_CTRL;       Register */
    __IOM uint32_t STE_LTH_CTRL;                 /* Offset: 0xC       (R/W)  STE_LTH_CTRL;       Register */
    __IM  uint32_t VAD_INTR_MFLAG;               /* Offset:0x10       (R/W)  VAD_INTR_MFLAG;      Register */
    __OM  uint32_t VAD_INTR_CLR;                 /* Offset: 0x14      (W)    VAD_INTR_CLR;       Register */
    __IM  uint32_t VAD_INTR_FLAG;                /* Offset: 0x18      (R)    VAD_INTR_FLAG;       Register */
    __IOM uint32_t VAD_INTR_MASK;                /* Offset: 0x1c      (R/W)  VAD_INTR_MASK;      Register */
    __IOM uint32_t DAI_EN;                       /* Offset: 0x20      (R/W)  DAI_EN;             Register */
    __IOM uint32_t DAI_CTRL;                     /* Offset: 0x24      (R/W)  DAI_CTRL;           Register */
    __IM  uint32_t FIFO_CNT;                     /* Offset: 0x28      (R)    FIFO_CNT;           Register */
    __IOM uint32_t FIFO_TH_CTRL;                 /* Offset: 0x2c      (R/W)  FIFO_TH_CTRL;       Register */
    __IM  uint32_t FIFO_INTR_MFLAG;              /* Offset: 0x30      (R)    FIFO_INTR_FLAG;      Register */
    __OM  uint32_t FIFO_INTR_CLR;                /* Offset: 0x34      (W)    FIFO_INTR_CLR;      Register */
    __IM  uint32_t FIFO_INTR_FLAG;               /* Offset: 0x38      (R)    FIFO_INTR_FLAG;      Register */
    __IOM uint32_t FIFO_INTR_MASK;               /* Offset: 0x3c      (R/W)  FIFO_INTR_MASK;     Register */
    __IOM uint32_t CIC_L_ST_OFFSET;              /* Offset: 0x40      (R/W)  CIC_L_ST_OFFSET;    Register */
    __IOM uint32_t CIC_L_LP_OFFSET;              /* Offset: 0x44      (R/W)  CIC_L_LP_OFFSET;    Register */
    __IOM uint32_t CIC_R_ST_OFFSET;              /* Offset: 0x48      (R/W)  CIC_R_ST_OFFSET;    Register */
    __IOM uint32_t CIC_R_LP_OFFSET;              /* Offset: 0x4c      (R/W)  CIC_R_LP_OFFSET;    Register */
    __IM  uint32_t FIFO0_L_READ;                 /* Offset: 0x50      (R)    FIFO_L_READ;        Register */
    __IM  uint32_t FIFO0_R_READ;                 /* Offset: 0x54      (R)    FIFO_R_READ;        Register */
    __IM  uint32_t FIFO1_L_READ;                 /* Offset: 0x58      (R)    FIFO_L_READ;        Register */
    __IM  uint32_t FIFO1_R_READ;                 /* Offset: 0x5c      (R)    FIFO_R_READ;        Register */
    __IM  uint32_t FIFO2_L_READ;                 /* Offset: 0x60      (R)    FIFO_L_READ;        Register */
    __IM  uint32_t FIFO2_R_READ;                 /* Offset: 0x64      (R)    FIFO_R_READ;        Register */
    __IM  uint32_t FIFO3_L_READ;                 /* Offset: 0x68      (R)    FIFO_L_READ;        Register */
    __IM  uint32_t FIFO3_R_READ;                 /* Offset: 0x6c      (R)    FIFO_R_READ;        Register */
         uint32_t RESERVED[4];                  /* Offset: 0x70      (R)    RESERVED */
    __IM  uint32_t IP_ID;                        /* Offset: 0x74      (R)    IP_ID;              Register */
} wj_vad_regs_t;

static inline void wj_vad_enable(wj_vad_regs_t *vad_base)
{
    vad_base->VAD_CTRL |= WJ_VAD_CTRL_VAD_EN;
}

static inline void wj_vad_disable(wj_vad_regs_t *vad_base)
{
    vad_base->VAD_CTRL &= ~WJ_VAD_CTRL_VAD_EN;
}

static inline void wj_vad_data_trans_enable(wj_vad_regs_t *vad_base)
{
    vad_base->VAD_CTRL |= WJ_VAD_CTRL_DATA_TRANS_EN;
}

static inline void wj_vad_data_trans_disable(wj_vad_regs_t *vad_base)
{
    vad_base->VAD_CTRL &= ~WJ_VAD_CTRL_DATA_TRANS_EN;
}

static inline void wj_vad_set_bus_ready(wj_vad_regs_t *vad_base)
{
    vad_base->VAD_CTRL |= WJ_VAD_CTRL_BUS_READY_EN;
}

static inline uint32_t wj_vad_get_bus_ready(wj_vad_regs_t *vad_base)
{
    return (uint32_t)(vad_base->VAD_CTRL & WJ_VAD_CTRL_BUS_READY_EN);
}

static inline void wj_vad_enable_ch(wj_vad_regs_t *vad_base, uint32_t ch)
{
    vad_base->VAD_CTRL |= (ch << WJ_VAD_CTRL_VAD_CH0_EN_Pos);
}

static inline void wj_vad_disable_ch(wj_vad_regs_t *vad_base, uint32_t ch)
{
    vad_base->VAD_CTRL &= ~(ch << WJ_VAD_CTRL_VAD_CH0_EN_Pos);
}

static inline void wj_vad_set_cache_buffer_size(wj_vad_regs_t *vad_base, uint32_t size)
{
    vad_base->VAD_CTRL &= ~WJ_VAD_BUF_SEL_Msk;
    vad_base->VAD_CTRL |= size << WJ_VAD_BUF_SEL_Pos;
}

static inline void wj_vad_set_vad_nume(wj_vad_regs_t *vad_base, uint32_t nume)
{
    vad_base->VAD_CTRL &= ~WJ_VAD_NUME_Msk;
    vad_base->VAD_CTRL |= nume << WJ_VAD_NUME_Pos;
}

static inline void wj_vad_set_vad_deno(wj_vad_regs_t *vad_base, uint32_t deno)
{
    vad_base->VAD_CTRL &= ~WJ_VAD_DENO_Msk;
    vad_base->VAD_CTRL |= deno << WJ_VAD_DENO_Pos;
}

static inline void wj_vad_set_zcr_high_threshold(wj_vad_regs_t *vad_base, uint32_t zcr_h)
{
    vad_base->ZCR_TH_CTRL &= ~WJ_VAD_ZCR_HTH_Msk;
    vad_base->ZCR_TH_CTRL |= zcr_h << WJ_VAD_ZCR_HTH_Pos;
}

static inline void wj_vad_set_zcr_low_threshold(wj_vad_regs_t *vad_base, uint32_t zcr_l)
{
    vad_base->ZCR_TH_CTRL &= ~WJ_VAD_ZCR_LTH_Msk;
    vad_base->ZCR_TH_CTRL |= zcr_l << WJ_VAD_ZCR_LTH_Pos;
}

static inline void wj_vad_set_ste_high_threshold(wj_vad_regs_t *vad_base, uint32_t ste_h)
{
    vad_base->STE_HTH_CTRL = ste_h;
}

static inline void wj_vad_set_ste_low_threshold(wj_vad_regs_t *vad_base, uint32_t ste_l)
{
    vad_base->STE_LTH_CTRL = ste_l;
}

static inline uint32_t wj_vad_get_vad_trig_irq_status(wj_vad_regs_t *vad_base)
{
    return (uint32_t)(vad_base->VAD_INTR_MFLAG & WJ_VAD_INTR_MFLAG_EN);
}

static inline uint32_t wj_vad_get_vad_raw_trig_irq_status(wj_vad_regs_t *vad_base)
{
    return (uint32_t)(vad_base->VAD_INTR_FLAG & WJ_VAD_INTR_RAW_EN);
}

static inline void wj_vad_clear_vad_trig_irq(wj_vad_regs_t *vad_base)
{
    vad_base->VAD_INTR_CLR = WJ_VAD_CLR_EN;
}

static inline void wj_vad_enable_vad_trig_irq(wj_vad_regs_t *vad_base)
{
    vad_base->VAD_INTR_MASK = WJ_VAD_INTR_UNMASK;
}

static inline void wj_vad_disable_vad_trig_irq(wj_vad_regs_t *vad_base)
{
    vad_base->VAD_INTR_MASK = ~WJ_VAD_INTR_UNMASK;
}

static inline void wj_vad_enable_dai(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_EN = WJ_VAD_DAI_EN_EN;
}

static inline void wj_vad_disable_dai(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_EN = ~WJ_VAD_DAI_EN_EN;
}

static inline void wj_vad_dai_funcmode_pdm(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_FUNCMODE_I2S;
}

static inline void wj_vad_dai_funcmode_i2s(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL |= WJ_VAD_DAI_CTRL_FUNCMODE_I2S;
}

static inline void wj_vad_dai_mono_enable(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL |= WJ_VAD_DAI_CTRL_MONO_EN;
}

static inline void wj_vad_dai_mono_disable(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_MONO_EN;
}

static inline void wj_vad_dai_mono_left_sound_channel(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_VAD_CH_SEL_RIGHT;
}

static inline void wj_vad_dai_mono_right_sound_channel(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL |= WJ_VAD_DAI_CTRL_VAD_CH_SEL_RIGHT;
}

static inline void wj_vad_dai_mode_i2s(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_I2S_MODESEL_Msk;
}

static inline void wj_vad_dai_mode_pcm(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL |= WJ_VAD_DAI_CTRL_I2S_MODESEL_PCM;
}

static inline void wj_vad_dai_mode_right_justified(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_I2S_MODESEL_Msk;
    vad_base->DAI_CTRL |= WJ_VAD_DAI_CTRL_I2S_MODESEL_RIGHT_JUSTIFIED;
}

static inline void wj_vad_dai_mode_left_justified(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_I2S_MODESEL_Msk;
    vad_base->DAI_CTRL |= WJ_VAD_DAI_CTRL_I2S_MODESEL_LEFT_JUSTIFIED;
}

static inline void wj_vad_dai_mode_i2s_width_8bit(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL |= WJ_VAD_DAI_CTRL_I2S_WDSEL_8BITS;
}

static inline void wj_vad_dai_mode_i2s_width_16bit(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_I2S_WDSEL_Msk;
}

static inline void wj_vad_dai_mode_i2s_width_24bit(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_I2S_WDSEL_Msk;
    vad_base->DAI_CTRL |= WJ_VAD_DAI_CTRL_I2S_WDSEL_24BITS;
}

static inline void wj_vad_dai_mode_i2s_width_32bit(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_I2S_WDSEL_Msk;
    vad_base->DAI_CTRL |= WJ_VAD_DAI_CTRL_I2S_WDSEL_32BITS;
}

static inline void wj_vad_dai_mode_i2s_master_mode(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL |= WJ_VAD_DAI_CTRL_I2S_MODE_MASTER;
}

static inline void wj_vad_dai_mode_i2s_slave_mode(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_I2S_MODE_MASTER;
}

static inline void wj_vad_dai_mode_i2s_256fs_mclk(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_I2S_MCLK_SEL_384FS;
}

static inline void wj_vad_dai_mode_i2s_384fs_mclk(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL |= WJ_VAD_DAI_CTRL_I2S_MCLK_SEL_384FS;
}

static inline void wj_vad_set_dai_mode_i2s_div(wj_vad_regs_t *vad_base, uint32_t val)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_I2S_DIV_SEL_Msk;
    vad_base->DAI_CTRL |= val << WJ_VAD_DAI_CTRL_I2S_DIV_SEL_Pos;
}

static inline void wj_vad_dai_mode_pdm_low_power_mode(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_PDM_MODE_STANDARD;
}

static inline void wj_vad_dai_mode_pdm_standard_mode(wj_vad_regs_t *vad_base)
{
    vad_base->DAI_CTRL |= WJ_VAD_DAI_CTRL_PDM_MODE_STANDARD;
}

static inline void wj_vad_set_dai_mode_pdm_low_power_gain(wj_vad_regs_t *vad_base, uint32_t gain)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_LP_GAIN_Msk;
    vad_base->DAI_CTRL |= gain << WJ_VAD_DAI_CTRL_LP_GAIN_Pos;
}

static inline void wj_vad_set_dai_mode_pdm_standard_gain(wj_vad_regs_t *vad_base, uint32_t gain)
{
    vad_base->DAI_CTRL &= ~WJ_VAD_DAI_CTRL_ST_GAIN_Msk;
    vad_base->DAI_CTRL |= gain << WJ_VAD_DAI_CTRL_ST_GAIN_Pos;
}

static inline uint32_t wj_vad_get_ch_fifo(wj_vad_regs_t *vad_base, uint32_t ch)
{
    return (uint32_t)(vad_base->FIFO_CNT >> (8U * ch) & 0xfU);
}

static inline uint32_t wj_vad_get_ch_left_fifo(wj_vad_regs_t *vad_base, uint32_t ch)
{
    return (uint32_t)(vad_base->FIFO_CNT >> (8U * ch + 4U) & 0xfU);
}

static inline uint32_t wj_vad_get_ch_right_fifo(wj_vad_regs_t *vad_base, uint32_t ch)
{
    return (uint32_t)(vad_base->FIFO_CNT >> (8U * ch) & 0xfU);
}

static inline void wj_vad_set_dma_level(wj_vad_regs_t *vad_base, uint32_t th)
{
    vad_base->FIFO_TH_CTRL = (th & 0xfU);
}

static inline uint32_t wj_vad_get_fifo_irq_status(wj_vad_regs_t *vad_base)
{
    return vad_base->FIFO_INTR_MFLAG;
}

static inline uint32_t wj_vad_get_fifo_raw_irq_status(wj_vad_regs_t *vad_base)
{
    return vad_base->FIFO_INTR_FLAG;
}

static inline void wj_vad_claer_fifo_raw_irq_status(wj_vad_regs_t *vad_base, uint32_t irq_status)
{
    vad_base->FIFO_INTR_CLR = irq_status;
}

static inline void wj_vad_set_fifo_raw_irq(wj_vad_regs_t *vad_base, uint32_t mask)
{
    vad_base->FIFO_INTR_MASK = mask;
}

static inline void wj_vad_set_left_st_offset(wj_vad_regs_t *vad_base, uint32_t offset)
{
    vad_base->CIC_L_ST_OFFSET = offset;
}

static inline void wj_vad_set_left_lp_offset(wj_vad_regs_t *vad_base, uint32_t offset)
{
    vad_base->CIC_L_LP_OFFSET = offset;
}

static inline void wj_vad_set_right_st_offset(wj_vad_regs_t *vad_base, uint32_t offset)
{
    vad_base->CIC_R_ST_OFFSET = offset;
}

static inline void wj_vad_set_right_lp_offset(wj_vad_regs_t *vad_base, uint32_t offset)
{
    vad_base->CIC_R_LP_OFFSET = offset;
}

static inline uint32_t wj_vad_read_ch_fifo(wj_vad_regs_t *vad_base, uint32_t ch_idx)
{
    return *(volatile uint32_t *)(&(vad_base->FIFO0_L_READ) + 4U * ch_idx);
}

static inline uint32_t wj_vad_read_ch0_left_fifo(wj_vad_regs_t *vad_base)
{
    return vad_base->FIFO0_L_READ;
}

static inline uint32_t wj_vad_read_ch0_right_fifo(wj_vad_regs_t *vad_base)
{
    return vad_base->FIFO0_R_READ;
}

static inline uint32_t wj_vad_read_ch1_left_fifo(wj_vad_regs_t *vad_base)
{
    return vad_base->FIFO1_L_READ;
}

static inline uint32_t wj_vad_read_ch1_right_fifo(wj_vad_regs_t *vad_base)
{
    return vad_base->FIFO1_R_READ;
}

static inline uint32_t wj_vad_read_ch2_left_fifo(wj_vad_regs_t *vad_base)
{
    return vad_base->FIFO2_L_READ;
}

static inline uint32_t wj_vad_read_ch2_right_fifo(wj_vad_regs_t *vad_base)
{
    return vad_base->FIFO2_R_READ;
}

static inline uint32_t wj_vad_read_ch3_left_fifo(wj_vad_regs_t *vad_base)
{
    return vad_base->FIFO3_L_READ;
}

static inline uint32_t wj_vad_read_ch3_right_fifo(wj_vad_regs_t *vad_base)
{
    return vad_base->FIFO3_R_READ;
}


#ifdef __cplusplus
}
#endif

#endif /* WJ_VAD_LL_H */
