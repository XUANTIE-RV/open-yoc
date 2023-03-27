/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     dw_dma_ll.h
 * @brief    header file for dma ll driver
 * @version  V1.0
 * @date     16. Mar 2020
 ******************************************************************************/
#ifndef _DW_DMA_LL_H_
#define _DW_DMA_LL_H_
#include <stdio.h>
#include <soc.h>
#include <csi_core.h>
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Work Flow
 * 1.Configure register: SAR, DAR, CH_CTRLA, CH_CTRLB and any other register. And last set
 * DMACCFG.DMACEN and CH_EN.chnen 1'b1.
 * 2.Config ETB, configure channel trigger enable control registers
 * 3.Wait int flag, the CH_INTS.statustfr interrupt flag will clear CH_EN.chn_en to 0
 * 4. After receive CH_INTC.cleartfr clear interrupt fla. Before user this channel, should config
 * CH_EN.chn_en to 1.
 * Note: SAR, DAR, CH_CTRLA, CH_CTRLB is enable protected (use channel internal enable protect, not
 * DMACEN). Before set DMACCFG.DMACEN 1'b1, user should configure it's channel registers.
 */
/* The registers of single channel, channel is 0~15 */
/*!  SAR, offset: 0x000 */
#define DW_DMA_SAR_Pos                                      (0U)
#define DW_DMA_SAR_Msk                                      (0xFFFFFFFFU << DW_DMA_SAR_Pos)

/*!  DAR, offset: 0x008 */
#define DW_DMA_DAR_Pos                                      (0U)
#define DW_DMA_DAR_Msk                                      (0xFFFFFFFFU << DW_DMA_DAR_Pos)

/*!  LLP, offset: 0x010 */
#define DW_DMA_LLP_LLP_Pos                                  (0U)
#define DW_DMA_LLP_LLP_Msk                                  (0x3U << DW_DMA_DAR_Pos)
#define DW_DMA_LLP_PORT_1                                   (0x0U << DW_DMA_DAR_Pos)
#define DW_DMA_LLP_PORT_2                                   (0x1U << DW_DMA_DAR_Pos)
#define DW_DMA_LLP_PORT_3                                   (0x2U << DW_DMA_DAR_Pos)
#define DW_DMA_LLP_PORT_4                                   (0x3U << DW_DMA_DAR_Pos)

/*!  CTL, offset: 0x018 */
#define DW_DMA_CTL_INT_EN_Pos                               (0U)
#define DW_DMA_CTL_INT_EN_Msk                               (0x1U << DW_DMA_CTL_INT_EN_Pos)
#define DW_DMA_CTL_INT_EN_En                                DW_DMA_CTL_INT_EN_Msk
#define DW_DMA_CTL_DST_TR_WIDTH_Pos                         (1U)
#define DW_DMA_CTL_DST_TR_WIDTH_Msk                         (0x7U << DW_DMA_CTL_DST_TR_WIDTH_Pos)
#define DW_DMA_CTL_DST_TR_WIDTH_8                           (0x0U << DW_DMA_CTL_DST_TR_WIDTH_Pos)
#define DW_DMA_CTL_DST_TR_WIDTH_16                          (0x1U << DW_DMA_CTL_DST_TR_WIDTH_Pos)
#define DW_DMA_CTL_DST_TR_WIDTH_32                          (0x2U << DW_DMA_CTL_DST_TR_WIDTH_Pos)
#define DW_DMA_CTL_SRT_TR_WIDTH_Pos                         (4U)
#define DW_DMA_CTL_SRT_TR_WIDTH_Msk                         (0x7U << DW_DMA_CTL_SRT_TR_WIDTH_Pos)
#define DW_DMA_CTL_SRT_TR_WIDTH_En                          DW_DMA_CTL_SRT_TR_WIDTH_Msk
#define DW_DMA_CTL_SRT_TR_WIDTH_8                           (0x0U << DW_DMA_CTL_SRT_TR_WIDTH_Pos)
#define DW_DMA_CTL_SRT_TR_WIDTH_16                          (0x1U << DW_DMA_CTL_SRT_TR_WIDTH_Pos)
#define DW_DMA_CTL_SRT_TR_WIDTH_32                          (0x2U << DW_DMA_CTL_SRT_TR_WIDTH_Pos)
#define DW_DMA_CTL_DINC_Pos                                 (7U)
#define DW_DMA_CTL_DINC_Msk                                 (0x3U << DW_DMA_CTL_DINC_Pos)
#define DW_DMA_CTL_DINC_En                                  DW_DMA_CTL_DINC_Msk
#define DW_DMA_CTL_DINC_INC                                 (0x0U << DW_DMA_CTL_DINC_Pos)
#define DW_DMA_CTL_DINC_DINC                                (0x1U << DW_DMA_CTL_DINC_Pos)
#define DW_DMA_CTL_DINC_NOCHANAGE                           (0x2U << DW_DMA_CTL_DINC_Pos)
#define DW_DMA_CTL_SINC_Pos                                 (9U)
#define DW_DMA_CTL_SINC_Msk                                 (0x3U << DW_DMA_CTL_SINC_Pos)
#define DW_DMA_CTL_SINC_En                                  DW_DMA_CTL_SINC_Msk
#define DW_DMA_CTL_SINC_INC                                 (0x0U << DW_DMA_CTL_SINC_Pos)
#define DW_DMA_CTL_SINC_DINC                                (0x1U << DW_DMA_CTL_SINC_Pos)
#define DW_DMA_CTL_SINC_NOCHANAGE                           (0x2U << DW_DMA_CTL_SINC_Pos)
#define DW_DMA_CTL_DEST_MSIZE_Pos                           (11U)
#define DW_DMA_CTL_DEST_MSIZE_Msk                           (0x7U << DW_DMA_CTL_DEST_MSIZE_Pos)
#define DW_DMA_CTL_DEST_MSIZE_En                            DW_DMA_CTL_DEST_MSIZE_Msk
#define DW_DMA_CTL_DEST_MSIZE_1                             (0x0U << DW_DMA_CTL_DEST_MSIZE_Pos)
#define DW_DMA_CTL_DEST_MSIZE_4                             (0x1U << DW_DMA_CTL_DEST_MSIZE_Pos)
#define DW_DMA_CTL_DEST_MSIZE_8                             (0x2U << DW_DMA_CTL_DEST_MSIZE_Pos)
#define DW_DMA_CTL_DEST_MSIZE_16                            (0x3U << DW_DMA_CTL_DEST_MSIZE_Pos)
#define DW_DMA_CTL_DEST_MSIZE_32                            (0x4U << DW_DMA_CTL_DEST_MSIZE_Pos)
#define DW_DMA_CTL_DEST_MSIZE_64                            (0x5U << DW_DMA_CTL_DEST_MSIZE_Pos)
#define DW_DMA_CTL_DEST_MSIZE_128                           (0x6U << DW_DMA_CTL_DEST_MSIZE_Pos)
#define DW_DMA_CTL_DEST_MSIZE_256                           (0x7U << DW_DMA_CTL_DEST_MSIZE_Pos)
#define DW_DMA_CTL_SRC_MSIZE_Pos                            (14U)
#define DW_DMA_CTL_SRC_MSIZE_Msk                            (0x7U << DW_DMA_CTL_SRC_MSIZE_Pos)
#define DW_DMA_CTL_SRC_MSIZE_En                             DW_DMA_CTL_SRC_MSIZE_Msk
#define DW_DMA_CTL_SRC_MSIZE_1                              (0x0U << DW_DMA_CTL_SRC_MSIZE_Pos)
#define DW_DMA_CTL_SRC_MSIZE_4                              (0x1U << DW_DMA_CTL_SRC_MSIZE_Pos)
#define DW_DMA_CTL_SRC_MSIZE_8                              (0x2U << DW_DMA_CTL_SRC_MSIZE_Pos)
#define DW_DMA_CTL_SRC_MSIZE_16                             (0x3U << DW_DMA_CTL_SRC_MSIZE_Pos)
#define DW_DMA_CTL_SRC_MSIZE_32                             (0x4U << DW_DMA_CTL_SRC_MSIZE_Pos)
#define DW_DMA_CTL_SRC_MSIZE_64                             (0x5U << DW_DMA_CTL_SRC_MSIZE_Pos)
#define DW_DMA_CTL_SRC_MSIZE_128                            (0x6U << DW_DMA_CTL_SRC_MSIZE_Pos)
#define DW_DMA_CTL_SRC_MSIZE_256                            (0x7U << DW_DMA_CTL_SRC_MSIZE_Pos)
#define DW_DMA_CTL_TT_FC_Pos                                (20U)
#define DW_DMA_CTL_TT_FC_Msk                                (0x7U << DW_DMA_CTL_TT_FC_Pos)
#define DW_DMA_CTL_TT_FC_En                                 DW_DMA_CTL_TT_FC_Msk
#define DW_DMA_CTL_TT_FC_M2M                                (0x0U << DW_DMA_CTL_TT_FC_Pos) //Memory to Memory
#define DW_DMA_CTL_TT_FC_M2P                                (0x1U << DW_DMA_CTL_TT_FC_Pos) //Memory to Peripheral
#define DW_DMA_CTL_TT_FC_P2M                                (0x2U << DW_DMA_CTL_TT_FC_Pos) //Peripheral to Memory
#define DW_DMA_CTL_TT_FC_P2P                                (0x3U << DW_DMA_CTL_TT_FC_Pos) //Peripheral to Peripheral
#define DW_DMA_CTL_BLOCE_TS_Pos                             (32U)
#define DW_DMA_CTL_BLOCE_TS_Msk                             (0x7FFLLU << DW_DMA_CTL_BLOCE_TS_Pos)
#define DW_DMA_CTL_BLOCE_TS_En                              DW_DMA_CTL_BLOCE_TS_Msk

/*!  SSTAT, offset: 0x020 */
#define DW_DMA_SSTAT_SSTAT_Pos                              (0U)
#define DW_DMA_SSTAT_SSTAT_Msk                              (0xFFFFFFFFU << DW_DMA_SSTAT_SSTAT_Pos)
#define DW_DMA_SSTAT_SSTAT_En                               DW_DMA_SSTAT_SSTAT_Msk

/*!  DSTAT, offset: 0x028 */
#define DW_DMA_DSTAT_DSTAT_Pos                              (0U)
#define DW_DMA_DSTAT_DSTAT_Msk                              (0xFFFFFFFFU << DW_DMA_DSTAT_DSTAT_Pos)
#define DW_DMA_DSTAT_DSTAT_En                               DW_DMA_SSTAT_SSTAT_Msk

/*!  SSTATAR, offset: 0x030 */
#define DW_DMA_SSTATAR_SSTATAR_Pos                          (0U)
#define DW_DMA_SSTATAR_SSTATAR_Msk                          (0x3U << DW_DMA_SSTATAR_SSTATAR_Pos)
#define DW_DMA_SSTATAR_SSTATAR_En                           DW_DMA_SSTATAR_SSTATAR_Msk

/*!  DSTATAR, offset: 0x038 */
#define DW_DMA_DSTATAR_DSTATAR_Pos                          (0U)
#define DW_DMA_DSTATAR_DSTATAR_Msk                          (0x3U << DW_DMA_DSTATAR_DSTATAR_Pos)
#define DW_DMA_DSTATAR_DSTATAR_En                           DW_DMA_DSTATAR_DSTATAR_Msk

/*!  CFG, offset: 0x040 */
#define DW_DMA_CFG_CH_PRIOR_Pos                             (5U)
#define DW_DMA_CFG_CH_PRIOR_Msk                             (0x7U << DW_DMA_CFG_CH_PRIOR_Pos)
#define DW_DMA_CFG_CH_PRIOR_En                              DW_DMA_CFG_CH_PRIOR_Msk
#define DW_DMA_CFG_CH_PRIOR_0                               (0x0U << DW_DMA_CFG_CH_PRIOR_Pos)
#define DW_DMA_CFG_CH_PRIOR_1                               (0x1U << DW_DMA_CFG_CH_PRIOR_Pos)
#define DW_DMA_CFG_CH_PRIOR_2                               (0x2U << DW_DMA_CFG_CH_PRIOR_Pos)
#define DW_DMA_CFG_CH_PRIOR_3                               (0x3U << DW_DMA_CFG_CH_PRIOR_Pos)
#define DW_DMA_CFG_CH_PRIOR_4                               (0x4U << DW_DMA_CFG_CH_PRIOR_Pos)
#define DW_DMA_CFG_CH_SUSP_Pos                              (8U)
#define DW_DMA_CFG_CH_SUSP_Msk                              (0x1U << DW_DMA_CFG_CH_SUSP_Pos)
#define DW_DMA_CFG_CH_SUSP_En                               DW_DMA_CFG_CH_SUSP_Msk
#define DW_DMA_CFG_FIFO_EMPTY_Pos                           (9U)
#define DW_DMA_CFG_FIFO_EMPTY_Msk                           (0x1U << DW_DMA_CFG_FIFO_EMPTY_Pos)
#define DW_DMA_CFG_FIFO_EMPTY_En                            DW_DMA_CFG_FIFO_EMPTY_Msk
#define DW_DMA_CFG_HS_SEL_DST_Pos                           (10U)
#define DW_DMA_CFG_HS_SEL_DST_Msk                           (0x1U << DW_DMA_CFG_HS_SEL_DST_Pos)
#define DW_DMA_CFG_HS_SEL_DST_En                            DW_DMA_CFG_FIFO_EMPTY_Msk
#define DW_DMA_CFG_HS_SEL_SRC_Pos                           (11U)
#define DW_DMA_CFG_HS_SEL_SRC_Msk                           (0x1U << DW_DMA_CFG_HS_SEL_SRC_Pos)
#define DW_DMA_CFG_HS_SEL_SRC_En                            DW_DMA_CFG_HS_SEL_SRC_Msk
#define DW_DMA_CFG_LOCK_CH_L_Pos                            (12U)
#define DW_DMA_CFG_LOCK_CH_L_Msk                            (0x3U << DW_DMA_CFG_LOCK_CH_L_Pos)
#define DW_DMA_CFG_LOCK_CH_L_En                             DW_DMA_CFG_LOCK_CH_L_Msk
#define DW_DMA_CFG_LOCK_CH_L_OVER_TRANSFER                  (0x0U << DW_DMA_CFG_LOCK_CH_L_Pos)
#define DW_DMA_CFG_LOCK_CH_L_OVER_BLOCK_TRANSFER            (0x1U << DW_DMA_CFG_LOCK_CH_L_Pos)
#define DW_DMA_CFG_LOCK_CH_L_OVER_TRANSACTION               (0x10U << DW_DMA_CFG_LOCK_CH_L_Pos)
#define DW_DMA_CFG_LOCK_B_L_Pos                             (14U)
#define DW_DMA_CFG_LOCK_B_L_Msk                             (0x3U << DW_DMA_CFG_LOCK_B_L_Pos)
#define DW_DMA_CFG_LOCK_B_L_En                              DW_DMA_CFG_LOCK_B_L_Msk
#define DW_DMA_CFG_LOCK_B_L_OVER_TRANSFER                   (0x0U << DW_DMA_CFG_LOCK_B_L_Pos)
#define DW_DMA_CFG_LOCK_B_L_OVER_BLOCK_TRANSFER             (0x1U << DW_DMA_CFG_LOCK_B_L_Pos)
#define DW_DMA_CFG_LOCK_B_L_OVER_TRANSACTION                (0x10U << DW_DMA_CFG_LOCK_B_L_Pos)
#define DW_DMA_CFG_LOCK_CH_Pos                              (16U)
#define DW_DMA_CFG_LOCK_CH_Msk                              (0x1U << DW_DMA_CFG_LOCK_CH_Pos)
#define DW_DMA_CFG_LOCK_CH_En                               DW_DMA_CFG_LOCK_CH_Msk
#define DW_DMA_CFG_LOCK_B_Pos                               (17U)
#define DW_DMA_CFG_LOCK_B_Msk                               (0x1U << DW_DMA_CFG_LOCK_B_Pos)
#define DW_DMA_CFG_LOCK_B_En                                DW_DMA_CFG_LOCK_B_Msk
#define DW_DMA_CFG_DST_HS_POL_Pos                           (18U)
#define DW_DMA_CFG_DST_HS_POL_Msk                           (0x1U << DW_DMA_CFG_DST_HS_POL_Pos)
#define DW_DMA_CFG_DST_HS_POL_En                            DW_DMA_CFG_DST_HS_POL_Msk
#define DW_DMA_CFG_SRC_HS_POL_Pos                           (19U)
#define DW_DMA_CFG_SRC_HS_POL_Msk                           (0x1U << DW_DMA_CFG_SRC_HS_POL_Pos)
#define DW_DMA_CFG_SRC_HS_POL_En                            DW_DMA_CFG_SRC_HS_POL_Msk
#define DW_DMA_CFG_MAX_ABRST_Pos                            (20U)
#define DW_DMA_CFG_MAX_ABRST_Msk                            (0x1FFU << DW_DMA_CFG_MAX_ABRST_Pos)
#define DW_DMA_CFG_MAX_ABRST_En                             DW_DMA_CFG_MAX_ABRST_Msk
#define DW_DMA_CFG_RELOAD_SRC_Pos                           (30U)
#define DW_DMA_CFG_RELOAD_SRC_Msk                           (0x1U << DW_DMA_CFG_RELOAD_SRC_Pos)
#define DW_DMA_CFG_RELOAD_SRC_En                            DW_DMA_CFG_RELOAD_SRC_Msk
#define DW_DMA_CFG_RELOAD_DST_Pos                           (31U)
#define DW_DMA_CFG_RELOAD_DST_Msk                           (0x1U << DW_DMA_CFG_RELOAD_DST_Pos)
#define DW_DMA_CFG_RELOAD_DST_En                            DW_DMA_CFG_RELOAD_DST_Msk
#define DW_DMA_CFG_FCMODE_Pos                               (32U)
#define DW_DMA_CFG_FCMODE_Msk                               (0x1LLU << DW_DMA_CFG_FCMODE_Pos)
#define DW_DMA_CFG_FCMODE_En                                DW_DMA_CFG_FCMODE_Msk
#define DW_DMA_CFG_FIFO_MODE_Pos                            (33U)
#define DW_DMA_CFG_FIFO_MODE_Msk                            (0x1LLU << DW_DMA_CFG_FIFO_MODE_Pos)
#define DW_DMA_CFG_FIFO_MODE_En                             DW_DMA_CFG_FIFO_MODE_Msk
#define DW_DMA_CFG_PROTCTL_Pos                              (34U)
#define DW_DMA_CFG_PROTCTL_Msk                              (0x7LLU << DW_DMA_CFG_PROTCTL_Pos)
#define DW_DMA_CFG_PROTCTL_En                               DW_DMA_CFG_PROTCTL_Msk
#define DW_DMA_CFG_SRC_PER_Pos                              (39U)
#define DW_DMA_CFG_SRC_PER_Msk                              (0xFLLU << DW_DMA_CFG_SRC_PER_Pos)
#define DW_DMA_CFG_SRC_PER_En                               DW_DMA_CFG_SRC_PER_Msk
#define DW_DMA_CFG_DEST_PER_Pos                             (43U)
#define DW_DMA_CFG_DEST_PER_Msk                             (0xFLLU << DW_DMA_CFG_DEST_PER_Pos)
#define DW_DMA_CFG_DEST_PER_En                              DW_DMA_CFG_DEST_PER_Msk

/*!  RawTfr, offset: 0x2c0*/
#define DW_DMA_Rawtfr_Pos                                   (0U)
#define DW_DMA_Rawtfr_DMA0_Msk                              (0xFU << DW_DMA_Rawtfr_Pos)
#define DW_DMA_Rawtfr_DMA0_En                               DW_DMA_Rawtfr__DMA0_Msk
#define DW_DMA_Rawtfr_DMA1_Msk                              (0xFFU << DW_DMA_Rawtfr_Pos)
#define DW_DMA_Rawtfr_DMA1_En                               DW_DMA_Rawtfr_DMA1_Msk

/*!  RawBlock, offset: 0x2c8*/
#define DW_DMA_RawBlock_Pos                                 (0U)
#define DW_DMA_RawBlock_DMA0_Msk                            (0xFU << DW_DMA_RawBlock_Pos)
#define DW_DMA_RawBlock_DMA0_En                             DW_DMA_RawBlock_DMA0_Msk
#define DW_DMA_RawBlock_DMA1_Msk                            (0xFFU << DW_DMA_RawBlock_Pos)
#define DW_DMA_RawBlock_DMA1_En                             DW_DMA_RawBlock_DMA1_Msk

/*!  RawSrcTran, offset: 0x2d0*/
#define DW_DMA_RawSrcTran_Pos                               (0U)
#define DW_DMA_RawSrcTran_DMA0_Msk                          (0xFU << DW_DMA_RawSrcTran_Pos)
#define DW_DMA_RawSrcTran_DMA0_En                           DW_DMA_RawSrcTran_DMA0_Msk
#define DW_DMA_RawSrcTran_DMA1_Msk                          (0xFFU << DW_DMA_RawSrcTran_Pos)
#define DW_DMA_RawSrcTran_DMA1_En                           DW_DMA_RawSrcTran_DMA1_Msk

/*!  RawDstTran, offset: 0x2d8*/
#define DW_DMA_RawDstTran_Pos                               (0U)
#define DW_DMA_RawDstTran_Msk                               (0x1FU << DW_DMA_RawDstTran_Pos)
#define DW_DMA_RawDstTran_En                                DW_DMA_RawDstTran_Msk

/*!  RawErr, offset: 0x2e0*/
#define DW_DMA_RawErr_Pos                                   (0U)
#define DW_DMA_RawErr_Msk                                   (0x1FU << DW_DMA_RawErr_Pos)
#define DW_DMA_RawErr_En                                    DW_DMA_RawErr_Msk

/*!  StatusTfr, offset: 0x2e8*/
#define DW_DMA_StatusTfr_Pos                                (0U)
#define DW_DMA_StatusTfr_DMA0_Msk                           (0xFU << DW_DMA_StatusTfr_Pos)
#define DW_DMA_StatusTfr_DMA0_En_CH0                        (0x1U << DW_DMA_StatusTfr_DMA0_Msk)
#define DW_DMA_StatusTfr_DMA0_En_CH1                        (0x2U << DW_DMA_StatusTfr_DMA0_Msk)
#define DW_DMA_StatusTfr_DMA0_En_CH2                        (0x4U << DW_DMA_StatusTfr_DMA0_Msk)
#define DW_DMA_StatusTfr_DMA0_En_CH3                        (0x8U << DW_DMA_StatusTfr_DMA0_Msk)
#define DW_DMA_StatusTfr_DMA1_Msk                           (0xFFU <<DW_DMA_StatusTfr_Pos)
#define DW_DMA_StatusTfr_DMA1_En_CH0                        (0x1U << DW_DMA_StatusTfr_DMA1_Msk)
#define DW_DMA_StatusTfr_DMA1_En_CH1                        (0x2U << DW_DMA_StatusTfr_DMA1_Msk)
#define DW_DMA_StatusTfr_DMA1_En_CH2                        (0x4U << DW_DMA_StatusTfr_DMA1_Msk)
#define DW_DMA_StatusTfr_DMA1_En_CH3                        (0x8U << DW_DMA_StatusTfr_DMA1_Msk)
#define DW_DMA_StatusTfr_DMA1_En_CH4                        (0x10U <<DW_DMA_StatusTfr_DMA1_Msk)
#define DW_DMA_StatusTfr_DMA1_En_CH5                        (0x20U <<DW_DMA_StatusTfr_DMA1_Msk)
#define DW_DMA_StatusTfr_DMA1_En_CH6                        (0x40U <<DW_DMA_StatusTfr_DMA1_Msk)
#define DW_DMA_StatusTfr_DMA1_En_CH7                        (0x80U <<DW_DMA_StatusTfr_DMA1_Msk)

/*!  StatusBlock, offset: 0x2f0*/
#define DW_DMA_StatusBlock_Pos                              (0U)
#define DW_DMA_StatusBlock_DMA0_Msk                         (0xFU << DW_DMA_StatusBlock_Pos)
#define DW_DMA_StatusBlock_DMA0_En_CH0                      (0x1U << DW_DMA_StatusBlock_DMA0_Msk)
#define DW_DMA_StatusBlock_DMA0_En_CH1                      (0x2U << DW_DMA_StatusBlock_DMA0_Msk)
#define DW_DMA_StatusBlock_DMA0_En_CH2                      (0x4U << DW_DMA_StatusBlock_DMA0_Msk)
#define DW_DMA_StatusBlock_DMA0_En_CH3                      (0x8U << DW_DMA_StatusBlock_DMA0_Msk)
#define DW_DMA_StatusBlock_DMA1_Msk                         (0xFFU <<DW_DMA_StatusBlock_Pos)
#define DW_DMA_StatusBlock_DMA1_En_CH0                      (0x1U << DW_DMA_StatusBlock_DMA1_Msk)
#define DW_DMA_StatusBlock_DMA1_En_CH1                      (0x2U << DW_DMA_StatusBlock_DMA1_Msk)
#define DW_DMA_StatusBlock_DMA1_En_CH2                      (0x4U << DW_DMA_StatusBlock_DMA1_Msk)
#define DW_DMA_StatusBlock_DMA1_En_CH3                      (0x8U << DW_DMA_StatusBlock_DMA1_Msk)
#define DW_DMA_StatusBlock_DMA1_En_CH4                      (0x10U <<DW_DMA_StatusBlock_DMA1_Msk)
#define DW_DMA_StatusBlock_DMA1_En_CH5                      (0x20U <<DW_DMA_StatusBlock_DMA1_Msk)
#define DW_DMA_StatusBlock_DMA1_En_CH6                      (0x40U <<DW_DMA_StatusBlock_DMA1_Msk)
#define DW_DMA_StatusBlock_DMA1_En_CH7                      (0x80U <<DW_DMA_StatusBlock_DMA1_Msk)

/*!  StatusSrcTran, offset: 0x2f8*/
#define DW_DMA_StatusSrcTran_Pos                            (0U)
#define DW_DMA_StatusSrcTran_DMA0_Msk                       (0xFU << DW_DMA_StatusSrcTran_Pos)
#define DW_DMA_StatusSrcTran_DMA0_En_CH0                    (0x1U << DW_DMA_StatusSrcTran_DMA0_Msk)
#define DW_DMA_StatusSrcTran_DMA0_En_CH1                    (0x2U << DW_DMA_StatusSrcTran_DMA0_Msk)
#define DW_DMA_StatusSrcTran_DMA0_En_CH2                    (0x4U << DW_DMA_StatusSrcTran_DMA0_Msk)
#define DW_DMA_StatusSrcTran_DMA0_En_CH3                    (0x8U << DW_DMA_StatusSrcTran_DMA0_Msk)
#define DW_DMA_StatusSrcTran_DMA1_Msk                       (0xFFU <<DW_DMA_StatusSrcTran_Pos)
#define DW_DMA_StatusSrcTran_DMA1_En_CH0                    (0x1U << DW_DMA_StatusSrcTran_DMA1_Msk)
#define DW_DMA_StatusSrcTran_DMA1_En_CH1                    (0x2U << DW_DMA_StatusSrcTran_DMA1_Msk)
#define DW_DMA_StatusSrcTran_DMA1_En_CH2                    (0x4U << DW_DMA_StatusSrcTran_DMA1_Msk)
#define DW_DMA_StatusSrcTran_DMA1_En_CH3                    (0x8U << DW_DMA_StatusSrcTran_DMA1_Msk)
#define DW_DMA_StatusSrcTran_DMA1_En_CH4                    (0x10U <<DW_DMA_StatusSrcTran_DMA1_Msk)
#define DW_DMA_StatusSrcTran_DMA1_En_CH5                    (0x20U <<DW_DMA_StatusSrcTran_DMA1_Msk)
#define DW_DMA_StatusSrcTran_DMA1_En_CH6                    (0x40U <<DW_DMA_StatusSrcTran_DMA1_Msk)
#define DW_DMA_StatusSrcTran_DMA1_En_CH7                    (0x80U <<DW_DMA_StatusSrcTran_DMA1_Msk)

/*!  StatusDstTran, offset: 0x300*/
#define DW_DMA_StatusDstTran_Pos                            (0U)
#define DW_DMA_StatusDstTran_DMA0_Msk                       (0xFU << DW_DMA_StatusDstTran_Pos)
#define DW_DMA_StatusDstTran_DMA0_En_CH0                    (0x1U << DW_DMA_StatusDstTran_DMA0_Msk)
#define DW_DMA_StatusDstTran_DMA0_En_CH1                    (0x2U << DW_DMA_StatusDstTran_DMA0_Msk)
#define DW_DMA_StatusDstTran_DMA0_En_CH2                    (0x4U << DW_DMA_StatusDstTran_DMA0_Msk)
#define DW_DMA_StatusDstTran_DMA0_En_CH3                    (0x8U << DW_DMA_StatusDstTran_DMA0_Msk)
#define DW_DMA_StatusDstTran_DMA1_Msk                       (0xFFU <<DW_DMA_StatusDstTran_Pos)
#define DW_DMA_StatusDstTran_DMA1_En_CH0                    (0x1U << DW_DMA_StatusDstTran_DMA1_Msk)
#define DW_DMA_StatusDstTran_DMA1_En_CH1                    (0x2U << DW_DMA_StatusDstTran_DMA1_Msk)
#define DW_DMA_StatusDstTran_DMA1_En_CH2                    (0x4U << DW_DMA_StatusDstTran_DMA1_Msk)
#define DW_DMA_StatusDstTran_DMA1_En_CH3                    (0x8U << DW_DMA_StatusDstTran_DMA1_Msk)
#define DW_DMA_StatusDstTran_DMA1_En_CH4                    (0x10U <<DW_DMA_StatusDstTran_DMA1_Msk)
#define DW_DMA_StatusDstTran_DMA1_En_CH5                    (0x20U <<DW_DMA_StatusDstTran_DMA1_Msk)
#define DW_DMA_StatusDstTran_DMA1_En_CH6                    (0x40U <<DW_DMA_StatusDstTran_DMA1_Msk)
#define DW_DMA_StatusDstTran_DMA1_En_CH7                    (0x80U <<DW_DMA_StatusDstTran_DMA1_Msk)

/*!  StatusErr, offset: 0x308*/
#define DW_DMA_StatusErr_Pos                                (0U)
#define DW_DMA_StatusErr_DMA0_Msk                           (0xFU << DW_DMA_StatusErr_Pos)
#define DW_DMA_StatusErr_DMA0_En_CH0                        (0x1U << DW_DMA_StatusErr_DMA0_Msk)
#define DW_DMA_StatusErr_DMA0_En_CH1                        (0x2U << DW_DMA_StatusErr_DMA0_Msk)
#define DW_DMA_StatusErr_DMA0_En_CH2                        (0x4U << DW_DMA_StatusErr_DMA0_Msk)
#define DW_DMA_StatusErr_DMA0_En_CH3                        (0x8U << DW_DMA_StatusErr_DMA0_Msk)
#define DW_DMA_StatusErr_DMA1_Msk                           (0xFFU <<DW_DMA_StatusErr_Pos)
#define DW_DMA_StatusErr_DMA1_En_CH0                        (0x1U << DW_DMA_StatusErr_DMA1_Msk)
#define DW_DMA_StatusErr_DMA1_En_CH1                        (0x2U << DW_DMA_StatusErr_DMA1_Msk)
#define DW_DMA_StatusErr_DMA1_En_CH2                        (0x4U << DW_DMA_StatusErr_DMA1_Msk)
#define DW_DMA_StatusErr_DMA1_En_CH3                        (0x8U << DW_DMA_StatusErr_DMA1_Msk)
#define DW_DMA_StatusErr_DMA1_En_CH4                        (0x10U <<DW_DMA_StatusErr_DMA1_Msk)
#define DW_DMA_StatusErr_DMA1_En_CH5                        (0x20U <<DW_DMA_StatusErr_DMA1_Msk)
#define DW_DMA_StatusErr_DMA1_En_CH6                        (0x40U <<DW_DMA_StatusErr_DMA1_Msk)
#define DW_DMA_StatusErr_DMA1_En_CH7                        (0x80U <<DW_DMA_StatusErr_DMA1_Msk)

/*!  MaskTfr, offset: 0x310*/
#define DW_DMA_MaskTfr_Pos                                  (0U)
#define DW_DMA_MaskTfr_DMA0_Msk                             (0xFU << DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_DMA0_En_CH0                          (0x1U << DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_DMA0_En_CH1                          (0x2U << DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_DMA0_En_CH2                          (0x4U << DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_DMA0_En_CH3                          (0x8U << DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_DMA1_Msk                             (0xFFU << DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_DMA1_En_CH0                          (0x1U << DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_DMA1_En_CH1                          (0x2U << DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_DMA1_En_CH2                          (0x4U << DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_DMA1_En_CH3                          (0x8U << DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_DMA1_En_CH4                          (0x10U <<DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_DMA1_En_CH5                          (0x20U <<DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_DMA1_En_CH6                          (0x40U <<DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_DMA1_En_CH7                          (0x80U <<DW_DMA_MaskTfr_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_Pos                      (8U)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA0_Msk                 (0xFU << DW_DMA_MaskTfr_INT_MASK_WE_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA0_En_CH0              (0x1UL <<DW_DMA_MaskTfr_INT_MASK_WE_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA0_En_CH1              (0x2U << DW_DMA_MaskTfr_INT_MASK_WE_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA0_En_CH2              (0x4U << DW_DMA_MaskTfr_INT_MASK_WE_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA0_En_CH3              (0x8U << DW_DMA_MaskTfr_INT_MASK_WE_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA1_Msk                 (0xFFU<< DW_DMA_MaskTfr_INT_MASK_WE_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA1_En_CH0              (0x1U << DW_DMA_MaskTfr_INT_MASK_WE_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA1_En_CH1              (0x2U << DW_DMA_MaskTfr_INT_MASK_WE_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA1_En_CH2              (0x4U << DW_DMA_MaskTfr_INT_MASK_WE_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA1_En_CH3              (0x8U << DW_DMA_MaskTfr_INT_MASK_WE_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA1_En_CH4              (0x10U <<DW_DMA_MaskTfr_INT_MASK_WE_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA1_En_CH5              (0x20U <<DW_DMA_MaskTfr_INT_MASK_WE_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA1_En_CH6              (0x40U <<DW_DMA_MaskTfr_INT_MASK_WE_Pos)
#define DW_DMA_MaskTfr_INT_MASK_WE_DMA1_En_CH7              (0x80U <<DW_DMA_MaskTfr_INT_MASK_WE_Pos)

/*!  MaskBlock offset: 0x318*/
#define DW_DMA_MaskBlock_Pos                                (0U)
#define DW_DMA_MaskBlock_DMA0_Msk                           (0xFU << DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_DMA0_En_CH0                        (0x1U << DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_DMA0_En_CH1                        (0x2U << DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_DMA0_En_CH2                        (0x4U << DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_DMA0_En_CH3                        (0x8U << DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_DMA1_Msk                           (0xFFU <<DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_DMA1_En_CH0                        (0x1U << DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_DMA1_En_CH1                        (0x2U << DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_DMA1_En_CH2                        (0x4U << DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_DMA1_En_CH3                        (0x8U << DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_DMA1_En_CH4                        (0x10U <<DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_DMA1_En_CH5                        (0x20U <<DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_DMA1_En_CH6                        (0x40U <<DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_DMA1_En_CH7                        (0x80U <<DW_DMA_MaskBlock_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_Pos                    (8U)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA0_Msk               (0xFU << DW_DMA_MaskBlock_INT_MASK_WE_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA0_En_CH0            (0x1U << DW_DMA_MaskBlock_INT_MASK_WE_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA0_En_CH1            (0x2U << DW_DMA_MaskBlock_INT_MASK_WE_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA0_En_CH2            (0x4U << DW_DMA_MaskBlock_INT_MASK_WE_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA0_En_CH3            (0x8U << DW_DMA_MaskBlock_INT_MASK_WE_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA1_Msk               (0xFFU<< DW_DMA_MaskBlock_INT_MASK_WE_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA1_En_CH0            (0x1U << DW_DMA_MaskBlock_INT_MASK_WE_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA1_En_CH1            (0x2U << DW_DMA_MaskBlock_INT_MASK_WE_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA1_En_CH2            (0x4U << DW_DMA_MaskBlock_INT_MASK_WE_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA1_En_CH3            (0x8U << DW_DMA_MaskBlock_INT_MASK_WE_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA1_En_CH4            (0x10U <<DW_DMA_MaskBlock_INT_MASK_WE_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA1_En_CH5            (0x20U <<DW_DMA_MaskBlock_INT_MASK_WE_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA1_En_CH6            (0x40U <<DW_DMA_MaskBlock_INT_MASK_WE_Pos)
#define DW_DMA_MaskBlock_INT_MASK_WE_DMA1_En_CH7            (0x80U <<DW_DMA_MaskBlock_INT_MASK_WE_Pos)

/*!  MaskSrcTran  offset: 0x320*/
#define DW_DMA_MaskSrcTran_Pos                              (0U)
#define DW_DMA_MaskSrcTran_DMA0_Msk                         (0xFU << DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_DMA0_En_CH0                      (0x1U << DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_DMA0_En_CH1                      (0x2U << DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_DMA0_En_CH2                      (0x4U << DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_DMA0_En_CH3                      (0x8U << DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_DMA1_Msk                         (0xFFU <<DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_DMA1_En_CH0                      (0x1U << DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_DMA1_En_CH1                      (0x2U << DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_DMA1_En_CH2                      (0x4U << DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_DMA1_En_CH3                      (0x8U << DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_DMA1_En_CH4                      (0x10U <<DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_DMA1_En_CH5                      (0x20U <<DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_DMA1_En_CH6                      (0x40U <<DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_DMA1_En_CH7                      (0x80U <<DW_DMA_MaskSrcTran_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_Pos                  (8U)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA0_Msk             (0xFU << DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA0_En_CH0          (0x1U << DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA0_En_CH1          (0x2U << DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA0_En_CH2          (0x4U << DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA0_En_CH3          (0x8U << DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA1_Msk             (0xFFU<< DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA1_En_CH0          (0x1U << DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA1_En_CH1          (0x2U << DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA1_En_CH2          (0x4U << DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA1_En_CH3          (0x8U << DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA1_En_CH4          (0x10U <<DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA1_En_CH5          (0x20U <<DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA1_En_CH6          (0x40U <<DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskSrcTran_INT_MASK_WE_DMA1_En_CH7          (0x80U <<DW_DMA_MaskSrcTran_INT_MASK_WE_Pos)

/*!  MaskDstTran  offset: 0x328*/
#define DW_DMA_MaskDstTran_Pos                              (0U)
#define DW_DMA_MaskDstTran_DMA0_Msk                         (0xFU << DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_DMA0_En_CH0                      (0x1U << DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_DMA0_En_CH1                      (0x2U << DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_DMA0_En_CH2                      (0x4U << DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_DMA0_En_CH3                      (0x8U << DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_DMA1_Msk                         (0xFFU <<DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_DMA1_En_CH0                      (0x1U << DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_DMA1_En_CH1                      (0x2U << DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_DMA1_En_CH2                      (0x4U << DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_DMA1_En_CH3                      (0x8U << DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_DMA1_En_CH4                      (0x10U <<DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_DMA1_En_CH5                      (0x20U <<DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_DMA1_En_CH6                      (0x40U <<DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_DMA1_En_CH7                      (0x80U <<DW_DMA_MaskDstTran_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_Pos                  (8U)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA0_Msk             (0xFU << DW_DMA_MaskDstTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA0_En_CH0          (0x1U << DW_DMA_MaskDstTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA0_En_CH1          (0x2U << DW_DMA_MaskDstTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA0_En_CH2          (0x4U << DW_DMA_MaskDstTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA0_En_CH3          (0x8U << DW_DMA_MaskDstTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA1_Msk             (0xFFU<< DW_DMA_MaskDstTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA1_En_CH0          (0x1U << DW_DMA_MaskDstTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA1_En_CH1          (0x2U << DW_DMA_MaskDstTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA1_En_CH2          (0x4U << DW_DMA_MaskDstTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA1_En_CH3          (0x8U << DW_DMA_MaskDstTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA1_En_CH4          (0x10U <<DW_DMA_MaskDstTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA1_En_CH5          (0x20U <<DW_DMA_MaskDstTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA1_En_CH6          (0x40U <<DW_DMA_MaskDstTran_INT_MASK_WE_Pos)
#define DW_DMA_MaskDstTran_INT_MASK_WE_DMA1_En_CH7          (0x80U <<DW_DMA_MaskDstTran_INT_MASK_WE_Pos)

/*!  MaskErr  offset: 0x330*/
#define DW_DMA_MaskErr_Pos                                  (0U)
#define DW_DMA_MaskErr_DMA0_Msk                             (0xFU << DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_DMA0_En_CH0                          (0x1U << DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_DMA0_En_CH1                          (0x2U << DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_DMA0_En_CH2                          (0x4U << DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_DMA0_En_CH3                          (0x8U << DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_DMA1_Msk                             (0xFFU <<DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_DMA1_En_CH0                          (0x1U << DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_DMA1_En_CH1                          (0x2U << DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_DMA1_En_CH2                          (0x4U << DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_DMA1_En_CH3                          (0x8U << DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_DMA1_En_CH4                          (0x10U <<DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_DMA1_En_CH5                          (0x20U <<DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_DMA1_En_CH6                          (0x40U <<DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_DMA1_En_CH7                          (0x80U <<DW_DMA_MaskErr_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_Pos                      (8U)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA0_Msk                 (0xFU << DW_DMA_MaskErr_INT_MASK_WE_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA0_En_CH0              (0x1U << DW_DMA_MaskErr_INT_MASK_WE_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA0_En_CH1              (0x2U << DW_DMA_MaskErr_INT_MASK_WE_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA0_En_CH2              (0x4U << DW_DMA_MaskErr_INT_MASK_WE_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA0_En_CH3              (0x8U << DW_DMA_MaskErr_INT_MASK_WE_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA1_Msk                 (0xFFU<< DW_DMA_MaskErr_INT_MASK_WE_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA1_En_CH0              (0x1U << DW_DMA_MaskErr_INT_MASK_WE_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA1_En_CH1              (0x2U << DW_DMA_MaskErr_INT_MASK_WE_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA1_En_CH2              (0x4U << DW_DMA_MaskErr_INT_MASK_WE_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA1_En_CH3              (0x8U << DW_DMA_MaskErr_INT_MASK_WE_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA1_En_CH4              (0x10U <<DW_DMA_MaskErr_INT_MASK_WE_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA1_En_CH5              (0x20U <<DW_DMA_MaskErr_INT_MASK_WE_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA1_En_CH6              (0x40U <<DW_DMA_MaskErr_INT_MASK_WE_Pos)
#define DW_DMA_MaskErr_INT_MASK_WE_DMA1_En_CH7              (0x80U <<DW_DMA_MaskErr_INT_MASK_WE_Pos)

/*!  ClearTfr, offset: 0x338*/
#define DW_DMA_ClearTfr_Pos                                 (0U)
#define DW_DMA_ClearTfr_DMA0_Msk                            (0xFU << DW_DMA_ClearTfr_Pos)
#define DW_DMA_ClearTfr_DMA0_En_CH0                         (0x1U << DW_DMA_ClearTfr_Pos)
#define DW_DMA_ClearTfr_DMA0_En_CH1                         (0x2U << DW_DMA_ClearTfr_Pos)
#define DW_DMA_ClearTfr_DMA0_En_CH2                         (0x4U << DW_DMA_ClearTfr_Pos)
#define DW_DMA_ClearTfr_DMA0_En_CH3                         (0x8U << DW_DMA_ClearTfr_Pos)
#define DW_DMA_ClearTfr_DMA1_Msk                            (0xFFU <<DW_DMA_ClearTfr_Pos)
#define DW_DMA_ClearTfr_DMA1_En_CH0                         (0x1U << DW_DMA_ClearTfr_Pos)
#define DW_DMA_ClearTfr_DMA1_En_CH1                         (0x2U << DW_DMA_ClearTfr_Pos)
#define DW_DMA_ClearTfr_DMA1_En_CH2                         (0x4U << DW_DMA_ClearTfr_Pos)
#define DW_DMA_ClearTfr_DMA1_En_CH3                         (0x8U << DW_DMA_ClearTfr_Pos)
#define DW_DMA_ClearTfr_DMA1_En_CH4                         (0x10U <<DW_DMA_ClearTfr_Pos)
#define DW_DMA_ClearTfr_DMA1_En_CH5                         (0x20U <<DW_DMA_ClearTfr_Pos)
#define DW_DMA_ClearTfr_DMA1_En_CH6                         (0x40U <<DW_DMA_ClearTfr_Pos)
#define DW_DMA_ClearTfr_DMA1_En_CH7                         (0x80U <<DW_DMA_ClearTfr_Pos)

/*!  ClearBlock, offset: 0x340*/
#define DW_DMA_ClearBlock_Pos                               (0U)
#define DW_DMA_ClearBlock_DMA0_Msk                          (0xFU << DW_DMA_ClearBlock_Pos)
#define DW_DMA_ClearBlock_DMA0_En_CH0                       (0x1U << DW_DMA_ClearBlock_Pos)
#define DW_DMA_ClearBlock_DMA0_En_CH1                       (0x2U << DW_DMA_ClearBlock_Pos)
#define DW_DMA_ClearBlock_DMA0_En_CH2                       (0x4U << DW_DMA_ClearBlock_Pos)
#define DW_DMA_ClearBlock_DMA0_En_CH3                       (0x8U << DW_DMA_ClearBlock_Pos)
#define DW_DMA_ClearBlock_DMA1_Msk                          (0xFFU <<DW_DMA_ClearBlock_Pos)
#define DW_DMA_ClearBlock_DMA1_En_CH0                       (0x1U << DW_DMA_ClearBlock_Pos)
#define DW_DMA_ClearBlock_DMA1_En_CH1                       (0x2U << DW_DMA_ClearBlock_Pos)
#define DW_DMA_ClearBlock_DMA1_En_CH2                       (0x4U << DW_DMA_ClearBlock_Pos)
#define DW_DMA_ClearBlock_DMA1_En_CH3                       (0x8U << DW_DMA_ClearBlock_Pos)
#define DW_DMA_ClearBlock_DMA1_En_CH4                       (0x10U <<DW_DMA_ClearBlock_Pos)
#define DW_DMA_ClearBlock_DMA1_En_CH5                       (0x20U <<DW_DMA_ClearBlock_Pos)
#define DW_DMA_ClearBlock_DMA1_En_CH6                       (0x40U <<DW_DMA_ClearBlock_Pos)
#define DW_DMA_ClearBlock_DMA1_En_CH7                       (0x80U <<DW_DMA_ClearBlock_Pos)

/*!  ClearSrcTran, offset: 0x348*/
#define DW_DMA_ClearSrcTran_Pos                             (0U)
#define DW_DMA_ClearSrcTran_DMA0_Msk                        (0xFU << DW_DMA_ClearSrcTran_Pos)
#define DW_DMA_ClearSrcTran_DMA0_En_CH0                     (0x1U << DW_DMA_ClearSrcTran_Pos)
#define DW_DMA_ClearSrcTran_DMA0_En_CH1                     (0x2U << DW_DMA_ClearSrcTran_Pos)
#define DW_DMA_ClearSrcTran_DMA0_En_CH2                     (0x4U << DW_DMA_ClearSrcTran_Pos)
#define DW_DMA_ClearSrcTran_DMA0_En_CH3                     (0x8U << DW_DMA_ClearSrcTran_Pos)
#define DW_DMA_ClearSrcTran_DMA1_Msk                        (0xFFU <<DW_DMA_ClearSrcTran_Pos)
#define DW_DMA_ClearSrcTran_DMA1_En_CH0                     (0x1U << DW_DMA_ClearSrcTran_Pos)
#define DW_DMA_ClearSrcTran_DMA1_En_CH1                     (0x2U << DW_DMA_ClearSrcTran_Pos)
#define DW_DMA_ClearSrcTran_DMA1_En_CH2                     (0x4U << DW_DMA_ClearSrcTran_Pos)
#define DW_DMA_ClearSrcTran_DMA1_En_CH3                     (0x8U << DW_DMA_ClearSrcTran_Pos)
#define DW_DMA_ClearSrcTran_DMA1_En_CH4                     (0x10U <<DW_DMA_ClearSrcTran_Pos)
#define DW_DMA_ClearSrcTran_DMA1_En_CH5                     (0x20U <<DW_DMA_ClearSrcTran_Pos)
#define DW_DMA_ClearSrcTran_DMA1_En_CH6                     (0x40U <<DW_DMA_ClearSrcTran_Pos)
#define DW_DMA_ClearSrcTran_DMA1_En_CH7                     (0x80U <<DW_DMA_ClearSrcTran_Pos)

/*!  ClearDstTran, offset: 0x350*/
#define DW_DMA_ClearDstTran_Pos                             (0U)
#define DW_DMA_ClearDstTran_DMA0_Msk                        (0xFU << DW_DMA_ClearDstTran_Pos)
#define DW_DMA_ClearDstTran_DMA0_En_CH0                     (0x1U << DW_DMA_ClearDstTran_Pos)
#define DW_DMA_ClearDstTran_DMA0_En_CH1                     (0x2U << DW_DMA_ClearDstTran_Pos)
#define DW_DMA_ClearDstTran_DMA0_En_CH2                     (0x4U << DW_DMA_ClearDstTran_Pos)
#define DW_DMA_ClearDstTran_DMA0_En_CH3                     (0x8U << DW_DMA_ClearDstTran_Pos)
#define DW_DMA_ClearDstTran_DMA1_Msk                        (0xFFU <<DW_DMA_ClearDstTran_Pos)
#define DW_DMA_ClearDstTran_DMA1_En_CH0                     (0x1U << DW_DMA_ClearDstTran_Pos)
#define DW_DMA_ClearDstTran_DMA1_En_CH1                     (0x2U << DW_DMA_ClearDstTran_Pos)
#define DW_DMA_ClearDstTran_DMA1_En_CH2                     (0x4U << DW_DMA_ClearDstTran_Pos)
#define DW_DMA_ClearDstTran_DMA1_En_CH3                     (0x8U << DW_DMA_ClearDstTran_Pos)
#define DW_DMA_ClearDstTran_DMA1_En_CH4                     (0x10U <<DW_DMA_ClearDstTran_Pos)
#define DW_DMA_ClearDstTran_DMA1_En_CH5                     (0x20U <<DW_DMA_ClearDstTran_Pos)
#define DW_DMA_ClearDstTran_DMA1_En_CH6                     (0x40U <<DW_DMA_ClearDstTran_Pos)
#define DW_DMA_ClearDstTran_DMA1_En_CH7                     (0x80U <<DW_DMA_ClearDstTran_Pos)

/*!  ClearErr, offset: 0x350*/
#define DW_DMA_ClearErr_Pos                                 (0U)
#define DW_DMA_ClearErr_DMA0_Msk                            (0xFU << DW_DMA_ClearErr_Pos)
#define DW_DMA_ClearErr_DMA0_En_CH0                         (0x1U << DW_DMA_ClearErr_Pos)
#define DW_DMA_ClearErr_DMA0_En_CH1                         (0x2U << DW_DMA_ClearErr_Pos)
#define DW_DMA_ClearErr_DMA0_En_CH2                         (0x4U << DW_DMA_ClearErr_Pos)
#define DW_DMA_ClearErr_DMA0_En_CH3                         (0x8U << DW_DMA_ClearErr_Pos)
#define DW_DMA_ClearErr_DMA1_Msk                            (0xFFU <<DW_DMA_ClearErr_Pos)
#define DW_DMA_ClearErr_DMA1_En_CH0                         (0x1U << DW_DMA_ClearErr_Pos)
#define DW_DMA_ClearErr_DMA1_En_CH1                         (0x2U << DW_DMA_ClearErr_Pos)
#define DW_DMA_ClearErr_DMA1_En_CH2                         (0x4U << DW_DMA_ClearErr_Pos)
#define DW_DMA_ClearErr_DMA1_En_CH3                         (0x8U << DW_DMA_ClearErr_Pos)
#define DW_DMA_ClearErr_DMA1_En_CH4                         (0x10U <<DW_DMA_ClearErr_Pos)
#define DW_DMA_ClearErr_DMA1_En_CH5                         (0x20U <<DW_DMA_ClearErr_Pos)
#define DW_DMA_ClearErr_DMA1_En_CH6                         (0x40U <<DW_DMA_ClearErr_Pos)
#define DW_DMA_ClearErr_DMA1_En_CH7                         (0x80U <<DW_DMA_ClearErr_Pos)

/*!  DmaCfgReg, offset: 0x398*/
#define DW_DMA_DmaCfgReg_DMA_EN_Pos                         (0U)
#define DW_DMA_DmaCfgReg_DMA_EN_Msk                         (0x1U << DW_DMA_DmaCfgReg_DMA_EN_Pos)
#define DW_DMA_DmaCfgReg_DMA_EN_En                          DW_DMA_DmaCfgReg_DMA_EN_Msk

/*!  ChEnReg, offset: 0x3a0*/
#define DW_DMA_ChEnReg_CH_EN_Pos                            (0U)
#define DW_DMA_ChEnReg_CH_EN_Msk                            (0xFFU << DW_DMA_ChEnReg_CH_EN_Pos)
// #define DW_DMA_ChEnReg_CH_EN_En                          DW_DMA_ChEnReg_CH_EN_Msk
#define DW_DMA_ChEnReg_CH_EN_En_CH0                         (0x1U << DW_DMA_ChEnReg_CH_EN_Pos)
#define DW_DMA_ChEnReg_CH_EN_En_CH1                         (0x2U << DW_DMA_ChEnReg_CH_EN_Pos)
#define DW_DMA_ChEnReg_CH_EN_En_CH2                         (0x4U << DW_DMA_ChEnReg_CH_EN_Pos)
#define DW_DMA_ChEnReg_CH_EN_En_CH3                         (0x8U << DW_DMA_ChEnReg_CH_EN_Pos)
#define DW_DMA_ChEnReg_CH_EN_En_CH4                         (0x10U << DW_DMA_ChEnReg_CH_EN_Pos)
#define DW_DMA_ChEnReg_CH_EN_En_CH5                         (0x20U << DW_DMA_ChEnReg_CH_EN_Pos)
#define DW_DMA_ChEnReg_CH_EN_En_CH6                         (0x40U << DW_DMA_ChEnReg_CH_EN_Pos)
#define DW_DMA_ChEnReg_CH_EN_En_CH7                         (0x80U << DW_DMA_ChEnReg_CH_EN_Pos)
#define DW_DMA_ChEnReg_CH_EN_WE_Pos                         (8U)
#define DW_DMA_ChEnReg_CH_EN_WE_Msk                         (0xFFU << DW_DMA_ChEnReg_CH_EN_WE_Pos)
#define DW_DMA_ChEnReg_CH_EN_WE_CH0                         (0x1U  << DW_DMA_ChEnReg_CH_EN_WE_Pos)
#define DW_DMA_ChEnReg_CH_EN_WE_CH1                         (0x2U  << DW_DMA_ChEnReg_CH_EN_WE_Pos)
#define DW_DMA_ChEnReg_CH_EN_WE_CH2                         (0x4U  << DW_DMA_ChEnReg_CH_EN_WE_Pos)
#define DW_DMA_ChEnReg_CH_EN_WE_CH3                         (0x8U  << DW_DMA_ChEnReg_CH_EN_WE_Pos)
#define DW_DMA_ChEnReg_CH_EN_WE_CH4                         (0x10U << DW_DMA_ChEnReg_CH_EN_WE_Pos)
#define DW_DMA_ChEnReg_CH_EN_WE_CH5                         (0x20U << DW_DMA_ChEnReg_CH_EN_WE_Pos)
#define DW_DMA_ChEnReg_CH_EN_WE_CH6                         (0x40U << DW_DMA_ChEnReg_CH_EN_WE_Pos)
#define DW_DMA_ChEnReg_CH_EN_WE_CH7                         (0x80U << DW_DMA_ChEnReg_CH_EN_WE_Pos)
typedef struct {
    __IOM uint64_t  SAR;        /* offset: 0x00+n*0x58 (R/W) Channel Source Address Register        */
    __IOM uint64_t  DAR;        /* offset: 0x08+n*0x58 (R/W) Channel Destination Address Register   */
    __IOM uint64_t  LLP;        /* offset: 0x10+n*0x58 (R/W) Channel Link Point Register            */
    __IOM uint64_t  CTL;        /* offset: 0x18+n*0x58 (R/W) Channel Control Register               */
    __IOM uint64_t  SSTAT;       /* offset: 0x20+n*0x58 (R/W) Channel Status Register of the Source  */
    __IOM uint64_t  DSTAT;       /* offset: 0x28+n*0x58 (R/ ) Channel Channel Status Register of the Destination   */
    __IOM uint64_t  SSTATAR;    /* offset: 0x30+n*0x58 (R/W) Channel Status Register of the Source Address     */
    __IOM uint64_t  DSTATAR;    /* offset: 0x38+n*0x58 (R/W) Channel Status Register of the Destination Address     */
    __IOM uint64_t  CFG;        /* offset: 0x40+n*0x58 (R/W) Channel Configure Register                */
} dw_dma_ch_regs_t;
typedef struct {
    dw_dma_ch_regs_t dma_array[8];
    uint64_t RESERVED0[16];
    __IOM  uint64_t  RawTfr          ;//        0x2c0                  0x00000000       搬运原始中断状态寄存器
    __IOM  uint64_t  RawBlock        ;//        0x2c8                  0x00000000       块搬运原始中断状态寄存器
    __IOM  uint64_t  RawSrcTran      ;//        0x2d0                  0x00000000       源搬运原始中断状态寄存器
    __IOM  uint64_t  RawDstTran      ;//        0x2d8                  0x00000000       目的搬运原始中断状态寄存器
    __IOM  uint64_t  RawErr          ;//        0x2e0                  0x00000000       错误原始中断状态寄存器
    __IOM  uint64_t  StatusTfr       ;//        0x2e8                  0x00000000       搬运中断状态寄存器
    __IOM  uint64_t  StatusBlock     ;//        0x2f0                  0x00000000       块搬运中断状态寄存器
    __IOM  uint64_t  StatusSrcTran   ;//        0x2f8                  0x00000000       源搬运中断状态寄存器
    __IOM  uint64_t  StatusDstTran   ;//        0x300                  0x00000000       目的搬运中断状态寄存器
    __IOM  uint64_t  StatusErr       ;//        0x308                  0x00000000       错误中断状态寄存器
    __IOM  uint64_t  MaskTfr         ;//        0x310                  0x00000000       搬运中断屏蔽寄存器
    __IOM  uint64_t  MaskBlock       ;//        0x318                  0x00000000       块搬运中断屏蔽寄存器
    __IOM  uint64_t  MaskSrcTran     ;//        0x320                  0x00000000       源搬运中断屏蔽寄存器
    __IOM  uint64_t  MaskDstTran     ;//        0x328                  0x00000000       目的搬运中断屏蔽寄存器
    __IOM  uint64_t  MaskErr         ;//        0x330                  0x00000000       错误中断屏蔽寄存器
    __IOM  uint64_t  ClearTfr        ;//        0x338                  0x00000000       搬运中断清除寄存器
    __IOM  uint64_t  ClearBlock      ;//        0x340                  0x00000000       块搬运中断清除寄存器
    __IOM  uint64_t  ClearSrcTran    ;//        0x348                  0x00000000       源搬运中断清除寄存器
    __IOM  uint64_t  ClearDstTran    ;//        0x350                  0x00000000       目的搬运中断清除寄存器
    __IOM  uint64_t  ClearErr        ;//        0x358                  0x00000000       错误中断清除寄存器
    __IOM  uint64_t  ClearStatusInt  ;//        0x360                  0x00000000       搬运中断清除寄存器
    __IOM  uint64_t  ReqSrcReg       ;//       0x368                 源软件搬运请求寄存器
    __IOM  uint64_t  ReqDstReg       ;//       0x370                 目的软件搬运请求寄存器
    __IOM  uint64_t  SglReqSrcReg    ;//       0x378                 源单次搬运请求寄存器
    __IOM  uint64_t RESERVED01;
    __IOM  uint64_t  LstSrcReg       ;//       0x388                 源最终搬运请求寄存器
    __IOM  uint64_t  LstDstReg       ;//       0x390                 目的最终搬运请求寄存器
    // __IOM  uint64_t  StatusInt       ;//       0x360                 中断类型状态寄存器
    // __IOM  uint64_t RESERVED01[7];
    __IOM  uint64_t  DmaCfgReg       ;//       0x398                  0x00000000       总开关寄存器
    __IOM  uint64_t  ChEnReg         ;//       0x3a0                  0x00000000       通道使能寄存器
} dw_dma_regs_t;

static inline void dw_dma_set_sar(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->SAR  = value;
}

static inline uint32_t dw_dma_get_sar(dw_dma_ch_regs_t *dma, uint32_t value)
{
    return (dma->SAR & DW_DMA_SAR_Msk);
}

static inline void dw_dma_set_dar(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->DAR  = value;
}

static inline uint32_t dw_dma_get_dar(dw_dma_ch_regs_t *dma)
{
    return (dma->DAR & DW_DMA_DAR_Msk);
}

static inline void dw_dma_set_llp_llp(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->DAR  |= value & DW_DMA_LLP_LLP_Msk;
}

static inline uint32_t dw_dma_get_llp_llp(dw_dma_ch_regs_t *dma)
{
    return (dma->SAR & DW_DMA_DAR_Msk);
}

static inline void dw_dma_en_int(dw_dma_ch_regs_t *dma)
{
    dma->CTL  |= DW_DMA_CTL_INT_EN_En;
}

static inline void dw_dma_dis_int(dw_dma_ch_regs_t *dma)
{
    dma->CTL &= ~DW_DMA_CTL_INT_EN_En;
}

static inline void dw_dma_set_dst_transfer_width(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->CTL &= ~DW_DMA_CTL_DST_TR_WIDTH_Msk;
    dma->CTL  |= DW_DMA_CTL_DST_TR_WIDTH_Msk & value;
}

static inline uint32_t dw_dma_get_dst_transfer_width(dw_dma_ch_regs_t *dma)
{
    return (dma->CTL & DW_DMA_CTL_DST_TR_WIDTH_Msk);
}

static inline void dw_dma_set_src_transfer_width(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->CTL &= ~DW_DMA_CTL_SRT_TR_WIDTH_Msk;
    dma->CTL  |= DW_DMA_CTL_SRT_TR_WIDTH_Msk & value;
}

static inline uint32_t dw_dma_get_src_transfer_width(dw_dma_ch_regs_t *dma)
{
    return (dma->CTL & DW_DMA_CTL_SRT_TR_WIDTH_Msk);
}

static inline void dw_dma_set_dinc(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->CTL &= ~DW_DMA_CTL_DINC_Msk;
    dma->CTL  |= DW_DMA_CTL_DINC_Msk & value;
}

static inline uint32_t dw_dma_get_dinc(dw_dma_ch_regs_t *dma)
{
    return (dma->CTL & DW_DMA_CTL_DINC_Msk);
}

static inline void dw_dma_set_sinc(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->CTL &= ~DW_DMA_CTL_SINC_Msk;
    dma->CTL  |= DW_DMA_CTL_SINC_Msk & value;
}

static inline uint32_t dw_dma_get_sinc(dw_dma_ch_regs_t *dma)
{
    return (dma->CTL & DW_DMA_CTL_SINC_Msk);
}

static inline void dw_dma_set_dest_burst_size(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->CTL &= ~DW_DMA_CTL_DEST_MSIZE_Msk;
    dma->CTL  |= DW_DMA_CTL_DEST_MSIZE_Msk & value;
}

static inline uint32_t dw_dma_get_dest_burst_size(dw_dma_ch_regs_t *dma)
{
    return (dma->CTL & DW_DMA_CTL_DEST_MSIZE_Msk);
}

static inline void dw_dma_set_src_burst_size(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->CTL &= ~DW_DMA_CTL_SRC_MSIZE_Msk;
    dma->CTL  |= DW_DMA_CTL_SRC_MSIZE_Msk & value;
}

static inline uint32_t dw_dma_get_src_burst_size(dw_dma_ch_regs_t *dma)
{
    return (dma->CTL & DW_DMA_CTL_SRC_MSIZE_Msk);
}

static inline void dw_dma_set_transfer_type(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->CTL &= ~DW_DMA_CTL_TT_FC_Msk;
    dma->CTL  |= DW_DMA_CTL_TT_FC_Msk & value;
}

static inline uint32_t dw_dma_get_transfer_type(dw_dma_ch_regs_t *dma)
{
    return (dma->CTL & DW_DMA_CTL_TT_FC_Msk);
}

static inline void dw_dma_set_transfer_size(dw_dma_ch_regs_t *dma, uint64_t value)
{
    dma->CTL  &= ~DW_DMA_CTL_BLOCE_TS_Msk;
    dma->CTL  |= (value << DW_DMA_CTL_BLOCE_TS_Pos);
}

static inline uint64_t dw_dma_get_transfer_size(dw_dma_ch_regs_t *dma)
{
    return (dma->CTL & DW_DMA_CTL_BLOCE_TS_Msk);
}

static inline void dw_dma_set_sstat(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->SSTAT |= DW_DMA_SSTAT_SSTAT_Msk & value;
}

static inline uint32_t dw_dma_get_sstat(dw_dma_ch_regs_t *dma)
{
    return (dma->SSTAT & DW_DMA_SSTAT_SSTAT_Msk);
}

static inline void dw_dma_set_dstat(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->DSTAT  |= DW_DMA_DSTAT_DSTAT_Msk & value;
}

static inline uint32_t dw_dma_get_dstat(dw_dma_ch_regs_t *dma)
{
    return (dma->DSTAT & DW_DMA_DSTAT_DSTAT_Msk);
}

static inline void dw_dma_set_sstatar(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->SSTATAR  |= DW_DMA_SSTATAR_SSTATAR_Msk & value;
}

static inline uint32_t dw_dma_get_sstatar(dw_dma_ch_regs_t *dma)
{
    return (dma->SSTATAR & DW_DMA_SSTATAR_SSTATAR_Msk);
}

static inline void dw_dma_set_dstatar(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->SSTATAR  |= DW_DMA_DSTATAR_DSTATAR_Msk & value;
}

static inline uint32_t dw_dma_get_dstatar(dw_dma_ch_regs_t *dma)
{
    return (dma->SSTATAR & DW_DMA_DSTATAR_DSTATAR_Msk);
}

static inline void dw_dma_set_ch_prior(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->CFG  &= ~DW_DMA_CFG_CH_PRIOR_Msk;
    dma->CFG  |= DW_DMA_CFG_CH_PRIOR_Msk & value;
}

static inline uint32_t dw_dma_get_ch_prior(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_CH_PRIOR_Msk);
}

static inline void dw_dma_en_ch_susp(dw_dma_ch_regs_t *dma)
{
    dma->CFG  |= DW_DMA_CFG_CH_SUSP_En;
}

static inline void dw_dma_dis_ch_susp(dw_dma_ch_regs_t *dma)
{
    dma->CFG &= ~DW_DMA_CFG_CH_SUSP_En;
}

static inline uint32_t dw_dma_get_ch_susp(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_CH_SUSP_Msk);
}

static inline uint32_t dw_dma_get_fifo_empty(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_FIFO_EMPTY_Msk);
}

static inline void dw_dma_dst_hs_sft(dw_dma_ch_regs_t *dma)
{
    dma->CFG  |= DW_DMA_CFG_HS_SEL_DST_En;
}

static inline void dw_dma_dst_hs_hw(dw_dma_ch_regs_t *dma)
{
    dma->CFG &= ~DW_DMA_CFG_HS_SEL_DST_En;
}

static inline uint32_t dw_dma_get_hs_sel_dst(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_HS_SEL_DST_Msk);
}

static inline void dw_dma_src_hs_sft(dw_dma_ch_regs_t *dma)
{
    dma->CFG  |= DW_DMA_CFG_HS_SEL_SRC_En;
}

static inline void dw_dma_src_hs_hw(dw_dma_ch_regs_t *dma)
{
    dma->CFG &= ~DW_DMA_CFG_HS_SEL_SRC_En;
}

static inline uint32_t dw_dma_get_hs_sel_src(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_HS_SEL_SRC_Msk);
}

static inline void dw_dma_set_lock_ch_l(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->CFG  |= DW_DMA_CFG_LOCK_CH_L_Msk & value;
}

static inline uint32_t dw_dma_get_lock_ch_l(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_LOCK_CH_L_Msk);
}

static inline void dw_dma_set_lock_b_l(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->CFG  |= DW_DMA_CFG_LOCK_B_L_Msk & value;
}

static inline uint32_t dw_dma_get_lock_b_l(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_LOCK_B_L_Msk);
}

static inline void dw_dma_en_lock_ch(dw_dma_ch_regs_t *dma)
{
    dma->CFG  |= DW_DMA_CFG_LOCK_CH_En;
}

static inline void dw_dma_dis_lock_ch(dw_dma_ch_regs_t *dma)
{
    dma->CFG &= ~DW_DMA_CFG_LOCK_CH_En;
}

static inline uint32_t dw_dma_get_lock_ch(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_LOCK_CH_Msk);
}

static inline void dw_dma_en_lock_b(dw_dma_ch_regs_t *dma)
{
    dma->CFG  |= DW_DMA_CFG_LOCK_B_En;
}

static inline void dw_dma_dis_lock_b(dw_dma_ch_regs_t *dma)
{
    dma->CFG &= ~DW_DMA_CFG_LOCK_B_En;
}

static inline uint32_t dw_dma_get_lock_b(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_LOCK_B_Msk);
}

static inline void dw_dma_set_dst_hs_pol_high(dw_dma_ch_regs_t *dma)
{
    dma->CFG  |= DW_DMA_CFG_DST_HS_POL_En;
}

static inline void dw_dma_set_dst_hs_pol_low(dw_dma_ch_regs_t *dma)
{
    dma->CFG &= ~DW_DMA_CFG_DST_HS_POL_En;
}

static inline uint32_t dw_dma_get_dst_hs_pol(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_DST_HS_POL_Msk);
}

static inline void dw_dma_set_src_hs_pol_high(dw_dma_ch_regs_t *dma)
{
    dma->CFG  |= DW_DMA_CFG_SRC_HS_POL_En;
}

static inline void dw_dma_set_src_hs_pol_low(dw_dma_ch_regs_t *dma)
{
    dma->CFG &= ~DW_DMA_CFG_SRC_HS_POL_En;
}

static inline uint32_t dw_dma_get_src_hs_pol(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_SRC_HS_POL_Msk);
}

static inline void dw_dma_set_max_abrst(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->CFG  |= DW_DMA_CFG_MAX_ABRST_Msk & value;
}

static inline uint32_t dw_dma_get_max_abrst(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_MAX_ABRST_Msk);
}

static inline void dw_dma_en_reload_src(dw_dma_ch_regs_t *dma)
{
    dma->CFG  |= DW_DMA_CFG_RELOAD_SRC_En;
}

static inline void dw_dma_dis_reload_src(dw_dma_ch_regs_t *dma)
{
    dma->CFG &= ~DW_DMA_CFG_RELOAD_SRC_En;
}

static inline uint32_t dw_dma_get_reload_src(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_RELOAD_SRC_Msk);
}

static inline void dw_dma_en_reload_dst(dw_dma_ch_regs_t *dma)
{
    dma->CFG  |= DW_DMA_CFG_RELOAD_DST_En;
}

static inline void dw_dma_dis_reload_dst(dw_dma_ch_regs_t *dma)
{
    dma->CFG &= ~DW_DMA_CFG_RELOAD_DST_En;
}

static inline uint32_t dw_dma_get_reload_dst(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_RELOAD_DST_Msk);
}

static inline void dw_dma_en_fcmode(dw_dma_ch_regs_t *dma)
{
    dma->CFG  |= DW_DMA_CFG_FCMODE_En;
}

static inline void dw_dma_dis_fcmode(dw_dma_ch_regs_t *dma)
{
    dma->CFG &= ~DW_DMA_CFG_FCMODE_En;
}

static inline uint32_t dw_dma_get_fcmode(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_FCMODE_Msk);
}

static inline void dw_dma_en_fifo_mode(dw_dma_ch_regs_t *dma)
{
    dma->CFG  |= DW_DMA_CFG_FIFO_MODE_En;
}

static inline void dw_dma_dis_fifo_mode(dw_dma_ch_regs_t *dma)
{
    dma->CFG &= ~DW_DMA_CFG_FIFO_MODE_En;
}

static inline uint32_t dw_dma_get_fifo_mode(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_FIFO_MODE_Msk);
}

static inline void dw_dma_set_protctl(dw_dma_ch_regs_t *dma, uint32_t value)
{
    dma->CFG  |= DW_DMA_CFG_PROTCTL_En & value;
}

static inline uint32_t dw_dma_get_protctl(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_PROTCTL_En);
}

static inline void dw_dma_set_src_per(dw_dma_ch_regs_t *dma, uint64_t value)
{
    dma->CFG  &= ~DW_DMA_CFG_SRC_PER_Msk;
    dma->CFG  |= (value << DW_DMA_CFG_SRC_PER_Pos);
}

static inline uint32_t dw_dma_get_src_per(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_SRC_PER_Msk);
}

static inline void dw_dma_set_dest_per(dw_dma_ch_regs_t *dma, uint64_t value)
{
    dma->CFG  &= ~DW_DMA_CFG_DEST_PER_Msk;
    dma->CFG  |= (value << DW_DMA_CFG_DEST_PER_Pos);
}

static inline uint32_t dw_dma_get_dest_per(dw_dma_ch_regs_t *dma)
{
    return (dma->CFG & DW_DMA_CFG_DEST_PER_Msk);
}

static inline void dw_dma_set_rawtfr_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->RawTfr  |= DW_DMA_Rawtfr_DMA0_Msk & value;
}

static inline uint32_t dw_dma_get_rawtfr_dma0(dw_dma_regs_t *dma)
{
    return (dma->RawTfr & DW_DMA_Rawtfr_DMA0_Msk);
}

static inline void dw_dma_set_rawtfr_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->RawTfr  |= DW_DMA_Rawtfr_DMA1_Msk & value;
}

static inline uint32_t dw_dma_get_rawtfr_dma1(dw_dma_regs_t *dma)
{
    return (dma->RawTfr & DW_DMA_Rawtfr_DMA1_Msk);
}

static inline void dw_dma_set_rawblock_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->RawBlock  |= DW_DMA_RawBlock_DMA0_Msk & value;
}

static inline uint32_t dw_dma_get_rawblock_dma0(dw_dma_regs_t *dma)
{
    return (dma->RawBlock & DW_DMA_RawBlock_DMA0_Msk);
}

static inline void dw_dma_set_rawblock_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->RawBlock  |= DW_DMA_RawBlock_DMA1_Msk & value;
}

static inline uint32_t dw_dma_get_rawblock_dma1(dw_dma_regs_t *dma)
{
    return (dma->RawBlock & DW_DMA_RawBlock_DMA1_Msk);
}

static inline void dw_dma_set_rawsrctran_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->RawSrcTran  |= DW_DMA_RawSrcTran_DMA0_Msk & value;
}

static inline uint32_t dw_dma_get_rawsrctran_dma0(dw_dma_regs_t *dma)
{
    return (dma->RawSrcTran & DW_DMA_RawSrcTran_DMA0_Msk);
}

static inline void dw_dma_set_rawsrctran_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->RawSrcTran  |= DW_DMA_RawSrcTran_DMA1_Msk & value;
}

static inline uint32_t dw_dma_get_rawsrctran_dma1(dw_dma_regs_t *dma)
{
    return (dma->RawSrcTran & DW_DMA_RawSrcTran_DMA1_Msk);
}

static inline void dw_dma_set_rawdsttran(dw_dma_regs_t *dma, uint32_t value)
{
    dma->RawDstTran  |= DW_DMA_RawDstTran_Msk & value;
}

static inline uint32_t dw_dma_get_rawdsttran(dw_dma_regs_t *dma)
{
    return (dma->RawDstTran & DW_DMA_RawDstTran_Msk);
}

static inline void dw_dma_set_rawerr(dw_dma_regs_t *dma, uint32_t value)
{
    dma->RawErr  |= DW_DMA_RawErr_Msk & value;
}

static inline uint32_t dw_dma_get_rawerr(dw_dma_regs_t *dma)
{
    return (dma->RawErr & DW_DMA_RawErr_Msk);
}

static inline void dw_dma_set_statustfr_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->StatusTfr  |= DW_DMA_StatusTfr_DMA0_Msk & value;
}

static inline uint32_t dw_dma_get_statustfr_dma0(dw_dma_regs_t *dma)
{
    return (dma->StatusTfr & DW_DMA_StatusTfr_DMA0_Msk);
}

static inline void dw_dma_set_statustfr_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->StatusTfr  |= DW_DMA_StatusTfr_DMA1_Msk & value;
}

static inline uint32_t dw_dma_get_statustfr_dma1(dw_dma_regs_t *dma)
{
    return (dma->StatusTfr & DW_DMA_StatusTfr_DMA1_Msk);
}

static inline void dw_dma_set_statusblock_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->StatusBlock  |= DW_DMA_StatusBlock_DMA0_Msk & value;
}

static inline uint32_t dw_dma_get_statusblock_dma0(dw_dma_regs_t *dma)
{
    return (dma->StatusBlock & DW_DMA_StatusBlock_DMA0_Msk);
}

static inline void dw_dma_set_statusblock_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->StatusBlock  |= DW_DMA_StatusBlock_DMA1_Msk & value;
}

static inline uint32_t dw_dma_get_statusblock_dma1(dw_dma_regs_t *dma)
{
    return (dma->StatusBlock & DW_DMA_StatusBlock_DMA1_Msk);
}

static inline void dw_dma_set_statussrctran_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->StatusSrcTran  |= DW_DMA_StatusSrcTran_DMA0_Msk & value;
}

static inline uint32_t dw_dma_get_statussrctran_dma0(dw_dma_regs_t *dma)
{
    return (dma->StatusSrcTran & DW_DMA_StatusSrcTran_DMA0_Msk);
}

static inline void dw_dma_set_statussrctran_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->StatusSrcTran  |= DW_DMA_StatusSrcTran_DMA1_Msk & value;
}

static inline uint32_t dw_dma_get_statussrctran_dma1(dw_dma_regs_t *dma)
{
    return (dma->StatusSrcTran & DW_DMA_StatusSrcTran_DMA1_Msk);
}

static inline void dw_dma_set_statusdsttran_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->StatusDstTran  |= DW_DMA_StatusDstTran_DMA0_Msk & value;
}

static inline uint32_t dw_dma_get_statusdsttran_dma0(dw_dma_regs_t *dma)
{
    return (dma->StatusDstTran & DW_DMA_StatusDstTran_DMA0_Msk);
}

static inline void dw_dma_set_statusdsttran_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->StatusDstTran  |= DW_DMA_StatusDstTran_DMA1_Msk & value;
}

static inline uint32_t dw_dma_get_statusdsttran_dma1(dw_dma_regs_t *dma)
{
    return (dma->StatusDstTran & DW_DMA_StatusDstTran_DMA1_Msk);
}

static inline void dw_dma_set_statuserr_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->StatusErr  |= DW_DMA_StatusErr_DMA0_Msk & value;
}

static inline uint32_t dw_dma_get_statuserr_dma0(dw_dma_regs_t *dma)
{
    return (dma->StatusErr & DW_DMA_StatusErr_DMA0_Msk);
}

static inline void dw_dma_set_statuserr_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->StatusErr  |= DW_DMA_StatusErr_DMA1_Msk & value;
}

static inline uint32_t dw_dma_get_statuserr_dma1(dw_dma_regs_t *dma)
{
    return (dma->StatusErr & DW_DMA_StatusErr_DMA1_Msk);
}

static inline void dw_dma_set_masktfr_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    //calculate we+en
    value |= (value << DW_DMA_MaskTfr_INT_MASK_WE_Pos);
    dma->MaskTfr  |= value;
}

static inline void dw_dma_reset_masktfr_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    //calculate we+en
    value = (value << DW_DMA_MaskTfr_INT_MASK_WE_Pos);
    dma->MaskTfr  = value;
}

static inline uint32_t dw_dma_get_masktfr_dma0(dw_dma_regs_t *dma)
{
    return (dma->MaskTfr & DW_DMA_MaskTfr_DMA0_Msk);
}

static inline void dw_dma_set_masktfr_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    //calculate we+en
    value |= (value << DW_DMA_MaskTfr_INT_MASK_WE_Pos);
    dma->MaskTfr  |= value;
}

static inline void dw_dma_reset_masktfr_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    //calculate we+en
    value = (value << DW_DMA_MaskTfr_INT_MASK_WE_Pos);
    dma->MaskTfr  = value;
}

static inline uint32_t dw_dma_get_masktfr_dma1(dw_dma_regs_t *dma)
{
    return (dma->MaskTfr & DW_DMA_MaskTfr_DMA1_Msk);
}

static inline void dw_dma_set_masktfr_int_mask_we_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->MaskTfr = (dma->MaskTfr & ~DW_DMA_MaskTfr_INT_MASK_WE_DMA0_Msk) | value;
}

static inline uint32_t dw_dma_get_masktfr_int_mask_we_dma0(dw_dma_regs_t *dma)
{
    return (dma->MaskTfr & DW_DMA_MaskTfr_INT_MASK_WE_DMA0_Msk);
}

static inline void dw_dma_set_masktfr_int_mask_we_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->MaskTfr  = (dma->MaskTfr & ~DW_DMA_MaskTfr_INT_MASK_WE_DMA1_Msk) | value;
}

static inline uint32_t dw_dma_get_masktfr_int_mask_we_dma1(dw_dma_regs_t *dma)
{
    return (dma->MaskTfr & DW_DMA_MaskTfr_INT_MASK_WE_DMA1_Msk);
}

static inline void dw_dma_set_maskblock_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    value |= (value << DW_DMA_MaskBlock_INT_MASK_WE_Pos);
    dma->MaskBlock |=  value;
}

static inline void dw_dma_reset_maskblock_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    value = (value << DW_DMA_MaskBlock_INT_MASK_WE_Pos);
    dma->MaskBlock =  value;
}

static inline uint32_t dw_dma_get_maskblock_dma0(dw_dma_regs_t *dma)
{
    return (dma->MaskBlock & DW_DMA_MaskBlock_DMA0_Msk);
}

static inline void dw_dma_set_maskblock_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    value |= (value << DW_DMA_MaskBlock_INT_MASK_WE_Pos);
    dma->MaskBlock |=  value;
}

static inline void dw_dma_reset_maskblock_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    value = (value << DW_DMA_MaskBlock_INT_MASK_WE_Pos);
    dma->MaskBlock =  value;
}

static inline uint32_t dw_dma_get_maskblock_dma1(dw_dma_regs_t *dma)
{
    return (dma->MaskBlock & DW_DMA_MaskBlock_DMA1_Msk);
}

static inline void dw_dma_set_maskblock_int_mask_we_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->MaskBlock = (dma->MaskBlock & ~DW_DMA_MaskBlock_INT_MASK_WE_DMA0_Msk) | value;
}

static inline uint32_t dw_dma_get_maskblock_int_mask_we_dma0(dw_dma_regs_t *dma)
{
    return (dma->MaskBlock & DW_DMA_MaskBlock_INT_MASK_WE_DMA0_Msk);
}

static inline void dw_dma_set_maskblock_int_mask_we_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->MaskBlock = (dma->MaskBlock & ~DW_DMA_MaskBlock_INT_MASK_WE_DMA1_Msk) | value;
}

static inline uint32_t dw_dma_get_maskblock_int_mask_we_dma1(dw_dma_regs_t *dma)
{
    return (dma->MaskBlock & DW_DMA_MaskBlock_INT_MASK_WE_DMA1_Msk);
}

static inline void dw_dma_set_masksrctran_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    value |= (value << DW_DMA_MaskSrcTran_INT_MASK_WE_Pos);
    dma->MaskSrcTran |=  value;
}

static inline void dw_dma_reset_masksrctran_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    value = (value << DW_DMA_MaskSrcTran_INT_MASK_WE_Pos);
    dma->MaskSrcTran =  value;
}

static inline uint32_t dw_dma_get_masksrctran_dma0(dw_dma_regs_t *dma)
{
    return (dma->MaskSrcTran & DW_DMA_MaskSrcTran_DMA0_Msk);
}

static inline void dw_dma_set_masksrctran_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    value |= (value << DW_DMA_MaskSrcTran_INT_MASK_WE_Pos);
    dma->MaskSrcTran |=  value;
}

static inline void dw_dma_reset_masksrctran_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    value = (value << DW_DMA_MaskSrcTran_INT_MASK_WE_Pos);
    dma->MaskSrcTran =  value;
}

static inline uint32_t dw_dma_get_masksrctran_dma1(dw_dma_regs_t *dma)
{
    return (dma->MaskSrcTran & DW_DMA_MaskSrcTran_DMA1_Msk);
}

static inline void dw_dma_set_masksrctran_int_mask_we_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->MaskSrcTran = (dma->MaskSrcTran & ~DW_DMA_MaskSrcTran_INT_MASK_WE_DMA0_Msk) | value;
}

static inline uint32_t dw_dma_get_masksrctran_int_mask_we_dma0(dw_dma_regs_t *dma)
{
    return (dma->MaskSrcTran & DW_DMA_MaskSrcTran_INT_MASK_WE_DMA0_Msk);
}

static inline void dw_dma_set_masksrctran_int_mask_we_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->MaskSrcTran = (dma->MaskSrcTran & ~DW_DMA_MaskSrcTran_INT_MASK_WE_DMA1_Msk) | value;
}

static inline uint32_t dw_dma_get_masksrctran_int_mask_we_dma1(dw_dma_regs_t *dma)
{
    return (dma->MaskSrcTran & DW_DMA_MaskSrcTran_INT_MASK_WE_DMA1_Msk);
}

static inline void dw_dma_set_maskdsttran_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    value |= (value << DW_DMA_MaskDstTran_INT_MASK_WE_Pos);
    dma->MaskDstTran |=  value;
}

static inline void dw_dma_reset_maskdsttran_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    value = (value << DW_DMA_MaskDstTran_INT_MASK_WE_Pos);
    dma->MaskDstTran =  value;
}

static inline uint32_t dw_dma_get_maskdsttran_dma0(dw_dma_regs_t *dma)
{
    return (dma->MaskDstTran & DW_DMA_MaskDstTran_DMA0_Msk);
}

static inline void dw_dma_set_maskdsttran_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    value |= (value << DW_DMA_MaskDstTran_INT_MASK_WE_Pos);
    dma->MaskDstTran |=  value;
}

static inline void dw_dma_reset_maskdsttran_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    value = (value << DW_DMA_MaskDstTran_INT_MASK_WE_Pos);
    dma->MaskDstTran =  value;
}

static inline uint32_t dw_dma_get_maskdsttran_dma1(dw_dma_regs_t *dma)
{
    return (dma->MaskDstTran & DW_DMA_MaskDstTran_DMA1_Msk);
}

static inline void dw_dma_set_maskdsttran_int_mask_we_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->MaskDstTran = (dma->MaskDstTran & ~DW_DMA_MaskDstTran_INT_MASK_WE_DMA0_Msk) | value;
}

static inline uint32_t dw_dma_get_maskdsttran_int_mask_we_dma0(dw_dma_regs_t *dma)
{
    return (dma->MaskDstTran & DW_DMA_MaskDstTran_INT_MASK_WE_DMA0_Msk);
}

static inline void dw_dma_set_maskdsttran_int_mask_we_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->MaskDstTran = (dma->MaskDstTran & ~DW_DMA_MaskDstTran_INT_MASK_WE_DMA1_Msk) | value;
}

static inline uint32_t dw_dma_get_maskdsttran_int_mask_we_dma1(dw_dma_regs_t *dma)
{
    return (dma->MaskDstTran & DW_DMA_MaskDstTran_INT_MASK_WE_DMA1_Msk);
}

static inline void dw_dma_set_maskerr_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    value |= (value << DW_DMA_MaskErr_INT_MASK_WE_Pos);
    dma->MaskErr |=  value;
}

static inline void dw_dma_reset_maskerr_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    value = (value << DW_DMA_MaskErr_INT_MASK_WE_Pos);
    dma->MaskErr =  value;
}

static inline uint32_t dw_dma_get_maskerr_dma0(dw_dma_regs_t *dma)
{
    return (dma->MaskErr & DW_DMA_MaskErr_DMA0_Msk);
}

static inline void dw_dma_set_maskerr_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    value |= (value << DW_DMA_MaskErr_INT_MASK_WE_Pos);
    dma->MaskErr |=  value;
}

static inline void dw_dma_reset_maskerr_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    value = (value << DW_DMA_MaskErr_INT_MASK_WE_Pos);
    dma->MaskErr =  value;
}

static inline uint32_t dw_dma_get_maskerr_dma1(dw_dma_regs_t *dma)
{
    return (dma->MaskErr & DW_DMA_MaskErr_DMA1_Msk);
}

static inline void dw_dma_set_maskerr_int_mask_we_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->MaskErr = (dma->MaskErr & ~DW_DMA_MaskErr_INT_MASK_WE_DMA0_Msk) | value;
}

static inline uint32_t dw_dma_get_maskerr_int_mask_we_dma0(dw_dma_regs_t *dma)
{
    return (dma->MaskErr & DW_DMA_MaskErr_INT_MASK_WE_DMA0_Msk);
}

static inline void dw_dma_set_maskerr_int_mask_we_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->MaskErr = (dma->MaskErr & ~DW_DMA_MaskErr_INT_MASK_WE_DMA1_Msk) | value;
}

static inline uint32_t dw_dma_get_maskerr_int_mask_we_dma1(dw_dma_regs_t *dma)
{
    return (dma->MaskErr & DW_DMA_MaskErr_INT_MASK_WE_DMA1_Msk);
}

static inline void dw_dma_set_cleartfr_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->ClearTfr = DW_DMA_ClearTfr_DMA0_Msk & value;
}

static inline uint32_t dw_dma_get_cleartfr_dma0(dw_dma_regs_t *dma)
{
    return (dma->ClearTfr & DW_DMA_ClearTfr_DMA0_Msk);
}

static inline void dw_dma_set_cleartfr_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->ClearTfr  = DW_DMA_ClearTfr_DMA1_Msk & value;
}

static inline uint32_t dw_dma_get_cleartfr_dma1(dw_dma_regs_t *dma)
{
    return (dma->ClearTfr & DW_DMA_ClearTfr_DMA1_Msk);
}

static inline void dw_dma_set_clearblock_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->ClearBlock  = DW_DMA_ClearBlock_DMA0_Msk & value;
}

static inline uint32_t dw_dma_get_clearblock_dma0(dw_dma_regs_t *dma)
{
    return (dma->ClearBlock & DW_DMA_ClearBlock_DMA0_Msk);
}

static inline void dw_dma_set_clearblock_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->ClearBlock  = DW_DMA_ClearBlock_DMA1_Msk & value;
}

static inline uint32_t dw_dma_get_clearblock_dma1(dw_dma_regs_t *dma)
{
    return (dma->ClearBlock & DW_DMA_ClearBlock_DMA1_Msk);
}

static inline void dw_dma_set_clearsrctran_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->ClearSrcTran  = DW_DMA_ClearSrcTran_DMA0_Msk & value;
}

static inline uint32_t dw_dma_get_clearsrctran_dma0(dw_dma_regs_t *dma)
{
    return (dma->ClearSrcTran & DW_DMA_ClearSrcTran_DMA0_Msk);
}

static inline void dw_dma_set_clearsrctran_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->ClearSrcTran = DW_DMA_ClearSrcTran_DMA1_Msk & value;
}

static inline uint32_t dw_dma_get_clearsrctran_dma1(dw_dma_regs_t *dma)
{
    return (dma->ClearSrcTran & DW_DMA_ClearSrcTran_DMA1_Msk);
}

static inline void dw_dma_set_cleardsttran_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->ClearDstTran = DW_DMA_ClearDstTran_DMA0_Msk & value;
}

static inline uint32_t dw_dma_get_cleardsttran_dma0(dw_dma_regs_t *dma)
{
    return (dma->ClearDstTran & DW_DMA_ClearDstTran_DMA0_Msk);
}

static inline void dw_dma_set_cleardsttran_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->ClearDstTran = DW_DMA_ClearDstTran_DMA1_Msk & value;
}

static inline uint32_t dw_dma_get_cleardsttran_dma1(dw_dma_regs_t *dma)
{
    return (dma->ClearDstTran & DW_DMA_ClearDstTran_DMA1_Msk);
}

static inline void dw_dma_set_clearerr_dma0(dw_dma_regs_t *dma, uint32_t value)
{
    dma->ClearErr = DW_DMA_ClearErr_DMA0_Msk & value;
}

static inline uint32_t dw_dma_get_clearerr_dma0(dw_dma_regs_t *dma)
{
    return (dma->ClearErr & DW_DMA_ClearErr_DMA0_Msk);
}

static inline void dw_dma_set_clearerr_dma1(dw_dma_regs_t *dma, uint32_t value)
{
    dma->ClearErr = DW_DMA_ClearErr_DMA1_Msk & value;
}

static inline uint32_t dw_dma_get_clearerr_dma1(dw_dma_regs_t *dma)
{
    return (dma->ClearErr & DW_DMA_ClearErr_DMA1_Msk);
}

static inline void dw_dma_en(dw_dma_regs_t *dma)
{
    dma->DmaCfgReg  |= DW_DMA_DmaCfgReg_DMA_EN_En;
}

static inline void dw_dma_dis(dw_dma_regs_t *dma)
{
    dma->DmaCfgReg &= ~DW_DMA_DmaCfgReg_DMA_EN_En;
}

static inline uint32_t dw_dma_get_dma(dw_dma_regs_t *dma)
{
    return (dma->DmaCfgReg & DW_DMA_DmaCfgReg_DMA_EN_Msk);
}

static inline void dw_dma_set_channel_en(dw_dma_regs_t *dma, uint32_t value)
{
    //calculate we+en
    value |= (value << DW_DMA_ChEnReg_CH_EN_WE_Pos);
    dma->ChEnReg  |=  value;
}

static inline void dw_dma_reset_channel_en(dw_dma_regs_t *dma, uint32_t value)
{
    //calculate we
    value = (value << DW_DMA_ChEnReg_CH_EN_WE_Pos);
    dma->ChEnReg  =  value;
}

static inline uint32_t dw_dma_get_channel_en(dw_dma_regs_t *dma)
{
    return (dma->ChEnReg & DW_DMA_ChEnReg_CH_EN_Msk);
}

static inline void dw_dma_set_channel_en_we(dw_dma_regs_t *dma, uint32_t value)
{
    dma->ChEnReg  = value;
}

static inline uint32_t dw_dma_get_channel_en_we(dw_dma_regs_t *dma)
{
    return (dma->ChEnReg & DW_DMA_ChEnReg_CH_EN_WE_Msk);
}
#ifdef __cplusplus
}
#endif
#endif /* _DW_DMA_LL_H_ */