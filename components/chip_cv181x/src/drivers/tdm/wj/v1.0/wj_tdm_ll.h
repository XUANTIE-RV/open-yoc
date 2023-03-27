/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_tdm_ll.h
 * @brief    header file for wj tdm ll driver
 * @version  V1.0
 * @date     20. Oct 2020
 ******************************************************************************/

#ifndef _WJ_TDM_LL_H_
#define _WJ_TDM_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * TDM register bit definitions
 */

/* TDM_EN , offset: 0x00 */
#define WJ_TDM_EN_Pos                             (0U)
#define WJ_TDM_EN_Msk                             (0x1U << WJ_TDM_EN_Pos)
#define WJ_TDM_EN                                 WJ_TDM_EN_Msk

/* TDM_CTL , offset: 0x04 */
#define WJ_TDM_CTL_MODE_Pos                       (0U)
#define WJ_TDM_CTL_MODE_Msk                       (0x1U << WJ_TDM_CTL_MODE_Pos)
#define WJ_TDM_CTL_MODE_MASTER                    WJ_TDM_CTL_MODE_Msk

#define WJ_TDM_CTL_DATAWTH_Pos                    (4U)
#define WJ_TDM_CTL_DATAWTH_Msk                    (0x3U << WJ_TDM_CTL_DATAWTH_Pos)
#define WJ_TDM_CTL_DATAWTH                        WJ_TDM_CTL_DATAWTH_Msk
#define WJ_TDM_CTL_DATAWTH_16BITS                 (0x0U << WJ_TDM_CTL_DATAWTH_Pos)
#define WJ_TDM_CTL_DATAWTH_16_SAVE_32BITS         (0x1U << WJ_TDM_CTL_DATAWTH_Pos)
#define WJ_TDM_CTL_DATAWTH_24BITS                 (0x2U << WJ_TDM_CTL_DATAWTH_Pos)
#define WJ_TDM_CTL_DATAWTH_32BITS                 (0x3U << WJ_TDM_CTL_DATAWTH_Pos)

#define WJ_TDM_CTL_CHNUM_Pos                      (8U)
#define WJ_TDM_CTL_CHNUM_Msk                      (0x3U << WJ_TDM_CTL_CHNUM_Pos)
#define WJ_TDM_CTL_CHNUM                          WJ_TDM_CTL_CHNUM_Msk
#define WJ_TDM_CTL_CHNUM_2CH                      (0x0U << WJ_TDM_CTL_CHNUM_Pos)
#define WJ_TDM_CTL_CHNUM_4CH                      (0x1U << WJ_TDM_CTL_CHNUM_Pos)
#define WJ_TDM_CTL_CHNUM_8CH                      (0x2U << WJ_TDM_CTL_CHNUM_Pos)
#define WJ_TDM_CTL_CHNUM_16CH                     (0x3U << WJ_TDM_CTL_CHNUM_Pos)

#define WJ_TDM_CTL_CHORD_Pos                      (12U)
#define WJ_TDM_CTL_CHORD_Msk                      (0x1U << WJ_TDM_CTL_CHORD_Pos)
#define WJ_TDM_CTL_CHORD_RIGHT                    WJ_TDM_CTL_CHORD_Msk

#define WJ_TDM_CTL_SPEDGE_Pos                     (13U)
#define WJ_TDM_CTL_SPEDGE_Msk                     (0x1U << WJ_TDM_CTL_SPEDGE_Pos)
#define WJ_TDM_CTL_SPEDGE_FALL_EDGE               WJ_TDM_CTL_SPEDGE_Msk

/* CHOFFSET1 , offset: 0x08 */
#define WJ_TDM_CHOFFSET1_Pos                       (0U)
#define WJ_TDM_CHOFFSET1_Msk                       (0xFU << WJ_TDM_CHOFFSET1_Pos)
#define WJ_TDM_CHOFFSET1                           WJ_TDM_CTL_MODE_Msk

#define WJ_TDM_CHOFFSET2_Pos                       (8U)
#define WJ_TDM_CHOFFSET2_Msk                       (0xFU << WJ_TDM_CHOFFSET2_Pos)
#define WJ_TDM_CHOFFSET2                           WJ_TDM_CHOFFSET2_Msk

/* CHOFFSET2 , offset: 0x0c */
#define WJ_TDM_CHOFFSET3_Pos                       (0U)
#define WJ_TDM_CHOFFSET3_Msk                       (0xFU << WJ_TDM_CHOFFSET3_Pos)
#define WJ_TDM_CHOFFSET3                           WJ_TDM_CHOFFSET3_Msk

#define WJ_TDM_CHOFFSET4_Pos                       (8U)
#define WJ_TDM_CHOFFSET4_Msk                       (0xFU << WJ_TDM_CHOFFSET4_Pos)
#define WJ_TDM_CHOFFSET4                           WJ_TDM_CHOFFSET4_Msk

/* CHOFFSET3 , offset: 0x10 */
#define WJ_TDM_CHOFFSET5_Pos                       (0U)
#define WJ_TDM_CHOFFSET5_Msk                       (0xFU << WJ_TDM_CHOFFSET5_Pos)
#define WJ_TDM_CHOFFSET5                           WJ_TDM_CHOFFSET5_Msk

#define WJ_TDM_CHOFFSET6_Pos                       (8U)
#define WJ_TDM_CHOFFSET6_Msk                       (0xFU << WJ_TDM_CHOFFSET6_Pos)
#define WJ_TDM_CHOFFSET6                           WJ_TDM_CHOFFSET6_Msk

/* CHOFFSET4 , offset: 0x14 */
#define WJ_TDM_CHOFFSET7_Pos                       (0U)
#define WJ_TDM_CHOFFSET7_Msk                       (0xFU << WJ_TDM_CHOFFSET7_Pos)
#define WJ_TDM_CHOFFSET7                           WJ_TDM_CHOFFSET7_Msk

#define WJ_TDM_CHOFFSET8_Pos                       (8U)
#define WJ_TDM_CHOFFSET8_Msk                       (0xFU << WJ_TDM_CHOFFSET8_Pos)
#define WJ_TDM_CHOFFSET8                           WJ_TDM_CHOFFSET8_Msk

/* FIFOTL1 , offset: 0x18 */
#define WJ_TDM_FIFOTL1_LFT1_Pos                    (0U)
#define WJ_TDM_FIFOTL1_LFT1_Msk                    (0x7U << WJ_TDM_FIFOTL1_LFT1_Pos)
#define WJ_TDM_FIFOTL1_LFT1                        WJ_TDM_FIFOTL1_LFT1_Msk

#define WJ_TDM_FIFOTL1_RFT1_Pos                    (4U)
#define WJ_TDM_FIFOTL1_RFT1_Msk                    (0x7U << WJ_TDM_FIFOTL1_RFT1_Pos)
#define WJ_TDM_FIFOTL1_RFT1                        WJ_TDM_FIFOTL1_RFT1_Msk

/* FIFOTL2 , offset: 0x1C */
#define WJ_TDM_FIFOTL1_LFT2_Pos                    (0U)
#define WJ_TDM_FIFOTL1_LFT2_Msk                    (0x7U << WJ_TDM_FIFOTL1_LFT2_Pos)
#define WJ_TDM_FIFOTL1_LFT2                        WJ_TDM_FIFOTL1_LFT2_Msk

#define WJ_TDM_FIFOTL1_RFT2_Pos                    (4U)
#define WJ_TDM_FIFOTL1_RFT2_Msk                    (0x7U << WJ_TDM_FIFOTL1_RFT2_Pos)
#define WJ_TDM_FIFOTL1_RFT2                        WJ_TDM_FIFOTL1_RFT2_Msk

/* FIFOTL3 , offset: 0x20 */
#define WJ_TDM_FIFOTL1_LFT3_Pos                    (0U)
#define WJ_TDM_FIFOTL1_LFT3_Msk                    (0x7U << WJ_TDM_FIFOTL1_LFT3_Pos)
#define WJ_TDM_FIFOTL1_LFT3                        WJ_TDM_FIFOTL1_LFT3_Msk

#define WJ_TDM_FIFOTL1_RFT3_Pos                    (4U)
#define WJ_TDM_FIFOTL1_RFT3_Msk                    (0x7U << WJ_TDM_FIFOTL1_RFT3_Pos)
#define WJ_TDM_FIFOTL1_RFT3                        WJ_TDM_FIFOTL1_RFT3_Msk

/* FIFOTL4 , offset: 0x24 */
#define WJ_TDM_FIFOTL1_LFT4_Pos                    (0U)
#define WJ_TDM_FIFOTL1_LFT4_Msk                    (0x7U << WJ_TDM_FIFOTL1_LFT4_Pos)
#define WJ_TDM_FIFOTL1_LFT4                        WJ_TDM_FIFOTL1_LFT4_Msk

#define WJ_TDM_FIFOTL1_RFT4_Pos                    (4U)
#define WJ_TDM_FIFOTL1_RFT4_Msk                    (0x7U << WJ_TDM_FIFOTL1_RFT4_Pos)
#define WJ_TDM_FIFOTL1_RFT4                        WJ_TDM_FIFOTL1_RFT4_Msk

/* SR , offset: 0x28 */
#define WJ_TDM_SR_TDM_BUSY_Pos                     (0U)
#define WJ_TDM_SR_TDM_BUSY_Msk                     (0x1U << WJ_TDM_SR_TDM_BUSY_Pos)
#define WJ_TDM_SR_TDM_BUSY                         WJ_TDM_SR_TDM_BUSY_Msk

#define WJ_TDM_SR_LFNE1_Pos                        (4U)
#define WJ_TDM_SR_LFNE1_Msk                        (0x1U << WJ_TDM_SR_LFNE1_Pos)
#define WJ_TDM_SR_LFNE1                            WJ_TDM_SR_LFNE1_Msk

#define WJ_TDM_SR_LEF1_Pos                         (5U)
#define WJ_TDM_SR_LEF1_Msk                         (0x1U << WJ_TDM_SR_LEF1_Pos)
#define WJ_TDM_SR_LEF1                             WJ_TDM_SR_LEF1_Msk

#define WJ_TDM_SR_RFNE1_Pos                        (6U)
#define WJ_TDM_SR_RFNE1_Msk                        (0x1U << WJ_TDM_SR_RFNE1_Pos)
#define WJ_TDM_SR_RFNE1                            WJ_TDM_SR_RFNE1_Msk

#define WJ_TDM_SR_REF1_Pos                         (7U)
#define WJ_TDM_SR_REF1_Msk                         (0x1U << WJ_TDM_SR_REF1_Pos)
#define WJ_TDM_SR_REF1                             WJ_TDM_SR_REF1_Msk

#define WJ_TDM_SR_LFNE2_Pos                        (8U)
#define WJ_TDM_SR_LFNE2_Msk                        (0x1U << WJ_TDM_SR_LFNE2_Pos)
#define WJ_TDM_SR_LFNE2                            WJ_TDM_SR_LFNE2_Msk

#define WJ_TDM_SR_LEF2_Pos                         (9U)
#define WJ_TDM_SR_LEF2_Msk                         (0x1U << WJ_TDM_SR_LEF2_Pos)
#define WJ_TDM_SR_LEF2                             WJ_TDM_SR_LEF2_Msk

#define WJ_TDM_SR_RFNE2_Pos                        (10U)
#define WJ_TDM_SR_RFNE2_Msk                        (0x1U << WJ_TDM_SR_RFNE2_Pos)
#define WJ_TDM_SR_RFNE2                            WJ_TDM_SR_RFNE2_Msk

#define WJ_TDM_SR_REF2_Pos                         (11U)
#define WJ_TDM_SR_REF2_Msk                         (0x1U << WJ_TDM_SR_REF2_Pos)
#define WJ_TDM_SR_REF2                             WJ_TDM_SR_REF2_Msk

#define WJ_TDM_SR_LFNE3_Pos                        (12U)
#define WJ_TDM_SR_LFNE3_Msk                        (0x1U << WJ_TDM_SR_LFNE3_Pos)
#define WJ_TDM_SR_LFNE3                            WJ_TDM_SR_LFNE3_Msk

#define WJ_TDM_SR_LEF3_Pos                         (13U)
#define WJ_TDM_SR_LEF3_Msk                         (0x1U << WJ_TDM_SR_LEF3_Pos)
#define WJ_TDM_SR_LEF3                             WJ_TDM_SR_LEF3_Msk

#define WJ_TDM_SR_RFNE3_Pos                        (14U)
#define WJ_TDM_SR_RFNE3_Msk                        (0x1U << WJ_TDM_SR_RFNE3_Pos)
#define WJ_TDM_SR_RFNE3                            WJ_TDM_SR_RFNE3_Msk

#define WJ_TDM_SR_REF3_Pos                         (15U)
#define WJ_TDM_SR_REF3_Msk                         (0x1U << WJ_TDM_SR_REF3_Pos)
#define WJ_TDM_SR_REF3                             WJ_TDM_SR_REF3_Msk

#define WJ_TDM_SR_LFNE4_Pos                        (16U)
#define WJ_TDM_SR_LFNE4_Msk                        (0x1U << WJ_TDM_SR_LFNE4_Pos)
#define WJ_TDM_SR_LFNE4                            WJ_TDM_SR_LFNE4_Msk

#define WJ_TDM_SR_LEF4_Pos                         (17U)
#define WJ_TDM_SR_LEF4_Msk                         (0x1U << WJ_TDM_SR_LEF4_Pos)
#define WJ_TDM_SR_LEF4                             WJ_TDM_SR_LEF4_Msk

#define WJ_TDM_SR_RFNE4_Pos                        (18U)
#define WJ_TDM_SR_RFNE4_Msk                        (0x1U << WJ_TDM_SR_RFNE4_Pos)
#define WJ_TDM_SR_RFNE4                            WJ_TDM_SR_RFNE4_Msk

#define WJ_TDM_SR_REF4_Pos                         (19U)
#define WJ_TDM_SR_REF4_Msk                         (0x1U << WJ_TDM_SR_REF4_Pos)
#define WJ_TDM_SR_REF4                             WJ_TDM_SR_REF4_Msk

/* IMR , offset: 0x2C */
#define WJ_TDM_IMR_TDMBIM_Pos                      (0U)
#define WJ_TDM_IMR_TDMBIM_Msk                      (0x1U << WJ_TDM_IMR_TDMBIM_Pos)
#define WJ_TDM_IMR_TDMBIM                          WJ_TDM_IMR_TDMBIM_Msk

#define WJ_TDM_IMR_TDMWAEM_Pos                     (1U)
#define WJ_TDM_IMR_TDMWAEM_Msk                     (0x1U << WJ_TDM_IMR_TDMWAEM_Pos)
#define WJ_TDM_IMR_TDMWAEM                         WJ_TDM_IMR_TDMWAEM_Msk

#define WJ_TDM_IMR_LF1UEM_Pos                      (2U)
#define WJ_TDM_IMR_LF1UEM_Msk                      (0x1U << WJ_TDM_IMR_LF1UEM_Pos)
#define WJ_TDM_IMR_LF1UEM                          WJ_TDM_IMR_LF1UEM_Msk

#define WJ_TDM_IMR_LF1OEM_Pos                      (3U)
#define WJ_TDM_IMR_LF1OEM_Msk                      (0x1U << WJ_TDM_IMR_LF1OEM_Pos)
#define WJ_TDM_IMR_LF1OEM                          WJ_TDM_IMR_LF1OEM_Msk

#define WJ_TDM_IMR_LF1FIM_Pos                      (4U)
#define WJ_TDM_IMR_LF1FIM_Msk                      (0x1U << WJ_TDM_IMR_LF1FIM_Pos)
#define WJ_TDM_IMR_LF1FIM                          WJ_TDM_IMR_LF1FIM_Msk

#define WJ_TDM_IMR_RF1UEM_Pos                      (5U)
#define WJ_TDM_IMR_RF1UEM_Msk                      (0x1U << WJ_TDM_IMR_RF1UEM_Pos)
#define WJ_TDM_IMR_RF1UEM                          WJ_TDM_IMR_RF1UEM_Msk

#define WJ_TDM_IMR_RF1OEM_Pos                      (6U)
#define WJ_TDM_IMR_RF1OEM_Msk                      (0x1U << WJ_TDM_IMR_RF1OEM_Pos)
#define WJ_TDM_IMR_RF1OEM                          WJ_TDM_IMR_RF1OEM_Msk

#define WJ_TDM_IMR_RF1FIM_Pos                      (7U)
#define WJ_TDM_IMR_RF1FIM_Msk                      (0x1U << WJ_TDM_IMR_RF1FIM_Pos)
#define WJ_TDM_IMR_RF1FIM                          WJ_TDM_IMR_RF1FIM_Msk

#define WJ_TDM_IMR_LF2UEM_Pos                      (8U)
#define WJ_TDM_IMR_LF2UEM_Msk                      (0x1U << WJ_TDM_IMR_LF2UEM_Pos)
#define WJ_TDM_IMR_LF2UEM                          WJ_TDM_IMR_LF2UEM_Msk

#define WJ_TDM_IMR_LF2OEM_Pos                      (9U)
#define WJ_TDM_IMR_LF2OEM_Msk                      (0x1U << WJ_TDM_IMR_LF2OEM_Pos)
#define WJ_TDM_IMR_LF2OEM                          WJ_TDM_IMR_LF2OEM_Msk

#define WJ_TDM_IMR_LF2FIM_Pos                      (10U)
#define WJ_TDM_IMR_LF2FIM_Msk                      (0x1U << WJ_TDM_IMR_LF2FIM_Pos)
#define WJ_TDM_IMR_LF2FIM                          WJ_TDM_IMR_LF2FIM_Msk

#define WJ_TDM_IMR_RF2UEM_Pos                      (12U)
#define WJ_TDM_IMR_RF2UEM_Msk                      (0x1U << WJ_TDM_IMR_RF2UEM_Pos)
#define WJ_TDM_IMR_RF2UEM                          WJ_TDM_IMR_RF2UEM_Msk

#define WJ_TDM_IMR_RF2OEM_Pos                      (13U)
#define WJ_TDM_IMR_RF2OEM_Msk                      (0x1U << WJ_TDM_IMR_RF2OEM_Pos)
#define WJ_TDM_IMR_RF2OEM                          WJ_TDM_IMR_RF2OEM_Msk

#define WJ_TDM_IMR_RF2FIM_Pos                      (14U)
#define WJ_TDM_IMR_RF2FIM_Msk                      (0x1U << WJ_TDM_IMR_RF2FIM_Pos)
#define WJ_TDM_IMR_RF2FIM                          WJ_TDM_IMR_RF2FIM_Msk

#define WJ_TDM_IMR_LF3UEM_Pos                      (16U)
#define WJ_TDM_IMR_LF3UEM_Msk                      (0x1U << WJ_TDM_IMR_LF3UEM_Pos)
#define WJ_TDM_IMR_LF3UEM                          WJ_TDM_IMR_LF3UEM_Msk

#define WJ_TDM_IMR_LF3OEM_Pos                      (17U)
#define WJ_TDM_IMR_LF3OEM_Msk                      (0x1U << WJ_TDM_IMR_LF3OEM_Pos)
#define WJ_TDM_IMR_LF3OEM                          WJ_TDM_IMR_LF3OEM_Msk

#define WJ_TDM_IMR_LF3FIM_Pos                      (18U)
#define WJ_TDM_IMR_LF3FIM_Msk                      (0x1U << WJ_TDM_IMR_LF3FIM_Pos)
#define WJ_TDM_IMR_LF3FIM                          WJ_TDM_IMR_LF3FIM_Msk

#define WJ_TDM_IMR_RF3UEM_Pos                      (20U)
#define WJ_TDM_IMR_RF3UEM_Msk                      (0x1U << WJ_TDM_IMR_RF3UEM_Pos)
#define WJ_TDM_IMR_RF3UEM                          WJ_TDM_IMR_RF3UEM_Msk

#define WJ_TDM_IMR_RF3OEM_Pos                      (21U)
#define WJ_TDM_IMR_RF3OEM_Msk                      (0x1U << WJ_TDM_IMR_RF3OEM_Pos)
#define WJ_TDM_IMR_RF3OEM                          WJ_TDM_IMR_RF3OEM_Msk

#define WJ_TDM_IMR_RF3FIM_Pos                      (22U)
#define WJ_TDM_IMR_RF3FIM_Msk                      (0x1U << WJ_TDM_IMR_RF3FIM_Pos)
#define WJ_TDM_IMR_RF3FIM                          WJ_TDM_IMR_RF3FIM_Msk

#define WJ_TDM_IMR_LF4UEM_Pos                      (24U)
#define WJ_TDM_IMR_LF4UEM_Msk                      (0x1U << WJ_TDM_IMR_LF4UEM_Pos)
#define WJ_TDM_IMR_LF4UEM                          WJ_TDM_IMR_LF4UEM_Msk

#define WJ_TDM_IMR_LF4OEM_Pos                      (25U)
#define WJ_TDM_IMR_LF4OEM_Msk                      (0x1U << WJ_TDM_IMR_LF4OEM_Pos)
#define WJ_TDM_IMR_LF4OEM                          WJ_TDM_IMR_LF4OEM_Msk

#define WJ_TDM_IMR_LF4FIM_Pos                      (26U)
#define WJ_TDM_IMR_LF4FIM_Msk                      (0x1U << WJ_TDM_IMR_LF4FIM_Pos)
#define WJ_TDM_IMR_LF4FIM                          WJ_TDM_IMR_LF4FIM_Msk

#define WJ_TDM_IMR_RF4UEM_Pos                      (28U)
#define WJ_TDM_IMR_RF4UEM_Msk                      (0x1U << WJ_TDM_IMR_RF4UEM_Pos)
#define WJ_TDM_IMR_RF4UEM                          WJ_TDM_IMR_RF4UEM_Msk

#define WJ_TDM_IMR_RF4OEM_Pos                      (29U)
#define WJ_TDM_IMR_RF4OEM_Msk                      (0x1U << WJ_TDM_IMR_RF4OEM_Pos)
#define WJ_TDM_IMR_RF4OEM                          WJ_TDM_IMR_RF4OEM_Msk

#define WJ_TDM_IMR_RF4FIM_Pos                      (30U)
#define WJ_TDM_IMR_RF4FIM_Msk                      (0x1U << WJ_TDM_IMR_RF4FIM_Pos)
#define WJ_TDM_IMR_RF4FIM                          WJ_TDM_IMR_RF4FIM_Msk

/* ISR , offset: 0x30 */
#define WJ_TDM_ISR_TDMBIS_Pos                      (0U)
#define WJ_TDM_ISR_TDMBIS_Msk                      (0x1U << WJ_TDM_ISR_TDMBIS_Pos)
#define WJ_TDM_ISR_TDMBIS                          WJ_TDM_ISR_TDMBIS_Msk

#define WJ_TDM_ISR_TDMWAES_Pos                     (1U)
#define WJ_TDM_ISR_TDMWAES_Msk                     (0x1U << WJ_TDM_ISR_TDMWAES_Pos)
#define WJ_TDM_ISR_TDMWAES                         WJ_TDM_ISR_TDMWAES_Msk

#define WJ_TDM_ISR_LF1UES_Pos                      (2U)
#define WJ_TDM_ISR_LF1UES_Msk                      (0x1U << WJ_TDM_ISR_LF1UES_Pos)
#define WJ_TDM_ISR_LF1UES                          WJ_TDM_ISR_LF1UES_Msk

#define WJ_TDM_ISR_LF1OES_Pos                      (3U)
#define WJ_TDM_ISR_LF1OES_Msk                      (0x1U << WJ_TDM_ISR_LF1OES_Pos)
#define WJ_TDM_ISR_LF1OES                          WJ_TDM_ISR_LF1OES_Msk

#define WJ_TDM_ISR_LF1FIS_Pos                      (4U)
#define WJ_TDM_ISR_LF1FIS_Msk                      (0x1U << WJ_TDM_ISR_LF1FIS_Pos)
#define WJ_TDM_ISR_LF1FIS                          WJ_TDM_ISR_LF1FIS_Msk

#define WJ_TDM_ISR_RF1UES_Pos                      (5U)
#define WJ_TDM_ISR_RF1UES_Msk                      (0x1U << WJ_TDM_ISR_RF1UES_Pos)
#define WJ_TDM_ISR_RF1UES                          WJ_TDM_ISR_RF1UES_Msk

#define WJ_TDM_ISR_RF1OES_Pos                      (6U)
#define WJ_TDM_ISR_RF1OES_Msk                      (0x1U << WJ_TDM_ISR_RF1OES_Pos)
#define WJ_TDM_ISR_RF1OES                          WJ_TDM_ISR_RF1OES_Msk

#define WJ_TDM_ISR_RF1FIS_Pos                      (7U)
#define WJ_TDM_ISR_RF1FIS_Msk                      (0x1U << WJ_TDM_ISR_RF1FIS_Pos)
#define WJ_TDM_ISR_RF1FIS                          WJ_TDM_ISR_RF1FIS_Msk

#define WJ_TDM_ISR_LF2UES_Pos                      (8U)
#define WJ_TDM_ISR_LF2UES_Msk                      (0x1U << WJ_TDM_ISR_LF2UES_Pos)
#define WJ_TDM_ISR_LF2UES                          WJ_TDM_ISR_LF2UES_Msk

#define WJ_TDM_ISR_LF2OES_Pos                      (9U)
#define WJ_TDM_ISR_LF2OES_Msk                      (0x1U << WJ_TDM_ISR_LF2OES_Pos)
#define WJ_TDM_ISR_LF2OES                          WJ_TDM_ISR_LF2OES_Msk

#define WJ_TDM_ISR_LF2FIS_Pos                      (10U)
#define WJ_TDM_ISR_LF2FIS_Msk                      (0x1U << WJ_TDM_ISR_LF2FIS_Pos)
#define WJ_TDM_ISR_LF2FIS                          WJ_TDM_ISR_LF2FIS_Msk

#define WJ_TDM_ISR_RF2UES_Pos                      (12U)
#define WJ_TDM_ISR_RF2UES_Msk                      (0x1U << WJ_TDM_ISR_RF2UES_Pos)
#define WJ_TDM_ISR_RF2UES                          WJ_TDM_ISR_RF2UES_Msk

#define WJ_TDM_ISR_RF2OES_Pos                      (13U)
#define WJ_TDM_ISR_RF2OES_Msk                      (0x1U << WJ_TDM_ISR_RF2OES_Pos)
#define WJ_TDM_ISR_RF2OES                          WJ_TDM_ISR_RF2OES_Msk

#define WJ_TDM_ISR_RF2FIS_Pos                      (14U)
#define WJ_TDM_ISR_RF2FIS_Msk                      (0x1U << WJ_TDM_ISR_RF2FIS_Pos)
#define WJ_TDM_ISR_RF2FIS                          WJ_TDM_ISR_RF2FIS_Msk

#define WJ_TDM_ISR_LF3UES_Pos                      (16U)
#define WJ_TDM_ISR_LF3UES_Msk                      (0x1U << WJ_TDM_ISR_LF3UES_Pos)
#define WJ_TDM_ISR_LF3UES                          WJ_TDM_ISR_LF3UES_Msk

#define WJ_TDM_ISR_LF3OES_Pos                      (17U)
#define WJ_TDM_ISR_LF3OES_Msk                      (0x1U << WJ_TDM_ISR_LF3OES_Pos)
#define WJ_TDM_ISR_LF3OES                          WJ_TDM_ISR_LF3OES_Msk

#define WJ_TDM_ISR_LF3FIS_Pos                      (18U)
#define WJ_TDM_ISR_LF3FIS_Msk                      (0x1U << WJ_TDM_ISR_LF3FIS_Pos)
#define WJ_TDM_ISR_LF3FIS                          WJ_TDM_ISR_LF3FIS_Msk

#define WJ_TDM_ISR_RF3UES_Pos                      (20U)
#define WJ_TDM_ISR_RF3UES_Msk                      (0x1U << WJ_TDM_ISR_RF3UES_Pos)
#define WJ_TDM_ISR_RF3UES                          WJ_TDM_ISR_RF3UES_Msk

#define WJ_TDM_ISR_RF3OES_Pos                      (21U)
#define WJ_TDM_ISR_RF3OES_Msk                      (0x1U << WJ_TDM_ISR_RF3OES_Pos)
#define WJ_TDM_ISR_RF3OES                          WJ_TDM_ISR_RF3OES_Msk

#define WJ_TDM_ISR_RF3FIS_Pos                      (22U)
#define WJ_TDM_ISR_RF3FIS_Msk                      (0x1U << WJ_TDM_ISR_RF3FIS_Pos)
#define WJ_TDM_ISR_RF3FIS                          WJ_TDM_ISR_RF3FIS_Msk

#define WJ_TDM_ISR_LF4UES_Pos                      (24U)
#define WJ_TDM_ISR_LF4UES_Msk                      (0x1U << WJ_TDM_ISR_LF4UES_Pos)
#define WJ_TDM_ISR_LF4UES                          WJ_TDM_ISR_LF4UES_Msk

#define WJ_TDM_ISR_LF4OES_Pos                      (25U)
#define WJ_TDM_ISR_LF4OES_Msk                      (0x1U << WJ_TDM_ISR_LF4OES_Pos)
#define WJ_TDM_ISR_LF4OES                          WJ_TDM_ISR_LF4OES_Msk

#define WJ_TDM_ISR_LF4FIS_Pos                      (26U)
#define WJ_TDM_ISR_LF4FIS_Msk                      (0x1U << WJ_TDM_ISR_LF4FIS_Pos)
#define WJ_TDM_ISR_LF4FIS                          WJ_TDM_ISR_LF4FIS_Msk

#define WJ_TDM_ISR_RF4UES_Pos                      (28U)
#define WJ_TDM_ISR_RF4UES_Msk                      (0x1U << WJ_TDM_ISR_RF4UES_Pos)
#define WJ_TDM_ISR_RF4UES                          WJ_TDM_ISR_RF4UES_Msk

#define WJ_TDM_ISR_RF4OES_Pos                      (29U)
#define WJ_TDM_ISR_RF4OES_Msk                      (0x1U << WJ_TDM_ISR_RF4OES_Pos)
#define WJ_TDM_ISR_RF4OES                          WJ_TDM_ISR_RF4OES_Msk

#define WJ_TDM_ISR_RF4FIS_Pos                      (30U)
#define WJ_TDM_ISR_RF4FIS_Msk                      (0x1U << WJ_TDM_ISR_RF4FIS_Pos)
#define WJ_TDM_ISR_RF4FIS                          WJ_TDM_ISR_RF4FIS_Msk

/* RISR , offset: 0x34 */
#define WJ_TDM_ISR_RTDMBIS_Pos                      (0U)
#define WJ_TDM_ISR_RTDMBIS_Msk                      (0x1U << WJ_TDM_ISR_RTDMBIS_Pos)
#define WJ_TDM_ISR_RTDMBIS                          WJ_TDM_ISR_RTDMBIS_Msk

#define WJ_TDM_ISR_RTDMWAES_Pos                     (1U)
#define WJ_TDM_ISR_RTDMWAES_Msk                     (0x1U << WJ_TDM_ISR_RTDMWAES_Pos)
#define WJ_TDM_ISR_RTDMWAES                         WJ_TDM_ISR_RTDMWAES_Msk

#define WJ_TDM_ISR_RLF1UES_Pos                      (2U)
#define WJ_TDM_ISR_RLF1UES_Msk                      (0x1U << WJ_TDM_ISR_RLF1UES_Pos)
#define WJ_TDM_ISR_RLF1UES                          WJ_TDM_ISR_RLF1UES_Msk

#define WJ_TDM_ISR_RLF1OES_Pos                      (3U)
#define WJ_TDM_ISR_RLF1OES_Msk                      (0x1U << WJ_TDM_ISR_RLF1OES_Pos)
#define WJ_TDM_ISR_RLF1OES                          WJ_TDM_ISR_RLF1OES_Msk

#define WJ_TDM_ISR_RLF1FIS_Pos                      (4U)
#define WJ_TDM_ISR_RLF1FIS_Msk                      (0x1U << WJ_TDM_ISR_RLF1FIS_Pos)
#define WJ_TDM_ISR_RLF1FIS                          WJ_TDM_ISR_RLF1FIS_Msk

#define WJ_TDM_ISR_RRF1UES_Pos                      (5U)
#define WJ_TDM_ISR_RRF1UES_Msk                      (0x1U << WJ_TDM_ISR_RRF1UES_Pos)
#define WJ_TDM_ISR_RRF1UES                          WJ_TDM_ISR_RRF1UES_Msk

#define WJ_TDM_ISR_RRF1OES_Pos                      (6U)
#define WJ_TDM_ISR_RRF1OES_Msk                      (0x1U << WJ_TDM_ISR_RRF1OES_Pos)
#define WJ_TDM_ISR_RRF1OES                          WJ_TDM_ISR_RRF1OES_Msk

#define WJ_TDM_ISR_RRF1FIS_Pos                      (7U)
#define WJ_TDM_ISR_RRF1FIS_Msk                      (0x1U << WJ_TDM_ISR_RRF1FIS_Pos)
#define WJ_TDM_ISR_RRF1FIS                          WJ_TDM_ISR_RRF1FIS_Msk

#define WJ_TDM_ISR_RLF2UES_Pos                      (8U)
#define WJ_TDM_ISR_RLF2UES_Msk                      (0x1U << WJ_TDM_ISR_RLF2UES_Pos)
#define WJ_TDM_ISR_RLF2UES                          WJ_TDM_ISR_RLF2UES_Msk

#define WJ_TDM_ISR_RLF2OES_Pos                      (9U)
#define WJ_TDM_ISR_RLF2OES_Msk                      (0x1U << WJ_TDM_ISR_RLF2OES_Pos)
#define WJ_TDM_ISR_RLF2OES                          WJ_TDM_ISR_RLF2OES_Msk

#define WJ_TDM_ISR_RLF2FIS_Pos                      (10U)
#define WJ_TDM_ISR_RLF2FIS_Msk                      (0x1U << WJ_TDM_ISR_RLF2FIS_Pos)
#define WJ_TDM_ISR_RLF2FIS                          WJ_TDM_ISR_RLF2FIS_Msk

#define WJ_TDM_ISR_RRF2UES_Pos                      (12U)
#define WJ_TDM_ISR_RRF2UES_Msk                      (0x1U << WJ_TDM_ISR_RRF2UES_Pos)
#define WJ_TDM_ISR_RRF2UES                          WJ_TDM_ISR_RRF2UES_Msk

#define WJ_TDM_ISR_RRF2OES_Pos                      (13U)
#define WJ_TDM_ISR_RRF2OES_Msk                      (0x1U << WJ_TDM_ISR_RRF2OES_Pos)
#define WJ_TDM_ISR_RRF2OES                          WJ_TDM_ISR_RRF2OES_Msk

#define WJ_TDM_ISR_RRF2FIS_Pos                      (14U)
#define WJ_TDM_ISR_RRF2FIS_Msk                      (0x1U << WJ_TDM_ISR_RRF2FIS_Pos)
#define WJ_TDM_ISR_RRF2FIS                          WJ_TDM_ISR_RRF2FIS_Msk

#define WJ_TDM_ISR_RLF3UES_Pos                      (16U)
#define WJ_TDM_ISR_RLF3UES_Msk                      (0x1U << WJ_TDM_ISR_RLF3UES_Pos)
#define WJ_TDM_ISR_RLF3UES                          WJ_TDM_ISR_RLF3UES_Msk

#define WJ_TDM_ISR_RLF3OES_Pos                      (17U)
#define WJ_TDM_ISR_RLF3OES_Msk                      (0x1U << WJ_TDM_ISR_RLF3OES_Pos)
#define WJ_TDM_ISR_RLF3OES                          WJ_TDM_ISR_RLF3OES_Msk

#define WJ_TDM_ISR_RLF3FIS_Pos                      (18U)
#define WJ_TDM_ISR_RLF3FIS_Msk                      (0x1U << WJ_TDM_ISR_RLF3FIS_Pos)
#define WJ_TDM_ISR_RLF3FIS                          WJ_TDM_ISR_RLF3FIS_Msk

#define WJ_TDM_ISR_RRF3UES_Pos                      (20U)
#define WJ_TDM_ISR_RRF3UES_Msk                      (0x1U << WJ_TDM_ISR_RRF3UES_Pos)
#define WJ_TDM_ISR_RRF3UES                          WJ_TDM_ISR_RRF3UES_Msk

#define WJ_TDM_ISR_RRF3OES_Pos                      (21U)
#define WJ_TDM_ISR_RRF3OES_Msk                      (0x1U << WJ_TDM_ISR_RRF3OES_Pos)
#define WJ_TDM_ISR_RRF3OES                          WJ_TDM_ISR_RRF3OES_Msk

#define WJ_TDM_ISR_RRF3FIS_Pos                      (22U)
#define WJ_TDM_ISR_RRF3FIS_Msk                      (0x1U << WJ_TDM_ISR_RRF3FIS_Pos)
#define WJ_TDM_ISR_RRF3FIS                          WJ_TDM_ISR_RRF3FIS_Msk

#define WJ_TDM_ISR_RLF4UES_Pos                      (24U)
#define WJ_TDM_ISR_RLF4UES_Msk                      (0x1U << WJ_TDM_ISR_RLF4UES_Pos)
#define WJ_TDM_ISR_RLF4UES                          WJ_TDM_ISR_RLF4UES_Msk

#define WJ_TDM_ISR_RLF4OES_Pos                      (25U)
#define WJ_TDM_ISR_RLF4OES_Msk                      (0x1U << WJ_TDM_ISR_RLF4OES_Pos)
#define WJ_TDM_ISR_RLF4OES                          WJ_TDM_ISR_RLF4OES_Msk

#define WJ_TDM_ISR_RLF4FIS_Pos                      (26U)
#define WJ_TDM_ISR_RLF4FIS_Msk                      (0x1U << WJ_TDM_ISR_RLF4FIS_Pos)
#define WJ_TDM_ISR_RLF4FIS                          WJ_TDM_ISR_RLF4FIS_Msk

#define WJ_TDM_ISR_RRF4UES_Pos                      (28U)
#define WJ_TDM_ISR_RRF4UES_Msk                      (0x1U << WJ_TDM_ISR_RRF4UES_Pos)
#define WJ_TDM_ISR_RRF4UES                          WJ_TDM_ISR_RRF4UES_Msk

#define WJ_TDM_ISR_RRF4OES_Pos                      (29U)
#define WJ_TDM_ISR_RRF4OES_Msk                      (0x1U << WJ_TDM_ISR_RRF4OES_Pos)
#define WJ_TDM_ISR_RRF4OES                          WJ_TDM_ISR_RRF4OES_Msk

#define WJ_TDM_ISR_RRF4FIS_Pos                      (30U)
#define WJ_TDM_ISR_RRF4FIS_Msk                      (0x1U << WJ_TDM_ISR_RRF4FIS_Pos)
#define WJ_TDM_ISR_RRF4FIS                          WJ_TDM_ISR_RRF4FIS_Msk

/* ICR , offset: 0x38 */
#define WJ_TDM_ICR_TDMBIC_Pos                      (0U)
#define WJ_TDM_ICR_TDMBIC_Msk                      (0x1U << WJ_TDM_ICR_TDMBIC_Pos)
#define WJ_TDM_ICR_TDMBIC                          WJ_TDM_ICR_TDMBIC_Msk

#define WJ_TDM_ICR_TDMWAEC_Pos                     (1U)
#define WJ_TDM_ICR_TDMWAEC_Msk                     (0x1U << WJ_TDM_ICR_TDMWAEC_Pos)
#define WJ_TDM_ICR_TDMWAEC                         WJ_TDM_ICR_TDMWAEC_Msk

#define WJ_TDM_ICR_LF1UEC_Pos                      (2U)
#define WJ_TDM_ICR_LF1UEC_Msk                      (0x1U << WJ_TDM_ICR_LF1UEC_Pos)
#define WJ_TDM_ICR_LF1UEC                          WJ_TDM_ICR_LF1UEC_Msk

#define WJ_TDM_ICR_LF1OEC_Pos                      (3U)
#define WJ_TDM_ICR_LF1OEC_Msk                      (0x1U << WJ_TDM_ICR_LF1OEC_Pos)
#define WJ_TDM_ICR_LF1OEC                          WJ_TDM_ICR_LF1OEC_Msk

#define WJ_TDM_ICR_LF1FIC_Pos                      (4U)
#define WJ_TDM_ICR_LF1FIC_Msk                      (0x1U << WJ_TDM_ICR_LF1FIC_Pos)
#define WJ_TDM_ICR_LF1FIC                          WJ_TDM_ICR_LF1FIC_Msk

#define WJ_TDM_ICR_RF1UEC_Pos                      (5U)
#define WJ_TDM_ICR_RF1UEC_Msk                      (0x1U << WJ_TDM_ICR_RF1UEC_Pos)
#define WJ_TDM_ICR_RF1UEC                          WJ_TDM_ICR_RF1UEC_Msk

#define WJ_TDM_ICR_RF1OEC_Pos                      (6U)
#define WJ_TDM_ICR_RF1OEC_Msk                      (0x1U << WJ_TDM_ICR_RF1OEC_Pos)
#define WJ_TDM_ICR_RF1OEC                          WJ_TDM_ICR_RF1OEC_Msk

#define WJ_TDM_ICR_RF1FIC_Pos                      (7U)
#define WJ_TDM_ICR_RF1FIC_Msk                      (0x1U << WJ_TDM_ICR_RF1FIC_Pos)
#define WJ_TDM_ICR_RF1FIC                          WJ_TDM_ICR_RF1FIC_Msk

#define WJ_TDM_ICR_LF2UEC_Pos                      (8U)
#define WJ_TDM_ICR_LF2UEC_Msk                      (0x1U << WJ_TDM_ICR_LF2UEC_Pos)
#define WJ_TDM_ICR_LF2UEC                          WJ_TDM_ICR_LF2UEC_Msk

#define WJ_TDM_ICR_LF2OEC_Pos                      (9U)
#define WJ_TDM_ICR_LF2OEC_Msk                      (0x1U << WJ_TDM_ICR_LF2OEC_Pos)
#define WJ_TDM_ICR_LF2OEC                          WJ_TDM_ICR_LF2OEC_Msk

#define WJ_TDM_ICR_LF2FIC_Pos                      (10U)
#define WJ_TDM_ICR_LF2FIC_Msk                      (0x1U << WJ_TDM_ICR_LF2FIC_Pos)
#define WJ_TDM_ICR_LF2FIC                          WJ_TDM_ICR_LF2FIC_Msk

#define WJ_TDM_ICR_RF2UEC_Pos                      (12U)
#define WJ_TDM_ICR_RF2UEC_Msk                      (0x1U << WJ_TDM_ICR_RF2UEC_Pos)
#define WJ_TDM_ICR_RF2UEC                          WJ_TDM_ICR_RF2UEC_Msk

#define WJ_TDM_ICR_RF2OEC_Pos                      (13U)
#define WJ_TDM_ICR_RF2OEC_Msk                      (0x1U << WJ_TDM_ICR_RF2OEC_Pos)
#define WJ_TDM_ICR_RF2OEC                          WJ_TDM_ICR_RF2OEC_Msk

#define WJ_TDM_ICR_RF2FIC_Pos                      (14U)
#define WJ_TDM_ICR_RF2FIC_Msk                      (0x1U << WJ_TDM_ICR_RF2FIC_Pos)
#define WJ_TDM_ICR_RF2FIC                          WJ_TDM_ICR_RF2FIC_Msk

#define WJ_TDM_ICR_LF3UEC_Pos                      (16U)
#define WJ_TDM_ICR_LF3UEC_Msk                      (0x1U << WJ_TDM_ICR_LF3UEC_Pos)
#define WJ_TDM_ICR_LF3UEC                          WJ_TDM_ICR_LF3UEC_Msk

#define WJ_TDM_ICR_LF3OEC_Pos                      (17U)
#define WJ_TDM_ICR_LF3OEC_Msk                      (0x1U << WJ_TDM_ICR_LF3OEC_Pos)
#define WJ_TDM_ICR_LF3OEC                          WJ_TDM_ICR_LF3OEC_Msk

#define WJ_TDM_ICR_LF3FIC_Pos                      (18U)
#define WJ_TDM_ICR_LF3FIC_Msk                      (0x1U << WJ_TDM_ICR_LF3FIC_Pos)
#define WJ_TDM_ICR_LF3FIC                          WJ_TDM_ICR_LF3FIC_Msk

#define WJ_TDM_ICR_RF3UEC_Pos                      (20U)
#define WJ_TDM_ICR_RF3UEC_Msk                      (0x1U << WJ_TDM_ICR_RF3UEC_Pos)
#define WJ_TDM_ICR_RF3UEC                          WJ_TDM_ICR_RF3UEC_Msk

#define WJ_TDM_ICR_RF3OEC_Pos                      (21U)
#define WJ_TDM_ICR_RF3OEC_Msk                      (0x1U << WJ_TDM_ICR_RF3OEC_Pos)
#define WJ_TDM_ICR_RF3OEC                          WJ_TDM_ICR_RF3OEC_Msk

#define WJ_TDM_ICR_RF3FIC_Pos                      (22U)
#define WJ_TDM_ICR_RF3FIC_Msk                      (0x1U << WJ_TDM_ICR_RF3FIC_Pos)
#define WJ_TDM_ICR_RF3FIC                          WJ_TDM_ICR_RF3FIC_Msk

#define WJ_TDM_ICR_LF4UEC_Pos                      (24U)
#define WJ_TDM_ICR_LF4UEC_Msk                      (0x1U << WJ_TDM_ICR_LF4UEC_Pos)
#define WJ_TDM_ICR_LF4UEC                          WJ_TDM_ICR_LF4UEC_Msk

#define WJ_TDM_ICR_LF4OEC_Pos                      (25U)
#define WJ_TDM_ICR_LF4OEC_Msk                      (0x1U << WJ_TDM_ICR_LF4OEC_Pos)
#define WJ_TDM_ICR_LF4OEC                          WJ_TDM_ICR_LF4OEC_Msk

#define WJ_TDM_ICR_LF4FIC_Pos                      (26U)
#define WJ_TDM_ICR_LF4FIC_Msk                      (0x1U << WJ_TDM_ICR_LF4FIC_Pos)
#define WJ_TDM_ICR_LF4FIC                          WJ_TDM_ICR_LF4FIC_Msk

#define WJ_TDM_ICR_RF4UEC_Pos                      (28U)
#define WJ_TDM_ICR_RF4UEC_Msk                      (0x1U << WJ_TDM_ICR_RF4UEC_Pos)
#define WJ_TDM_ICR_RF4UEC                          WJ_TDM_ICR_RF4UEC_Msk

#define WJ_TDM_ICR_RF4OEC_Pos                      (29U)
#define WJ_TDM_ICR_RF4OEC_Msk                      (0x1U << WJ_TDM_ICR_RF4OEC_Pos)
#define WJ_TDM_ICR_RF4OEC                          WJ_TDM_ICR_RF4OEC_Msk

#define WJ_TDM_ICR_RF4FIC_Pos                      (30U)
#define WJ_TDM_ICR_RF4FIC_Msk                      (0x1U << WJ_TDM_ICR_RF4FIC_Pos)
#define WJ_TDM_ICR_RF4FIC                          WJ_TDM_ICR_RF4FIC_Msk

/* DMAEN , offset: 0x3C */
#define WJ_TDM_DMAEN_Pos                           (0U)
#define WJ_TDM_DMAEN_Msk                           (0x1U << WJ_TDM_DMAEN_Pos)
#define WJ_TDM_DMAEN                               WJ_TDM_DMAEN_Msk

/* DMAEN , offset: 0x40 */
#define WJ_TDM_DMADL_Pos                           (0U)
#define WJ_TDM_DMADL_Msk                           (0x7U << WJ_TDM_DMADL_Pos)
#define WJ_TDM_DMADL                               WJ_TDM_DMADL_Msk

/* DIV0_LEVEL , offset: 0x64 */
#define WJ_TDM_DIV0_Pos                            (0U)
#define WJ_TDM_DIV0_Msk                            (0x1U << WJ_TDM_DIV0_Pos)
#define WJ_TDM_DIV0                                WJ_TDM_DIV0_Msk

typedef struct {
    __IOM uint32_t TDMEN;
    __IOM uint32_t TDM_CTL;
    __IOM uint32_t CHOFFSET1;
    __IOM uint32_t CHOFFSET2;
    __IOM uint32_t CHOFFSET3;
    __IOM uint32_t CHOFFSET4;
    __IOM uint32_t FIFOTL1;
    __IOM uint32_t FIFOTL2;
    __IOM uint32_t FIFOTL3;
    __IOM uint32_t FIFOTL4;
    __IM  uint32_t SR;
    __IOM uint32_t IMR;
    __IOM uint32_t ISR;
    __IOM uint32_t RISR;
    __IOM uint32_t ICR;
    __IOM uint32_t DMAEN;
    __IOM uint32_t DMADL;
    __IOM uint32_t LDR1;
    __IOM uint32_t RDR1;
    __IOM uint32_t LDR2;
    __IOM uint32_t RDR2;
    __IOM uint32_t LDR3;
    __IOM uint32_t RDR3;
    __IOM uint32_t LDR4;
    __IOM uint32_t RDR4;
    __IOM uint32_t DIV0_LEVEL;
} wj_tdm_regs_t;

static inline void wj_tdm_enable(wj_tdm_regs_t *tdm_base)
{
    tdm_base->TDMEN |= WJ_TDM_EN;
}

static inline void wj_tdm_disable(wj_tdm_regs_t *tdm_base)
{
    tdm_base->TDMEN &= ~WJ_TDM_EN;
}

static inline void wj_tdm_master_mode(wj_tdm_regs_t *tdm_base)
{
    tdm_base->TDM_CTL |= WJ_TDM_CTL_MODE_MASTER;
}

static inline void wj_tdm_slave_mode(wj_tdm_regs_t *tdm_base)
{
    tdm_base->TDM_CTL &= ~WJ_TDM_CTL_MODE_MASTER;
}

static inline void wj_tdm_set_16bits_mode(wj_tdm_regs_t *tdm_base)
{
    uint32_t val = 0;
    val = tdm_base->TDM_CTL;
    val &= ~WJ_TDM_CTL_DATAWTH;
    val |= WJ_TDM_CTL_DATAWTH_16BITS;
    tdm_base->TDM_CTL = val;
}

static inline void wj_tdm_set_24bits_mode(wj_tdm_regs_t *tdm_base)
{
    uint32_t val = 0;
    val = tdm_base->TDM_CTL;
    val &= ~WJ_TDM_CTL_DATAWTH;
    val |= WJ_TDM_CTL_DATAWTH_24BITS;
    tdm_base->TDM_CTL = val;
}

static inline void wj_tdm_set_32bits_mode(wj_tdm_regs_t *tdm_base)
{
    uint32_t val = 0;
    val = tdm_base->TDM_CTL;
    val &= ~WJ_TDM_CTL_DATAWTH;
    val |= WJ_TDM_CTL_DATAWTH_32BITS;
    tdm_base->TDM_CTL = val;
}

static inline void wj_tdm_set_ch_num(wj_tdm_regs_t *tdm_base, uint32_t ch_num)
{
    uint32_t val = 0;
    val = tdm_base->TDM_CTL;
    val &= ~WJ_TDM_CTL_CHNUM_Msk;
    val |= ch_num << WJ_TDM_CTL_CHNUM_Pos;
    tdm_base->TDM_CTL = val;
}

static inline void wj_tdm_polarity_left_first(wj_tdm_regs_t *tdm_base)
{
    tdm_base->TDM_CTL &= ~WJ_TDM_CTL_CHORD_RIGHT;
}

static inline void wj_tdm_polarity_right_first(wj_tdm_regs_t *tdm_base)
{
    tdm_base->TDM_CTL |= WJ_TDM_CTL_CHORD_RIGHT;
}

static inline void wj_tdm_valid_data_edge(wj_tdm_regs_t *tdm_base, uint32_t edge)
{
    tdm_base->TDM_CTL &= ~WJ_TDM_CTL_SPEDGE_Msk;
    tdm_base->TDM_CTL |= edge << WJ_TDM_CTL_SPEDGE_Pos;
}

static inline void wj_tdm_set_ch12_choffset(wj_tdm_regs_t *tdm_base, uint32_t offset1, uint32_t offset2)
{
    tdm_base->CHOFFSET1 = (offset1 << WJ_TDM_CHOFFSET1_Pos) | (offset2 << WJ_TDM_CHOFFSET2_Pos);
}

static inline void wj_tdm_set_ch34_choffset(wj_tdm_regs_t *tdm_base, uint32_t offset1, uint32_t offset2)
{
    tdm_base->CHOFFSET2 = (offset1 << WJ_TDM_CHOFFSET3_Pos) | (offset2 << WJ_TDM_CHOFFSET4_Pos);
}

static inline void wj_tdm_set_ch56_choffset(wj_tdm_regs_t *tdm_base, uint32_t offset1, uint32_t offset2)
{
    tdm_base->CHOFFSET3 = (offset1 << WJ_TDM_CHOFFSET5_Pos) | (offset2 << WJ_TDM_CHOFFSET6_Pos);
}

static inline void wj_tdm_set_ch78_choffset(wj_tdm_regs_t *tdm_base, uint32_t offset1, uint32_t offset2)
{
    tdm_base->CHOFFSET4 = (offset1 << WJ_TDM_CHOFFSET7_Pos) | (offset2 << WJ_TDM_CHOFFSET8_Pos);
}

static inline void wj_tdm_set_ch12_fifo_threshold(wj_tdm_regs_t *tdm_base, uint32_t ch1_ths, uint32_t ch2_ths)
{
    tdm_base->FIFOTL1 = (ch1_ths << WJ_TDM_FIFOTL1_LFT1_Pos) | (ch2_ths << WJ_TDM_FIFOTL1_RFT1_Pos);
}

static inline void wj_tdm_set_ch34_fifo_threshold(wj_tdm_regs_t *tdm_base, uint32_t ch1_ths, uint32_t ch2_ths)
{
    tdm_base->FIFOTL2 = (ch1_ths << WJ_TDM_FIFOTL1_LFT2_Pos) | (ch2_ths << WJ_TDM_FIFOTL1_RFT2_Pos);
}

static inline void wj_tdm_set_ch56_fifo_threshold(wj_tdm_regs_t *tdm_base, uint32_t ch1_ths, uint32_t ch2_ths)
{
    tdm_base->FIFOTL3 = (ch1_ths << WJ_TDM_FIFOTL1_LFT3_Pos) | (ch2_ths << WJ_TDM_FIFOTL1_RFT3_Pos);
}

static inline void wj_tdm_set_ch78_fifo_thresold(wj_tdm_regs_t *tdm_base, uint32_t ch1_ths, uint32_t ch2_ths)
{
    tdm_base->FIFOTL4 = (ch1_ths << WJ_TDM_FIFOTL1_LFT4_Pos) | (ch2_ths << WJ_TDM_FIFOTL1_RFT4_Pos);
}

static inline uint32_t wj_tdm_get_current_transfer_status(wj_tdm_regs_t *tdm_base)
{
    return tdm_base->SR;
}

static inline void wj_tdm_set_irq_mask(wj_tdm_regs_t *tdm_base, uint32_t irq_mask)
{
    tdm_base->IMR = irq_mask;
}

static inline uint32_t wj_tdm_get_irq_mask(wj_tdm_regs_t *tdm_base)
{
    return tdm_base->IMR;
}

static inline uint32_t wj_tdm_get_irq_status(wj_tdm_regs_t *tdm_base)
{
    return tdm_base->ISR;
}

static inline uint32_t wj_tdm_get_irq_raw_status(wj_tdm_regs_t *tdm_base)
{
    return tdm_base->RISR;
}

static inline void wj_tdm_clear_irq(wj_tdm_regs_t *tdm_base, uint32_t irq_status)
{
    tdm_base->ICR = irq_status;
}

static inline void wj_tdm_dma_enable(wj_tdm_regs_t *tdm_base)
{
    tdm_base->DMAEN |= WJ_TDM_DMAEN;
}

static inline void wj_tdm_dma_disable(wj_tdm_regs_t *tdm_base)
{
    tdm_base->DMAEN &= ~WJ_TDM_DMAEN;
}

static inline void wj_tdm_set_dma_data_level(wj_tdm_regs_t *tdm_base, uint32_t level)
{
    tdm_base->DMADL = level;
}

static inline uint32_t wj_tdm_get_data_level(wj_tdm_regs_t *tdm_base)
{
    return tdm_base->DMADL;
}

static inline uint32_t wj_tdm_get_ldr1_data(wj_tdm_regs_t *tdm_base)
{
    return tdm_base->LDR1;
}

static inline uint32_t wj_tdm_get_ldr2_data(wj_tdm_regs_t *tdm_base)
{
    return tdm_base->LDR2;
}

static inline uint32_t wj_tdm_get_ldr3_data(wj_tdm_regs_t *tdm_base)
{
    return tdm_base->LDR3;
}

static inline uint32_t wj_tdm_get_ldr4_data(wj_tdm_regs_t *tdm_base)
{
    return tdm_base->LDR4;
}

static inline uint32_t wj_tdm_get_rdr1_data(wj_tdm_regs_t *tdm_base)
{
    return tdm_base->RDR1;
}

static inline uint32_t wj_tdm_get_rdr2_data(wj_tdm_regs_t *tdm_base)
{
    return tdm_base->RDR2;
}

static inline uint32_t wj_tdm_get_rdr3_data(wj_tdm_regs_t *tdm_base)
{
    return tdm_base->RDR3;
}

static inline uint32_t wj_tdm_get_rdr4_data(wj_tdm_regs_t *tdm_base)
{
    return tdm_base->RDR4;
}

static inline void wj_tdm_div0_level(wj_tdm_regs_t *tdm_base, uint32_t div)
{
    tdm_base->DIV0_LEVEL = div;
}

#ifdef __cplusplus
}
#endif

#endif /* _WJ_TDM_LL_H_ */