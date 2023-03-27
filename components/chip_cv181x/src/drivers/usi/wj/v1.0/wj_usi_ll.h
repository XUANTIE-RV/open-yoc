/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     wj_usi_ll.h
 * @brief    header file for usi ll driver
 * @version  V1.0
 * @date     1. Mon 2020
 ******************************************************************************/
#ifndef __WJ_USI_LL_H__
#define __WJ_USI_LL_H__
#include <stdio.h>
#include <soc.h>
#include <csi_core.h>
#ifdef __cplusplus
extern "C" {
#endif

/*! USI_CTRL Offset 0x000 */
#define WJ_USI_CTRL_Pos                                     (0U)
#define WJ_USI_CTRL_Msk                                     (0xFU << WJ_USI_CTRL_USI_EN_Pos)
#define WJ_USI_CTRL_USI_EN_Pos                              (0U)
#define WJ_USI_CTRL_USI_EN_Msk                              (0x1U << WJ_USI_CTRL_USI_EN_Pos)
#define WJ_USI_CTRL_USI_EN                                  WJ_USI_CTRL_USI_EN_Msk
#define WJ_USI_CTRL_FM_EN_Pos                               (1U)
#define WJ_USI_CTRL_FM_EN_Msk                               (0x1U << WJ_USI_CTRL_FM_EN_Pos)
#define WJ_USI_CTRL_FM_EN                                   WJ_USI_CTRL_FM_EN_Msk
#define WJ_USI_CTRL_TX_FIFO_EN_Pos                          (2U)
#define WJ_USI_CTRL_TX_FIFO_EN_Msk                          (0x1U << WJ_USI_CTRL_TX_FIFO_EN_Pos)
#define WJ_USI_CTRL_TX_FIFO_EN                              WJ_USI_CTRL_TX_FIFO_EN_Msk
#define WJ_USI_CTRL_RX_FIFO_EN_Pos                          (3U)
#define WJ_USI_CTRL_RX_FIFO_EN_Msk                          (0x1U << WJ_USI_CTRL_RX_FIFO_EN_Pos)
#define WJ_USI_CTRL_RX_FIFO_EN                              WJ_USI_CTRL_RX_FIFO_EN_Msk
#define WJ_USI_MODE_SEL_Pos                                 (0U)
#define WJ_USI_MODE_SEL_Msk                                 (0x3U << WJ_USI_MODE_SEL_Pos)
#define WJ_USI_MODE_SEL_UART                                (0x0U<< WJ_USI_MODE_SEL_Pos)
#define WJ_USI_MODE_SEL_I2C                                 (0x1U<< WJ_USI_MODE_SEL_Pos)
#define WJ_USI_MODE_SEL_SPI                                 (0x2U<< WJ_USI_MODE_SEL_Pos)

/*! TX_FIFO Offset 0x008 */
#define WJ_USI_TX_RX_FIFO_Pos                               (0U)
#define WJ_USI_TX_RX_FIFO_Msk                               (0x0000ffffU<<WJ_USI_TX_RX_FIFO_Pos)

/*! FIFO_STA Offset 0x00C */
#define WJ_USI_FIFO_STA_TX_EMPTY_Pos                        (0U)
#define WJ_USI_FIFO_STA_TX_EMPTY_Msk                        (0x1U << WJ_USI_FIFO_STA_TX_EMPTY_Pos)
#define WJ_USI_FIFO_STA_TX_EMPTY                            WJ_USI_FIFO_STA_TX_EMPTY_Msk
#define WJ_USI_FIFO_STA_TX_FULL_Pos                         (1U)
#define WJ_USI_FIFO_STA_TX_FULL_Msk                         (0x1U << WJ_USI_FIFO_STA_TX_FULL_Pos)
#define WJ_USI_FIFO_STA_TX_FULL                             WJ_USI_FIFO_STA_TX_FULL_Msk
#define WJ_USI_FIFO_STA_RX_EMPTY_Pos                        (2U)
#define WJ_USI_FIFO_STA_RX_EMPTY_Msk                        (0x1U << WJ_USI_FIFO_STA_RX_EMPTY_Pos)
#define WJ_USI_FIFO_STA_RX_EMPTY                            WJ_USI_FIFO_STA_RX_EMPTY_Msk
#define WJ_USI_FIFO_STA_RX_FULL_Pos                         (3U)
#define WJ_USI_FIFO_STA_RX_FULL_Msk                         (0x1U << WJ_USI_FIFO_STA_RX_FULL_Pos)
#define WJ_USI_FIFO_STA_RX_FULL                             WJ_USI_FIFO_STA_RX_FULL_Msk
#define WJ_USI_FIFO_STA_TX_NUM_Pos                          (8U)
#define WJ_USI_FIFO_STA_TX_NUM_Msk                          (0xFU << WJ_USI_FIFO_STA_TX_NUM_Pos)
#define WJ_USI_FIFO_STA_TX_NUM                              WJ_USI_FIFO_STA_TX_NUM_Msk
#define WJ_USI_FIFO_STA_RX_NUM_Pos                          (16U)
#define WJ_USI_FIFO_STA_RX_NUM_Msk                          (0xFU << WJ_USI_FIFO_STA_RX_NUM_Pos)
#define WJ_USI_FIFO_STA_RX_NUM                              WJ_USI_FIFO_STA_RX_NUM_Msk

/*! CLK_DIV0 Offset 0x010*/
#define WJ_USI_CLK_DIV0_Pos                                 (0U)
#define WJ_USI_CLK_DIV0_Msk                                 (0xFFFFFFU << WJ_USI_CLK_DIV0_Pos)
#define WJ_USI_CLK_DIV0                                     WJ_USI_CLK_DIV0_Msk

/*! CLK_DIV1 Offset 0x014*/
#define WJ_USI_CLK_DIV1_Pos                                 (0U)
#define WJ_USI_CLK_DIV1_Msk                                 (0xFFFFFFU << WJ_USI_CLK_DIV1_Pos)
#define WJ_USI_CLK_DIV1                                     WJ_USI_CLK_DIV1_Msk

/*! UART_CTRL Offset 0x018*/
#define WJ_USI_UART_CTRL_DBIT_Pos                           (0U)
#define WJ_USI_UART_CTRL_DBIT_Msk                           (0x3U << WJ_USI_UART_CTRL_DBIT_Pos)
#define WJ_USI_UART_CTRL_DBIT_5                             (0U<< WJ_USI_UART_CTRL_DBIT_Pos)
#define WJ_USI_UART_CTRL_DBIT_6                             (1U<< WJ_USI_UART_CTRL_DBIT_Pos)
#define WJ_USI_UART_CTRL_DBIT_7                             (2U<< WJ_USI_UART_CTRL_DBIT_Pos)
#define WJ_USI_UART_CTRL_DBIT_8                             (3U<< WJ_USI_UART_CTRL_DBIT_Pos)
#define WJ_USI_UART_CTRL_PBIT_Pos                           (2U)
#define WJ_USI_UART_CTRL_PBIT_Msk                           (0x3U << WJ_USI_UART_CTRL_PBIT_Pos)
#define WJ_USI_UART_CTRL_PBIT_1                             (0U<< WJ_USI_UART_CTRL_DBIT_Pos)
#define WJ_USI_UART_CTRL_PBIT_1P5                           (1U<< WJ_USI_UART_CTRL_DBIT_Pos)
#define WJ_USI_UART_CTRL_PBIT_2                             (2U<< WJ_USI_UART_CTRL_DBIT_Pos)
#define WJ_USI_UART_CTRL_PEN_Pos                            (4U)
#define WJ_USI_UART_CTRL_PEN_Msk                            (0x1U << WJ_USI_UART_CTRL_PEN_Pos)
#define WJ_USI_UART_CTRL_PEN_EN                             WJ_USI_UART_CTRL_PEN_Msk
#define WJ_USI_UART_CTRL_EPS_Pos                            (5U)
#define WJ_USI_UART_CTRL_EPS_Msk                            (0x1U << WJ_USI_UART_CTRL_EPS_Pos)
#define WJ_USI_UART_CTRL_EPS_EN                             WJ_USI_UART_CTRL_EPS_Msk

/*! UART_STA Offset 0x01C*/
#define WJ_USI_UART_STA_TXD_WORK_Pos                        (0U)
#define WJ_USI_UART_STA_TXD_WORK_Msk                        (0x1U << WJ_USI_UART_STA_TXD_WORK_Pos)
#define WJ_USI_UART_STA_RXD_WORK_Pos                        (0U)
#define WJ_USI_UART_STA_RXD_WORK_Msk                        (0x1U << WJ_USI_UART_STA_RXD_WORK_Pos)

/*! I2C_MODE Offset 0x020*/
#define WJ_USI_I2C_MODE_Pos                                 (0U)
#define WJ_USI_I2C_MODE_Msk                                 (0x1U << WJ_USI_I2C_MODE_Pos)
#define WJ_USI_I2C_MODE_MASTER                              WJ_USI_I2C_MODE_Msk

/*! I2C_ADDR Offset 0x024*/
#define WJ_USI_I2C_ADDR_Pos                                 (0U)
#define WJ_USI_I2C_ADDR_Msk                                 (0x3FFU << WJ_USI_I2C_ADDR_Pos)
#define WJ_USI_I2CM_CTRL_ADDR_MODE_Pos                      (0U)
#define WJ_USI_I2CM_CTRL_ADDR_MODE_Msk                      (0x1U << WJ_USI_I2CM_CTRL_ADDR_MODE_Pos)
#define WJ_USI_I2CM_CTRL_ADDR_MODE_10BIT                    WJ_USI_I2CM_CTRL_ADDR_MODE_Msk
#define WJ_USI_I2CM_CTRL_STOP_Pos                           (1U)
#define WJ_USI_I2CM_CTRL_STOP_Msk                           (0x1U << WJ_USI_I2CM_CTRL_STOP_Pos)
#define WJ_USI_I2CM_CTRL_STOP                               WJ_USI_I2CM_CTRL_STOP_Msk
#define WJ_USI_I2CM_CTRL_HS_MODE_Pos                        (2U)  //pangu not suport high speed mode
#define WJ_USI_I2CM_CTRL_HS_MODE_Msk                        (0x1U << WJ_USI_I2CM_CTRL_HS_MODE_Pos)
#define WJ_USI_I2CM_CTRL_HS_MODE_EN                         WJ_USI_I2CM_CTRL_HS_MODE_Msk
#define WJ_USI_I2CM_CTRL_SBYTE_Pos                          (3U)
#define WJ_USI_I2CM_CTRL_SBYTE_Msk                          (0x1U << WJ_USI_I2CM_CTRL_SBYTE_Pos)
#define WJ_USI_I2CM_CTRL_SBYTE                              WJ_USI_I2CM_CTRL_SBYTE_Msk

/*! I2CM_CODE Offset 0x02C*/
#define WJ_USI_I2CM_CODE_MCODE_Pos                          (0U)
#define WJ_USI_I2CM_CODE_MCODE_Msk                          (0x7U << WJ_USI_I2CM_CODE_MCODE_Pos)
#define WJ_USI_I2CS_CTRL_ADDR_MODE_Pos                      (0U)
#define WJ_USI_I2CS_CTRL_ADDR_MODE_Msk                      (0x1U << WJ_USI_I2CS_CTRL_ADDR_MODE_Pos)
#define WJ_USI_I2CS_CTRL_ADDR_MODE_7BIT                     WJ_USI_I2CS_CTRL_ADDR_MODE_Msk
#define WJ_USI_I2CS_CTRL_GCALL_MODE_Pos                     (1U)
#define WJ_USI_I2CS_CTRL_GCALL_MODE_Msk                     (0x1U << WJ_USI_I2CS_CTRL_GCALL_MODE_Pos)
#define WJ_USI_I2CS_CTRL_GCALL_MODE_EN                      WJ_USI_I2CS_CTRL_GCALL_MODE_Msk
#define WJ_USI_I2CS_CTRL_I2CS_HS_Pos                        (2U)
#define WJ_USI_I2CS_CTRL_I2CS_HS_Msk                        (0x1U << WJ_USI_I2CS_CTRL_I2CS_HS_Pos)
#define WJ_USI_I2CS_CTRL_I2CS_HS_EN                         WJ_USI_I2CS_CTRL_I2CS_HS_Msk
#define WJ_USI_I2CS_CTRL_START_BYTE_Pos                     (3U)
#define WJ_USI_I2CS_CTRL_START_BYTE_Msk                     (0x1U << WJ_USI_I2CS_CTRL_START_BYTE_Pos)
#define WJ_USI_I2CS_CTRL_START_BYTE_EN                      WJ_USI_I2CS_CTRL_START_BYTE_Msk

/*! USI_I2C_FS_DIV Offset 0x034*/
#define WJ_USI_I2C_FS_DIV_Pos                               (0U)
#define WJ_USI_I2C_FS_DIV_Msk                               (0xffU << WJ_USI_I2C_FS_DIV_Pos)

/*! I2C_HOLD Offset 0x038*/
#define WJ_USI_I2C_HOLD_Pos                                 (0U)
#define WJ_USI_I2C_HOLD_Msk                                 (0xFFU << WJ_USI_I2C_HOLD_Pos)

/*! I2C_STA Offset 0x03C*/
#define WJ_USI_I2C_STA_I2CM_WORK_Pos                        (0U)
#define WJ_USI_I2C_STA_I2CM_WORK_Msk                        (0x1U << WJ_USI_I2C_STA_I2CM_WORK_Pos)
#define WJ_USI_I2C_STA_I2CM_WORK_EN                         WJ_USI_I2C_STA_I2CM_WORK_Msk
#define WJ_USI_I2C_STA_I2CM_DATA_Pos                        (1U)
#define WJ_USI_I2C_STA_I2CM_DATA_Msk                        (0x1U << WJ_USI_I2C_STA_I2CM_DATA_Pos)
#define WJ_USI_I2C_STA_I2CM_DATA_ING                        WJ_USI_I2C_STA_I2CM_DATA_Msk
#define WJ_USI_I2C_STA_I2CS_WORK_Pos                        (8U)
#define WJ_USI_I2C_STA_I2CS_WORK_Msk                        (0x1U << WJ_USI_I2C_STA_I2CS_WORK_Pos)
#define WJ_USI_I2C_STA_I2CS_WORK_EN                         WJ_USI_I2C_STA_I2CS_WORK_Msk

/*! SPI_MODE Offset 0x040*/
#define WJ_USI_SPI_MODE_Pos                                 (0U)
#define WJ_USI_SPI_MODE_Msk                                 (0x1U << WJ_USI_SPI_MODE_Pos)
#define WJ_USI_SPI_MODE_MASTER                              WJ_USI_SPI_MODE_Msk

/*! SPI_CTRL  Offset 0x044*/
#define WJ_USI_SPI_CTRL_DATA_SIZE_Pos                       (0U)
#define WJ_USI_SPI_CTRL_DATA_SIZE_Msk                       (0xFU << WJ_USI_SPI_CTRL_DATA_SIZE_Pos)
#define WJ_USI_SPI_CTRL_DATA_SIZE_4BIT                      (3U<< WJ_USI_SPI_CTRL_DATA_SIZE_Pos))
#define WJ_USI_SPI_CTRL_DATA_SIZE_5BIT                      (4U<< WJ_USI_SPI_CTRL_DATA_SIZE_Pos))
#define WJ_USI_SPI_CTRL_DATA_SIZE_6BIT                      (5U<< WJ_USI_SPI_CTRL_DATA_SIZE_Pos))
#define WJ_USI_SPI_CTRL_DATA_SIZE_7BIT                      (6U<< WJ_USI_SPI_CTRL_DATA_SIZE_Pos))
#define WJ_USI_SPI_CTRL_DATA_SIZE_8BIT                      (7U<< WJ_USI_SPI_CTRL_DATA_SIZE_Pos))
#define WJ_USI_SPI_CTRL_DATA_SIZE_9BIT                      (8U<< WJ_USI_SPI_CTRL_DATA_SIZE_Pos))
#define WJ_USI_SPI_CTRL_DATA_SIZE_10BIT                     (9U<< WJ_USI_SPI_CTRL_DATA_SIZE_Pos))
#define WJ_USI_SPI_CTRL_DATA_SIZE_11BIT                     (10U<< WJ_USI_SPI_CTRL_DATA_SIZE_Pos))
#define WJ_USI_SPI_CTRL_DATA_SIZE_12BIT                     (11U<< WJ_USI_SPI_CTRL_DATA_SIZE_Pos))
#define WJ_USI_SPI_CTRL_DATA_SIZE_13BIT                     (12U<< WJ_USI_SPI_CTRL_DATA_SIZE_Pos))
#define WJ_USI_SPI_CTRL_DATA_SIZE_14BIT                     (13U<< WJ_USI_SPI_CTRL_DATA_SIZE_Pos))
#define WJ_USI_SPI_CTRL_DATA_SIZE_15BIT                     (14U<< WJ_USI_SPI_CTRL_DATA_SIZE_Pos))
#define WJ_USI_SPI_CTRL_DATA_SIZE_16BIT                     (15U<< WJ_USI_SPI_CTRL_DATA_SIZE_Pos))
#define WJ_USI_SPI_CTRL_TMOD_Pos                            (4U)
#define WJ_USI_SPI_CTRL_TMOD_Msk                            (0x3U << WJ_USI_SPI_CTRL_TMOD_Pos)
#define WJ_USI_SPI_CTRL_TMOD_SEND_RECEIVE                   (0U<< WJ_USI_SPI_CTRL_TMOD_Pos)
#define WJ_USI_SPI_CTRL_TMOD_SEND_ONLY                      (1U<< WJ_USI_SPI_CTRL_TMOD_Pos)
#define WJ_USI_SPI_CTRL_TMOD_RECEIVE_ONLY                   (2U<< WJ_USI_SPI_CTRL_TMOD_Pos)
#define WJ_USI_SPI_CTRL_CPHA_Pos                            (6U)
#define WJ_USI_SPI_CTRL_CPHA_Msk                            (0x1U << WJ_USI_SPI_CTRL_CPHA_Pos)
#define WJ_USI_SPI_CTRL_CPHA_EN                             WJ_USI_SPI_CTRL_CPHA_Msk
#define WJ_USI_SPI_CTRL_CPOL_Pos                            (7U)
#define WJ_USI_SPI_CTRL_CPOL_Msk                            (0x1U << WJ_USI_SPI_CTRL_CPOL_Pos)
#define WJ_USI_SPI_CTRL_CPOL_EN                             WJ_USI_SPI_CTRL_CPOL_Msk
#define WJ_USI_SPI_CTRL_NSS_TOGGLE_Pos                      (8U)
#define WJ_USI_SPI_CTRL_NSS_TOGGLE_Msk                      (0x1U << WJ_USI_SPI_CTRL_NSS_TOGGLE_Pos)
#define WJ_USI_SPI_CTRL_NSS_TOGGLE_EN                       WJ_USI_SPI_CTRL_NSS_TOGGLE_Msk
#define WJ_USI_SPI_CTRL_NSS_CTRL_Pos                        (9U)
#define WJ_USI_SPI_CTRL_NSS_CTRL_Msk                        (0x1U << WJ_USI_SPI_CTRL_NSS_CTRL_Pos)
#define WJ_USI_SPI_CTRL_NSS_CTRL_EN                         WJ_USI_SPI_CTRL_NSS_CTRL_Msk

/*! SPI_STA Offset 0x048*/
#define WJ_USI_SPI_STA_SPI_WORKING_Pos                      (0U)
#define WJ_USI_SPI_STA_SPI_WORKING_Msk                      (0x1U << WJ_USI_SPI_STA_SPI_WORKING_Pos)
#define WJ_USI_SPI_STA_SPI_WORKING                          WJ_USI_SPI_STA_SPI_WORKING_Msk

/*! INTR_CTRL Offset 0x04C*/
#define WJ_USI_INTR_CTRL_TX_FIFO_TH_Pos                     (0U)
#define WJ_USI_INTR_CTRL_TX_FIFO_TH_Msk                     (7U << WJ_USI_INTR_CTRL_TX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_TX_FIFO_TH_0BYTE                   (0U<< WJ_USI_INTR_CTRL_TX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_TX_FIFO_TH_1BYTE                   (1U<< WJ_USI_INTR_CTRL_TX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_TX_FIFO_TH_2BYTE                   (2U<< WJ_USI_INTR_CTRL_TX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_TX_FIFO_TH_3BYTE                   (3U<< WJ_USI_INTR_CTRL_TX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_TX_FIFO_TH_4BYTE                   (4U<< WJ_USI_INTR_CTRL_TX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_TX_FIFO_TH_5BYTE                   (5U<< WJ_USI_INTR_CTRL_TX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_TX_FIFO_TH_6BYTE                   (6U<< WJ_USI_INTR_CTRL_TX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_TX_FIFO_TH_7BYTE                   (7U<< WJ_USI_INTR_CTRL_TX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_RX_FIFO_TH_Pos                     (8U)
#define WJ_USI_INTR_CTRL_RX_FIFO_TH_Msk                     (7U << WJ_USI_INTR_CTRL_RX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_RX_FIFO_TH_0BYTE                   (0U<< WJ_USI_INTR_CTRL_RX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_RX_FIFO_TH_1BYTE                   (1U<< WJ_USI_INTR_CTRL_RX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_RX_FIFO_TH_2BYTE                   (2U<< WJ_USI_INTR_CTRL_RX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_RX_FIFO_TH_3BYTE                   (3U<< WJ_USI_INTR_CTRL_RX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_RX_FIFO_TH_4BYTE                   (4U<< WJ_USI_INTR_CTRL_RX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_RX_FIFO_TH_5BYTE                   (5U<< WJ_USI_INTR_CTRL_RX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_RX_FIFO_TH_6BYTE                   (6U<< WJ_USI_INTR_CTRL_RX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_RX_FIFO_TH_7BYTE                   (7U<< WJ_USI_INTR_CTRL_RX_FIFO_TH_Pos)
#define WJ_USI_INTR_CTRL_INTR_EDGE_Pos                      (16U)
#define WJ_USI_INTR_CTRL_INTR_EDGE_Msk                      (1U << WJ_USI_INTR_CTRL_INTR_EDGE_Pos)
#define WJ_USI_INTR_CTRL_INTR_EDGE_GE                       (1U<< WJ_USI_INTR_CTRL_INTR_EDGE_Pos)//greater and or equal
#define WJ_USI_INTR_CTRL_INTR_EDGE_LE                       (0U<< WJ_USI_INTR_CTRL_INTR_EDGE_Pos)//less  and or equal

/*! INTR_EN Offset 0x050*/
#define WJ_USI_INTR_EN_TX_THOLD_EN_Pos                      (0U)
#define WJ_USI_INTR_EN_TX_THOLD_EN_Msk                      (0x1U << WJ_USI_INTR_EN_TX_THOLD_EN_Pos)
#define WJ_USI_INTR_EN_TX_THOLD_EN                          WJ_USI_INTR_EN_TX_THOLD_EN_Msk
#define WJ_USI_INTR_EN_TX_EMPTY_EN_Pos                      (1U)
#define WJ_USI_INTR_EN_TX_EMPTY_EN_Msk                      (0x1U << WJ_USI_INTR_EN_TX_EMPTY_EN_Pos)
#define WJ_USI_INTR_EN_TX_EMPTY_EN                          WJ_USI_INTR_EN_TX_EMPTY_EN_Msk
#define WJ_USI_INTR_EN_TX_FULL_EN_Pos                       (2U)
#define WJ_USI_INTR_EN_TX_FULL_EN_Msk                       (0x1U << WJ_USI_INTR_EN_TX_FULL_EN_Pos)
#define WJ_USI_INTR_EN_TX_FULL_EN                           WJ_USI_INTR_EN_TX_FULL_EN_Msk
#define WJ_USI_INTR_EN_TX_RERR_EN_Pos                       (3U)
#define WJ_USI_INTR_EN_TX_RERR_EN_Msk                       (0x1U << WJ_USI_INTR_EN_TX_RERR_EN_Pos)
#define WJ_USI_INTR_EN_TX_RERR_EN                           WJ_USI_INTR_EN_TX_RERR_EN_Msk
#define WJ_USI_INTR_EN_TX_WERR_EN_Pos                       (4U)
#define WJ_USI_INTR_EN_TX_WERR_EN_Msk                       (0x1U << WJ_USI_INTR_EN_TX_WERR_EN_Pos)
#define WJ_USI_INTR_EN_TX_WERR_EN                           WJ_USI_INTR_EN_TX_WERR_EN_Msk
#define WJ_USI_INTR_EN_RX_THOLD_EN_Pos                      (5U)
#define WJ_USI_INTR_EN_RX_THOLD_EN_Msk                      (0x1U << WJ_USI_INTR_EN_RX_THOLD_EN_Pos)
#define WJ_USI_INTR_EN_RX_THOLD_EN                          WJ_USI_INTR_EN_RX_THOLD_EN_Msk
#define WJ_USI_INTR_EN_RX_EMPTY_EN_Pos                      (6U)
#define WJ_USI_INTR_EN_RX_EMPTY_EN_Msk                      (0x1U << WJ_USI_INTR_EN_RX_EMPTY_EN_Pos)
#define WJ_USI_INTR_EN_RX_EMPTY_EN                          WJ_USI_INTR_EN_RX_EMPTY_EN_CLOSE
#define WJ_USI_INTR_EN_RX_FULL_EN_Pos                       (7U)
#define WJ_USI_INTR_EN_RX_FULL_EN_Msk                       (0x1U << WJ_USI_INTR_EN_RX_FULL_EN_Pos)
#define WJ_USI_INTR_EN_RX_FULL_EN                           WJ_USI_INTR_EN_RX_FULL_EN_Msk
#define WJ_USI_INTR_EN_RX_RERR_EN_Pos                       (8U)
#define WJ_USI_INTR_EN_RX_RERR_EN_Msk                       (0x1U << WJ_USI_INTR_EN_RX_RERR_EN_Pos)
#define WJ_USI_INTR_EN_RX_RERR_EN                           WJ_USI_INTR_EN_RX_RERR_EN_Msk
#define WJ_USI_INTR_EN_RX_WERR_EN_Pos                       (9U)
#define WJ_USI_INTR_EN_RX_WERR_EN_Msk                       (0x1U << WJ_USI_INTR_EN_RX_WERR_EN_Pos)
#define WJ_USI_INTR_EN_RX_WERR_EN                           WJ_USI_INTR_EN_RX_WERR_EN_Msk
#define WJ_USI_INTR_EN_UART_TX_STOP_EN_Pos                  (10U)
#define WJ_USI_INTR_EN_UART_TX_STOP_EN_Msk                  (0x1U << WJ_USI_INTR_EN_UART_TX_STOP_EN_Pos)
#define WJ_USI_INTR_EN_UART_TX_STOP_EN                      WJ_USI_INTR_EN_UART_TX_STOP_EN_Msk
#define WJ_USI_INTR_EN_UART_RX_STOP_EN_Pos                  (11U)
#define WJ_USI_INTR_EN_UART_RX_STOP_EN_Msk                  (0x1U << WJ_USI_INTR_EN_UART_RX_STOP_EN_Pos)
#define WJ_USI_INTR_EN_UART_RX_STOP_EN                      WJ_USI_INTR_EN_UART_RX_STOP_EN_Msk
#define WJ_USI_INTR_EN_UART_PERR_EN_Pos                     (12U)
#define WJ_USI_INTR_EN_UART_PERR_EN_Msk                     (0x1U << WJ_USI_INTR_EN_UART_PERR_EN_Pos)
#define WJ_USI_INTR_EN_UART_PERR_EN                         WJ_USI_INTR_EN_UART_PERR_EN_Msk
#define WJ_USI_INTR_EN_I2C_STOP_EN_Pos                      (13U)
#define WJ_USI_INTR_EN_I2C_STOP_EN_Msk                      (0x1U << WJ_USI_INTR_EN_I2C_STOP_EN_Pos)
#define WJ_USI_INTR_EN_I2C_STOP_EN                          WJ_USI_INTR_EN_I2C_STOP_EN_Msk
#define WJ_USI_INTR_EN_I2C_NACK_EN_Pos                      (14U)
#define WJ_USI_INTR_EN_I2C_NACK_EN_Msk                      (0x1U << WJ_USI_INTR_EN_I2C_NACK_EN_Pos)
#define WJ_USI_INTR_EN_I2C_NACK_EN                          WJ_USI_INTR_EN_I2C_NACK_EN_Msk
#define WJ_USI_INTR_EN_I2CM_LOSE_ARBI_EN_Pos                (15U)
#define WJ_USI_INTR_EN_I2CM_LOSE_ARBI_EN_Msk                (0x1U << WJ_USI_INTR_EN_I2CM_LOSE_ARBI_EN_Pos)
#define WJ_USI_INTR_EN_I2CM_LOSE_ARBI_EN                    WJ_USI_INTR_EN_I2CM_LOSE_ARBI_EN_Msk
#define WJ_USI_INTR_EN_I2CS_GCALL_EN_Pos                    (16U)
#define WJ_USI_INTR_EN_I2CS_GCALL_EN_Msk                    (0x1U << WJ_USI_INTR_EN_I2CS_GCALL_EN_Pos)
#define WJ_USI_INTR_EN_I2CS_GCALL_EN                        WJ_USI_INTR_EN_I2CS_GCALL_EN_Msk
#define WJ_USI_INTR_EN_I2C_AERR_EN_Pos                      (17U)
#define WJ_USI_INTR_EN_I2C_AERR_EN_Msk                      (0x1U << WJ_USI_INTR_EN_I2C_AERR_EN_Pos)
#define WJ_USI_INTR_EN_I2C_AERR_EN                          WJ_USI_INTR_EN_I2C_AERR_EN_Msk
#define WJ_USI_INTR_EN_SPI_STOP_EN_Pos                      (18U)
#define WJ_USI_INTR_EN_SPI_STOP_EN_Msk                      (0x1U << WJ_USI_INTR_EN_SPI_STOP_EN_Pos)
#define WJ_USI_INTR_EN_SPI_STOP_EN                          WJ_USI_INTR_EN_SPI_STOP_EN_Msk

/*! INTR_STA  Offset 0x054*/
#define WJ_USI_INTR_STA_TX_THOLD_Pos                        (0U)
#define WJ_USI_INTR_STA_TX_THOLD_Msk                        (0x1U << WJ_USI_INTR_STA_TX_THOLD_Pos)
#define WJ_USI_INTR_STA_TX_EMPTY_Pos                        (1U)
#define WJ_USI_INTR_STA_TX_EMPTY_Msk                        (0x1U << WJ_USI_INTR_STA_TX_EMPTY_Pos)
#define WJ_USI_INTR_STA_TX_FULL_Pos                         (2U)
#define WJ_USI_INTR_STA_TX_FULL_Msk                         (0x1U << WJ_USI_INTR_STA_TX_FULL_Pos)
#define WJ_USI_INTR_STA_TX_RERR_Pos                         (3U)
#define WJ_USI_INTR_STA_TX_RERR_Msk                         (0x1U << WJ_USI_INTR_STA_TX_RERR_Pos)
#define WJ_USI_INTR_STA_TX_WERR_Pos                         (4U)
#define WJ_USI_INTR_STA_TX_WERR_Msk                         (0x1U << WJ_USI_INTR_STA_TX_WERR_Pos)
#define WJ_USI_INTR_STA_RX_THOLD_Pos                        (5U)
#define WJ_USI_INTR_STA_RX_THOLD_Msk                        (0x1U << WJ_USI_INTR_STA_RX_THOLD_Pos)
#define WJ_USI_INTR_STA_RX_EMPTY_Pos                        (6U)
#define WJ_USI_INTR_STA_RX_EMPTY_Msk                        (0x1U << WJ_USI_INTR_STA_RX_EMPTY_Pos)
#define WJ_USI_INTR_STA_RX_FULL_Pos                         (7U)
#define WJ_USI_INTR_STA_RX_FULL_Msk                         (0x1U << WJ_USI_INTR_STA_RX_FULL_Pos)
#define WJ_USI_INTR_STA_RX_RERR_Pos                         (8U)
#define WJ_USI_INTR_STA_RX_RERR_Msk                         (0x1U << WJ_USI_INTR_STA_RX_RERR_Pos)
#define WJ_USI_INTR_STA_RX_WERR_Pos                         (9U)
#define WJ_USI_INTR_STA_RX_WERR_Msk                         (0x1U << WJ_USI_INTR_STA_RX_WERR_Pos)
#define WJ_USI_INTR_STA_UART_TX_STOP_Pos                    (10U)
#define WJ_USI_INTR_STA_UART_TX_STOP_Msk                    (0x1U << WJ_USI_INTR_STA_UART_TX_STOP_Pos)
#define WJ_USI_INTR_STA_UART_RX_STOP_Pos                    (11U)
#define WJ_USI_INTR_STA_UART_RX_STOP_Msk                    (0x1U << WJ_USI_INTR_STA_UART_RX_STOP_Pos)
#define WJ_USI_INTR_STA_UART_PERR_Pos                       (12U)
#define WJ_USI_INTR_STA_UART_PERR_Msk                       (0x1U << WJ_USI_INTR_STA_UART_PERR_Pos)
#define WJ_USI_INTR_STA_I2C_STOP_Pos                        (13U)
#define WJ_USI_INTR_STA_I2C_STOP_Msk                        (0x1U << WJ_USI_INTR_STA_I2C_STOP_Pos)
#define WJ_USI_INTR_STA_I2C_NACK_Pos                        (14U)
#define WJ_USI_INTR_STA_I2C_NACK_Msk                        (0x1U << WJ_USI_INTR_STA_I2C_NACK_Pos)
#define WJ_USI_INTR_STA_I2CM_LOSE_ARBI_Pos                  (15U)
#define WJ_USI_INTR_STA_I2CM_LOSE_ARBI_Msk                  (0x1U << WJ_USI_INTR_STA_I2CM_LOSE_ARBI_Pos)
#define WJ_USI_INTR_STA_I2CS_GCALL_Pos                      (16U)
#define WJ_USI_INTR_STA_I2CS_GCALL_Msk                      (0x1U << WJ_USI_INTR_STA_I2CS_GCALL_Pos)
#define WJ_USI_INTR_STA_I2CS_AERR_Pos                       (17U)
#define WJ_USI_INTR_STA_I2CS_AERR_Msk                       (0x1U << WJ_USI_INTR_STA_I2CS_GCALL_Pos)
#define WJ_USI_INTR_STA_SPI_STOP_Pos                        (18U)
#define WJ_USI_INTR_STA_SPI_STOP_Msk                        (0x1U << WJ_USI_INTR_STA_SPI_STOP_Pos)

/*! RAW_INTR_STA  Offset 0x058*/
#define WJ_USI_RAW_INTR_STA_TX_THOLD_Pos                    (0U)
#define WJ_USI_RAW_INTR_STA_TX_THOLD_Msk                    (0x1U << WJ_USI_RAW_INTR_STA_TX_THOLD_Pos)
#define WJ_USI_RAW_INTR_STA_TX_EMPTY_Pos                    (1U)
#define WJ_USI_RAW_INTR_STA_TX_EMPTY_Msk                    (0x1U << WJ_USI_RAW_INTR_STA_TX_EMPTY_Pos)
#define WJ_USI_RAW_INTR_STA_TX_FULL_Pos                     (2U)
#define WJ_USI_RAW_INTR_STA_TX_FULL_Msk                     (0x1U << WJ_USI_RAW_INTR_STA_TX_FULL_Pos)
#define WJ_USI_RAW_INTR_STA_TX_RERR_Pos                     (3U)
#define WJ_USI_RAW_INTR_STA_TX_RERR_Msk                     (0x1U << WJ_USI_RAW_INTR_STA_TX_RERR_Pos)
#define WJ_USI_RAW_INTR_STA_TX_WERR_Pos                     (4U)
#define WJ_USI_RAW_INTR_STA_TX_WERR_Msk                     (0x1U << WJ_USI_RAW_INTR_STA_TX_WERR_Pos)
#define WJ_USI_RAW_INTR_STA_RX_THOLD_Pos                    (5U)
#define WJ_USI_RAW_INTR_STA_RX_THOLD_Msk                    (0x1U << WJ_USI_RAW_INTR_STA_RX_THOLD_Pos)
#define WJ_USI_RAW_INTR_STA_RX_EMPTY_Pos                    (6U)
#define WJ_USI_RAW_INTR_STA_RX_EMPTY_Msk                    (0x1U << WJ_USI_RAW_INTR_STA_RX_EMPTY_Pos)
#define WJ_USI_RAW_INTR_STA_RX_FULL_Pos                     (7U)
#define WJ_USI_RAW_INTR_STA_RX_FULL_Msk                     (0x1U << WJ_USI_RAW_INTR_STA_RX_FULL_Pos)
#define WJ_USI_RAW_INTR_STA_RX_RERR_Pos                     (8U)
#define WJ_USI_RAW_INTR_STA_RX_RERR_Msk                     (0x1U << WJ_USI_RAW_INTR_STA_RX_RERR_Pos)
#define WJ_USI_RAW_INTR_STA_RX_WERR_Pos                     (9U)
#define WJ_USI_RAW_INTR_STA_RX_WERR_Msk                     (0x1U << WJ_USI_RAW_INTR_STA_RX_WERR_Pos)
#define WJ_USI_RAW_INTR_STA_UART_TX_STOP_Pos                (10U)
#define WJ_USI_RAW_INTR_STA_UART_TX_STOP_Msk                (0x1U << WJ_USI_RAW_INTR_STA_UART_TX_STOP_Pos)
#define WJ_USI_RAW_INTR_STA_UART_RX_STOP_Pos                (11U)
#define WJ_USI_RAW_INTR_STA_UART_RX_STOP_Msk                (0x1U << WJ_USI_RAW_INTR_STA_UART_RX_STOP_Pos)
#define WJ_USI_RAW_INTR_STA_UART_PERR_Pos                   (12U)
#define WJ_USI_RAW_INTR_STA_UART_PERR_Msk                   (0x1U << WJ_USI_RAW_INTR_STA_UART_PERR_Pos)
#define WJ_USI_RAW_INTR_STA_I2C_STOP_Pos                    (13U)
#define WJ_USI_RAW_INTR_STA_I2C_STOP_Msk                    (0x1U << WJ_USI__AW_INTR_STA_I2C_STOP_Pos)
#define WJ_USI_RAW_INTR_STA_I2C_NACK_Pos                    (14U)
#define WJ_USI_RAW_INTR_STA_I2C_NACK_Msk                    (0x1U << WJ_USI_RAW_INTR_STA_I2C_NACK_Pos)
#define WJ_USI_RAW_INTR_STA_I2CM_LOSE_ARBI_Pos              (15U)
#define WJ_USI_RAW_INTR_STA_I2CM_LOSE_ARBI_Msk              (0x1U << WJ_USI_RAW_INTR_STA_I2CM_LOSE_ARBI_Pos)
#define WJ_USI_RAW_INTR_STA_I2CS_GCALL_Pos                  (16U)
#define WJ_USI_RAW_INTR_STA_I2CS_GCALL_Msk                  (0x1U << WJ_USI_RAW_INTR_STA_I2CS_GCALL_Pos)
#define WJ_USI_RAW_INTR_STA_I2C_AERR_Pos                    (17U)
#define WJ_USI_RAW_INTR_STA_I2C_AERR_Msk                    (0x1U << WJ_USI_RAW_INTR_STA_SPI_STOP_Pos)
#define WJ_USI_RAW_INTR_STA_SPI_STOP_Pos                    (18U)
#define WJ_USI_RAW_INTR_STA_SPI_STOP_Msk                    (0x1U << WJ_USI_RAW_INTR_STA_SPI_STOP_Pos)

/*! USI_INTR_UNMASK  Offset 0x05C*/
#define WJ_USI_INTR_UNMASK_TX_THOLD_MASK_Pos                (0U)
#define WJ_USI_INTR_UNMASK_TX_THOLD_MASK_Msk                (0x1U << WJ_USI_INTR_UNMASK_TX_THOLD_MASK_Pos)
#define WJ_USI_INTR_UNMASK_TX_THOLD_MASK                    WJ_USI_INTR_UNMASK_TX_THOLD_MASK_Msk
#define WJ_USI_INTR_UNMASK_TX_EMPTY_MASK_Pos                (1U)
#define WJ_USI_INTR_UNMASK_TX_EMPTY_MASK_Msk                (0x1U << WJ_USI_INTR_UNMASK_TX_EMPTY_MASK_Pos)
#define WJ_USI_INTR_UNMASK_TX_EMPTY_MASK                    WJ_USI_INTR_UNMASK_TX_EMPTY_MASK_Msk
#define WJ_USI_INTR_UNMASK_TX_FULL_MASK_Pos                 (2U)
#define WJ_USI_INTR_UNMASK_TX_FULL_MASK_Msk                 (0x1U << WJ_USI_INTR_UNMASK_TX_FULL_MASK_Pos)
#define WJ_USI_INTR_UNMASK_TX_FULL_MASK                     WJ_USI_INTR_UNMASK_TX_FULL_MASK_Msk
#define WJ_USI_INTR_UNMASK_TX_RERR_MASK_Pos                 (3U)
#define WJ_USI_INTR_UNMASK_TX_RERR_MASK_Msk                 (0x1U << WJ_USI_INTR_UNMASK_TX_RERR_MASK_Pos)
#define WJ_USI_INTR_UNMASK_TX_RERR_MASK                     WJ_USI_INTR_UNMASK_TX_RERR_MASK_Msk
#define WJ_USI_INTR_UNMASK_TX_WERR_MASK_Pos                 (4U)
#define WJ_USI_INTR_UNMASK_TX_WERR_MASK_Msk                 (0x1U << WJ_USI_INTR_UNMASK_TX_WERR_MASK_Pos)
#define WJ_USI_INTR_UNMASK_TX_WERR_MASK                     WJ_USI_INTR_UNMASK_TX_WERR_MASK_Msk
#define WJ_USI_INTR_UNMASK_RX_THOLD_MASK_Pos                (5U)
#define WJ_USI_INTR_UNMASK_RX_THOLD_MASK_Msk                (0x1U << WJ_USI_INTR_UNMASK_RX_THOLD_MASK_Pos)
#define WJ_USI_INTR_UNMASK_RX_THOLD_MASK                    WJ_USI_INTR_UNMASK_RX_THOLD_MASK_Msk
#define WJ_USI_INTR_UNMASK_RX_EMPTY_MASK_Pos                (6U)
#define WJ_USI_INTR_UNMASK_RX_EMPTY_MASK_Msk                (0x1U << WJ_USI_INTR_UNMASK_RX_EMPTY_MASK_Pos)
#define WJ_USI_INTR_UNMASK_RX_EMPTY_MASK                    WJ_USI_INTR_UNMASK_RX_EMPTY_MASK_Msk
#define WJ_USI_INTR_UNMASK_RX_FULL_MASK_Pos                 (7U)
#define WJ_USI_INTR_UNMASK_RX_FULL_MASK_Msk                 (0x1U << WJ_USI_INTR_UNMASK_RX_FULL_MASK_Pos)
#define WJ_USI_INTR_UNMASK_RX_FULL_MASK                     WJ_USI_INTR_UNMASK_RX_FULL_MASK_Msk
#define WJ_USI_INTR_UNMASK_RX_RERR_MASK_Pos                 (8U)
#define WJ_USI_INTR_UNMASK_RX_RERR_MASK_Msk                 (0x1U << WJ_USI_INTR_UNMASK_RX_RERR_MASK_Pos)
#define WJ_USI_INTR_UNMASK_RX_RERR_MASK                     WJ_USI_INTR_UNMASK_RX_RERR_MASK_Msk
#define WJ_USI_INTR_UNMASK_RX_WERR_MASK_Pos                 (9U)
#define WJ_USI_INTR_UNMASK_RX_WERR_MASK_Msk                 (0x1U << WJ_USI_INTR_UNMASK_RX_WERR_MASK_Pos)
#define WJ_USI_INTR_UNMASK_RX_WERR_MASK                     WJ_USI_INTR_UNMASK_RX_WERR_MASK_Msk
#define WJ_USI_INTR_UNMASK_UART_STOP_MASK_Pos               (10U)
#define WJ_USI_INTR_UNMASK_UART_STOP_MASK_Msk               (0x1U << WJ_USI_INTR_UNMASK_UART_STOP_MASK_Pos)
#define WJ_USI_INTR_UNMASK_UART_STOP_MASK                   WJ_USI_INTR_UNMASK_UART_STOP_MASK_Msk
#define WJ_USI_INTR_UNMASK_UART_PERR_MASK_Pos               (11U)
#define WJ_USI_INTR_UNMASK_UART_PERR_MASK_Msk               (0x1U << WJ_USI_INTR_UNMASK_UART_PERR_MASK_Pos)
#define WJ_USI_INTR_UNMASK_UART_PERR_MASK                   WJ_USI_INTR_UNMASK_UART_PERR_MASK_Msk
#define WJ_USI_INTR_UNMASK_I2C_STOP_MASK_Pos                (12U)
#define WJ_USI_INTR_UNMASK_I2C_STOP_MASK_Msk                (0x1U << WJ_USI_INTR_UNMASK_I2C_STOP_MASK_Pos)
#define WJ_USI_INTR_UNMASK_I2C_STOP_MASK                    WJ_USI_INTR_UNMASK_I2C_STOP_MASK_Msk
#define WJ_USI_INTR_UNMASK_I2C_NACK_MASK_Pos                (13U)
#define WJ_USI_INTR_UNMASK_I2C_NACK_MASK_Msk                (0x1U << WJ_USI_INTR_UNMASK_I2C_NACK_MASK_Pos)
#define WJ_USI_INTR_UNMASK_I2C_NACK_MASK                    WJ_USI_INTR_UNMASK_I2C_NACK_MASK_Msk
#define WJ_USI_INTR_UNMASK_I2CM_LOSE_ARBI_MASK_Pos          (14U)
#define WJ_USI_INTR_UNMASK_I2CM_LOSE_ARBI_MASK_Msk          (0x1U << WJ_USI_INTR_UNMASK_I2CM_LOSE_ARBI_MASK_Pos)
#define WJ_USI_INTR_UNMASK_I2CM_LOSE_ARBI_MASK              WJ_USI_INTR_UNMASK_I2CM_LOSE_ARBI_MASK_Msk
#define WJ_USI_INTR_UNMASK_I2CS_GCALL_MASK_Pos              (15U)
#define WJ_USI_INTR_UNMASK_I2CS_GCALL_MASK_Msk              (0x1U << WJ_USI_INTR_UNMASK_I2CS_GCALL_MASK_Pos)
#define WJ_USI_INTR_UNMASK_I2CS_GCALL_MASK                  WJ_USI_INTR_UNMASK_I2CS_GCALL_MASK_Msk
#define WJ_USI_INTR_UNMASK_I2C_AERR_MASK_Pos                (16U)
#define WJ_USI_INTR_UNMASK_I2C_AERR_MASK_Msk                (0x1U << WJ_USI_INTR_UNMASK_I2C_AERR_MASK_Pos)
#define WJ_USI_INTR_UNMASK_I2C_AERR_MASK                    WJ_USI_INTR_UNMASK_I2C_AERR_MASK_Msk
#define WJ_USI_INTR_UNMASK_SPI_STOP_MASK_Pos                (17U)
#define WJ_USI_INTR_UNMASK_SPI_STOP_MASK_Msk                (0x1U << WJ_USI_INTR_UNMASK_SPI_STOP_MASK_Pos)
#define WJ_USI_INTR_UNMASK_SPI_STOP_MASK                    WJ_USI_INTR_UNMASK_SPI_STOP_MASK_Msk

/*! INTR_CLR   Offset 0x060*/
#define WJ_USI_INTR_CLR_TX_THOLD_CLR_Pos                    (0U)
#define WJ_USI_INTR_CLR_TX_THOLD_CLR_Msk                    (0x1U << WJ_USI_INTR_CLR_TX_THOLD_CLR_Pos)
#define WJ_USI_INTR_CLR_TX_EMPTY_CLR_Pos                    (1U)
#define WJ_USI_INTR_CLR_TX_EMPTY_CLR_Msk                    (0x1U << WJ_USI_INTR_CLR_TX_EMPTY_CLR_Pos)
#define WJ_USI_INTR_CLR_TX_EMPTY_CLR                        WJ_USI_INTR_CLR_TX_EMPTY_CLR_Msk
#define WJ_USI_INTR_CLR_TX_FULL_CLR_Pos                     (2U)
#define WJ_USI_INTR_CLR_TX_FULL_CLR_Msk                     (0x1U << WJ_USI_INTR_CLR_TX_FULL_CLR_Pos)
#define WJ_USI_INTR_CLR_TX_RERR_CLR_Pos                     (3U)
#define WJ_USI_INTR_CLR_TX_RERR_CLR_Msk                     (0x1U << WJ_USI_INTR_CLR_TX_RERR_CLR_Pos)
#define WJ_USI_INTR_CLR_TX_WERR_CLR_Pos                     (4U)
#define WJ_USI_INTR_CLR_TX_WERR_CLR_Msk                     (0x1U << WJ_USI_INTR_CLR_TX_WERR_CLR_Pos)
#define WJ_USI_INTR_CLR_RX_THOLD_CLR_Pos                    (5U)
#define WJ_USI_INTR_CLR_RX_THOLD_CLR_Msk                    (0x1U << WJ_USI_INTR_CLR_RX_THOLD_CLR_Pos)
#define WJ_USI_INTR_CLR_RX_THOLD_CLR                        WJ_USI_INTR_CLR_RX_THOLD_CLR_Msk
#define WJ_USI_INTR_CLR_RX_EMPTY_CLR_Pos                    (6U)
#define WJ_USI_INTR_CLR_RX_EMPTY_CLR_Msk                    (0x1U << WJ_USI_INTR_CLR_RX_EMPTY_CLR_Pos)
#define WJ_USI_INTR_CLR_RX_FULL_CLR_Pos                     (7U)
#define WJ_USI_INTR_CLR_RX_FULL_CLR_Msk                     (0x1U << WJ_USI_INTR_CLR_RX_FULL_CLR_Pos)
#define WJ_USI_INTR_CLR_RX_RERR_CLR_Pos                     (8U)
#define WJ_USI_INTR_CLR_RX_RERR_CLR_Msk                     (0x1U << WJ_USI_INTR_CLR_RX_RERR_CLR_Pos)
#define WJ_USI_INTR_CLR_RX_WERR_CLR_Pos                     (9U)
#define WJ_USI_INTR_CLR_RX_WERR_CLR_Msk                     (0x1U << WJ_USI_INTR_CLR_RX_WERR_CLR_Pos)
#define WJ_USI_INTR_CLR_UART_TX_STOP_CLR_Pos                (10U)
#define WJ_USI_INTR_CLR_UART_TX_STOP_CLR_Msk                (0x1U << WJ_USI_INTR_CLR_UART_TX_STOP_CLR_Pos)
#define WJ_USI_INTR_CLR_UART_TX_STOP_CLR                    WJ_USI_INTR_CLR_UART_TX_STOP_CLR_Msk
#define WJ_USI_INTR_CLR_UART_RX_STOP_CLR_Pos                (11U)
#define WJ_USI_INTR_CLR_UART_RX_STOP_CLR_Msk                (0x1U << WJ_USI_INTR_CLR_UART_RX_STOP_CLR_Pos)
#define WJ_USI_INTR_CLR_UART_RX_STOP_CLR                    WJ_USI_INTR_CLR_UART_RX_STOP_CLR_Msk
#define WJ_USI_INTR_CLR_UART_PERR_CLR_Pos                   (12U)
#define WJ_USI_INTR_CLR_UART_PERR_CLR_Msk                   (0x1U << WJ_USI_INTR_CLR_UART_PERR_CLR_Pos)
#define WJ_USI_INTR_CLR_UART_PERR_CLR                       WJ_USI_INTR_CLR_UART_PERR_CLR_Msk
#define WJ_USI_INTR_CLR_I2C_STOP_CLR_Pos                    (13U)
#define WJ_USI_INTR_CLR_I2C_STOP_CLR_Msk                    (0x1U << WJ_USI_INTR_CLR_I2C_STOP_CLR_Pos)
#define WJ_USI_INTR_CLR_I2C_NACK_CLR_Pos                    (14U)
#define WJ_USI_INTR_CLR_I2C_NACK_CLR_Msk                    (0x1U << WJ_USI_INTR_CLR_I2C_NACK_CLR_Pos)
#define WJ_USI_INTR_CLR_I2CM_LOSE_ARBI_CLR_Pos              (15U)
#define WJ_USI_INTR_CLR_I2CM_LOSE_ARBI_CLR_Msk              (0x1U << WJ_USI_INTR_CLR_I2CM_LOSE_ARBI_CLR_Pos)
#define WJ_USI_INTR_CLR_I2CS_GCALL_CLR_Pos                  (16U)
#define WJ_USI_INTR_CLR_I2CS_GCALL_CLR_Msk                  (0x1U << WJ_USI_INTR_CLR_I2CS_GCALL_CLR_Pos)
#define WJ_USI_INTR_CLR_I2CS_AERR_CLR_Pos                   (17U)
#define WJ_USI_INTR_CLR_I2CS_AERR_CLR_Msk                   (0x1U << WJ_USI_INTR_CLR_I2CS_AERR_CLR_Pos)
#define WJ_USI_INTR_CLR_SPI_STOP_CLR_Pos                    (18U)
#define WJ_USI_INTR_CLR_SPI_STOP_CLR_Msk                    (0x1U << WJ_USI_INTR_CLR_SPI_STOP_CLR_Pos)

/*! DMA_CTRL   Offset 0x064*/
#define WJ_USI_DMA_CTRL_TX_DMA_EN_Pos                       (0U)
#define WJ_USI_DMA_CTRL_TX_DMA_EN_Msk                       (0x1U << WJ_USI_DMA_CTRL_TX_DMA_EN_Pos)
#define WJ_USI_DMA_CTRL_TX_DMA_EN                           WJ_USI_DMA_CTRL_TX_DMA_EN_Msk
#define WJ_USI_DMA_CTRL_RX_DMA_EN_Pos                       (1U)
#define WJ_USI_DMA_CTRL_RX_DMA_EN_Msk                       (0x1U << WJ_USI_DMA_CTRL_RX_DMA_EN_Pos)
#define WJ_USI_DMA_CTRL_RX_DMA_EN                           WJ_USI_DMA_CTRL_RX_DMA_EN_Msk

/*! DMA_THRESHOLD   Offset 0x068*/
#define WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Pos                  (0U)
#define WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Msk                  (0x1FU << WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_TX_DMA_TH_0                    (0x0U << WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_TX_DMA_TH_1                    (0x1U << WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_TX_DMA_TH_2                    (0x2U << WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_TX_DMA_TH_3                    (0x3U << WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_TX_DMA_TH_4                    (0x4U << WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_TX_DMA_TH_5                    (0x5U << WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_TX_DMA_TH_6                    (0x6U << WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_TX_DMA_TH_7                    (0x7U << WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_TX_DMA_TH_8                    (0x8U << WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Pos)

#define WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Pos                  (8U)
#define WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Msk                  (0x1FU << WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_RX_DMA_TH_0                    (0x0U << WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_RX_DMA_TH_1                    (0x1U << WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_RX_DMA_TH_2                    (0x2U << WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_RX_DMA_TH_3                    (0x3U << WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_RX_DMA_TH_4                    (0x4U << WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_RX_DMA_TH_5                    (0x5U << WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_RX_DMA_TH_6                    (0x6U << WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_RX_DMA_TH_7                    (0x7U << WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Pos)
#define WJ_USI_DMA_THRESHOLD_RX_DMA_TH_8                    (0x8U << WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Pos)

/*! SPI_NSS_DATA    Offset 0x06c*/
#define WJ_USI_SPI_NSS_DATA_NSS_DATA_Pos                    (0U)
#define WJ_USI_SPI_NSS_DATA_NSS_DATA_Msk                    (0x1U << WJ_USI_SPI_NSS_DATA_NSS_DATA_Pos)
#define WJ_USI_SPI_NSS_DATA_NSS_DATA_EN                     WJ_USI_SPI_NSS_DATA_NSS_DATA_Msk

#define WJ_USI_UART_INT_ENABLE_DEFAUL (WJ_USI_INTR_EN_UART_PERR_EN | WJ_USI_INTR_EN_UART_TX_STOP_EN | WJ_USI_INTR_EN_RX_WERR_EN | \
                                       WJ_USI_INTR_EN_TX_WERR_EN | WJ_USI_INTR_EN_TX_RERR_EN \
                                       | WJ_USI_INTR_EN_RX_RERR_EN | WJ_USI_INTR_EN_RX_FULL_EN)

#define WJ_USI_UART_INT_MASK_DEFAUL (WJ_USI_INTR_UNMASK_UART_PERR_MASK| WJ_USI_INTR_UNMASK_UART_STOP_MASK| WJ_USI_INTR_UNMASK_RX_WERR_MASK| \
                                     WJ_USI_INTR_UNMASK_TX_WERR_MASK| WJ_USI_INTR_UNMASK_TX_RERR_MASK \
                                     | WJ_USI_INTR_UNMASK_RX_RERR_MASK| WJ_USI_INTR_UNMASK_RX_FULL_MASK)

#define WJ_USI_FIFO_MAX      (8U)

typedef struct {
    __IOM uint32_t USI_CTRL;               /* Offset 0x000(R/W) */
    __IOM uint32_t USI_MODE_SEL;           /* Offset 0x004(R/W) */
    __IOM uint32_t USI_TX_RX_FIFO;         /* Offset 0x008(R/W) */
    __IOM uint32_t USI_FIFO_STA;           /* Offset 0x00c(R/W) */
    __IOM uint32_t USI_CLK_DIV0;           /* Offset 0x010(R/W) */
    __IOM uint32_t USI_CLK_DIV1;           /* Offset 0x014(R/W) */
    __IOM uint32_t USI_UART_CTRL;          /* Offset 0x018(R/W) */
    __IOM uint32_t USI_UART_STA;           /* Offset 0x01c(R/W) */
    __IOM uint32_t USI_I2C_MODE;           /* Offset 0x020(R/W) */
    __IOM uint32_t USI_I2C_ADDR;           /* Offset 0x024(R/W) */
    __IOM uint32_t USI_I2CM_CTRL;          /* Offset 0x028(R/W) */
    __IOM uint32_t USI_I2CM_CODE;          /* Offset 0x02c(R/W) */
    __IOM uint32_t USI_I2CS_CTRL;          /* Offset 0x030(R/W) */
    __IOM uint32_t USI_I2C_FS_DIV;         /* Offset 0x034(R/W) */
    __IOM uint32_t USI_I2C_HOLD;           /* Offset 0x038(R/W) */
    __IOM uint32_t USI_I2C_STA;            /* Offset 0x03c(R/W) */
    __IOM uint32_t USI_SPI_MODE;           /* Offset 0x040(R/W) */
    __IOM uint32_t USI_SPI_CTRL;           /* Offset 0x044(R/W) */
    __IOM uint32_t USI_SPI_STA;            /* Offset 0x048(R/W) */
    __IOM uint32_t USI_INTR_CTRL;          /* Offset 0x04c(R/W) */
    __IOM uint32_t USI_INTR_EN;            /* Offset 0x050(R/W) */
    __IOM uint32_t USI_INTR_STA;           /* Offset 0x054(R/W) */
    __IOM uint32_t USI_RAW_INTR_STA;       /* Offset 0x058(R/W) */
    __IOM uint32_t USI_INTR_UNMASK;        /* Offset 0x05c(R/W) */
    __IOM uint32_t USI_INTR_CLR;           /* Offset 0x060(R/W) */
    __IOM uint32_t USI_DMA_CTRL;           /* Offset 0x064(R/W) */
    __IOM uint32_t USI_DMA_THRESHOLD;      /* Offset 0x068(R/W) */
    __IOM uint32_t USI_SPI_NSS_DATA;       /* Offset 0x06C(R/W) */
} ck_usi_regs_t;

static inline void  wj_usi_set_ctrl(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_CTRL = value & WJ_USI_CTRL_Msk;
}

static inline void  wj_usi_en_ctrl_usi_en(ck_usi_regs_t *addr)
{
    addr->USI_CTRL |= WJ_USI_CTRL_USI_EN_Msk;
}

static inline void  wj_usi_dis_ctrl_usi_en(ck_usi_regs_t *addr)
{
    addr->USI_CTRL &= (~WJ_USI_CTRL_USI_EN_Msk);
}

static inline uint32_t  wj_usi_get_ctrl_usi_en(ck_usi_regs_t *addr)
{
    return (addr->USI_CTRL & WJ_USI_CTRL_USI_EN_Msk);
}

static inline void  wj_usi_en_ctrl_fm_en(ck_usi_regs_t *addr)
{
    addr->USI_CTRL |= WJ_USI_CTRL_FM_EN_Msk;
}

static inline void  wj_usi_dis_ctrl_fm_en(ck_usi_regs_t *addr)
{
    addr->USI_CTRL &= (~WJ_USI_CTRL_FM_EN_Msk);
}

static inline uint32_t  wj_usi_get_ctrl_fm_en(ck_usi_regs_t *addr)
{
    return (addr->USI_CTRL & WJ_USI_CTRL_FM_EN_Msk);
}

static inline void  wj_usi_en_ctrl_tx_fifo_en(ck_usi_regs_t *addr)
{
    addr->USI_CTRL |= WJ_USI_CTRL_TX_FIFO_EN_Msk;
}

static inline void  wj_usi_dis_ctrl_tx_fifo_en(ck_usi_regs_t *addr)
{
    addr->USI_CTRL &= (~WJ_USI_CTRL_TX_FIFO_EN_Msk);
}

static inline uint32_t  wj_usi_get_ctrl_tx_fifo_en(ck_usi_regs_t *addr)
{
    return (addr->USI_CTRL & WJ_USI_CTRL_TX_FIFO_EN_Msk);
}

static inline void  wj_usi_en_ctrl_rx_fifo_en(ck_usi_regs_t *addr)
{
    addr->USI_CTRL |= WJ_USI_CTRL_RX_FIFO_EN_Msk;
}

static inline void  wj_usi_dis_ctrl_rx_fifo_en(ck_usi_regs_t *addr)
{
    addr->USI_CTRL &= (~WJ_USI_CTRL_RX_FIFO_EN_Msk);
}

static inline uint32_t  wj_usi_get_ctrl_rx_fifo_en(ck_usi_regs_t *addr)
{
    return (addr->USI_CTRL & WJ_USI_CTRL_RX_FIFO_EN_Msk);
}
/*MODE_SEL Offset 0x004*/

static inline void  wj_usi_set_mode_sel(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_MODE_SEL &= (~WJ_USI_MODE_SEL_Msk);
    addr->USI_MODE_SEL |= (value & WJ_USI_MODE_SEL_Msk);
}

static inline uint32_t  wj_usi_get_mode_sel(ck_usi_regs_t *addr)
{
    return (addr->USI_MODE_SEL & WJ_USI_MODE_SEL_Msk);
}

static inline void  wj_usi_set_tx_rx_fifo(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_TX_RX_FIFO = value & WJ_USI_TX_RX_FIFO_Msk;
}

static inline uint32_t  wj_usi_get_tx_rx_fifo(ck_usi_regs_t *addr)
{
    return (addr->USI_TX_RX_FIFO & WJ_USI_TX_RX_FIFO_Msk);
}

static inline uint32_t  wj_usi_get_fifo_sta_tx_empty(ck_usi_regs_t *addr)
{
    return (addr->USI_FIFO_STA & WJ_USI_FIFO_STA_TX_EMPTY_Msk);
}

static inline uint32_t  wj_usi_get_fifo_sta_tx_full(ck_usi_regs_t *addr)
{
    return (addr->USI_FIFO_STA & WJ_USI_FIFO_STA_TX_FULL_Msk);
}

static inline uint32_t  wj_usi_get_fifo_sta_rx_empty(ck_usi_regs_t *addr)
{
    return (addr->USI_FIFO_STA & WJ_USI_FIFO_STA_RX_EMPTY_Msk);
}

static inline uint32_t  wj_usi_get_fifo_sta_rx_full(ck_usi_regs_t *addr)
{
    return (addr->USI_FIFO_STA & WJ_USI_FIFO_STA_RX_FULL_Msk);
}

static inline uint32_t  wj_usi_get_fifo_sta_tx_num(ck_usi_regs_t *addr)
{
    return ((addr->USI_FIFO_STA & WJ_USI_FIFO_STA_TX_NUM_Msk) >> WJ_USI_FIFO_STA_TX_NUM_Pos);
}

static inline uint32_t  wj_usi_get_fifo_sta_rx_num(ck_usi_regs_t *addr)
{
    return ((addr->USI_FIFO_STA & WJ_USI_FIFO_STA_RX_NUM_Msk) >> WJ_USI_FIFO_STA_RX_NUM_Pos);
}

static inline void  wj_usi_set_clk_div0(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_CLK_DIV0 = WJ_USI_CLK_DIV0_Msk & value;
}

static inline uint32_t  wj_usi_get_clk_div0(ck_usi_regs_t *addr)
{
    return (addr->USI_CLK_DIV0 & WJ_USI_CLK_DIV0_Msk);
}

static inline void  wj_usi_set_clk_div1(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_CLK_DIV1 = WJ_USI_CLK_DIV1_Msk & value;
}

static inline uint32_t  wj_usi_get_clk_div1(ck_usi_regs_t *addr)
{
    return (addr->USI_CLK_DIV1 & WJ_USI_CLK_DIV1_Msk);
}

static inline void  wj_usi_set_uart_ctrl_dbit(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_UART_CTRL &= (~WJ_USI_UART_CTRL_DBIT_Msk);
    addr->USI_UART_CTRL |= WJ_USI_UART_CTRL_DBIT_Msk & value;
}

static inline uint32_t  wj_usi_get_uart_ctrl_dbit(ck_usi_regs_t *addr)
{
    return (addr->USI_UART_CTRL & WJ_USI_UART_CTRL_DBIT_Msk);
}

static inline void  wj_usi_set_uart_ctrl_pbit(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_UART_CTRL &= (~WJ_USI_UART_CTRL_PBIT_Msk);
    addr->USI_UART_CTRL |= WJ_USI_UART_CTRL_PBIT_Msk & value;
}

static inline uint32_t  wj_usi_get_uart_ctrl_pbit(ck_usi_regs_t *addr)
{
    return (addr->USI_UART_CTRL & WJ_USI_UART_CTRL_PBIT_Msk);
}

static inline void  wj_usi_en_uart_ctrl_pen(ck_usi_regs_t *addr)
{
    addr->USI_UART_CTRL &= (~WJ_USI_UART_CTRL_PEN_Msk);
}

static inline void  wj_usi_dis_uart_ctrl_pen(ck_usi_regs_t *addr)
{
    addr->USI_UART_CTRL &= (~WJ_USI_UART_CTRL_PEN_Msk);
}

static inline uint32_t  wj_usi_get_uart_ctrl_pen(ck_usi_regs_t *addr)
{
    return (addr->USI_UART_CTRL & WJ_USI_UART_CTRL_PEN_Msk);
}

static inline void  wj_usi_en_uart_ctrl_eps(ck_usi_regs_t *addr)//ODD
{
    addr->USI_UART_CTRL &= (~WJ_USI_UART_CTRL_EPS_Msk);
}

static inline void  wj_usi_dis_uart_ctrl_eps(ck_usi_regs_t *addr)//EVEN
{
    addr->USI_UART_CTRL &= (~WJ_USI_UART_CTRL_EPS_Msk);
}

static inline uint32_t  wj_usi_get_uart_ctrl_eps(ck_usi_regs_t *addr)
{
    return (addr->USI_UART_CTRL & WJ_USI_UART_CTRL_EPS_Msk);
}

static inline uint32_t wj_usi_get_uart_sta_tx_work(ck_usi_regs_t *addr)
{
    return (addr->USI_UART_STA & WJ_USI_UART_STA_TXD_WORK_Msk);
}

static inline uint32_t  wj_usi_uart_sta_rx_work(ck_usi_regs_t *addr)
{
    return (addr->USI_UART_STA & WJ_USI_UART_STA_TXD_WORK_Msk);
}

static inline void  wj_usi_set_i2c_mode_master(ck_usi_regs_t *addr)
{
    addr->USI_I2C_MODE |= WJ_USI_I2C_MODE_MASTER;
}

static inline void  wj_usi_set_i2c_mode_slave(ck_usi_regs_t *addr)
{
    addr->USI_I2C_MODE &= (~WJ_USI_I2C_MODE_MASTER);
}

static inline uint32_t  wj_usi_get_i2c_mode(ck_usi_regs_t *addr)
{
    return (addr->USI_I2C_MODE & WJ_USI_I2C_MODE_Msk);
}

static inline void  wj_usi_set_i2c_addr(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_I2C_ADDR = value & WJ_USI_I2C_ADDR_Msk;
}
/* I2CM_CTRL Offset 0x028*/

static inline void wj_usi_set_i2cm_ctrl_addr_mode_10bit(ck_usi_regs_t *addr)
{
    addr->USI_I2CM_CTRL |= WJ_USI_I2CM_CTRL_ADDR_MODE_10BIT;
}

static inline void wj_usi_set_i2cm_ctrl_addr_mode_7bit(ck_usi_regs_t *addr)
{
    addr->USI_I2CM_CTRL &= (~WJ_USI_I2CM_CTRL_ADDR_MODE_10BIT);
}

static inline uint32_t wj_usi_get_i2cm_ctrl_addr_mode(ck_usi_regs_t *addr)
{
    return (addr->USI_I2CM_CTRL & WJ_USI_I2CM_CTRL_ADDR_MODE_Msk);
}

static inline void wj_usi_en_i2cm_ctrl_stop(ck_usi_regs_t *addr)
{
    addr->USI_I2CM_CTRL |= WJ_USI_I2CM_CTRL_STOP;
}

static inline void wj_usi_dis_i2cm_ctrl_stop(ck_usi_regs_t *addr)
{
    addr->USI_I2CM_CTRL &= (~WJ_USI_I2CM_CTRL_STOP);
}

static inline uint32_t wj_usi_get_i2cm_ctrl_stop(ck_usi_regs_t *addr)
{
    return (addr->USI_I2CM_CTRL & WJ_USI_I2CM_CTRL_STOP_Msk);
}

static inline void wj_usi_en_i2cm_ctrl_addr_sbyte(ck_usi_regs_t *addr)
{
    addr->USI_I2CM_CTRL |= WJ_USI_I2CM_CTRL_SBYTE;
}

static inline void wj_usi_dis_i2cm_ctrl_addr_sbyte(ck_usi_regs_t *addr)
{
    addr->USI_I2CM_CTRL &= (~WJ_USI_I2CM_CTRL_SBYTE);
}

static inline void wj_usi_set_i2cm_addr_code(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_I2CM_CODE &= (~WJ_USI_I2CM_CODE_MCODE_Msk);
    addr->USI_I2CM_CODE |= (value & WJ_USI_I2CM_CODE_MCODE_Msk);
}

static inline uint32_t wj_usi_get_i2cm_addr_code(ck_usi_regs_t *addr)
{
    return (addr->USI_I2CM_CODE & WJ_USI_I2CM_CODE_MCODE_Msk);
}
/* I2CS_CTRL Offset 0x030*/
static inline void wj_usi_en_i2cs_ctrl_addr_start(ck_usi_regs_t *addr)
{
    addr->USI_I2CS_CTRL |= WJ_USI_I2CS_CTRL_START_BYTE_EN;
}

static inline void wj_usi_dis_i2cs_ctrl_addr_start(ck_usi_regs_t *addr)
{
    addr->USI_I2CS_CTRL &= (~WJ_USI_I2CS_CTRL_START_BYTE_EN);
}

static inline void wj_usi_set_i2cs_ctrl_addr_mode_7bit(ck_usi_regs_t *addr)
{
    addr->USI_I2CS_CTRL |= WJ_USI_I2CS_CTRL_ADDR_MODE_7BIT;
}

static inline void wj_usi_set_i2cs_ctrl_addr_mode_auto(ck_usi_regs_t *addr)
{
    addr->USI_I2CS_CTRL &= (~WJ_USI_I2CS_CTRL_ADDR_MODE_7BIT);
}

static inline uint32_t wj_usi_get_i2cs_ctrl_addr_mode(ck_usi_regs_t *addr)
{
    return (addr->USI_I2CS_CTRL & WJ_USI_I2CS_CTRL_ADDR_MODE_Msk);
}

static inline void wj_usi_en_i2cs_ctrl_addr_gcall_mode(ck_usi_regs_t *addr)
{
    addr->USI_I2CS_CTRL |= WJ_USI_I2CS_CTRL_GCALL_MODE_EN;
}

static inline void wj_usi_dis_i2cs_ctrl_addr_gcall_mode(ck_usi_regs_t *addr)
{
    addr->USI_I2CS_CTRL &= (~WJ_USI_I2CS_CTRL_GCALL_MODE_EN);
}

static inline uint32_t wj_usi_get_i2cs_ctrl_addr_gcall_mode(ck_usi_regs_t *addr)
{
    return (addr->USI_I2CS_CTRL & WJ_USI_I2CS_CTRL_GCALL_MODE_Msk);
}

static inline void wj_usi_set_i2c_fs_div(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_I2C_FS_DIV &= (~WJ_USI_I2C_FS_DIV_Msk);
    addr->USI_I2C_FS_DIV |= (value & WJ_USI_I2C_FS_DIV_Msk);
}

static inline uint32_t wj_usi_get_i2c_fs_div(ck_usi_regs_t *addr)
{
    return (addr->USI_I2C_FS_DIV & WJ_USI_I2C_FS_DIV_Msk);
}

static inline void wj_usi_set_i2c_hold(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_I2C_HOLD &= (~WJ_USI_I2C_FS_DIV_Msk);
    addr->USI_I2C_HOLD |= (value & WJ_USI_I2C_FS_DIV_Msk);
}

static inline uint32_t wj_usi_get_i2c_hold(ck_usi_regs_t *addr)
{
    return (addr->USI_I2C_HOLD & WJ_USI_I2C_FS_DIV_Msk);
}

static inline uint32_t wj_usi_get_i2c_sta_i2cm_work(ck_usi_regs_t *addr)
{
    return (addr->USI_I2C_STA & WJ_USI_I2C_STA_I2CM_WORK_Msk);
}

static inline uint32_t wj_usi_get_i2c_sta_i2cm_data(ck_usi_regs_t *addr)
{
    return (addr->USI_I2C_STA & WJ_USI_I2C_STA_I2CM_DATA_Msk);
}

static inline uint32_t wj_usi_get_usi_i2c_sta_i2cs_work(ck_usi_regs_t *addr)
{
    return (addr->USI_I2C_STA & WJ_USI_I2C_STA_I2CS_WORK_Msk);
}

static inline void wj_usi_set_spi_mode_master(ck_usi_regs_t *addr)
{
    addr->USI_SPI_MODE |= WJ_USI_SPI_MODE_MASTER;
}

static inline void wj_usi_set_spi_mode_slave(ck_usi_regs_t *addr)
{
    addr->USI_SPI_MODE &= (~WJ_USI_SPI_MODE_MASTER);
}

static inline uint32_t wj_usi_get_spi_mode(ck_usi_regs_t *addr)
{
    return (addr->USI_SPI_MODE & WJ_USI_SPI_MODE_Msk);
}

static inline void wj_usi_set_spi_ctrl_data_size(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_SPI_CTRL  &= (~WJ_USI_SPI_CTRL_DATA_SIZE_Msk);
    addr->USI_SPI_CTRL  |= (value & WJ_USI_SPI_CTRL_DATA_SIZE_Msk);
}

static inline uint32_t wj_usi_get_spi_ctrl_data_size(ck_usi_regs_t *addr)
{
    return (addr->USI_SPI_CTRL & WJ_USI_SPI_CTRL_DATA_SIZE_Msk);
}

static inline void wj_usi_set_spi_ctrl_tmode(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_SPI_CTRL  &= (~WJ_USI_SPI_CTRL_TMOD_Msk);
    addr->USI_SPI_CTRL  |= (value & WJ_USI_SPI_CTRL_TMOD_Msk);
}

static inline uint32_t wj_usi_get_spi_ctrl_tmode(ck_usi_regs_t *addr)
{
    return (addr->USI_SPI_CTRL & WJ_USI_SPI_CTRL_TMOD_Msk);
}

static inline void wj_usi_en_spi_ctrl_cpha(ck_usi_regs_t *addr)
{
    addr->USI_SPI_CTRL  |= WJ_USI_SPI_CTRL_CPHA_EN;
}

static inline void wj_usi_dis_spi_ctrl_cpha(ck_usi_regs_t *addr)
{
    addr->USI_SPI_CTRL  &= (~WJ_USI_SPI_CTRL_CPHA_EN);
}

static inline void wj_usi_en_spi_ctrl_cpol(ck_usi_regs_t *addr)
{
    addr->USI_SPI_CTRL  |= WJ_USI_SPI_CTRL_CPOL_EN;
}

static inline void wj_usi_dis_spi_ctrl_cpol(ck_usi_regs_t *addr)
{
    addr->USI_SPI_CTRL  &= (~WJ_USI_SPI_CTRL_CPOL_EN);
}

static inline void wj_usi_en_spi_ctrl_nss_toggle(ck_usi_regs_t *addr)
{
    addr->USI_SPI_CTRL  |= WJ_USI_SPI_CTRL_NSS_TOGGLE_EN;
}

static inline void wj_usi_dis_spi_ctrl_nss_toggle(ck_usi_regs_t *addr)
{
    addr->USI_SPI_CTRL  &= (~WJ_USI_SPI_CTRL_NSS_TOGGLE_EN);
}

static inline void wj_usi_en_nss(ck_usi_regs_t *addr)
{
    addr->USI_SPI_CTRL  |= WJ_USI_SPI_CTRL_NSS_CTRL_EN;
}

static inline void wj_usi_dis_nss(ck_usi_regs_t *addr)
{
    addr->USI_SPI_CTRL  &= (~WJ_USI_SPI_CTRL_NSS_CTRL_EN);
}

static inline uint32_t wj_usi_get_spi_sta_working(ck_usi_regs_t *addr)
{
    return (addr->USI_SPI_STA & WJ_USI_SPI_STA_SPI_WORKING_Msk);
}

static inline void  wj_usi_set_tx_fifo_th(ck_usi_regs_t *addr, uint32_t value)
{
    uint32_t tmp =addr->USI_INTR_CTRL;
    tmp &= (~WJ_USI_INTR_CTRL_TX_FIFO_TH_Msk);
    tmp |=value;
    addr->USI_INTR_CTRL =tmp;
}

static inline uint32_t  wj_usi_get_tx_fifo_th(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_CTRL & WJ_USI_INTR_CTRL_TX_FIFO_TH_Msk);
}

static inline void  wj_usi_set_rx_fifo_th(ck_usi_regs_t *addr, uint32_t value)
{
    uint32_t tmp =addr->USI_INTR_CTRL;
    tmp &= (~WJ_USI_INTR_CTRL_RX_FIFO_TH_Msk);
    tmp |=value;
    addr->USI_INTR_CTRL =tmp;
}

static inline uint32_t  wj_usi_get_rx_fifo_th(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_CTRL & WJ_USI_INTR_CTRL_RX_FIFO_TH_Msk);
}

static inline void  wj_usi_set_rx_edge_ge(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CTRL &= (~WJ_USI_INTR_CTRL_INTR_EDGE_Msk);
    addr->USI_INTR_CTRL |= WJ_USI_INTR_CTRL_INTR_EDGE_GE;
}

static inline void  wj_usi_set_rx_edge_le(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CTRL &= (~WJ_USI_INTR_CTRL_INTR_EDGE_Msk);
    addr->USI_INTR_CTRL |= WJ_USI_INTR_CTRL_INTR_EDGE_LE;
}

static inline uint32_t  wj_usi_get_rx_edge(ck_usi_regs_t *addr, uint32_t value)
{
    return (addr->USI_INTR_CTRL & WJ_USI_INTR_CTRL_INTR_EDGE_Msk);
}

static inline void wj_usi_set_intr_en(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_INTR_EN = value;
}


static inline uint32_t wj_usi_get_intr_en(ck_usi_regs_t *addr)
{
    return addr->USI_INTR_EN;
}

static inline void wj_usi_en_intr_tx_thold(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_TX_THOLD_EN_Msk;
}

static inline void wj_usi_dis_intr_tx_thold(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_TX_THOLD_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_tx_thold(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_TX_THOLD_EN_Msk);
}

static inline void wj_usi_en_intr_tx_empty(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_TX_EMPTY_EN_Msk;
}

static inline void wj_usi_dis_intr_tx_empty(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_TX_EMPTY_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_tx_empty(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_TX_EMPTY_EN_Msk);
}

static inline void wj_usi_en_intr_tx_full(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_TX_FULL_EN_Msk;
}

static inline void wj_usi_dis_intr_tx_full(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_TX_FULL_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_tx_full(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_TX_FULL_EN_Msk);
}

static inline void wj_usi_en_intr_tx_rerr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_TX_RERR_EN_Msk;
}

static inline void wj_usi_dis_intr_tx_rerr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_TX_RERR_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_tx_rerr(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_TX_RERR_EN_Msk);
}

static inline void wj_usi_en_intr_tx_werr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_TX_WERR_EN_Msk;
}

static inline void wj_usi_dis_intr_tx_werr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_TX_WERR_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_tx_werr(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_TX_WERR_EN_Msk);
}

static inline void wj_usi_en_intr_rx_thold(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_RX_THOLD_EN_Msk;
}

static inline void wj_usi_dis_intr_rx_thold(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_RX_THOLD_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_rx_thold(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_RX_THOLD_EN_Msk);
}

static inline void wj_usi_en_intr_rx_empty(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_RX_EMPTY_EN_Msk;
}

static inline void wj_usi_dis_intr_rx_empty(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_RX_EMPTY_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_rx_empty(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_RX_EMPTY_EN_Msk);
}

static inline void wj_usi_en_intr_rx_full(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_RX_FULL_EN_Msk;
}

static inline void wj_usi_dis_intr_rx_full(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_RX_FULL_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_rx_full(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_RX_FULL_EN_Msk);
}

static inline void wj_usi_en_intr_rx_rerr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_RX_RERR_EN_Msk;
}

static inline void wj_usi_dis_intr_rx_rerr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_RX_RERR_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_rx_rerr(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_RX_RERR_EN_Msk);
}

static inline void wj_usi_en_intr_rx_werr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_RX_WERR_EN_Msk;
}

static inline void wj_usi_dis_intr_rx_werr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_RX_WERR_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_rx_werr(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_RX_WERR_EN_Msk);
}

static inline void wj_usi_en_intr_uart_tx_stop(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_UART_TX_STOP_EN_Msk;
}

static inline void wj_usi_dis_intr_uart_tx_stop(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_UART_TX_STOP_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_uart_tx_stop(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_UART_TX_STOP_EN_Msk);
}

static inline void wj_usi_en_intr_uart_rx_stop(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_UART_RX_STOP_EN_Msk;
}

static inline void wj_usi_dis_intr_uart_rx_stop(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_UART_RX_STOP_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_uart_rx_stop(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_UART_RX_STOP_EN_Msk);
}

static inline void wj_usi_en_intr_uart_perr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_UART_PERR_EN_Msk;
}

static inline void wj_usi_dis_intr_uart_perr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_UART_PERR_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_uart_perr(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_UART_PERR_EN_Msk);
}

static inline void wj_usi_en_intr_i2c_stop(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_I2C_STOP_EN_Msk;
}

static inline void wj_usi_dis_intr_i2c_stop(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_I2C_STOP_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_i2c_stop(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_I2C_STOP_EN_Msk);
}

static inline void wj_usi_en_intr_i2c_nack(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_I2C_NACK_EN_Msk;
}

static inline void wj_usi_dis_intr_i2c_nack(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_I2C_NACK_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_i2c_nack(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_I2C_NACK_EN_Msk);
}

static inline void wj_usi_en_intr_i2cm_arbi_en(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_I2CM_LOSE_ARBI_EN_Msk;
}

static inline void wj_usi_dis_intr_i2cm_arbi_en(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_I2CM_LOSE_ARBI_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_i2cm_arbi_en(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_I2CM_LOSE_ARBI_EN_Msk);
}

static inline void wj_usi_en_intr_i2cs_gcall_en(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_I2CS_GCALL_EN_Msk;
}

static inline void wj_usi_dis_intr_i2cs_gcall_en(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_I2CS_GCALL_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_i2cs_gcall_en(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_I2CS_GCALL_EN_Msk);
}

static inline void wj_usi_en_intr_i2c_aerr_en(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_I2C_AERR_EN_Msk;
}

static inline void wj_usi_dis_intr_i2c_aerr_en(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_I2C_AERR_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_i2c_aerr_en(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_I2C_AERR_EN_Msk);
}

static inline void wj_usi_en_intr_en_spi_stop_en(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN |= WJ_USI_INTR_EN_SPI_STOP_EN_Msk;
}

static inline void wj_usi_dis_intr_en_spi_stop_en(ck_usi_regs_t *addr)
{
    addr->USI_INTR_EN &= (~WJ_USI_INTR_EN_SPI_STOP_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_en_spi_stop_en(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_EN & WJ_USI_INTR_EN_SPI_STOP_EN_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta(ck_usi_regs_t *addr)
{
    return (addr->USI_RAW_INTR_STA);
}

static inline uint32_t  wj_usi_get_intr_sta_rx_thold(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_RX_THOLD_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_tx_thold(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_TX_THOLD_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_rx_full(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_RX_FULL_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_rx_empty(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_RX_EMPTY_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_tx_full(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_TX_FULL_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_tx_empty(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_TX_EMPTY_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_tx_rerr(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_TX_RERR_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_tx_werr(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_TX_WERR_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_rx_rerr(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_RX_RERR_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_rx_werr(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_RX_WERR_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_uart_tx_stop(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_UART_TX_STOP_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_uart_rx_stop(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_UART_RX_STOP_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_uart_perr(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_UART_PERR_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_i2c_stop(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_I2C_STOP_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_i2c_nack_arbi(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_I2C_NACK_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_i2cm_lose_arbi(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_I2CM_LOSE_ARBI_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_i2cs_gcall(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_I2CS_GCALL_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_i2c_aerr(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_I2CS_AERR_Msk);
}

static inline uint32_t  wj_usi_get_intr_sta_spi_stop(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_STA & WJ_USI_INTR_STA_SPI_STOP_Msk);
}

static inline void wj_usi_set_intr_unmask(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_INTR_UNMASK = value;
}

static inline void wj_usi_en_tx_thold_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_TX_THOLD_MASK_Msk);
}

static inline void wj_usi_dis_tx_thold_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_TX_THOLD_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_tx_thold_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_TX_THOLD_MASK_Msk);
}

static inline void wj_usi_en_rx_thold_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_RX_THOLD_MASK_Msk);
}

static inline void wj_usi_dis_rx_thold_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_RX_THOLD_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_rx_thold_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_RX_THOLD_MASK_Msk);
}

static inline void wj_usi_en_rx_empty_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_RX_EMPTY_MASK_Msk);
}

static inline void wj_usi_dis_rx_empty_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_RX_EMPTY_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_rx_empty_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_RX_EMPTY_MASK_Msk);
}

static inline void wj_usi_en_rx_full_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_RX_FULL_MASK_Msk);
}

static inline void wj_usi_dis_rx_full_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_RX_FULL_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_rx_full_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_RX_FULL_MASK_Msk);
}

static inline void wj_usi_en_tx_empty_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_TX_EMPTY_MASK_Msk);
}

static inline void wj_usi_dis_tx_empty_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_TX_EMPTY_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_tx_empty_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_TX_EMPTY_MASK_Msk);
}

static inline void wj_usi_en_tx_full_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_TX_FULL_MASK_Msk);
}

static inline void wj_usi_dis_tx_full_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_TX_FULL_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_tx_full_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_TX_FULL_MASK_Msk);
}

static inline void wj_usi_en_tx_rerr_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_TX_RERR_MASK_Msk);
}

static inline void wj_usi_dis_tx_rerr_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_TX_RERR_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_tx_rerr_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_TX_RERR_MASK_Msk);
}

static inline void wj_usi_en_tx_werr_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_TX_WERR_MASK_Msk);
}

static inline void wj_usi_dis_tx_werr_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_TX_WERR_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_tx_werr_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_TX_WERR_MASK_Msk);
}

static inline void wj_usi_en_rx_rerr_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_RX_RERR_MASK_Msk);
}

static inline void wj_usi_dis_rx_rerr_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_RX_RERR_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_rx_rerr_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_RX_RERR_MASK_Msk);
}

static inline void wj_usi_en_rx_werr_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_RX_WERR_MASK_Msk);
}

static inline void wj_usi_dis_rx_werr_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_RX_WERR_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_rx_werr_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_RX_WERR_MASK_Msk);
}

static inline void wj_usi_en_uart_stop_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_UART_STOP_MASK_Msk);
}

static inline void wj_usi_dis_uart_tx_stop_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_UART_STOP_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_uart_stop_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_UART_STOP_MASK_Msk);
}

static inline void wj_usi_en_uart_perr_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_UART_PERR_MASK_Msk);
}

static inline void wj_usi_dis_uart_perr_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_UART_PERR_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_uart_perr_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_UART_PERR_MASK_Msk);
}

static inline void wj_usi_en_i2c_stop_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_I2C_STOP_MASK_Msk);
}

static inline void wj_usi_dis_i2c_stop_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_I2C_STOP_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_i2c_stop_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_I2C_STOP_MASK_Msk);
}

static inline void wj_usi_en_i2c_nack_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_I2C_NACK_MASK_Msk);
}

static inline void wj_usi_dis_i2c_nack_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_I2C_NACK_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_i2c_nack_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_I2C_NACK_MASK_Msk);
}

static inline void wj_usi_en_i2cm_lose_arbi_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_I2CM_LOSE_ARBI_MASK_Msk);
}

static inline void wj_usi_dis_i2cm_lose_arbi_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_I2CM_LOSE_ARBI_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_i2cm_lose_arbi_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_I2CM_LOSE_ARBI_MASK_Msk);
}

static inline void wj_usi_en_i2cs_gcall_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_I2CS_GCALL_MASK_Msk);
}

static inline void wj_usi_dis_i2cs_gcall_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_I2CS_GCALL_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_i2cs_gcall_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_I2CS_GCALL_MASK_Msk);
}

static inline void wj_usi_en_i2c_aerr_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_I2C_AERR_MASK_Msk);
}

static inline void wj_usi_dis_i2c_aerr_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_I2C_AERR_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_i2c_aerr_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_I2C_AERR_MASK_Msk);
}

static inline void wj_usi_en_spi_stop_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK &= (~WJ_USI_INTR_UNMASK_SPI_STOP_MASK_Msk);
}

static inline void wj_usi_dis_spi_stop_mask(ck_usi_regs_t *addr)
{
    addr->USI_INTR_UNMASK |= WJ_USI_INTR_UNMASK_SPI_STOP_MASK_Msk;
}

static inline uint32_t  wj_usi_get_intr_unmask_spi_stop_mask(ck_usi_regs_t *addr)
{
    return (addr->USI_INTR_UNMASK & WJ_USI_INTR_UNMASK_SPI_STOP_MASK_Msk);
}

static inline void wj_usi_set_intr_clr(ck_usi_regs_t *addr, uint32_t value)
{
    addr->USI_INTR_CLR = value;
}

static inline void wj_usi_set_intr_clr_tx_thold_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_TX_THOLD_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_rx_thold_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_RX_THOLD_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_rx_empty_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_RX_EMPTY_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_rx_full_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_RX_FULL_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_tx_empty_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_TX_EMPTY_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_tx_full_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_TX_FULL_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_tx_rerr_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_TX_RERR_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_tx_werr_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_TX_WERR_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_rx_rerr_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_RX_RERR_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_rx_werr_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_RX_WERR_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_uart_tx_stop_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_UART_TX_STOP_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_uart_rx_stop_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_UART_RX_STOP_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_uart_perr_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_UART_PERR_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_i2c_stop_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_I2C_STOP_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_i2c_nack_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_I2C_NACK_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_i2cm_lose_arbi_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_I2CM_LOSE_ARBI_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_i2cs_gcall_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_I2CS_GCALL_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_i2cs_aerr_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_I2CS_AERR_CLR_Msk;
}

static inline void wj_usi_set_intr_clr_spi_stop_clr(ck_usi_regs_t *addr)
{
    addr->USI_INTR_CLR |= WJ_USI_INTR_CLR_SPI_STOP_CLR_Msk;
}

static inline void wj_usi_en_dma_ctrl_tx(ck_usi_regs_t *addr)
{
    addr->USI_DMA_CTRL |= WJ_USI_DMA_CTRL_TX_DMA_EN;
}

static inline void wj_usi_en_dma_ctrl_rx(ck_usi_regs_t *addr)
{
    addr->USI_DMA_CTRL |= WJ_USI_DMA_CTRL_RX_DMA_EN;
}

static inline void wj_usi_dis_dma_ctrl_tx(ck_usi_regs_t *addr)
{
    addr->USI_DMA_CTRL &= ~WJ_USI_DMA_CTRL_TX_DMA_EN;
}

static inline void wj_usi_dis_dma_ctrl_rx(ck_usi_regs_t *addr)
{
    addr->USI_DMA_CTRL &= ~WJ_USI_DMA_CTRL_RX_DMA_EN;
}

static inline void wj_usi_set_dma_threshold_tx(ck_usi_regs_t *addr, uint32_t value)
{
    uint32_t temp = addr->USI_DMA_THRESHOLD;
    temp &= ~WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Msk;
    temp |= (value << WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Pos) & WJ_USI_DMA_THRESHOLD_TX_DMA_TH_Msk;
    addr->USI_DMA_THRESHOLD = temp;
}

static inline void wj_usi_set_dma_threshold_rx(ck_usi_regs_t *addr, uint32_t value)
{
    uint32_t temp = addr->USI_DMA_THRESHOLD;
    temp &= ~WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Msk;
    temp |= (value << WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Pos) & WJ_USI_DMA_THRESHOLD_RX_DMA_TH_Msk;
    addr->USI_DMA_THRESHOLD = temp;
}

static inline void wj_usi_en_spi_nss_data(ck_usi_regs_t *addr)
{
    addr->USI_SPI_NSS_DATA  |= WJ_USI_SPI_NSS_DATA_NSS_DATA_EN;
}

static inline void wj_usi_dis_spi_nss_data(ck_usi_regs_t *addr)
{
    addr->USI_SPI_NSS_DATA  &= (~WJ_USI_SPI_NSS_DATA_NSS_DATA_EN);
}

#ifdef CONFIG_USI_V25
#define USI_TX_MAX_FIFO        0x8U
#define USI_RX_MAX_FIFO        0x8U
#else
#define USI_TX_MAX_FIFO        0x10U
#define USI_RX_MAX_FIFO        0x10U
#endif
uint32_t wj_usi_get_recvfifo_waiting_num(ck_usi_regs_t *addr);
uint32_t wj_usi_uart_wait_timeout(ck_usi_regs_t *uart_base);
int32_t  wj_usi_uart_config_baudrate(ck_usi_regs_t *uart_base, uint8_t index, uint32_t baud, uint32_t uart_freq);
int32_t  wj_usi_uart_config_stop_bits(ck_usi_regs_t *uart_base, uint32_t stop_bits);
int32_t  wj_usi_uart_config_parity_none(ck_usi_regs_t *uart_base);
int32_t  wj_usi_uart_config_parity_odd(ck_usi_regs_t *uart_base);
int32_t  wj_usi_uart_config_parity_even(ck_usi_regs_t *uart_base);
int32_t  wj_usi_uart_config_data_bits(ck_usi_regs_t *uart_base, uint32_t data_bits);
void wj_usi_rst_regs(ck_usi_regs_t *addr);
#define USI_MAX_FIFO   0x10
#ifdef __cplusplus
}
#endif
#endif /* __WJ_USI_LL_H__ */
