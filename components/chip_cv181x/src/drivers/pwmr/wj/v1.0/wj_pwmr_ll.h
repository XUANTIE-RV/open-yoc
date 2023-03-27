/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_pwm_ll.h
 * @brief    header file for pwmr driver
 *           <In the hardware manual, there are 12 PWM channels in total,
 *           but only 6 channels are actually operable. So only supports
 *           6 channels for signal output and input acquisition.>
 * @version  V1.0
 * @date     23. Sep 2020
 ******************************************************************************/
#ifndef _WJ_PWMR_LL_H_
#define _WJ_PWMR_LL_H_

#include <stdio.h>
#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!  PWMR control Register, 	offset: 0x00 */
#define WJ_PWMR_CTRL_EN_Pos                                         (0U)
#define WJ_PWMR_CTRL_EN_Msk                                         (0x1U << WJ_PWMR_CTRL_EN_Pos)
#define WJ_PWMR_CTRL_EN                                             WJ_PWMR_CTRL_EN_Msk
/*!  PWMR config Register, 	offset: 0x04 */
#define WJ_PWMR_CONFIG_CNT0MODE_Pos                                 (0U)
#define WJ_PWMR_CONFIG_CNT0MODE_Msk                                 (0x1U << WJ_PWMR_CONFIG_CNT0MODE_Pos)
#define WJ_PWMR_CONFIG_CNT0MODE_UP_DOWN                             WJ_PWMR_CONFIG_CNT0MODE_Msk
#define WJ_PWMR_CONFIG_CNT0MODE_UP                                  (0U)

#define WJ_PWMR_CONFIG_UPDATE0MODE_Pos                              (1U)
#define WJ_PWMR_CONFIG_UPDATE0MODE_Msk                              (0x3U << WJ_PWMR_CONFIG_UPDATE0MODE_Pos)
#define WJ_PWMR_CONFIG_UPDATE0MODE_NOT_UPDATE                       (0x0U << WJ_PWMR_CONFIG_UPDATE0MODE_Pos)
#define WJ_PWMR_CONFIG_UPDATE0MODE_UPDATE_ZERO                      (0x1U << WJ_PWMR_CONFIG_UPDATE0MODE_Pos)
#define WJ_PWMR_CONFIG_UPDATE0MODE_UPDATE_TOP                       (0x2U << WJ_PWMR_CONFIG_UPDATE0MODE_Pos)
#define WJ_PWMR_CONFIG_UPDATE0MODE_UPDATE_TOP_ZERO                  (0x3U << WJ_PWMR_CONFIG_UPDATE0MODE_Pos)

#define WJ_PWMR_CONFIG_INV0EN_Pos                                   (3U)
#define WJ_PWMR_CONFIG_INV0EN_Msk                                   (0x1U << WJ_PWMR_CONFIG_INV0EN_Pos)
#define WJ_PWMR_CONFIG_INV0EN_EN                                    WJ_PWMR_CONFIG_INV0EN_Msk

#define WJ_PWMR_CONFIG_DEFAULT0LEVEL_Pos                            (4U)
#define WJ_PWMR_CONFIG_DEFAULT0LEVEL_Msk                            (0x1U << WJ_PWMR_CONFIG_DEFAULT0LEVEL_Pos)
#define WJ_PWMR_CONFIG_DEFAULT0LEVEL_HIGH                           WJ_PWMR_CONFIG_DEFAULT0LEVEL_Msk

#define WJ_PWMR_CONFIG_TRIGGER0EN_Pos                               (5U)
#define WJ_PWMR_CONFIG_TRIGGER0EN_Msk                               (0x1U << WJ_PWMR_CONFIG_TRIGGER0EN_Pos)
#define WJ_PWMR_CONFIG_TRIGGER0EN_EN                                WJ_PWMR_CONFIG_TRIGGER0EN_Msk

#define WJ_PWMR_CONFIG_CNT1MODE_Pos                                 (8U)
#define WJ_PWMR_CONFIG_CNT1MODE_Msk                                 (0x1U << WJ_PWMR_CONFIG_CNT1MODE_Pos)
#define WJ_PWMR_CONFIG_CNT1MODE_UP_DOWN                             WJ_PWMR_CONFIG_CNT1MODE_Msk
#define WJ_PWMR_CONFIG_CNT1MODE_UP                                  (0U)

#define WJ_PWMR_CONFIG_UPDATE1MODE_Pos                              (9U)
#define WJ_PWMR_CONFIG_UPDATE1MODE_Msk                              (0x3U << WJ_PWMR_CONFIG_UPDATE1MODE_Pos)
#define WJ_PWMR_CONFIG_UPDATE1MODE_NOT_UPDATE                       (0x1U << WJ_PWMR_CONFIG_UPDATE1MODE_Pos)
#define WJ_PWMR_CONFIG_UPDATE1MODE_UPDATE_ZERO                      (0x1U << WJ_PWMR_CONFIG_UPDATE1MODE_Pos)
#define WJ_PWMR_CONFIG_UPDATE1MODE_UPDATE_TOP                       (0x2U << WJ_PWMR_CONFIG_UPDATE1MODE_Pos)
#define WJ_PWMR_CONFIG_UPDATE1MODE_UPDATE_TOP_ZERO                  (0x3U << WJ_PWMR_CONFIG_UPDATE1MODE_Pos)

#define WJ_PWMR_CONFIG_INV1EN_Pos                                   (11U)
#define WJ_PWMR_CONFIG_INV1EN_Msk                                   (0x1U << WJ_PWMR_CONFIG_INV1EN_Pos)
#define WJ_PWMR_CONFIG_INV1EN_EN                                    WJ_PWMR_CONFIG_INV1EN_Msk

#define WJ_PWMR_CONFIG_DEFAULT1LEVEL_Pos                            (12U)
#define WJ_PWMR_CONFIG_DEFAULT1LEVEL_Msk                            (0x1U << WJ_PWMR_CONFIG_DEFAULT1LEVEL_Pos)
#define WJ_PWMR_CONFIG_DEFAULT1LEVEL_HIGH                           WJ_PWMR_CONFIG_DEFAULT1LEVEL_Msk

#define WJ_PWMR_CONFIG_TRIGGER1EN_Pos                               (13U)
#define WJ_PWMR_CONFIG_TRIGGER1EN_Msk                               (0x1U << WJ_PWMR_CONFIG_TRIGGER1EN_Pos)
#define WJ_PWMR_CONFIG_TRIGGER1EN_EN                                WJ_PWMR_CONFIG_TRIGGER1EN_Msk

#define WJ_PWMR_CONFIG_DIV_MODE_Pos                                (16U)
#define WJ_PWMR_CONFIG_DIV_MODE_Msk                                (0x7U << WJ_PWMR_CONFIG_DIV_MODE_Pos)
#define WJ_PWMR_CONFIG_DIV_MODE_2                                  (0x0U << WJ_PWMR_CONFIG_DIV_MODE_Pos)
#define WJ_PWMR_CONFIG_DIV_MODE_4                                  (0x1U << WJ_PWMR_CONFIG_DIV_MODE_Pos)
#define WJ_PWMR_CONFIG_DIV_MODE_8                                  (0x2U << WJ_PWMR_CONFIG_DIV_MODE_Pos)
#define WJ_PWMR_CONFIG_DIV_MODE_16                                 (0x3U << WJ_PWMR_CONFIG_DIV_MODE_Pos)
#define WJ_PWMR_CONFIG_DIV_MODE_32                                 (0x4U << WJ_PWMR_CONFIG_DIV_MODE_Pos)
#define WJ_PWMR_CONFIG_DIV_MODE_64                                 (0x5U << WJ_PWMR_CONFIG_DIV_MODE_Pos)
#define WJ_PWMR_CONFIG_DIV_MODE_128                                (0x6U << WJ_PWMR_CONFIG_DIV_MODE_Pos)

#define WJ_PWMR_CONFIG_DIV_EN_Pos                                  (19U)
#define WJ_PWMR_CONFIG_DIV_EN_Msk                                  (0x1U << WJ_PWMR_CONFIG_DIV_EN_Pos)
#define WJ_PWMR_CONFIG_DIV_EN                                      WJ_PWMR_CONFIG_DIV_EN_Msk
/*!  PWMR value0 Register, 	offset: 0x08 */
#define WJ_PWMR_VALUE0BUF_TV0BUFFER_Pos                            (0U)
#define WJ_PWMR_VALUE0BUF_TV0BUFFER_Msk                            (0xFFFFU << WJ_PWMR_VALUE0BUF_TV0BUFFER_Pos)

#define WJ_PWMR_VALUE0BUF_CV0BUFFER_Pos                            (16U)
#define WJ_PWMR_VALUE0BUF_CV0BUFFER_Msk                            (0xFFFFU << WJ_PWMR_VALUE0BUF_CV0BUFFER_Pos)

/*!  PWMR value1 Register, 	offset: 0x0c */
#define WJ_PWMR_VALUE1BUF_TV1BUFFER_Pos                            (0U)
#define WJ_PWMR_VALUE1BUF_TV1BUFFER_Msk                            (0xFFFFU << WJ_PWMR_VALUE1BUF_TV1BUFFER_Pos)

#define WJ_PWMR_VALUE1BUF_CV1BUFFER_Pos                            (16U)
#define WJ_PWMR_VALUE1BUF_CV1BUFFER_Msk                            (0xFFFFU << WJ_PWMR_VALUE1BUF_CV1BUFFER_Pos)

/*!  PWMR interrupt mask  Register, 	offset: 0x10 */
#define WJ_PWMR_IM_INT0M_CNTZERO_Pos                               (0U)
#define WJ_PWMR_IM_INT0M_CNTZERO_Msk                               (0x1U << WJ_PWMR_IM_INT0M_CNTZERO_Pos)
#define WJ_PWMR_IM_INT0M_CNTZERO_EN                                WJ_PWMR_IM_INT0M_CNTZERO_Msk

#define WJ_PWMR_IM_INT0M_CNTTOP_Pos                                (1U)
#define WJ_PWMR_IM_INT0M_CNTTOP_Msk                                (0x1U << WJ_PWMR_IM_INT0M_CNTTOP_Pos)
#define WJ_PWMR_IM_INT0M_CNTTOP_EN                                 WJ_PWMR_IM_INT0M_CNTTOP_Msk

#define WJ_PWMR_IM_INT0M_CMPU_Pos                                  (2U)
#define WJ_PWMR_IM_INT0M_CMPU_Msk                                  (0x1U << WJ_PWMR_IM_INT0M_CMPU_Pos)
#define WJ_PWMR_IM_INT0M_CMPU_EN                                   WJ_PWMR_IM_INT0M_CMPU_Msk

#define WJ_PWMR_IM_INT0M_CMPD_Pos                                  (3U)
#define WJ_PWMR_IM_INT0M_CMPD_Msk                                  (0x1U << WJ_PWMR_IM_INT0M_CMPD_Pos)
#define WJ_PWMR_IM_INT0M_CMPD_EN                                   WJ_PWMR_IM_INT0M_CMPD_Msk

#define WJ_PWMR_IM_INT0M_OVERTIME_Pos                              (4U)
#define WJ_PWMR_IM_INT0M_OVERTIME_Msk                              (0x1U << WJ_PWMR_IM_INT0M_OVERTIME_Pos)
#define WJ_PWMR_IM_INT0M_OVERTIME_EN                               WJ_PWMR_IM_INT0M_OVERTIME_Msk

#define WJ_PWMR_IM_INT1M_CNTZERO_Pos                               (8U)
#define WJ_PWMR_IM_INT1M_CNTZERO_Msk                               (0X1U << WJ_PWMR_IM_INT1M_CNTZERO_Pos)
#define WJ_PWMR_IM_INT1M_CNTZERO_EN                                WJ_PWMR_IM_INT1M_CNTZERO_Msk

#define WJ_PWMR_IM_INT1M_CNTTOP_Pos                                (9U)
#define WJ_PWMR_IM_INT1M_CNTTOP_Msk                                (0X1U << WJ_PWMR_IM_INT1M_CNTTOP_Pos)
#define WJ_PWMR_IM_INT1M_CNTTOP_EN                                 WJ_PWMR_IM_INT1M_CNTTOP_Msk

#define WJ_PWMR_IM_INT1M_CMPU_Pos                                  (10U)
#define WJ_PWMR_IM_INT1M_CMPU_Msk                                  (0X1U << WJ_PWMR_IM_INT1M_CMPU_Pos)
#define WJ_PWMR_IM_INT1M_CMPU_EN                                   WJ_PWMR_IM_INT1M_CMPU_Msk

#define WJ_PWMR_IM_INT1M_CMPD_Pos                                  (11U)
#define WJ_PWMR_IM_INT1M_CMPD_Msk                                  (0X1U << WJ_PWMR_IM_INT1M_CMPD_Pos)
#define WJ_PWMR_IM_INT1M_CMPD_EN                                   WJ_PWMR_IM_INT1M_CMPD_Msk

#define WJ_PWMR_IM_INT1M_OVERTIME_Pos                              (12U)
#define WJ_PWMR_IM_INT1M_OVERTIME_Msk                              (0X1U << WJ_PWMR_IM_INT1M_OVERTIME_Pos)
#define WJ_PWMR_IM_INT1M_OVERTIME_EN                               WJ_PWMR_IM_INT1M_OVERTIME_Msk

/*!  PWMR raw interrupt status Register, 	offset: 0x14 */
#define WJ_PWMR_RIS_INT0M_CNTZERO_Pos                               (0U)
#define WJ_PWMR_RIS_INT0M_CNTZERO_Msk                               (0x1U << WJ_PWMR_RIS_INT0M_CNTZERO_Pos)
#define WJ_PWMR_RIS_INT0M_CNTZERO_EN                                WJ_PWMR_RIS_INT0M_CNTZERO_Msk

#define WJ_PWMR_RIS_INT0M_CNTTOP_Pos                                (1U)
#define WJ_PWMR_RIS_INT0M_CNTTOP_Msk                                (0x1U << WJ_PWMR_RIS_INT0M_CNTTOP_Pos)
#define WJ_PWMR_RIS_INT0M_CNTTOP_EN                                 WJ_PWMR_RIS_INT0M_CNTTOP_Msk

#define WJ_PWMR_RIS_INT0M_CMPU_Pos                                  (2U)
#define WJ_PWMR_RIS_INT0M_CMPU_Msk                                  (0x1U << WJ_PWMR_RIS_INT0M_CMPU_Pos)
#define WJ_PWMR_RIS_INT0M_CMPU_EN                                   WJ_PWMR_RIS_INT0M_CMPU_Msk

#define WJ_PWMR_RIS_INT0M_CMPD_Pos                                  (3U)
#define WJ_PWMR_RIS_INT0M_CMPD_Msk                                  (0x1U << WJ_PWMR_RIS_INT0M_CMPD_Pos)
#define WJ_PWMR_RIS_INT0M_CMPD_EN                                   WJ_PWMR_RIS_INT0M_CMPD_Msk

#define WJ_PWMR_RIS_INT0M_OVERTIME_Pos                              (4U)
#define WJ_PWMR_RIS_INT0M_OVERTIME_Msk                              (0x1U << WJ_PWMR_RIS_INT0M_OVERTIME_Pos)
#define WJ_PWMR_RIS_INT0M_OVERTIME_EN                               WJ_PWMR_RIS_INT0M_OVERTIME_Msk

#define WJ_PWMR_RIS_INT1M_CNTZERO_Pos                               (8U)
#define WJ_PWMR_RIS_INT1M_CNTZERO_Msk                               (0X1U << WJ_PWMR_RIS_INT1M_CNTZERO_Pos)
#define WJ_PWMR_RIS_INT1M_CNTZERO_EN                                WJ_PWMR_RIS_INT1M_CNTZERO_Msk

#define WJ_PWMR_RIS_INT1M_CNTTOP_Pos                                (9U)
#define WJ_PWMR_RIS_INT1M_CNTTOP_Msk                                (0X1U << WJ_PWMR_RIS_INT1M_CNTTOP_Pos)
#define WJ_PWMR_RIS_INT1M_CNTTOP_EN                                 WJ_PWMR_RIS_INT1M_CNTTOP_Msk

#define WJ_PWMR_RIS_INT1M_CMPU_Pos                                  (10U)
#define WJ_PWMR_RIS_INT1M_CMPU_Msk                                  (0X1U << WJ_PWMR_RIS_INT1M_CMPU_Pos)
#define WJ_PWMR_RIS_INT1M_CMPU_EN                                   WJ_PWMR_RIS_INT1M_CMPU_Msk

#define WJ_PWMR_RIS_INT1M_CMPD_Pos                                  (11U)
#define WJ_PWMR_RIS_INT1M_CMPD_Msk                                  (0X1U << WJ_PWMR_RIS_INT1M_CMPD_Pos)
#define WJ_PWMR_RIS_INT1M_CMPD_EN                                   WJ_PWMR_RIS_INT1M_CMPD_Msk

#define WJ_PWMR_RIS_INT1M_OVERTIME_Pos                              (12U)
#define WJ_PWMR_RIS_INT1M_OVERTIME_Msk                              (0X1U << WJ_PWMR_RIS_INT1M_OVERTIME_Pos)
#define WJ_PWMR_RIS_INT1M_OVERTIME_EN                               WJ_PWMR_RIS_INT1M_OVERTIME_Msk
/*!  PWMR interrupt status Register, 	offset: 0x18 */
#define WJ_PWMR_IS_INT0M_CNTZERO_Pos                               (0U)
#define WJ_PWMR_IS_INT0M_CNTZERO_Msk                               (0x1U << WJ_PWMR_IS_INT0M_CNTZERO_Pos)
#define WJ_PWMR_IS_INT0M_CNTZERO_EN                                WJ_PWMR_IS_INT0M_CNTZERO_Msk

#define WJ_PWMR_IS_INT0M_CNTTOP_Pos                                (1U)
#define WJ_PWMR_IS_INT0M_CNTTOP_Msk                                (0x1U << WJ_PWMR_IS_INT0M_CNTTOP_Pos)
#define WJ_PWMR_IS_INT0M_CNTTOP_EN                                 WJ_PWMR_IS_INT0M_CNTTOP_Msk

#define WJ_PWMR_IS_INT0M_CMPU_Pos                                  (2U)
#define WJ_PWMR_IS_INT0M_CMPU_Msk                                  (0x1U << WJ_PWMR_IS_INT0M_CMPU_Pos)
#define WJ_PWMR_IS_INT0M_CMPU_EN                                   WJ_PWMR_IS_INT0M_CMPU_Msk

#define WJ_PWMR_IS_INT0M_CMPD_Pos                                  (3U)
#define WJ_PWMR_IS_INT0M_CMPD_Msk                                  (0x1U << WJ_PWMR_IS_INT0M_CMPD_Pos)
#define WJ_PWMR_IS_INT0M_CMPD_EN                                   WJ_PWMR_IS_INT0M_CMPD_Msk

#define WJ_PWMR_IS_INT0M_OVERTIME_Pos                              (4U)
#define WJ_PWMR_IS_INT0M_OVERTIME_Msk                              (0x1U << WJ_PWMR_IS_INT0M_OVERTIME_Pos)
#define WJ_PWMR_IS_INT0M_OVERTIME_EN                               WJ_PWMR_IS_INT0M_OVERTIME_Msk

#define WJ_PWMR_IS_INT1M_CNTZERO_Pos                               (8U)
#define WJ_PWMR_IS_INT1M_CNTZERO_Msk                               (0X1U << WJ_PWMR_IS_INT1M_CNTZERO_Pos)
#define WJ_PWMR_IS_INT1M_CNTZERO_EN                                WJ_PWMR_IS_INT1M_CNTZERO_Msk

#define WJ_PWMR_IS_INT1M_CNTTOP_Pos                                (9U)
#define WJ_PWMR_IS_INT1M_CNTTOP_Msk                                (0X1U << WJ_PWMR_IS_INT1M_CNTTOP_Pos)
#define WJ_PWMR_IS_INT1M_CNTTOP_EN                                 WJ_PWMR_IS_INT1M_CNTTOP_Msk

#define WJ_PWMR_IS_INT1M_CMPU_Pos                                  (10U)
#define WJ_PWMR_IS_INT1M_CMPU_Msk                                  (0X1U << WJ_PWMR_IS_INT1M_CMPU_Pos)
#define WJ_PWMR_IS_INT1M_CMPU_EN                                   WJ_PWMR_IS_INT1M_CMPU_Msk

#define WJ_PWMR_IS_INT1M_CMPD_Pos                                  (11U)
#define WJ_PWMR_IS_INT1M_CMPD_Msk                                  (0X1U << WJ_PWMR_IS_INT1M_CMPD_Pos)
#define WJ_PWMR_IS_INT1M_CMPD_EN                                   WJ_PWMR_IS_INT1M_CMPD_Msk

#define WJ_PWMR_IS_INT1M_OVERTIME_Pos                              (12U)
#define WJ_PWMR_IS_INT1M_OVERTIME_Msk                              (0X1U << WJ_PWMR_IS_INT1M_OVERTIME_Pos)
#define WJ_PWMR_IS_INT1M_OVERTIME_EN                               WJ_PWMR_IS_INT1M_OVERTIME_Msk

/*!  PWMR interrupt clear Register, 	offset: 0x1c */
#define WJ_PWMR_IC_INT0M_CNTZERO_Pos                               (0U)
#define WJ_PWMR_IC_INT0M_CNTZERO_Msk                               (0x1U << WJ_PWMR_IC_INT0M_CNTZERO_Pos)
#define WJ_PWMR_IC_INT0M_CNTZERO_EN                                WJ_PWMR_IC_INT0M_CNTZERO_Msk

#define WJ_PWMR_IC_INT0M_CNTTOP_Pos                                (1U)
#define WJ_PWMR_IC_INT0M_CNTTOP_Msk                                (0x1U << WJ_PWMR_IC_INT0M_CNTTOP_Pos)
#define WJ_PWMR_IC_INT0M_CNTTOP_EN                                 WJ_PWMR_IC_INT0M_CNTTOP_Msk

#define WJ_PWMR_IC_INT0M_CMPU_Pos                                  (2U)
#define WJ_PWMR_IC_INT0M_CMPU_Msk                                  (0x1U << WJ_PWMR_IC_INT0M_CMPU_Pos)
#define WJ_PWMR_IC_INT0M_CMPU_EN                                   WJ_PWMR_IC_INT0M_CMPU_Msk

#define WJ_PWMR_IC_INT0M_CMPD_Pos                                  (3U)
#define WJ_PWMR_IC_INT0M_CMPD_Msk                                  (0x1U << WJ_PWMR_IC_INT0M_CMPD_Pos)
#define WJ_PWMR_IC_INT0M_CMPD_EN                                   WJ_PWMR_IC_INT0M_CMPD_Msk

#define WJ_PWMR_IC_INT0M_OVERTIME_Pos                              (4U)
#define WJ_PWMR_IC_INT0M_OVERTIME_Msk                              (0x1U << WJ_PWMR_IC_INT0M_OVERTIME_Pos)
#define WJ_PWMR_IC_INT0M_OVERTIME_EN                               WJ_PWMR_IC_INT0M_OVERTIME_Msk

#define WJ_PWMR_IC_INT1M_CNTZERO_Pos                               (8U)
#define WJ_PWMR_IC_INT1M_CNTZERO_Msk                               (0X1U << WJ_PWMR_IC_INT1M_CNTZERO_Pos)
#define WJ_PWMR_IC_INT1M_CNTZERO_EN                                WJ_PWMR_IC_INT1M_CNTZERO_Msk

#define WJ_PWMR_IC_INT1M_CNTTOP_Pos                                (9U)
#define WJ_PWMR_IC_INT1M_CNTTOP_Msk                                (0X1U << WJ_PWMR_IC_INT1M_CNTTOP_Pos)
#define WJ_PWMR_IC_INT1M_CNTTOP_EN                                 WJ_PWMR_IC_INT1M_CNTTOP_Msk

#define WJ_PWMR_IC_INT1M_CMPU_Pos                                  (10U)
#define WJ_PWMR_IC_INT1M_CMPU_Msk                                  (0X1U << WJ_PWMR_IC_INT1M_CMPU_Pos)
#define WJ_PWMR_IC_INT1M_CMPU_EN                                   WJ_PWMR_IC_INT1M_CMPU_Msk

#define WJ_PWMR_IC_INT1M_CMPD_Pos                                  (11U)
#define WJ_PWMR_IC_INT1M_CMPD_Msk                                  (0X1U << WJ_PWMR_IC_INT1M_CMPD_Pos)
#define WJ_PWMR_IC_INT1M_CMPD_EN                                   WJ_PWMR_IC_INT1M_CMPD_Msk

#define WJ_PWMR_IC_INT1M_OVERTIME_Pos                              (12U)
#define WJ_PWMR_IC_INT1M_OVERTIME_Msk                              (0X1U << WJ_PWMR_IC_INT1M_OVERTIME_Pos)
#define WJ_PWMR_IC_INT1M_OVERTIME_EN                               WJ_PWMR_IC_INT1M_OVERTIME_Msk

/*!  PWMR count0 Register, 	offset: 0x20 */
#define WJ_PWMR_COUNT0_Pos                            (0U)
#define WJ_PWMR_COUNT0_Msk                            (0xFFFFU << WJ_PWMR_COUNT0_Pos)
/*!  PWMR count1 Register, 	offset: 0x24 */
#define WJ_PWMR_COUNT1_Pos                            (0U)
#define WJ_PWMR_COUNT1_Msk                            (0xFFFFU << WJ_PWMR_COUNT1_Pos)
typedef struct {
    __IOM uint32_t	PWMR_CTRL;
    __IOM uint32_t	PWMR_CONFIG;
    __IOM uint32_t	PWMR_VALUE0_BUF;
    __IOM uint32_t	PWMR_VALUE1_BUF;
    __IOM uint32_t	PWMR_IM;
    __IOM uint32_t	PWMR_RIS;
    __OM uint32_t	PWMR_IS;
    __IOM uint32_t	PWMR_IC;
    __OM  uint32_t	PWMR_COUNT0;
    __OM  uint32_t	PWMR_COUNT1;
} wj_pwmr_regs_t;

static inline void wj_pwmr_en(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CTRL |= WJ_PWMR_CTRL_EN;
}

static inline void wj_pwmr_dis(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CTRL &= (~WJ_PWMR_CTRL_EN);
}

static inline void wj_pwmr_en_cnt0_updown_mode(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG |= WJ_PWMR_CONFIG_CNT0MODE_UP_DOWN;
}

static inline void wj_pwmr_en_cnt0_up_mode(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG &= (~WJ_PWMR_CONFIG_CNT0MODE_UP_DOWN);
}

static inline void wj_pwmr_set_cnt0_update_mode(wj_pwmr_regs_t *pwmr_base, uint32_t value)
{
    pwmr_base->PWMR_CONFIG &= (~WJ_PWMR_CONFIG_UPDATE0MODE_Msk);
    pwmr_base->PWMR_CONFIG |= (value & WJ_PWMR_CONFIG_UPDATE0MODE_Msk);
}

static inline void wj_pwmr_en_cnt0_invert(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG |= WJ_PWMR_CONFIG_INV0EN_EN;
}

static inline void wj_pwmr_dis_cnt0_invert(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG &= (~WJ_PWMR_CONFIG_INV0EN_EN);
}

static inline void wj_pwmr_dis_cnt0_default_high(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG |= WJ_PWMR_CONFIG_DEFAULT0LEVEL_HIGH;
}

static inline void wj_pwmr_dis_cnt0_default_low(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG &= (~WJ_PWMR_CONFIG_DEFAULT0LEVEL_HIGH);
}

static inline void wj_pwmr_en_cnt0_trig(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG |= WJ_PWMR_CONFIG_TRIGGER0EN_EN;
}

static inline void wj_pwmr_dis_cnt0_trig(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG &= (~WJ_PWMR_CONFIG_TRIGGER0EN_EN);
}

static inline void wj_pwmr_set_cnt1_updown_mode(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG |= WJ_PWMR_CONFIG_CNT1MODE_UP_DOWN;
}

static inline void wj_pwmr_set_cnt1_up_mode(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG &= (~WJ_PWMR_CONFIG_CNT1MODE_UP_DOWN);
}

static inline void wj_pwmr_set_cnt1_update_mode(wj_pwmr_regs_t *pwmr_base, uint32_t value)
{
    pwmr_base->PWMR_CONFIG &= (~WJ_PWMR_CONFIG_UPDATE1MODE_Msk);
    pwmr_base->PWMR_CONFIG |= (value & WJ_PWMR_CONFIG_UPDATE1MODE_Msk);
}

static inline void wj_pwmr_en_cnt1_invert(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG |= WJ_PWMR_CONFIG_INV1EN_EN;
}

static inline void wj_pwmr_dis_cnt1_invert(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG &= (~WJ_PWMR_CONFIG_INV1EN_EN);
}

static inline void wj_pwmr_dis_cnt1_default_high(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG |= WJ_PWMR_CONFIG_DEFAULT1LEVEL_HIGH;
}

static inline void wj_pwmr_dis_cnt1_default_low(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG &= (~WJ_PWMR_CONFIG_DEFAULT1LEVEL_HIGH);
}

static inline void wj_pwmr_en_cnt1_trig(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG |= WJ_PWMR_CONFIG_TRIGGER1EN_EN;
}

static inline void wj_pwmr_dis_cnt1_trig(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG &= (~WJ_PWMR_CONFIG_TRIGGER1EN_EN);
}

static inline void wj_pwmr_div_en(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG |= WJ_PWMR_CONFIG_DIV_EN;
}
static inline void wj_pwmr_div_dis(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG &= (~WJ_PWMR_CONFIG_DIV_EN);
}

static inline void wj_pwmr_clear_value0_buf(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_VALUE0_BUF = 0;
}

static inline void wj_pwmr_clear_value1_buf(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_VALUE1_BUF = 0;
}

static inline uint32_t wj_pwmr_get_value0_buf(wj_pwmr_regs_t *pwmr_base)
{
    return pwmr_base->PWMR_VALUE0_BUF;
}

static inline uint32_t wj_pwmr_get_value1_buf(wj_pwmr_regs_t *pwmr_base)
{
    return pwmr_base->PWMR_VALUE1_BUF;
}


static inline void wj_pwmr_set_gen0_top_value(wj_pwmr_regs_t *pwmr_base, uint32_t value)
{
    pwmr_base->PWMR_VALUE0_BUF &= (~ WJ_PWMR_VALUE0BUF_TV0BUFFER_Msk);
    pwmr_base->PWMR_VALUE0_BUF |= (value & WJ_PWMR_VALUE0BUF_TV0BUFFER_Msk);
}

static inline void wj_pwmr_set_gen0_compare_value(wj_pwmr_regs_t *pwmr_base, uint32_t value)
{
    pwmr_base->PWMR_VALUE0_BUF &= (~WJ_PWMR_VALUE0BUF_CV0BUFFER_Msk);
    pwmr_base->PWMR_VALUE0_BUF |= ((value<<WJ_PWMR_VALUE0BUF_CV0BUFFER_Pos) & WJ_PWMR_VALUE0BUF_CV0BUFFER_Msk);
}

static inline void wj_pwmr_set_gen1_top_value(wj_pwmr_regs_t *pwmr_base, uint32_t value)
{
    pwmr_base->PWMR_VALUE1_BUF &= (~WJ_PWMR_VALUE1BUF_TV1BUFFER_Msk);
    pwmr_base->PWMR_VALUE1_BUF |= (value & WJ_PWMR_VALUE1BUF_TV1BUFFER_Msk);
}

static inline void wj_pwmr_set_gen1_compare_value(wj_pwmr_regs_t *pwmr_base, uint32_t value)
{
    pwmr_base->PWMR_VALUE1_BUF &= (~ WJ_PWMR_VALUE1BUF_CV1BUFFER_Msk);
    pwmr_base->PWMR_VALUE1_BUF |= ((value<<WJ_PWMR_VALUE1BUF_CV1BUFFER_Pos) & WJ_PWMR_VALUE1BUF_CV1BUFFER_Msk);
}

static inline void wj_pwmr_set_div(wj_pwmr_regs_t *pwmr_base,uint32_t value)
{
    pwmr_base->PWMR_CONFIG &= (~WJ_PWMR_CONFIG_DIV_MODE_Msk);
    pwmr_base->PWMR_CONFIG |= (value &WJ_PWMR_CONFIG_DIV_MODE_Msk);
}

static inline void wj_pwmr_en_div_en(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG |= WJ_PWMR_CONFIG_DIV_EN;
}

static inline void wj_pwmr_dis_div_en(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_CONFIG &= (~WJ_PWMR_CONFIG_DIV_EN);
}

static inline uint32_t wj_pwmr_get_div(wj_pwmr_regs_t *pwmr_base)
{
    return (pwmr_base->PWMR_CONFIG &WJ_PWMR_CONFIG_DIV_MODE_Msk);
}

static inline void wj_pwmr_en_int0_cntzero_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM |= WJ_PWMR_IM_INT0M_CNTZERO_EN;
}

static inline void wj_pwmr_dis_int0_cntzero_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM &= (~WJ_PWMR_IM_INT0M_CNTZERO_EN);
}

static inline void wj_pwmr_en_int0_cnttop_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM |= WJ_PWMR_IM_INT0M_CNTTOP_EN;
}

static inline void wj_pwmr_dis_int0_cnttop_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM &= (~WJ_PWMR_IM_INT0M_CNTTOP_EN);
}

static inline void wj_pwmr_en_int0_cnt_cnt_equ_comp_up_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM |= WJ_PWMR_IM_INT0M_CMPU_EN;
}

static inline void wj_pwmr_dis_int0_cnt_cnt_equ_comp_up_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM &= (~WJ_PWMR_IM_INT0M_CMPU_EN);
}

static inline void wj_pwmr_en_int0_cnt_cnt_equ_comp_down_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM |= WJ_PWMR_IM_INT0M_CMPD_EN;
}

static inline void wj_pwmr_dis_int0_cnt_cnt_equ_comp_down_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM &= (~WJ_PWMR_IM_INT0M_CMPD_EN);
}

static inline void wj_pwmr_en_int0_overtime_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM |= WJ_PWMR_IM_INT0M_CMPD_EN;
}

static inline void wj_pwmr_dis_int0_overtime_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM &= (~WJ_PWMR_IM_INT0M_CMPD_EN);
}


static inline void wj_pwmr_en_int1_cntzero_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM |= WJ_PWMR_IM_INT1M_CNTZERO_EN;
}

static inline void wj_pwmr_dis_int1_cntzero_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM &= (~WJ_PWMR_IM_INT1M_CNTZERO_EN);
}

static inline void wj_pwmr_en_int1_cnttop_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM |= WJ_PWMR_IM_INT1M_CNTTOP_EN;
}

static inline void wj_pwmr_dis_int1_cnttop_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM &= (~WJ_PWMR_IM_INT1M_CNTTOP_EN);
}

static inline void wj_pwmr_en_int1_cnt_cnt_equ_comp_up_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM |= WJ_PWMR_IM_INT1M_CMPU_EN;
}

static inline void wj_pwmr_dis_int1_cnt_cnt_equ_comp_up_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM &= (~WJ_PWMR_IM_INT1M_CMPU_EN);
}

static inline void wj_pwmr_en_int1_cnt_cnt_equ_comp_down_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM |= WJ_PWMR_IM_INT1M_CMPD_EN;
}

static inline void wj_pwmr_dis_int1_cnt_cnt_equ_comp_down_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM &= (~WJ_PWMR_IM_INT1M_CMPD_EN);
}

static inline void wj_pwmr_en_int1_overtime_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM |= WJ_PWMR_IM_INT1M_CMPD_EN;
}

static inline void wj_pwmr_dis_int1_overtime_mask(wj_pwmr_regs_t *pwmr_base)
{
    pwmr_base->PWMR_IM &= (~WJ_PWMR_IM_INT1M_CMPD_EN);
}

static inline uint32_t wj_pwmr_get_raw_int_status(wj_pwmr_regs_t *pwmr_base)
{
    return pwmr_base->PWMR_RIS;
}

static inline uint32_t wj_pwmr_get_int_status(wj_pwmr_regs_t *pwmr_base)
{
    return pwmr_base->PWMR_IS;
}

static inline uint32_t wj_pwmr_get_clear_int(wj_pwmr_regs_t *pwmr_base)
{
    return pwmr_base->PWMR_IC;
}

static inline void wj_pwmr_set_clear_int(wj_pwmr_regs_t *pwmr_base, uint32_t value)
{
    pwmr_base->PWMR_IC = value;
}

static inline uint32_t wj_pwmr_get_count0(wj_pwmr_regs_t *pwmr_base)
{
    return (pwmr_base->PWMR_COUNT0 & WJ_PWMR_IC_INT0M_OVERTIME_Msk);
}

static inline uint32_t wj_pwmr_get_count1(wj_pwmr_regs_t *pwmr_base)
{
    return (pwmr_base->PWMR_COUNT1&WJ_PWMR_IC_INT1M_OVERTIME_Msk);
}
#ifdef __cplusplus
}
#endif

#endif
