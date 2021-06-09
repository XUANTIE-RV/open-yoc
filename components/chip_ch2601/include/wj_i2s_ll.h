/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_i2s_ll.h
 * @brief    header file for wj i2s v1 ll driver
 * @version  V1.0
 * @date     21. Feb 2020
 ******************************************************************************/

#ifndef _WJ_IIS_LL_H_
#define _WJ_IIS_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * I2S register bit definitions
 */

/* AUDIOEN , offset: 0x00 */
#define WJ_AUDIOEN_Pos                             (0U)
#define WJ_AUDIOEN_Msk                             (0x3U << WJ_AUDIOEN_Pos)
#define WJ_AUDIOEN_IISEN_EN                        (0x1U << WJ_AUDIOEN_Pos)
#define WJ_AUDIOEN_SPDIFEN_EN                      (0x2U << WJ_AUDIOEN_Pos)

/* FUNCMODE, offset: 0x04 */
#define WJ_FUNCMODE_MODE_Pos                       (0U)
#define WJ_FUNCMODE_MODE_Msk                       (0x1U << WJ_FUNCMODE_MODE_Pos)
#define WJ_FUNCMODE_MODE_TX                        WJ_FUNCMODE_MODE_Msk

#define WJ_FUNCMODE_MODEWEN_Pos                    (1U)
#define WJ_FUNCMODE_MODEWEN_Msk                    (0x1U << WJ_FUNCMODE_MODEWEN_Pos)
#define WJ_FUNCMODE_MODEWEN                        WJ_FUNCMODE_MODEWEN_Msk

/* IISCNFIN, offset: 0x08 */
#define WJ_IISCNFIN_RSAFS_Pos                      (0U)
#define WJ_IISCNFIN_RSAFS_Msk                      (0x3U << WJ_IISCNFIN_RSAFS_Pos)
#define WJ_IISCNFIN_RSAFS_I2S                      (0x0U << WJ_IISCNFIN_RSAFS_Pos)
#define WJ_IISCNFIN_RSAFS_RIGHT_JUSTIFIED          (0x1U << WJ_IISCNFIN_RSAFS_Pos)
#define WJ_IISCNFIN_RSAFS_LEFT_JUSTIFIED           (0x2U << WJ_IISCNFIN_RSAFS_Pos)

#define WJ_IISCNFIN_RALOLRC_Pos                    (2U)
#define WJ_IISCNFIN_RALOLRC_Msk                    (0x1U << WJ_IISCNFIN_RALOLRC_Pos)
#define WJ_IISCNFIN_RALOLRC_HIGHFORLEFT            WJ_IISCNFIN_RALOLRC_Msk

#define WJ_IISCNFIN_RVOICEEN_Pos                   (4U)
#define WJ_IISCNFIN_RVOICEEN_Msk                   (0x1U << WJ_IISCNFIN_RVOICEEN_Pos)
#define WJ_IISCNFIN_RVOICEEN_VOICE                 WJ_IISCNFIN_RVOICEEN_Msk

#define WJ_IISCNFIN_ADEN_Pos                       (8U)
#define WJ_IISCNFIN_ADEN_Msk                       (0x1U << WJ_IISCNFIN_ADEN_Pos)
#define WJ_IISCNFIN_ADEN_MASTER_MODE               WJ_IISCNFIN_ADEN_Msk

/* FSSTA , offset: 0x0C */
#define WJ_FSSTA_AIRAD_Pos                         (0U)
#define WJ_FSSTA_AIRAD_Msk                         (0x1U << WJ_FSSTA_AIRAD_Pos)
#define WJ_FSSTA_AIRAD                             WJ_FSSTA_AIRAD_Msk

#define WJ_FSSTA_I2SMODE_Pos                       (1U)
#define WJ_FSSTA_I2SMODE_Msk                       (0x3U << WJ_FSSTA_I2SMODE_Pos)
#define WJ_FSSTA_I2SMODE_16I16S                    (0x0U << WJ_FSSTA_I2SMODE_Pos)
#define WJ_FSSTA_I2SMODE_16I24S                    (0x1U << WJ_FSSTA_I2SMODE_Pos)
#define WJ_FSSTA_I2SMODE_24I16S                    (0x2U << WJ_FSSTA_I2SMODE_Pos)
#define WJ_FSSTA_I2SMODE_24I24S                    (0x3U << WJ_FSSTA_I2SMODE_Pos)

#define WJ_FSSTA_AFR_Pos                           (4U)
#define WJ_FSSTA_AFR_Msk                           (0x3U << WJ_FSSTA_AFR_Pos)
#define WJ_FSSTA_AFR_44_1KSPS                      (0x0U << WJ_FSSTA_AFR_Pos)
#define WJ_FSSTA_AFR_48KSPS                        (0x1U << WJ_FSSTA_AFR_Pos)
#define WJ_FSSTA_AFR_32KSPS                        (0x2U << WJ_FSSTA_AFR_Pos)
#define WJ_FSSTA_AFR_96KSPS                        (0x3U << WJ_FSSTA_AFR_Pos)

#define WJ_FSSTA_ARS_Pos                           (6U)
#define WJ_FSSTA_ARS_Msk                           (0x3U << WJ_FSSTA_ARS_Pos)
#define WJ_FSSTA_ARS_1                             (0x0U << WJ_FSSTA_ARS_Pos)
#define WJ_FSSTA_ARS_0_5                           (0x1U << WJ_FSSTA_ARS_Pos)
#define WJ_FSSTA_ARS_0_25                          (0x2U << WJ_FSSTA_ARS_Pos)


/* IISCNFOUT, offset: 0x10 */
#define WJ_IISCNFOUT_TSAFS_Pos                     (0U)
#define WJ_IISCNFOUT_TSAFS_Msk                     (0x3U << WJ_IISCNFOUT_TSAFS_Pos)
#define WJ_IISCNFOUT_TSAFS_I2S                     (0x0U << WJ_IISCNFOUT_TSAFS_Pos)
#define WJ_IISCNFOUT_TSAFS_RIGHT_JUSTIFIED         (0x1U << WJ_IISCNFOUT_TSAFS_Pos)
#define WJ_IISCNFOUT_TSAFS_LEFT_JUSTIFIED          (0x2U << WJ_IISCNFOUT_TSAFS_Pos)

#define WJ_IISCNFOUT_TALOLRC_Pos                   (2U)
#define WJ_IISCNFOUT_TALOLRC_Msk                   (0x1U << WJ_IISCNFOUT_TALOLRC_Pos)
#define WJ_IISCNFOUT_TALOLRC_HIGHFORLEFT           WJ_IISCNFOUT_TALOLRC_Msk

#define WJ_IISCNFOUT_TVOICEEN_Pos                  (3U)
#define WJ_IISCNFOUT_TVOICEEN_Msk                  (0x1U << WJ_IISCNFOUT_TVOICEEN_Pos)
#define WJ_IISCNFOUT_TVOICEEN_VOICE                WJ_IISCNFOUT_TVOICEEN_Msk

#define WJ_IISCNFOUT_TWMS_Pos                      (4U)
#define WJ_IISCNFOUT_TWMS_Msk                      (0x1U << WJ_IISCNFOUT_TWMS_Pos)
#define WJ_IISCNFOUT_TWMS_SLAVE                    WJ_IISCNFOUT_TWMS_Msk

/* FADTLR, offset: 0x14 */
#define WJ_FADTLR_48FTR_Pos                        (0U)
#define WJ_FADTLR_48FTR_Msk                        (0x7FU << WJ_FADTLR_48FTR_Pos)

#define WJ_FADTLR_44FTR_Pos                        (8U)
#define WJ_FADTLR_44FTR_Msk                        (0x7FU << WJ_FADTLR_44FTR_Pos)

#define WJ_FADTLR_32FTR_Pos                        (16U)
#define WJ_FADTLR_32FTR_Msk                        (0x7FU << WJ_FADTLR_32FTR_Pos)

#define WJ_FADTLR_96FTR_Pos                        (24U)
#define WJ_FADTLR_96FTR_Msk                        (0x7FU << WJ_FADTLR_96FTR_Pos)

/* SCCR, offset: 0x18 */
#define WJ_SCCR_VCCR_Pos                           (0U)
#define WJ_SCCR_VCCR_Msk                           (0x1FU << WJ_SCCR_VCCR_Pos)

#define WJ_SCCR_SLCCR_Pos                          (8U)
#define WJ_SCCR_SLCCR_Msk                          (0xFU << WJ_SCCR_SLCCR_Pos)

#define WJ_SCCR_SSRCR_Pos                          (12U)
#define WJ_SCCR_SSRCR_Msk                          (0x3U << WJ_SCCR_SSRCR_Pos)
#define WJ_SCCR_SSRCR_NO_COMPRESS                  (0x0U << WJ_SCCR_SSRCR_Pos)
#define WJ_SCCR_SSRCR_ONE_COMPRESS                 (0x1U << WJ_SCCR_SSRCR_Pos)
#define WJ_SCCR_SSRCR_THREE_COMPRESS               (0x2U << WJ_SCCR_SSRCR_Pos)

#define WJ_SCCR_TVCCR_Pos                          (16U)
#define WJ_SCCR_TVCCR_Msk                          (0x1FU << WJ_SCCR_TVCCR_Pos)
/* TXFTLR, offset: 0x1C */
#define WJ_TXFTLR_TFT_Pos                          (0U)
#define WJ_TXFTLR_TFT_Msk                          (0x1FU << WJ_TXFTLR_TFT_Pos)

/* RXFTLR, offset: 0x20 */
#define WJ_RXFTLR_RFT_Pos                          (0U)
#define WJ_RXFTLR_RFT_Msk                          (0x1FU << WJ_RXFTLR_RFT_Pos)

/* TXFLR, offset: 0x24 */
#define WJ_TXFLR_TXTFL_Pos                         (0U)
#define WJ_TXFLR_TXTFL_Msk                         (0x1FU << WJ_TXFLR_TXTFL_Pos)

/* RXFLR, offset: 0x28 */
#define WJ_RXFLR_RXTFL_Pos                         (0U)
#define WJ_RXFLR_RXTFL_Msk                         (0x1FU << WJ_RXFLR_RXTFL_Pos)

/* SR, offset: 0x2C */
#define WJ_SR_RXBUSY_Pos                           (0U)
#define WJ_SR_RXBUSY_Msk                           (0x1U << WJ_SR_RXBUSY_Pos)
#define WJ_SR_RXBUSY_RX_RECEIVING                  WJ_SR_RXBUSY_Msk

#define WJ_SR_TXBUSY_Pos                           (1U)
#define WJ_SR_TXBUSY_Msk                           (0x1U << WJ_SR_TXBUSY_Pos)
#define WJ_SR_TXBUSY_TX_TRANSMITTING               WJ_SR_TXBUSY_Msk

#define WJ_SR_TFNF_Pos                             (2U)
#define WJ_SR_TFNF_Msk                             (0x1U << WJ_SR_TFNF_Pos)
#define WJ_SR_TFNF_TX_FIFO_NOT_FULL                WJ_SR_TFNF_Msk

#define WJ_SR_TFE_Pos                              (3U)
#define WJ_SR_TFE_Msk                              (0x1U << WJ_SR_TFE_Pos)
#define WJ_SR_TFE_TX_FIFO_EMPTY                    WJ_SR_TFE_Msk

#define WJ_SR_RFNE_Pos                             (4U)
#define WJ_SR_RFNE_Msk                             (0x1U << WJ_SR_RFNE_Pos)
#define WJ_SR_RFNE_RX_FIFO_NOT_EMPTY                WJ_SR_RFNE_Msk

#define WJ_SR_RFF_Pos                              (5U)
#define WJ_SR_RFF_Msk                              (0x1U << WJ_SR_RFF_Pos)
#define WJ_SR_RFF_RX_FIFO_FULL                      WJ_SR_RFF_Msk

/* IMR, offset: 0x30 */
#define WJ_IMR_TXEIM_Pos                            (0U)
#define WJ_IMR_TXEIM_Msk                            (0x1U << WJ_IMR_TXEIM_Pos)
#define WJ_IMR_TX_FIFO_EMPTY_INTR_MSK               WJ_IMR_TXEIM_Msk

#define WJ_IMR_TXOIM_Pos                            (1U)
#define WJ_IMR_TXOIM_Msk                            (0x1U << WJ_IMR_TXOIM_Pos)
#define WJ_IMR_TX_FIFO_OVERFLOW_INTR_MSK            WJ_IMR_TXOIM_Msk

#define WJ_IMR_RXUIM_Pos                            (2U)
#define WJ_IMR_RXUIM_Msk                            (0x1U << WJ_IMR_RXUIM_Pos)
#define WJ_IMR_RX_FIFO_UNDERFLOW_INTR_MSK           WJ_IMR_RXUIM_Msk

#define WJ_IMR_RXOIM_Pos                            (3U)
#define WJ_IMR_RXOIM_Msk                            (0x1U << WJ_IMR_RXOIM_Pos)
#define WJ_IMR_RX_FIFO_OVERFLOW_INTR_MSK            WJ_IMR_RXOIM_Msk

#define WJ_IMR_RXFIM_Pos                            (4U)
#define WJ_IMR_RXFIM_Msk                            (0x1U << WJ_IMR_RXFIM_Pos)
#define WJ_IMR_RX_FIFO_FULL_INTR_MSK                WJ_IMR_RXFIM_Msk

/* ISR, offset: 0x34 */
#define WJ_ISR_TXEIS_Pos                            (0U)
#define WJ_ISR_TXEIS_Msk                            (0x1U << WJ_ISR_TXEIS_Pos)
#define WJ_ISR_TXEIS_INTR_STATUS                    WJ_ISR_TXEIS_Msk

#define WJ_ISR_TXOIS_Pos                            (1U)
#define WJ_ISR_TXOIS_Msk                            (0x1U << WJ_ISR_TXOIS_Pos)
#define WJ_ISR_TXOIS_INTR_STATUS                    WJ_ISR_TXOIS_Msk

#define WJ_ISR_RXUIS_Pos                            (2U)
#define WJ_ISR_RXUIS_Msk                            (0x1U << WJ_ISR_RXUIS_Pos)
#define WJ_ISR_RXUIS_INTR_STATUS                    WJ_ISR_RXUIS_Msk

#define WJ_ISR_RXOIS_Pos                            (3U)
#define WJ_ISR_RXOIS_Msk                            (0x1U << WJ_ISR_RXOIS_Pos)
#define WJ_ISR_RXOIS_INTR_STATUS                    WJ_ISR_RXOIS_Msk

#define WJ_ISR_RXFIS_Pos                            (4U)
#define WJ_ISR_RXFIS_Msk                            (0x1U << WJ_ISR_RXFIS_Pos)
#define WJ_ISR_RXFIS_INTR_STATUS                    WJ_ISR_RXFIS_Msk

/* RISR, offset: 0x38 */
#define WJ_RISR_TXEIR_Pos                           (0U)
#define WJ_RISR_TXEIR_Msk                           (0x1U << WJ_RISR_TXEIR_Pos)
#define WJ_RISR_TXEIR_RAW                           WJ_RISR_TXEIR_Msk

#define WJ_RISR_TXOIR_Pos                           (1U)
#define WJ_RISR_TXOIR_Msk                           (0x1U << WJ_RISR_TXOIR_Pos)
#define WJ_RISR_TXOIR_RAW                           WJ_RISR_TXOIR_Msk

#define WJ_RISR_RXUIR_Pos                           (2U)
#define WJ_RISR_RXUIR_Msk                           (0x1U << WJ_RISR_RXUIR_Pos)
#define WJ_RISR_RXUIR_RAW                           WJ_RISR_RXUIR_Msk

#define WJ_RISR_RXOIR_Pos                           (3U)
#define WJ_RISR_RXOIR_Msk                           (0x1U << WJ_RISR_RXOIR_Pos)
#define WJ_RISR_RXOIR_RAW                           WJ_RISR_RXOIR_Msk

#define WJ_RISR_RXFIR_Pos                           (4U)
#define WJ_RISR_RXFIR_Msk                           (0x1U << WJ_RISR_RXFIR_Pos)
#define WJ_RISR_RXFIR_RAW                           WJ_RISR_RXFIR_Msk

/* FICR, offset: 0x3C */
#define WJ_RISR_TXEIC_Pos                           (0U)
#define WJ_RISR_TXEIC_Msk                           (0x1U << WJ_RISR_TXEIC_Pos)
#define WJ_RISR_TXEIC_CLEAR                         WJ_RISR_TXEIC_Msk

#define WJ_RISR_TXOIC_Pos                           (1U)
#define WJ_RISR_TXOIC_Msk                           (0x1U << WJ_RISR_TXOIC_Pos)
#define WJ_RISR_TXOIC_CLEAR                         WJ_RISR_TXOIC_Msk

#define WJ_RISR_RXUIC_Pos                           (2U)
#define WJ_RISR_RXUIC_Msk                           (0x1U << WJ_RISR_RXUIC_Pos)
#define WJ_RISR_RXUIC_CLEAR                         WJ_RISR_RXUIC_Msk

#define WJ_RISR_RXOIC_Pos                           (3U)
#define WJ_RISR_RXOIC_Msk                           (0x1U << WJ_RISR_RXOIC_Pos)
#define WJ_RISR_RXOIC_CLEAR                         WJ_RISR_RXOIC_Msk

#define WJ_RISR_RXFIC_Pos                           (4U)
#define WJ_RISR_RXFIC_Msk                           (0x1U << WJ_RISR_RXFIC_Pos)
#define WJ_RISR_RXFIC_CLEAR                         WJ_RISR_RXFIC_Msk

/* DMACR, offset: 0x4C */
#define WJ_DMACR_RDMAE_Pos                          (0U)
#define WJ_DMACR_RDMAE_Msk                          (0x1U << WJ_DMACR_RDMAE_Pos)
#define WJ_DMACR_RDMAE_EN                           WJ_DMACR_RDMAE_Msk

#define WJ_DMACR_TDMAE_Pos                          (1U)
#define WJ_DMACR_TDMAE_Msk                          (0x1U << WJ_DMACR_TDMAE_Pos)
#define WJ_DMACR_TDMAE_EN                           WJ_DMACR_TDMAE_Msk

/* DMATDLR, offset: 0x50 */
#define WJ_DMATDLR_DMATDL_Pos                       (0U)
#define WJ_DMATDLR_DMATDL_Msk                       (0xFU << WJ_DMATDLR_DMATDL_Pos)

/* DMARDLR, offset: 0x54 */
#define WJ_DMARDLR_DMATDL_Pos                       (0U)
#define WJ_DMARDLR_DMATDL_Msk                       (0xFU << WJ_DMARDLR_DMATDL_Pos)

/* DR, offset: 0x60 */
#define WJ_DR_DRR_Pos                               (0U)

#define WJ_DR_DRL_Pos                               (16U)

/* SRCR, offset: 0x70 */
#define WJ_SRCR_SPBV_Pos                            (0U)
#define WJ_SRCR_SPBV_Msk                            (0x1U << WJ_SRCR_SPBV_Pos)
#define WJ_SRCR_SPBV                                WJ_SRCR_SPBV_Msk

#define WJ_SRCR_SVBV_Pos                            (1U)
#define WJ_SRCR_SVBV_Msk                            (0x1U << WJ_SRCR_SVBV_Pos)
#define WJ_SRCR_SVBV                                WJ_SRCR_SVBV_Msk

/* SRSSR, offset: 0x74 */
#define WJ_SRSSR_SPBF_Pos                           (0U)
#define WJ_SRSSR_SPBF_Msk                           (0x1U << WJ_SRSSR_SPBF_Pos)
#define WJ_SRSSR_SPBF                               WJ_SRSSR_SPBF_Msk

#define WJ_SRSSR_SPRO_Pos                           (1U)
#define WJ_SRSSR_SPRO_Msk                           (0x1U << WJ_SRSSR_SPRO_Pos)
#define WJ_SRSSR_SPRO                               WJ_SRSSR_SPRO_Msk

#define WJ_SRSSR_SAUDIO_Pos                         (2U)
#define WJ_SRSSR_SAUDIO_Msk                         (0x1U << WJ_SRSSR_SAUDIO_Pos)
#define WJ_SRSSR_SAUDIO                             WJ_SRSSR_SAUDIO_Msk

#define WJ_SRSSR_SCOPY_Pos                          (3U)
#define WJ_SRSSR_SCOPY_Msk                          (0x1U << WJ_SRSSR_SCOPY_Pos)
#define WJ_SRSSR_SCOPY                              WJ_SRSSR_SCOPY_Msk

#define WJ_SRSSR_SEMP_Pos                           (4U)
#define WJ_SRSSR_SEMP_Msk                           (0x1U << WJ_SRSSR_SEMP_Pos)
#define WJ_SRSSR_SEMP                               WJ_SRSSR_SEMP_Msk

#define WJ_SRSSR_SFS_Pos                            (5U)
#define WJ_SRSSR_SFS_Msk                            (0x3U << WJ_SRSSR_SFS_Pos)
#define WJ_SRSSR_SFS_44_1KHZ                        (0x0U << WJ_SRSSR_SFS_Pos)
#define WJ_SRSSR_SFS_48KHZ                          (0x1U << WJ_SRSSR_SFS_Pos)
#define WJ_SRSSR_SFS_32KHZ                          (0x2U << WJ_SRSSR_SFS_Pos)

/* STSSR, offset: 0x78 */
#define WJ_STSSR_SPBF_Pos                           (0U)
#define WJ_STSSR_SPBF_Msk                           (0x1U << WJ_STSSR_SPBF_Pos)
#define WJ_STSSR_SPBF                               WJ_STSSR_SPBF_Msk

#define WJ_STSSR_SPRO_Pos                           (1U)
#define WJ_STSSR_SPRO_Msk                           (0x1U << WJ_STSSR_SPRO_Pos)
#define WJ_STSSR_SPRO                               WJ_STSSR_SPRO_Msk

#define WJ_STSSR_SAUDIO_Pos                         (2U)
#define WJ_STSSR_SAUDIO_Msk                         (0x1U << WJ_STSSR_SAUDIO_Pos)
#define WJ_STSSR_SAUDIO                             WJ_STSSR_SAUDIO_Msk

#define WJ_STSSR_SCOPY_Pos                          (3U)
#define WJ_STSSR_SCOPY_Msk                          (0x1U << WJ_STSSR_SCOPY_Pos)
#define WJ_STSSR_SCOPY                              WJ_STSSR_SCOPY_Msk

#define WJ_STSSR_SEMP_Pos                           (4U)
#define WJ_STSSR_SEMP_Msk                           (0x1U << WJ_STSSR_SEMP_Pos)
#define WJ_STSSR_SEMP                               WJ_STSSR_SEMP_Msk

#define WJ_STSSR_SFS_Pos                            (5U)
#define WJ_STSSR_SFS_Msk                            (0x3U << WJ_STSSR_SFS_Pos)
#define WJ_STSSR_SFS_44_1KHZ                        (0x0U << WJ_STSSR_SFS_Pos)
#define WJ_STSSR_SFS_48KHZ                          (0x1U << WJ_STSSR_SFS_Pos)
#define WJ_STSSR_SFS_32KHZ                          (0x2U << WJ_STSSR_SFS_Pos)

/* SCSR, offset: 0x7C */
#define WJ_SCSR_SCS_Pos                             (0U)

/* MMR, offset: 0x80 */
#define WJ_MMR_IRBFCM_Pos                           (0U)
#define WJ_MMR_IRBFCM_Msk                           (0x1U << WJ_MMR_IRBFCM_Pos)
#define WJ_MMR_IRBFCM_INTR_MASK                     WJ_MMR_IRBFCM_Msk

#define WJ_MMR_ITBFCM_Pos                           (1U)
#define WJ_MMR_ITBFCM_Msk                           (0x1U << WJ_MMR_ITBFCM_Pos)
#define WJ_MMR_ITBFCM_INTR_MASK                     WJ_MMR_ITBFCM_Msk

#define WJ_MMR_SRBFCM_Pos                           (2U)
#define WJ_MMR_SRBFCM_Msk                           (0x1U << WJ_MMR_SRBFCM_Pos)
#define WJ_MMR_SRBFCM_INTR_MASK                     WJ_MMR_SRBFCM_Msk

#define WJ_MMR_STBFCM_Pos                           (3U)
#define WJ_MMR_STBFCM_Msk                           (0x1U << WJ_MMR_STBFCM_Pos)
#define WJ_MMR_STBFCM_INTR_MASK                     WJ_MMR_STBFCM_Msk

#define WJ_MMR_IFSCM_Pos                            (4U)
#define WJ_MMR_IFSCM_Msk                            (0x1U << WJ_MMR_IFSCM_Pos)
#define WJ_MMR_IFSCM_INTR_MASK                      WJ_MMR_IFSCM_Msk

#define WJ_MMR_SDCM_Pos                             (5U)
#define WJ_MMR_SDCM_Msk                             (0x1U << WJ_MMR_SDCM_Pos)
#define WJ_MMR_SDCM_INTR_MASK                       WJ_MMR_SDCM_Msk

/* MISR, offset: 0x84 */
#define WJ_MISR_IRBFCS_Pos                          (0U)
#define WJ_MISR_IRBFCS_Msk                          (0x1U << WJ_MISR_IRBFCS_Pos)
#define WJ_MISR_IRBFCS_INTR_STATUS                  WJ_MISR_IRBFCS_Msk

#define WJ_MISR_ITBFCS_Pos                          (1U)
#define WJ_MISR_ITBFCS_Msk                          (0x1U << WJ_MISR_ITBFCS_Pos)
#define WJ_MISR_ITBFCS_INTR_STATUS                  WJ_MISR_ITBFCS_Msk

#define WJ_MISR_SRBFCS_Pos                          (2U)
#define WJ_MISR_SRBFCS_Msk                          (0x1U << WJ_MISR_SRBFCS_Pos)
#define WJ_MISR_SRBFCS_INTR_STATUS                  WJ_MISR_SRBFCS_Msk

#define WJ_MISR_STBFCS_Pos                          (3U)
#define WJ_MISR_STBFCS_Msk                          (0x1U << WJ_MISR_STBFCS_Pos)
#define WJ_MISR_STBFCS_INTR_STATUS                  WJ_MISR_STBFCS_Msk

#define WJ_MISR_IFSCS_Pos                           (4U)
#define WJ_MISR_IFSCS_Msk                           (0x1U << WJ_MISR_IFSCS_Pos)
#define WJ_MISR_IFSCS_INTR_STATUS                   WJ_MISR_IFSCS_Msk

#define WJ_MISR_SDCS_Pos                            (5U)
#define WJ_MISR_SDCS_Msk                            (0x1U << WJ_MISR_SDCS_Pos)
#define WJ_MISR_SDCS_INTR_STATUS                    WJ_MISR_SDCS_Msk

/* RMISR, offset: 0x88 */
#define WJ_RMISR_RIRBFCS_Pos                        (0U)
#define WJ_RMISR_RIRBFCS_Msk                        (0x1U << WJ_RMISR_RIRBFCS_Pos)
#define WJ_RMISR_RIRBFCS_RAW                        WJ_RMISR_RIRBFCS_Msk

#define WJ_RMISR_RITBFCS_Pos                        (1U)
#define WJ_RMISR_RITBFCS_Msk                        (0x1U << WJ_RMISR_RITBFCS_Pos)
#define WJ_RMISR_RITBFCS_RAW                        WJ_RMISR_RITBFCS_Msk

#define WJ_RMISR_RSRBFCS_Pos                        (2U)
#define WJ_RMISR_RSRBFCS_Msk                        (0x1U << WJ_RMISR_RSRBFCS_Pos)
#define WJ_RMISR_RSRBFCS_RAW                        WJ_RMISR_RSRBFCS_Msk

#define WJ_RMISR_RSTBFCS_Pos                        (3U)
#define WJ_RMISR_RSTBFCS_Msk                        (0x1U << WJ_RMISR_RSTBFCS_Pos)
#define WJ_RMISR_RSTBFCS_RAW                        WJ_RMISR_RSTBFCS_Msk

#define WJ_RMISR_RIFSCS_Pos                         (4U)
#define WJ_RMISR_RIFSCS_Msk                         (0x1U << WJ_RMISR_RIFSCS_Pos)
#define WJ_RMISR_RIFSCS_RAW                         WJ_RMISR_RIFSCS_Msk

#define WJ_RMISR_RSDCS_Pos                          (5U)
#define WJ_RMISR_RSDCS_Msk                          (0x1U << WJ_RMISR_RSDCS_Pos)
#define WJ_RMISR_RSDCS_RAW                          WJ_RMISR_RSDCS_Msk

/* CMIR, offset: 0x8C */
#define WJ_CMIR_CRIRBFC_Pos                         (0U)
#define WJ_CMIR_CRIRBFC_Msk                         (0x1U << WJ_CMIR_CRIRBFC_Pos)
#define WJ_CMIR_CRIRBFC_CLEAR                       WJ_CMIR_CRIRBFC_Msk

#define WJ_CMIR_CRITBFC_Pos                         (1U)
#define WJ_CMIR_CRITBFC_Msk                         (0x1U << WJ_CMIR_CRITBFC_Pos)
#define WJ_CMIR_CRITBFC_CLEAR                       WJ_CMIR_CRITBFC_Msk

#define WJ_CMIR_CRSRBFC_Pos                         (2U)
#define WJ_CMIR_CRSRBFC_Msk                         (0x1U << WJ_CMIR_CRSRBFC_Pos)
#define WJ_CMIR_CRSRBFC_CLEAR                       WJ_CMIR_CRSRBFC_Msk

#define WJ_CMIR_CRSTBFC_Pos                         (3U)
#define WJ_CMIR_CRSTBFC_Msk                         (0x1U << WJ_CMIR_CRSTBFC_Pos)
#define WJ_CMIR_CRSTBFC_CLEAR                       WJ_CMIR_CRSTBFC_Msk

#define WJ_CMIR_CRIFSC_Pos                          (4U)
#define WJ_CMIR_CRIFSC_Msk                          (0x1U << WJ_CMIR_CRIFSC_Pos)
#define WJ_CMIR_CRIFSC_CLEAR                        WJ_CMIR_CRIFSC_Msk

#define WJ_CMIR_CRSDC_Pos                           (5U)
#define WJ_CMIR_CRSDC_Msk                           (0x1U << WJ_CMIR_CRSDC_Pos)
#define WJ_CMIR_CRSDC_CLEAR                         WJ_CMIR_CRSDC_Msk

/* DIV0LEVEL, offset: 0x90 */
#define WJ_DIV0LEVEL_DIV0_Pos                       (0U)

#define WJ_DIV0LEVEL_DIV0_ODD_Pos                   (8U)
#define WJ_DIV0LEVEL_DIV0_ODD_Msk                   (0x1U << WJ_DIV0LEVEL_DIV0_ODD_Pos)
#define WJ_DIV0LEVEL_DIV0_ODD                       WJ_DIV0LEVEL_DIV0_ODD_Msk

#define WJ_DIV0LEVEL_DIV0_MCLKOE_B_Pos              (9U)
#define WJ_DIV0LEVEL_DIV0_MCLKOE_B_Msk              (0x1U << WJ_DIV0LEVEL_DIV0_MCLKOE_B_Pos)
#define WJ_DIV0LEVEL_DIV0_MCLKOE_B                  WJ_DIV0LEVEL_DIV0_MCLKOE_B_Msk

/* DIV1LEVEL, offset: 0x94 */
#define WJ_DIV1LEVEL_DIV1_Pos                       (0U)

/* DIV2LEVEL, offset: 0x98 */
#define WJ_DIV2LEVEL_DIV2_Pos                       (0U)

/* DIV3LEVEL, offset: 0x9C */
#define WJ_DIV3LEVEL_DIV3_Pos                       (0U)

#define TXFIFO_IRQ_TH                               (0x8U)
#define RXFIFO_IRQ_TH                               (0x20U)
#define I2S_MAX_FIFO                                (0x20U)

typedef struct {
    __IOM uint32_t I2S_AUDIOEN;          /* Offset: 0x000h (R/W)  AUDIO_IO Enable Register */
    __IOM uint32_t I2S_FUNCMODE;         /* Offset: 0x004h (R/W)  AUDIO_IO function mode */
    __IOM uint32_t I2S_IISCNF_IN;        /* Offset: 0x008h (R/W)  IIS interface configuration in (on ARX side) */
    __IOM uint32_t I2S_FSSTA;            /* Offset: 0x00ch (R/W)  IIS ATX audio input control/state register */
    __IOM uint32_t I2S_IISCNF_OUT;       /* Offset: 0x010h (R/W)  IIS interface configuration in (on ATX side) */
    __IOM uint32_t I2S_FADTLR;           /* Offset: 0x014h (R/W)  IIS Fs auto detected Threshold level register */
    __IOM uint32_t I2S_SCCR;             /* Offset: 0x018h (R/W)  Sample compress control register */
    __IOM uint32_t I2S_TXFTLR;           /* Offset: 0x01ch (R/W)  Transmit FIFO Threshold Level */
    __IOM uint32_t I2S_RXFTLR;           /* Offset: 0x020h (R/W)  Receive FIFO Threshold Level */
    __IOM uint32_t I2S_TXFLR;            /* Offset: 0x024h (R/W)  Transmit FIFO Level Register */
    __IOM uint32_t I2S_RXFLR;            /* Offset: 0x028h (R/W)  Receive FIFO Level Register */
    __IOM uint32_t I2S_SR;               /* Offset: 0x02ch (R/W)  Status Register */
    __IOM uint32_t I2S_IMR;              /* Offset: 0x030h (R/W)  Interrupt Mask Register */
    __IOM uint32_t I2S_ISR;              /* Offset: 0x034h (R/W)  Interrupt Status Register */
    __IOM uint32_t I2S_RISR;             /* Offset: 0x038h (R/W)  Raw Interrupt Status Register */
    __IOM uint32_t I2S_ICR;              /* Offset: 0x03ch (R/W)  FIFO Interrupt Clear Register */
    uint32_t RESERVED1[3];
    __IOM uint32_t I2S_DMACR;            /* Offset: 0x04ch (R/W)  DMA Control Register */
    __IOM uint32_t I2S_DMATDLR;          /* Offset: 0x050h (R/W)  DMA Transmit Data Level */
    __IOM uint32_t I2S_DMARDLR;          /* Offset: 0x054h (R/W)  DMA Receive Data Level */
    uint32_t RESERVED2[2];
    __IOM uint32_t I2S_DR;               /* Offset: 0x060h (R/W)  Data Register */
    uint32_t RESERVED3[3];
    __IOM uint32_t I2S_SRCR;             /* Offset: 0x070h (R/W)  SPDIF Receiver Configuration register */
    __IOM uint32_t I2S_SRSSR;            /* Offset: 0x074h (R/W)  SPDIF Receiver Signal Status Register */
    __IOM uint32_t I2S_STSSR;            /* Offset: 0x078h (R/W)  SPDIF Transmitter Signal Status Register */
    __IOM uint32_t I2S_SCSR;             /* Offset: 0x07ch (R/W)  SPDIF Channel status Bits Register */
    __IOM uint32_t I2S_MIMR;             /* Offset: 0x080h (R/W)  Mode Interrupt Mask Register */
    __IOM uint32_t I2S_MISR;             /* Offset: 0x084h (R/W)  Raw Mode Interrupt Status Register */
    __IOM uint32_t I2S_RMISR;            /* Offset: 0x088h (R/W)  Mode Interrupt Status Register */
    __IOM uint32_t I2S_CMIR;             /* Offset: 0x08ch (R/W)  Clear Mode Interrupt Register */
    __IOM uint32_t I2S_DIV0_LEVEL;       /* Offset: 0x090h (R/W)  Divide i2s_clkgen source clock, get mclk or sclk */
    __IOM uint32_t I2S_DIV1_LEVEL;       /* Offset: 0x094h (R/W)  Divide i2s_clkgen source clock, get spdif_clk */
    __IOM uint32_t I2S_DIV2_LEVEL;       /* Offset: 0x098h (R/W)  Divide i2s_clkgen sclk, get wsclk */
    __IOM uint32_t I2S_DIV3_LEVEL;       /* Offset: 0x09ch (R/W)  Divide i2s_clkgen source clock, get reference clock */
} wj_i2s_regs_t;

static inline uint8_t wj_i2s_get_enable_state(wj_i2s_regs_t *i2s_base)
{
    return (uint8_t)(i2s_base->I2S_AUDIOEN & WJ_AUDIOEN_IISEN_EN);
}

static inline void wj_i2s_enable(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_AUDIOEN |= WJ_AUDIOEN_IISEN_EN;
}

static inline void WJ_spdif_enable(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_AUDIOEN |= WJ_AUDIOEN_SPDIFEN_EN;
}

static inline void wj_i2s_disable(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_AUDIOEN &= ~WJ_AUDIOEN_Msk;
}

static inline void WJ_spdif_disable(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_AUDIOEN &= ~WJ_AUDIOEN_Msk;
}

static inline void wj_i2s_set_transmit_mode(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FUNCMODE |= WJ_FUNCMODE_MODE_TX | WJ_FUNCMODE_MODEWEN;
}

static inline void wj_i2s_set_receive_mode(wj_i2s_regs_t *i2s_base)
{
    uint32_t val = i2s_base->I2S_FUNCMODE;

    val |= WJ_FUNCMODE_MODEWEN;
    val &= ~WJ_FUNCMODE_MODE_TX;

    i2s_base->I2S_FUNCMODE = val;
}

static inline void wj_i2s_receive_mode_master(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_ADEN_MASTER_MODE;
}

static inline void wj_i2s_receive_mode_slave(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_ADEN_MASTER_MODE;
}

static inline void wj_i2s_receive_mode_sample_is_voice(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_RVOICEEN_VOICE;
}

static inline void wj_i2s_receive_mode_sample_is_audio(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_RVOICEEN_VOICE;
}

static inline void wj_i2s_receive_mode_channel_polarity_high_for_left(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_RALOLRC_HIGHFORLEFT;
}

static inline void wj_i2s_receive_mode_channel_polarity_low_for_left(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_RALOLRC_HIGHFORLEFT;
}

static inline void wj_i2s_receive_mode_format_i2s(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_RSAFS_Msk;
}

static inline void wj_i2s_receive_mode_format_right_justified(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_RSAFS_Msk;
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_RSAFS_RIGHT_JUSTIFIED;
}

static inline void wj_i2s_receive_mode_format_left_justified(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_IN &= ~WJ_IISCNFIN_RSAFS_Msk;
    i2s_base->I2S_IISCNF_IN |= WJ_IISCNFIN_RSAFS_LEFT_JUSTIFIED;
}

static inline void wj_i2s_receive_mode_audio_rate_scale_factor_1(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_ARS_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_ARS_1;
}

static inline void wj_i2s_receive_mode_audio_rate_scale_factor_0_5(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_ARS_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_ARS_0_5;
}

static inline void wj_i2s_receive_mode_audio_rate_scale_factor_0_25(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_ARS_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_ARS_0_25;
}

static inline void wj_i2s_receive_mode_audio_fundamental_rate_44_1k(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_AFR_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_AFR_44_1KSPS;
}

static inline void wj_i2s_receive_mode_audio_fundamental_rate_48k(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_AFR_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_AFR_48KSPS;
}

static inline void wj_i2s_receive_mode_audio_fundamental_rate_32k(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_AFR_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_AFR_32KSPS;
}

static inline void wj_i2s_receive_mode_audio_fundamental_rate_96k(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_AFR_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_AFR_96KSPS;
}

static inline void wj_i2s_receive_mode_16bit_input_16bit_store(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_I2SMODE_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_I2SMODE_16I16S;
}

static inline void wj_i2s_receive_mode_16bit_input_24bit_store(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_I2SMODE_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_I2SMODE_16I24S;
}

static inline void wj_i2s_receive_mode_24bit_input_16bit_store(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_I2SMODE_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_I2SMODE_24I16S;
}

static inline void wj_i2s_receive_mode_24bit_input_24bit_store(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_I2SMODE_Msk;
    i2s_base->I2S_FSSTA |= WJ_FSSTA_I2SMODE_24I24S;
}

static inline void wj_i2s_receive_mode_audio_input_rate_set_by_user(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA &= ~WJ_FSSTA_AIRAD;
}

static inline void wj_i2s_receive_mode_audio_input_rate_detected_hardware(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_FSSTA |= WJ_FSSTA_AIRAD;
}

static inline void wj_i2s_transmit_mode_master(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_TWMS_SLAVE;
}

static inline void wj_i2s_transmit_mode_slave(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_TWMS_SLAVE;
}

static inline void wj_i2s_transmit_mode_source_is_single_channel(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_TVOICEEN_VOICE;
}

static inline void wj_i2s_transmit_mode_source_is_dual_channel(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_TVOICEEN_VOICE;
}

static inline void wj_i2s_transmit_mode_channel_polarity_high_for_left(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_TALOLRC_HIGHFORLEFT;
}

static inline void wj_i2s_transmit_mode_channel_polarity_low_for_left(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_TALOLRC_HIGHFORLEFT;
}

static inline void wj_i2s_transmit_mode_format_i2s(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_TSAFS_Msk;
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_TSAFS_I2S;
}

static inline void wj_i2s_transmit_mode_format_right_justified(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_TSAFS_Msk;
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_TSAFS_RIGHT_JUSTIFIED;
}

static inline void wj_i2s_transmit_mode_format_left_justified(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_IISCNF_OUT &= ~WJ_IISCNFOUT_TSAFS_Msk;
    i2s_base->I2S_IISCNF_OUT |= WJ_IISCNFOUT_TSAFS_LEFT_JUSTIFIED;
}

static inline void wj_i2s_96k_fs_threshold(wj_i2s_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_FADTLR |= level << WJ_FADTLR_96FTR_Pos;
}

static inline void wj_i2s_32k_fs_threshold(wj_i2s_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_FADTLR |= level << WJ_FADTLR_32FTR_Pos;
}

static inline void wj_i2s_44k_fs_threshold(wj_i2s_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_FADTLR |= level << WJ_FADTLR_44FTR_Pos;
}

static inline void wj_i2s_48k_fs_threshold(wj_i2s_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_FADTLR |= level << WJ_FADTLR_48FTR_Pos;
}

static inline void wj_i2s_tx_volume_compress_bits(wj_i2s_regs_t *i2s_base, uint32_t shift)
{
    i2s_base->I2S_SCCR |= shift << WJ_SCCR_TVCCR_Pos;
}

static inline void wj_i2s_rx_volume_compress_bits(wj_i2s_regs_t *i2s_base, uint32_t shift)
{
    i2s_base->I2S_SCCR |= shift << WJ_SCCR_VCCR_Pos;
}

static inline void wj_i2s_sample_length_compress_bits(wj_i2s_regs_t *i2s_base, uint32_t shift)
{
    i2s_base->I2S_SCCR &= ~WJ_SCCR_SLCCR_Msk;
    i2s_base->I2S_SCCR |= shift << WJ_SCCR_SLCCR_Pos;
}

static inline void wj_i2s_sub_sample_rate_compress_no_compress(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SCCR &= ~WJ_SCCR_SSRCR_Msk;
    i2s_base->I2S_SCCR |= WJ_SCCR_SSRCR_NO_COMPRESS;
}

static inline void wj_i2s_sub_sample_rate_compress_one_time(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SCCR &= ~WJ_SCCR_SSRCR_Msk;
    i2s_base->I2S_SCCR |= WJ_SCCR_SSRCR_ONE_COMPRESS;
}

static inline void wj_i2s_sub_sample_rate_compress_three_time(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SCCR &= ~WJ_SCCR_SSRCR_Msk;
    i2s_base->I2S_SCCR |= WJ_SCCR_SSRCR_THREE_COMPRESS;
}

static inline void wj_i2s_set_transmit_fifo_threshold(wj_i2s_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_TXFTLR = level - 1U;
}

static inline void wj_i2s_set_receive_fifo_threshold(wj_i2s_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_RXFTLR = level - 1U;
}

static inline uint32_t wj_i2s_get_transmit_fifo_level(wj_i2s_regs_t *i2s_base)
{
    return i2s_base->I2S_TXFLR;
}

static inline uint32_t wj_i2s_get_receive_fifo_level(wj_i2s_regs_t *i2s_base)
{
    return i2s_base->I2S_RXFLR;
}

static inline uint32_t wj_i2s_get_rx_busy_state(wj_i2s_regs_t *i2s_base)
{
    return (i2s_base->I2S_SR & WJ_SR_RXBUSY_RX_RECEIVING);
}

static inline uint32_t wj_i2s_get_tx_busy_state(wj_i2s_regs_t *i2s_base)
{
    return (i2s_base->I2S_SR & WJ_SR_TXBUSY_TX_TRANSMITTING);
}

static inline uint32_t wj_i2s_get_tx_fifo_full_state(wj_i2s_regs_t *i2s_base)
{
    return (i2s_base->I2S_SR & WJ_SR_TFNF_TX_FIFO_NOT_FULL);
}

static inline uint32_t wj_i2s_get_tx_fifo_empty_state(wj_i2s_regs_t *i2s_base)
{
    return (i2s_base->I2S_SR & WJ_SR_TFE_TX_FIFO_EMPTY);
}

static inline uint32_t wj_i2s_get_rx_fifo_full_state(wj_i2s_regs_t *i2s_base)
{
    return (i2s_base->I2S_SR & WJ_SR_RFF_RX_FIFO_FULL);
}

static inline uint32_t wj_i2s_get_rx_fifo_empty_state(wj_i2s_regs_t *i2s_base)
{
    return (i2s_base->I2S_SR & WJ_SR_RFNE_RX_FIFO_NOT_EMPTY);
}

static inline uint32_t wj_i2s_get_tx_fifo_empty_raw_state(wj_i2s_regs_t *i2s_base)
{
    return (i2s_base->I2S_RISR & WJ_RISR_TXEIR_RAW);
}

static inline uint32_t wj_i2s_get_rx_fifo_full_raw_state(wj_i2s_regs_t *i2s_base)
{
    return (i2s_base->I2S_RISR & WJ_RISR_RXFIR_RAW);
}

static inline uint32_t wj_i2s_get_irq_state(wj_i2s_regs_t *i2s_base)
{
    return (i2s_base->I2S_ISR);
}

static inline void wj_i2s_clear_all_irq(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_ICR |= (WJ_RISR_TXEIC_CLEAR | WJ_RISR_TXOIC_CLEAR | WJ_RISR_RXUIC_CLEAR | WJ_RISR_RXOIC_CLEAR | WJ_RISR_RXFIC_CLEAR);
}

static inline void wj_i2s_clear_irq(wj_i2s_regs_t *i2s_base, uint32_t mask)
{
    i2s_base->I2S_ICR |= mask;
}

static inline void wj_i2s_enable_transmit_irq(wj_i2s_regs_t *iiS_base)
{
    iiS_base->I2S_IMR = 0U;
    iiS_base->I2S_IMR |= WJ_IMR_TX_FIFO_EMPTY_INTR_MSK;
}

static inline void wj_i2s_disable_transmit_irq(wj_i2s_regs_t *iiS_base)
{
    iiS_base->I2S_IMR &= ~WJ_IMR_TX_FIFO_EMPTY_INTR_MSK;
}

static inline void wj_i2s_enable_receive_irq(wj_i2s_regs_t *iiS_base)
{
    iiS_base->I2S_IMR = 0U;
    iiS_base->I2S_IMR |= WJ_IMR_RX_FIFO_FULL_INTR_MSK;
}

static inline void wj_i2s_disable_receive_irq(wj_i2s_regs_t *iiS_base)
{
    iiS_base->I2S_IMR &= ~WJ_IMR_RX_FIFO_FULL_INTR_MSK;
}

static inline void wj_i2s_set_receive_dma_enable(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_DMACR |= WJ_DMACR_RDMAE_EN;
}

static inline void wj_i2s_set_transmit_dma_enable(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_DMACR |= WJ_DMACR_TDMAE_EN;
}

static inline void wj_i2s_set_transmit_dma_data_num_level(wj_i2s_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_DMATDLR = level;
}

static inline void wj_i2s_set_receive_dma_data_num_level(wj_i2s_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_DMARDLR = level;
}

static inline void wj_i2s_left_channel_transmit_data(wj_i2s_regs_t *i2s_base, uint16_t data)
{
    i2s_base->I2S_DR = (uint32_t)data << WJ_DR_DRL_Pos;
}

static inline void wj_i2s_transmit_data(wj_i2s_regs_t *i2s_base, uint32_t data)
{
    i2s_base->I2S_DR = data;
}

static inline uint32_t wj_i2s_receive_data(wj_i2s_regs_t *i2s_base)
{
    return (uint32_t)i2s_base->I2S_DR;
}

static inline void wj_i2s_spdif_rx_validity_bit_store(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRCR &= ~WJ_SRCR_SPBV;
}

static inline void wj_i2s_spdif_rx_validity_bit_not_store(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRCR |= WJ_SRCR_SPBV;
}

static inline void wj_i2s_spdif_rx_parity_bit_store(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRCR &= ~WJ_SRCR_SVBV;
}

static inline void wj_i2s_spdif_rx_parity_bit_not_store(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRCR |= WJ_SRCR_SVBV;
}

static inline void wj_i2s_spdif_rx_sample_rate_44_1k(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRSSR &= ~WJ_SRSSR_SFS_Msk;
    i2s_base->I2S_SRSSR |= WJ_SRSSR_SFS_44_1KHZ;
}

static inline void wj_i2s_spdif_rx_sample_rate_48k(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRSSR &= ~WJ_SRSSR_SFS_Msk;
    i2s_base->I2S_SRSSR |= WJ_SRSSR_SFS_48KHZ;
}

static inline void wj_i2s_spdif_rx_sample_rate_32k(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRSSR &= ~WJ_SRSSR_SFS_Msk;
    i2s_base->I2S_SRSSR |= WJ_SRSSR_SFS_32KHZ;
}

static inline void wj_i2s_spdif_rx_no_pre_emphasis(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRSSR &= ~WJ_SRSSR_SEMP;
}

static inline void wj_i2s_spdif_rx_pre_emphasis(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRSSR |= WJ_SRSSR_SEMP;
}

static inline void wj_i2s_spdif_rx_copy_inhibited(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRSSR &= ~WJ_SRSSR_SCOPY;
}

static inline void wj_i2s_spdif_rx_copy_permitted(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRSSR |= WJ_SRSSR_SCOPY;
}

static inline void wj_i2s_spdif_rx_signal_non_audio(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRSSR &= ~WJ_SRSSR_SAUDIO;
}

static inline void wj_i2s_spdif_rx_signal_audio(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRSSR |= WJ_SRSSR_SAUDIO;
}

static inline void wj_i2s_spdif_rx_signal_format_consumer(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRSSR &= ~WJ_SRSSR_SPRO;
}

static inline void wj_i2s_spdif_rx_signal_format_professional(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRSSR |= WJ_SRSSR_SPRO;
}

static inline void wj_i2s_spdif_rx_busy_no_valid(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRSSR &= ~WJ_SRSSR_SPBF;
}

static inline void wj_i2s_spdif_rx_busy_locked(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_SRSSR |= WJ_SRSSR_SPBF;
}

static inline void wj_i2s_spdif_tx_sample_rate_44_1k(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_STSSR &= ~WJ_STSSR_SFS_Msk;
    i2s_base->I2S_STSSR |= WJ_STSSR_SFS_44_1KHZ;
}

static inline void wj_i2s_spdif_tx_sample_rate_48k(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_STSSR &= ~WJ_STSSR_SFS_Msk;
    i2s_base->I2S_STSSR |= WJ_STSSR_SFS_48KHZ;
}

static inline void wj_i2s_spdif_tx_sample_rate_32k(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_STSSR &= ~WJ_STSSR_SFS_Msk;
    i2s_base->I2S_STSSR |= WJ_STSSR_SFS_32KHZ;
}

static inline void wj_i2s_spdif_tx_no_pre_emphasis(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_STSSR &= ~WJ_STSSR_SEMP;
}

static inline void wj_i2s_spdif_tx_pre_emphasis(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_STSSR |= WJ_STSSR_SEMP;
}

static inline void wj_i2s_spdif_tx_copy_inhibited(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_STSSR &= ~WJ_STSSR_SCOPY;
}

static inline void wj_i2s_spdif_tx_copy_permitted(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_STSSR |= WJ_STSSR_SCOPY;
}

static inline void wj_i2s_spdif_tx_signal_non_audio(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_STSSR &= ~WJ_STSSR_SAUDIO;
}

static inline void wj_i2s_spdif_tx_signal_audio(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_STSSR |= WJ_STSSR_SAUDIO;
}

static inline void wj_i2s_spdif_tx_signal_format_consumer(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_STSSR &= ~WJ_STSSR_SPRO;
}

static inline void wj_i2s_spdif_tx_signal_format_professional(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_STSSR |= WJ_STSSR_SPRO;
}

static inline void wj_i2s_spdif_tx_busy_no_valid(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_STSSR &= ~WJ_STSSR_SPBF;
}

static inline void wj_i2s_spdif_tx_busy_locked(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_STSSR |= WJ_STSSR_SPBF;
}

static inline void wj_i2s_set_div_mclk(wj_i2s_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_DIV0_LEVEL = level;
}

static inline void wj_i2s_set_div_mclk_odd(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_DIV0_LEVEL |= WJ_DIV0LEVEL_DIV0_ODD;
}

static inline void wj_i2s_clear_div_mclk_odd(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_DIV0_LEVEL &= ~WJ_DIV0LEVEL_DIV0_ODD;
}

static inline void wj_i2s_set_div_mclk_oe_b(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_DIV0_LEVEL |= WJ_DIV0LEVEL_DIV0_MCLKOE_B;
}

static inline void wj_i2s_clear_div_mclk_oe_b(wj_i2s_regs_t *i2s_base)
{
    i2s_base->I2S_DIV0_LEVEL &= ~WJ_DIV0LEVEL_DIV0_MCLKOE_B;
}

static inline void wj_i2s_set_div_spdif_clk(wj_i2s_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_DIV1_LEVEL = level;
}

static inline void wj_i2s_set_div_wsclk(wj_i2s_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_DIV2_LEVEL = level;
}

static inline void wj_i2s_set_div_ref_clk(wj_i2s_regs_t *i2s_base, uint32_t level)
{
    i2s_base->I2S_DIV3_LEVEL = level;
}

#ifdef __cplusplus
}
#endif

#endif /* _WJ_IIS_LL_H_ */

