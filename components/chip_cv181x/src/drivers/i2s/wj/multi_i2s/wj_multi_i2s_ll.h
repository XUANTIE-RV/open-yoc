/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_multi_i2s_ll.h
 * @brief    header file for wj multi_i2s ll driver
 * @version  V1.0
 * @date     11. Nov 2020
 ******************************************************************************/

#ifndef _WJ_MULTI_I2S_LL_H_
#define _WJ_MULTI_I2S_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * I2S register bit definitions
 */

/* IISEN , offset: 0x00 */
#define WJ_IISEN_I2SEN_Pos                         (0U)
#define WJ_IISEN_I2SEN_Msk                         (0x1U << WJ_IISEN_I2SEN_Pos)
#define WJ_IISEN_I2SEN                             WJ_IISEN_I2SEN_Msk

/* FUNCMODE, offset: 0x04 */
#define WJ_FUNCMODE_TMODE_Pos                      (0U)
#define WJ_FUNCMODE_TMODE_Msk                      (0x1U << WJ_FUNCMODE_TMODE_Pos)
#define WJ_FUNCMODE_TMODE                          WJ_FUNCMODE_TMODE_Msk

#define WJ_FUNCMODE_TMODE_WEN_Pos                  (1U)
#define WJ_FUNCMODE_TMODE_WEN_Msk                  (0x1U << WJ_FUNCMODE_TMODE_WEN_Pos)
#define WJ_FUNCMODE_TMODE_WEN                      WJ_FUNCMODE_TMODE_WEN_Msk

#define WJ_FUNCMODE_RMODE_Pos                      (4U)
#define WJ_FUNCMODE_RMODE_Msk                      (0x1U << WJ_FUNCMODE_RMODE_Pos)
#define WJ_FUNCMODE_RMODE                          WJ_FUNCMODE_RMODE_Msk

#define WJ_FUNCMODE_RMODE_WEN_Pos                  (5U)
#define WJ_FUNCMODE_RMODE_WEN_Msk                  (0x1U << WJ_FUNCMODE_RMODE_WEN_Pos)
#define WJ_FUNCMODE_RMODE_WEN                      WJ_FUNCMODE_RMODE_WEN_Msk

#define WJ_FUNCMODE_CH0_ENABLE_Pos                 (8U)
#define WJ_FUNCMODE_CH0_ENABLE_Msk                 (0x1U << WJ_FUNCMODE_CH0_ENABLE_Pos)
#define WJ_FUNCMODE_CH0_ENABLE                     WJ_FUNCMODE_CH0_ENABLE_Msk

#define WJ_FUNCMODE_CH1_ENABLE_Pos                 (9U)
#define WJ_FUNCMODE_CH1_ENABLE_Msk                 (0x1U << WJ_FUNCMODE_CH1_ENABLE_Pos)
#define WJ_FUNCMODE_CH1_ENABLE                     WJ_FUNCMODE_CH1_ENABLE_Msk

#define WJ_FUNCMODE_CH2_ENABLE_Pos                 (10U)
#define WJ_FUNCMODE_CH2_ENABLE_Msk                 (0x1U << WJ_FUNCMODE_CH2_ENABLE_Pos)
#define WJ_FUNCMODE_CH2_ENABLE                     WJ_FUNCMODE_CH2_ENABLE_Msk

#define WJ_FUNCMODE_CH3_ENABLE_Pos                 (11U)
#define WJ_FUNCMODE_CH3_ENABLE_Msk                 (0x1U << WJ_FUNCMODE_CH3_ENABLE_Pos)
#define WJ_FUNCMODE_CH3_ENABLE                     WJ_FUNCMODE_CH3_ENABLE_Msk

#define WJ_FUNCMODE_CH4_ENABLE_Pos                 (12U)
#define WJ_FUNCMODE_CH4_ENABLE_Msk                 (0x1U << WJ_FUNCMODE_CH4_ENABLE_Pos)
#define WJ_FUNCMODE_CH4_ENABLE                     WJ_FUNCMODE_CH4_ENABLE_Msk

/* IISCNFIN, offset: 0x08 */
#define WJ_IISCNFIN_RSAFS_Pos                      (0U)
#define WJ_IISCNFIN_RSAFS_Msk                      (0x3U << WJ_IISCNFIN_RSAFS_Pos)
#define WJ_IISCNFIN_RSAFS_I2S                      (0x0U << WJ_IISCNFIN_RSAFS_Pos)
#define WJ_IISCNFIN_RSAFS_RIGHT_JUSTIFIED          (0x1U << WJ_IISCNFIN_RSAFS_Pos)
#define WJ_IISCNFIN_RSAFS_LEFT_JUSTIFIED           (0x2U << WJ_IISCNFIN_RSAFS_Pos)
#define WJ_IISCNFIN_RSAFS_PCM                      (0x3U << WJ_IISCNFIN_RSAFS_Pos)

#define WJ_IISCNFIN_RALOLRC_Pos                    (2U)
#define WJ_IISCNFIN_RALOLRC_Msk                    (0x1U << WJ_IISCNFIN_RALOLRC_Pos)
#define WJ_IISCNFIN_RALOLRC_HIGHFORLEFT            WJ_IISCNFIN_RALOLRC_Msk

#define WJ_IISCNFIN_RVOICEEN_Pos                   (4U)
#define WJ_IISCNFIN_RVOICEEN_Msk                   (0x1U << WJ_IISCNFIN_RVOICEEN_Pos)
#define WJ_IISCNFIN_RVOICEEN_MONO                  WJ_IISCNFIN_RVOICEEN_Msk

#define WJ_IISCNFIN_RX_CH_SEL_Pos                  (5U)
#define WJ_IISCNFIN_RX_CH_SEL_Msk                  (0x1U << WJ_IISCNFIN_RX_CH_SEL_Pos)
#define WJ_IISCNFIN_RX_CH_SEL_LEFT                 WJ_IISCNFIN_RX_CH_SEL_Msk

#define WJ_IISCNFIN_I2S_RXMODE_Pos                 (8U)
#define WJ_IISCNFIN_I2S_RXMODE_Msk                 (0x1U << WJ_IISCNFIN_I2S_RXMODE_Pos)
#define WJ_IISCNFIN_I2S_RXMODE_MASTER_MODE         WJ_IISCNFIN_I2S_RXMODE_Msk

#define WJ_IISCNFIN_I2S_RX_CLK_SEL_Pos             (9U)
#define WJ_IISCNFIN_I2S_RX_CLK_SEL_Msk             (0x1U << WJ_IISCNFIN_I2S_RX_CLK_SEL_Pos)
#define WJ_IISCNFIN_I2S_RX_CLK_SEL_MCLK            WJ_IISCNFIN_I2S_RX_CLK_SEL_Msk

#define WJ_IISCNFIN_I2S_RDELAY_Pos                 (12U)
#define WJ_IISCNFIN_I2S_RDELAY_Msk                 (0x3U << WJ_IISCNFIN_I2S_RDELAY_Pos)
#define WJ_IISCNFIN_I2S_RDELAY_NO_DELAY            (0x0U << WJ_IISCNFIN_I2S_RDELAY_Pos)
#define WJ_IISCNFIN_I2S_RDELAY_1_DELAY             (0x1U << WJ_IISCNFIN_I2S_RDELAY_Pos)
#define WJ_IISCNFIN_I2S_RDELAY_2_DELAY             (0x2U << WJ_IISCNFIN_I2S_RDELAY_Pos)
#define WJ_IISCNFIN_I2S_RDELAY_3_DELAY             (0x3U << WJ_IISCNFIN_I2S_RDELAY_Pos)

/* FSSTA , offset: 0x0C */
#define WJ_FSSTA_AIRAD_Pos                         (0U)
#define WJ_FSSTA_AIRAD_Msk                         (0x1U << WJ_FSSTA_AIRAD_Pos)
#define WJ_FSSTA_AIRAD                             WJ_FSSTA_AIRAD_Msk

#define WJ_FSSTA_AFR_Pos                           (4U)
#define WJ_FSSTA_AFR_Msk                           (0x3U << WJ_FSSTA_AFR_Pos)
#define WJ_FSSTA_AFR_88_2KSPS                      (0x0U << WJ_FSSTA_AFR_Pos)
#define WJ_FSSTA_AFR_96KSPS                        (0x1U << WJ_FSSTA_AFR_Pos)
#define WJ_FSSTA_AFR_64KSPS                        (0x2U << WJ_FSSTA_AFR_Pos)
#define WJ_FSSTA_AFR_192KSPS                       (0x3U << WJ_FSSTA_AFR_Pos)

#define WJ_FSSTA_ARS_Pos                           (6U)
#define WJ_FSSTA_ARS_Msk                           (0x3U << WJ_FSSTA_ARS_Pos)
#define WJ_FSSTA_ARS_1                             (0x0U << WJ_FSSTA_ARS_Pos)
#define WJ_FSSTA_ARS_0_5                           (0x1U << WJ_FSSTA_ARS_Pos)
#define WJ_FSSTA_ARS_0_25                          (0x2U << WJ_FSSTA_ARS_Pos)
#define WJ_FSSTA_ARS_0_125                         (0x3U << WJ_FSSTA_ARS_Pos)

#define WJ_FSSTA_DATAWTH_Pos                       (8U)
#define WJ_FSSTA_DATAWTH_Msk                       (0xFU << WJ_FSSTA_DATAWTH_Pos)

#define WJ_FSSTA_SCLK_SEL_Pos                      (12U)
#define WJ_FSSTA_SCLK_SEL_Msk                      (0x3U << WJ_FSSTA_SCLK_SEL_Pos)
#define WJ_FSSTA_SCLK_SEL_32                       (0x0U << WJ_FSSTA_SCLK_SEL_Pos)
#define WJ_FSSTA_SCLK_SEL_48                       (0x1U << WJ_FSSTA_SCLK_SEL_Pos)
#define WJ_FSSTA_SCLK_SEL_64                       (0x2U << WJ_FSSTA_SCLK_SEL_Pos)
#define WJ_FSSTA_SCLK_SEL_16                       (0x3U << WJ_FSSTA_SCLK_SEL_Pos)

#define WJ_FSSTA_MCLK_SEL_Pos                      (16U)
#define WJ_FSSTA_MCLK_SEL_Msk                      (0x1U << WJ_FSSTA_MCLK_SEL_Pos)
#define WJ_FSSTA_MCLK_SEL_384                      WJ_FSSTA_MCLK_SEL_Msk

/* IISCNFOUT, offset: 0x10 */
#define WJ_IISCNFOUT_TSAFS_Pos                     (0U)
#define WJ_IISCNFOUT_TSAFS_Msk                     (0x3U << WJ_IISCNFOUT_TSAFS_Pos)
#define WJ_IISCNFOUT_TSAFS_I2S                     (0x0U << WJ_IISCNFOUT_TSAFS_Pos)
#define WJ_IISCNFOUT_TSAFS_RIGHT_JUSTIFIED         (0x1U << WJ_IISCNFOUT_TSAFS_Pos)
#define WJ_IISCNFOUT_TSAFS_LEFT_JUSTIFIED          (0x2U << WJ_IISCNFOUT_TSAFS_Pos)
#define WJ_IISCNFOUT_TSAFS_PCM                     (0x3U << WJ_IISCNFOUT_TSAFS_Pos)

#define WJ_IISCNFOUT_TALOLRC_Pos                   (2U)
#define WJ_IISCNFOUT_TALOLRC_Msk                   (0x1U << WJ_IISCNFOUT_TALOLRC_Pos)
#define WJ_IISCNFOUT_TALOLRC_HIGHFORLEFT           WJ_IISCNFOUT_TALOLRC_Msk

#define WJ_IISCNFOUT_TX_VOICE_EN_Pos               (3U)
#define WJ_IISCNFOUT_TX_VOICE_EN_Msk               (0x1U << WJ_IISCNFOUT_TX_VOICE_EN_Pos)
#define WJ_IISCNFOUT_TX_VOICE_EN_MONO              WJ_IISCNFOUT_TX_VOICE_EN_Msk

#define WJ_IISCNFOUT_I2S_TXMODE_Pos                (4U)
#define WJ_IISCNFOUT_I2S_TXMODE_Msk                (0x1U << WJ_IISCNFOUT_I2S_TXMODE_Pos)
#define WJ_IISCNFOUT_I2S_TXMODE_SLAVE              WJ_IISCNFOUT_I2S_TXMODE_Msk

#define WJ_IISCNFOUT_TX_CLK_SEL_Pos                (5U)
#define WJ_IISCNFOUT_TX_CLK_SEL_Msk                (0x1U << WJ_IISCNFOUT_TX_CLK_SEL_Pos)
#define WJ_IISCNFOUT_TX_CLK_SEL_MCLK               WJ_IISCNFOUT_TX_CLK_SEL_Msk

/* FADTLR, offset: 0x14 */
#define WJ_FADTLR_96FTR_Pos                        (0U)
#define WJ_FADTLR_96FTR_Msk                        (0x3FU << WJ_FADTLR_96FTR_Pos)

#define WJ_FADTLR_88FTR_Pos                        (8U)
#define WJ_FADTLR_88FTR_Msk                        (0x3FU << WJ_FADTLR_88FTR_Pos)

#define WJ_FADTLR_64FTR_Pos                        (16U)
#define WJ_FADTLR_64FTR_Msk                        (0x3FU << WJ_FADTLR_64FTR_Pos)

#define WJ_FADTLR_192FTR_Pos                       (24U)
#define WJ_FADTLR_192FTR_Msk                       (0x3FU << WJ_FADTLR_192FTR_Pos)

/* SCCR, offset: 0x18 */
#define WJ_SCCR_RVCCR_Pos                          (0U)
#define WJ_SCCR_RVCCR_Msk                          (0x1FU << WJ_SCCR_RVCCR_Pos)

#define WJ_SCCR_SSRCR_Pos                          (5U)
#define WJ_SCCR_SSRCR_Msk                          (0x3U << WJ_SCCR_SSRCR_Pos)
#define WJ_SCCR_SSRCR_NO_COMPRESS                  (0x0U << WJ_SCCR_SSRCR_Pos)
#define WJ_SCCR_SSRCR_ONE_COMPRESS                 (0x1U << WJ_SCCR_SSRCR_Pos)
#define WJ_SCCR_SSRCR_THREE_COMPRESS               (0x2U << WJ_SCCR_SSRCR_Pos)

#define WJ_SCCR_TVCCR_Pos                          (8U)
#define WJ_SCCR_TVCCR_Msk                          (0x3U << WJ_SCCR_TVCCR_Pos)
#define WJ_SCCR_TVCCR_NO_COMPRESS                  (0x0U << WJ_SCCR_TVCCR_Pos)
#define WJ_SCCR_TVCCR_ONE_COMPRESS                 (0x1U << WJ_SCCR_TVCCR_Pos)
#define WJ_SCCR_TVCCR_THREE_COMPRESS               (0x2U << WJ_SCCR_TVCCR_Pos)

/* TXFTLR, offset: 0x1C */
#define WJ_TXFTLR_TFT_Pos                          (0U)
#define WJ_TXFTLR_TFT_Msk                          (0xFU << WJ_TXFTLR_TFT_Pos)

/* RXFTLR, offset: 0x20 */
#define WJ_RXFTLR_RFT_Pos                          (0U)
#define WJ_RXFTLR_RFT_Msk                          (0xFU << WJ_RXFTLR_RFT_Pos)

/* TXFLR, offset: 0x24 */
#define WJ_TXFLR_TXTFL_Pos                         (0U)

/* RXFLR, offset: 0x28 */
#define WJ_RXFLR_RXTFL_Pos                         (0U)

/* SR, offset: 0x2C */
#define WJ_SR_RXBUSY_Pos                           (0U)
#define WJ_SR_RXBUSY_Msk                           (0x1U << WJ_SR_RXBUSY_Pos)
#define WJ_SR_RXBUSY_STATUS                        WJ_SR_RXBUSY_Msk

#define WJ_SR_TXBUSY_Pos                           (1U)
#define WJ_SR_TXBUSY_Msk                           (0x1U << WJ_SR_TXBUSY_Pos)
#define WJ_SR_TXBUSY_STATUS                        WJ_SR_TXBUSY_Msk

#define WJ_SR_TFNF_Pos                             (2U)
#define WJ_SR_TFNF_Msk                             (0x1U << WJ_SR_TFNF_Pos)
#define WJ_SR_TFNF_TX_FIFO_NOT_FULL                WJ_SR_TFNF_Msk

#define WJ_SR_TFE_Pos                              (3U)
#define WJ_SR_TFE_Msk                              (0x1U << WJ_SR_TFE_Pos)
#define WJ_SR_TFE_TX_FIFO_EMPTY                    WJ_SR_TFE_Msk

#define WJ_SR_RFNE_Pos                             (4U)
#define WJ_SR_RFNE_Msk                             (0x1U << WJ_SR_RFNE_Pos)
#define WJ_SR_RFNE_RX_FIFO_NOT_EMPTY               WJ_SR_RFNE_Msk

#define WJ_SR_RFF_Pos                              (5U)
#define WJ_SR_RFF_Msk                              (0x1U << WJ_SR_RFF_Pos)
#define WJ_SR_RFF_RX_FIFO_FULL                     WJ_SR_RFF_Msk

/* IMR, offset: 0x30 */
#define WJ_IMR_WADEM_Pos                            (0U)
#define WJ_IMR_WADEM_Msk                            (0x1U << WJ_IMR_WADEM_Pos)
#define WJ_IMR_WADEM_INTR_MSK                       WJ_IMR_WADEM_Msk

#define WJ_IMR_TXUIRM_Pos                           (1U)
#define WJ_IMR_TXUIRM_Msk                           (0x1U << WJ_IMR_TXUIRM_Pos)
#define WJ_IMR_TXUIRM_INTR_MSK                      WJ_IMR_TXUIRM_Msk

#define WJ_IMR_TXOIM_Pos                            (2U)
#define WJ_IMR_TXOIM_Msk                            (0x1U << WJ_IMR_TXOIM_Pos)
#define WJ_IMR_TXOIM_INTR_MSK                       WJ_IMR_TXOIM_Msk

#define WJ_IMR_RXUIM_Pos                            (3U)
#define WJ_IMR_RXUIM_Msk                            (0x1U << WJ_IMR_RXUIM_Pos)
#define WJ_IMR_RXUIM_INTR_MSK                       WJ_IMR_RXUIM_Msk

#define WJ_IMR_RXOIM_Pos                            (4U)
#define WJ_IMR_RXOIM_Msk                            (0x1U << WJ_IMR_RXOIM_Pos)
#define WJ_IMR_RXOIM_INTR_MSK                       WJ_IMR_RXOIM_Msk

#define WJ_IMR_TXEIM_Pos                            (5U)
#define WJ_IMR_TXEIM_Msk                            (0x1U << WJ_IMR_TXEIM_Pos)
#define WJ_IMR_TXEIM_INTR_MSK                       WJ_IMR_TXEIM_Msk

#define WJ_IMR_RXFIM_Pos                            (6U)
#define WJ_IMR_RXFIM_Msk                            (0x1U << WJ_IMR_RXFIM_Pos)
#define WJ_IMR_RXFIM_INTR_MSK                       WJ_IMR_RXFIM_Msk

#define WJ_IMR_IRBFCM_Pos                           (7U)
#define WJ_IMR_IRBFCM_Msk                           (0x1U << WJ_IMR_IRBFCM_Pos)
#define WJ_IMR_IRBFCM_INTR_MSK                      WJ_IMR_IRBFCM_Msk

#define WJ_IMR_ITBFCM_Pos                           (8U)
#define WJ_IMR_ITBFCM_Msk                           (0x1U << WJ_IMR_ITBFCM_Pos)
#define WJ_IMR_ITBFCM_INTR_MSK                      WJ_IMR_ITBFCM_Msk

#define WJ_IMR_IFSCM_Pos                            (9U)
#define WJ_IMR_IFSCM_Msk                            (0x1U << WJ_IMR_IFSCM_Pos)
#define WJ_IMR_IFSCM_INTR_MSK                       WJ_IMR_IFSCM_Msk

/* ISR, offset: 0x34 */
#define WJ_ISR_WADES_Pos                            (0U)
#define WJ_ISR_WADES_Msk                            (0x1U << WJ_ISR_WADES_Pos)
#define WJ_ISR_WADES_STATUS                         WJ_ISR_WADES_Msk

#define WJ_ISR_TXUIRS_Pos                           (1U)
#define WJ_ISR_TXUIRS_Msk                           (0x1U << WJ_ISR_TXUIRS_Pos)
#define WJ_ISR_TXUIRS_STATUS                        WJ_ISR_TXUIRS_Msk

#define WJ_ISR_TXOIS_Pos                            (2U)
#define WJ_ISR_TXOIS_Msk                            (0x1U << WJ_ISR_TXOIS_Pos)
#define WJ_ISR_TXOIS_STATUS                         WJ_ISR_TXOIS_Msk

#define WJ_ISR_RXUIS_Pos                            (3U)
#define WJ_ISR_RXUIS_Msk                            (0x1U << WJ_ISR_RXUIS_Pos)
#define WJ_ISR_RXUIS_STATUS                         WJ_ISR_RXUIS_Msk

#define WJ_ISR_RXOIS_Pos                            (4U)
#define WJ_ISR_RXOIS_Msk                            (0x1U << WJ_ISR_RXOIS_Pos)
#define WJ_ISR_RXOIS_STATUS                         WJ_ISR_RXOIS_Msk

#define WJ_ISR_TXEIS_Pos                            (5U)
#define WJ_ISR_TXEIS_Msk                            (0x1U << WJ_ISR_TXEIS_Pos)
#define WJ_ISR_TXEIS_STATUS                         WJ_ISR_TXEIS_Msk

#define WJ_ISR_RXFIS_Pos                            (6U)
#define WJ_ISR_RXFIS_Msk                            (0x1U << WJ_ISR_RXFIS_Pos)
#define WJ_ISR_RXFIS_STATUS                         WJ_ISR_RXFIS_Msk

#define WJ_ISR_IRBFCS_Pos                           (7U)
#define WJ_ISR_IRBFCS_Msk                           (0x1U << WJ_ISR_IRBFCS_Pos)
#define WJ_ISR_IRBFCS_STATUS                        WJ_ISR_IRBFCS_Msk

#define WJ_ISR_ITBFCS_Pos                           (8U)
#define WJ_ISR_ITBFCS_Msk                           (0x1U << WJ_ISR_ITBFCS_Pos)
#define WJ_ISR_ITBFCS_STATUS                        WJ_ISR_ITBFCS_Msk

#define WJ_ISR_IFSCS_Pos                            (9U)
#define WJ_ISR_IFSCS_Msk                            (0x1U << WJ_ISR_IFSCS_Pos)
#define WJ_ISR_IFSCS_STATUS                         WJ_ISR_IFSCS_Msk

/* RISR, offset: 0x38 */
#define WJ_RISR_RWADES_Pos                           (0U)
#define WJ_RISR_RWADES_Msk                           (0x1U << WJ_RISR_RWADES_Pos)
#define WJ_RISR_RWADES_RAW                           WJ_RISR_RWADES_Msk

#define WJ_RISR_TXUIR_Pos                            (1U)
#define WJ_RISR_TXUIR_Msk                            (0x1U << WJ_RISR_TXUIR_Pos)
#define WJ_RISR_TXUIR_RAW                            WJ_RISR_TXUIR_Msk

#define WJ_RISR_TXOIR_Pos                           (2U)
#define WJ_RISR_TXOIR_Msk                           (0x1U << WJ_RISR_TXOIR_Pos)
#define WJ_RISR_TXOIR_RAW                           WJ_RISR_TXOIR_Msk

#define WJ_RISR_RXUIR_Pos                           (3U)
#define WJ_RISR_RXUIR_Msk                           (0x1U << WJ_RISR_RXUIR_Pos)
#define WJ_RISR_RXUIR_RAW                           WJ_RISR_RXUIR_Msk

#define WJ_RISR_RXOIR_Pos                           (4U)
#define WJ_RISR_RXOIR_Msk                           (0x1U << WJ_RISR_RXOIR_Pos)
#define WJ_RISR_RXOIR_RAW                           WJ_RISR_RXOIR_Msk

#define WJ_RISR_TXEIR_Pos                           (5U)
#define WJ_RISR_TXEIR_Msk                           (0x1U << WJ_RISR_TXEIR_Pos)
#define WJ_RISR_TXEIR_RAW                           WJ_RISR_TXEIR_Msk

#define WJ_RISR_RXFIR_Pos                           (6U)
#define WJ_RISR_RXFIR_Msk                           (0x1U << WJ_RISR_RXFIR_Pos)
#define WJ_RISR_RXFIR_RAW                           WJ_RISR_RXFIR_Msk

#define WJ_RISR_RIRBFCS_Pos                         (7U)
#define WJ_RISR_RIRBFCS_Msk                         (0x1U << WJ_RISR_RIRBFCS_Pos)
#define WJ_RISR_RIRBFCS_RAW                         WJ_RISR_RIRBFCS_Msk

#define WJ_RISR_RITBFCS_Pos                         (8U)
#define WJ_RISR_RITBFCS_Msk                         (0x1U << WJ_RISR_RITBFCS_Pos)
#define WJ_RISR_RITBFCS_RAW                         WJ_RISR_RITBFCS_Msk

#define WJ_RISR_RIFSCS_Pos                          (9U)
#define WJ_RISR_RIFSCS_Msk                          (0x1U << WJ_RISR_RIFSCS_Pos)
#define WJ_RISR_RIFSCS_RAW                          WJ_RISR_RIFSCS_Msk

/* ICR, offset: 0x3C */
#define WJ_ICR_CWADEC_Pos                           (0U)
#define WJ_ICR_CWADEC_Msk                           (0x1U << WJ_ICR_CWADEC_Pos)
#define WJ_ICR_CWADEC_CLEAR                         WJ_ICR_CWADEC_Msk

#define WJ_ICR_TXUIC_Pos                            (1U)
#define WJ_ICR_TXUIC_Msk                            (0x1U << WJ_ICR_TXUIC_Pos)
#define WJ_ICR_TXUIC_CLEAR                          WJ_ICR_TXUIC_Msk

#define WJ_ICR_TXOIC_Pos                            (2U)
#define WJ_ICR_TXOIC_Msk                            (0x1U << WJ_ICR_TXOIC_Pos)
#define WJ_ICR_TXOIC_CLEAR                          WJ_ICR_TXOIC_Msk

#define WJ_ICR_RXUIC_Pos                            (3U)
#define WJ_ICR_RXUIC_Msk                            (0x1U << WJ_ICR_RXUIC_Pos)
#define WJ_ICR_RXUIC_CLEAR                          WJ_ICR_RXUIC_Msk

#define WJ_ICR_RXOIC_Pos                            (4U)
#define WJ_ICR_RXOIC_Msk                            (0x1U << WJ_ICR_RXOIC_Pos)
#define WJ_ICR_RXOIC_CLEAR                          WJ_ICR_RXOIC_Msk

#define WJ_ICR_TXEIC_Pos                            (5U)
#define WJ_ICR_TXEIC_Msk                            (0x1U << WJ_ICR_TXEIC_Pos)
#define WJ_ICR_TXEIC_CLEAR                          WJ_ICR_TXEIC_Msk

#define WJ_ICR_RXFIC_Pos                            (6U)
#define WJ_ICR_RXFIC_Msk                            (0x1U << WJ_ICR_RXFIC_Pos)
#define WJ_ICR_RXFIC_CLEAR                          WJ_ICR_RXFIC_Msk

#define WJ_ICR_CRIRBFC_Pos                          (7U)
#define WJ_ICR_CRIRBFC_Msk                          (0x1U << WJ_ICR_CRIRBFC_Pos)
#define WJ_ICR_CRIRBFC_CLEAR                        WJ_ICR_CRIRBFC_Msk

#define WJ_ICR_CRITBFC_Pos                          (8U)
#define WJ_ICR_CRITBFC_Msk                          (0x1U << WJ_ICR_CRITBFC_Pos)
#define WJ_ICR_CRITBFC_CLEAR                        WJ_ICR_CRITBFC_Msk

#define WJ_ICR_CRIFSC_Pos                           (9U)
#define WJ_ICR_CRIFSC_Msk                           (0x1U << WJ_ICR_CRIFSC_Pos)
#define WJ_ICR_CRIFSC_CLEAR                         WJ_ICR_CRIFSC_Msk

/* DMACR, offset: 0x40 */
#define WJ_DMACR_RDMAE_Pos                          (0U)
#define WJ_DMACR_RDMAE_Msk                          (0x1U << WJ_DMACR_RDMAE_Pos)
#define WJ_DMACR_RDMAE_EN                           WJ_DMACR_RDMAE_Msk

#define WJ_DMACR_TDMAE_Pos                          (1U)
#define WJ_DMACR_TDMAE_Msk                          (0x1U << WJ_DMACR_TDMAE_Pos)
#define WJ_DMACR_TDMAE_EN                           WJ_DMACR_TDMAE_Msk

/* DMATDLR, offset: 0x44 */
#define WJ_DMATDLR_DMATDL_Pos                       (0U)
#define WJ_DMATDLR_DMATDL_Msk                       (0x1FU << WJ_DMATDLR_DMATDL_Pos)

/* DMARDLR, offset: 0x48 */
#define WJ_DMARDLR_DMATDL_Pos                       (0U)
#define WJ_DMARDLR_DMATDL_Msk                       (0x1FU << WJ_DMARDLR_DMATDL_Pos)

/* DR, offset: 0x4C */
#define WJ_DR_DR_Pos                                (0U)

/* DIV0LEVEL, offset: 0x50 */
#define WJ_DIV0LEVEL_DIV0_Pos                       (0U)
#define WJ_DIV0LEVEL_DIV0_Msk                       (0XFFU << WJ_DIV0LEVEL_DIV0_Pos)

/* DIV3LEVEL, offset: 0x54 */
#define WJ_DIV3LEVEL_DIV3_Pos                       (0U)
#define WJ_DIV0LEVEL_DIV3_Msk                       (0XFFU << WJ_DIV3LEVEL_DIV3_Pos)

#define WJ_I2S_DATA_WIDTH_8BIT                      (0xFU)
#define WJ_I2S_DATA_WIDTH_16BIT                     (0U)
#define WJ_I2S_DATA_WIDTH_24BIT                     (0x5U)
#define WJ_I2S_DATA_WIDTH_32BIT                     (0xAU)
#define WJ_I2S_DATA_WIDTH_32BIT_OUPUT               (0x8U)

#define TXFIFO_IRQ_TH                               (0x8U)
#define RXFIFO_IRQ_TH                               (0x20U)
#define I2S_MAX_FIFO                                (0x20U)

typedef struct {
    __IOM uint32_t I2S_IISEN;            /* Offset: 0x000h (R/W)  IIS Enable Register */
    __IOM uint32_t I2S_FUNCMODE;         /* Offset: 0x004h (R/W)  IIS function mode */
    __IOM uint32_t I2S_IISCNF_IN;        /* Offset: 0x008h (R/W)  IIS interface configuration in (on RX side) */
    __IOM uint32_t I2S_FSSTA;            /* Offset: 0x00ch (R/W)  IIS ATX audio input control/state register */
    __IOM uint32_t I2S_IISCNF_OUT;       /* Offset: 0x010h (R/W)  IIS interface configuration in (on TX side) */
    __IOM uint32_t I2S_FADTLR;           /* Offset: 0x014h (R/W)  IIS Fs auto detected Threshold level register */
    __IOM uint32_t I2S_SCCR;             /* Offset: 0x018h (R/W)  Sample compress control register */
    __IOM uint32_t I2S_TXFTLR;           /* Offset: 0x01ch (R/W)  Transmit FIFO Threshold Level */
    __IOM uint32_t I2S_RXFTLR;           /* Offset: 0x020h (R/W)  Receive FIFO Threshold Level */
    __IOM uint32_t I2S_TXFLR;            /* Offset: 0x024h (R/W)  Transmit FIFO Level Register */
    __IOM uint32_t I2S_RXFLR;            /* Offset: 0x028h (R/W)  Receive FIFO Level Register */
    __IM  uint32_t I2S_SR;               /* Offset: 0x02ch (R)    Status Register */
    __IOM uint32_t I2S_IMR;              /* Offset: 0x030h (R/W)  Interrupt Mask Register */
    __IM  uint32_t I2S_ISR;              /* Offset: 0x034h (R)    Interrupt Status Register */
    __IOM uint32_t I2S_RISR;             /* Offset: 0x038h (R/W)  Raw Interrupt Status Register */
    __OM  uint32_t I2S_ICR;              /* Offset: 0x03ch (W)    FIFO Interrupt Clear Register */
    __IOM uint32_t I2S_DMACR;            /* Offset: 0x040h (R/W)  DMA Control Register */
    __IOM uint32_t I2S_DMATDLR;          /* Offset: 0x044h (R/W)  DMA Transmit Data Level */
    __IOM uint32_t I2S_DMARDLR;          /* Offset: 0x048h (R/W)  DMA Receive Data Level */
    __IOM uint32_t I2S_DR;               /* Offset: 0x04Ch (R/W)  Data Register */
    __IOM uint32_t I2S_DIV0_LEVEL;       /* Offset: 0x050h (R/W)  Divide i2s_clkgen source clock, get mclk_o */
    __IOM uint32_t I2S_DIV3_LEVEL;       /* Offset: 0x054h (R/W)  Divide i2s_clkgen source clock, get reference clock */
    uint32_t RESERVED;                   /* Offset: 0x058h (R)    RESERVED */
    uint32_t RESERVED1;                  /* Offset: 0x05Ch (R)    RESERVED */
    __IOM uint32_t I2S_DR1;              /* Offset: 0x060h (R/W)  CH1_Data Register */
    __IOM uint32_t I2S_DR2;              /* Offset: 0x064h (R/W)  CH2_Data Register */
    __IOM uint32_t I2S_DR3;              /* Offset: 0x068h (R/W)  CH3_Data Register */
    __IOM uint32_t I2S_DR4;              /* Offset: 0x06Ch (R/W)  CH4_Data Register */
} wj_i2s_multi_regs_t;


static inline void wj_i2s_enable(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISEN |= WJ_IISEN_I2SEN;
}

static inline void wj_i2s_disable(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISEN &= ~WJ_IISEN_I2SEN;
}

static inline void wj_i2s_enable_ch(wj_i2s_multi_regs_t *i2s_base, uint32_t ch)
{
    i2s_base->I2S_FUNCMODE |= 1 << (WJ_FUNCMODE_CH0_ENABLE_Pos + ch);
}

static inline void wj_i2s_disable_ch(wj_i2s_multi_regs_t *i2s_base, uint32_t ch)
{
    i2s_base->I2S_FUNCMODE &= ~(0 << (WJ_FUNCMODE_CH0_ENABLE_Pos + ch));
}

static inline void wj_i2s_set_transmit_mode(wj_i2s_multi_regs_t *i2s_base, uint32_t en)
{
    uint32_t val = i2s_base->I2S_FUNCMODE;

    val |= WJ_FUNCMODE_TMODE_WEN;
    val &= ~WJ_FUNCMODE_TMODE;

    if (en) {
        val |= WJ_FUNCMODE_TMODE;
    }

    i2s_base->I2S_FUNCMODE = val;
}

static inline void wj_i2s_set_receive_mode(wj_i2s_multi_regs_t *i2s_base, uint32_t en)
{
    uint32_t val = i2s_base->I2S_FUNCMODE;

    val |= WJ_FUNCMODE_RMODE_WEN;
    val &= ~WJ_FUNCMODE_RMODE;

    if (en) {
        val |= WJ_FUNCMODE_RMODE;
    }

    i2s_base->I2S_FUNCMODE = val;
}

static inline void wj_i2s_set_full_duplex_mode(wj_i2s_multi_regs_t *i2s_base, uint32_t en)
{
    uint32_t val = i2s_base->I2S_FUNCMODE;

    val |= WJ_FUNCMODE_TMODE_WEN;
    val |= WJ_FUNCMODE_RMODE_WEN;
    val &= ~WJ_FUNCMODE_TMODE;
    val &= ~WJ_FUNCMODE_RMODE;

    if (en) {
        val |= WJ_FUNCMODE_TMODE;
        val |= WJ_FUNCMODE_RMODE;
    }

    i2s_base->I2S_FUNCMODE = val;
}

static inline void wj_i2s_set_receive_no_delay(wj_i2s_multi_regs_t *i2s_base, uint32_t delay)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_I2S_RDELAY_Msk;
}

static inline void wj_i2s_set_receive_1_cycle_delay(wj_i2s_multi_regs_t *i2s_base, uint32_t delay)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_I2S_RDELAY_Msk;
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_I2S_RDELAY_1_DELAY;
}

static inline void wj_i2s_set_receive_2_cycle_delay(wj_i2s_multi_regs_t *i2s_base, uint32_t delay)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_I2S_RDELAY_Msk;
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_I2S_RDELAY_2_DELAY;
}

static inline void wj_i2s_set_receive_3_cycle_delay(wj_i2s_multi_regs_t *i2s_base, uint32_t delay)
{
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_I2S_RDELAY_3_DELAY;
}

static inline void wj_i2s_receive_mode_master(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_I2S_RXMODE_MASTER_MODE;
}

static inline void wj_i2s_receive_mode_slave(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_I2S_RXMODE_MASTER_MODE;
}

static inline void wj_i2s_receive_mode_slave_sclk_src_clk(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_I2S_RX_CLK_SEL_MCLK;
}

static inline void wj_i2s_receive_mode_slave_sclk_mclk_i(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_I2S_RX_CLK_SEL_MCLK;
}

static inline void wj_i2s_receive_mode_source_is_single_channel(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_RVOICEEN_MONO;
}

static inline void wj_i2s_receive_mode_source_is_dual_channel(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_RVOICEEN_MONO;
}

static inline void wj_i2s_receive_mode_right_channel(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_RX_CH_SEL_LEFT;
}

static inline void wj_i2s_receive_mode_left_channel(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_RX_CH_SEL_LEFT;
}

static inline void wj_i2s_receive_mode_channel_polarity_low_for_left(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_RALOLRC_HIGHFORLEFT;
}

static inline void wj_i2s_receive_mode_channel_polarity_high_for_left(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_RALOLRC_HIGHFORLEFT;
}

static inline void wj_i2s_receive_mode_format_i2s(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_RSAFS_Msk;
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_RSAFS_I2S;
}

static inline void wj_i2s_receive_mode_format_right_justified(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_RSAFS_Msk;
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_RSAFS_RIGHT_JUSTIFIED;
}

static inline void wj_i2s_receive_mode_format_left_justified(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_RSAFS_Msk;
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_RSAFS_LEFT_JUSTIFIED;
}

static inline void wj_i2s_receive_mode_format_pcm(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_RSAFS_Msk;
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_RSAFS_PCM;
}

static inline void wj_i2s_receive_mode_mclk_sel_256_fs(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_MCLK_SEL_384;
}

static inline void wj_i2s_receive_mode_mclk_sel_384_fs(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA |= WJ_FSSTA_MCLK_SEL_384;
}

static inline void wj_i2s_receive_mode_sclk_sel_32_fs(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_SCLK_SEL_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_SCLK_SEL_32;
}

static inline void wj_i2s_receive_mode_sclk_sel_48_fs(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_SCLK_SEL_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_SCLK_SEL_48;
}

static inline void wj_i2s_receive_mode_sclk_sel_64_fs(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_SCLK_SEL_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_SCLK_SEL_64;
}

static inline void wj_i2s_receive_mode_sclk_sel_16_fs(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_SCLK_SEL_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_SCLK_SEL_16;
}

static inline void wj_i2s_data_width_mode(wj_i2s_multi_regs_t *i2s_base, uint32_t mode)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_DATAWTH_Msk;
    mode &= 0xf;
    i2s_base->I2S_FSSTA |= mode << WJ_FSSTA_DATAWTH_Pos;
}

static inline void wj_i2s_receive_mode_audio_rate_scale_factor_1(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_ARS_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_ARS_1;
}

static inline void wj_i2s_receive_mode_audio_rate_scale_factor_0_5(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_ARS_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_ARS_0_5;
}

static inline void wj_i2s_receive_mode_audio_rate_scale_factor_0_25(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_ARS_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_ARS_0_25;
}

static inline void wj_i2s_receive_mode_audio_rate_scale_factor_0_125(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_ARS_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_ARS_0_125;
}

static inline void wj_i2s_receive_mode_audio_fundamental_rate_88_2k(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_AFR_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_AFR_88_2KSPS;
}

static inline void wj_i2s_receive_mode_audio_fundamental_rate_96k(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_AFR_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_AFR_96KSPS;
}

static inline void wj_i2s_receive_mode_audio_fundamental_rate_64k(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_AFR_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_AFR_64KSPS;
}

static inline void wj_i2s_receive_mode_audio_fundamental_rate_192k(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_AFR_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_AFR_192KSPS;
}

static inline void wj_i2s_receive_mode_audio_input_rate_no_detected(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_AIRAD;
}

static inline void wj_i2s_receive_mode_audio_input_rate_detected_hardware(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA |= WJ_FSSTA_AIRAD;
}

static inline void wj_i2s_transmit_mode_source_is_single_channel(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_TX_VOICE_EN_MONO;
}

static inline void wj_i2s_transmit_mode_source_is_dual_channel(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_TX_VOICE_EN_MONO;
}

static inline void wj_i2s_transmit_mode_channel_polarity_low_for_left(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_TALOLRC_HIGHFORLEFT;
}

static inline void wj_i2s_transmit_mode_channel_polarity_high_for_left(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_TALOLRC_HIGHFORLEFT;
}

static inline void wj_i2s_transmit_mode_format_i2s(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_TSAFS_Msk;
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_TSAFS_I2S;
}

static inline void wj_i2s_transmit_mode_format_right_justified(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_TSAFS_Msk;
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_TSAFS_RIGHT_JUSTIFIED;
}

static inline void wj_i2s_transmit_mode_format_left_justified(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_TSAFS_Msk;
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_TSAFS_LEFT_JUSTIFIED;
}

static inline void wj_i2s_transmit_mode_format_pcm(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_TSAFS_Msk;
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_TSAFS_PCM;
}

static inline void wj_i2s_transmit_mode_master(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_I2S_TXMODE_SLAVE;
}

static inline void wj_i2s_transmit_mode_slave(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_I2S_TXMODE_SLAVE;
}

static inline void wj_i2s_transmit_mode_slave_sclk_src_clk(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_TX_CLK_SEL_MCLK;
}

static inline void wj_i2s_transmit_mode_slave_sclk_mclk_i(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_TX_CLK_SEL_MCLK;
}

static inline void wj_i2s_96k_fs_threshold(wj_i2s_multi_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_FADTLR |= level << WJ_FADTLR_96FTR_Pos;
}

static inline void wj_i2s_88_2k_fs_threshold(wj_i2s_multi_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_FADTLR |= level << WJ_FADTLR_88FTR_Pos;
}

static inline void wj_i2s_64k_fs_threshold(wj_i2s_multi_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_FADTLR |= level << WJ_FADTLR_64FTR_Pos;
}

static inline void wj_i2s_192k_fs_threshold(wj_i2s_multi_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_FADTLR |= level << WJ_FADTLR_192FTR_Pos;
}

static inline void wj_i2s_tx_volume_compress_bits(wj_i2s_multi_regs_t *i2s_base, uint32_t shift)
{
    i2s_base->I2S_SCCR &= ~WJ_SCCR_TVCCR_Msk;
    i2s_base->I2S_SCCR |= shift << WJ_SCCR_TVCCR_Pos;
}

static inline void wj_i2s_rx_volume_compress_bits(wj_i2s_multi_regs_t *i2s_base, uint32_t shift)
{
    i2s_base->I2S_SCCR &= ~WJ_SCCR_RVCCR_Msk;
    i2s_base->I2S_SCCR |= shift << WJ_SCCR_RVCCR_Pos;
}

static inline void wj_i2s_sub_sample_rate_compress_no_compress(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_SCCR &= ~WJ_SCCR_SSRCR_Msk;
    i2s_base->I2S_SCCR |= WJ_SCCR_SSRCR_NO_COMPRESS;
}

static inline void wj_i2s_sub_sample_rate_compress_one_time(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_SCCR &= ~WJ_SCCR_SSRCR_Msk;
    i2s_base->I2S_SCCR |= WJ_SCCR_SSRCR_ONE_COMPRESS;
}

static inline void wj_i2s_sub_sample_rate_compress_three_time(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_SCCR &= ~WJ_SCCR_SSRCR_Msk;
    i2s_base->I2S_SCCR |= WJ_SCCR_SSRCR_THREE_COMPRESS;
}

static inline void wj_i2s_set_transmit_fifo_level(wj_i2s_multi_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_TXFTLR &= ~WJ_TXFTLR_TFT_Msk;
    i2s_base->I2S_TXFTLR = level << WJ_TXFTLR_TFT_Pos;
}

static inline void wj_i2s_set_receive_fifo_level(wj_i2s_multi_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_RXFTLR &= ~WJ_RXFTLR_RFT_Msk;
    i2s_base->I2S_RXFTLR = level << WJ_RXFTLR_RFT_Pos;
}

static inline uint32_t wj_i2s_get_transmit_fifo_level(wj_i2s_multi_regs_t *i2s_base, uint32_t ch)
{
    return ((i2s_base->I2S_TXFLR >> (6 * ch)) & 0x3F);
}

static inline uint32_t wj_i2s_get_receive_fifo_level(wj_i2s_multi_regs_t *i2s_base, uint32_t ch)
{
    return ((i2s_base->I2S_RXFLR >> (6 * ch)) & 0x3F);
}

static inline uint32_t wj_i2s_get_i2s_status(wj_i2s_multi_regs_t *i2s_base, uint32_t ch)
{
    uint32_t i2s_status;
    i2s_status = (((i2s_base->I2S_SR) >> (6 * ch)) & 0x3F);
    return i2s_status;
}

static inline uint32_t wj_i2s_get_i2s_irq_status(wj_i2s_multi_regs_t *i2s_base, uint32_t ch)
{
    uint32_t irq_status;
    irq_status = ((i2s_base->I2S_ISR) >> (7 * ch)) & 0x7FU;
    return irq_status;
}

static inline uint32_t wj_i2s_get_i2s_irq_raw_status(wj_i2s_multi_regs_t *i2s_base, uint32_t ch)
{
    uint32_t raw_irq;
    raw_irq = ((i2s_base->I2S_RISR) >> (7 * ch)) & 0x7FU;
    return raw_irq;
}

static inline void wj_i2s_set_irq(wj_i2s_multi_regs_t *i2s_base, uint32_t ch, uint32_t val)
{
    i2s_base->I2S_IMR = (val & 0x7FU) << (7 * ch);
}

static inline void wj_i2s_clear_irq(wj_i2s_multi_regs_t *i2s_base, uint32_t ch, uint32_t val)
{
    i2s_base->I2S_ICR = (val & 0x7FU) << (7 * ch);
}

static inline void wj_i2s_set_receive_dma_enable(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_DMACR |= WJ_DMACR_RDMAE_EN;
}

static inline void wj_i2s_set_receive_dma_disable(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_DMACR &= ~WJ_DMACR_RDMAE_EN;
}

static inline void wj_i2s_set_transmit_dma_enable(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_DMACR |= WJ_DMACR_TDMAE_EN;
}

static inline void wj_i2s_set_transmit_dma_disable(wj_i2s_multi_regs_t *i2s_base)
{
    i2s_base->I2S_DMACR &= ~WJ_DMACR_TDMAE_EN;
}

static inline void wj_i2s_set_transmit_dma_data_num_level(wj_i2s_multi_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_DMATDLR = level;
}

static inline void wj_i2s_set_receive_dma_data_num_level(wj_i2s_multi_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_DMARDLR = level;
}

static inline void wj_i2s_transmit_data(wj_i2s_multi_regs_t *i2s_base, uint32_t data)
{
    i2s_base->I2S_DR = data;
}

static inline uint32_t wj_i2s_receive_data(wj_i2s_multi_regs_t *i2s_base)
{
    return (uint32_t)(i2s_base->I2S_DR);
}

static inline void wj_i2s_set_div_sclk(wj_i2s_multi_regs_t *i2s_base, uint32_t div)
{
    i2s_base->I2S_DIV0_LEVEL = div;
}

static inline void wj_i2s_set_div_ref_clk(wj_i2s_multi_regs_t *i2s_base, uint32_t div)
{
    i2s_base->I2S_DIV3_LEVEL = div;
}

#ifdef __cplusplus
}
#endif

#endif /* _WJ_MULTI_I2S_LL_H_ */
