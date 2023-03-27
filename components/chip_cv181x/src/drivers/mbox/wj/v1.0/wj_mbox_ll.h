/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_mbox_ll.h
 * @brief    header file for wj mbox ll driver
 * @version  V1.0
 * @date     30. July 2020
 ******************************************************************************/

#ifndef _WJ_MBOX_LL_H_
#define _WJ_MBOX_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * mbox register bit definitions
 */

/* RIS_WCPU0 , offset: 0x00 */
#define WJ_RIS_WCPU0_RIS_WCPU0_0_Pos               (0U)
#define WJ_RIS_WCPU0_RIS_WCPU0_0_Msk               (0x1U << WJ_RIS_WCPU0_RIS_WCPU0_0_Pos)
#define WJ_RIS_WCPU0_RIS_WCPU0_0                   WJ_RIS_WCPU0_RIS_WCPU0_0_Msk

#define WJ_RIS_WCPU0_RIS_WCPU0_1_Pos               (1U)
#define WJ_RIS_WCPU0_RIS_WCPU0_1_Msk               (0x1U << WJ_RIS_WCPU0_RIS_WCPU0_1_Pos)
#define WJ_RIS_WCPU0_RIS_WCPU0_1                   WJ_RIS_WCPU0_RIS_WCPU0_1_Msk

#define WJ_RIS_WCPU0_RIS_WCPU0_2_Pos               (2U)
#define WJ_RIS_WCPU0_RIS_WCPU0_2_Msk               (0x1U << WJ_RIS_WCPU0_RIS_WCPU0_2_Pos)
#define WJ_RIS_WCPU0_RIS_WCPU0_2                   WJ_RIS_WCPU0_RIS_WCPU0_2_Msk

#define WJ_RIS_WCPU0_RIS_WCPU0_3_Pos               (3U)
#define WJ_RIS_WCPU0_RIS_WCPU0_3_Msk               (0x1U << WJ_RIS_WCPU0_RIS_WCPU0_3_Pos)
#define WJ_RIS_WCPU0_RIS_WCPU0_3                   WJ_RIS_WCPU0_RIS_WCPU0_3_Msk

#define WJ_RIS_WCPU0_RIS_WCPU0_4_Pos               (4U)
#define WJ_RIS_WCPU0_RIS_WCPU0_4_Msk               (0x1U << WJ_RIS_WCPU0_RIS_WCPU0_4_Pos)
#define WJ_RIS_WCPU0_RIS_WCPU0_4                   WJ_RIS_WCPU0_RIS_WCPU0_4_Msk

#define WJ_RIS_WCPU0_RIS_WCPU0_5_Pos               (5U)
#define WJ_RIS_WCPU0_RIS_WCPU0_5_Msk               (0x1U << WJ_RIS_WCPU0_RIS_WCPU0_5_Pos)
#define WJ_RIS_WCPU0_RIS_WCPU0_5                   WJ_RIS_WCPU0_RIS_WCPU0_5_Msk

#define WJ_RIS_WCPU0_RIS_WCPU0_6_Pos               (6U)
#define WJ_RIS_WCPU0_RIS_WCPU0_6_Msk               (0x1U << WJ_RIS_WCPU0_RIS_WCPU0_6_Pos)
#define WJ_RIS_WCPU0_RIS_WCPU0_6                   WJ_RIS_WCPU0_RIS_WCPU0_6_Msk

#define WJ_RIS_WCPU0_RIS_WCPU0_7_Pos               (7U)
#define WJ_RIS_WCPU0_RIS_WCPU0_7_Msk               (0x7U << WJ_RIS_WCPU0_RIS_WCPU0_7_Pos)
#define WJ_RIS_WCPU0_RIS_WCPU0_7                   WJ_RIS_WCPU0_RIS_WCPU0_7_Msk

#define WJ_RIS_WCPU0_RIS_WCPU0_8_Pos               (8U)
#define WJ_RIS_WCPU0_RIS_WCPU0_8_Msk               (0x1U << WJ_RIS_WCPU0_RIS_WCPU0_8_Pos)
#define WJ_RIS_WCPU0_RIS_WCPU0_8                   WJ_RIS_WCPU0_RIS_WCPU0_8_Msk

#define WJ_RIS_WCPU0_RIS_WCPU0_9_Pos               (9U)
#define WJ_RIS_WCPU0_RIS_WCPU0_9_Msk               (0x1U << WJ_RIS_WCPU0_RIS_WCPU0_9_Pos)
#define WJ_RIS_WCPU0_RIS_WCPU0_9                   WJ_RIS_WCPU0_RIS_WCPU0_9_Msk

#define WJ_RIS_WCPU0_RIS_WCPU0_10_Pos              (10U)
#define WJ_RIS_WCPU0_RIS_WCPU0_10_Msk              (0x1U << WJ_RIS_WCPU0_RIS_WCPU0_10_Pos)
#define WJ_RIS_WCPU0_RIS_WCPU0_10                  WJ_RIS_WCPU0_RIS_WCPU0_10_Msk

#define WJ_RIS_WCPU0_RIS_WCPU0_11_Pos              (11U)
#define WJ_RIS_WCPU0_RIS_WCPU0_11_Msk              (0x1U << WJ_RIS_WCPU0_RIS_WCPU0_11_Pos)
#define WJ_RIS_WCPU0_RIS_WCPU0_11                  WJ_RIS_WCPU0_RIS_WCPU0_11_Msk


/* IUM_WCPU0, offset: 0x04 */
#define WJ_IUM_WCPU0_IUM_WCPU0_0_Pos               (0U)
#define WJ_IUM_WCPU0_IUM_WCPU0_0_Msk               (0x1U << WJ_IUM_WCPU0_IUM_WCPU0_0_Pos)
#define WJ_IUM_WCPU0_IUM_WCPU0_0                   WJ_IUM_WCPU0_IUM_WCPU0_0_Msk

#define WJ_IUM_WCPU0_IUM_WCPU0_1_Pos               (1U)
#define WJ_IUM_WCPU0_IUM_WCPU0_1_Msk               (0x1U << WJ_IUM_WCPU0_IUM_WCPU0_1_Pos)
#define WJ_IUM_WCPU0_IUM_WCPU0_1                   WJ_IUM_WCPU0_IUM_WCPU0_1_Msk

#define WJ_IUM_WCPU0_IUM_WCPU0_2_Pos               (2U)
#define WJ_IUM_WCPU0_IUM_WCPU0_2_Msk               (0x1U << WJ_IUM_WCPU0_IUM_WCPU0_2_Pos)
#define WJ_IUM_WCPU0_IUM_WCPU0_2                   WJ_IUM_WCPU0_IUM_WCPU0_2_Msk

#define WJ_IUM_WCPU0_IUM_WCPU0_3_Pos               (3U)
#define WJ_IUM_WCPU0_IUM_WCPU0_3_Msk               (0x1U << WJ_IUM_WCPU0_IUM_WCPU0_3_Pos)
#define WJ_IUM_WCPU0_IUM_WCPU0_3                   WJ_IUM_WCPU0_IUM_WCPU0_3_Msk

#define WJ_IUM_WCPU0_IUM_WCPU0_4_Pos               (4U)
#define WJ_IUM_WCPU0_IUM_WCPU0_4_Msk               (0x1U << WJ_IUM_WCPU0_IUM_WCPU0_4_Pos)
#define WJ_IUM_WCPU0_IUM_WCPU0_4                   WJ_IUM_WCPU0_IUM_WCPU0_4_Msk

#define WJ_IUM_WCPU0_IUM_WCPU0_5_Pos               (5U)
#define WJ_IUM_WCPU0_IUM_WCPU0_5_Msk               (0x1U << WJ_IUM_WCPU0_IUM_WCPU0_5_Pos)
#define WJ_IUM_WCPU0_IUM_WCPU0_5                   WJ_IUM_WCPU0_IUM_WCPU0_5_Msk

#define WJ_IUM_WCPU0_IUM_WCPU0_6_Pos               (6U)
#define WJ_IUM_WCPU0_IUM_WCPU0_6_Msk               (0x1U << WJ_IUM_WCPU0_IUM_WCPU0_6_Pos)
#define WJ_IUM_WCPU0_IUM_WCPU0_6                   WJ_IUM_WCPU0_IUM_WCPU0_6_Msk

#define WJ_IUM_WCPU0_IUM_WCPU0_7_Pos               (7U)
#define WJ_IUM_WCPU0_IUM_WCPU0_7_Msk               (0x7U << WJ_IUM_WCPU0_IUM_WCPU0_7_Pos)
#define WJ_IUM_WCPU0_IUM_WCPU0_7                   WJ_IUM_WCPU0_IUM_WCPU0_7_Msk

#define WJ_IUM_WCPU0_IUM_WCPU0_8_Pos               (8U)
#define WJ_IUM_WCPU0_IUM_WCPU0_8_Msk               (0x1U << WJ_IUM_WCPU0_IUM_WCPU0_8_Pos)
#define WJ_IUM_WCPU0_IUM_WCPU0_8                   WJ_IUM_WCPU0_IUM_WCPU0_8_Msk

#define WJ_IUM_WCPU0_IUM_WCPU0_9_Pos               (9U)
#define WJ_IUM_WCPU0_IUM_WCPU0_9_Msk               (0x1U << WJ_IUM_WCPU0_IUM_WCPU0_9_Pos)
#define WJ_IUM_WCPU0_IUM_WCPU0_9                   WJ_IUM_WCPU0_IUM_WCPU0_9_Msk

#define WJ_IUM_WCPU0_IUM_WCPU0_10_Pos              (10U)
#define WJ_IUM_WCPU0_IUM_WCPU0_10_Msk              (0x1U << WJ_IUM_WCPU0_IUM_WCPU0_10_Pos)
#define WJ_IUM_WCPU0_IUM_WCPU0_10                  WJ_IUM_WCPU0_IUM_WCPU0_10_Msk

#define WJ_IUM_WCPU0_IUM_WCPU0_11_Pos              (11U)
#define WJ_IUM_WCPU0_IUM_WCPU0_11_Msk              (0x1U << WJ_IUM_WCPU0_IUM_WCPU0_11_Pos)
#define WJ_IUM_WCPU0_IUM_WCPU0_11                  WJ_IUM_WCPU0_IUM_WCPU0_11_Msk

/* IS_WCPU0, offset: 0x08 */
#define WJ_IS_WCPU0_IS_WCPU0_0_Pos                 (0U)
#define WJ_IS_WCPU0_IS_WCPU0_0_Msk                 (0x1U << WJ_IS_WCPU0_IS_WCPU0_0_Pos)
#define WJ_IS_WCPU0_IS_WCPU0_0                     WJ_IS_WCPU0_IS_WCPU0_0_Msk

#define WJ_IS_WCPU0_IS_WCPU0_1_Pos                 (1U)
#define WJ_IS_WCPU0_IS_WCPU0_1_Msk                 (0x1U << WJ_IS_WCPU0_IS_WCPU0_1_Pos)
#define WJ_IS_WCPU0_IS_WCPU0_1                     WJ_IS_WCPU0_IS_WCPU0_1_Msk

#define WJ_IS_WCPU0_IS_WCPU0_2_Pos                 (2U)
#define WJ_IS_WCPU0_IS_WCPU0_2_Msk                 (0x1U << WJ_IS_WCPU0_IS_WCPU0_2_Pos)
#define WJ_IS_WCPU0_IS_WCPU0_2                     WJ_IS_WCPU0_IS_WCPU0_2_Msk

#define WJ_IS_WCPU0_IS_WCPU0_3_Pos                 (3U)
#define WJ_IS_WCPU0_IS_WCPU0_3_Msk                 (0x1U << WJ_IS_WCPU0_IS_WCPU0_3_Pos)
#define WJ_IS_WCPU0_IS_WCPU0_3                     WJ_IS_WCPU0_IS_WCPU0_3_Msk

#define WJ_IS_WCPU0_IS_WCPU0_4_Pos                 (4U)
#define WJ_IS_WCPU0_IS_WCPU0_4_Msk                 (0x1U << WJ_IS_WCPU0_IS_WCPU0_4_Pos)
#define WJ_IS_WCPU0_IS_WCPU0_4                     WJ_IS_WCPU0_IS_WCPU0_4_Msk

#define WJ_IS_WCPU0_IS_WCPU0_5_Pos                 (5U)
#define WJ_IS_WCPU0_IS_WCPU0_5_Msk                 (0x1U << WJ_IS_WCPU0_IS_WCPU0_5_Pos)
#define WJ_IS_WCPU0_IS_WCPU0_5                     WJ_IS_WCPU0_IS_WCPU0_5_Msk

#define WJ_IS_WCPU0_IS_WCPU0_6_Pos                 (6U)
#define WJ_IS_WCPU0_IS_WCPU0_6_Msk                 (0x1U << WJ_IS_WCPU0_IS_WCPU0_6_Pos)
#define WJ_IS_WCPU0_IS_WCPU0_6                     WJ_IS_WCPU0_IS_WCPU0_6_Msk

#define WJ_IS_WCPU0_IS_WCPU0_7_Pos                 (7U)
#define WJ_IS_WCPU0_IS_WCPU0_7_Msk                 (0x7U << WJ_IS_WCPU0_IS_WCPU0_7_Pos)
#define WJ_IS_WCPU0_IS_WCPU0_7                     WJ_IS_WCPU0_IS_WCPU0_7_Msk

#define WJ_IS_WCPU0_IS_WCPU0_8_Pos                 (8U)
#define WJ_IS_WCPU0_IS_WCPU0_8_Msk                 (0x1U << WJ_IS_WCPU0_IS_WCPU0_8_Pos)
#define WJ_IS_WCPU0_IS_WCPU0_8                     WJ_IS_WCPU0_IS_WCPU0_8_Msk

#define WJ_IS_WCPU0_IS_WCPU0_9_Pos                 (9U)
#define WJ_IS_WCPU0_IS_WCPU0_9_Msk                 (0x1U << WJ_IS_WCPU0_IS_WCPU0_9_Pos)
#define WJ_IS_WCPU0_IS_WCPU0_9                     WJ_IS_WCPU0_IS_WCPU0_9_Msk

#define WJ_IS_WCPU0_IS_WCPU0_10_Pos                (10U)
#define WJ_IS_WCPU0_IS_WCPU0_10_Msk                (0x1U << WJ_IS_WCPU0_IS_WCPU0_10_Pos)
#define WJ_IS_WCPU0_IS_WCPU0_10                    WJ_IS_WCPU0_IS_WCPU0_10_Msk

#define WJ_IS_WCPU0_IS_WCPU0_11_Pos                (11U)
#define WJ_IS_WCPU0_IS_WCPU0_11_Msk                (0x1U << WJ_IS_WCPU0_IS_WCPU0_11_Pos)
#define WJ_IS_WCPU0_IS_WCPU0_11                    WJ_IS_WCPU0_IS_WCPU0_11_Msk

/* IC_WCPU0 , offset: 0x0C */
#define WJ_IC_WCPU0_IC_WCPU0_0_Pos                 (0U)
#define WJ_IC_WCPU0_IC_WCPU0_0_Msk                 (0x1U << WJ_IC_WCPU0_IC_WCPU0_0_Pos)
#define WJ_IC_WCPU0_IC_WCPU0_0                     WJ_IC_WCPU0_IC_WCPU0_0_Msk

#define WJ_IC_WCPU0_IC_WCPU0_1_Pos                 (1U)
#define WJ_IC_WCPU0_IC_WCPU0_1_Msk                 (0x1U << WJ_IC_WCPU0_IC_WCPU0_1_Pos)
#define WJ_IC_WCPU0_IC_WCPU0_1                     WJ_IC_WCPU0_IC_WCPU0_1_Msk

#define WJ_IC_WCPU0_IC_WCPU0_2_Pos                 (2U)
#define WJ_IC_WCPU0_IC_WCPU0_2_Msk                 (0x1U << WJ_IC_WCPU0_IC_WCPU0_2_Pos)
#define WJ_IC_WCPU0_IC_WCPU0_2                     WJ_IC_WCPU0_IC_WCPU0_2_Msk

#define WJ_IC_WCPU0_IC_WCPU0_3_Pos                 (3U)
#define WJ_IC_WCPU0_IC_WCPU0_3_Msk                 (0x1U << WJ_IC_WCPU0_IC_WCPU0_3_Pos)
#define WJ_IC_WCPU0_IC_WCPU0_3                     WJ_IC_WCPU0_IC_WCPU0_3_Msk

#define WJ_IC_WCPU0_IC_WCPU0_4_Pos                 (4U)
#define WJ_IC_WCPU0_IC_WCPU0_4_Msk                 (0x1U << WJ_IC_WCPU0_IC_WCPU0_4_Pos)
#define WJ_IC_WCPU0_IC_WCPU0_4                     WJ_IC_WCPU0_IC_WCPU0_4_Msk

#define WJ_IC_WCPU0_IC_WCPU0_5_Pos                 (5U)
#define WJ_IC_WCPU0_IC_WCPU0_5_Msk                 (0x1U << WJ_IC_WCPU0_IC_WCPU0_5_Pos)
#define WJ_IC_WCPU0_IC_WCPU0_5                     WJ_IC_WCPU0_IC_WCPU0_5_Msk

#define WJ_IC_WCPU0_IC_WCPU0_6_Pos                 (6U)
#define WJ_IC_WCPU0_IC_WCPU0_6_Msk                 (0x1U << WJ_IC_WCPU0_IC_WCPU0_6_Pos)
#define WJ_IC_WCPU0_IC_WCPU0_6                     WJ_IC_WCPU0_IC_WCPU0_6_Msk

#define WJ_IC_WCPU0_IC_WCPU0_7_Pos                 (7U)
#define WJ_IC_WCPU0_IC_WCPU0_7_Msk                 (0x7U << WJ_IC_WCPU0_IC_WCPU0_7_Pos)
#define WJ_IC_WCPU0_IC_WCPU0_7                     WJ_IC_WCPU0_IC_WCPU0_7_Msk

#define WJ_IC_WCPU0_IC_WCPU0_8_Pos                 (8U)
#define WJ_IC_WCPU0_IC_WCPU0_8_Msk                 (0x1U << WJ_IC_WCPU0_IC_WCPU0_8_Pos)
#define WJ_IC_WCPU0_IC_WCPU0_8                     WJ_IC_WCPU0_IC_WCPU0_8_Msk

#define WJ_IC_WCPU0_IC_WCPU0_9_Pos                 (9U)
#define WJ_IC_WCPU0_IC_WCPU0_9_Msk                 (0x1U << WJ_IC_WCPU0_IC_WCPU0_9_Pos)
#define WJ_IC_WCPU0_IC_WCPU0_9                     WJ_IC_WCPU0_IC_WCPU0_9_Msk

#define WJ_IC_WCPU0_IC_WCPU0_10_Pos                (10U)
#define WJ_IC_WCPU0_IC_WCPU0_10_Msk                (0x1U << WJ_IC_WCPU0_IC_WCPU0_10_Pos)
#define WJ_IC_WCPU0_IC_WCPU0_10                    WJ_IC_WCPU0_IC_WCPU0_10_Msk

#define WJ_IC_WCPU0_IC_WCPU0_11_Pos                (11U)
#define WJ_IC_WCPU0_IC_WCPU0_11_Msk                (0x1U << WJ_IC_WCPU0_IC_WCPU0_11_Pos)
#define WJ_IC_WCPU0_IC_WCPU0_11                    WJ_IC_WCPU0_IC_WCPU0_11_Msk

/* RIS_RCPU0, offset: 0x10 */
#define WJ_RIS_RCPU0_RIS_RCPU1_0_Pos               (0U)
#define WJ_RIS_RCPU0_RIS_RCPU1_0_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU1_0_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU1_0                   WJ_RIS_RCPU0_RIS_RCPU1_0_Msk

#define WJ_RIS_RCPU0_RIS_RCPU1_1_Pos               (1U)
#define WJ_RIS_RCPU0_RIS_RCPU1_1_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU1_1_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU1_1                   WJ_RIS_RCPU0_RIS_RCPU1_1_Msk

#define WJ_RIS_RCPU0_RIS_RCPU1_2_Pos               (2U)
#define WJ_RIS_RCPU0_RIS_RCPU1_2_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU1_2_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU1_2                   WJ_RIS_RCPU0_RIS_RCPU1_2_Msk

#define WJ_RIS_RCPU0_RIS_RCPU1_3_Pos               (3U)
#define WJ_RIS_RCPU0_RIS_RCPU1_3_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU1_3_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU1_3                   WJ_RIS_RCPU0_RIS_RCPU1_3_Msk

#define WJ_RIS_RCPU0_RIS_RCPU1_4_Pos               (4U)
#define WJ_RIS_RCPU0_RIS_RCPU1_4_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU1_4_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU1_4                   WJ_RIS_RCPU0_RIS_RCPU1_4_Msk

#define WJ_RIS_RCPU0_RIS_RCPU1_5_Pos               (5U)
#define WJ_RIS_RCPU0_RIS_RCPU1_5_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU1_5_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU1_5                   WJ_RIS_RCPU0_RIS_RCPU1_5_Msk

#define WJ_RIS_RCPU0_RIS_RCPU1_6_Pos               (6U)
#define WJ_RIS_RCPU0_RIS_RCPU1_6_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU1_6_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU1_6                   WJ_RIS_RCPU0_RIS_RCPU1_6_Msk

#define WJ_RIS_RCPU0_RIS_RCPU1_7_Pos               (7U)
#define WJ_RIS_RCPU0_RIS_RCPU1_7_Msk               (0x7U << WJ_RIS_RCPU0_RIS_RCPU1_7_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU1_7                   WJ_RIS_RCPU0_RIS_RCPU1_7_Msk

#define WJ_RIS_RCPU0_RIS_RCPU1_8_Pos               (8U)
#define WJ_RIS_RCPU0_RIS_RCPU1_8_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU1_8_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU1_8                   WJ_RIS_RCPU0_RIS_RCPU1_8_Msk

#define WJ_RIS_RCPU0_RIS_RCPU1_9_Pos               (9U)
#define WJ_RIS_RCPU0_RIS_RCPU1_9_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU1_9_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU1_9                   WJ_RIS_RCPU0_RIS_RCPU1_9_Msk

#define WJ_RIS_RCPU0_RIS_RCPU1_10_Pos              (10U)
#define WJ_RIS_RCPU0_RIS_RCPU1_10_Msk              (0x1U << WJ_RIS_RCPU0_RIS_RCPU1_10_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU1_10                  WJ_RIS_RCPU0_RIS_RCPU1_10_Msk

#define WJ_RIS_RCPU0_RIS_RCPU1_11_Pos              (11U)
#define WJ_RIS_RCPU0_RIS_RCPU1_11_Msk              (0x1U << WJ_RIS_RCPU0_RIS_RCPU1_11_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU1_11                  WJ_RIS_RCPU0_RIS_RCPU1_11_Msk

#define WJ_RIS_RCPU0_RIS_RCPU2_0_Pos               (16U)
#define WJ_RIS_RCPU0_RIS_RCPU2_0_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU2_0_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU2_0                   WJ_RIS_RCPU0_RIS_RCPU2_0_Msk

#define WJ_RIS_RCPU0_RIS_RCPU2_1_Pos               (17U)
#define WJ_RIS_RCPU0_RIS_RCPU2_1_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU2_1_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU2_1                   WJ_RIS_RCPU0_RIS_RCPU2_1_Msk

#define WJ_RIS_RCPU0_RIS_RCPU2_2_Pos               (18U)
#define WJ_RIS_RCPU0_RIS_RCPU2_2_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU2_2_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU2_2                   WJ_RIS_RCPU0_RIS_RCPU2_2_Msk

#define WJ_RIS_RCPU0_RIS_RCPU2_3_Pos               (19U)
#define WJ_RIS_RCPU0_RIS_RCPU2_3_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU2_3_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU2_3                   WJ_RIS_RCPU0_RIS_RCPU2_3_Msk

#define WJ_RIS_RCPU0_RIS_RCPU2_4_Pos               (20U)
#define WJ_RIS_RCPU0_RIS_RCPU2_4_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU2_4_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU2_4                   WJ_RIS_RCPU0_RIS_RCPU2_4_Msk

#define WJ_RIS_RCPU0_RIS_RCPU2_5_Pos               (21U)
#define WJ_RIS_RCPU0_RIS_RCPU2_5_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU2_5_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU2_5                   WJ_RIS_RCPU0_RIS_RCPU2_5_Msk

#define WJ_RIS_RCPU0_RIS_RCPU2_6_Pos               (22U)
#define WJ_RIS_RCPU0_RIS_RCPU2_6_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU2_6_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU2_6                   WJ_RIS_RCPU0_RIS_RCPU2_6_Msk

#define WJ_RIS_RCPU0_RIS_RCPU2_7_Pos               (23U)
#define WJ_RIS_RCPU0_RIS_RCPU2_7_Msk               (0x7U << WJ_RIS_RCPU0_RIS_RCPU2_7_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU2_7                   WJ_RIS_RCPU0_RIS_RCPU2_7_Msk

#define WJ_RIS_RCPU0_RIS_RCPU2_8_Pos               (24U)
#define WJ_RIS_RCPU0_RIS_RCPU2_8_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU2_8_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU2_8                   WJ_RIS_RCPU0_RIS_RCPU2_8_Msk

#define WJ_RIS_RCPU0_RIS_RCPU2_9_Pos               (25U)
#define WJ_RIS_RCPU0_RIS_RCPU2_9_Msk               (0x1U << WJ_RIS_RCPU0_RIS_RCPU2_9_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU2_9                   WJ_RIS_RCPU0_RIS_RCPU2_9_Msk

#define WJ_RIS_RCPU0_RIS_RCPU2_10_Pos              (26U)
#define WJ_RIS_RCPU0_RIS_RCPU2_10_Msk              (0x1U << WJ_RIS_RCPU0_RIS_RCPU2_10_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU2_10                  WJ_RIS_RCPU0_RIS_RCPU2_10_Msk

#define WJ_RIS_RCPU0_RIS_RCPU2_11_Pos              (27U)
#define WJ_RIS_RCPU0_RIS_RCPU2_11_Msk              (0x1U << WJ_RIS_RCPU0_RIS_RCPU2_11_Pos)
#define WJ_RIS_RCPU0_RIS_RCPU2_11                  WJ_RIS_RCPU0_RIS_RCPU2_11_Msk


/* IUM_RCPU0, offset: 0x14 */
#define WJ_IUM_RCPU0_IUM_RCPU1_0_Pos               (0U)
#define WJ_IUM_RCPU0_IUM_RCPU1_0_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU1_0_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU1_0                   WJ_IUM_RCPU0_IUM_RCPU1_0_Msk

#define WJ_IUM_RCPU0_IUM_RCPU1_1_Pos               (1U)
#define WJ_IUM_RCPU0_IUM_RCPU1_1_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU1_1_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU1_1                   WJ_IUM_RCPU0_IUM_RCPU1_1_Msk

#define WJ_IUM_RCPU0_IUM_RCPU1_2_Pos               (2U)
#define WJ_IUM_RCPU0_IUM_RCPU1_2_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU1_2_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU1_2                   WJ_IUM_RCPU0_IUM_RCPU1_2_Msk

#define WJ_IUM_RCPU0_IUM_RCPU1_3_Pos               (3U)
#define WJ_IUM_RCPU0_IUM_RCPU1_3_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU1_3_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU1_3                   WJ_IUM_RCPU0_IUM_RCPU1_3_Msk

#define WJ_IUM_RCPU0_IUM_RCPU1_4_Pos               (4U)
#define WJ_IUM_RCPU0_IUM_RCPU1_4_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU1_4_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU1_4                   WJ_IUM_RCPU0_IUM_RCPU1_4_Msk

#define WJ_IUM_RCPU0_IUM_RCPU1_5_Pos               (5U)
#define WJ_IUM_RCPU0_IUM_RCPU1_5_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU1_5_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU1_5                   WJ_IUM_RCPU0_IUM_RCPU1_5_Msk

#define WJ_IUM_RCPU0_IUM_RCPU1_6_Pos               (6U)
#define WJ_IUM_RCPU0_IUM_RCPU1_6_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU1_6_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU1_6                   WJ_IUM_RCPU0_IUM_RCPU1_6_Msk

#define WJ_IUM_RCPU0_IUM_RCPU1_7_Pos               (7U)
#define WJ_IUM_RCPU0_IUM_RCPU1_7_Msk               (0x7U << WJ_IUM_RCPU0_IUM_RCPU1_7_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU1_7                   WJ_IUM_RCPU0_IUM_RCPU1_7_Msk

#define WJ_IUM_RCPU0_IUM_RCPU1_8_Pos               (8U)
#define WJ_IUM_RCPU0_IUM_RCPU1_8_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU1_8_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU1_8                   WJ_IUM_RCPU0_IUM_RCPU1_8_Msk

#define WJ_IUM_RCPU0_IUM_RCPU1_9_Pos               (9U)
#define WJ_IUM_RCPU0_IUM_RCPU1_9_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU1_9_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU1_9                   WJ_IUM_RCPU0_IUM_RCPU1_9_Msk

#define WJ_IUM_RCPU0_IUM_RCPU1_10_Pos              (10U)
#define WJ_IUM_RCPU0_IUM_RCPU1_10_Msk              (0x1U << WJ_IUM_RCPU0_IUM_RCPU1_10_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU1_10                  WJ_IUM_RCPU0_IUM_RCPU1_10_Msk

#define WJ_IUM_RCPU0_IUM_RCPU1_11_Pos              (11U)
#define WJ_IUM_RCPU0_IUM_RCPU1_11_Msk              (0x1U << WJ_IUM_RCPU0_IUM_RCPU1_11_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU1_11                  WJ_IUM_RCPU0_IUM_RCPU1_11_Msk

#define WJ_IUM_RCPU0_IUM_RCPU2_0_Pos               (16U)
#define WJ_IUM_RCPU0_IUM_RCPU2_0_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU2_0_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU2_0                   WJ_IUM_RCPU0_IUM_RCPU2_0_Msk

#define WJ_IUM_RCPU0_IUM_RCPU2_1_Pos               (17U)
#define WJ_IUM_RCPU0_IUM_RCPU2_1_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU2_1_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU2_1                   WJ_IUM_RCPU0_IUM_RCPU2_1_Msk

#define WJ_IUM_RCPU0_IUM_RCPU2_2_Pos               (18U)
#define WJ_IUM_RCPU0_IUM_RCPU2_2_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU2_2_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU2_2                   WJ_IUM_RCPU0_IUM_RCPU2_2_Msk

#define WJ_IUM_RCPU0_IUM_RCPU2_3_Pos               (19U)
#define WJ_IUM_RCPU0_IUM_RCPU2_3_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU2_3_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU2_3                   WJ_IUM_RCPU0_IUM_RCPU2_3_Msk

#define WJ_IUM_RCPU0_IUM_RCPU2_4_Pos               (20U)
#define WJ_IUM_RCPU0_IUM_RCPU2_4_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU2_4_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU2_4                   WJ_IUM_RCPU0_IUM_RCPU2_4_Msk

#define WJ_IUM_RCPU0_IUM_RCPU2_5_Pos               (21U)
#define WJ_IUM_RCPU0_IUM_RCPU2_5_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU2_5_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU2_5                   WJ_IUM_RCPU0_IUM_RCPU2_5_Msk

#define WJ_IUM_RCPU0_IUM_RCPU2_6_Pos               (22U)
#define WJ_IUM_RCPU0_IUM_RCPU2_6_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU2_6_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU2_6                   WJ_IUM_RCPU0_IUM_RCPU2_6_Msk

#define WJ_IUM_RCPU0_IUM_RCPU2_7_Pos               (23U)
#define WJ_IUM_RCPU0_IUM_RCPU2_7_Msk               (0x7U << WJ_IUM_RCPU0_IUM_RCPU2_7_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU2_7                   WJ_IUM_RCPU0_IUM_RCPU2_7_Msk

#define WJ_IUM_RCPU0_IUM_RCPU2_8_Pos               (24U)
#define WJ_IUM_RCPU0_IUM_RCPU2_8_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU2_8_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU2_8                   WJ_IUM_RCPU0_IUM_RCPU2_8_Msk

#define WJ_IUM_RCPU0_IUM_RCPU2_9_Pos               (25U)
#define WJ_IUM_RCPU0_IUM_RCPU2_9_Msk               (0x1U << WJ_IUM_RCPU0_IUM_RCPU2_9_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU2_9                   WJ_IUM_RCPU0_IUM_RCPU2_9_Msk

#define WJ_IUM_RCPU0_IUM_RCPU2_10_Pos              (26U)
#define WJ_IUM_RCPU0_IUM_RCPU2_10_Msk              (0x1U << WJ_IUM_RCPU0_IUM_RCPU2_10_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU2_10                  WJ_IUM_RCPU0_IUM_RCPU2_10_Msk

#define WJ_IUM_RCPU0_IUM_RCPU2_11_Pos              (27U)
#define WJ_IUM_RCPU0_IUM_RCPU2_11_Msk              (0x1U << WJ_IUM_RCPU0_IUM_RCPU2_11_Pos)
#define WJ_IUM_RCPU0_IUM_RCPU2_11                  WJ_IUM_RCPU0_IUM_RCPU2_11_Msk

/* IS_RCPU0, offset: 0x18 */
#define WJ_IS_RCPU0_IS_RCPU1_0_Pos                 (0U)
#define WJ_IS_RCPU0_IS_RCPU1_0_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU1_0_Pos)
#define WJ_IS_RCPU0_IS_RCPU1_0                     WJ_IS_RCPU0_IS_RCPU1_0_Msk

#define WJ_IS_RCPU0_IS_RCPU1_1_Pos                 (1U)
#define WJ_IS_RCPU0_IS_RCPU1_1_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU1_1_Pos)
#define WJ_IS_RCPU0_IS_RCPU1_1                     WJ_IS_RCPU0_IS_RCPU1_1_Msk

#define WJ_IS_RCPU0_IS_RCPU1_2_Pos                 (2U)
#define WJ_IS_RCPU0_IS_RCPU1_2_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU1_2_Pos)
#define WJ_IS_RCPU0_IS_RCPU1_2                     WJ_IS_RCPU0_IS_RCPU1_2_Msk

#define WJ_IS_RCPU0_IS_RCPU1_3_Pos                 (3U)
#define WJ_IS_RCPU0_IS_RCPU1_3_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU1_3_Pos)
#define WJ_IS_RCPU0_IS_RCPU1_3                     WJ_IS_RCPU0_IS_RCPU1_3_Msk

#define WJ_IS_RCPU0_IS_RCPU1_4_Pos                 (4U)
#define WJ_IS_RCPU0_IS_RCPU1_4_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU1_4_Pos)
#define WJ_IS_RCPU0_IS_RCPU1_4                     WJ_IS_RCPU0_IS_RCPU1_4_Msk

#define WJ_IS_RCPU0_IS_RCPU1_5_Pos                 (5U)
#define WJ_IS_RCPU0_IS_RCPU1_5_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU1_5_Pos)
#define WJ_IS_RCPU0_IS_RCPU1_5                     WJ_IS_RCPU0_IS_RCPU1_5_Msk

#define WJ_IS_RCPU0_IS_RCPU1_6_Pos                 (6U)
#define WJ_IS_RCPU0_IS_RCPU1_6_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU1_6_Pos)
#define WJ_IS_RCPU0_IS_RCPU1_6                     WJ_IS_RCPU0_IS_RCPU1_6_Msk

#define WJ_IS_RCPU0_IS_RCPU1_7_Pos                 (7U)
#define WJ_IS_RCPU0_IS_RCPU1_7_Msk                 (0x7U << WJ_IS_RCPU0_IS_RCPU1_7_Pos)
#define WJ_IS_RCPU0_IS_RCPU1_7                     WJ_IS_RCPU0_IS_RCPU1_7_Msk

#define WJ_IS_RCPU0_IS_RCPU1_8_Pos                 (8U)
#define WJ_IS_RCPU0_IS_RCPU1_8_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU1_8_Pos)
#define WJ_IS_RCPU0_IS_RCPU1_8                     WJ_IS_RCPU0_IS_RCPU1_8_Msk

#define WJ_IS_RCPU0_IS_RCPU1_9_Pos                 (9U)
#define WJ_IS_RCPU0_IS_RCPU1_9_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU1_9_Pos)
#define WJ_IS_RCPU0_IS_RCPU1_9                     WJ_IS_RCPU0_IS_RCPU1_9_Msk

#define WJ_IS_RCPU0_IS_RCPU1_10_Pos                (10U)
#define WJ_IS_RCPU0_IS_RCPU1_10_Msk                (0x1U << WJ_IS_RCPU0_IS_RCPU1_10_Pos)
#define WJ_IS_RCPU0_IS_RCPU1_10                    WJ_IS_RCPU0_IS_RCPU1_10_Msk

#define WJ_IS_RCPU0_IS_RCPU1_11_Pos                (11U)
#define WJ_IS_RCPU0_IS_RCPU1_11_Msk                (0x1U << WJ_IS_RCPU0_IS_RCPU1_11_Pos)
#define WJ_IS_RCPU0_IS_RCPU1_11                    WJ_IS_RCPU0_IS_RCPU1_11_Msk

#define WJ_IS_RCPU0_IS_RCPU2_0_Pos                 (16U)
#define WJ_IS_RCPU0_IS_RCPU2_0_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU2_0_Pos)
#define WJ_IS_RCPU0_IS_RCPU2_0                     WJ_IS_RCPU0_IS_RCPU2_0_Msk

#define WJ_IS_RCPU0_IS_RCPU2_1_Pos                 (17U)
#define WJ_IS_RCPU0_IS_RCPU2_1_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU2_1_Pos)
#define WJ_IS_RCPU0_IS_RCPU2_1                     WJ_IS_RCPU0_IS_RCPU2_1_Msk

#define WJ_IS_RCPU0_IS_RCPU2_2_Pos                 (18U)
#define WJ_IS_RCPU0_IS_RCPU2_2_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU2_2_Pos)
#define WJ_IS_RCPU0_IS_RCPU2_2                     WJ_IS_RCPU0_IS_RCPU2_2_Msk

#define WJ_IS_RCPU0_IS_RCPU2_3_Pos                 (19U)
#define WJ_IS_RCPU0_IS_RCPU2_3_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU2_3_Pos)
#define WJ_IS_RCPU0_IS_RCPU2_3                     WJ_IS_RCPU0_IS_RCPU2_3_Msk

#define WJ_IS_RCPU0_IS_RCPU2_4_Pos                 (20U)
#define WJ_IS_RCPU0_IS_RCPU2_4_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU2_4_Pos)
#define WJ_IS_RCPU0_IS_RCPU2_4                     WJ_IS_RCPU0_IS_RCPU2_4_Msk

#define WJ_IS_RCPU0_IS_RCPU2_5_Pos                 (21U)
#define WJ_IS_RCPU0_IS_RCPU2_5_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU2_5_Pos)
#define WJ_IS_RCPU0_IS_RCPU2_5                     WJ_IS_RCPU0_IS_RCPU2_5_Msk

#define WJ_IS_RCPU0_IS_RCPU2_6_Pos                 (22U)
#define WJ_IS_RCPU0_IS_RCPU2_6_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU2_6_Pos)
#define WJ_IS_RCPU0_IS_RCPU2_6                     WJ_IS_RCPU0_IS_RCPU2_6_Msk

#define WJ_IS_RCPU0_IS_RCPU2_7_Pos                 (23U)
#define WJ_IS_RCPU0_IS_RCPU2_7_Msk                 (0x7U << WJ_IS_RCPU0_IS_RCPU2_7_Pos)
#define WJ_IS_RCPU0_IS_RCPU2_7                     WJ_IS_RCPU0_IS_RCPU2_7_Msk

#define WJ_IS_RCPU0_IS_RCPU2_8_Pos                 (24U)
#define WJ_IS_RCPU0_IS_RCPU2_8_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU2_8_Pos)
#define WJ_IS_RCPU0_IS_RCPU2_8                     WJ_IS_RCPU0_IS_RCPU2_8_Msk

#define WJ_IS_RCPU0_IS_RCPU2_9_Pos                 (25U)
#define WJ_IS_RCPU0_IS_RCPU2_9_Msk                 (0x1U << WJ_IS_RCPU0_IS_RCPU2_9_Pos)
#define WJ_IS_RCPU0_IS_RCPU2_9                     WJ_IS_RCPU0_IS_RCPU2_9_Msk

#define WJ_IS_RCPU0_IS_RCPU2_10_Pos                (26U)
#define WJ_IS_RCPU0_IS_RCPU2_10_Msk                (0x1U << WJ_IS_RCPU0_IS_RCPU2_10_Pos)
#define WJ_IS_RCPU0_IS_RCPU2_10                    WJ_IS_RCPU0_IS_RCPU2_10_Msk

#define WJ_IS_RCPU0_IS_RCPU2_11_Pos                (27U)
#define WJ_IS_RCPU0_IS_RCPU2_11_Msk                (0x1U << WJ_IS_RCPU0_IS_RCPU2_11_Pos)
#define WJ_IS_RCPU0_IS_RCPU2_11                    WJ_IS_RCPU0_IS_RCPU2_11_Msk

/* IC_RCPU0, offset: 0x1C */
#define WJ_IC_RCPU0_IC_RCPU1_0_Pos                 (0U)
#define WJ_IC_RCPU0_IC_RCPU1_0_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU1_0_Pos)
#define WJ_IC_RCPU0_IC_RCPU1_0                     WJ_IC_RCPU0_IC_RCPU1_0_Msk

#define WJ_IC_RCPU0_IC_RCPU1_1_Pos                 (1U)
#define WJ_IC_RCPU0_IC_RCPU1_1_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU1_1_Pos)
#define WJ_IC_RCPU0_IC_RCPU1_1                     WJ_IC_RCPU0_IC_RCPU1_1_Msk

#define WJ_IC_RCPU0_IC_RCPU1_2_Pos                 (2U)
#define WJ_IC_RCPU0_IC_RCPU1_2_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU1_2_Pos)
#define WJ_IC_RCPU0_IC_RCPU1_2                     WJ_IC_RCPU0_IC_RCPU1_2_Msk

#define WJ_IC_RCPU0_IC_RCPU1_3_Pos                 (3U)
#define WJ_IC_RCPU0_IC_RCPU1_3_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU1_3_Pos)
#define WJ_IC_RCPU0_IC_RCPU1_3                     WJ_IC_RCPU0_IC_RCPU1_3_Msk

#define WJ_IC_RCPU0_IC_RCPU1_4_Pos                 (4U)
#define WJ_IC_RCPU0_IC_RCPU1_4_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU1_4_Pos)
#define WJ_IC_RCPU0_IC_RCPU1_4                     WJ_IC_RCPU0_IC_RCPU1_4_Msk

#define WJ_IC_RCPU0_IC_RCPU1_5_Pos                 (5U)
#define WJ_IC_RCPU0_IC_RCPU1_5_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU1_5_Pos)
#define WJ_IC_RCPU0_IC_RCPU1_5                     WJ_IC_RCPU0_IC_RCPU1_5_Msk

#define WJ_IC_RCPU0_IC_RCPU1_6_Pos                 (6U)
#define WJ_IC_RCPU0_IC_RCPU1_6_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU1_6_Pos)
#define WJ_IC_RCPU0_IC_RCPU1_6                     WJ_IC_RCPU0_IC_RCPU1_6_Msk

#define WJ_IC_RCPU0_IC_RCPU1_7_Pos                 (7U)
#define WJ_IC_RCPU0_IC_RCPU1_7_Msk                 (0x7U << WJ_IC_RCPU0_IC_RCPU1_7_Pos)
#define WJ_IC_RCPU0_IC_RCPU1_7                     WJ_IC_RCPU0_IC_RCPU1_7_Msk

#define WJ_IC_RCPU0_IC_RCPU1_8_Pos                 (8U)
#define WJ_IC_RCPU0_IC_RCPU1_8_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU1_8_Pos)
#define WJ_IC_RCPU0_IC_RCPU1_8                     WJ_IC_RCPU0_IC_RCPU1_8_Msk

#define WJ_IC_RCPU0_IC_RCPU1_9_Pos                 (9U)
#define WJ_IC_RCPU0_IC_RCPU1_9_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU1_9_Pos)
#define WJ_IC_RCPU0_IC_RCPU1_9                     WJ_IC_RCPU0_IC_RCPU1_9_Msk

#define WJ_IC_RCPU0_IC_RCPU1_10_Pos                (10U)
#define WJ_IC_RCPU0_IC_RCPU1_10_Msk                (0x1U << WJ_IC_RCPU0_IC_RCPU1_10_Pos)
#define WJ_IC_RCPU0_IC_RCPU1_10                    WJ_IC_RCPU0_IC_RCPU1_10_Msk

#define WJ_IC_RCPU0_IC_RCPU1_11_Pos                (11U)
#define WJ_IC_RCPU0_IC_RCPU1_11_Msk                (0x1U << WJ_IC_RCPU0_IC_RCPU1_11_Pos)
#define WJ_IC_RCPU0_IC_RCPU1_11                    WJ_IC_RCPU0_IC_RCPU1_11_Msk

#define WJ_IC_RCPU0_IC_RCPU2_0_Pos                 (16U)
#define WJ_IC_RCPU0_IC_RCPU2_0_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU2_0_Pos)
#define WJ_IC_RCPU0_IC_RCPU2_0                     WJ_IC_RCPU0_IC_RCPU2_0_Msk

#define WJ_IC_RCPU0_IC_RCPU2_1_Pos                 (17U)
#define WJ_IC_RCPU0_IC_RCPU2_1_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU2_1_Pos)
#define WJ_IC_RCPU0_IC_RCPU2_1                     WJ_IC_RCPU0_IC_RCPU2_1_Msk

#define WJ_IC_RCPU0_IC_RCPU2_2_Pos                 (18U)
#define WJ_IC_RCPU0_IC_RCPU2_2_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU2_2_Pos)
#define WJ_IC_RCPU0_IC_RCPU2_2                     WJ_IC_RCPU0_IC_RCPU2_2_Msk

#define WJ_IC_RCPU0_IC_RCPU2_3_Pos                 (19U)
#define WJ_IC_RCPU0_IC_RCPU2_3_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU2_3_Pos)
#define WJ_IC_RCPU0_IC_RCPU2_3                     WJ_IC_RCPU0_IC_RCPU2_3_Msk

#define WJ_IC_RCPU0_IC_RCPU2_4_Pos                 (20U)
#define WJ_IC_RCPU0_IC_RCPU2_4_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU2_4_Pos)
#define WJ_IC_RCPU0_IC_RCPU2_4                     WJ_IC_RCPU0_IC_RCPU2_4_Msk

#define WJ_IC_RCPU0_IC_RCPU2_5_Pos                 (21U)
#define WJ_IC_RCPU0_IC_RCPU2_5_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU2_5_Pos)
#define WJ_IC_RCPU0_IC_RCPU2_5                     WJ_IC_RCPU0_IC_RCPU2_5_Msk

#define WJ_IC_RCPU0_IC_RCPU2_6_Pos                 (22U)
#define WJ_IC_RCPU0_IC_RCPU2_6_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU2_6_Pos)
#define WJ_IC_RCPU0_IC_RCPU2_6                     WJ_IC_RCPU0_IC_RCPU2_6_Msk

#define WJ_IC_RCPU0_IC_RCPU2_7_Pos                 (23U)
#define WJ_IC_RCPU0_IC_RCPU2_7_Msk                 (0x7U << WJ_IC_RCPU0_IC_RCPU2_7_Pos)
#define WJ_IC_RCPU0_IC_RCPU2_7                     WJ_IC_RCPU0_IC_RCPU2_7_Msk

#define WJ_IC_RCPU0_IC_RCPU2_8_Pos                 (24U)
#define WJ_IC_RCPU0_IC_RCPU2_8_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU2_8_Pos)
#define WJ_IC_RCPU0_IC_RCPU2_8                     WJ_IC_RCPU0_IC_RCPU2_8_Msk

#define WJ_IC_RCPU0_IC_RCPU2_9_Pos                 (25U)
#define WJ_IC_RCPU0_IC_RCPU2_9_Msk                 (0x1U << WJ_IC_RCPU0_IC_RCPU2_9_Pos)
#define WJ_IC_RCPU0_IC_RCPU2_9                     WJ_IC_RCPU0_IC_RCPU2_9_Msk

#define WJ_IC_RCPU0_IC_RCPU2_10_Pos                (26U)
#define WJ_IC_RCPU0_IC_RCPU2_10_Msk                (0x1U << WJ_IC_RCPU0_IC_RCPU2_10_Pos)
#define WJ_IC_RCPU0_IC_RCPU2_10                    WJ_IC_RCPU0_IC_RCPU2_10_Msk

#define WJ_IC_RCPU0_IC_RCPU2_11_Pos                (27U)
#define WJ_IC_RCPU0_IC_RCPU2_11_Msk                (0x1U << WJ_IC_RCPU0_IC_RCPU2_11_Pos)
#define WJ_IC_RCPU0_IC_RCPU2_11                    WJ_IC_RCPU0_IC_RCPU2_11_Msk

/* RIS_WCPU1 , offset: 0x100 */
#define WJ_RIS_WCPU1_RIS_WCPU1_0_Pos               (0U)
#define WJ_RIS_WCPU1_RIS_WCPU1_0_Msk               (0x1U << WJ_RIS_WCPU1_RIS_WCPU1_0_Pos)
#define WJ_RIS_WCPU1_RIS_WCPU1_0                   WJ_RIS_WCPU1_RIS_WCPU1_0_Msk

#define WJ_RIS_WCPU1_RIS_WCPU1_1_Pos               (1U)
#define WJ_RIS_WCPU1_RIS_WCPU1_1_Msk               (0x1U << WJ_RIS_WCPU1_RIS_WCPU1_1_Pos)
#define WJ_RIS_WCPU1_RIS_WCPU1_1                   WJ_RIS_WCPU1_RIS_WCPU1_1_Msk

#define WJ_RIS_WCPU1_RIS_WCPU1_2_Pos               (2U)
#define WJ_RIS_WCPU1_RIS_WCPU1_2_Msk               (0x1U << WJ_RIS_WCPU1_RIS_WCPU1_2_Pos)
#define WJ_RIS_WCPU1_RIS_WCPU1_2                   WJ_RIS_WCPU1_RIS_WCPU1_2_Msk

#define WJ_RIS_WCPU1_RIS_WCPU1_3_Pos               (3U)
#define WJ_RIS_WCPU1_RIS_WCPU1_3_Msk               (0x1U << WJ_RIS_WCPU1_RIS_WCPU1_3_Pos)
#define WJ_RIS_WCPU1_RIS_WCPU1_3                   WJ_RIS_WCPU1_RIS_WCPU1_3_Msk

#define WJ_RIS_WCPU1_RIS_WCPU1_4_Pos               (4U)
#define WJ_RIS_WCPU1_RIS_WCPU1_4_Msk               (0x1U << WJ_RIS_WCPU1_RIS_WCPU1_4_Pos)
#define WJ_RIS_WCPU1_RIS_WCPU1_4                   WJ_RIS_WCPU1_RIS_WCPU1_4_Msk

#define WJ_RIS_WCPU1_RIS_WCPU1_5_Pos               (5U)
#define WJ_RIS_WCPU1_RIS_WCPU1_5_Msk               (0x1U << WJ_RIS_WCPU1_RIS_WCPU1_5_Pos)
#define WJ_RIS_WCPU1_RIS_WCPU1_5                   WJ_RIS_WCPU1_RIS_WCPU1_5_Msk

#define WJ_RIS_WCPU1_RIS_WCPU1_6_Pos               (6U)
#define WJ_RIS_WCPU1_RIS_WCPU1_6_Msk               (0x1U << WJ_RIS_WCPU1_RIS_WCPU1_6_Pos)
#define WJ_RIS_WCPU1_RIS_WCPU1_6                   WJ_RIS_WCPU1_RIS_WCPU1_6_Msk

#define WJ_RIS_WCPU1_RIS_WCPU1_7_Pos               (7U)
#define WJ_RIS_WCPU1_RIS_WCPU1_7_Msk               (0x7U << WJ_RIS_WCPU1_RIS_WCPU1_7_Pos)
#define WJ_RIS_WCPU1_RIS_WCPU1_7                   WJ_RIS_WCPU1_RIS_WCPU1_7_Msk

#define WJ_RIS_WCPU1_RIS_WCPU1_8_Pos               (8U)
#define WJ_RIS_WCPU1_RIS_WCPU1_8_Msk               (0x1U << WJ_RIS_WCPU1_RIS_WCPU1_8_Pos)
#define WJ_RIS_WCPU1_RIS_WCPU1_8                   WJ_RIS_WCPU1_RIS_WCPU1_8_Msk

#define WJ_RIS_WCPU1_RIS_WCPU1_9_Pos               (9U)
#define WJ_RIS_WCPU1_RIS_WCPU1_9_Msk               (0x1U << WJ_RIS_WCPU1_RIS_WCPU1_9_Pos)
#define WJ_RIS_WCPU1_RIS_WCPU1_9                   WJ_RIS_WCPU1_RIS_WCPU1_9_Msk

#define WJ_RIS_WCPU1_RIS_WCPU1_10_Pos              (10U)
#define WJ_RIS_WCPU1_RIS_WCPU1_10_Msk              (0x1U << WJ_RIS_WCPU1_RIS_WCPU1_10_Pos)
#define WJ_RIS_WCPU1_RIS_WCPU1_10                  WJ_RIS_WCPU1_RIS_WCPU1_10_Msk

#define WJ_RIS_WCPU1_RIS_WCPU1_11_Pos              (11U)
#define WJ_RIS_WCPU1_RIS_WCPU1_11_Msk              (0x1U << WJ_RIS_WCPU1_RIS_WCPU1_11_Pos)
#define WJ_RIS_WCPU1_RIS_WCPU1_11                  WJ_RIS_WCPU1_RIS_WCPU1_11_Msk


/* IUM_WCPU1, offset: 0x104 */
#define WJ_IUM_WCPU1_IUM_WCPU1_0_Pos               (0U)
#define WJ_IUM_WCPU1_IUM_WCPU1_0_Msk               (0x1U << WJ_IUM_WCPU1_IUM_WCPU1_0_Pos)
#define WJ_IUM_WCPU1_IUM_WCPU1_0                   WJ_IUM_WCPU1_IUM_WCPU1_0_Msk

#define WJ_IUM_WCPU1_IUM_WCPU1_1_Pos               (1U)
#define WJ_IUM_WCPU1_IUM_WCPU1_1_Msk               (0x1U << WJ_IUM_WCPU1_IUM_WCPU1_1_Pos)
#define WJ_IUM_WCPU1_IUM_WCPU1_1                   WJ_IUM_WCPU1_IUM_WCPU1_1_Msk

#define WJ_IUM_WCPU1_IUM_WCPU1_2_Pos               (2U)
#define WJ_IUM_WCPU1_IUM_WCPU1_2_Msk               (0x1U << WJ_IUM_WCPU1_IUM_WCPU1_2_Pos)
#define WJ_IUM_WCPU1_IUM_WCPU1_2                   WJ_IUM_WCPU1_IUM_WCPU1_2_Msk

#define WJ_IUM_WCPU1_IUM_WCPU1_3_Pos               (3U)
#define WJ_IUM_WCPU1_IUM_WCPU1_3_Msk               (0x1U << WJ_IUM_WCPU1_IUM_WCPU1_3_Pos)
#define WJ_IUM_WCPU1_IUM_WCPU1_3                   WJ_IUM_WCPU1_IUM_WCPU1_3_Msk

#define WJ_IUM_WCPU1_IUM_WCPU1_4_Pos               (4U)
#define WJ_IUM_WCPU1_IUM_WCPU1_4_Msk               (0x1U << WJ_IUM_WCPU1_IUM_WCPU1_4_Pos)
#define WJ_IUM_WCPU1_IUM_WCPU1_4                   WJ_IUM_WCPU1_IUM_WCPU1_4_Msk

#define WJ_IUM_WCPU1_IUM_WCPU1_5_Pos               (5U)
#define WJ_IUM_WCPU1_IUM_WCPU1_5_Msk               (0x1U << WJ_IUM_WCPU1_IUM_WCPU1_5_Pos)
#define WJ_IUM_WCPU1_IUM_WCPU1_5                   WJ_IUM_WCPU1_IUM_WCPU1_5_Msk

#define WJ_IUM_WCPU1_IUM_WCPU1_6_Pos               (6U)
#define WJ_IUM_WCPU1_IUM_WCPU1_6_Msk               (0x1U << WJ_IUM_WCPU1_IUM_WCPU1_6_Pos)
#define WJ_IUM_WCPU1_IUM_WCPU1_6                   WJ_IUM_WCPU1_IUM_WCPU1_6_Msk

#define WJ_IUM_WCPU1_IUM_WCPU1_7_Pos               (7U)
#define WJ_IUM_WCPU1_IUM_WCPU1_7_Msk               (0x7U << WJ_IUM_WCPU1_IUM_WCPU1_7_Pos)
#define WJ_IUM_WCPU1_IUM_WCPU1_7                   WJ_IUM_WCPU1_IUM_WCPU1_7_Msk

#define WJ_IUM_WCPU1_IUM_WCPU1_8_Pos               (8U)
#define WJ_IUM_WCPU1_IUM_WCPU1_8_Msk               (0x1U << WJ_IUM_WCPU1_IUM_WCPU1_8_Pos)
#define WJ_IUM_WCPU1_IUM_WCPU1_8                   WJ_IUM_WCPU1_IUM_WCPU1_8_Msk

#define WJ_IUM_WCPU1_IUM_WCPU1_9_Pos               (9U)
#define WJ_IUM_WCPU1_IUM_WCPU1_9_Msk               (0x1U << WJ_IUM_WCPU1_IUM_WCPU1_9_Pos)
#define WJ_IUM_WCPU1_IUM_WCPU1_9                   WJ_IUM_WCPU1_IUM_WCPU1_9_Msk

#define WJ_IUM_WCPU1_IUM_WCPU1_10_Pos              (10U)
#define WJ_IUM_WCPU1_IUM_WCPU1_10_Msk              (0x1U << WJ_IUM_WCPU1_IUM_WCPU1_10_Pos)
#define WJ_IUM_WCPU1_IUM_WCPU1_10                  WJ_IUM_WCPU1_IUM_WCPU1_10_Msk

#define WJ_IUM_WCPU1_IUM_WCPU1_11_Pos              (11U)
#define WJ_IUM_WCPU1_IUM_WCPU1_11_Msk              (0x1U << WJ_IUM_WCPU1_IUM_WCPU1_11_Pos)
#define WJ_IUM_WCPU1_IUM_WCPU1_11                  WJ_IUM_WCPU1_IUM_WCPU1_11_Msk

/* IS_WCPU1, offset: 0x108 */
#define WJ_IS_WCPU1_IS_WCPU1_0_Pos                 (0U)
#define WJ_IS_WCPU1_IS_WCPU1_0_Msk                 (0x1U << WJ_IS_WCPU1_IS_WCPU1_0_Pos)
#define WJ_IS_WCPU1_IS_WCPU1_0                     WJ_IS_WCPU1_IS_WCPU1_0_Msk

#define WJ_IS_WCPU1_IS_WCPU1_1_Pos                 (1U)
#define WJ_IS_WCPU1_IS_WCPU1_1_Msk                 (0x1U << WJ_IS_WCPU1_IS_WCPU1_1_Pos)
#define WJ_IS_WCPU1_IS_WCPU1_1                     WJ_IS_WCPU1_IS_WCPU1_1_Msk

#define WJ_IS_WCPU1_IS_WCPU1_2_Pos                 (2U)
#define WJ_IS_WCPU1_IS_WCPU1_2_Msk                 (0x1U << WJ_IS_WCPU1_IS_WCPU1_2_Pos)
#define WJ_IS_WCPU1_IS_WCPU1_2                     WJ_IS_WCPU1_IS_WCPU1_2_Msk

#define WJ_IS_WCPU1_IS_WCPU1_3_Pos                 (3U)
#define WJ_IS_WCPU1_IS_WCPU1_3_Msk                 (0x1U << WJ_IS_WCPU1_IS_WCPU1_3_Pos)
#define WJ_IS_WCPU1_IS_WCPU1_3                     WJ_IS_WCPU1_IS_WCPU1_3_Msk

#define WJ_IS_WCPU1_IS_WCPU1_4_Pos                 (4U)
#define WJ_IS_WCPU1_IS_WCPU1_4_Msk                 (0x1U << WJ_IS_WCPU1_IS_WCPU1_4_Pos)
#define WJ_IS_WCPU1_IS_WCPU1_4                     WJ_IS_WCPU1_IS_WCPU1_4_Msk

#define WJ_IS_WCPU1_IS_WCPU1_5_Pos                 (5U)
#define WJ_IS_WCPU1_IS_WCPU1_5_Msk                 (0x1U << WJ_IS_WCPU1_IS_WCPU1_5_Pos)
#define WJ_IS_WCPU1_IS_WCPU1_5                     WJ_IS_WCPU1_IS_WCPU1_5_Msk

#define WJ_IS_WCPU1_IS_WCPU1_6_Pos                 (6U)
#define WJ_IS_WCPU1_IS_WCPU1_6_Msk                 (0x1U << WJ_IS_WCPU1_IS_WCPU1_6_Pos)
#define WJ_IS_WCPU1_IS_WCPU1_6                     WJ_IS_WCPU1_IS_WCPU1_6_Msk

#define WJ_IS_WCPU1_IS_WCPU1_7_Pos                 (7U)
#define WJ_IS_WCPU1_IS_WCPU1_7_Msk                 (0x7U << WJ_IS_WCPU1_IS_WCPU1_7_Pos)
#define WJ_IS_WCPU1_IS_WCPU1_7                     WJ_IS_WCPU1_IS_WCPU1_7_Msk

#define WJ_IS_WCPU1_IS_WCPU1_8_Pos                 (8U)
#define WJ_IS_WCPU1_IS_WCPU1_8_Msk                 (0x1U << WJ_IS_WCPU1_IS_WCPU1_8_Pos)
#define WJ_IS_WCPU1_IS_WCPU1_8                     WJ_IS_WCPU1_IS_WCPU1_8_Msk

#define WJ_IS_WCPU1_IS_WCPU1_9_Pos                 (9U)
#define WJ_IS_WCPU1_IS_WCPU1_9_Msk                 (0x1U << WJ_IS_WCPU1_IS_WCPU1_9_Pos)
#define WJ_IS_WCPU1_IS_WCPU1_9                     WJ_IS_WCPU1_IS_WCPU1_9_Msk

#define WJ_IS_WCPU1_IS_WCPU1_10_Pos                (10U)
#define WJ_IS_WCPU1_IS_WCPU1_10_Msk                (0x1U << WJ_IS_WCPU1_IS_WCPU1_10_Pos)
#define WJ_IS_WCPU1_IS_WCPU1_10                    WJ_IS_WCPU1_IS_WCPU1_10_Msk

#define WJ_IS_WCPU1_IS_WCPU1_11_Pos                (11U)
#define WJ_IS_WCPU1_IS_WCPU1_11_Msk                (0x1U << WJ_IS_WCPU1_IS_WCPU1_11_Pos)
#define WJ_IS_WCPU1_IS_WCPU1_11                    WJ_IS_WCPU1_IS_WCPU1_11_Msk

/* IC_WCPU1 , offset: 0x10C */
#define WJ_IC_WCPU1_IC_WCPU1_0_Pos                 (0U)
#define WJ_IC_WCPU1_IC_WCPU1_0_Msk                 (0x1U << WJ_IC_WCPU1_IC_WCPU1_0_Pos)
#define WJ_IC_WCPU1_IC_WCPU1_0                     WJ_IC_WCPU1_IC_WCPU1_0_Msk

#define WJ_IC_WCPU1_IC_WCPU1_1_Pos                 (1U)
#define WJ_IC_WCPU1_IC_WCPU1_1_Msk                 (0x1U << WJ_IC_WCPU1_IC_WCPU1_1_Pos)
#define WJ_IC_WCPU1_IC_WCPU1_1                     WJ_IC_WCPU1_IC_WCPU1_1_Msk

#define WJ_IC_WCPU1_IC_WCPU1_2_Pos                 (2U)
#define WJ_IC_WCPU1_IC_WCPU1_2_Msk                 (0x1U << WJ_IC_WCPU1_IC_WCPU1_2_Pos)
#define WJ_IC_WCPU1_IC_WCPU1_2                     WJ_IC_WCPU1_IC_WCPU1_2_Msk

#define WJ_IC_WCPU1_IC_WCPU1_3_Pos                 (3U)
#define WJ_IC_WCPU1_IC_WCPU1_3_Msk                 (0x1U << WJ_IC_WCPU1_IC_WCPU1_3_Pos)
#define WJ_IC_WCPU1_IC_WCPU1_3                     WJ_IC_WCPU1_IC_WCPU1_3_Msk

#define WJ_IC_WCPU1_IC_WCPU1_4_Pos                 (4U)
#define WJ_IC_WCPU1_IC_WCPU1_4_Msk                 (0x1U << WJ_IC_WCPU1_IC_WCPU1_4_Pos)
#define WJ_IC_WCPU1_IC_WCPU1_4                     WJ_IC_WCPU1_IC_WCPU1_4_Msk

#define WJ_IC_WCPU1_IC_WCPU1_5_Pos                 (5U)
#define WJ_IC_WCPU1_IC_WCPU1_5_Msk                 (0x1U << WJ_IC_WCPU1_IC_WCPU1_5_Pos)
#define WJ_IC_WCPU1_IC_WCPU1_5                     WJ_IC_WCPU1_IC_WCPU1_5_Msk

#define WJ_IC_WCPU1_IC_WCPU1_6_Pos                 (6U)
#define WJ_IC_WCPU1_IC_WCPU1_6_Msk                 (0x1U << WJ_IC_WCPU1_IC_WCPU1_6_Pos)
#define WJ_IC_WCPU1_IC_WCPU1_6                     WJ_IC_WCPU1_IC_WCPU1_6_Msk

#define WJ_IC_WCPU1_IC_WCPU1_7_Pos                 (7U)
#define WJ_IC_WCPU1_IC_WCPU1_7_Msk                 (0x7U << WJ_IC_WCPU1_IC_WCPU1_7_Pos)
#define WJ_IC_WCPU1_IC_WCPU1_7                     WJ_IC_WCPU1_IC_WCPU1_7_Msk

#define WJ_IC_WCPU1_IC_WCPU1_8_Pos                 (8U)
#define WJ_IC_WCPU1_IC_WCPU1_8_Msk                 (0x1U << WJ_IC_WCPU1_IC_WCPU1_8_Pos)
#define WJ_IC_WCPU1_IC_WCPU1_8                     WJ_IC_WCPU1_IC_WCPU1_8_Msk

#define WJ_IC_WCPU1_IC_WCPU1_9_Pos                 (9U)
#define WJ_IC_WCPU1_IC_WCPU1_9_Msk                 (0x1U << WJ_IC_WCPU1_IC_WCPU1_9_Pos)
#define WJ_IC_WCPU1_IC_WCPU1_9                     WJ_IC_WCPU1_IC_WCPU1_9_Msk

#define WJ_IC_WCPU1_IC_WCPU1_10_Pos                (10U)
#define WJ_IC_WCPU1_IC_WCPU1_10_Msk                (0x1U << WJ_IC_WCPU1_IC_WCPU1_10_Pos)
#define WJ_IC_WCPU1_IC_WCPU1_10                    WJ_IC_WCPU1_IC_WCPU1_10_Msk

#define WJ_IC_WCPU1_IC_WCPU1_11_Pos                (11U)
#define WJ_IC_WCPU1_IC_WCPU1_11_Msk                (0x1U << WJ_IC_WCPU1_IC_WCPU1_11_Pos)
#define WJ_IC_WCPU1_IC_WCPU1_11                    WJ_IC_WCPU1_IC_WCPU1_11_Msk

/* RIS_RCPU1, offset: 0x110 */
#define WJ_RIS_RCPU1_RIS_RCPU0_0_Pos               (0U)
#define WJ_RIS_RCPU1_RIS_RCPU0_0_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU0_0_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU0_0                   WJ_RIS_RCPU1_RIS_RCPU0_0_Msk

#define WJ_RIS_RCPU1_RIS_RCPU0_1_Pos               (1U)
#define WJ_RIS_RCPU1_RIS_RCPU0_1_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU0_1_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU0_1                   WJ_RIS_RCPU1_RIS_RCPU0_1_Msk

#define WJ_RIS_RCPU1_RIS_RCPU0_2_Pos               (2U)
#define WJ_RIS_RCPU1_RIS_RCPU0_2_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU0_2_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU0_2                   WJ_RIS_RCPU1_RIS_RCPU0_2_Msk

#define WJ_RIS_RCPU1_RIS_RCPU0_3_Pos               (3U)
#define WJ_RIS_RCPU1_RIS_RCPU0_3_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU0_3_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU0_3                   WJ_RIS_RCPU1_RIS_RCPU0_3_Msk

#define WJ_RIS_RCPU1_RIS_RCPU0_4_Pos               (4U)
#define WJ_RIS_RCPU1_RIS_RCPU0_4_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU0_4_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU0_4                   WJ_RIS_RCPU1_RIS_RCPU0_4_Msk

#define WJ_RIS_RCPU1_RIS_RCPU0_5_Pos               (5U)
#define WJ_RIS_RCPU1_RIS_RCPU0_5_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU0_5_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU0_5                   WJ_RIS_RCPU1_RIS_RCPU0_5_Msk

#define WJ_RIS_RCPU1_RIS_RCPU0_6_Pos               (6U)
#define WJ_RIS_RCPU1_RIS_RCPU0_6_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU0_6_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU0_6                   WJ_RIS_RCPU1_RIS_RCPU0_6_Msk

#define WJ_RIS_RCPU1_RIS_RCPU0_7_Pos               (7U)
#define WJ_RIS_RCPU1_RIS_RCPU0_7_Msk               (0x7U << WJ_RIS_RCPU1_RIS_RCPU0_7_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU0_7                   WJ_RIS_RCPU1_RIS_RCPU0_7_Msk

#define WJ_RIS_RCPU1_RIS_RCPU0_8_Pos               (8U)
#define WJ_RIS_RCPU1_RIS_RCPU0_8_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU0_8_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU0_8                   WJ_RIS_RCPU1_RIS_RCPU0_8_Msk

#define WJ_RIS_RCPU1_RIS_RCPU0_9_Pos               (9U)
#define WJ_RIS_RCPU1_RIS_RCPU0_9_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU0_9_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU0_9                   WJ_RIS_RCPU1_RIS_RCPU0_9_Msk

#define WJ_RIS_RCPU1_RIS_RCPU0_10_Pos              (10U)
#define WJ_RIS_RCPU1_RIS_RCPU0_10_Msk              (0x1U << WJ_RIS_RCPU1_RIS_RCPU0_10_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU0_10                  WJ_RIS_RCPU1_RIS_RCPU0_10_Msk

#define WJ_RIS_RCPU1_RIS_RCPU0_11_Pos              (11U)
#define WJ_RIS_RCPU1_RIS_RCPU0_11_Msk              (0x1U << WJ_RIS_RCPU1_RIS_RCPU0_11_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU0_11                  WJ_RIS_RCPU1_RIS_RCPU0_11_Msk

#define WJ_RIS_RCPU1_RIS_RCPU2_0_Pos               (16U)
#define WJ_RIS_RCPU1_RIS_RCPU2_0_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU2_0_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU2_0                   WJ_RIS_RCPU1_RIS_RCPU2_0_Msk

#define WJ_RIS_RCPU1_RIS_RCPU2_1_Pos               (17U)
#define WJ_RIS_RCPU1_RIS_RCPU2_1_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU2_1_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU2_1                   WJ_RIS_RCPU1_RIS_RCPU2_1_Msk

#define WJ_RIS_RCPU1_RIS_RCPU2_2_Pos               (18U)
#define WJ_RIS_RCPU1_RIS_RCPU2_2_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU2_2_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU2_2                   WJ_RIS_RCPU1_RIS_RCPU2_2_Msk

#define WJ_RIS_RCPU1_RIS_RCPU2_3_Pos               (19U)
#define WJ_RIS_RCPU1_RIS_RCPU2_3_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU2_3_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU2_3                   WJ_RIS_RCPU1_RIS_RCPU2_3_Msk

#define WJ_RIS_RCPU1_RIS_RCPU2_4_Pos               (20U)
#define WJ_RIS_RCPU1_RIS_RCPU2_4_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU2_4_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU2_4                   WJ_RIS_RCPU1_RIS_RCPU2_4_Msk

#define WJ_RIS_RCPU1_RIS_RCPU2_5_Pos               (21U)
#define WJ_RIS_RCPU1_RIS_RCPU2_5_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU2_5_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU2_5                   WJ_RIS_RCPU1_RIS_RCPU2_5_Msk

#define WJ_RIS_RCPU1_RIS_RCPU2_6_Pos               (22U)
#define WJ_RIS_RCPU1_RIS_RCPU2_6_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU2_6_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU2_6                   WJ_RIS_RCPU1_RIS_RCPU2_6_Msk

#define WJ_RIS_RCPU1_RIS_RCPU2_7_Pos               (23U)
#define WJ_RIS_RCPU1_RIS_RCPU2_7_Msk               (0x7U << WJ_RIS_RCPU1_RIS_RCPU2_7_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU2_7                   WJ_RIS_RCPU1_RIS_RCPU2_7_Msk

#define WJ_RIS_RCPU1_RIS_RCPU2_8_Pos               (24U)
#define WJ_RIS_RCPU1_RIS_RCPU2_8_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU2_8_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU2_8                   WJ_RIS_RCPU1_RIS_RCPU2_8_Msk

#define WJ_RIS_RCPU1_RIS_RCPU2_9_Pos               (25U)
#define WJ_RIS_RCPU1_RIS_RCPU2_9_Msk               (0x1U << WJ_RIS_RCPU1_RIS_RCPU2_9_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU2_9                   WJ_RIS_RCPU1_RIS_RCPU2_9_Msk

#define WJ_RIS_RCPU1_RIS_RCPU2_10_Pos              (26U)
#define WJ_RIS_RCPU1_RIS_RCPU2_10_Msk              (0x1U << WJ_RIS_RCPU1_RIS_RCPU2_10_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU2_10                  WJ_RIS_RCPU1_RIS_RCPU2_10_Msk

#define WJ_RIS_RCPU1_RIS_RCPU2_11_Pos              (27U)
#define WJ_RIS_RCPU1_RIS_RCPU2_11_Msk              (0x1U << WJ_RIS_RCPU1_RIS_RCPU2_11_Pos)
#define WJ_RIS_RCPU1_RIS_RCPU2_11                  WJ_RIS_RCPU1_RIS_RCPU2_11_Msk


/* IUM_RCPU1, offset: 0x114 */
#define WJ_IUM_RCPU1_IUM_RCPU0_0_Pos               (0U)
#define WJ_IUM_RCPU1_IUM_RCPU0_0_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU0_0_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU0_0                   WJ_IUM_RCPU1_IUM_RCPU0_0_Msk

#define WJ_IUM_RCPU1_IUM_RCPU0_1_Pos               (1U)
#define WJ_IUM_RCPU1_IUM_RCPU0_1_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU0_1_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU0_1                   WJ_IUM_RCPU1_IUM_RCPU0_1_Msk

#define WJ_IUM_RCPU1_IUM_RCPU0_2_Pos               (2U)
#define WJ_IUM_RCPU1_IUM_RCPU0_2_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU0_2_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU0_2                   WJ_IUM_RCPU1_IUM_RCPU0_2_Msk

#define WJ_IUM_RCPU1_IUM_RCPU0_3_Pos               (3U)
#define WJ_IUM_RCPU1_IUM_RCPU0_3_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU0_3_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU0_3                   WJ_IUM_RCPU1_IUM_RCPU0_3_Msk

#define WJ_IUM_RCPU1_IUM_RCPU0_4_Pos               (4U)
#define WJ_IUM_RCPU1_IUM_RCPU0_4_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU0_4_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU0_4                   WJ_IUM_RCPU1_IUM_RCPU0_4_Msk

#define WJ_IUM_RCPU1_IUM_RCPU0_5_Pos               (5U)
#define WJ_IUM_RCPU1_IUM_RCPU0_5_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU0_5_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU0_5                   WJ_IUM_RCPU1_IUM_RCPU0_5_Msk

#define WJ_IUM_RCPU1_IUM_RCPU0_6_Pos               (6U)
#define WJ_IUM_RCPU1_IUM_RCPU0_6_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU0_6_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU0_6                   WJ_IUM_RCPU1_IUM_RCPU0_6_Msk

#define WJ_IUM_RCPU1_IUM_RCPU0_7_Pos               (7U)
#define WJ_IUM_RCPU1_IUM_RCPU0_7_Msk               (0x7U << WJ_IUM_RCPU1_IUM_RCPU0_7_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU0_7                   WJ_IUM_RCPU1_IUM_RCPU0_7_Msk

#define WJ_IUM_RCPU1_IUM_RCPU0_8_Pos               (8U)
#define WJ_IUM_RCPU1_IUM_RCPU0_8_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU0_8_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU0_8                   WJ_IUM_RCPU1_IUM_RCPU0_8_Msk

#define WJ_IUM_RCPU1_IUM_RCPU0_9_Pos               (9U)
#define WJ_IUM_RCPU1_IUM_RCPU0_9_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU0_9_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU0_9                   WJ_IUM_RCPU1_IUM_RCPU0_9_Msk

#define WJ_IUM_RCPU1_IUM_RCPU0_10_Pos              (10U)
#define WJ_IUM_RCPU1_IUM_RCPU0_10_Msk              (0x1U << WJ_IUM_RCPU1_IUM_RCPU0_10_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU0_10                  WJ_IUM_RCPU1_IUM_RCPU0_10_Msk

#define WJ_IUM_RCPU1_IUM_RCPU0_11_Pos              (11U)
#define WJ_IUM_RCPU1_IUM_RCPU0_11_Msk              (0x1U << WJ_IUM_RCPU1_IUM_RCPU0_11_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU0_11                  WJ_IUM_RCPU1_IUM_RCPU0_11_Msk

#define WJ_IUM_RCPU1_IUM_RCPU2_0_Pos               (16U)
#define WJ_IUM_RCPU1_IUM_RCPU2_0_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU2_0_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU2_0                   WJ_IUM_RCPU1_IUM_RCPU2_0_Msk

#define WJ_IUM_RCPU1_IUM_RCPU2_1_Pos               (17U)
#define WJ_IUM_RCPU1_IUM_RCPU2_1_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU2_1_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU2_1                   WJ_IUM_RCPU1_IUM_RCPU2_1_Msk

#define WJ_IUM_RCPU1_IUM_RCPU2_2_Pos               (18U)
#define WJ_IUM_RCPU1_IUM_RCPU2_2_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU2_2_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU2_2                   WJ_IUM_RCPU1_IUM_RCPU2_2_Msk

#define WJ_IUM_RCPU1_IUM_RCPU2_3_Pos               (19U)
#define WJ_IUM_RCPU1_IUM_RCPU2_3_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU2_3_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU2_3                   WJ_IUM_RCPU1_IUM_RCPU2_3_Msk

#define WJ_IUM_RCPU1_IUM_RCPU2_4_Pos               (20U)
#define WJ_IUM_RCPU1_IUM_RCPU2_4_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU2_4_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU2_4                   WJ_IUM_RCPU1_IUM_RCPU2_4_Msk

#define WJ_IUM_RCPU1_IUM_RCPU2_5_Pos               (21U)
#define WJ_IUM_RCPU1_IUM_RCPU2_5_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU2_5_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU2_5                   WJ_IUM_RCPU1_IUM_RCPU2_5_Msk

#define WJ_IUM_RCPU1_IUM_RCPU2_6_Pos               (22U)
#define WJ_IUM_RCPU1_IUM_RCPU2_6_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU2_6_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU2_6                   WJ_IUM_RCPU1_IUM_RCPU2_6_Msk

#define WJ_IUM_RCPU1_IUM_RCPU2_7_Pos               (23U)
#define WJ_IUM_RCPU1_IUM_RCPU2_7_Msk               (0x7U << WJ_IUM_RCPU1_IUM_RCPU2_7_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU2_7                   WJ_IUM_RCPU1_IUM_RCPU2_7_Msk

#define WJ_IUM_RCPU1_IUM_RCPU2_8_Pos               (24U)
#define WJ_IUM_RCPU1_IUM_RCPU2_8_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU2_8_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU2_8                   WJ_IUM_RCPU1_IUM_RCPU2_8_Msk

#define WJ_IUM_RCPU1_IUM_RCPU2_9_Pos               (25U)
#define WJ_IUM_RCPU1_IUM_RCPU2_9_Msk               (0x1U << WJ_IUM_RCPU1_IUM_RCPU2_9_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU2_9                   WJ_IUM_RCPU1_IUM_RCPU2_9_Msk

#define WJ_IUM_RCPU1_IUM_RCPU2_10_Pos              (26U)
#define WJ_IUM_RCPU1_IUM_RCPU2_10_Msk              (0x1U << WJ_IUM_RCPU1_IUM_RCPU2_10_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU2_10                  WJ_IUM_RCPU1_IUM_RCPU2_10_Msk

#define WJ_IUM_RCPU1_IUM_RCPU2_11_Pos              (27U)
#define WJ_IUM_RCPU1_IUM_RCPU2_11_Msk              (0x1U << WJ_IUM_RCPU1_IUM_RCPU2_11_Pos)
#define WJ_IUM_RCPU1_IUM_RCPU2_11                  WJ_IUM_RCPU1_IUM_RCPU2_11_Msk

/* IS_RCPU1, offset: 0x118 */
#define WJ_IS_RCPU1_IS_RCPU0_0_Pos                 (0U)
#define WJ_IS_RCPU1_IS_RCPU0_0_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU0_0_Pos)
#define WJ_IS_RCPU1_IS_RCPU0_0                     WJ_IS_RCPU1_IS_RCPU0_0_Msk

#define WJ_IS_RCPU1_IS_RCPU0_1_Pos                 (1U)
#define WJ_IS_RCPU1_IS_RCPU0_1_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU0_1_Pos)
#define WJ_IS_RCPU1_IS_RCPU0_1                     WJ_IS_RCPU1_IS_RCPU0_1_Msk

#define WJ_IS_RCPU1_IS_RCPU0_2_Pos                 (2U)
#define WJ_IS_RCPU1_IS_RCPU0_2_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU0_2_Pos)
#define WJ_IS_RCPU1_IS_RCPU0_2                     WJ_IS_RCPU1_IS_RCPU0_2_Msk

#define WJ_IS_RCPU1_IS_RCPU0_3_Pos                 (3U)
#define WJ_IS_RCPU1_IS_RCPU0_3_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU0_3_Pos)
#define WJ_IS_RCPU1_IS_RCPU0_3                     WJ_IS_RCPU1_IS_RCPU0_3_Msk

#define WJ_IS_RCPU1_IS_RCPU0_4_Pos                 (4U)
#define WJ_IS_RCPU1_IS_RCPU0_4_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU0_4_Pos)
#define WJ_IS_RCPU1_IS_RCPU0_4                     WJ_IS_RCPU1_IS_RCPU0_4_Msk

#define WJ_IS_RCPU1_IS_RCPU0_5_Pos                 (5U)
#define WJ_IS_RCPU1_IS_RCPU0_5_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU0_5_Pos)
#define WJ_IS_RCPU1_IS_RCPU0_5                     WJ_IS_RCPU1_IS_RCPU0_5_Msk

#define WJ_IS_RCPU1_IS_RCPU0_6_Pos                 (6U)
#define WJ_IS_RCPU1_IS_RCPU0_6_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU0_6_Pos)
#define WJ_IS_RCPU1_IS_RCPU0_6                     WJ_IS_RCPU1_IS_RCPU0_6_Msk

#define WJ_IS_RCPU1_IS_RCPU0_7_Pos                 (7U)
#define WJ_IS_RCPU1_IS_RCPU0_7_Msk                 (0x7U << WJ_IS_RCPU1_IS_RCPU0_7_Pos)
#define WJ_IS_RCPU1_IS_RCPU0_7                     WJ_IS_RCPU1_IS_RCPU0_7_Msk

#define WJ_IS_RCPU1_IS_RCPU0_8_Pos                 (8U)
#define WJ_IS_RCPU1_IS_RCPU0_8_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU0_8_Pos)
#define WJ_IS_RCPU1_IS_RCPU0_8                     WJ_IS_RCPU1_IS_RCPU0_8_Msk

#define WJ_IS_RCPU1_IS_RCPU0_9_Pos                 (9U)
#define WJ_IS_RCPU1_IS_RCPU0_9_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU0_9_Pos)
#define WJ_IS_RCPU1_IS_RCPU0_9                     WJ_IS_RCPU1_IS_RCPU0_9_Msk

#define WJ_IS_RCPU1_IS_RCPU0_10_Pos                (10U)
#define WJ_IS_RCPU1_IS_RCPU0_10_Msk                (0x1U << WJ_IS_RCPU1_IS_RCPU0_10_Pos)
#define WJ_IS_RCPU1_IS_RCPU0_10                    WJ_IS_RCPU1_IS_RCPU0_10_Msk

#define WJ_IS_RCPU1_IS_RCPU0_11_Pos                (11U)
#define WJ_IS_RCPU1_IS_RCPU0_11_Msk                (0x1U << WJ_IS_RCPU1_IS_RCPU0_11_Pos)
#define WJ_IS_RCPU1_IS_RCPU0_11                    WJ_IS_RCPU1_IS_RCPU0_11_Msk

#define WJ_IS_RCPU1_IS_RCPU2_0_Pos                 (16U)
#define WJ_IS_RCPU1_IS_RCPU2_0_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU2_0_Pos)
#define WJ_IS_RCPU1_IS_RCPU2_0                     WJ_IS_RCPU1_IS_RCPU2_0_Msk

#define WJ_IS_RCPU1_IS_RCPU2_1_Pos                 (17U)
#define WJ_IS_RCPU1_IS_RCPU2_1_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU2_1_Pos)
#define WJ_IS_RCPU1_IS_RCPU2_1                     WJ_IS_RCPU1_IS_RCPU2_1_Msk

#define WJ_IS_RCPU1_IS_RCPU2_2_Pos                 (18U)
#define WJ_IS_RCPU1_IS_RCPU2_2_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU2_2_Pos)
#define WJ_IS_RCPU1_IS_RCPU2_2                     WJ_IS_RCPU1_IS_RCPU2_2_Msk

#define WJ_IS_RCPU1_IS_RCPU2_3_Pos                 (19U)
#define WJ_IS_RCPU1_IS_RCPU2_3_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU2_3_Pos)
#define WJ_IS_RCPU1_IS_RCPU2_3                     WJ_IS_RCPU1_IS_RCPU2_3_Msk

#define WJ_IS_RCPU1_IS_RCPU2_4_Pos                 (20U)
#define WJ_IS_RCPU1_IS_RCPU2_4_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU2_4_Pos)
#define WJ_IS_RCPU1_IS_RCPU2_4                     WJ_IS_RCPU1_IS_RCPU2_4_Msk

#define WJ_IS_RCPU1_IS_RCPU2_5_Pos                 (21U)
#define WJ_IS_RCPU1_IS_RCPU2_5_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU2_5_Pos)
#define WJ_IS_RCPU1_IS_RCPU2_5                     WJ_IS_RCPU1_IS_RCPU2_5_Msk

#define WJ_IS_RCPU1_IS_RCPU2_6_Pos                 (22U)
#define WJ_IS_RCPU1_IS_RCPU2_6_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU2_6_Pos)
#define WJ_IS_RCPU1_IS_RCPU2_6                     WJ_IS_RCPU1_IS_RCPU2_6_Msk

#define WJ_IS_RCPU1_IS_RCPU2_7_Pos                 (23U)
#define WJ_IS_RCPU1_IS_RCPU2_7_Msk                 (0x7U << WJ_IS_RCPU1_IS_RCPU2_7_Pos)
#define WJ_IS_RCPU1_IS_RCPU2_7                     WJ_IS_RCPU1_IS_RCPU2_7_Msk

#define WJ_IS_RCPU1_IS_RCPU2_8_Pos                 (24U)
#define WJ_IS_RCPU1_IS_RCPU2_8_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU2_8_Pos)
#define WJ_IS_RCPU1_IS_RCPU2_8                     WJ_IS_RCPU1_IS_RCPU2_8_Msk

#define WJ_IS_RCPU1_IS_RCPU2_9_Pos                 (25U)
#define WJ_IS_RCPU1_IS_RCPU2_9_Msk                 (0x1U << WJ_IS_RCPU1_IS_RCPU2_9_Pos)
#define WJ_IS_RCPU1_IS_RCPU2_9                     WJ_IS_RCPU1_IS_RCPU2_9_Msk

#define WJ_IS_RCPU1_IS_RCPU2_10_Pos                (26U)
#define WJ_IS_RCPU1_IS_RCPU2_10_Msk                (0x1U << WJ_IS_RCPU1_IS_RCPU2_10_Pos)
#define WJ_IS_RCPU1_IS_RCPU2_10                    WJ_IS_RCPU1_IS_RCPU2_10_Msk

#define WJ_IS_RCPU1_IS_RCPU2_11_Pos                (27U)
#define WJ_IS_RCPU1_IS_RCPU2_11_Msk                (0x1U << WJ_IS_RCPU1_IS_RCPU2_11_Pos)
#define WJ_IS_RCPU1_IS_RCPU2_11                    WJ_IS_RCPU1_IS_RCPU2_11_Msk

/* IC_RCPU1, offset: 0x11C */
#define WJ_IC_RCPU1_IC_RCPU0_0_Pos                 (0U)
#define WJ_IC_RCPU1_IC_RCPU0_0_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU0_0_Pos)
#define WJ_IC_RCPU1_IC_RCPU0_0                     WJ_IC_RCPU1_IC_RCPU0_0_Msk

#define WJ_IC_RCPU1_IC_RCPU0_1_Pos                 (1U)
#define WJ_IC_RCPU1_IC_RCPU0_1_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU0_1_Pos)
#define WJ_IC_RCPU1_IC_RCPU0_1                     WJ_IC_RCPU1_IC_RCPU0_1_Msk

#define WJ_IC_RCPU1_IC_RCPU0_2_Pos                 (2U)
#define WJ_IC_RCPU1_IC_RCPU0_2_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU0_2_Pos)
#define WJ_IC_RCPU1_IC_RCPU0_2                     WJ_IC_RCPU1_IC_RCPU0_2_Msk

#define WJ_IC_RCPU1_IC_RCPU0_3_Pos                 (3U)
#define WJ_IC_RCPU1_IC_RCPU0_3_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU0_3_Pos)
#define WJ_IC_RCPU1_IC_RCPU0_3                     WJ_IC_RCPU1_IC_RCPU0_3_Msk

#define WJ_IC_RCPU1_IC_RCPU0_4_Pos                 (4U)
#define WJ_IC_RCPU1_IC_RCPU0_4_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU0_4_Pos)
#define WJ_IC_RCPU1_IC_RCPU0_4                     WJ_IC_RCPU1_IC_RCPU0_4_Msk

#define WJ_IC_RCPU1_IC_RCPU0_5_Pos                 (5U)
#define WJ_IC_RCPU1_IC_RCPU0_5_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU0_5_Pos)
#define WJ_IC_RCPU1_IC_RCPU0_5                     WJ_IC_RCPU1_IC_RCPU0_5_Msk

#define WJ_IC_RCPU1_IC_RCPU0_6_Pos                 (6U)
#define WJ_IC_RCPU1_IC_RCPU0_6_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU0_6_Pos)
#define WJ_IC_RCPU1_IC_RCPU0_6                     WJ_IC_RCPU1_IC_RCPU0_6_Msk

#define WJ_IC_RCPU1_IC_RCPU0_7_Pos                 (7U)
#define WJ_IC_RCPU1_IC_RCPU0_7_Msk                 (0x7U << WJ_IC_RCPU1_IC_RCPU0_7_Pos)
#define WJ_IC_RCPU1_IC_RCPU0_7                     WJ_IC_RCPU1_IC_RCPU0_7_Msk

#define WJ_IC_RCPU1_IC_RCPU0_8_Pos                 (8U)
#define WJ_IC_RCPU1_IC_RCPU0_8_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU0_8_Pos)
#define WJ_IC_RCPU1_IC_RCPU0_8                     WJ_IC_RCPU1_IC_RCPU0_8_Msk

#define WJ_IC_RCPU1_IC_RCPU0_9_Pos                 (9U)
#define WJ_IC_RCPU1_IC_RCPU0_9_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU0_9_Pos)
#define WJ_IC_RCPU1_IC_RCPU0_9                     WJ_IC_RCPU1_IC_RCPU0_9_Msk

#define WJ_IC_RCPU1_IC_RCPU0_10_Pos                (10U)
#define WJ_IC_RCPU1_IC_RCPU0_10_Msk                (0x1U << WJ_IC_RCPU1_IC_RCPU0_10_Pos)
#define WJ_IC_RCPU1_IC_RCPU0_10                    WJ_IC_RCPU1_IC_RCPU0_10_Msk

#define WJ_IC_RCPU1_IC_RCPU0_11_Pos                (11U)
#define WJ_IC_RCPU1_IC_RCPU0_11_Msk                (0x1U << WJ_IC_RCPU1_IC_RCPU0_11_Pos)
#define WJ_IC_RCPU1_IC_RCPU0_11                    WJ_IC_RCPU1_IC_RCPU0_11_Msk

#define WJ_IC_RCPU1_IC_RCPU2_0_Pos                 (16U)
#define WJ_IC_RCPU1_IC_RCPU2_0_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU2_0_Pos)
#define WJ_IC_RCPU1_IC_RCPU2_0                     WJ_IC_RCPU1_IC_RCPU2_0_Msk

#define WJ_IC_RCPU1_IC_RCPU2_1_Pos                 (17U)
#define WJ_IC_RCPU1_IC_RCPU2_1_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU2_1_Pos)
#define WJ_IC_RCPU1_IC_RCPU2_1                     WJ_IC_RCPU1_IC_RCPU2_1_Msk

#define WJ_IC_RCPU1_IC_RCPU2_2_Pos                 (18U)
#define WJ_IC_RCPU1_IC_RCPU2_2_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU2_2_Pos)
#define WJ_IC_RCPU1_IC_RCPU2_2                     WJ_IC_RCPU1_IC_RCPU2_2_Msk

#define WJ_IC_RCPU1_IC_RCPU2_3_Pos                 (19U)
#define WJ_IC_RCPU1_IC_RCPU2_3_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU2_3_Pos)
#define WJ_IC_RCPU1_IC_RCPU2_3                     WJ_IC_RCPU1_IC_RCPU2_3_Msk

#define WJ_IC_RCPU1_IC_RCPU2_4_Pos                 (20U)
#define WJ_IC_RCPU1_IC_RCPU2_4_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU2_4_Pos)
#define WJ_IC_RCPU1_IC_RCPU2_4                     WJ_IC_RCPU1_IC_RCPU2_4_Msk

#define WJ_IC_RCPU1_IC_RCPU2_5_Pos                 (21U)
#define WJ_IC_RCPU1_IC_RCPU2_5_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU2_5_Pos)
#define WJ_IC_RCPU1_IC_RCPU2_5                     WJ_IC_RCPU1_IC_RCPU2_5_Msk

#define WJ_IC_RCPU1_IC_RCPU2_6_Pos                 (22U)
#define WJ_IC_RCPU1_IC_RCPU2_6_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU2_6_Pos)
#define WJ_IC_RCPU1_IC_RCPU2_6                     WJ_IC_RCPU1_IC_RCPU2_6_Msk

#define WJ_IC_RCPU1_IC_RCPU2_7_Pos                 (23U)
#define WJ_IC_RCPU1_IC_RCPU2_7_Msk                 (0x7U << WJ_IC_RCPU1_IC_RCPU2_7_Pos)
#define WJ_IC_RCPU1_IC_RCPU2_7                     WJ_IC_RCPU1_IC_RCPU2_7_Msk

#define WJ_IC_RCPU1_IC_RCPU2_8_Pos                 (24U)
#define WJ_IC_RCPU1_IC_RCPU2_8_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU2_8_Pos)
#define WJ_IC_RCPU1_IC_RCPU2_8                     WJ_IC_RCPU1_IC_RCPU2_8_Msk

#define WJ_IC_RCPU1_IC_RCPU2_9_Pos                 (25U)
#define WJ_IC_RCPU1_IC_RCPU2_9_Msk                 (0x1U << WJ_IC_RCPU1_IC_RCPU2_9_Pos)
#define WJ_IC_RCPU1_IC_RCPU2_9                     WJ_IC_RCPU1_IC_RCPU2_9_Msk

#define WJ_IC_RCPU1_IC_RCPU2_10_Pos                (26U)
#define WJ_IC_RCPU1_IC_RCPU2_10_Msk                (0x1U << WJ_IC_RCPU1_IC_RCPU2_10_Pos)
#define WJ_IC_RCPU1_IC_RCPU2_10                    WJ_IC_RCPU1_IC_RCPU2_10_Msk

#define WJ_IC_RCPU1_IC_RCPU2_11_Pos                (27U)
#define WJ_IC_RCPU1_IC_RCPU2_11_Msk                (0x1U << WJ_IC_RCPU1_IC_RCPU2_11_Pos)
#define WJ_IC_RCPU1_IC_RCPU2_11                    WJ_IC_RCPU1_IC_RCPU2_11_Msk

/* RIS_WCPU2 , offset: 0x200 */
#define WJ_RIS_WCPU2_RIS_WCPU2_0_Pos               (0U)
#define WJ_RIS_WCPU2_RIS_WCPU2_0_Msk               (0x1U << WJ_RIS_WCPU2_RIS_WCPU2_0_Pos)
#define WJ_RIS_WCPU2_RIS_WCPU2_0                   WJ_RIS_WCPU2_RIS_WCPU2_0_Msk

#define WJ_RIS_WCPU2_RIS_WCPU2_1_Pos               (1U)
#define WJ_RIS_WCPU2_RIS_WCPU2_1_Msk               (0x1U << WJ_RIS_WCPU2_RIS_WCPU2_1_Pos)
#define WJ_RIS_WCPU2_RIS_WCPU2_1                   WJ_RIS_WCPU2_RIS_WCPU2_1_Msk

#define WJ_RIS_WCPU2_RIS_WCPU2_2_Pos               (2U)
#define WJ_RIS_WCPU2_RIS_WCPU2_2_Msk               (0x1U << WJ_RIS_WCPU2_RIS_WCPU2_2_Pos)
#define WJ_RIS_WCPU2_RIS_WCPU2_2                   WJ_RIS_WCPU2_RIS_WCPU2_2_Msk

#define WJ_RIS_WCPU2_RIS_WCPU2_3_Pos               (3U)
#define WJ_RIS_WCPU2_RIS_WCPU2_3_Msk               (0x1U << WJ_RIS_WCPU2_RIS_WCPU2_3_Pos)
#define WJ_RIS_WCPU2_RIS_WCPU2_3                   WJ_RIS_WCPU2_RIS_WCPU2_3_Msk

#define WJ_RIS_WCPU2_RIS_WCPU2_4_Pos               (4U)
#define WJ_RIS_WCPU2_RIS_WCPU2_4_Msk               (0x1U << WJ_RIS_WCPU2_RIS_WCPU2_4_Pos)
#define WJ_RIS_WCPU2_RIS_WCPU2_4                   WJ_RIS_WCPU2_RIS_WCPU2_4_Msk

#define WJ_RIS_WCPU2_RIS_WCPU2_5_Pos               (5U)
#define WJ_RIS_WCPU2_RIS_WCPU2_5_Msk               (0x1U << WJ_RIS_WCPU2_RIS_WCPU2_5_Pos)
#define WJ_RIS_WCPU2_RIS_WCPU2_5                   WJ_RIS_WCPU2_RIS_WCPU2_5_Msk

#define WJ_RIS_WCPU2_RIS_WCPU2_6_Pos               (6U)
#define WJ_RIS_WCPU2_RIS_WCPU2_6_Msk               (0x1U << WJ_RIS_WCPU2_RIS_WCPU2_6_Pos)
#define WJ_RIS_WCPU2_RIS_WCPU2_6                   WJ_RIS_WCPU2_RIS_WCPU2_6_Msk

#define WJ_RIS_WCPU2_RIS_WCPU2_7_Pos               (7U)
#define WJ_RIS_WCPU2_RIS_WCPU2_7_Msk               (0x7U << WJ_RIS_WCPU2_RIS_WCPU2_7_Pos)
#define WJ_RIS_WCPU2_RIS_WCPU2_7                   WJ_RIS_WCPU2_RIS_WCPU2_7_Msk

#define WJ_RIS_WCPU2_RIS_WCPU2_8_Pos               (8U)
#define WJ_RIS_WCPU2_RIS_WCPU2_8_Msk               (0x1U << WJ_RIS_WCPU2_RIS_WCPU2_8_Pos)
#define WJ_RIS_WCPU2_RIS_WCPU2_8                   WJ_RIS_WCPU2_RIS_WCPU2_8_Msk

#define WJ_RIS_WCPU2_RIS_WCPU2_9_Pos               (9U)
#define WJ_RIS_WCPU2_RIS_WCPU2_9_Msk               (0x1U << WJ_RIS_WCPU2_RIS_WCPU2_9_Pos)
#define WJ_RIS_WCPU2_RIS_WCPU2_9                   WJ_RIS_WCPU2_RIS_WCPU2_9_Msk

#define WJ_RIS_WCPU2_RIS_WCPU2_10_Pos              (10U)
#define WJ_RIS_WCPU2_RIS_WCPU2_10_Msk              (0x1U << WJ_RIS_WCPU2_RIS_WCPU2_10_Pos)
#define WJ_RIS_WCPU2_RIS_WCPU2_10                  WJ_RIS_WCPU2_RIS_WCPU2_10_Msk

#define WJ_RIS_WCPU2_RIS_WCPU2_11_Pos              (11U)
#define WJ_RIS_WCPU2_RIS_WCPU2_11_Msk              (0x1U << WJ_RIS_WCPU2_RIS_WCPU2_11_Pos)
#define WJ_RIS_WCPU2_RIS_WCPU2_11                  WJ_RIS_WCPU2_RIS_WCPU2_11_Msk


/* IUM_WCPU2, offset: 0x204 */
#define WJ_IUM_WCPU2_IUM_WCPU2_0_Pos               (0U)
#define WJ_IUM_WCPU2_IUM_WCPU2_0_Msk               (0x1U << WJ_IUM_WCPU2_IUM_WCPU2_0_Pos)
#define WJ_IUM_WCPU2_IUM_WCPU2_0                   WJ_IUM_WCPU2_IUM_WCPU2_0_Msk

#define WJ_IUM_WCPU2_IUM_WCPU2_1_Pos               (1U)
#define WJ_IUM_WCPU2_IUM_WCPU2_1_Msk               (0x1U << WJ_IUM_WCPU2_IUM_WCPU2_1_Pos)
#define WJ_IUM_WCPU2_IUM_WCPU2_1                   WJ_IUM_WCPU2_IUM_WCPU2_1_Msk

#define WJ_IUM_WCPU2_IUM_WCPU2_2_Pos               (2U)
#define WJ_IUM_WCPU2_IUM_WCPU2_2_Msk               (0x1U << WJ_IUM_WCPU2_IUM_WCPU2_2_Pos)
#define WJ_IUM_WCPU2_IUM_WCPU2_2                   WJ_IUM_WCPU2_IUM_WCPU2_2_Msk

#define WJ_IUM_WCPU2_IUM_WCPU2_3_Pos               (3U)
#define WJ_IUM_WCPU2_IUM_WCPU2_3_Msk               (0x1U << WJ_IUM_WCPU2_IUM_WCPU2_3_Pos)
#define WJ_IUM_WCPU2_IUM_WCPU2_3                   WJ_IUM_WCPU2_IUM_WCPU2_3_Msk

#define WJ_IUM_WCPU2_IUM_WCPU2_4_Pos               (4U)
#define WJ_IUM_WCPU2_IUM_WCPU2_4_Msk               (0x1U << WJ_IUM_WCPU2_IUM_WCPU2_4_Pos)
#define WJ_IUM_WCPU2_IUM_WCPU2_4                   WJ_IUM_WCPU2_IUM_WCPU2_4_Msk

#define WJ_IUM_WCPU2_IUM_WCPU2_5_Pos               (5U)
#define WJ_IUM_WCPU2_IUM_WCPU2_5_Msk               (0x1U << WJ_IUM_WCPU2_IUM_WCPU2_5_Pos)
#define WJ_IUM_WCPU2_IUM_WCPU2_5                   WJ_IUM_WCPU2_IUM_WCPU2_5_Msk

#define WJ_IUM_WCPU2_IUM_WCPU2_6_Pos               (6U)
#define WJ_IUM_WCPU2_IUM_WCPU2_6_Msk               (0x1U << WJ_IUM_WCPU2_IUM_WCPU2_6_Pos)
#define WJ_IUM_WCPU2_IUM_WCPU2_6                   WJ_IUM_WCPU2_IUM_WCPU2_6_Msk

#define WJ_IUM_WCPU2_IUM_WCPU2_7_Pos               (7U)
#define WJ_IUM_WCPU2_IUM_WCPU2_7_Msk               (0x7U << WJ_IUM_WCPU2_IUM_WCPU2_7_Pos)
#define WJ_IUM_WCPU2_IUM_WCPU2_7                   WJ_IUM_WCPU2_IUM_WCPU2_7_Msk

#define WJ_IUM_WCPU2_IUM_WCPU2_8_Pos               (8U)
#define WJ_IUM_WCPU2_IUM_WCPU2_8_Msk               (0x1U << WJ_IUM_WCPU2_IUM_WCPU2_8_Pos)
#define WJ_IUM_WCPU2_IUM_WCPU2_8                   WJ_IUM_WCPU2_IUM_WCPU2_8_Msk

#define WJ_IUM_WCPU2_IUM_WCPU2_9_Pos               (9U)
#define WJ_IUM_WCPU2_IUM_WCPU2_9_Msk               (0x1U << WJ_IUM_WCPU2_IUM_WCPU2_9_Pos)
#define WJ_IUM_WCPU2_IUM_WCPU2_9                   WJ_IUM_WCPU2_IUM_WCPU2_9_Msk

#define WJ_IUM_WCPU2_IUM_WCPU2_10_Pos              (10U)
#define WJ_IUM_WCPU2_IUM_WCPU2_10_Msk              (0x1U << WJ_IUM_WCPU2_IUM_WCPU2_10_Pos)
#define WJ_IUM_WCPU2_IUM_WCPU2_10                  WJ_IUM_WCPU2_IUM_WCPU2_10_Msk

#define WJ_IUM_WCPU2_IUM_WCPU2_11_Pos              (11U)
#define WJ_IUM_WCPU2_IUM_WCPU2_11_Msk              (0x1U << WJ_IUM_WCPU2_IUM_WCPU2_11_Pos)
#define WJ_IUM_WCPU2_IUM_WCPU2_11                  WJ_IUM_WCPU2_IUM_WCPU2_11_Msk

/* IS_WCPU1, offset: 0x208 */
#define WJ_IS_WCPU2_IS_WCPU2_0_Pos                 (0U)
#define WJ_IS_WCPU2_IS_WCPU2_0_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_0_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_0                     WJ_IS_WCPU2_IS_WCPU2_0_Msk

#define WJ_IS_WCPU2_IS_WCPU2_1_Pos                 (1U)
#define WJ_IS_WCPU2_IS_WCPU2_1_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_1_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_1                     WJ_IS_WCPU2_IS_WCPU2_1_Msk

#define WJ_IS_WCPU2_IS_WCPU2_2_Pos                 (2U)
#define WJ_IS_WCPU2_IS_WCPU2_2_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_2_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_2                     WJ_IS_WCPU2_IS_WCPU2_2_Msk

#define WJ_IS_WCPU2_IS_WCPU2_3_Pos                 (3U)
#define WJ_IS_WCPU2_IS_WCPU2_3_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_3_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_3                     WJ_IS_WCPU2_IS_WCPU2_3_Msk

#define WJ_IS_WCPU2_IS_WCPU2_4_Pos                 (4U)
#define WJ_IS_WCPU2_IS_WCPU2_4_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_4_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_4                     WJ_IS_WCPU2_IS_WCPU2_4_Msk

#define WJ_IS_WCPU2_IS_WCPU2_5_Pos                 (5U)
#define WJ_IS_WCPU2_IS_WCPU2_5_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_5_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_5                     WJ_IS_WCPU2_IS_WCPU2_5_Msk

#define WJ_IS_WCPU2_IS_WCPU2_6_Pos                 (6U)
#define WJ_IS_WCPU2_IS_WCPU2_6_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_6_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_6                     WJ_IS_WCPU2_IS_WCPU2_6_Msk

#define WJ_IS_WCPU2_IS_WCPU2_7_Pos                 (7U)
#define WJ_IS_WCPU2_IS_WCPU2_7_Msk                 (0x7U << WJ_IS_WCPU2_IS_WCPU2_7_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_7                     WJ_IS_WCPU2_IS_WCPU2_7_Msk

#define WJ_IS_WCPU2_IS_WCPU2_8_Pos                 (8U)
#define WJ_IS_WCPU2_IS_WCPU2_8_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_8_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_8                     WJ_IS_WCPU2_IS_WCPU2_8_Msk

#define WJ_IS_WCPU2_IS_WCPU2_9_Pos                 (9U)
#define WJ_IS_WCPU2_IS_WCPU2_9_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_9_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_9                     WJ_IS_WCPU2_IS_WCPU2_9_Msk

#define WJ_IS_WCPU2_IS_WCPU2_10_Pos                (10U)
#define WJ_IS_WCPU2_IS_WCPU2_10_Msk                (0x1U << WJ_IS_WCPU2_IS_WCPU2_10_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_10                    WJ_IS_WCPU2_IS_WCPU2_10_Msk

#define WJ_IS_WCPU2_IS_WCPU2_11_Pos                (11U)
#define WJ_IS_WCPU2_IS_WCPU2_11_Msk                (0x1U << WJ_IS_WCPU2_IS_WCPU2_11_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_11                    WJ_IS_WCPU2_IS_WCPU2_11_Msk

/* IC_WCPU2 , offset: 0x20C */
#define WJ_IS_WCPU2_IS_WCPU2_0_Pos                 (0U)
#define WJ_IS_WCPU2_IS_WCPU2_0_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_0_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_0                     WJ_IS_WCPU2_IS_WCPU2_0_Msk

#define WJ_IS_WCPU2_IS_WCPU2_1_Pos                 (1U)
#define WJ_IS_WCPU2_IS_WCPU2_1_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_1_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_1                     WJ_IS_WCPU2_IS_WCPU2_1_Msk

#define WJ_IS_WCPU2_IS_WCPU2_2_Pos                 (2U)
#define WJ_IS_WCPU2_IS_WCPU2_2_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_2_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_2                     WJ_IS_WCPU2_IS_WCPU2_2_Msk

#define WJ_IS_WCPU2_IS_WCPU2_3_Pos                 (3U)
#define WJ_IS_WCPU2_IS_WCPU2_3_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_3_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_3                     WJ_IS_WCPU2_IS_WCPU2_3_Msk

#define WJ_IS_WCPU2_IS_WCPU2_4_Pos                 (4U)
#define WJ_IS_WCPU2_IS_WCPU2_4_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_4_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_4                     WJ_IS_WCPU2_IS_WCPU2_4_Msk

#define WJ_IS_WCPU2_IS_WCPU2_5_Pos                 (5U)
#define WJ_IS_WCPU2_IS_WCPU2_5_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_5_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_5                     WJ_IS_WCPU2_IS_WCPU2_5_Msk

#define WJ_IS_WCPU2_IS_WCPU2_6_Pos                 (6U)
#define WJ_IS_WCPU2_IS_WCPU2_6_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_6_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_6                     WJ_IS_WCPU2_IS_WCPU2_6_Msk

#define WJ_IS_WCPU2_IS_WCPU2_7_Pos                 (7U)
#define WJ_IS_WCPU2_IS_WCPU2_7_Msk                 (0x7U << WJ_IS_WCPU2_IS_WCPU2_7_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_7                     WJ_IS_WCPU2_IS_WCPU2_7_Msk

#define WJ_IS_WCPU2_IS_WCPU2_8_Pos                 (8U)
#define WJ_IS_WCPU2_IS_WCPU2_8_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_8_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_8                     WJ_IS_WCPU2_IS_WCPU2_8_Msk

#define WJ_IS_WCPU2_IS_WCPU2_9_Pos                 (9U)
#define WJ_IS_WCPU2_IS_WCPU2_9_Msk                 (0x1U << WJ_IS_WCPU2_IS_WCPU2_9_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_9                     WJ_IS_WCPU2_IS_WCPU2_9_Msk

#define WJ_IS_WCPU2_IS_WCPU2_10_Pos                (10U)
#define WJ_IS_WCPU2_IS_WCPU2_10_Msk                (0x1U << WJ_IS_WCPU2_IS_WCPU2_10_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_10                    WJ_IS_WCPU2_IS_WCPU2_10_Msk

#define WJ_IS_WCPU2_IS_WCPU2_11_Pos                (11U)
#define WJ_IS_WCPU2_IS_WCPU2_11_Msk                (0x1U << WJ_IS_WCPU2_IS_WCPU2_11_Pos)
#define WJ_IS_WCPU2_IS_WCPU2_11                    WJ_IS_WCPU2_IS_WCPU2_11_Msk

/* RIS_RCPU2, offset: 0x210 */
#define WJ_RIS_RCPU2_RIS_RCPU0_0_Pos               (0U)
#define WJ_RIS_RCPU2_RIS_RCPU0_0_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU0_0_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU0_0                   WJ_RIS_RCPU2_RIS_RCPU0_0_Msk

#define WJ_RIS_RCPU2_RIS_RCPU0_1_Pos               (1U)
#define WJ_RIS_RCPU2_RIS_RCPU0_1_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU0_1_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU0_1                   WJ_RIS_RCPU2_RIS_RCPU0_1_Msk

#define WJ_RIS_RCPU2_RIS_RCPU0_2_Pos               (2U)
#define WJ_RIS_RCPU2_RIS_RCPU0_2_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU0_2_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU0_2                   WJ_RIS_RCPU2_RIS_RCPU0_2_Msk

#define WJ_RIS_RCPU2_RIS_RCPU0_3_Pos               (3U)
#define WJ_RIS_RCPU2_RIS_RCPU0_3_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU0_3_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU0_3                   WJ_RIS_RCPU2_RIS_RCPU0_3_Msk

#define WJ_RIS_RCPU2_RIS_RCPU0_4_Pos               (4U)
#define WJ_RIS_RCPU2_RIS_RCPU0_4_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU0_4_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU0_4                   WJ_RIS_RCPU2_RIS_RCPU0_4_Msk

#define WJ_RIS_RCPU2_RIS_RCPU0_5_Pos               (5U)
#define WJ_RIS_RCPU2_RIS_RCPU0_5_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU0_5_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU0_5                   WJ_RIS_RCPU2_RIS_RCPU0_5_Msk

#define WJ_RIS_RCPU2_RIS_RCPU0_6_Pos               (6U)
#define WJ_RIS_RCPU2_RIS_RCPU0_6_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU0_6_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU0_6                   WJ_RIS_RCPU2_RIS_RCPU0_6_Msk

#define WJ_RIS_RCPU2_RIS_RCPU0_7_Pos               (7U)
#define WJ_RIS_RCPU2_RIS_RCPU0_7_Msk               (0x7U << WJ_RIS_RCPU2_RIS_RCPU0_7_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU0_7                   WJ_RIS_RCPU2_RIS_RCPU0_7_Msk

#define WJ_RIS_RCPU2_RIS_RCPU0_8_Pos               (8U)
#define WJ_RIS_RCPU2_RIS_RCPU0_8_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU0_8_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU0_8                   WJ_RIS_RCPU2_RIS_RCPU0_8_Msk

#define WJ_RIS_RCPU2_RIS_RCPU0_9_Pos               (9U)
#define WJ_RIS_RCPU2_RIS_RCPU0_9_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU0_9_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU0_9                   WJ_RIS_RCPU2_RIS_RCPU0_9_Msk

#define WJ_RIS_RCPU2_RIS_RCPU0_10_Pos              (10U)
#define WJ_RIS_RCPU2_RIS_RCPU0_10_Msk              (0x1U << WJ_RIS_RCPU2_RIS_RCPU0_10_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU0_10                  WJ_RIS_RCPU2_RIS_RCPU0_10_Msk

#define WJ_RIS_RCPU2_RIS_RCPU0_11_Pos              (11U)
#define WJ_RIS_RCPU2_RIS_RCPU0_11_Msk              (0x1U << WJ_RIS_RCPU2_RIS_RCPU0_11_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU0_11                  WJ_RIS_RCPU2_RIS_RCPU0_11_Msk

#define WJ_RIS_RCPU2_RIS_RCPU1_0_Pos               (16U)
#define WJ_RIS_RCPU2_RIS_RCPU1_0_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU1_0_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU1_0                   WJ_RIS_RCPU2_RIS_RCPU1_0_Msk

#define WJ_RIS_RCPU2_RIS_RCPU1_1_Pos               (17U)
#define WJ_RIS_RCPU2_RIS_RCPU1_1_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU1_1_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU1_1                   WJ_RIS_RCPU2_RIS_RCPU1_1_Msk

#define WJ_RIS_RCPU2_RIS_RCPU1_2_Pos               (18U)
#define WJ_RIS_RCPU2_RIS_RCPU1_2_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU1_2_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU1_2                   WJ_RIS_RCPU2_RIS_RCPU1_2_Msk

#define WJ_RIS_RCPU2_RIS_RCPU1_3_Pos               (19U)
#define WJ_RIS_RCPU2_RIS_RCPU1_3_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU1_3_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU1_3                   WJ_RIS_RCPU2_RIS_RCPU1_3_Msk

#define WJ_RIS_RCPU2_RIS_RCPU1_4_Pos               (20U)
#define WJ_RIS_RCPU2_RIS_RCPU1_4_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU1_4_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU1_4                   WJ_RIS_RCPU2_RIS_RCPU1_4_Msk

#define WJ_RIS_RCPU2_RIS_RCPU1_5_Pos               (21U)
#define WJ_RIS_RCPU2_RIS_RCPU1_5_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU1_5_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU1_5                   WJ_RIS_RCPU2_RIS_RCPU1_5_Msk

#define WJ_RIS_RCPU2_RIS_RCPU1_6_Pos               (22U)
#define WJ_RIS_RCPU2_RIS_RCPU1_6_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU1_6_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU1_6                   WJ_RIS_RCPU2_RIS_RCPU1_6_Msk

#define WJ_RIS_RCPU2_RIS_RCPU1_7_Pos               (23U)
#define WJ_RIS_RCPU2_RIS_RCPU1_7_Msk               (0x7U << WJ_RIS_RCPU2_RIS_RCPU1_7_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU1_7                   WJ_RIS_RCPU2_RIS_RCPU1_7_Msk

#define WJ_RIS_RCPU2_RIS_RCPU1_8_Pos               (24U)
#define WJ_RIS_RCPU2_RIS_RCPU1_8_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU1_8_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU1_8                   WJ_RIS_RCPU2_RIS_RCPU1_8_Msk

#define WJ_RIS_RCPU2_RIS_RCPU1_9_Pos               (25U)
#define WJ_RIS_RCPU2_RIS_RCPU1_9_Msk               (0x1U << WJ_RIS_RCPU2_RIS_RCPU1_9_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU1_9                   WJ_RIS_RCPU2_RIS_RCPU1_9_Msk

#define WJ_RIS_RCPU2_RIS_RCPU1_10_Pos              (26U)
#define WJ_RIS_RCPU2_RIS_RCPU1_10_Msk              (0x1U << WJ_RIS_RCPU2_RIS_RCPU1_10_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU1_10                  WJ_RIS_RCPU2_RIS_RCPU1_10_Msk

#define WJ_RIS_RCPU2_RIS_RCPU1_11_Pos              (27U)
#define WJ_RIS_RCPU2_RIS_RCPU1_11_Msk              (0x1U << WJ_RIS_RCPU2_RIS_RCPU1_11_Pos)
#define WJ_RIS_RCPU2_RIS_RCPU1_11                  WJ_RIS_RCPU2_RIS_RCPU1_11_Msk


/* IUM_RCPU2, offset: 0x214 */
#define WJ_IUM_RCPU2_IUM_RCPU0_0_Pos               (0U)
#define WJ_IUM_RCPU2_IUM_RCPU0_0_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU0_0_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU0_0                   WJ_IUM_RCPU2_IUM_RCPU0_0_Msk

#define WJ_IUM_RCPU2_IUM_RCPU0_1_Pos               (1U)
#define WJ_IUM_RCPU2_IUM_RCPU0_1_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU0_1_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU0_1                   WJ_IUM_RCPU2_IUM_RCPU0_1_Msk

#define WJ_IUM_RCPU2_IUM_RCPU0_2_Pos               (2U)
#define WJ_IUM_RCPU2_IUM_RCPU0_2_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU0_2_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU0_2                   WJ_IUM_RCPU2_IUM_RCPU0_2_Msk

#define WJ_IUM_RCPU2_IUM_RCPU0_3_Pos               (3U)
#define WJ_IUM_RCPU2_IUM_RCPU0_3_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU0_3_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU0_3                   WJ_IUM_RCPU2_IUM_RCPU0_3_Msk

#define WJ_IUM_RCPU2_IUM_RCPU0_4_Pos               (4U)
#define WJ_IUM_RCPU2_IUM_RCPU0_4_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU0_4_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU0_4                   WJ_IUM_RCPU2_IUM_RCPU0_4_Msk

#define WJ_IUM_RCPU2_IUM_RCPU0_5_Pos               (5U)
#define WJ_IUM_RCPU2_IUM_RCPU0_5_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU0_5_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU0_5                   WJ_IUM_RCPU2_IUM_RCPU0_5_Msk

#define WJ_IUM_RCPU2_IUM_RCPU0_6_Pos               (6U)
#define WJ_IUM_RCPU2_IUM_RCPU0_6_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU0_6_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU0_6                   WJ_IUM_RCPU2_IUM_RCPU0_6_Msk

#define WJ_IUM_RCPU2_IUM_RCPU0_7_Pos               (7U)
#define WJ_IUM_RCPU2_IUM_RCPU0_7_Msk               (0x7U << WJ_IUM_RCPU2_IUM_RCPU0_7_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU0_7                   WJ_IUM_RCPU2_IUM_RCPU0_7_Msk

#define WJ_IUM_RCPU2_IUM_RCPU0_8_Pos               (8U)
#define WJ_IUM_RCPU2_IUM_RCPU0_8_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU0_8_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU0_8                   WJ_IUM_RCPU2_IUM_RCPU0_8_Msk

#define WJ_IUM_RCPU2_IUM_RCPU0_9_Pos               (9U)
#define WJ_IUM_RCPU2_IUM_RCPU0_9_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU0_9_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU0_9                   WJ_IUM_RCPU2_IUM_RCPU0_9_Msk

#define WJ_IUM_RCPU2_IUM_RCPU0_10_Pos              (10U)
#define WJ_IUM_RCPU2_IUM_RCPU0_10_Msk              (0x1U << WJ_IUM_RCPU2_IUM_RCPU0_10_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU0_10                  WJ_IUM_RCPU2_IUM_RCPU0_10_Msk

#define WJ_IUM_RCPU2_IUM_RCPU0_11_Pos              (11U)
#define WJ_IUM_RCPU2_IUM_RCPU0_11_Msk              (0x1U << WJ_IUM_RCPU2_IUM_RCPU0_11_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU0_11                  WJ_IUM_RCPU2_IUM_RCPU0_11_Msk

#define WJ_IUM_RCPU2_IUM_RCPU1_0_Pos               (16U)
#define WJ_IUM_RCPU2_IUM_RCPU1_0_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU1_0_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU1_0                   WJ_IUM_RCPU2_IUM_RCPU1_0_Msk

#define WJ_IUM_RCPU2_IUM_RCPU1_1_Pos               (17U)
#define WJ_IUM_RCPU2_IUM_RCPU1_1_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU1_1_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU1_1                   WJ_IUM_RCPU2_IUM_RCPU1_1_Msk

#define WJ_IUM_RCPU2_IUM_RCPU1_2_Pos               (18U)
#define WJ_IUM_RCPU2_IUM_RCPU1_2_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU1_2_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU1_2                   WJ_IUM_RCPU2_IUM_RCPU1_2_Msk

#define WJ_IUM_RCPU2_IUM_RCPU1_3_Pos               (19U)
#define WJ_IUM_RCPU2_IUM_RCPU1_3_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU1_3_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU1_3                   WJ_IUM_RCPU2_IUM_RCPU1_3_Msk

#define WJ_IUM_RCPU2_IUM_RCPU1_4_Pos               (20U)
#define WJ_IUM_RCPU2_IUM_RCPU1_4_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU1_4_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU1_4                   WJ_IUM_RCPU2_IUM_RCPU1_4_Msk

#define WJ_IUM_RCPU2_IUM_RCPU1_5_Pos               (21U)
#define WJ_IUM_RCPU2_IUM_RCPU1_5_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU1_5_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU1_5                   WJ_IUM_RCPU2_IUM_RCPU1_5_Msk

#define WJ_IUM_RCPU2_IUM_RCPU1_6_Pos               (22U)
#define WJ_IUM_RCPU2_IUM_RCPU1_6_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU1_6_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU1_6                   WJ_IUM_RCPU2_IUM_RCPU1_6_Msk

#define WJ_IUM_RCPU2_IUM_RCPU1_7_Pos               (23U)
#define WJ_IUM_RCPU2_IUM_RCPU1_7_Msk               (0x7U << WJ_IUM_RCPU2_IUM_RCPU1_7_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU1_7                   WJ_IUM_RCPU2_IUM_RCPU1_7_Msk

#define WJ_IUM_RCPU2_IUM_RCPU1_8_Pos               (24U)
#define WJ_IUM_RCPU2_IUM_RCPU1_8_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU1_8_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU1_8                   WJ_IUM_RCPU2_IUM_RCPU1_8_Msk

#define WJ_IUM_RCPU2_IUM_RCPU1_9_Pos               (25U)
#define WJ_IUM_RCPU2_IUM_RCPU1_9_Msk               (0x1U << WJ_IUM_RCPU2_IUM_RCPU1_9_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU1_9                   WJ_IUM_RCPU2_IUM_RCPU1_9_Msk

#define WJ_IUM_RCPU2_IUM_RCPU1_10_Pos              (26U)
#define WJ_IUM_RCPU2_IUM_RCPU1_10_Msk              (0x1U << WJ_IUM_RCPU2_IUM_RCPU1_10_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU1_10                  WJ_IUM_RCPU2_IUM_RCPU1_10_Msk

#define WJ_IUM_RCPU2_IUM_RCPU1_11_Pos              (27U)
#define WJ_IUM_RCPU2_IUM_RCPU1_11_Msk              (0x1U << WJ_IUM_RCPU2_IUM_RCPU1_11_Pos)
#define WJ_IUM_RCPU2_IUM_RCPU1_11                  WJ_IUM_RCPU2_IUM_RCPU1_11_Msk

/* IS_RCPU2, offset: 0x218 */
#define WJ_IS_RCPU2_IS_RCPU0_0_Pos                 (0U)
#define WJ_IS_RCPU2_IS_RCPU0_0_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU0_0_Pos)
#define WJ_IS_RCPU2_IS_RCPU0_0                     WJ_IS_RCPU2_IS_RCPU0_0_Msk

#define WJ_IS_RCPU2_IS_RCPU0_1_Pos                 (1U)
#define WJ_IS_RCPU2_IS_RCPU0_1_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU0_1_Pos)
#define WJ_IS_RCPU2_IS_RCPU0_1                     WJ_IS_RCPU2_IS_RCPU0_1_Msk

#define WJ_IS_RCPU2_IS_RCPU0_2_Pos                 (2U)
#define WJ_IS_RCPU2_IS_RCPU0_2_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU0_2_Pos)
#define WJ_IS_RCPU2_IS_RCPU0_2                     WJ_IS_RCPU2_IS_RCPU0_2_Msk

#define WJ_IS_RCPU2_IS_RCPU0_3_Pos                 (3U)
#define WJ_IS_RCPU2_IS_RCPU0_3_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU0_3_Pos)
#define WJ_IS_RCPU2_IS_RCPU0_3                     WJ_IS_RCPU2_IS_RCPU0_3_Msk

#define WJ_IS_RCPU2_IS_RCPU0_4_Pos                 (4U)
#define WJ_IS_RCPU2_IS_RCPU0_4_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU0_4_Pos)
#define WJ_IS_RCPU2_IS_RCPU0_4                     WJ_IS_RCPU2_IS_RCPU0_4_Msk

#define WJ_IS_RCPU2_IS_RCPU0_5_Pos                 (5U)
#define WJ_IS_RCPU2_IS_RCPU0_5_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU0_5_Pos)
#define WJ_IS_RCPU2_IS_RCPU0_5                     WJ_IS_RCPU2_IS_RCPU0_5_Msk

#define WJ_IS_RCPU2_IS_RCPU0_6_Pos                 (6U)
#define WJ_IS_RCPU2_IS_RCPU0_6_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU0_6_Pos)
#define WJ_IS_RCPU2_IS_RCPU0_6                     WJ_IS_RCPU2_IS_RCPU0_6_Msk

#define WJ_IS_RCPU2_IS_RCPU0_7_Pos                 (7U)
#define WJ_IS_RCPU2_IS_RCPU0_7_Msk                 (0x7U << WJ_IS_RCPU2_IS_RCPU0_7_Pos)
#define WJ_IS_RCPU2_IS_RCPU0_7                     WJ_IS_RCPU2_IS_RCPU0_7_Msk

#define WJ_IS_RCPU2_IS_RCPU0_8_Pos                 (8U)
#define WJ_IS_RCPU2_IS_RCPU0_8_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU0_8_Pos)
#define WJ_IS_RCPU2_IS_RCPU0_8                     WJ_IS_RCPU2_IS_RCPU0_8_Msk

#define WJ_IS_RCPU2_IS_RCPU0_9_Pos                 (9U)
#define WJ_IS_RCPU2_IS_RCPU0_9_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU0_9_Pos)
#define WJ_IS_RCPU2_IS_RCPU0_9                     WJ_IS_RCPU2_IS_RCPU0_9_Msk

#define WJ_IS_RCPU2_IS_RCPU0_10_Pos                (10U)
#define WJ_IS_RCPU2_IS_RCPU0_10_Msk                (0x1U << WJ_IS_RCPU2_IS_RCPU0_10_Pos)
#define WJ_IS_RCPU2_IS_RCPU0_10                    WJ_IS_RCPU2_IS_RCPU0_10_Msk

#define WJ_IS_RCPU2_IS_RCPU0_11_Pos                (11U)
#define WJ_IS_RCPU2_IS_RCPU0_11_Msk                (0x1U << WJ_IS_RCPU2_IS_RCPU0_11_Pos)
#define WJ_IS_RCPU2_IS_RCPU0_11                    WJ_IS_RCPU2_IS_RCPU0_11_Msk

#define WJ_IS_RCPU2_IS_RCPU1_0_Pos                 (16U)
#define WJ_IS_RCPU2_IS_RCPU1_0_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU1_0_Pos)
#define WJ_IS_RCPU2_IS_RCPU1_0                     WJ_IS_RCPU2_IS_RCPU1_0_Msk

#define WJ_IS_RCPU2_IS_RCPU1_1_Pos                 (17U)
#define WJ_IS_RCPU2_IS_RCPU1_1_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU1_1_Pos)
#define WJ_IS_RCPU2_IS_RCPU1_1                     WJ_IS_RCPU2_IS_RCPU1_1_Msk

#define WJ_IS_RCPU2_IS_RCPU1_2_Pos                 (18U)
#define WJ_IS_RCPU2_IS_RCPU1_2_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU1_2_Pos)
#define WJ_IS_RCPU2_IS_RCPU1_2                     WJ_IS_RCPU2_IS_RCPU1_2_Msk

#define WJ_IS_RCPU2_IS_RCPU1_3_Pos                 (19U)
#define WJ_IS_RCPU2_IS_RCPU1_3_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU1_3_Pos)
#define WJ_IS_RCPU2_IS_RCPU1_3                     WJ_IS_RCPU2_IS_RCPU1_3_Msk

#define WJ_IS_RCPU2_IS_RCPU1_4_Pos                 (20U)
#define WJ_IS_RCPU2_IS_RCPU1_4_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU1_4_Pos)
#define WJ_IS_RCPU2_IS_RCPU1_4                     WJ_IS_RCPU2_IS_RCPU1_4_Msk

#define WJ_IS_RCPU2_IS_RCPU1_5_Pos                 (21U)
#define WJ_IS_RCPU2_IS_RCPU1_5_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU1_5_Pos)
#define WJ_IS_RCPU2_IS_RCPU1_5                     WJ_IS_RCPU2_IS_RCPU1_5_Msk

#define WJ_IS_RCPU2_IS_RCPU1_6_Pos                 (22U)
#define WJ_IS_RCPU2_IS_RCPU1_6_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU1_6_Pos)
#define WJ_IS_RCPU2_IS_RCPU1_6                     WJ_IS_RCPU2_IS_RCPU1_6_Msk

#define WJ_IS_RCPU2_IS_RCPU1_7_Pos                 (23U)
#define WJ_IS_RCPU2_IS_RCPU1_7_Msk                 (0x7U << WJ_IS_RCPU2_IS_RCPU1_7_Pos)
#define WJ_IS_RCPU2_IS_RCPU1_7                     WJ_IS_RCPU2_IS_RCPU1_7_Msk

#define WJ_IS_RCPU2_IS_RCPU1_8_Pos                 (24U)
#define WJ_IS_RCPU2_IS_RCPU1_8_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU1_8_Pos)
#define WJ_IS_RCPU2_IS_RCPU1_8                     WJ_IS_RCPU2_IS_RCPU1_8_Msk

#define WJ_IS_RCPU2_IS_RCPU1_9_Pos                 (25U)
#define WJ_IS_RCPU2_IS_RCPU1_9_Msk                 (0x1U << WJ_IS_RCPU2_IS_RCPU1_9_Pos)
#define WJ_IS_RCPU2_IS_RCPU1_9                     WJ_IS_RCPU2_IS_RCPU1_9_Msk

#define WJ_IS_RCPU2_IS_RCPU1_10_Pos                (26U)
#define WJ_IS_RCPU2_IS_RCPU1_10_Msk                (0x1U << WJ_IS_RCPU2_IS_RCPU1_10_Pos)
#define WJ_IS_RCPU2_IS_RCPU1_10                    WJ_IS_RCPU2_IS_RCPU1_10_Msk

#define WJ_IS_RCPU2_IS_RCPU1_11_Pos                (27U)
#define WJ_IS_RCPU2_IS_RCPU1_11_Msk                (0x1U << WJ_IS_RCPU2_IS_RCPU1_11_Pos)
#define WJ_IS_RCPU2_IS_RCPU1_11                    WJ_IS_RCPU2_IS_RCPU1_11_Msk

/* IC_RCPU2, offset: 0x21C */
#define WJ_IC_RCPU2_IC_RCPU0_0_Pos                 (0U)
#define WJ_IC_RCPU2_IC_RCPU0_0_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU0_0_Pos)
#define WJ_IC_RCPU2_IC_RCPU0_0                     WJ_IC_RCPU2_IC_RCPU0_0_Msk

#define WJ_IC_RCPU2_IC_RCPU0_1_Pos                 (1U)
#define WJ_IC_RCPU2_IC_RCPU0_1_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU0_1_Pos)
#define WJ_IC_RCPU2_IC_RCPU0_1                     WJ_IC_RCPU2_IC_RCPU0_1_Msk

#define WJ_IC_RCPU2_IC_RCPU0_2_Pos                 (2U)
#define WJ_IC_RCPU2_IC_RCPU0_2_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU0_2_Pos)
#define WJ_IC_RCPU2_IC_RCPU0_2                     WJ_IC_RCPU2_IC_RCPU0_2_Msk

#define WJ_IC_RCPU2_IC_RCPU0_3_Pos                 (3U)
#define WJ_IC_RCPU2_IC_RCPU0_3_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU0_3_Pos)
#define WJ_IC_RCPU2_IC_RCPU0_3                     WJ_IC_RCPU2_IC_RCPU0_3_Msk

#define WJ_IC_RCPU2_IC_RCPU0_4_Pos                 (4U)
#define WJ_IC_RCPU2_IC_RCPU0_4_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU0_4_Pos)
#define WJ_IC_RCPU2_IC_RCPU0_4                     WJ_IC_RCPU2_IC_RCPU0_4_Msk

#define WJ_IC_RCPU2_IC_RCPU0_5_Pos                 (5U)
#define WJ_IC_RCPU2_IC_RCPU0_5_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU0_5_Pos)
#define WJ_IC_RCPU2_IC_RCPU0_5                     WJ_IC_RCPU2_IC_RCPU0_5_Msk

#define WJ_IC_RCPU2_IC_RCPU0_6_Pos                 (6U)
#define WJ_IC_RCPU2_IC_RCPU0_6_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU0_6_Pos)
#define WJ_IC_RCPU2_IC_RCPU0_6                     WJ_IC_RCPU2_IC_RCPU0_6_Msk

#define WJ_IC_RCPU2_IC_RCPU0_7_Pos                 (7U)
#define WJ_IC_RCPU2_IC_RCPU0_7_Msk                 (0x7U << WJ_IC_RCPU2_IC_RCPU0_7_Pos)
#define WJ_IC_RCPU2_IC_RCPU0_7                     WJ_IC_RCPU2_IC_RCPU0_7_Msk

#define WJ_IC_RCPU2_IC_RCPU0_8_Pos                 (8U)
#define WJ_IC_RCPU2_IC_RCPU0_8_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU0_8_Pos)
#define WJ_IC_RCPU2_IC_RCPU0_8                     WJ_IC_RCPU2_IC_RCPU0_8_Msk

#define WJ_IC_RCPU2_IC_RCPU0_9_Pos                 (9U)
#define WJ_IC_RCPU2_IC_RCPU0_9_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU0_9_Pos)
#define WJ_IC_RCPU2_IC_RCPU0_9                     WJ_IC_RCPU2_IC_RCPU0_9_Msk

#define WJ_IC_RCPU2_IC_RCPU0_10_Pos                (10U)
#define WJ_IC_RCPU2_IC_RCPU0_10_Msk                (0x1U << WJ_IC_RCPU2_IC_RCPU0_10_Pos)
#define WJ_IC_RCPU2_IC_RCPU0_10                    WJ_IC_RCPU2_IC_RCPU0_10_Msk

#define WJ_IC_RCPU2_IC_RCPU0_11_Pos                (11U)
#define WJ_IC_RCPU2_IC_RCPU0_11_Msk                (0x1U << WJ_IC_RCPU2_IC_RCPU0_11_Pos)
#define WJ_IC_RCPU2_IC_RCPU0_11                    WJ_IC_RCPU2_IC_RCPU0_11_Msk

#define WJ_IC_RCPU2_IC_RCPU1_0_Pos                 (16U)
#define WJ_IC_RCPU2_IC_RCPU1_0_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU1_0_Pos)
#define WJ_IC_RCPU2_IC_RCPU1_0                     WJ_IC_RCPU2_IC_RCPU1_0_Msk

#define WJ_IC_RCPU2_IC_RCPU1_1_Pos                 (17U)
#define WJ_IC_RCPU2_IC_RCPU1_1_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU1_1_Pos)
#define WJ_IC_RCPU2_IC_RCPU1_1                     WJ_IC_RCPU2_IC_RCPU1_1_Msk

#define WJ_IC_RCPU2_IC_RCPU1_2_Pos                 (18U)
#define WJ_IC_RCPU2_IC_RCPU1_2_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU1_2_Pos)
#define WJ_IC_RCPU2_IC_RCPU1_2                     WJ_IC_RCPU2_IC_RCPU1_2_Msk

#define WJ_IC_RCPU2_IC_RCPU1_3_Pos                 (19U)
#define WJ_IC_RCPU2_IC_RCPU1_3_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU1_3_Pos)
#define WJ_IC_RCPU2_IC_RCPU1_3                     WJ_IC_RCPU2_IC_RCPU1_3_Msk

#define WJ_IC_RCPU2_IC_RCPU1_4_Pos                 (20U)
#define WJ_IC_RCPU2_IC_RCPU1_4_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU1_4_Pos)
#define WJ_IC_RCPU2_IC_RCPU1_4                     WJ_IC_RCPU2_IC_RCPU1_4_Msk

#define WJ_IC_RCPU2_IC_RCPU1_5_Pos                 (21U)
#define WJ_IC_RCPU2_IC_RCPU1_5_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU1_5_Pos)
#define WJ_IC_RCPU2_IC_RCPU1_5                     WJ_IC_RCPU2_IC_RCPU1_5_Msk

#define WJ_IC_RCPU2_IC_RCPU1_6_Pos                 (22U)
#define WJ_IC_RCPU2_IC_RCPU1_6_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU1_6_Pos)
#define WJ_IC_RCPU2_IC_RCPU1_6                     WJ_IC_RCPU2_IC_RCPU1_6_Msk

#define WJ_IC_RCPU2_IC_RCPU1_7_Pos                 (23U)
#define WJ_IC_RCPU2_IC_RCPU1_7_Msk                 (0x7U << WJ_IC_RCPU2_IC_RCPU1_7_Pos)
#define WJ_IC_RCPU2_IC_RCPU1_7                     WJ_IC_RCPU2_IC_RCPU1_7_Msk

#define WJ_IC_RCPU2_IC_RCPU1_8_Pos                 (24U)
#define WJ_IC_RCPU2_IC_RCPU1_8_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU1_8_Pos)
#define WJ_IC_RCPU2_IC_RCPU1_8                     WJ_IC_RCPU2_IC_RCPU1_8_Msk

#define WJ_IC_RCPU2_IC_RCPU1_9_Pos                 (25U)
#define WJ_IC_RCPU2_IC_RCPU1_9_Msk                 (0x1U << WJ_IC_RCPU2_IC_RCPU1_9_Pos)
#define WJ_IC_RCPU2_IC_RCPU1_9                     WJ_IC_RCPU2_IC_RCPU1_9_Msk

#define WJ_IC_RCPU2_IC_RCPU1_10_Pos                (26U)
#define WJ_IC_RCPU2_IC_RCPU1_10_Msk                (0x1U << WJ_IC_RCPU2_IC_RCPU1_10_Pos)
#define WJ_IC_RCPU2_IC_RCPU1_10                    WJ_IC_RCPU2_IC_RCPU1_10_Msk

#define WJ_IC_RCPU2_IC_RCPU1_11_Pos                (27U)
#define WJ_IC_RCPU2_IC_RCPU1_11_Msk                (0x1U << WJ_IC_RCPU2_IC_RCPU1_11_Pos)
#define WJ_IC_RCPU2_IC_RCPU1_11                    WJ_IC_RCPU2_IC_RCPU1_11_Msk

#define CPU_NUM                                   (3U)

typedef struct {
    __IM uint32_t RIS_WCPU;               /* Offset: 0x000h (R  )  CPU Raw interrupt state of CPU0s command register */
    __IOM uint32_t IUM_WCPU;              /* Offset: 0x004h (R/W)  CPU Interrupt Mask Register */
    __IM uint32_t IS_WCPU;                /* Offset: 0x008h (R/W)  CPU Interrupt Status Register */
    __OM uint32_t IC_WCPU;                /* Offset: 0x00Ch (R  )  CPU Interrupt Clear Register */
    __IM uint32_t RIS_RCPU;               /* Offset: 0x010h (R  )  CPU Raw interrupt state register */
    __IOM uint32_t IUM_RCPU;              /* Offset: 0x014h (R/W)  Unmask register CPU2’s command registers was read, which will generate interrupt to CPU */
    __IOM uint32_t IS_RCPU;               /* Offset: 0x018h (R/W)  Interrupt state bits, if some were set, it indicates the interrupt was reported to CPU. */
    __OM uint32_t IC_RCPU;                /* Offset: 0x01Ch (W1C)  If some bits were write by 1’b1 then the corresponding state bits of raw interrupt state is clear. */
    __IOM uint32_t CPU_COM_0;             /* Offset: 0x020h (R/W)  CPU command register 0 */
    __IOM uint32_t CPU_COM_1;             /* Offset: 0x024h (R  )  CPU command register 1 */
    __IOM uint32_t CPU_COM_2;             /* Offset: 0x028h (R  )  CPU command register 2 */
    __IOM uint32_t CPU_COM_3;             /* Offset: 0x02Ch (R/W)  CPU command register 3 */
    __IOM uint32_t CPU_COM_4;             /* Offset: 0x030h (R/W)  CPU command register 4 */
    __IOM uint32_t CPU_COM_5;             /* Offset: 0x034h (R/W)  CPU command register 5 */
    __IOM uint32_t CPU_COM_6;             /* Offset: 0x038h (R/W)  CPU command register 6 */
    __IOM uint32_t CPU_COM_7;             /* Offset: 0x03Ch (R/W)  CPU command register 7 */
    __IOM uint32_t CPU_COM_8;             /* Offset: 0x040h (R/W)  CPU command register 8 */
    __IOM uint32_t CPU_COM_9;             /* Offset: 0x044h (R/W)  CPU command register 9 */
    __IOM uint32_t CPU_COM_10;            /* Offset: 0x048h (R/W)  CPU command register 10 */
    __IOM uint32_t CPU_COM_11;            /* Offset: 0x04Ch (R/W)  CPU command register 11 */
    uint32_t RESERVED[44];
} wj_mbox_cpu_t;

typedef struct {
    wj_mbox_cpu_t MBOX_CPU[CPU_NUM];
} wj_mbox_regs_t;

static inline uint32_t wj_cpu_command_read_wr_raw_interrupt_state(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return mbox_base->MBOX_CPU[idx].RIS_WCPU;
}

static inline void wj_cpu_command_write_wr_interrupt_mask(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t state)
{
    mbox_base->MBOX_CPU[idx].IUM_WCPU = state;
}

static inline uint32_t wj_cpu_command_read_wr_interrupt_mask(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return mbox_base->MBOX_CPU[idx].IUM_WCPU;
}

static inline uint32_t wj_cpu_command_read_wr_interrupt_state(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return mbox_base->MBOX_CPU[idx].IS_WCPU;
}

static inline void wj_cpu_command_write_wr_interrupt_clear(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t state)
{
    mbox_base->MBOX_CPU[idx].IC_WCPU = state;
}

static inline uint32_t wj_cpu_command_read_rd_raw_interrupt_state(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return mbox_base->MBOX_CPU[idx].RIS_RCPU;
}

static inline void wj_cpu_command_write_rd_interrupt_mask(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t state)
{
    mbox_base->MBOX_CPU[idx].IUM_RCPU = state;
}

static inline uint32_t wj_cpu_command_read_rd_interrupt_state(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return mbox_base->MBOX_CPU[idx].IS_RCPU;
}

static inline void wj_cpu_command_write_rd_interrupt_clear(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t state)
{
    mbox_base->MBOX_CPU[idx].IC_RCPU = state;
}

static inline void wj_write_cpu_command_0(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t command)
{
    mbox_base->MBOX_CPU[idx].CPU_COM_0 = command;
}

static inline void wj_write_cpu_command_1(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t command)
{
    mbox_base->MBOX_CPU[idx].CPU_COM_1 = command;
}

static inline void wj_write_cpu_command_2(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t command)
{
    mbox_base->MBOX_CPU[idx].CPU_COM_2 = command;
}

static inline void wj_write_cpu_command_3(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t command)
{
    mbox_base->MBOX_CPU[idx].CPU_COM_3 = command;
}

static inline void wj_write_cpu_command_4(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t command)
{
    mbox_base->MBOX_CPU[idx].CPU_COM_4 = command;
}

static inline void wj_write_cpu_command_5(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t command)
{
    mbox_base->MBOX_CPU[idx].CPU_COM_5 = command;
}

static inline void wj_write_cpu_command_6(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t command)
{
    mbox_base->MBOX_CPU[idx].CPU_COM_6 = command;
}

static inline void wj_write_cpu_command_7(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t command)
{
    mbox_base->MBOX_CPU[idx].CPU_COM_7 = command;
}

static inline void wj_write_cpu_command_8(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t command)
{
    mbox_base->MBOX_CPU[idx].CPU_COM_8 = command;
}

static inline void wj_write_cpu_command_9(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t command)
{
    mbox_base->MBOX_CPU[idx].CPU_COM_9 = command;
}

static inline void wj_write_cpu_command_10(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t command)
{
    mbox_base->MBOX_CPU[idx].CPU_COM_10 = command;
}

static inline void wj_write_cpu_command_11(uint32_t idx, wj_mbox_regs_t *mbox_base, uint32_t command)
{
    mbox_base->MBOX_CPU[idx].CPU_COM_11 = command;
}

static inline uint32_t *wj_read_cpu_command_0(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return (uint32_t *)&mbox_base->MBOX_CPU[idx].CPU_COM_0;
}

static inline uint32_t *wj_read_cpu_command_1(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return (uint32_t *)&mbox_base->MBOX_CPU[idx].CPU_COM_1;
}

static inline uint32_t *wj_read_cpu_command_2(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return (uint32_t *)&mbox_base->MBOX_CPU[idx].CPU_COM_2;
}

static inline uint32_t *wj_read_cpu_command_3(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return (uint32_t *)&mbox_base->MBOX_CPU[idx].CPU_COM_3;
}

static inline uint32_t *wj_read_cpu_command_4(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return (uint32_t *)&mbox_base->MBOX_CPU[idx].CPU_COM_4;
}

static inline uint32_t *wj_read_cpu_command_5(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return (uint32_t *)&mbox_base->MBOX_CPU[idx].CPU_COM_5;
}

static inline uint32_t *wj_read_cpu_command_6(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return (uint32_t *)&mbox_base->MBOX_CPU[idx].CPU_COM_6;
}

static inline uint32_t *wj_read_cpu_command_7(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return (uint32_t *)&mbox_base->MBOX_CPU[idx].CPU_COM_7;
}

static inline uint32_t *wj_read_cpu_command_8(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return (uint32_t *)&mbox_base->MBOX_CPU[idx].CPU_COM_8;
}

static inline uint32_t *wj_read_cpu_command_9(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return (uint32_t *)&mbox_base->MBOX_CPU[idx].CPU_COM_9;
}

static inline uint32_t *wj_read_cpu_command_10(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return (uint32_t *)&mbox_base->MBOX_CPU[idx].CPU_COM_10;
}

static inline uint32_t *wj_read_cpu_command_11(uint32_t idx, wj_mbox_regs_t *mbox_base)
{
    return (uint32_t *)&mbox_base->MBOX_CPU[idx].CPU_COM_11;
}


#ifdef __cplusplus
}
#endif

#endif /* _WJ_MBOX_LL_H_ */
