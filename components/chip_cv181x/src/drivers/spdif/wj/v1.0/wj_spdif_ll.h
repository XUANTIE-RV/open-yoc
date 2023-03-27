/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_spdif_ll.h
 * @brief    header file for spdif ll driver
 * @version  V1.0
 * @date     21. Oct 2020
 ******************************************************************************/

#ifndef _SPDIF_LL_H
#define _SPDIF_LL_H

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
* SPDIF register bit definitions
*/

/* SPDIF_EN_REG, offset: 0x00 */
#define WJ_SPDIF_EN_REG_Pos                     (0U)
#define WJ_SPDIF_EN_REG_Msk                     (0x1U << WJ_SPDIF_EN_REG_Pos)
#define WJ_SPDIF_EN                             WJ_SPDIF_EN_REG_Msk

/* TX_EN_REG, offset: 0x04 */
#define WJ_SPDIF_TX_EN_REG_TX_EN_Pos            (0U)
#define WJ_SPDIF_TX_EN_REG_TX_EN_Msk            (0x1U << WJ_SPDIF_TX_EN_REG_TX_EN_Pos)
#define WJ_SPDIF_TX_EN_REG_TX_EN                WJ_SPDIF_TX_EN_REG_TX_EN_Msk

/* TX_CTL_REG, offset: 0x08 */
#define WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_Pos     (0U)
#define WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_Msk     (0x3U << WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_Pos)
#define WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_24BITS  (0x0U << WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_Pos)
#define WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_20BITS  (0x1U << WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_Pos)
#define WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_16BITS  (0x2U << WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_Pos)
#define WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_16BITS_H (0x3U << WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_Pos)

#define WJ_SPDIF_TX_CTL_REG_TX_CH_SEL_Pos       (4U)
#define WJ_SPDIF_TX_CTL_REG_TX_CH_SEL_Msk       (0x1U << WJ_SPDIF_TX_CTL_REG_TX_CH_SEL_Pos)
#define WJ_SPDIF_TX_CTL_REG_TX_CH_SEL_2_CH      (0x0U << WJ_SPDIF_TX_CTL_REG_TX_CH_SEL_Pos)
#define WJ_SPDIF_TX_CTL_REG_TX_CH_SEL_1_CH      (0x1U << WJ_SPDIF_TX_CTL_REG_TX_CH_SEL_Pos)

#define WJ_SPDIF_TX_CTL_REG_TX_DIV_BYPASS_Pos   (8U)
#define WJ_SPDIF_TX_CTL_REG_TX_DIV_BYPASS_Msk   (0x1U << WJ_SPDIF_TX_CTL_REG_TX_DIV_BYPASS_Pos)
#define WJ_SPDIF_TX_CTL_REG_TX_DIV_NOT_BYPASS   WJ_SPDIF_TX_CTL_REG_TX_DIV_BYPASS_Msk

#define WJ_SPDIF_TX_CTL_REGTX_DIV_Pos           (9U)
#define WJ_SPDIF_TX_CTL_REGTX_DIV_Msk           (0x7FU << WJ_SPDIF_TX_CTL_REGTX_DIV_Pos)

/* TX_CS_REG, offset: 0x0C */
#define WJ_SPDIF_TX_CS_REG_T_PRO_EN_Pos         (0U)
#define WJ_SPDIF_TX_CS_REG_T_PRO_EN_Msk         (0x1U << WJ_SPDIF_TX_CS_REG_T_PRO_EN_Pos)
#define WJ_SPDIF_TX_CS_REG_T_PRO_EN             WJ_SPDIF_TX_CS_REG_T_PRO_EN_Msk

#define WJ_SPDIF_TX_CS_REG_T_AUIDO_EN_Pos       (1U)
#define WJ_SPDIF_TX_CS_REG_T_AUIDO_EN_Msk       (0x1U << WJ_SPDIF_TX_CS_REG_T_AUIDO_EN_Pos)
#define WJ_SPDIF_TX_CS_REG_T_AUIDO_EN           WJ_SPDIF_TX_CS_REG_T_AUIDO_EN_Msk

#define WJ_SPDIF_TX_CS_REG_T_COPY_Pos           (2U)
#define WJ_SPDIF_TX_CS_REG_T_COPY_Msk           (0x1U << WJ_SPDIF_TX_CS_REG_T_COPY_Pos)
#define WJ_SPDIF_TX_CS_REG_T_COPY               WJ_SPDIF_TX_CS_REG_T_COPY_Msk

#define WJ_SPDIF_TX_CS_REG_T_EMP_MODE_Pos       (3U)
#define WJ_SPDIF_TX_CS_REG_T_EMP_MODE_Msk       (0x1U << WJ_SPDIF_TX_CS_REG_T_EMP_MODE_Pos)
#define WJ_SPDIF_TX_CS_REG_T_EMP_MODE           WJ_SPDIF_TX_CS_REG_T_EMP_MODE_Msk

#define WJ_SPDIF_TX_CS_REG_T_MODE_Pos           (6U)
#define WJ_SPDIF_TX_CS_REG_T_MODE_Msk           (0x3U << WJ_SPDIF_TX_CS_REG_T_MODE_Pos)
#define WJ_SPDIF_TX_CS_REG_T_MODE               WJ_SPDIF_TX_CS_REG_T_MODE_Msk

#define WJ_SPDIF_TX_CS_REG_T_CC_SEL_Pos         (8U)
#define WJ_SPDIF_TX_CS_REG_T_CC_SEL_Msk         (0xFFU << WJ_SPDIF_TX_CS_REG_T_CC_SEL_Pos)
#define WJ_SPDIF_TX_CS_REG_T_CC_SEL             WJ_SPDIF_TX_CS_REG_T_CC_SEL_Msk

#define WJ_SPDIF_TX_CS_REG_T_SRC_NUM_Pos        (16U)
#define WJ_SPDIF_TX_CS_REG_T_SRC_NUM_Msk        (0xFU << WJ_SPDIF_TX_CS_REG_T_SRC_NUM_Pos)
#define WJ_SPDIF_TX_CS_REG_T_SRC_NUM            WJ_SPDIF_TX_CS_REG_T_SRC_NUM_Msk

#define WJ_SPDIF_TX_CS_REG_T_CH_NUM_Pos         (20U)
#define WJ_SPDIF_TX_CS_REG_T_CH_NUM_Msk         (0xFU << WJ_SPDIF_TX_CS_REG_T_CH_NUM_Pos)
#define WJ_SPDIF_TX_CS_REG_T_CH_NUM             WJ_SPDIF_TX_CS_REG_T_CH_NUM_Msk

#define WJ_SPDIF_TX_CS_REG_T_FS_SEL_Pos         (24U)
#define WJ_SPDIF_TX_CS_REG_T_FS_SEL_Msk         (0xFU << WJ_SPDIF_TX_CS_REG_T_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_REG_T_FS_SEL_32KHZ       (0x3U << WJ_SPDIF_TX_CS_REG_T_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_REG_T_FS_SEL_44_1KHZ     (0x0U << WJ_SPDIF_TX_CS_REG_T_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_REG_T_FS_SEL_48KHZ       (0x2U << WJ_SPDIF_TX_CS_REG_T_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_REG_T_FS_SEL_88_2KHZ     (0x8U << WJ_SPDIF_TX_CS_REG_T_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_REG_T_FS_SEL_96KHZ       (0xAU << WJ_SPDIF_TX_CS_REG_T_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_REG_T_FS_SEL_192KHZ      (0xEU << WJ_SPDIF_TX_CS_REG_T_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_REG_T_FS_SEL_DEFAULT     (0x1U << WJ_SPDIF_TX_CS_REG_T_FS_SEL_Pos)

#define WJ_SPDIF_TX_CS_REG_T_CLK_ACC_Pos        (28U)
#define WJ_SPDIF_TX_CS_REG_T_CLK_ACC_Msk        (0x3U << WJ_SPDIF_TX_CS_REG_T_CLK_ACC_Pos)
#define WJ_SPDIF_TX_CS_REG_T_CLK_ACC            WJ_SPDIF_TX_CS_REG_T_CLK_ACC_Msk

/* TX_FIFO_TH, offset: 0x2C */
#define WJ_SPDIF_ENTX_FIFO_TH_Pos               (0U)
#define WJ_SPDIF_ENTX_FIFO_TH_Msk               (0xFU << WJ_SPDIF_ENTX_FIFO_TH_Pos)
#define WJ_SPDIF_ENTX_FIFO_TH                   WJ_SPDIF_ENTX_FIFO_TH_Msk

/* TX_FIFO_DL, offset: 0x30 */
#define WJ_SPDIF_ENTX_FIFO_DL_Pos               (0U)
#define WJ_SPDIF_ENTX_FIFO_DL_Msk               (0x1FU << WJ_SPDIF_ENTX_FIFO_DL_Pos)
#define WJ_SPDIF_ENTX_FIFO_DL                   WJ_SPDIF_ENTX_FIFO_DL_Msk

/* TX_DMA_EN, offset： 0x34 */
#define WJ_SPDIF_TX_DMA_EN_Pos                  (0U)
#define WJ_SPDIF_TX_DMA_EN_Msk                  (0x1U << WJ_SPDIF_TX_DMA_EN_Pos)
#define WJ_SPDIF_TX_DMA_EN                      WJ_SPDIF_TX_DMA_EN_Msk

/* TX_DMA_TH, offset： 0x38 */
#define WJ_SPDIF_TX_DMA_TH_Pos                  (0U)
#define WJ_SPDIF_TX_DMA_TH_Msk                  (0xFU << WJ_SPDIF_TX_DMA_TH_Pos)
#define WJ_SPDIF_TX_DMA_TH                      WJ_SPDIF_TX_DMA_TH_Msk

/* SPDIF_SR, offset: 0x40*/
#define WJ_SPDIF_SPDIF_SR_TX_BUSY_Pos           (0U)
#define WJ_SPDIF_SPDIF_SR_TX_BUSY_Msk           (0x1U << WJ_SPDIF_SPDIF_SR_TX_BUSY_Pos)
#define WJ_SPDIF_SPDIF_SR_TX_BUSY               WJ_SPDIF_SPDIF_SR_TX_BUSY_Msk

#define WJ_SPDIF_SPDIF_SR_TF_NF_Pos             (1U)
#define WJ_SPDIF_SPDIF_SR_TF_NF_Msk             (0x1U << WJ_SPDIF_SPDIF_SR_TF_NF_Pos)
#define WJ_SPDIF_SPDIF_SR_TF_NF                 WJ_SPDIF_SPDIF_SR_TF_NF_Msk

#define WJ_SPDIF_SPDIF_SR_TF_E_Pos              (2U)
#define WJ_SPDIF_SPDIF_SR_TF_E_Msk              (0x1U << WJ_SPDIF_SPDIF_SR_TF_E_Pos)
#define WJ_SPDIF_SPDIF_SR_TF_E                  WJ_SPDIF_SPDIF_SR_TF_E_Msk

#define WJ_SPDIF_SPDIF_SR_RX_BUSY_Pos           (4U)
#define WJ_SPDIF_SPDIF_SR_RX_BUSY_Msk           (0x1U << WJ_SPDIF_SPDIF_SR_RX_BUSY_Pos)
#define WJ_SPDIF_SPDIF_SR_RX_BUSY               WJ_SPDIF_SPDIF_SR_RX_BUSY_Msk

#define WJ_SPDIF_SPDIF_SR_RF_NE_Pos             (5U)
#define WJ_SPDIF_SPDIF_SR_RF_NE_Msk             (0x1U << WJ_SPDIF_SPDIF_SR_RF_NE_Pos)
#define WJ_SPDIF_SPDIF_SR_RF_NE                 WJ_SPDIF_SPDIF_SR_RF_NE_Msk

#define WJ_SPDIF_SPDIF_SR_RF_F_Pos              (6U)
#define WJ_SPDIF_SPDIF_SR_RF_F_Msk              (0x1U << WJ_SPDIF_SPDIF_SR_RF_F_Pos)
#define WJ_SPDIF_SPDIF_SR_RF_F                  WJ_SPDIF_SPDIF_SR_RF_F_Msk

#define WJ_SPDIF_SPDIF_RX_CHNUM_Pos             (7U)
#define WJ_SPDIF_SPDIF_RX_CHNUM_Msk             (0x1U << WJ_SPDIF_SPDIF_RX_CHNUM_Pos)
#define WJ_SPDIF_SPDIF_RX_CHNUM                 WJ_SPDIF_SPDIF_RX_CHNUM_Msk

/* SPDIF_IMR, offset: 0x40*/
#define WJ_SPDIF_IMR_WADDR_EM_Pos                   (0U)
#define WJ_SPDIF_IMR_WADDR_EM_Msk                   (0x1U << WJ_SPDIF_IMR_WADDR_EM_Pos)
#define WJ_SPDIF_IMR_WADDR_EM                       WJ_SPDIF_IMR_WADDR_EM_Msk

#define WJ_SPDIF_IMR_ACCESS_EM_Pos                  (1U)
#define WJ_SPDIF_IMR_ACCESS_EM_Msk                  (0x1U << WJ_SPDIF_IMR_ACCESS_EM_Pos)
#define WJ_SPDIF_IMR_ACCESS_EM                      WJ_SPDIF_IMR_ACCESS_EM_Msk

#define WJ_SPDIF_IMR_TF_O_EM_Pos                    (4U)
#define WJ_SPDIF_IMR_TF_O_EM_Msk                    (0x1U << WJ_SPDIF_IMR_TF_O_EM_Pos)
#define WJ_SPDIF_IMR_TF_O_EM                        WJ_SPDIF_IMR_TF_O_EM_Msk

#define WJ_SPDIF_IMR_TF_U_EM_Pos                    (5U)
#define WJ_SPDIF_IMR_TF_U_EM_Msk                    (0x1U << WJ_SPDIF_IMR_TF_U_EM_Pos)
#define WJ_SPDIF_IMR_TF_U_EM                        WJ_SPDIF_IMR_TF_U_EM_Msk

#define WJ_SPDIF_IMR_TF_TE_IM_Pos                   (6U)
#define WJ_SPDIF_IMR_TF_TE_IM_Msk                   (0x1U << WJ_SPDIF_IMR_TF_TE_IM_Pos)
#define WJ_SPDIF_IMR_TF_TE_IM                       WJ_SPDIF_IMR_TF_TE_IM_Msk

#define WJ_SPDIF_IMR_TX_DONE_IM_Pos                 (7U)
#define WJ_SPDIF_IMR_TX_DONE_IM_Msk                 (0x1U << WJ_SPDIF_IMR_TX_DONE_IM_Pos)
#define WJ_SPDIF_IMR_TX_DONE_IM                     WJ_SPDIF_IMR_TX_DONE_IM_Msk

#define WJ_SPDIF_IMR_RF_U_EM_Pos                    (8U)
#define WJ_SPDIF_IMR_RF_U_EM_Msk                    (0x1U << WJ_SPDIF_IMR_RF_U_EM_Pos)
#define WJ_SPDIF_IMR_RF_U_EM                        WJ_SPDIF_IMR_RF_U_EM_Msk

#define WJ_SPDIF_IMR_RF_O_EM_Pos                    (9U)
#define WJ_SPDIF_IMR_RF_O_EM_MsK                    (0x1U << WJ_SPDIF_IMR_RF_O_EM_Pos)
#define WJ_SPDIF_IMR_RF_O_EM                        WJ_SPDIF_IMR_RF_O_EM_MsK

#define WJ_SPDIF_IMR_RF_TF_IM_Pos                   (10U)
#define WJ_SPDIF_IMR_RF_TF_IM_Msk                   (0x1U << WJ_SPDIF_IMR_RF_TF_IM_Pos)
#define WJ_SPDIF_IMR_RF_TF_IM                       WJ_SPDIF_IMR_RF_TF_IM_Msk

#define WJ_SPDIF_IMR_RPC_EM_Pos                     (11U)
#define WJ_SPDIF_IMR_RPC_EM_Msk                     (0x1U << WJ_SPDIF_IMR_RPC_EM_Pos)
#define WJ_SPDIF_IMR_RPC_EM                         WJ_SPDIF_IMR_RPC_EM_Msk

#define WJ_SPDIF_IMR_RCLK_EM_Pos                    (12U)
#define WJ_SPDIF_IMR_RCLK_EM_Msk                    (0x1U << WJ_SPDIF_IMR_RCLK_EM_Pos)
#define WJ_SPDIF_IMR_RCLK_EM                        WJ_SPDIF_IMR_RCLK_EM_Msk

#define WJ_SPDIF_IMR_RPB_EM_Pos                     (13U)
#define WJ_SPDIF_IMR_RPB_EM_Msk                     (0x1U << WJ_SPDIF_IMR_RPB_EM_Pos)
#define WJ_SPDIF_IMR_RPB_EM                         WJ_SPDIF_IMR_RPB_EM_Msk

/* SPDIF_ISR, offset: 0x44*/
#define WJ_SPDIF_ISR_WADDR_EM_Pos                   (0U)
#define WJ_SPDIF_ISR_WADDR_EM_Msk                   (0x1U << WJ_SPDIF_ISR_WADDR_EM_Pos)
#define WJ_SPDIF_ISR_WADDR_EM                       WJ_SPDIF_ISR_WADDR_EM_Msk

#define WJ_SPDIF_ISR_ACCESS_EM_Pos                  (1U)
#define WJ_SPDIF_ISR_ACCESS_EM_Msk                  (0x1U << WJ_SPDIF_ISR_ACCESS_EM_Pos)
#define WJ_SPDIF_ISR_ACCESS_EM                      WJ_SPDIF_ISR_ACCESS_EM_Msk

#define WJ_SPDIF_ISR_TF_O_EM_Pos                    (4U)
#define WJ_SPDIF_ISR_TF_O_EM_Msk                    (0x1U << WJ_SPDIF_ISR_TF_O_EM_Pos)
#define WJ_SPDIF_ISR_TF_O_EM                        WJ_SPDIF_ISR_TF_O_EM_Msk

#define WJ_SPDIF_ISR_TF_U_EM_Pos                    (5U)
#define WJ_SPDIF_ISR_TF_U_EM_Msk                    (0x1U << WJ_SPDIF_ISR_TF_U_EM_Pos)
#define WJ_SPDIF_ISR_TF_U_EM                        WJ_SPDIF_ISR_TF_U_EM_Msk

#define WJ_SPDIF_ISR_TF_TE_IM_Pos                   (6U)
#define WJ_SPDIF_ISR_TF_TE_IM_Msk                   (0x1U << WJ_SPDIF_ISR_TF_TE_IM_Pos)
#define WJ_SPDIF_ISR_TF_TE_IM                       WJ_SPDIF_ISR_TF_TE_IM_Msk

#define WJ_SPDIF_ISR_TX_DONE_IM_Pos                 (7U)
#define WJ_SPDIF_ISR_TX_DONE_IM_Msk                 (0x1U << WJ_SPDIF_ISR_TX_DONE_IM_Pos)
#define WJ_SPDIF_ISR_TX_DONE_IM                     WJ_SPDIF_ISR_TX_DONE_IM_Msk

#define WJ_SPDIF_ISR_RF_U_EM_Pos                    (8U)
#define WJ_SPDIF_ISR_RF_U_EM_Msk                    (0x1U << WJ_SPDIF_ISR_RF_U_EM_Pos)
#define WJ_SPDIF_ISR_RF_U_EM                        WJ_SPDIF_ISR_RF_U_EM_Msk

#define WJ_SPDIF_ISR_RF_O_EM_Pos                    (9U)
#define WJ_SPDIF_ISR_RF_O_EM_MsK                    (0x1U << WJ_SPDIF_ISR_RF_O_EM_Pos)
#define WJ_SPDIF_ISR_RF_O_EM                        WJ_SPDIF_ISR_RF_O_EM_MsK

#define WJ_SPDIF_ISR_RF_TF_IM_Pos                   (10U)
#define WJ_SPDIF_ISR_RF_TF_IM_Msk                   (0x1U << WJ_SPDIF_ISR_RF_TF_IM_Pos)
#define WJ_SPDIF_ISR_RF_TF_IM                       WJ_SPDIF_ISR_RF_TF_IM_Msk

#define WJ_SPDIF_ISR_RPC_EM_Pos                     (11U)
#define WJ_SPDIF_ISR_RPC_EM_Msk                     (0x1U << WJ_SPDIF_ISR_RPC_EM_Pos)
#define WJ_SPDIF_ISR_RPC_EM                         WJ_SPDIF_ISR_RPC_EM_Msk

#define WJ_SPDIF_ISR_RCLK_EM_Pos                    (12U)
#define WJ_SPDIF_ISR_RCLK_EM_Msk                    (0x1U << WJ_SPDIF_ISR_RCLK_EM_Pos)
#define WJ_SPDIF_ISR_RCLK_EM                        WJ_SPDIF_ISR_RCLK_EM_Msk

#define WJ_SPDIF_ISR_RPB_EM_Pos                     (13U)
#define WJ_SPDIF_ISR_RPB_EM_Msk                     (0x1U << WJ_SPDIF_ISR_RPB_EM_Pos)
#define WJ_SPDIF_ISR_RPB_EM                         WJ_SPDIF_ISR_RPB_EM_Msk

/* SPDIF_RISR, offset: 0x44*/
#define WJ_SPDIF_RISR_WADDR_EM_Pos                   (0U)
#define WJ_SPDIF_RISR_WADDR_EM_Msk                   (0x1U << WJ_SPDIF_RISR_WADDR_EM_Pos)
#define WJ_SPDIF_RISR_WADDR_EM                       WJ_SPDIF_RISR_WADDR_EM_Msk

#define WJ_SPDIF_RISR_ACCESS_EM_Pos                  (1U)
#define WJ_SPDIF_RISR_ACCESS_EM_Msk                  (0x1U << WJ_SPDIF_RISR_ACCESS_EM_Pos)
#define WJ_SPDIF_RISR_ACCESS_EM                      WJ_SPDIF_RISR_ACCESS_EM_Msk

#define WJ_SPDIF_RISR_TF_O_EM_Pos                    (4U)
#define WJ_SPDIF_RISR_TF_O_EM_Msk                    (0x1U << WJ_SPDIF_RISR_TF_O_EM_Pos)
#define WJ_SPDIF_RISR_TF_O_EM                        WJ_SPDIF_RISR_TF_O_EM_Msk

#define WJ_SPDIF_RISR_TF_U_EM_Pos                    (5U)
#define WJ_SPDIF_RISR_TF_U_EM_Msk                    (0x1U << WJ_SPDIF_RISR_TF_U_EM_Pos)
#define WJ_SPDIF_RISR_TF_U_EM                        WJ_SPDIF_RISR_TF_U_EM_Msk

#define WJ_SPDIF_RISR_TF_TE_IM_Pos                   (6U)
#define WJ_SPDIF_RISR_TF_TE_IM_Msk                   (0x1U << WJ_SPDIF_RISR_TF_TE_IM_Pos)
#define WJ_SPDIF_RISR_TF_TE_IM                       WJ_SPDIF_RISR_TF_TE_IM_Msk

#define WJ_SPDIF_RISR_TX_DONE_IM_Pos                 (7U)
#define WJ_SPDIF_RISR_TX_DONE_IM_Msk                 (0x1U << WJ_SPDIF_RISR_TX_DONE_IM_Pos)
#define WJ_SPDIF_RISR_TX_DONE_IM                     WJ_SPDIF_RISR_TX_DONE_IM_Msk

#define WJ_SPDIF_RISR_RF_U_EM_Pos                    (8U)
#define WJ_SPDIF_RISR_RF_U_EM_Msk                    (0x1U << WJ_SPDIF_RISR_RF_U_EM_Pos)
#define WJ_SPDIF_RISR_RF_U_EM                        WJ_SPDIF_RISR_RF_U_EM_Msk

#define WJ_SPDIF_RISR_RF_O_EM_Pos                    (9U)
#define WJ_SPDIF_RISR_RF_O_EM_MsK                    (0x1U << WJ_SPDIF_RISR_RF_O_EM_Pos)
#define WJ_SPDIF_RISR_RF_O_EM                        WJ_SPDIF_RISR_RF_O_EM_MsK

#define WJ_SPDIF_RISR_RF_TF_IM_Pos                   (10U)
#define WJ_SPDIF_RISR_RF_TF_IM_Msk                   (0x1U << WJ_SPDIF_RISR_RF_TF_IM_Pos)
#define WJ_SPDIF_RISR_RF_TF_IM                       WJ_SPDIF_RISR_RF_TF_IM_Msk

#define WJ_SPDIF_RISR_RPC_EM_Pos                     (11U)
#define WJ_SPDIF_RISR_RPC_EM_Msk                     (0x1U << WJ_SPDIF_RISR_RPC_EM_Pos)
#define WJ_SPDIF_RISR_RPC_EM                         WJ_SPDIF_RISR_RPC_EM_Msk

#define WJ_SPDIF_RISR_RCLK_EM_Pos                    (12U)
#define WJ_SPDIF_RISR_RCLK_EM_Msk                    (0x1U << WJ_SPDIF_RISR_RCLK_EM_Pos)
#define WJ_SPDIF_RISR_RCLK_EM                        WJ_SPDIF_RISR_RCLK_EM_Msk

#define WJ_SPDIF_RISR_RPB_EM_Pos                     (13U)
#define WJ_SPDIF_RISR_RPB_EM_Msk                     (0x1U << WJ_SPDIF_RISR_RPB_EM_Pos)
#define WJ_SPDIF_RISR_RPB_EM                         WJ_SPDIF_RISR_RPB_EM_Msk

/* SPDIF_ICR, offset: 0x4c*/
#define WJ_SPDIF_ICR_WADDR_EC_Pos                   (0U)
#define WJ_SPDIF_ICR_WADDR_EC_Msk                   (0x1U << WJ_SPDIF_ICR_WADDR_EC_Pos)
#define WJ_SPDIF_ICR_WADDR_EC                       WJ_SPDIF_ICR_WADDR_EC_Msk

#define WJ_SPDIF_ICR_ACCESS_EC_Pos                  (1U)
#define WJ_SPDIF_ICR_ACCESS_EC_Msk                  (0x1U << WJ_SPDIF_ICR_ACCESS_EC_Pos)
#define WJ_SPDIF_ICR_ACCESS_EC                      WJ_SPDIF_ICR_ACCESS_EC_Msk

#define WJ_SPDIF_ICR_TF_O_EC_Pos                    (4U)
#define WJ_SPDIF_ICR_TF_O_EC_Msk                    (0x1U << WJ_SPDIF_ICR_TF_O_EC_Pos)
#define WJ_SPDIF_ICR_TF_O_EC                        WJ_SPDIF_ICR_TF_O_EC_Msk

#define WJ_SPDIF_ICR_TF_U_EC_Pos                    (5U)
#define WJ_SPDIF_ICR_TF_U_EC_Msk                    (0x1U << WJ_SPDIF_ICR_TF_U_EC_Pos)
#define WJ_SPDIF_ICR_TF_U_EC                        WJ_SPDIF_ICR_TF_U_EC_Msk

#define WJ_SPDIF_ICR_TF_TE_IC_Pos                   (6U)
#define WJ_SPDIF_ICR_TF_TE_IC_Msk                   (0x1U << WJ_SPDIF_ICR_TF_TE_IC_Pos)
#define WJ_SPDIF_ICR_TF_TE_IC                       WJ_SPDIF_ICR_TF_TE_IC_Msk

#define WJ_SPDIF_ICR_TX_DONE_IC_Pos                 (7U)
#define WJ_SPDIF_ICR_TX_DONE_IC_Msk                 (0x1U << WJ_SPDIF_ICR_TX_DONE_IC_Pos)
#define WJ_SPDIF_ICR_TX_DONE_IC                     WJ_SPDIF_ICR_TX_DONE_IC_Msk

#define WJ_SPDIF_ICR_RF_U_EC_Pos                    (8U)
#define WJ_SPDIF_ICR_RF_U_EC_Msk                    (0x1U << WJ_SPDIF_ICR_RF_U_EC_Pos)
#define WJ_SPDIF_ICR_RF_U_EC                        WJ_SPDIF_ICR_RF_U_EC_Msk

#define WJ_SPDIF_ICR_RF_O_EC_Pos                    (9U)
#define WJ_SPDIF_ICR_RF_O_EC_Msk                    (0x1U << WJ_SPDIF_ICR_RF_O_EC_Pos)
#define WJ_SPDIF_ICR_RF_O_EC                        WJ_SPDIF_ICR_RF_O_EC_Msk

#define WJ_SPDIF_ICR_RF_TF_IC_Pos                   (10U)
#define WJ_SPDIF_ICR_RF_TF_IC_Msk                   (0x1U << WJ_SPDIF_ICR_RF_TF_IC_Pos)
#define WJ_SPDIF_ICR_RF_TF_IC                       WJ_SPDIF_ICR_RF_TF_IC_Msk

#define WJ_SPDIF_ICR_RPC_EC_Pos                     (11U)
#define WJ_SPDIF_ICR_RPC_EC_Msk                     (0x1U << WJ_SPDIF_ICR_RPC_EC_Pos)
#define WJ_SPDIF_ICR_RPC_EC                         WJ_SPDIF_ICR_RPC_EC_Msk

#define WJ_SPDIF_ICR_RCLK_EC_Pos                    (12U)
#define WJ_SPDIF_ICR_RCLK_EC_Msk                    (0x1U << WJ_SPDIF_ICR_RCLK_EC_Pos)
#define WJ_SPDIF_ICR_RCLK_EC                        WJ_SPDIF_ICR_RCLK_EC_Msk

#define WJ_SPDIF_ICR_RPB_EC_Pos                     (13U)
#define WJ_SPDIF_ICR_RPB_EC_Msk                     (0x1U << WJ_SPDIF_ICR_RPB_EC_Pos)
#define WJ_SPDIF_ICR_RPB_EC                         WJ_SPDIF_ICR_RPB_EC_Msk

/* RX_EN_REG, offset: 0x50 */
#define WJ_SPDIF_RX_EN_Pos                          (0U)
#define WJ_SPDIF_RX_EN_Msk                          (0x1U << WJ_SPDIF_RX_EN_Pos)
#define WJ_SPDIF_RX_EN                              WJ_SPDIF_RX_EN_Msk

/* RX_CTL_REG, offset: 0X54 */
#define WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_Pos         (0U)
#define WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_Msk         (0x3U << WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_Pos)
#define WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_16BITS      (0x2U << WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_Pos)
#define WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_20BITS      (0x1U << WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_Pos)
#define WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_24BITS      (0x0U << WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_Pos)

#define WJ_SPDIF_RX_CTL_REG_RX_PARITY_Pos           (4U)
#define WJ_SPDIF_RX_CTL_REG_RX_PARITY_Msk           (0x1U << WJ_SPDIF_RX_CTL_REG_RX_PARITY_Pos)
#define WJ_SPDIF_RX_CTL_REG_RX_PARITY_EN            WJ_SPDIF_RX_CTL_REG_RX_PARITY_Msk

#define WJ_SPDIF_RX_CTL_REG_RX_VALID_EN_Pos         (8U)
#define WJ_SPDIF_RX_CTL_REG_RX_VALID_EN_Msk         (0x1U << WJ_SPDIF_RX_CTL_REG_RX_VALID_EN_Pos)
#define WJ_SPDIF_RX_CTL_REG_RX_VALID_EN             WJ_SPDIF_RX_CTL_REG_RX_VALID_EN_Msk

#define WJ_SPDIF_RX_CTL_REG_RX_DIV_BYPASS_Pos       (12U)
#define WJ_SPDIF_RX_CTL_REG_RX_DIV_BYPASS_Msk       (0x1U << WJ_SPDIF_RX_CTL_REG_RX_DIV_BYPASS_Pos)
#define WJ_SPDIF_RX_CTL_REG_RX_DIV_NOT_BYPASS       WJ_SPDIF_RX_CTL_REG_RX_DIV_BYPASS_Msk

#define WJ_SPDIF_RX_CTL_REG_RX_DIV_Pos              (13U)
#define WJ_SPDIF_RX_CTL_REG_RX_DIV_Msk              (0x7fU << WJ_SPDIF_RX_CTL_REG_RX_DIV_Pos)
#define WJ_SPDIF_RX_CTL_REG_RX_DIV                  WJ_SPDIF_RX_CTL_REG_RX_DIV_Msk

/* RX_FIFO_TH, offset: 0x78 */
#define WJ_SPDIF_RX_FIFO_TH_Pos                     (0U)
#define WJ_SPDIF_RX_FIFO_TH_Msk                     (0xFU << WJ_SPDIF_RX_FIFO_TH_Pos)
#define WJ_SPDIF_RX_FIFO_TH                         WJ_SPDIF_RX_FIFO_TH_Msk

/* RX_FIFO_DL, offset: 0x7C */
#define WJ_SPDIF_RX_FIFO_DL_Pos                     (0U)
#define WJ_SPDIF_RX_FIFO_DL_Msk                     (0x1FU << WJ_SPDIF_RX_FIFO_DL_Pos)
#define WJ_SPDIF_RX_FIFO_DL                         WJ_SPDIF_RX_FIFO_DL_Msk

/* RX_DMA_EN, offset: 0x80 */
#define WJ_SPDIF_RDMA_EN_Pos                        (0U)
#define WJ_SPDIF_RDMA_EN_Msk                        (0x1U << WJ_SPDIF_RDMA_EN_Pos)
#define WJ_SPDIF_RDMA_EN                            WJ_SPDIF_RDMA_EN_Msk

/* RX_DMA_TH, offset: 0x84 */
#define WJ_SPDIF_RDMA_TH_Pos                        (0U)
#define WJ_SPDIF_RDMA_TH_Msk                        (0xFU << WJ_SPDIF_RDMA_TH_Pos)
#define WJ_SPDIF_RDMA_TH                            WJ_SPDIF_RDMA_TH_Msk

/* TX_CS_B_REG, offset: 0x0C */
#define WJ_SPDIF_TX_CS_B_REG_T_B_PRO_EN_Pos         (0U)
#define WJ_SPDIF_TX_CS_B_REG_T_B_PRO_EN_Msk         (0x1U << WJ_SPDIF_TX_CS_B_REG_T_B_PRO_EN_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_PRO_EN             WJ_SPDIF_TX_CS_B_REG_T_B_PRO_EN_Msk

#define WJ_SPDIF_TX_CS_B_REG_T_B_AUIDO_EN_Pos       (1U)
#define WJ_SPDIF_TX_CS_B_REG_T_B_AUIDO_EN_Msk       (0x1U << WJ_SPDIF_TX_CS_B_REG_T_B_AUIDO_EN_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_AUIDO_EN           WJ_SPDIF_TX_CS_B_REG_T_B_AUIDO_EN_Msk

#define WJ_SPDIF_TX_CS_B_REG_T_B_COPY_Pos           (2U)
#define WJ_SPDIF_TX_CS_B_REG_T_B_COPY_Msk           (0x1U << WJ_SPDIF_TX_CS_B_REG_T_B_COPY_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_COPY               WJ_SPDIF_TX_CS_B_REG_T_B_COPY_Msk

#define WJ_SPDIF_TX_CS_B_REG_T_B_EMP_MODE_Pos       (3U)
#define WJ_SPDIF_TX_CS_B_REG_T_B_EMP_MODE_Msk       (0x1U << WJ_SPDIF_TX_CS_B_REG_T_B_EMP_MODE_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_EMP_MODE           WJ_SPDIF_TX_CS_B_REG_T_B_EMP_MODE_Msk

#define WJ_SPDIF_TX_CS_B_REG_T_B_MODE_Pos           (6U)
#define WJ_SPDIF_TX_CS_B_REG_T_B_MODE_Msk           (0x3U << WJ_SPDIF_TX_CS_B_REG_T_B_MODE_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_MODE               WJ_SPDIF_TX_CS_B_REG_T_B_MODE_Msk

#define WJ_SPDIF_TX_CS_B_REG_T_B_CC_SEL_Pos         (8U)
#define WJ_SPDIF_TX_CS_B_REG_T_B_CC_SEL_Msk         (0xFFU << WJ_SPDIF_TX_CS_B_REG_T_B_CC_SEL_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_CC_SEL             WJ_SPDIF_TX_CS_B_REG_T_B_CC_SEL_Msk

#define WJ_SPDIF_TX_CS_B_REG_T_B_SRC_NUM_Pos        (16U)
#define WJ_SPDIF_TX_CS_B_REG_T_B_SRC_NUM_Msk        (0xFU << WJ_SPDIF_TX_CS_B_REG_T_B_SRC_NUM_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_SRC_NUM            WJ_SPDIF_TX_CS_B_REG_T_B_SRC_NUM_Msk

#define WJ_SPDIF_TX_CS_B_REG_T_B_CH_NUM_Pos         (20U)
#define WJ_SPDIF_TX_CS_B_REG_T_B_CH_NUM_Msk         (0xFU << WJ_SPDIF_TX_CS_B_REG_T_B_CH_NUM_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_CH_NUM             WJ_SPDIF_TX_CS_B_REG_T_B_CH_NUM_Msk

#define WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Pos         (24U)
#define WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Msk         (0xFU << WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_32KHZ       (0x3U << WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_44_1KHZ     (0x0U << WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_48KHZ       (0x2U << WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_88_2KHZ     (0x8U << WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_96KHZ       (0xAU << WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_192KHZ      (0xEU << WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_DEFAULT     (0x1U << WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Pos)

#define WJ_SPDIF_TX_CS_B_REG_T_B_CLK_ACC_Pos        (28U)
#define WJ_SPDIF_TX_CS_B_REG_T_B_CLK_ACC_Msk        (0x3U << WJ_SPDIF_TX_CS_B_REG_T_B_CLK_ACC_Pos)
#define WJ_SPDIF_TX_CS_B_REG_T_B_CLK_ACC            WJ_SPDIF_TX_CS_B_REG_T_B_CLK_ACC_Msk


typedef struct {
    __IOM uint32_t SPDIF_EN_REG;        /* offeset: 0x00 (R/W) SPDIF enable register */
    __IOM uint32_t TX_EN_REG;           /* offeset: 0x04 (R/W) SPDIF TX enable register */
    __IOM uint32_t TX_CTL_REG;          /* offeset: 0x08 (R/W) SPDIF TX control register */
    __IOM uint32_t TX_CS_A_REG;         /* offeset: 0x0c (R/W) SPDIF TX channel A channel status configure register */
    __IOM uint32_t TX_USER_A0_REG;      /* offeset: 0x10 (R/W) SPDIF SPDIF_OUT channel A user bit [31:0] */
    __IOM uint32_t TX_USER_A1_REG;      /* offeset: 0x14 (R/W) SPDIF_OUT channel A user bit [63:32] */
    __IOM uint32_t TX_USER_A2_REG;      /* offeset: 0x18 (R/W) SPDIF_OUT channel A user bit [95:64] */
    __IOM uint32_t TX_USER_A3_REG;      /* offeset: 0x1c (R/W) SPDIF_OUT channel A user bit [127:96] */
    __IOM uint32_t TX_USER_A4_REG;      /* offeset: 0x20 (R/W) SPDIF_OUT channel A user bit [159:128] */
    __IOM uint32_t TX_USER_A5_REG;      /* offeset: 0x24 (R/W) SPDIF_OUT channel A user bit [191:160] */
    __IOM uint32_t TX_FIFO_DR;          /* offeset: 0x28 (R/W) SPDIF TX FIFO write data buffer */
    __IOM uint32_t TX_FIFO_TH;          /* offeset: 0x2c (R/W) SPDIF TX FIFO threshold register */
    __IOM uint32_t TX_FIFO_DL;          /* offeset: 0x30 (R/W) SPDIF TX FIFO data level register */
    __IOM uint32_t TX_DMA_EN;           /* offeset: 0x34 (R/W) SPDIF TX DMA Enable register */
    __IOM uint32_t TX_DMA_TH;           /* offeset: 0x38 (R/W) SPDIF TX DMA threshold register */
    __IOM uint32_t SPDIF_SR;            /* offeset: 0x3c (R/W) SPDIF status register */
    __IOM uint32_t SPDIF_IMR;           /* offeset: 0x40 (R/W) SPDIF interrupt & error mask register */
    __IOM uint32_t SPDIF_ISR;           /* offeset: 0x44 (R/W) SPDIF interrupt & error status register */
    __IOM uint32_t SPDIF_RISR;          /* offeset: 0x48 (R/W) SPDIF Raw interrupt & error status register */
    __IOM uint32_t SPDIF_ICR;           /* offeset: 0x4c (R/W) SPDIF interrupt & error clear register */
    __IOM uint32_t RX_EN_REG;           /* offeset: 0x50 (R/W) SPDIF RX enable register */
    __IOM uint32_t RX_CTL_REG;          /* offeset: 0x54 (R/W) SPDIF RX control register */
    __IOM uint32_t RX_CS_A_REG;         /* offeset: 0x58 (R/W) SPDIF RX channel A channel status register */
    __IOM uint32_t RX_USER_A0_REG;      /* offeset: 0x5c (R/W) SPDIF_IN channel A user bit [31:0] */
    __IOM uint32_t RX_USER_A1_REG;      /* offeset: 0x60 (R/W) SPDIF_IN channel A user bit [63:32] */
    __IOM uint32_t RX_USER_A2_REG;      /* offeset: 0x64 (R/W) SPDIF_IN channel A user bit [95:64] */
    __IOM uint32_t RX_USER_A3_REG;      /* offeset: 0x68 (R/W) SPDIF_IN channel A user bit [127:96] */
    __IOM uint32_t RX_USER_A4_REG;      /* offeset: 0x6c (R/W) SPDIF_IN channel A user bit [159:128] */
    __IOM uint32_t RX_USER_A5_REG;      /* offeset: 0x70 (R/W) SPDIF_IN user bit [191:160] */
    __IOM uint32_t RX_FIFO_DR;          /* offeset: 0x74 (R/W) SPDIF RX FIFO data register */
    __IOM uint32_t RX_FIFO_TH;          /* offeset: 0x78 (R/W) SPDIF RX FIFO threshold register */
    __IOM uint32_t RX_FIFO_DL;          /* offeset: 0x7c (R/W) SPDIF RX FIFO data level register */
    __IOM uint32_t RX_DMA_EN;          /* offeset: 0x80 (R/W) SPDIF RX DMA Enable register */
    __IOM uint32_t RX_DMA_TH;          /* offeset: 0x84 (R/W) SPDIF RX DMA threshold register */
    __IOM uint32_t TX_CS_B_REG;         /* offeset: 0x88 (R/W) SPDIF TX channel B channel status configure register */
    __IOM uint32_t TX_USER_B0_REG;      /* offeset: 0x8c (R/W) SPDIF_OUT channel B user bit [31:0] */
    __IOM uint32_t TX_USER_B1_REG;      /* offeset: 0x90 (R/W) SPDIF_OUT channel B user bit [63:32] */
    __IOM uint32_t TX_USER_B2_REG;      /* offeset: 0x94 (R/W) SPDIF_OUT channel B user bit [95:64] */
    __IOM uint32_t TX_USER_B3_REG;      /* offeset: 0x98 (R/W) SPDIF_OUT channel B user bit [127:96] */
    __IOM uint32_t TX_USER_B4_REG;      /* offeset: 0x9c (R/W) SPDIF_OUT channel B user bit [159:128] */
    __IOM uint32_t TX_USER_B5_REG;      /* offeset: 0xa0 (R/W) SPDIF_OUT channel B user bit [191:160] */
    __IOM uint32_t RX_CS_B_REG;         /* offeset: 0xa4 (R/W) SPDIF RX channel B channel status register */
    __IOM uint32_t RX_USER_B0_REG;      /* offeset: 0xa8 (R/W) SPDIF_IN channel B user bit [31:0] */
    __IOM uint32_t RX_USER_B1_REG;      /* offeset: 0xac (R/W) SPDIF_IN channel B user bit [63:32] */
    __IOM uint32_t RX_USER_B2_REG;      /* offeset: 0xb0 (R/W) SPDIF_IN channel B user bit [95:64] */
    __IOM uint32_t RX_USER_B3_REG;      /* offeset: 0xb4 (R/W) SPDIF_IN channel B user bit [127:96] */
    __IOM uint32_t RX_USER_B4_REG;      /* offeset: 0xb8 (R/W) SPDIF_IN channel B user bit [159:128] */
    __IOM uint32_t RX_USER_B5_REG;      /* offeset: 0xbc (R/W) SPDIF_IN channel B user bit [191:160] */
} wj_spdif_regs_t;

static inline void wj_spdif_enable(wj_spdif_regs_t *spdif_base)
{
    spdif_base->SPDIF_EN_REG |= WJ_SPDIF_EN;
}

static inline void wj_spdif_disable(wj_spdif_regs_t *spdif_base)
{
    spdif_base->SPDIF_EN_REG &= ~WJ_SPDIF_EN;
}

static inline void wj_spdif_enable_tx(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_EN_REG |= WJ_SPDIF_TX_EN_REG_TX_EN;
}

static inline void wj_spdif_disable_tx(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_EN_REG &= ~WJ_SPDIF_TX_EN_REG_TX_EN;
}

static inline void wj_spdif_set_tx_div_bypass(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CTL_REG &= ~WJ_SPDIF_TX_CTL_REG_TX_DIV_NOT_BYPASS;
}

static inline void wj_spdif_set_tx_div_not_bypass(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CTL_REG |= WJ_SPDIF_TX_CTL_REG_TX_DIV_NOT_BYPASS;
}

static inline void wj_spdif_set_tx_div(wj_spdif_regs_t *spdif_base, uint32_t div)
{
    spdif_base->TX_CTL_REG |= div << WJ_SPDIF_TX_CTL_REGTX_DIV_Pos;
}

static inline void wj_spdif_tx_datamode_24bit(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CTL_REG &= ~WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_Msk;
    spdif_base->TX_CTL_REG |= WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_24BITS;
}

static inline void wj_spdif_tx_datamode_20bit(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CTL_REG &= ~WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_Msk;
    spdif_base->TX_CTL_REG |= WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_20BITS;
}

static inline void wj_spdif_tx_datamode_16bit_oneworld(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CTL_REG &= ~WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_Msk;
    spdif_base->TX_CTL_REG |= WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_16BITS;
}

static inline void wj_spdif_tx_datamode_16bit_halfworld(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CTL_REG &= ~WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_Msk;
    spdif_base->TX_CTL_REG |= WJ_SPDIF_TX_CTL_REG_TX_DATAMODE_16BITS_H;
}

static inline void wj_spdif_set_tx_signal_format_is_consumer(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_A_REG &= ~WJ_SPDIF_TX_CS_REG_T_PRO_EN;
}

static inline void wj_spdif_set_tx_signal_format_is_professional(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_A_REG |= WJ_SPDIF_TX_CS_REG_T_PRO_EN;
}

static inline void wj_spdif_set_tx_mono(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_A_REG |= WJ_SPDIF_TX_CS_REG_T_AUIDO_EN;
}

static inline void wj_spdif_set_tx_stero(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_A_REG &= ~WJ_SPDIF_TX_CS_REG_T_AUIDO_EN;
}

static inline void wj_spdif_set_tx_copy_right_data(wj_spdif_regs_t *spdif_base, uint32_t permit)
{
    spdif_base->TX_CS_A_REG &= ~WJ_SPDIF_TX_CS_REG_T_COPY;

    if (permit) {
        spdif_base->TX_CS_A_REG |= WJ_SPDIF_TX_CS_REG_T_COPY;
    }
}

static inline void wj_spdif_set_tx_emp_mode(wj_spdif_regs_t *spdif_base, uint32_t emphasis)
{
    spdif_base->TX_CS_A_REG &= ~WJ_SPDIF_TX_CS_REG_T_EMP_MODE;

    if (emphasis) {
        spdif_base->TX_CS_A_REG |= WJ_SPDIF_TX_CS_REG_T_EMP_MODE;
    }
}

static inline void wj_spdif_set_tx_sample_rate_32khz(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_A_REG &= ~WJ_SPDIF_TX_CS_REG_T_FS_SEL_Msk;
    spdif_base->TX_CS_A_REG |= WJ_SPDIF_TX_CS_REG_T_FS_SEL_32KHZ;
}

static inline void wj_spdif_set_tx_sample_rate_44_1khz(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_A_REG &= ~WJ_SPDIF_TX_CS_REG_T_FS_SEL_Msk;
    spdif_base->TX_CS_A_REG |= WJ_SPDIF_TX_CS_REG_T_FS_SEL_44_1KHZ;
}

static inline void wj_spdif_set_tx_sample_rate_48khz(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_A_REG &= ~WJ_SPDIF_TX_CS_REG_T_FS_SEL_Msk;
    spdif_base->TX_CS_A_REG |= WJ_SPDIF_TX_CS_REG_T_FS_SEL_48KHZ;
}

static inline void wj_spdif_set_tx_sample_rate_88_2khz(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_A_REG &= ~WJ_SPDIF_TX_CS_REG_T_FS_SEL_Msk;
    spdif_base->TX_CS_A_REG |= WJ_SPDIF_TX_CS_REG_T_FS_SEL_88_2KHZ;
}

static inline void wj_spdif_set_tx_sample_rate_96khz(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_A_REG &= ~WJ_SPDIF_TX_CS_REG_T_FS_SEL_Msk;
    spdif_base->TX_CS_A_REG |= WJ_SPDIF_TX_CS_REG_T_FS_SEL_96KHZ;
}

static inline void wj_spdif_set_tx_sample_rate_192khz(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_A_REG &= ~WJ_SPDIF_TX_CS_REG_T_FS_SEL_Msk;
    spdif_base->TX_CS_A_REG |= WJ_SPDIF_TX_CS_REG_T_FS_SEL_192KHZ;
}

static inline void wj_spdif_set_tx_sample_rate_default(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_A_REG &= ~WJ_SPDIF_TX_CS_REG_T_FS_SEL_Msk;
    spdif_base->TX_CS_A_REG |= WJ_SPDIF_TX_CS_REG_T_FS_SEL_DEFAULT;
}

static inline void wj_spdif_set_tx_fifo_threshold(wj_spdif_regs_t *spdif_base, uint32_t val)
{
    spdif_base->TX_FIFO_TH &= ~WJ_SPDIF_ENTX_FIFO_TH_Msk;
    spdif_base->TX_FIFO_TH = val;
}

static inline uint32_t wj_spdif_get_tx_fifo_level(wj_spdif_regs_t *spdif_base)
{
    return spdif_base->TX_FIFO_DL;
}

static inline void wj_spdif_set_tx_dma_enable(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_DMA_EN |= WJ_SPDIF_TX_DMA_EN;
}

static inline void wj_spdif_set_tx_dma_disable(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_DMA_EN &= ~WJ_SPDIF_TX_DMA_EN;
}

static inline void wj_spdif_set_tx_dma_th_level(wj_spdif_regs_t *spdif_base, uint32_t level)
{
    spdif_base->TX_DMA_TH = level;
}

static inline uint32_t wj_spdif_get_tx_dma_th_level(wj_spdif_regs_t *spdif_base)
{
    return spdif_base->TX_DMA_TH;
}

static inline uint32_t wj_spdif_get_fifo_status(wj_spdif_regs_t *spdif_base)
{
    return spdif_base->SPDIF_SR;
}

static inline void wj_spdif_set_irq(wj_spdif_regs_t *spdif_base, uint32_t mask)
{
    spdif_base->SPDIF_IMR = mask;
}

static inline uint32_t wj_spdif_get_irq_status(wj_spdif_regs_t *spdif_base)
{
    return spdif_base->SPDIF_ISR;
}

static inline uint32_t wj_spdif_get_raw_irq_status(wj_spdif_regs_t *spdif_base)
{
    return spdif_base->SPDIF_RISR;
}

static inline void wj_spdif_clear_irq(wj_spdif_regs_t *spdif_base, uint32_t irq_status)
{
    spdif_base->SPDIF_ICR = irq_status;
}

static inline void wj_spdif_enable_rx(wj_spdif_regs_t *spdif_base)
{
    spdif_base->RX_EN_REG |= WJ_SPDIF_RX_EN;
}

static inline void wj_spdif_disable_rx(wj_spdif_regs_t *spdif_base)
{
    spdif_base->RX_EN_REG &= ~WJ_SPDIF_RX_EN;
}

static inline void wj_spdif_set_rx_div_bypass(wj_spdif_regs_t *spdif_base)
{
    spdif_base->RX_CTL_REG &= ~WJ_SPDIF_RX_CTL_REG_RX_DIV_NOT_BYPASS;
}

static inline void wj_spdif_set_rx_div_not_bypass(wj_spdif_regs_t *spdif_base)
{
    spdif_base->RX_CTL_REG |= WJ_SPDIF_RX_CTL_REG_RX_DIV_NOT_BYPASS;
}

static inline void wj_spdif_set_rx_div(wj_spdif_regs_t *spdif_base, uint32_t div)
{
    spdif_base->RX_CTL_REG |= div << WJ_SPDIF_RX_CTL_REG_RX_DIV_Pos;
}

static inline void wj_spdif_rx_datamode_24bit(wj_spdif_regs_t *spdif_base)
{
    spdif_base->RX_CTL_REG &= ~WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_Msk;
    spdif_base->RX_CTL_REG |= WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_24BITS;
}

static inline void wj_spdif_rx_datamode_20bit(wj_spdif_regs_t *spdif_base)
{
    spdif_base->RX_CTL_REG &= ~WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_Msk;
    spdif_base->RX_CTL_REG |= WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_20BITS;
}

static inline void wj_spdif_rx_datamode_16bit_oneworld(wj_spdif_regs_t *spdif_base)
{
    spdif_base->RX_CTL_REG &= ~WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_Msk;
    spdif_base->RX_CTL_REG |= WJ_SPDIF_RX_CTL_REG_RX_DATAMODE_16BITS;
}

static inline void wj_spdif_set_rx_fifo_threshold(wj_spdif_regs_t *spdif_base, uint32_t val)
{
    spdif_base->RX_FIFO_TH &= ~WJ_SPDIF_RX_FIFO_TH;
    spdif_base->RX_FIFO_TH = val;
}

static inline uint32_t wj_spdif_get_rx_fifo_level(wj_spdif_regs_t *spdif_base)
{
    return spdif_base->RX_FIFO_TH;
}

static inline void wj_spdif_set_rx_dma_enable(wj_spdif_regs_t *spdif_base)
{
    spdif_base->RX_DMA_EN |= WJ_SPDIF_RDMA_EN;
}

static inline void wj_spdif_set_rx_dma_disable(wj_spdif_regs_t *spdif_base)
{
    spdif_base->RX_DMA_EN &= ~WJ_SPDIF_RDMA_EN;
}

static inline void wj_spdif_set_rx_dma_th_level(wj_spdif_regs_t *spdif_base, uint32_t level)
{
    spdif_base->RX_DMA_TH = level;
}

static inline uint32_t wj_spdif_get_rx_dma_th_level(wj_spdif_regs_t *spdif_base)
{
    return spdif_base->RX_DMA_TH;
}

static inline void wj_spdif_transmit_data(wj_spdif_regs_t *spdif_base, uint32_t data)
{
    spdif_base->TX_FIFO_DR = data;
}

static inline uint32_t wj_spdif_receive_data(wj_spdif_regs_t *spdif_base)
{
    return spdif_base->RX_FIFO_DR;
}

static inline void wj_spdif_set_tx_channel_b_signal_format_is_consumer(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_B_REG &= ~WJ_SPDIF_TX_CS_REG_T_PRO_EN;
}

static inline void wj_spdif_set_tx_channel_b_signal_format_is_professional(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_B_REG |= WJ_SPDIF_TX_CS_REG_T_PRO_EN;
}

static inline void wj_spdif_set_tx_channel_b_mono(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_B_REG |= WJ_SPDIF_TX_CS_B_REG_T_B_AUIDO_EN;
}

static inline void wj_spdif_set_tx_channel_b_stero(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_B_REG &= ~WJ_SPDIF_TX_CS_B_REG_T_B_AUIDO_EN;
}

static inline void wj_spdif_set_tx_channel_b_copy_right_data(wj_spdif_regs_t *spdif_base, uint32_t permit)
{
    spdif_base->TX_CS_B_REG &= ~WJ_SPDIF_TX_CS_B_REG_T_B_COPY;

    if (permit) {
        spdif_base->TX_CS_B_REG |= WJ_SPDIF_TX_CS_B_REG_T_B_COPY;
    }
}

static inline void wj_spdif_set_tx_channel_b_emp_mode(wj_spdif_regs_t *spdif_base, uint32_t emphasis)
{
    spdif_base->TX_CS_B_REG &= ~WJ_SPDIF_TX_CS_B_REG_T_B_EMP_MODE;

    if (emphasis) {
        spdif_base->TX_CS_B_REG |= WJ_SPDIF_TX_CS_B_REG_T_B_EMP_MODE;
    }
}

static inline void wj_spdif_set_tx_channel_b_sample_rate_32khz(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_B_REG &= ~WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Msk;
    spdif_base->TX_CS_B_REG |= WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_32KHZ;
}

static inline void wj_spdif_set_tx_channel_b_sample_rate_44_1khz(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_B_REG &= ~WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Msk;
    spdif_base->TX_CS_B_REG |= WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_44_1KHZ;
}

static inline void wj_spdif_set_tx_channel_b_sample_rate_48khz(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_B_REG &= ~WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Msk;
    spdif_base->TX_CS_B_REG |= WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_48KHZ;
}

static inline void wj_spdif_set_tx_channel_b_sample_rate_88_2khz(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_B_REG &= ~WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Msk;
    spdif_base->TX_CS_B_REG |= WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_88_2KHZ;
}

static inline void wj_spdif_set_tx_channel_b_sample_rate_96khz(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_B_REG &= ~WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Msk;
    spdif_base->TX_CS_B_REG |= WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_96KHZ;
}

static inline void wj_spdif_set_tx_channel_b_sample_rate_192khz(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_B_REG &= ~WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Msk;
    spdif_base->TX_CS_B_REG |= WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_192KHZ;
}

static inline void wj_spdif_set_tx_channel_b_sample_rate_default(wj_spdif_regs_t *spdif_base)
{
    spdif_base->TX_CS_B_REG &= ~WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_Msk;
    spdif_base->TX_CS_B_REG |= WJ_SPDIF_TX_CS_B_REG_T_B_FS_SEL_DEFAULT;
}

#ifdef __cplusplus
}
#endif

#endif /* _SPDIF_LL_H_ */
