/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_pwm_ll.h
 * @brief    header file for pwm driver
 *           <In the hardware manual, there are 12 PWM channels in total,
 *           but only 6 channels are actually operable. So only supports
 *           6 channels for signal output and input acquisition.>
 * @version  V1.1
 * @date     23. Sep 2020
 ******************************************************************************/
#ifndef _WJ_PWM_H_
#define _WJ_PWM_H_

#include <stdio.h>
#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! PWM Configure Register, 	offset: 0x00 */
#define WJ_PWM_CONF_CH_Pos(_ch_)                                  (_ch_)
#define WJ_PWM_CONF_CH_Msk(_ch_)                                  ((uint32_t)0x1U << WJ_PWM_CONF_CH_Pos(_ch_))
#define WJ_PWM_CONF_CH_EN(_ch_)                                   WJ_PWM_CONF_CH_Msk(_ch_)

#define WJ_PWM_CONF_IC_Pos(_ch_)                                  (12U + (_ch_))
#define WJ_PWM_CONF_IC_Msk(_ch_)                                  ((uint32_t)0x1U << (WJ_PWM_CONF_IC_Pos(_ch_)))
#define WJ_PWM_CONF_IC_EN(_ch_)                                   WJ_PWM_CONF_IC_Msk(_ch_)

#define WJ_PWM_CONF_OC_0_Pos                                      (18U)
#define WJ_PWM_CONF_OC_0_Msk                                      (0x1U << WJ_PWM_CONF_OC_0_Pos)
#define WJ_PWM_CONF_OC_0_EN                                       WJ_PWM_CONF_OC_0_Msk
#define WJ_PWM_CONF_OC_1_Pos                                      (19U)
#define WJ_PWM_CONF_OC_1_Msk                                      (0x1U << WJ_PWM_CONF_OC_1_Pos)
#define WJ_PWM_CONF_OC_1_EN                                       WJ_PWM_CONF_OC_1_Msk
#define WJ_PWM_CONF_OC_2_Pos                                      (20U)
#define WJ_PWM_CONF_OC_2_Msk                                      (0x1U << WJ_PWM_CONF_OC_2_Pos)
#define WJ_PWM_CONF_OC_2_EN                                       WJ_PWM_CONF_OC_2_Msk

#define WJ_PWM_CONF_DIV_SEL_Pos                                   (24U)
#define WJ_PWM_CONF_DIV_SEL_Msk                                   (0x7U << WJ_PWM_CONF_DIV_SEL_Pos)
#define WJ_PWM_CONF_DIV_SEL_2                                     (0x0U << WJ_PWM_CONF_DIV_SEL_Pos)
#define WJ_PWM_CONF_DIV_SEL_4                                     (0x1U << WJ_PWM_CONF_DIV_SEL_Pos)
#define WJ_PWM_CONF_DIV_SEL_8                                     (0x2U << WJ_PWM_CONF_DIV_SEL_Pos)
#define WJ_PWM_CONF_DIV_SEL_16                                    (0x3U << WJ_PWM_CONF_DIV_SEL_Pos)
#define WJ_PWM_CONF_DIV_SEL_32                                    (0x4U << WJ_PWM_CONF_DIV_SEL_Pos)
#define WJ_PWM_CONF_DIV_SEL_64                                    (0x5U << WJ_PWM_CONF_DIV_SEL_Pos)
#define WJ_PWM_CONF_DIV_SEL_128                                   (0x6U << WJ_PWM_CONF_DIV_SEL_Pos)
#define WJ_PWM_CONF_DIV_SEL_RAV                                   (0x7U << WJ_PWM_CONF_DIV_SEL_Pos)

#define WJ_PWM_CONF_DIV_EN_Pos                                    (27U)
#define WJ_PWM_CONF_DIV_EN_Msk                                    (0x1U << WJ_PWM_CONF_DIV_EN_Pos)
#define WJ_PWM_CONF_DIV_EN                                        WJ_PWM_CONF_DIV_EN_Msk

#define WJ_PWM_CONF_HT_0_Pos                                      (28U)
#define WJ_PWM_CONF_HT_0_Msk                                      (0x1U << WJ_PWM_CONF_HT_0_Pos)
#define WJ_PWM_CONF_HT_0_EN                                       WJ_PWM_CONF_HT_0_Msk

#define WJ_PWM_CONF_HT_1_Pos                                      (29U)
#define WJ_PWM_CONF_HT_1_Msk                                      (0x1U << WJ_PWM_CONF_HT_1_Pos)
#define WJ_PWM_CONF_HT_1_EN                                       WJ_PWM_CONF_HT_1_Msk

#define WJ_PWM_CONF_HT_2_Pos                                      (30U)
#define WJ_PWM_CONF_HT_2_Msk                                      (0x1U << WJ_PWM_CONF_HT_2_Pos)
#define WJ_PWM_CONF_HT_2_EN                                       WJ_PWM_CONF_HT_2_Msk

/*!  PWM Output Inversion and trigger signal Register, 	offset: 0x04 */
#define WJ_PWM_INVTR_INV_Pos                                      (0U)
#define WJ_PWM_INVTR_INV_Msk                                      (0xFFFU << WJ_PWM_INVTR_INV_Pos)
#define WJ_PWM_INVTR_INV_EN                                       WJ_PWM_INVTR_INV_Msk

#define WJ_PWM_INVTR_TR_INV_Pos                                   (12U)
#define WJ_PWM_INVTR_TR_INV_Msk                                   (0x1U << WJ_PWM_INVTR_TR_INV_Pos)
#define WJ_PWM_INVTR_TR_INV_EN                                    WJ_PWM_INVTR_TR_INV_Msk

#define WJ_PWM_INVTR_TR_UP_0_Pos                                  (13U)
#define WJ_PWM_INVTR_TR_UP_0_Msk                                  (0x1U << WJ_PWM_INVTR_TR_UP_0_Pos)
#define WJ_PWM_INVTR_TR_UP_0_EN                                   WJ_PWM_INVTR_TR_UP_0_Msk

#define WJ_PWM_INVTR_TR_DW_0_Pos                                  (14U)
#define WJ_PWM_INVTR_TR_DW_0_Msk                                  (0x1U << WJ_PWM_INVTR_TR_UP_0_Pos)
#define WJ_PWM_INVTR_TR_DW_0_EN                                   WJ_PWM_INVTR_TR_UP_0_Msk

#define WJ_PWM_INVTR_TR_UP_1_Pos                                  (15U)
#define WJ_PWM_INVTR_TR_UP_1_Msk                                  (0x1U << WJ_PWM_INVTR_TR_UP_1_Pos)
#define WJ_PWM_INVTR_TR_UP_1_EN                                   WJ_PWM_INVTR_TR_UP_1_Msk

#define WJ_PWM_INVTR_TR_DW_1_Pos                                  (16U)
#define WJ_PWM_INVTR_TR_DW_1_Msk                                  (0x1U << WJ_PWM_INVTR_TR_UP_1_Pos)
#define WJ_PWM_INVTR_TR_DW_1_EN                                   WJ_PWM_INVTR_TR_UP_1_Msk

#define WJ_PWM_INVTR_TR_UP_2_Pos                                  (17U)
#define WJ_PWM_INVTR_TR_UP_2_Msk                                  (0x1U << WJ_PWM_INVTR_TR_UP_2_Pos)
#define WJ_PWM_INVTR_TR_UP_2_EN                                   WJ_PWM_INVTR_TR_UP_2_Msk

#define WJ_PWM_INVTR_TR_DW_2_Pos                                  (18U)
#define WJ_PWM_INVTR_TR_DW_2_Msk                                  (0x1U << WJ_PWM_INVTR_TR_UP_2_Pos)
#define WJ_PWM_INVTR_TR_DW_2_EN                                   WJ_PWM_INVTR_TR_UP_2_Msk

#define WJ_PWM_INVTR_TR_UP_3_Pos                                  (19U)
#define WJ_PWM_INVTR_TR_UP_3_Msk                                  (0x1U << WJ_PWM_INVTR_TR_UP_3_Pos)
#define WJ_PWM_INVTR_TR_UP_3_EN                                   WJ_PWM_INVTR_TR_UP_3_Msk

#define WJ_PWM_INVTR_TR_DW_3_Pos                                  (20U)
#define WJ_PWM_INVTR_TR_DW_3_Msk                                  (0x1U << WJ_PWM_INVTR_TR_UP_3_Pos)
#define WJ_PWM_INVTR_TR_DW_3_EN                                   WJ_PWM_INVTR_TR_UP_3_Msk

#define WJ_PWM_INVTR_TR_UP_4_Pos                                  (21U)
#define WJ_PWM_INVTR_TR_UP_4_Msk                                  (0x1U << WJ_PWM_INVTR_TR_UP_4_Pos)
#define WJ_PWM_INVTR_TR_UP_4_EN                                   WJ_PWM_INVTR_TR_UP_4_Msk

#define WJ_PWM_INVTR_TR_DW_4_Pos                                  (22U)
#define WJ_PWM_INVTR_TR_DW_4_Msk                                  (0x1U << WJ_PWM_INVTR_TR_UP_4_Pos)
#define WJ_PWM_INVTR_TR_DW_4_EN                                   WJ_PWM_INVTR_TR_UP_4_Msk

#define WJ_PWM_INVTR_TR_UP_5_Pos                                  (23U)
#define WJ_PWM_INVTR_TR_UP_5_Msk                                  (0x1U << WJ_PWM_INVTR_TR_UP_5_Pos)
#define WJ_PWM_INVTR_TR_UP_5_EN                                   WJ_PWM_INVTR_TR_UP_5_Msk

#define WJ_PWM_INVTR_TR_DW_5_Pos                                  (24U)
#define WJ_PWM_INVTR_TR_DW_5_Msk                                  (0x1U << WJ_PWM_INVTR_TR_UP_5_Pos)
#define WJ_PWM_INVTR_TR_DW_5_EN                                   WJ_PWM_INVTR_TR_UP_5_Msk

/*!  PWM  channel 0 and channel 1 trigger compare value, 	offset: 0x08 */
#define WJ_PWM_TRIG_COMP_VAL_0_Pos                                (0U)
#define WJ_PWM_TRIG_COMP_VAL_0_Msk                                (0xFFFFU << WJ_PWM_TRIG_COMP_VAL_0_Pos)
#define WJ_PWM_TRIG_COMP_VAL_0_EN                                 WJ_PWM_TRIG_COMP_VAL_0_Msk

#define WJ_PWM_TRIG_COMP_VAL_1_Pos                                (16U)
#define WJ_PWM_TRIG_COMP_VAL_1_Msk                                (0xFFFFU << WJ_PWM_TRIG_COMP_VAL_1_Pos)
#define WJ_PWM_TRIG_COMP_VAL_1_EN                                 WJ_PWM_TRIG_COMP_VAL_1_Msk

/*!  PWM  channel 2 and channel 3 trigger compare value, 	offset: 0x0c */
#define WJ_PWM_TRIG_COMP_VAL_2_Pos                                (0U)
#define WJ_PWM_TRIG_COMP_VAL_2_Msk                                (0xFFFFU << WJ_PWM_TRIG_COMP_VAL_2_Pos)
#define WJ_PWM_TRIG_COMP_VAL_2_EN                                 WJ_PWM_TRIG_COMP_VAL_2_Msk

#define WJ_PWM_TRIG_COMP_VAL_3_Pos                                (16U)
#define WJ_PWM_TRIG_COMP_VAL_3_Msk                                (0xFFFFU << WJ_PWM_TRIG_COMP_VAL_3_Pos)
#define WJ_PWM_TRIG_COMP_VAL_3_EN                                 WJ_PWM_TRIG_COMP_VAL_3_Msk

/*!  PWM  channel 4 and channel 5 trigger compare value, 	offset: 0x10 */
#define WJ_PWM_TRIG_COMP_VAL_4_Pos                                (0U)
#define WJ_PWM_TRIG_COMP_VAL_4_Msk                                (0xFFFFU << WJ_PWM_TRIG_COMP_VAL_4_Pos)
#define WJ_PWM_TRIG_COMP_VAL_4_EN                                 WJ_PWM_TRIG_COMP_VAL_4_Msk

#define WJ_PWM_TRIG_COMP_VAL_5_Pos                                (16U)
#define WJ_PWM_TRIG_COMP_VAL_5_Msk                                (0xFFFFU << WJ_PWM_TRIG_COMP_VAL_5_Pos)
#define WJ_PWM_TRIG_COMP_VAL_5_EN                                 WJ_PWM_TRIG_COMP_VAL_5_Msk

/*!  PWM  Interrupt enable 1 Register, 	offset: 0x14 */

#define WJ_PWM_INTEN_FAULT_Pos                                      (0U)
#define WJ_PWM_INTEN_FAULT_Msk                                      (0x1U << WJ_PWM_INTEN_FAULT_Pos)
#define WJ_PWM_INTEN_FAULT_EN                                       WJ_PWM_INTEN_FAULT_Msk

#define WJ_PWM_INTEN_0_ZERO_Pos                                     (8U)
#define WJ_PWM_INTEN_0_ZERO_Msk                                     (0x1U << WJ_PWM_INTEN_0_ZERO_Pos)
#define WJ_PWM_INTEN_0_ZERO_EN                                      WJ_PWM_INTEN_0_ZERO_Msk

#define WJ_PWM_INTEN_0_LOAD_Pos                                     (9U)
#define WJ_PWM_INTEN_0_LOAD_Msk                                     (0x1U << WJ_PWM_INTEN_0_LOAD_Pos)
#define WJ_PWM_INTEN_0_LOAD_EN                                      WJ_PWM_INTEN_0_LOAD_Msk

#define WJ_PWM_INTEN_0_CA_UP_Pos                                    (10U)
#define WJ_PWM_INTEN_0_CA_UP_Msk                                    (0x1U << WJ_PWM_INTEN_0_CA_UP_Pos)
#define WJ_PWM_INTEN_0_CA_UP_EN                                     WJ_PWM_INTEN_0_CA_UP_Msk

#define WJ_PWM_INTEN_0_CB_UP_Pos                                    (11U)
#define WJ_PWM_INTEN_0_CB_UP_Msk                                    (0x1U << WJ_PWM_INTEN_0_CB_UP_Pos)
#define WJ_PWM_INTEN_0_CB_UP_EN                                     WJ_PWM_INTEN_0_CB_UP_Msk

#define WJ_PWM_INTEN_0_CA_DW_Pos                                    (12U)
#define WJ_PWM_INTEN_0_CA_DW_Msk                                    (0x1U << WJ_PWM_INTEN_0_CA_DW_Pos)
#define WJ_PWM_INTEN_0_CA_DW_EN                                     WJ_PWM_INTEN_0_CA_DW_Msk

#define WJ_PWM_INTEN_0_CB_DW_Pos                                    (13U)
#define WJ_PWM_INTEN_0_CB_DW_Msk                                    (0x1U << WJ_PWM_INTEN_0_CB_DW_Pos)
#define WJ_PWM_INTEN_0_CB_DW_EN                                     WJ_PWM_INTEN_0_CB_DW_Msk

#define WJ_PWM_INTEN_1_ZERO_Pos                                     (16U)
#define WJ_PWM_INTEN_1_ZERO_Msk                                     (0x1U << WJ_PWM_INTEN_1_ZERO_Pos)
#define WJ_PWM_INTEN_1_ZERO_EN                                      WJ_PWM_INTEN_1_ZERO_Msk

#define WJ_PWM_INTEN_1_LOAD_Pos                                     (17U)
#define WJ_PWM_INTEN_1_LOAD_Msk                                     (0x1U << WJ_PWM_INTEN_1_LOAD_Pos)
#define WJ_PWM_INTEN_1_LOAD_EN                                      WJ_PWM_INTEN_1_LOAD_Msk

#define WJ_PWM_INTEN_1_CA_UP_Pos                                    (18U)
#define WJ_PWM_INTEN_1_CA_UP_Msk                                    (0x1U << WJ_PWM_INTEN_1_CA_UP_Pos)
#define WJ_PWM_INTEN_1_CA_UP_EN                                     WJ_PWM_INTEN_1_CA_UP_Msk

#define WJ_PWM_INTEN_1_CB_UP_Pos                                    (19U)
#define WJ_PWM_INTEN_1_CB_UP_Msk                                    (0x1U << WJ_PWM_INTEN_1_CB_UP_Pos)
#define WJ_PWM_INTEN_1_CB_UP_EN                                     WJ_PWM_INTEN_1_CB_UP_Msk

#define WJ_PWM_INTEN_1_CA_DW_Pos                                    (20U)
#define WJ_PWM_INTEN_1_CA_DW_Msk                                    (0x1U << WJ_PWM_INTEN_1_CA_DW_Pos)
#define WJ_PWM_INTEN_1_CA_DW_EN                                     WJ_PWM_INTEN_1_CA_DW_Msk

#define WJ_PWM_INTEN_1_CB_DW_Pos                                    (21U)
#define WJ_PWM_INTEN_1_CB_DW_Msk                                    (0x1U << WJ_PWM_INTEN_1_CB_DW_Pos)
#define WJ_PWM_INTEN_1_CB_DW_EN                                     WJ_PWM_INTEN_1_CB_DW_Msk

#define WJ_PWM_INTEN_2_ZERO_Pos                                     (24U)
#define WJ_PWM_INTEN_2_ZERO_Msk                                     (0x1U << WJ_PWM_INTEN_2_ZERO_Pos)
#define WJ_PWM_INTEN_2_ZERO_EN                                      WJ_PWM_INTEN_2_ZERO_Msk

#define WJ_PWM_INTEN_2_LOAD_Pos                                     (25U)
#define WJ_PWM_INTEN_2_LOAD_Msk                                     (0x1U << WJ_PWM_INTEN_2_LOAD_Pos)
#define WJ_PWM_INTEN_2_LOAD_EN                                      WJ_PWM_INTEN_2_LOAD_Msk

#define WJ_PWM_INTEN_2_CA_UP_Pos                                    (26U)
#define WJ_PWM_INTEN_2_CA_UP_Msk                                    (0x1U << WJ_PWM_INTEN_2_CA_UP_Pos)
#define WJ_PWM_INTEN_2_CA_UP_EN                                     WJ_PWM_INTEN_2_CA_UP_Msk

#define WJ_PWM_INTEN_2_CB_UP_Pos                                    (27U)
#define WJ_PWM_INTEN_2_CB_UP_Msk                                    (0x1U << WJ_PWM_INTEN_2_CB_UP_Pos)
#define WJ_PWM_INTEN_2_CB_UP_EN                                     WJ_PWM_INTEN_2_CB_UP_Msk

#define WJ_PWM_INTEN_2_CA_DW_Pos                                    (28U)
#define WJ_PWM_INTEN_2_CA_DW_Msk                                    (0x1U << WJ_PWM_INTEN_2_CA_DW_Pos)
#define WJ_PWM_INTEN_2_CA_DW_EN                                     WJ_PWM_INTEN_2_CA_DW_Msk

#define WJ_PWM_INTEN_2_CB_DW_Pos                                    (29U)
#define WJ_PWM_INTEN_2_CB_DW_Msk                                    (0x1U << WJ_PWM_INTEN_2_CB_DW_Pos)
#define WJ_PWM_INTEN_2_CB_DW_EN                                     WJ_PWM_INTEN_2_CB_DW_Msk

/*!  PWM  Interrupt enable 2 Register, 	offset: 0x18 */

#define WJ_PWM_INTEN_3_ZERO_Pos                                     (0U)
#define WJ_PWM_INTEN_3_ZERO_Msk                                     (0x1U << WJ_PWM_INTEN_3_ZERO_Pos)
#define WJ_PWM_INTEN_3_ZERO_EN                                      WJ_PWM_INTEN_3_ZERO_Msk

#define WJ_PWM_INTEN_3_LOAD_Pos                                     (1U)
#define WJ_PWM_INTEN_3_LOAD_Msk                                     (0x1U << WJ_PWM_INTEN_3_LOAD_Pos)
#define WJ_PWM_INTEN_3_LOAD_EN                                      WJ_PWM_INTEN_3_LOAD_Msk

#define WJ_PWM_INTEN_3_CA_UP_Pos                                    (2U)
#define WJ_PWM_INTEN_3_CA_UP_Msk                                    (0x1U << WJ_PWM_INTEN_3_CA_UP_Pos)
#define WJ_PWM_INTEN_3_CA_UP_EN                                     WJ_PWM_INTEN_3_CA_UP_Msk

#define WJ_PWM_INTEN_3_CB_UP_Pos                                    (3U)
#define WJ_PWM_INTEN_3_CB_UP_Msk                                    (0x1U << WJ_PWM_INTEN_3_CB_UP_Pos)
#define WJ_PWM_INTEN_3_CB_UP_EN                                     WJ_PWM_INTEN_3_CB_UP_Msk

#define WJ_PWM_INTEN_3_CA_DW_Pos                                    (4U)
#define WJ_PWM_INTEN_3_CA_DW_Msk                                    (0x1U << WJ_PWM_INTEN_3_CA_DW_Pos)
#define WJ_PWM_INTEN_3_CA_DW_EN                                     WJ_PWM_INTEN_3_CA_DW_Msk

#define WJ_PWM_INTEN_3_CB_DW_Pos                                    (5U)
#define WJ_PWM_INTEN_3_CB_DW_Msk                                    (0x1U << WJ_PWM_INTEN_3_CB_DW_Pos)
#define WJ_PWM_INTEN_3_CB_DW_EN                                     WJ_PWM_INTEN_3_CB_DW_Msk

#define WJ_PWM_INTEN_4_ZERO_Pos                                     (8U)
#define WJ_PWM_INTEN_4_ZERO_Msk                                     (0x1U << WJ_PWM_INTEN_4_ZERO_Pos)
#define WJ_PWM_INTEN_4_ZERO_EN                                      WJ_PWM_INTEN_4_ZERO_Msk

#define WJ_PWM_INTEN_4_LOAD_Pos                                     (9U)
#define WJ_PWM_INTEN_4_LOAD_Msk                                     (0x1U << WJ_PWM_INTEN_4_LOAD_Pos)
#define WJ_PWM_INTEN_4_LOAD_EN                                      WJ_PWM_INTEN_4_LOAD_Msk

#define WJ_PWM_INTEN_4_CA_UP_Pos                                    (10U)
#define WJ_PWM_INTEN_4_CA_UP_Msk                                    (0x1U << WJ_PWM_INTEN_4_CA_UP_Pos)
#define WJ_PWM_INTEN_4_CA_UP_EN                                     WJ_PWM_INTEN_4_CA_UP_Msk

#define WJ_PWM_INTEN_4_CB_UP_Pos                                    (11U)
#define WJ_PWM_INTEN_4_CB_UP_Msk                                    (0x1U << WJ_PWM_INTEN_4_CB_UP_Pos)
#define WJ_PWM_INTEN_4_CB_UP_EN                                     WJ_PWM_INTEN_4_CB_UP_Msk

#define WJ_PWM_INTEN_4_CA_DW_Pos                                    (12U)
#define WJ_PWM_INTEN_4_CA_DW_Msk                                    (0x1U << WJ_PWM_INTEN_4_CA_DW_Pos)
#define WJ_PWM_INTEN_4_CA_DW_EN                                     WJ_PWM_INTEN_4_CA_DW_Msk

#define WJ_PWM_INTEN_4_CB_DW_Pos                                    (13U)
#define WJ_PWM_INTEN_4_CB_DW_Msk                                    (0x1U << WJ_PWM_INTEN_4_CB_DW_Pos)
#define WJ_PWM_INTEN_4_CB_DW_EN                                     WJ_PWM_INTEN_4_CB_DW_Msk

#define WJ_PWM_INTEN_5_ZERO_Pos                                     (16U)
#define WJ_PWM_INTEN_5_ZERO_Msk                                     (0x1U << WJ_PWM_INTEN_5_ZERO_Pos)
#define WJ_PWM_INTEN_5_ZERO_EN                                      WJ_PWM_INTEN_5_ZERO_Msk

#define WJ_PWM_INTEN_5_LOAD_Pos                                     (17U)
#define WJ_PWM_INTEN_5_LOAD_Msk                                     (0x1U << WJ_PWM_INTEN_5_LOAD_Pos)
#define WJ_PWM_INTEN_5_LOAD_EN                                      WJ_PWM_INTEN_5_LOAD_Msk

#define WJ_PWM_INTEN_5_CA_UP_Pos                                    (18U)
#define WJ_PWM_INTEN_5_CA_UP_Msk                                    (0x1U << WJ_PWM_INTEN_5_CA_UP_Pos)
#define WJ_PWM_INTEN_5_CA_UP_EN                                     WJ_PWM_INTEN_5_CA_UP_Msk

#define WJ_PWM_INTEN_5_CB_UP_Pos                                    (19U)
#define WJ_PWM_INTEN_5_CB_UP_Msk                                    (0x1U << WJ_PWM_INTEN_5_CB_UP_Pos)
#define WJ_PWM_INTEN_5_CB_UP_EN                                     WJ_PWM_INTEN_5_CB_UP_Msk

#define WJ_PWM_INTEN_5_CA_DW_Pos                                    (20U)
#define WJ_PWM_INTEN_5_CA_DW_Msk                                    (0x1U << WJ_PWM_INTEN_5_CA_DW_Pos)
#define WJ_PWM_INTEN_5_CA_DW_EN                                     WJ_PWM_INTEN_5_CA_DW_Msk

#define WJ_PWM_INTEN_5_CB_DW_Pos                                    (21U)
#define WJ_PWM_INTEN_5_CB_DW_Msk                                    (0x1U << WJ_PWM_INTEN_5_CB_DW_Pos)
#define WJ_PWM_INTEN_5_CB_DW_EN                                     WJ_PWM_INTEN_5_CB_DW_Msk


/*!  PWM  Raw Interrupt asserted 1 Register, 	offset: 0x20 */

#define WJ_PWM_INTRIS_FAULT_Pos                                      (0U)
#define WJ_PWM_INTRIS_FAULT_Msk                                      (0x1U << WJ_PWM_INTRIS_FAULT_Pos)
#define WJ_PWM_INTRIS_FAULT_EN                                       WJ_PWM_INTRIS_FAULT_Msk

#define WJ_PWM_INTRIS_0_ZERO_Pos                                     (8U)
#define WJ_PWM_INTRIS_0_ZERO_Msk                                     (0x1U << WJ_PWM_INTRIS_0_ZERO_Pos)
#define WJ_PWM_INTRIS_0_ZERO_EN                                      WJ_PWM_INTRIS_0_ZERO_Msk

#define WJ_PWM_INTRIS_0_LOAD_Pos                                     (9U)
#define WJ_PWM_INTRIS_0_LOAD_Msk                                     (0x1U << WJ_PWM_INTRIS_0_LOAD_Pos)
#define WJ_PWM_INTRIS_0_LOAD_EN                                      WJ_PWM_INTRIS_0_LOAD_Msk

#define WJ_PWM_INTRIS_0_CA_UP_Pos                                    (10U)
#define WJ_PWM_INTRIS_0_CA_UP_Msk                                    (0x1U << WJ_PWM_INTRIS_0_CA_UP_Pos)
#define WJ_PWM_INTRIS_0_CA_UP_EN                                     WJ_PWM_INTRIS_0_CA_UP_Msk

#define WJ_PWM_INTRIS_0_CB_UP_Pos                                    (11U)
#define WJ_PWM_INTRIS_0_CB_UP_Msk                                    (0x1U << WJ_PWM_INTRIS_0_CB_UP_Pos)
#define WJ_PWM_INTRIS_0_CB_UP_EN                                     WJ_PWM_INTRIS_0_CB_UP_Msk

#define WJ_PWM_INTRIS_0_CA_DW_Pos                                    (12U)
#define WJ_PWM_INTRIS_0_CA_DW_Msk                                    (0x1U << WJ_PWM_INTRIS_0_CA_DW_Pos)
#define WJ_PWM_INTRIS_0_CA_DW_EN                                     WJ_PWM_INTRIS_0_CA_DW_Msk

#define WJ_PWM_INTRIS_0_CB_DW_Pos                                    (13U)
#define WJ_PWM_INTRIS_0_CB_DW_Msk                                    (0x1U << WJ_PWM_INTRIS_0_CB_DW_Pos)
#define WJ_PWM_INTRIS_0_CB_DW_EN                                     WJ_PWM_INTRIS_0_CB_DW_Msk

#define WJ_PWM_INTRIS_1_ZERO_Pos                                     (16U)
#define WJ_PWM_INTRIS_1_ZERO_Msk                                     (0x1U << WJ_PWM_INTRIS_1_ZERO_Pos)
#define WJ_PWM_INTRIS_1_ZERO_EN                                      WJ_PWM_INTRIS_1_ZERO_Msk

#define WJ_PWM_INTRIS_1_LOAD_Pos                                     (17U)
#define WJ_PWM_INTRIS_1_LOAD_Msk                                     (0x1U << WJ_PWM_INTRIS_1_LOAD_Pos)
#define WJ_PWM_INTRIS_1_LOAD_EN                                      WJ_PWM_INTRIS_1_LOAD_Msk

#define WJ_PWM_INTRIS_1_CA_UP_Pos                                    (18U)
#define WJ_PWM_INTRIS_1_CA_UP_Msk                                    (0x1U << WJ_PWM_INTRIS_1_CA_UP_Pos)
#define WJ_PWM_INTRIS_1_CA_UP_EN                                     WJ_PWM_INTRIS_1_CA_UP_Msk

#define WJ_PWM_INTRIS_1_CB_UP_Pos                                    (19U)
#define WJ_PWM_INTRIS_1_CB_UP_Msk                                    (0x1U << WJ_PWM_INTRIS_1_CB_UP_Pos)
#define WJ_PWM_INTRIS_1_CB_UP_EN                                     WJ_PWM_INTRIS_1_CB_UP_Msk

#define WJ_PWM_INTRIS_1_CA_DW_Pos                                    (20U)
#define WJ_PWM_INTRIS_1_CA_DW_Msk                                    (0x1U << WJ_PWM_INTRIS_1_CA_DW_Pos)
#define WJ_PWM_INTRIS_1_CA_DW_EN                                     WJ_PWM_INTRIS_1_CA_DW_Msk

#define WJ_PWM_INTRIS_1_CB_DW_Pos                                    (21U)
#define WJ_PWM_INTRIS_1_CB_DW_Msk                                    (0x1U << WJ_PWM_INTRIS_1_CB_DW_Pos)
#define WJ_PWM_INTRIS_1_CB_DW_EN                                     WJ_PWM_INTRIS_1_CB_DW_Msk

#define WJ_PWM_INTRIS_2_ZERO_Pos                                     (24U)
#define WJ_PWM_INTRIS_2_ZERO_Msk                                     (0x1U << WJ_PWM_INTRIS_2_ZERO_Pos)
#define WJ_PWM_INTRIS_2_ZERO_EN                                      WJ_PWM_INTRIS_2_ZERO_Msk

#define WJ_PWM_INTRIS_2_LOAD_Pos                                     (25U)
#define WJ_PWM_INTRIS_2_LOAD_Msk                                     (0x1U << WJ_PWM_INTRIS_2_LOAD_Pos)
#define WJ_PWM_INTRIS_2_LOAD_EN                                      WJ_PWM_INTRIS_2_LOAD_Msk

#define WJ_PWM_INTRIS_2_CA_UP_Pos                                    (26U)
#define WJ_PWM_INTRIS_2_CA_UP_Msk                                    (0x1U << WJ_PWM_INTRIS_2_CA_UP_Pos)
#define WJ_PWM_INTRIS_2_CA_UP_EN                                     WJ_PWM_INTRIS_2_CA_UP_Msk

#define WJ_PWM_INTRIS_2_CB_UP_Pos                                    (27U)
#define WJ_PWM_INTRIS_2_CB_UP_Msk                                    (0x1U << WJ_PWM_INTRIS_2_CB_UP_Pos)
#define WJ_PWM_INTRIS_2_CB_UP_EN                                     WJ_PWM_INTRIS_2_CB_UP_Msk

#define WJ_PWM_INTRIS_2_CA_DW_Pos                                    (28U)
#define WJ_PWM_INTRIS_2_CA_DW_Msk                                    (0x1U << WJ_PWM_INTRIS_2_CA_DW_Pos)
#define WJ_PWM_INTRIS_2_CA_DW_EN                                     WJ_PWM_INTRIS_2_CA_DW_Msk

#define WJ_PWM_INTRIS_2_CB_DW_Pos                                    (29U)
#define WJ_PWM_INTRIS_2_CB_DW_Msk                                    (0x1U << WJ_PWM_INTRIS_2_CB_DW_Pos)
#define WJ_PWM_INTRIS_2_CB_DW_EN                                     WJ_PWM_INTRIS_2_CB_DW_Msk

/*!  PWM  Raw Interrupt asserted 2 Register, 	offset: 0x24 */

#define WJ_PWM_INTRIS_3_ZERO_Pos                                     (0U)
#define WJ_PWM_INTRIS_3_ZERO_Msk                                     (0x1U << WJ_PWM_INTRIS_3_ZERO_Pos)
#define WJ_PWM_INTRIS_3_ZERO_EN                                      WJ_PWM_INTRIS_3_ZERO_Msk

#define WJ_PWM_INTRIS_3_LOAD_Pos                                     (1U)
#define WJ_PWM_INTRIS_3_LOAD_Msk                                     (0x1U << WJ_PWM_INTRIS_3_LOAD_Pos)
#define WJ_PWM_INTRIS_3_LOAD_EN                                      WJ_PWM_INTRIS_3_LOAD_Msk

#define WJ_PWM_INTRIS_3_CA_UP_Pos                                    (2U)
#define WJ_PWM_INTRIS_3_CA_UP_Msk                                    (0x1U << WJ_PWM_INTRIS_3_CA_UP_Pos)
#define WJ_PWM_INTRIS_3_CA_UP_EN                                     WJ_PWM_INTRIS_3_CA_UP_Msk

#define WJ_PWM_INTRIS_3_CB_UP_Pos                                    (3U)
#define WJ_PWM_INTRIS_3_CB_UP_Msk                                    (0x1U << WJ_PWM_INTRIS_3_CB_UP_Pos)
#define WJ_PWM_INTRIS_3_CB_UP_EN                                     WJ_PWM_INTRIS_3_CB_UP_Msk

#define WJ_PWM_INTRIS_3_CA_DW_Pos                                    (4U)
#define WJ_PWM_INTRIS_3_CA_DW_Msk                                    (0x1U << WJ_PWM_INTRIS_3_CA_DW_Pos)
#define WJ_PWM_INTRIS_3_CA_DW_EN                                     WJ_PWM_INTRIS_3_CA_DW_Msk

#define WJ_PWM_INTRIS_3_CB_DW_Pos                                    (5U)
#define WJ_PWM_INTRIS_3_CB_DW_Msk                                    (0x1U << WJ_PWM_INTRIS_3_CB_DW_Pos)
#define WJ_PWM_INTRIS_3_CB_DW_EN                                     WJ_PWM_INTRIS_3_CB_DW_Msk

#define WJ_PWM_INTRIS_4_ZERO_Pos                                     (8U)
#define WJ_PWM_INTRIS_4_ZERO_Msk                                     (0x1U << WJ_PWM_INTRIS_4_ZERO_Pos)
#define WJ_PWM_INTRIS_4_ZERO_EN                                      WJ_PWM_INTRIS_4_ZERO_Msk

#define WJ_PWM_INTRIS_4_LOAD_Pos                                     (9U)
#define WJ_PWM_INTRIS_4_LOAD_Msk                                     (0x1U << WJ_PWM_INTRIS_4_LOAD_Pos)
#define WJ_PWM_INTRIS_4_LOAD_EN                                      WJ_PWM_INTRIS_4_LOAD_Msk

#define WJ_PWM_INTRIS_4_CA_UP_Pos                                    (10U)
#define WJ_PWM_INTRIS_4_CA_UP_Msk                                    (0x1U << WJ_PWM_INTRIS_4_CA_UP_Pos)
#define WJ_PWM_INTRIS_4_CA_UP_EN                                     WJ_PWM_INTRIS_4_CA_UP_Msk

#define WJ_PWM_INTRIS_4_CB_UP_Pos                                    (11U)
#define WJ_PWM_INTRIS_4_CB_UP_Msk                                    (0x1U << WJ_PWM_INTRIS_4_CB_UP_Pos)
#define WJ_PWM_INTRIS_4_CB_UP_EN                                     WJ_PWM_INTRIS_4_CB_UP_Msk

#define WJ_PWM_INTRIS_4_CA_DW_Pos                                    (12U)
#define WJ_PWM_INTRIS_4_CA_DW_Msk                                    (0x1U << WJ_PWM_INTRIS_4_CA_DW_Pos)
#define WJ_PWM_INTRIS_4_CA_DW_EN                                     WJ_PWM_INTRIS_4_CA_DW_Msk

#define WJ_PWM_INTRIS_4_CB_DW_Pos                                    (13U)
#define WJ_PWM_INTRIS_4_CB_DW_Msk                                    (0x1U << WJ_PWM_INTRIS_4_CB_DW_Pos)
#define WJ_PWM_INTRIS_4_CB_DW_EN                                     WJ_PWM_INTRIS_4_CB_DW_Msk

#define WJ_PWM_INTRIS_5_ZERO_Pos                                     (16U)
#define WJ_PWM_INTRIS_5_ZERO_Msk                                     (0x1U << WJ_PWM_INTRIS_5_ZERO_Pos)
#define WJ_PWM_INTRIS_5_ZERO_EN                                      WJ_PWM_INTRIS_5_ZERO_Msk

#define WJ_PWM_INTRIS_5_LOAD_Pos                                     (17U)
#define WJ_PWM_INTRIS_5_LOAD_Msk                                     (0x1U << WJ_PWM_INTRIS_5_LOAD_Pos)
#define WJ_PWM_INTRIS_5_LOAD_EN                                      WJ_PWM_INTRIS_5_LOAD_Msk

#define WJ_PWM_INTRIS_5_CA_UP_Pos                                    (18U)
#define WJ_PWM_INTRIS_5_CA_UP_Msk                                    (0x1U << WJ_PWM_INTRIS_5_CA_UP_Pos)
#define WJ_PWM_INTRIS_5_CA_UP_EN                                     WJ_PWM_INTRIS_5_CA_UP_Msk

#define WJ_PWM_INTRIS_5_CB_UP_Pos                                    (19U)
#define WJ_PWM_INTRIS_5_CB_UP_Msk                                    (0x1U << WJ_PWM_INTRIS_5_CB_UP_Pos)
#define WJ_PWM_INTRIS_5_CB_UP_EN                                     WJ_PWM_INTRIS_5_CB_UP_Msk

#define WJ_PWM_INTRIS_5_CA_DW_Pos                                    (20U)
#define WJ_PWM_INTRIS_5_CA_DW_Msk                                    (0x1U << WJ_PWM_INTRIS_5_CA_DW_Pos)
#define WJ_PWM_INTRIS_5_CA_DW_EN                                     WJ_PWM_INTRIS_5_CA_DW_Msk

#define WJ_PWM_INTRIS_5_CB_DW_Pos                                    (21U)
#define WJ_PWM_INTRIS_5_CB_DW_Msk                                    (0x1U << WJ_PWM_INTRIS_5_CB_DW_Pos)
#define WJ_PWM_INTRIS_5_CB_DW_EN                                     WJ_PWM_INTRIS_5_CB_DW_Msk


/*!  PWM  clear Interrupt 1 Register, 	offset: 0x28 */

#define WJ_PWM_INTIC_FAULT_Pos                                      (0U)
#define WJ_PWM_INTIC_FAULT_Msk                                      (0x1U << WJ_PWM_INTIC_FAULT_Pos)
#define WJ_PWM_INTIC_FAULT_EN                                       WJ_PWM_INTIC_FAULT_Msk

#define WJ_PWM_INTIC_0_ZERO_Pos                                     (8U)
#define WJ_PWM_INTIC_0_ZERO_Msk                                     (0x1U << WJ_PWM_INTIC_0_ZERO_Pos)
#define WJ_PWM_INTIC_0_ZERO_EN                                      WJ_PWM_INTIC_0_ZERO_Msk

#define WJ_PWM_INTIC_0_LOAD_Pos                                     (9U)
#define WJ_PWM_INTIC_0_LOAD_Msk                                     (0x1U << WJ_PWM_INTIC_0_LOAD_Pos)
#define WJ_PWM_INTIC_0_LOAD_EN                                      WJ_PWM_INTIC_0_LOAD_Msk

#define WJ_PWM_INTIC_0_CA_UP_Pos                                    (10U)
#define WJ_PWM_INTIC_0_CA_UP_Msk                                    (0x1U << WJ_PWM_INTIC_0_CA_UP_Pos)
#define WJ_PWM_INTIC_0_CA_UP_EN                                     WJ_PWM_INTIC_0_CA_UP_Msk

#define WJ_PWM_INTIC_0_CB_UP_Pos                                    (11U)
#define WJ_PWM_INTIC_0_CB_UP_Msk                                    (0x1U << WJ_PWM_INTIC_0_CB_UP_Pos)
#define WJ_PWM_INTIC_0_CB_UP_EN                                     WJ_PWM_INTIC_0_CB_UP_Msk

#define WJ_PWM_INTIC_0_CA_DW_Pos                                    (12U)
#define WJ_PWM_INTIC_0_CA_DW_Msk                                    (0x1U << WJ_PWM_INTIC_0_CA_DW_Pos)
#define WJ_PWM_INTIC_0_CA_DW_EN                                     WJ_PWM_INTIC_0_CA_DW_Msk

#define WJ_PWM_INTIC_0_CB_DW_Pos                                    (13U)
#define WJ_PWM_INTIC_0_CB_DW_Msk                                    (0x1U << WJ_PWM_INTIC_0_CB_DW_Pos)
#define WJ_PWM_INTIC_0_CB_DW_EN                                     WJ_PWM_INTIC_0_CB_DW_Msk

#define WJ_PWM_INTIC_1_ZERO_Pos                                     (16U)
#define WJ_PWM_INTIC_1_ZERO_Msk                                     (0x1U << WJ_PWM_INTIC_1_ZERO_Pos)
#define WJ_PWM_INTIC_1_ZERO_EN                                      WJ_PWM_INTIC_1_ZERO_Msk

#define WJ_PWM_INTIC_1_LOAD_Pos                                     (17U)
#define WJ_PWM_INTIC_1_LOAD_Msk                                     (0x1U << WJ_PWM_INTIC_1_LOAD_Pos)
#define WJ_PWM_INTIC_1_LOAD_EN                                      WJ_PWM_INTIC_1_LOAD_Msk

#define WJ_PWM_INTIC_1_CA_UP_Pos                                    (18U)
#define WJ_PWM_INTIC_1_CA_UP_Msk                                    (0x1U << WJ_PWM_INTIC_1_CA_UP_Pos)
#define WJ_PWM_INTIC_1_CA_UP_EN                                     WJ_PWM_INTIC_1_CA_UP_Msk

#define WJ_PWM_INTIC_1_CB_UP_Pos                                    (19U)
#define WJ_PWM_INTIC_1_CB_UP_Msk                                    (0x1U << WJ_PWM_INTIC_1_CB_UP_Pos)
#define WJ_PWM_INTIC_1_CB_UP_EN                                     WJ_PWM_INTIC_1_CB_UP_Msk

#define WJ_PWM_INTIC_1_CA_DW_Pos                                    (20U)
#define WJ_PWM_INTIC_1_CA_DW_Msk                                    (0x1U << WJ_PWM_INTIC_1_CA_DW_Pos)
#define WJ_PWM_INTIC_1_CA_DW_EN                                     WJ_PWM_INTIC_1_CA_DW_Msk

#define WJ_PWM_INTIC_1_CB_DW_Pos                                    (21U)
#define WJ_PWM_INTIC_1_CB_DW_Msk                                    (0x1U << WJ_PWM_INTIC_1_CB_DW_Pos)
#define WJ_PWM_INTIC_1_CB_DW_EN                                     WJ_PWM_INTIC_1_CB_DW_Msk

#define WJ_PWM_INTIC_2_ZERO_Pos                                     (24U)
#define WJ_PWM_INTIC_2_ZERO_Msk                                     (0x1U << WJ_PWM_INTIC_2_ZERO_Pos)
#define WJ_PWM_INTIC_2_ZERO_EN                                      WJ_PWM_INTIC_2_ZERO_Msk

#define WJ_PWM_INTIC_2_LOAD_Pos                                     (25U)
#define WJ_PWM_INTIC_2_LOAD_Msk                                     (0x1U << WJ_PWM_INTIC_2_LOAD_Pos)
#define WJ_PWM_INTIC_2_LOAD_EN                                      WJ_PWM_INTIC_2_LOAD_Msk

#define WJ_PWM_INTIC_2_CA_UP_Pos                                    (26U)
#define WJ_PWM_INTIC_2_CA_UP_Msk                                    (0x1U << WJ_PWM_INTIC_2_CA_UP_Pos)
#define WJ_PWM_INTIC_2_CA_UP_EN                                     WJ_PWM_INTIC_2_CA_UP_Msk

#define WJ_PWM_INTIC_2_CB_UP_Pos                                    (27U)
#define WJ_PWM_INTIC_2_CB_UP_Msk                                    (0x1U << WJ_PWM_INTIC_2_CB_UP_Pos)
#define WJ_PWM_INTIC_2_CB_UP_EN                                     WJ_PWM_INTIC_2_CB_UP_Msk

#define WJ_PWM_INTIC_2_CA_DW_Pos                                    (28U)
#define WJ_PWM_INTIC_2_CA_DW_Msk                                    (0x1U << WJ_PWM_INTIC_2_CA_DW_Pos)
#define WJ_PWM_INTIC_2_CA_DW_EN                                     WJ_PWM_INTIC_2_CA_DW_Msk

#define WJ_PWM_INTIC_2_CB_DW_Pos                                    (29U)
#define WJ_PWM_INTIC_2_CB_DW_Msk                                    (0x1U << WJ_PWM_INTIC_2_CB_DW_Pos)
#define WJ_PWM_INTIC_2_CB_DW_EN                                     WJ_PWM_INTIC_2_CB_DW_Msk

/*!  PWM  Clear Interrupt 2 Register, 	offset: 0x2C */

#define WJ_PWM_INTIC_3_ZERO_Pos                                     (0U)
#define WJ_PWM_INTIC_3_ZERO_Msk                                     (0x1U << WJ_PWM_INTIC_3_ZERO_Pos)
#define WJ_PWM_INTIC_3_ZERO_EN                                      WJ_PWM_INTIC_3_ZERO_Msk

#define WJ_PWM_INTIC_3_LOAD_Pos                                     (1U)
#define WJ_PWM_INTIC_3_LOAD_Msk                                     (0x1U << WJ_PWM_INTIC_3_LOAD_Pos)
#define WJ_PWM_INTIC_3_LOAD_EN                                      WJ_PWM_INTIC_3_LOAD_Msk

#define WJ_PWM_INTIC_3_CA_UP_Pos                                    (2U)
#define WJ_PWM_INTIC_3_CA_UP_Msk                                    (0x1U << WJ_PWM_INTIC_3_CA_UP_Pos)
#define WJ_PWM_INTIC_3_CA_UP_EN                                     WJ_PWM_INTIC_3_CA_UP_Msk

#define WJ_PWM_INTIC_3_CB_UP_Pos                                    (3U)
#define WJ_PWM_INTIC_3_CB_UP_Msk                                    (0x1U << WJ_PWM_INTIC_3_CB_UP_Pos)
#define WJ_PWM_INTIC_3_CB_UP_EN                                     WJ_PWM_INTIC_3_CB_UP_Msk

#define WJ_PWM_INTIC_3_CA_DW_Pos                                    (4U)
#define WJ_PWM_INTIC_3_CA_DW_Msk                                    (0x1U << WJ_PWM_INTIC_3_CA_DW_Pos)
#define WJ_PWM_INTIC_3_CA_DW_EN                                     WJ_PWM_INTIC_3_CA_DW_Msk

#define WJ_PWM_INTIC_3_CB_DW_Pos                                    (5U)
#define WJ_PWM_INTIC_3_CB_DW_Msk                                    (0x1U << WJ_PWM_INTIC_3_CB_DW_Pos)
#define WJ_PWM_INTIC_3_CB_DW_EN                                     WJ_PWM_INTIC_3_CB_DW_Msk

#define WJ_PWM_INTIC_4_ZERO_Pos                                     (8U)
#define WJ_PWM_INTIC_4_ZERO_Msk                                     (0x1U << WJ_PWM_INTIC_4_ZERO_Pos)
#define WJ_PWM_INTIC_4_ZERO_EN                                      WJ_PWM_INTIC_4_ZERO_Msk

#define WJ_PWM_INTIC_4_LOAD_Pos                                     (9U)
#define WJ_PWM_INTIC_4_LOAD_Msk                                     (0x1U << WJ_PWM_INTIC_4_LOAD_Pos)
#define WJ_PWM_INTIC_4_LOAD_EN                                      WJ_PWM_INTIC_4_LOAD_Msk

#define WJ_PWM_INTIC_4_CA_UP_Pos                                    (10U)
#define WJ_PWM_INTIC_4_CA_UP_Msk                                    (0x1U << WJ_PWM_INTIC_4_CA_UP_Pos)
#define WJ_PWM_INTIC_4_CA_UP_EN                                     WJ_PWM_INTIC_4_CA_UP_Msk

#define WJ_PWM_INTIC_4_CB_UP_Pos                                    (11U)
#define WJ_PWM_INTIC_4_CB_UP_Msk                                    (0x1U << WJ_PWM_INTIC_4_CB_UP_Pos)
#define WJ_PWM_INTIC_4_CB_UP_EN                                     WJ_PWM_INTIC_4_CB_UP_Msk

#define WJ_PWM_INTIC_4_CA_DW_Pos                                    (12U)
#define WJ_PWM_INTIC_4_CA_DW_Msk                                    (0x1U << WJ_PWM_INTIC_4_CA_DW_Pos)
#define WJ_PWM_INTIC_4_CA_DW_EN                                     WJ_PWM_INTIC_4_CA_DW_Msk

#define WJ_PWM_INTIC_4_CB_DW_Pos                                    (13U)
#define WJ_PWM_INTIC_4_CB_DW_Msk                                    (0x1U << WJ_PWM_INTIC_4_CB_DW_Pos)
#define WJ_PWM_INTIC_4_CB_DW_EN                                     WJ_PWM_INTIC_4_CB_DW_Msk

#define WJ_PWM_INTIC_5_ZERO_Pos                                     (16U)
#define WJ_PWM_INTIC_5_ZERO_Msk                                     (0x1U << WJ_PWM_INTIC_5_ZERO_Pos)
#define WJ_PWM_INTIC_5_ZERO_EN                                      WJ_PWM_INTIC_5_ZERO_Msk

#define WJ_PWM_INTIC_5_LOAD_Pos                                     (17U)
#define WJ_PWM_INTIC_5_LOAD_Msk                                     (0x1U << WJ_PWM_INTIC_5_LOAD_Pos)
#define WJ_PWM_INTIC_5_LOAD_EN                                      WJ_PWM_INTIC_5_LOAD_Msk

#define WJ_PWM_INTIC_5_CA_UP_Pos                                    (18U)
#define WJ_PWM_INTIC_5_CA_UP_Msk                                    (0x1U << WJ_PWM_INTIC_5_CA_UP_Pos)
#define WJ_PWM_INTIC_5_CA_UP_EN                                     WJ_PWM_INTIC_5_CA_UP_Msk

#define WJ_PWM_INTIC_5_CB_UP_Pos                                    (19U)
#define WJ_PWM_INTIC_5_CB_UP_Msk                                    (0x1U << WJ_PWM_INTIC_5_CB_UP_Pos)
#define WJ_PWM_INTIC_5_CB_UP_EN                                     WJ_PWM_INTIC_5_CB_UP_Msk

#define WJ_PWM_INTIC_5_CA_DW_Pos                                    (20U)
#define WJ_PWM_INTIC_5_CA_DW_Msk                                    (0x1U << WJ_PWM_INTIC_5_CA_DW_Pos)
#define WJ_PWM_INTIC_5_CA_DW_EN                                     WJ_PWM_INTIC_5_CA_DW_Msk

#define WJ_PWM_INTIC_5_CB_DW_Pos                                    (21U)
#define WJ_PWM_INTIC_5_CB_DW_Msk                                    (0x1U << WJ_PWM_INTIC_5_CB_DW_Pos)
#define WJ_PWM_INTIC_5_CB_DW_EN                                     WJ_PWM_INTIC_5_CB_DW_Msk


/*!  PWM  Current Set of Interrupt asserted 1 Register, 	offset: 0x2C */

#define WJ_PWM_INTIS_FAULT_Pos                                      (0U)
#define WJ_PWM_INTIS_FAULT_Msk                                      (0x1U << WJ_PWM_INTIS_FAULT_Pos)
#define WJ_PWM_INTIS_FAULT_EN                                       WJ_PWM_INTIS_FAULT_Msk

#define WJ_PWM_INTIS_0_ZERO_Pos                                     (8U)
#define WJ_PWM_INTIS_0_ZERO_Msk                                     (0x1U << WJ_PWM_INTIS_0_ZERO_Pos)
#define WJ_PWM_INTIS_0_ZERO_EN                                      WJ_PWM_INTIS_0_ZERO_Msk

#define WJ_PWM_INTIS_0_LOAD_Pos                                     (9U)
#define WJ_PWM_INTIS_0_LOAD_Msk                                     (0x1U << WJ_PWM_INTIS_0_LOAD_Pos)
#define WJ_PWM_INTIS_0_LOAD_EN                                      WJ_PWM_INTIS_0_LOAD_Msk

#define WJ_PWM_INTIS_0_CA_UP_Pos                                    (10U)
#define WJ_PWM_INTIS_0_CA_UP_Msk                                    (0x1U << WJ_PWM_INTIS_0_CA_UP_Pos)
#define WJ_PWM_INTIS_0_CA_UP_EN                                     WJ_PWM_INTIS_0_CA_UP_Msk

#define WJ_PWM_INTIS_0_CB_UP_Pos                                    (11U)
#define WJ_PWM_INTIS_0_CB_UP_Msk                                    (0x1U << WJ_PWM_INTIS_0_CB_UP_Pos)
#define WJ_PWM_INTIS_0_CB_UP_EN                                     WJ_PWM_INTIS_0_CB_UP_Msk

#define WJ_PWM_INTIS_0_CA_DW_Pos                                    (12U)
#define WJ_PWM_INTIS_0_CA_DW_Msk                                    (0x1U << WJ_PWM_INTIS_0_CA_DW_Pos)
#define WJ_PWM_INTIS_0_CA_DW_EN                                     WJ_PWM_INTIS_0_CA_DW_Msk

#define WJ_PWM_INTIS_0_CB_DW_Pos                                    (13U)
#define WJ_PWM_INTIS_0_CB_DW_Msk                                    (0x1U << WJ_PWM_INTIS_0_CB_DW_Pos)
#define WJ_PWM_INTIS_0_CB_DW_EN                                     WJ_PWM_INTIS_0_CB_DW_Msk

#define WJ_PWM_INTIS_1_ZERO_Pos                                     (16U)
#define WJ_PWM_INTIS_1_ZERO_Msk                                     (0x1U << WJ_PWM_INTIS_1_ZERO_Pos)
#define WJ_PWM_INTIS_1_ZERO_EN                                      WJ_PWM_INTIS_1_ZERO_Msk

#define WJ_PWM_INTIS_1_LOAD_Pos                                     (17U)
#define WJ_PWM_INTIS_1_LOAD_Msk                                     (0x1U << WJ_PWM_INTIS_1_LOAD_Pos)
#define WJ_PWM_INTIS_1_LOAD_EN                                      WJ_PWM_INTIS_1_LOAD_Msk

#define WJ_PWM_INTIS_1_CA_UP_Pos                                    (18U)
#define WJ_PWM_INTIS_1_CA_UP_Msk                                    (0x1U << WJ_PWM_INTIS_1_CA_UP_Pos)
#define WJ_PWM_INTIS_1_CA_UP_EN                                     WJ_PWM_INTIS_1_CA_UP_Msk

#define WJ_PWM_INTIS_1_CB_UP_Pos                                    (19U)
#define WJ_PWM_INTIS_1_CB_UP_Msk                                    (0x1U << WJ_PWM_INTIS_1_CB_UP_Pos)
#define WJ_PWM_INTIS_1_CB_UP_EN                                     WJ_PWM_INTIS_1_CB_UP_Msk

#define WJ_PWM_INTIS_1_CA_DW_Pos                                    (20U)
#define WJ_PWM_INTIS_1_CA_DW_Msk                                    (0x1U << WJ_PWM_INTIS_1_CA_DW_Pos)
#define WJ_PWM_INTIS_1_CA_DW_EN                                     WJ_PWM_INTIS_1_CA_DW_Msk

#define WJ_PWM_INTIS_1_CB_DW_Pos                                    (21U)
#define WJ_PWM_INTIS_1_CB_DW_Msk                                    (0x1U << WJ_PWM_INTIS_1_CB_DW_Pos)
#define WJ_PWM_INTIS_1_CB_DW_EN                                     WJ_PWM_INTIS_1_CB_DW_Msk

#define WJ_PWM_INTIS_2_ZERO_Pos                                     (24U)
#define WJ_PWM_INTIS_2_ZERO_Msk                                     (0x1U << WJ_PWM_INTIS_2_ZERO_Pos)
#define WJ_PWM_INTIS_2_ZERO_EN                                      WJ_PWM_INTIS_2_ZERO_Msk

#define WJ_PWM_INTIS_2_LOAD_Pos                                     (25U)
#define WJ_PWM_INTIS_2_LOAD_Msk                                     (0x1U << WJ_PWM_INTIS_2_LOAD_Pos)
#define WJ_PWM_INTIS_2_LOAD_EN                                      WJ_PWM_INTIS_2_LOAD_Msk

#define WJ_PWM_INTIS_2_CA_UP_Pos                                    (26U)
#define WJ_PWM_INTIS_2_CA_UP_Msk                                    (0x1U << WJ_PWM_INTIS_2_CA_UP_Pos)
#define WJ_PWM_INTIS_2_CA_UP_EN                                     WJ_PWM_INTIS_2_CA_UP_Msk

#define WJ_PWM_INTIS_2_CB_UP_Pos                                    (27U)
#define WJ_PWM_INTIS_2_CB_UP_Msk                                    (0x1U << WJ_PWM_INTIS_2_CB_UP_Pos)
#define WJ_PWM_INTIS_2_CB_UP_EN                                     WJ_PWM_INTIS_2_CB_UP_Msk

#define WJ_PWM_INTIS_2_CA_DW_Pos                                    (28U)
#define WJ_PWM_INTIS_2_CA_DW_Msk                                    (0x1U << WJ_PWM_INTIS_2_CA_DW_Pos)
#define WJ_PWM_INTIS_2_CA_DW_EN                                     WJ_PWM_INTIS_2_CA_DW_Msk

#define WJ_PWM_INTIS_2_CB_DW_Pos                                    (29U)
#define WJ_PWM_INTIS_2_CB_DW_Msk                                    (0x1U << WJ_PWM_INTIS_2_CB_DW_Pos)
#define WJ_PWM_INTIS_2_CB_DW_EN                                     WJ_PWM_INTIS_2_CB_DW_Msk

/*!  PWM  Current Set of Interrupt asserted 2 Register, 	offset: 0x30 */

#define WJ_PWM_INTIS_3_ZERO_Pos                                     (0U)
#define WJ_PWM_INTIS_3_ZERO_Msk                                     (0x1U << WJ_PWM_INTIS_3_ZERO_Pos)
#define WJ_PWM_INTIS_3_ZERO_EN                                      WJ_PWM_INTIS_3_ZERO_Msk

#define WJ_PWM_INTIS_3_LOAD_Pos                                     (1U)
#define WJ_PWM_INTIS_3_LOAD_Msk                                     (0x1U << WJ_PWM_INTIS_3_LOAD_Pos)
#define WJ_PWM_INTIS_3_LOAD_EN                                      WJ_PWM_INTIS_3_LOAD_Msk

#define WJ_PWM_INTIS_3_CA_UP_Pos                                    (2U)
#define WJ_PWM_INTIS_3_CA_UP_Msk                                    (0x1U << WJ_PWM_INTIS_3_CA_UP_Pos)
#define WJ_PWM_INTIS_3_CA_UP_EN                                     WJ_PWM_INTIS_3_CA_UP_Msk

#define WJ_PWM_INTIS_3_CB_UP_Pos                                    (3U)
#define WJ_PWM_INTIS_3_CB_UP_Msk                                    (0x1U << WJ_PWM_INTIS_3_CB_UP_Pos)
#define WJ_PWM_INTIS_3_CB_UP_EN                                     WJ_PWM_INTIS_3_CB_UP_Msk

#define WJ_PWM_INTIS_3_CA_DW_Pos                                    (4U)
#define WJ_PWM_INTIS_3_CA_DW_Msk                                    (0x1U << WJ_PWM_INTIS_3_CA_DW_Pos)
#define WJ_PWM_INTIS_3_CA_DW_EN                                     WJ_PWM_INTIS_3_CA_DW_Msk

#define WJ_PWM_INTIS_3_CB_DW_Pos                                    (5U)
#define WJ_PWM_INTIS_3_CB_DW_Msk                                    (0x1U << WJ_PWM_INTIS_3_CB_DW_Pos)
#define WJ_PWM_INTIS_3_CB_DW_EN                                     WJ_PWM_INTIS_3_CB_DW_Msk

#define WJ_PWM_INTIS_4_ZERO_Pos                                     (8U)
#define WJ_PWM_INTIS_4_ZERO_Msk                                     (0x1U << WJ_PWM_INTIS_4_ZERO_Pos)
#define WJ_PWM_INTIS_4_ZERO_EN                                      WJ_PWM_INTIS_4_ZERO_Msk

#define WJ_PWM_INTIS_4_LOAD_Pos                                     (9U)
#define WJ_PWM_INTIS_4_LOAD_Msk                                     (0x1U << WJ_PWM_INTIS_4_LOAD_Pos)
#define WJ_PWM_INTIS_4_LOAD_EN                                      WJ_PWM_INTIS_4_LOAD_Msk

#define WJ_PWM_INTIS_4_CA_UP_Pos                                    (10U)
#define WJ_PWM_INTIS_4_CA_UP_Msk                                    (0x1U << WJ_PWM_INTIS_4_CA_UP_Pos)
#define WJ_PWM_INTIS_4_CA_UP_EN                                     WJ_PWM_INTIS_4_CA_UP_Msk

#define WJ_PWM_INTIS_4_CB_UP_Pos                                    (11U)
#define WJ_PWM_INTIS_4_CB_UP_Msk                                    (0x1U << WJ_PWM_INTIS_4_CB_UP_Pos)
#define WJ_PWM_INTIS_4_CB_UP_EN                                     WJ_PWM_INTIS_4_CB_UP_Msk

#define WJ_PWM_INTIS_4_CA_DW_Pos                                    (12U)
#define WJ_PWM_INTIS_4_CA_DW_Msk                                    (0x1U << WJ_PWM_INTIS_4_CA_DW_Pos)
#define WJ_PWM_INTIS_4_CA_DW_EN                                     WJ_PWM_INTIS_4_CA_DW_Msk

#define WJ_PWM_INTIS_4_CB_DW_Pos                                    (13U)
#define WJ_PWM_INTIS_4_CB_DW_Msk                                    (0x1U << WJ_PWM_INTIS_4_CB_DW_Pos)
#define WJ_PWM_INTIS_4_CB_DW_EN                                     WJ_PWM_INTIS_4_CB_DW_Msk

#define WJ_PWM_INTIS_5_ZERO_Pos                                     (16U)
#define WJ_PWM_INTIS_5_ZERO_Msk                                     (0x1U << WJ_PWM_INTIS_5_ZERO_Pos)
#define WJ_PWM_INTIS_5_ZERO_EN                                      WJ_PWM_INTIS_5_ZERO_Msk

#define WJ_PWM_INTIS_5_LOAD_Pos                                     (17U)
#define WJ_PWM_INTIS_5_LOAD_Msk                                     (0x1U << WJ_PWM_INTIS_5_LOAD_Pos)
#define WJ_PWM_INTIS_5_LOAD_EN                                      WJ_PWM_INTIS_5_LOAD_Msk

#define WJ_PWM_INTIS_5_CA_UP_Pos                                    (18U)
#define WJ_PWM_INTIS_5_CA_UP_Msk                                    (0x1U << WJ_PWM_INTIS_5_CA_UP_Pos)
#define WJ_PWM_INTIS_5_CA_UP_EN                                     WJ_PWM_INTIS_5_CA_UP_Msk

#define WJ_PWM_INTIS_5_CB_UP_Pos                                    (19U)
#define WJ_PWM_INTIS_5_CB_UP_Msk                                    (0x1U << WJ_PWM_INTIS_5_CB_UP_Pos)
#define WJ_PWM_INTIS_5_CB_UP_EN                                     WJ_PWM_INTIS_5_CB_UP_Msk

#define WJ_PWM_INTIS_5_CA_DW_Pos                                    (20U)
#define WJ_PWM_INTIS_5_CA_DW_Msk                                    (0x1U << WJ_PWM_INTIS_5_CA_DW_Pos)
#define WJ_PWM_INTIS_5_CA_DW_EN                                     WJ_PWM_INTIS_5_CA_DW_Msk

#define WJ_PWM_INTIS_5_CB_DW_Pos                                    (21U)
#define WJ_PWM_INTIS_5_CB_DW_Msk                                    (0x1U << WJ_PWM_INTIS_5_CB_DW_Pos)
#define WJ_PWM_INTIS_5_CB_DW_EN                                     WJ_PWM_INTIS_5_CB_DW_Msk

/*!  PWM  Configure counter mode Register, 	offset: 0x34 */

#define WJ_PWM_CTL_MODE_UP_Pos(_ch_)                                (_ch_)
#define WJ_PWM_CTL_MODE_UP_Msk(_ch_)                                ((uint32_t)0x1U << WJ_PWM_CTL_MODE_UP_Pos(_ch_))
#define WJ_PWM_CTL_MODE_UP_DW(_ch_)                                 WJ_PWM_CTL_MODE_UP_Msk(_ch_)
#define WJ_PWM_CTL_MODE_UP(_ch_)                                    (~WJ_PWM_CTL_MODE_UP_Msk(_ch_))

#define WJ_PWM_CTL_SYNC_0_Pos                                       (6U)
#define WJ_PWM_CTL_SYNC_0_Msk                                       (0x3U << WJ_PWM_CTL_SYNC_0_Pos)
#define WJ_PWM_CTL_SYNC_0_ZERO                                      (0x0U << WJ_PWM_CTL_SYNC_0_Pos)
#define WJ_PWM_CTL_SYNC_0_LOAD                                      (0x1U << WJ_PWM_CTL_SYNC_0_Pos)
#define WJ_PWM_CTL_SYNC_0_ZERO_LOAD                                 (0x2U << WJ_PWM_CTL_SYNC_0_Pos)
#define WJ_PWM_CTL_SYNC_0_NO                                        (0x3U << WJ_PWM_CTL_SYNC_0_Pos)

#define WJ_PWM_CTL_SYNC_1_Pos                                       (8U)
#define WJ_PWM_CTL_SYNC_1_Msk                                       (0x3U << WJ_PWM_CTL_SYNC_1_Pos)
#define WJ_PWM_CTL_SYNC_1_ZERO                                      (0x0U << WJ_PWM_CTL_SYNC_1_Pos)
#define WJ_PWM_CTL_SYNC_1_LOAD                                      (0x1U << WJ_PWM_CTL_SYNC_1_Pos)
#define WJ_PWM_CTL_SYNC_1_ZERO_LOAD                                 (0x2U << WJ_PWM_CTL_SYNC_1_Pos)
#define WJ_PWM_CTL_SYNC_1_NO                                        (0x3U << WJ_PWM_CTL_SYNC_1_Pos)

#define WJ_PWM_CTL_SYNC_2_Pos                                       (10U)
#define WJ_PWM_CTL_SYNC_2_Msk                                       (0x3U << WJ_PWM_CTL_SYNC_2_Pos)
#define WJ_PWM_CTL_SYNC_2_ZERO                                      (0x0U << WJ_PWM_CTL_SYNC_2_Pos)
#define WJ_PWM_CTL_SYNC_2_LOAD                                      (0x1U << WJ_PWM_CTL_SYNC_2_Pos)
#define WJ_PWM_CTL_SYNC_2_ZERO_LOAD                                 (0x2U << WJ_PWM_CTL_SYNC_2_Pos)
#define WJ_PWM_CTL_SYNC_2_NO                                        (0x3U << WJ_PWM_CTL_SYNC_2_Pos)

#define WJ_PWM_CTL_SYNC_3_Pos                                       (12U)
#define WJ_PWM_CTL_SYNC_3_Msk                                       (0x3U << WJ_PWM_CTL_SYNC_3_Pos)
#define WJ_PWM_CTL_SYNC_3_ZERO                                      (0x0U << WJ_PWM_CTL_SYNC_3_Pos)
#define WJ_PWM_CTL_SYNC_3_LOAD                                      (0x1U << WJ_PWM_CTL_SYNC_3_Pos)
#define WJ_PWM_CTL_SYNC_3_ZERO_LOAD                                 (0x2U << WJ_PWM_CTL_SYNC_3_Pos)
#define WJ_PWM_CTL_SYNC_3_NO                                        (0x3U << WJ_PWM_CTL_SYNC_3_Pos)

#define WJ_PWM_CTL_SYNC_4_Pos                                       (14U)
#define WJ_PWM_CTL_SYNC_4_Msk                                       (0x3U << WJ_PWM_CTL_SYNC_4_Pos)
#define WJ_PWM_CTL_SYNC_4_ZERO                                      (0x0U << WJ_PWM_CTL_SYNC_4_Pos)
#define WJ_PWM_CTL_SYNC_4_LOAD                                      (0x1U << WJ_PWM_CTL_SYNC_4_Pos)
#define WJ_PWM_CTL_SYNC_4_ZERO_LOAD                                 (0x2U << WJ_PWM_CTL_SYNC_4_Pos)
#define WJ_PWM_CTL_SYNC_4_NO                                        (0x3U << WJ_PWM_CTL_SYNC_4_Pos)

#define WJ_PWM_CTL_SYNC_5_Pos                                       (16U)
#define WJ_PWM_CTL_SYNC_5_Msk                                       (0x3U << WJ_PWM_CTL_SYNC_5_Pos)
#define WJ_PWM_CTL_SYNC_5_ZERO                                      (0x0U << WJ_PWM_CTL_SYNC_5_Pos)
#define WJ_PWM_CTL_SYNC_5_LOAD                                      (0x1U << WJ_PWM_CTL_SYNC_5_Pos)
#define WJ_PWM_CTL_SYNC_5_ZERO_LOAD                                 (0x2U << WJ_PWM_CTL_SYNC_5_Pos)
#define WJ_PWM_CTL_SYNC_5_NO                                        (0x3U << WJ_PWM_CTL_SYNC_5_Pos)


/*!  PWM load value for the PWM counter Register, 	offset: 0x38 ~ 0x40*/

#define WJ_PWM_COUNTER_LOAD_CH_L_Pos                                        (0U)
#define WJ_PWM_COUNTER_LOAD_CH_L_Msk                                        (0x0000FFFFU)
#define WJ_PWM_COUNTER_LOAD_CH_H_Pos                                        (16U)
#define WJ_PWM_COUNTER_LOAD_CH_H_Msk                                        (0xFFFF0000U)

#define WJ_PWM_COUNTER_LOAD_CH_Pos(_ch_)                                    (((_ch_) % 2U) ? WJ_PWM_COUNTER_LOAD_CH_H_Pos : WJ_PWM_COUNTER_LOAD_CH_L_Pos)
#define WJ_PWM_COUNTER_LOAD_CH_Msk(_ch_)                                    (((_ch_) % 2U) ? WJ_PWM_COUNTER_LOAD_CH_H_Msk : WJ_PWM_COUNTER_LOAD_CH_L_Msk)

/*!  PWM counter 0/1 value for the PWM Register, 	offset: 0x44 */
#define WJ_PWM_COUNTER_COUNT_CH_0_Pos                                        (0U)
#define WJ_PWM_COUNTER_COUNT_CH_0_Msk                                        (0xFFFFU << WJ_PWM_COUNTER_COUNT_CH_0_Pos)

#define WJ_PWM_COUNTER_COUNT_CH_1_Pos                                        (16U)
#define WJ_PWM_COUNTER_COUNT_CH_1_Msk                                        (0xFFFFU << WJ_PWM_COUNTER_COUNT_CH_1_Pos)

/*!  PWM counter 2/3 value for the PWM Register, 	offset: 0x48 */
#define WJ_PWM_COUNTER_COUNT_CH_2_Pos                                        (0U)
#define WJ_PWM_COUNTER_COUNT_CH_2_Msk                                        (0xFFFFU << WJ_PWM_COUNTER_COUNT_CH_2_Pos)

#define WJ_PWM_COUNTER_COUNT_CH_3_Pos                                        (16U)
#define WJ_PWM_COUNTER_COUNT_CH_3_Msk                                        (0xFFFFU << WJ_PWM_COUNTER_COUNT_CH_3_Pos)

/*!  PWM counter 4/5 value for the PWM Register, 	offset: 0x4c */
#define WJ_PWM_COUNTER_COUNT_CH_4_Pos                                        (0U)
#define WJ_PWM_COUNTER_COUNT_CH_4_Msk                                        (0xFFFFU << WJ_PWM_COUNTER_COUNT_CH_4_Pos)

#define WJ_PWM_COUNTER_COUNT_CH_5_Pos                                        (16U)
#define WJ_PWM_COUNTER_COUNT_CH_5_Msk                                        (0xFFFFU << WJ_PWM_COUNTER_COUNT_CH_5_Pos)

/*!  PWM 0 compared against the counter Register, 	offset: 0x50 */
#define WJ_PWM_COUNTER_COMP_CH_0_A_Pos                                       (0U)
#define WJ_PWM_COUNTER_COMP_CH_0_A_Msk                                       (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_0_A_Pos)

#define WJ_PWM_COUNTER_COMP_CH_0_B_Pos                                       (16U)
#define WJ_PWM_COUNTER_COMP_CH_0_B_Msk                                       (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_0_B_Pos)

/*!  PWM 1 compared against the counter Register, 	offset: 0x54 */
#define WJ_PWM_COUNTER_COMP_CH_1_A_Pos                                       (0U)
#define WJ_PWM_COUNTER_COMP_CH_1_A_Msk                                       (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_1_A_Pos)

#define WJ_PWM_COUNTER_COMP_CH_1_B_Pos                                       (16U)
#define WJ_PWM_COUNTER_COMP_CH_1_B_Msk                                       (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_1_B_Pos)

/*!  PWM 2 compared against the counter Register, 	offset: 0x58 */
#define WJ_PWM_COUNTER_COMP_CH_2_A_Pos                                       (0U)
#define WJ_PWM_COUNTER_COMP_CH_2_A_Msk                                       (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_2_A_Pos)

#define WJ_PWM_COUNTER_COMP_CH_2_B_Pos                                       (16U)
#define WJ_PWM_COUNTER_COMP_CH_2_B_Msk                                       (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_2_B_Pos)

/*!  PWM 3 compared against the counter Register, 	offset: 0x5c */
#define WJ_PWM_COUNTER_COMP_CH_3_A_Pos                                       (0U)
#define WJ_PWM_COUNTER_COMP_CH_3_A_Msk                                       (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_3_A_Pos)

#define WJ_PWM_COUNTER_COMP_CH_3_B_Pos                                       (16U)
#define WJ_PWM_COUNTER_COMP_CH_3_B_Msk                                       (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_3_B_Pos)

/*!  PWM 4 compared against the counter Register, 	offset: 0x60 */
#define WJ_PWM_COUNTER_COMP_CH_4_A_Pos                                       (0U)
#define WJ_PWM_COUNTER_COMP_CH_4_A_Msk                                       (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_4_A_Pos)

#define WJ_PWM_COUNTER_COMP_CH_4_B_Pos                                       (16U)
#define WJ_PWM_COUNTER_COMP_CH_4_B_Msk                                       (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_4_B_Pos)

/*!  PWM 5 compared against the counter Register, 	offset: 0x64 */
#define WJ_PWM_COUNTER_COMP_CH_5_A_Pos                                       (0U)
#define WJ_PWM_COUNTER_COMP_CH_5_A_Msk                                       (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_5_A_Pos)

#define WJ_PWM_COUNTER_COMP_CH_5_B_Pos                                       (16U)
#define WJ_PWM_COUNTER_COMP_CH_5_B_Msk                                       (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_5_B_Pos)

#define WJ_PWM_COUNTER_COMP_CH_A_Pos                                         (0U)
#define WJ_PWM_COUNTER_COMP_CH_A_Msk                                         (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_A_Pos)

#define WJ_PWM_COUNTER_COMP_CH_B_Pos                                         (16U)
#define WJ_PWM_COUNTER_COMP_CH_B_Msk                                         (0xFFFFU << WJ_PWM_COUNTER_COMP_CH_B_Pos)

/*!  PWM Set the number of clock ticks to delay Register, 	offset: 0x68 */
#define WJ_PWM_DEADBAND_CH_0_DELAY_Pos                                 (0U)
#define WJ_PWM_DEADBAND_CH_0_DELAY_Msk                                 (0xFFFU << WJ_PWM_DEADBAND_CH_0_DELAY_Pos)

#define WJ_PWM_DEADBAND_CH_1_DELAY_Pos                                 (12U)
#define WJ_PWM_DEADBAND_CH_1_DELAY_Msk                                 (0xFFFU << WJ_PWM_DEADBAND_CH_1_DELAY_Pos)

#define WJ_PWM_DEADBAND_CH_0_EN_Pos                                    (24U)
#define WJ_PWM_DEADBAND_CH_0_EN_Msk                                    (0x1U << WJ_PWM_DEADBAND_CH_0_EN_Pos)
#define WJ_PWM_DEADBAND_CH_0_EN                                        WJ_PWM_DEADBAND_CH_0_EN_Msk

#define WJ_PWM_DEADBAND_CH_1_EN_Pos                                    (25U)
#define WJ_PWM_DEADBAND_CH_1_EN_Msk                                    (0x1U << WJ_PWM_DEADBAND_CH_1_EN_Pos)
#define WJ_PWM_DEADBAND_CH_1_EN                                        WJ_PWM_DEADBAND_CH_1_EN_Msk

/*!  PWM Set the number of clock ticks to delay Register, 	offset: 0x6C */
#define WJ_PWM_DEADBAND_CH_2_DELAY_Pos                                 (0U)
#define WJ_PWM_DEADBAND_CH_2_DELAY_Msk                                 (0xFFFU << WJ_PWM_DEADBAND_CH_2_DELAY_Pos)

#define WJ_PWM_DEADBAND_CH_3_DELAY_Pos                                 (12U)
#define WJ_PWM_DEADBAND_CH_3_DELAY_Msk                                 (0xFFFU << WJ_PWM_DEADBAND_CH_3_DELAY_Pos)

#define WJ_PWM_DEADBAND_CH_2_EN_Pos                                    (24U)
#define WJ_PWM_DEADBAND_CH_2_EN_Msk                                    (0x1U << WJ_PWM_DEADBAND_CH_2_EN_Pos)
#define WJ_PWM_DEADBAND_CH_2_EN                                        WJ_PWM_DEADBAND_CH_2_EN_Msk

#define WJ_PWM_DEADBAND_CH_3_EN_Pos                                    (25U)
#define WJ_PWM_DEADBAND_CH_3_EN_Msk                                    (0x1U << WJ_PWM_DEADBAND_CH_3_EN_Pos)
#define WJ_PWM_DEADBAND_CH_3_EN                                        WJ_PWM_DEADBAND_CH_3_EN_Msk

/*!  PWM Set the number of clock ticks to delay Register, 	offset: 0x70 */
#define WJ_PWM_DEADBAND_CH_4_DELAY_Pos                                 (0U)
#define WJ_PWM_DEADBAND_CH_4_DELAY_Msk                                 (0xFFFU << WJ_PWM_DEADBAND_CH_4_DELAY_Pos)

#define WJ_PWM_DEADBAND_CH_5_DELAY_Pos                                 (12U)
#define WJ_PWM_DEADBAND_CH_5_DELAY_Msk                                 (0xFFFU << WJ_PWM_DEADBAND_CH_5_DELAY_Pos)

#define WJ_PWM_DEADBAND_CH_4_EN_Pos                                    (24U)
#define WJ_PWM_DEADBAND_CH_4_EN_Msk                                    (0x1U << WJ_PWM_DEADBAND_CH_4_EN_Pos)
#define WJ_PWM_DEADBAND_CH_4_EN                                        WJ_PWM_DEADBAND_CH_4_EN_Msk

#define WJ_PWM_DEADBAND_CH_5_EN_Pos                                    (25U)
#define WJ_PWM_DEADBAND_CH_5_EN_Msk                                    (0x1U << WJ_PWM_DEADBAND_CH_5_EN_Pos)
#define WJ_PWM_DEADBAND_CH_5_EN                                        WJ_PWM_DEADBAND_CH_5_EN_Msk


/*!  PWM Configures input capture Register, 	offset: 0x74 */

#define WJ_PWM_CAPCTL_MODE_Pos(_ch_)                                   (_ch_)
#define WJ_PWM_CAPCTL_MODE_Msk(_ch_)                                   ((uint32_t)0x1U << WJ_PWM_CAPCTL_MODE_Pos(_ch_))
#define WJ_PWM_CAPCTL_MODE_TIME(_ch_)                                  (~WJ_PWM_CAPCTL_MODE_Msk(_ch_))
#define WJ_PWM_CAPCTL_MODE_CNT(_ch_)                                   WJ_PWM_CAPCTL_MODE_Msk(_ch_)

#define WJ_PWM_CAPCTL_EVENT_Pos(_ch_)                                  (6U + ((_ch_) << 1U))
#define WJ_PWM_CAPCTL_EVENT_Msk(_ch_)                                  ((uint32_t)0x3U << WJ_PWM_CAPCTL_EVENT_Pos(_ch_))
#define WJ_PWM_CAPCTL_EVENT_POSE(_ch_)                                 ((uint32_t)0x0U << WJ_PWM_CAPCTL_EVENT_Pos(_ch_))
#define WJ_PWM_CAPCTL_EVENT_NEGE(_ch_)                                 ((uint32_t)0x1U << WJ_PWM_CAPCTL_EVENT_Pos(_ch_))
#define WJ_PWM_CAPCTL_EVENT_RAV(_ch_)                                  ((uint32_t)0x2U << WJ_PWM_CAPCTL_EVENT_Pos(_ch_))
#define WJ_PWM_CAPCTL_EVENT_BOTH(_ch_)                                 ((uint32_t)0x3U << WJ_PWM_CAPCTL_EVENT_Pos(_ch_))

/*!  PWM Capture interrupt enable Register, 	offset: 0x78 */

#define WJ_PWM_CAPINT_IE_CNT_Pos(_ch_)                                (_ch_)
#define WJ_PWM_CAPINT_IE_CNT_Msk(_ch_)                                ((uint32_t)0x1U << WJ_PWM_CAPINT_IE_CNT_Pos(_ch_))
#define WJ_PWM_CAPINT_IE_CNT_EN(_ch_)                                 WJ_PWM_CAPINT_IE_CNT_Msk(_ch_)

#define WJ_PWM_CAPINT_IE_TIM_Pos(_ch_)                                (6U + (_ch_))
#define WJ_PWM_CAPINT_IE_TIM_Msk(_ch_)                                ((uint32_t)0x1U << WJ_PWM_CAPINT_IE_TIM_Pos(_ch_))
#define WJ_PWM_CAPINT_IE_TIM_EN(_ch_)                                 WJ_PWM_CAPINT_IE_TIM_Msk(_ch_)

/*!  PWM Capture raw interrupt state Register, 	offset: 0x7C */

#define WJ_PWM_CAPINT_RIS_CNT_Pos(_ch_)                                (_ch_)
#define WJ_PWM_CAPINT_RIS_CNT_Msk(_ch_)                                ((uint32_t)0x1U << WJ_PWM_CAPINT_RIS_CNT_Pos(_ch_))
#define WJ_PWM_CAPINT_RIS_CNT_EN(_ch_)                                 WJ_PWM_CAPINT_RIS_CNT_Msk(_ch_)

#define WJ_PWM_CAPINT_RIS_TIM_Pos(_ch_)                                ((_ch_) + 6U)
#define WJ_PWM_CAPINT_RIS_TIM_Msk(_ch_)                                ((uint32_t)0x1U << WJ_PWM_CAPINT_RIS_TIM_Pos(_ch_))
#define WJ_PWM_CAPINT_RIS_TIM_EN(_ch_)                                 WJ_PWM_CAPINT_RIS_TIM_Msk(_ch_)

/*!  PWM Capture interrupt clear Register, 	offset: 0x80 */

#define WJ_PWM_CAPINT_IC_CNT_Pos(_ch_)                                (_ch_)
#define WJ_PWM_CAPINT_IC_CNT_Msk(_ch_)                                ((uint32_t)0x1U << WJ_PWM_CAPINT_IC_CNT_Pos(_ch_))
#define WJ_PWM_CAPINT_IC_CNT_EN(_ch_)                                 WJ_PWM_CAPINT_IC_CNT_Msk(_ch_)

#define WJ_PWM_CAPINT_IC_TIM_Pos(_ch_)                                ((_ch_) + 6U)
#define WJ_PWM_CAPINT_IC_TIM_Msk(_ch_)                                ((uint32_t)0x1U << WJ_PWM_CAPINT_IC_TIM_Pos(_ch_))
#define WJ_PWM_CAPINT_IC_TIM_EN(_ch_)                                 WJ_PWM_CAPINT_IC_TIM_Msk(_ch_)

/*!  PWM Capture interrupt state Register, 	offset: 0x84 */

#define WJ_PWM_CAPINT_IS_CNT_0_Pos                                    (0U)
#define WJ_PWM_CAPINT_IS_CNT_0_Msk                                    ((uint32_t)0x1U << WJ_PWM_CAPINT_IS_CNT_0_Pos)
#define WJ_PWM_CAPINT_IS_CNT_0_EN                                     WJ_PWM_CAPINT_IS_CNT_0_Msk

#define WJ_PWM_CAPINT_IS_CNT_1_Pos                                    (1U)
#define WJ_PWM_CAPINT_IS_CNT_1_Msk                                    (0x1U << WJ_PWM_CAPINT_IS_CNT_1_Pos)
#define WJ_PWM_CAPINT_IS_CNT_1_EN                                     WJ_PWM_CAPINT_IS_CNT_1_Msk

#define WJ_PWM_CAPINT_IS_CNT_2_Pos                                    (2U)
#define WJ_PWM_CAPINT_IS_CNT_2_Msk                                    (0x1U << WJ_PWM_CAPINT_IS_CNT_2_Pos)
#define WJ_PWM_CAPINT_IS_CNT_2_EN                                     WJ_PWM_CAPINT_IS_CNT_2_Msk

#define WJ_PWM_CAPINT_IS_CNT_3_Pos                                    (3U)
#define WJ_PWM_CAPINT_IS_CNT_3_Msk                                    (0x1U << WJ_PWM_CAPINT_IS_CNT_3_Pos)
#define WJ_PWM_CAPINT_IS_CNT_3_EN                                     WJ_PWM_CAPINT_IS_CNT_3_Msk

#define WJ_PWM_CAPINT_IS_CNT_4_Pos                                    (4U)
#define WJ_PWM_CAPINT_IS_CNT_4_Msk                                    (0x1U << WJ_PWM_CAPINT_IS_CNT_4_Pos)
#define WJ_PWM_CAPINT_IS_CNT_4_EN                                     WJ_PWM_CAPINT_IS_CNT_4_Msk

#define WJ_PWM_CAPINT_IS_CNT_5_Pos                                    (5U)
#define WJ_PWM_CAPINT_IS_CNT_5_Msk                                    (0x1U << WJ_PWM_CAPINT_IS_CNT_5_Pos)
#define WJ_PWM_CAPINT_IS_CNT_5_EN                                     WJ_PWM_CAPINT_IS_CNT_5_Msk

#define WJ_PWM_CAPINT_IS_CNT_Pos                                      (0U)
#define WJ_PWM_CAPINT_IS_CNT_Msk                                      (0x3FU << WJ_PWM_CAPINT_IS_CNT_Pos)

#define WJ_PWM_CAPINT_IS_TIM_0_Pos                                    (6U)
#define WJ_PWM_CAPINT_IS_TIM_0_Msk                                    ((uint32_t)0x1U << WJ_PWM_CAPINT_IS_TIM_0_Pos)
#define WJ_PWM_CAPINT_IS_TIM_0_EN                                     WJ_PWM_CAPINT_IS_TIM_0_Msk

#define WJ_PWM_CAPINT_IS_TIM_1_Pos                                    (7U)
#define WJ_PWM_CAPINT_IS_TIM_1_Msk                                    (0x1U << WJ_PWM_CAPINT_IS_TIM_1_Pos)
#define WJ_PWM_CAPINT_IS_TIM_1_EN                                     WJ_PWM_CAPINT_IS_TIM_1_Msk

#define WJ_PWM_CAPINT_IS_TIM_2_Pos                                    (8U)
#define WJ_PWM_CAPINT_IS_TIM_2_Msk                                    (0x1U << WJ_PWM_CAPINT_IS_TIM_2_Pos)
#define WJ_PWM_CAPINT_IS_TIM_2_EN                                     WJ_PWM_CAPINT_IS_TIM_2_Msk

#define WJ_PWM_CAPINT_IS_TIM_3_Pos                                    (9U)
#define WJ_PWM_CAPINT_IS_TIM_3_Msk                                    (0x1U << WJ_PWM_CAPINT_IS_TIM_3_Pos)
#define WJ_PWM_CAPINT_IS_TIM_3_EN                                     WJ_PWM_CAPINT_IS_TIM_3_Msk

#define WJ_PWM_CAPINT_IS_TIM_4_Pos                                    (10U)
#define WJ_PWM_CAPINT_IS_TIM_4_Msk                                    (0x1U << WJ_PWM_CAPINT_IS_TIM_4_Pos)
#define WJ_PWM_CAPINT_IS_TIM_4_EN                                     WJ_PWM_CAPINT_IS_TIM_4_Msk

#define WJ_PWM_CAPINT_IS_TIM_5_Pos                                    (11U)
#define WJ_PWM_CAPINT_IS_TIM_5_Msk                                    (0x1U << WJ_PWM_CAPINT_IS_TIM_5_Pos)
#define WJ_PWM_CAPINT_IS_TIM_5_EN                                     WJ_PWM_CAPINT_IS_TIM_5_Msk

#define WJ_PWM_CAPINT_IS_TIM_Pos                                      (6U)
#define WJ_PWM_CAPINT_IS_TIM_Msk                                      (0x3FU << WJ_PWM_CAPINT_IS_TIM_Pos)

/*!  PWM counter 0/1 value for the PWM Register, 	offset: 0x88~0x90 */

#define WJ_PWM_CAPTURE_COUNT_CH_L_Pos                                        (0U)
#define WJ_PWM_CAPTURE_COUNT_CH_L_Msk                                        (0x0000FFFFU)
#define WJ_PWM_CAPTURE_COUNT_CH_H_Pos                                        (16U)
#define WJ_PWM_CAPTURE_COUNT_CH_H_Msk                                        (0xFFFF0000U)

#define WJ_PWM_CAPTURE_COUNT_CH_Pos(_ch_)                                    (((_ch_) % 2U) ? WJ_PWM_CAPTURE_COUNT_CH_H_Pos : WJ_PWM_CAPTURE_COUNT_CH_L_Pos)
#define WJ_PWM_CAPTURE_COUNT_CH_Msk(_ch_)                                    (((_ch_) % 2U) ? WJ_PWM_CAPTURE_COUNT_CH_H_Msk : WJ_PWM_CAPTURE_COUNT_CH_L_Msk)

/*!  PWM 0 compared against the counter Register, 	offset: 0x94~0x9c */

#define WJ_PWM_CAPTURE_MATCH_CH_L_Pos                                        (0U)
#define WJ_PWM_CAPTURE_MATCH_CH_L_Msk                                        (0x0000FFFFU)
#define WJ_PWM_CAPTURE_MATCH_CH_H_Pos                                        (16U)
#define WJ_PWM_CAPTURE_MATCH_CH_H_Msk                                        (0xFFFF0000U)

#define WJ_PWM_CAPTURE_MATCH_CH_Pos(_ch_)                                    (((_ch_) % 2U) ? WJ_PWM_CAPTURE_MATCH_CH_H_Pos : WJ_PWM_CAPTURE_MATCH_CH_L_Pos)
#define WJ_PWM_CAPTURE_MATCH_CH_Msk(_ch_)                                    (((_ch_) % 2U) ? WJ_PWM_CAPTURE_MATCH_CH_H_Msk : WJ_PWM_CAPTURE_MATCH_CH_L_Msk)

/*!  PWM time interrupt enable Register, 	offset: 0xa0 */

#define WJ_PWM_TIMINT_IE_0_Pos                                               (0U)
#define WJ_PWM_TIMINT_IE_0_Msk                                               (0x1U << WJ_PWM_TIMINT_IE_0_Pos)
#define WJ_PWM_TIMINT_IE_0_EN                                                WJ_PWM_TIMINT_IE_0_Msk

#define WJ_PWM_TIMINT_IE_1_Pos                                               (1U)
#define WJ_PWM_TIMINT_IE_1_Msk                                               (0x1U << WJ_PWM_TIMINT_IE_1_Pos)
#define WJ_PWM_TIMINT_IE_1_EN                                                WJ_PWM_TIMINT_IE_1_Msk

#define WJ_PWM_TIMINT_IE_2_Pos                                               (2U)
#define WJ_PWM_TIMINT_IE_2_Msk                                               (0x1U << WJ_PWM_TIMINT_IE_2_Pos)
#define WJ_PWM_TIMINT_IE_2_EN                                                WJ_PWM_TIMINT_IE_2_Msk

#define WJ_PWM_TIMINT_IE_3_Pos                                               (3U)
#define WJ_PWM_TIMINT_IE_3_Msk                                               (0x1U << WJ_PWM_TIMINT_IE_3_Pos)
#define WJ_PWM_TIMINT_IE_3_EN                                                WJ_PWM_TIMINT_IE_3_Msk

#define WJ_PWM_TIMINT_IE_4_Pos                                               (4U)
#define WJ_PWM_TIMINT_IE_4_Msk                                               (0x1U << WJ_PWM_TIMINT_IE_4_Pos)
#define WJ_PWM_TIMINT_IE_4_EN                                                WJ_PWM_TIMINT_IE_4_Msk

#define WJ_PWM_TIMINT_IE_5_Pos                                               (5U)
#define WJ_PWM_TIMINT_IE_5_Msk                                               (0x1U << WJ_PWM_TIMINT_IE_5_Pos)
#define WJ_PWM_TIMINT_IE_5_EN                                                WJ_PWM_TIMINT_IE_5_Msk

/*!  PWM time raw interrupt state Register, 	offset: 0xa4 */

#define WJ_PWM_TIMINT_RIS_0_Pos                                             (0U)
#define WJ_PWM_TIMINT_RIS_0_Msk                                             (0x1U << WJ_PWM_TIMINT_RIS_0_Pos)
#define WJ_PWM_TIMINT_RIS_0_EN                                              WJ_PWM_TIMINT_RIS_0_Msk

#define WJ_PWM_TIMINT_RIS_1_Pos                                             (1U)
#define WJ_PWM_TIMINT_RIS_1_Msk                                             (0x1U << WJ_PWM_TIMINT_RIS_1_Pos)
#define WJ_PWM_TIMINT_RIS_1_EN                                              WJ_PWM_TIMINT_RIS_1_Msk

#define WJ_PWM_TIMINT_RIS_2_Pos                                             (2U)
#define WJ_PWM_TIMINT_RIS_2_Msk                                             (0x1U << WJ_PWM_TIMINT_RIS_2_Pos)
#define WJ_PWM_TIMINT_RIS_2_EN                                              WJ_PWM_TIMINT_RIS_2_Msk

#define WJ_PWM_TIMINT_RIS_3_Pos                                             (3U)
#define WJ_PWM_TIMINT_RIS_3_Msk                                             (0x1U << WJ_PWM_TIMINT_RIS_3_Pos)
#define WJ_PWM_TIMINT_RIS_3_EN                                              WJ_PWM_TIMINT_RIS_3_Msk

#define WJ_PWM_TIMINT_RIS_4_Pos                                             (4U)
#define WJ_PWM_TIMINT_RIS_4_Msk                                             (0x1U << WJ_PWM_TIMINT_RIS_4_Pos)
#define WJ_PWM_TIMINT_RIS_4_EN                                              WJ_PWM_TIMINT_RIS_4_Msk

#define WJ_PWM_TIMINT_RIS_5_Pos                                             (5U)
#define WJ_PWM_TIMINT_RIS_5_Msk                                             (0x1U << WJ_PWM_TIMINT_RIS_5_Pos)
#define WJ_PWM_TIMINT_RIS_5_EN                                              WJ_PWM_TIMINT_RIS_5_Msk

/*!  PWM time interrupt clear Register, 	offset: 0xa8 */

#define WJ_PWM_TIMINT_IC_0_Pos                                             (0U)
#define WJ_PWM_TIMINT_IC_0_Msk                                             (0x1U << WJ_PWM_TIMINT_IC_0_Pos)
#define WJ_PWM_TIMINT_IC_0_EN                                              WJ_PWM_TIMINT_IC_0_Msk

#define WJ_PWM_TIMINT_IC_1_Pos                                             (1U)
#define WJ_PWM_TIMINT_IC_1_Msk                                             (0x1U << WJ_PWM_TIMINT_IC_1_Pos)
#define WJ_PWM_TIMINT_IC_1_EN                                              WJ_PWM_TIMINT_IC_1_Msk

#define WJ_PWM_TIMINT_IC_2_Pos                                             (2U)
#define WJ_PWM_TIMINT_IC_2_Msk                                             (0x1U << WJ_PWM_TIMINT_IC_2_Pos)
#define WJ_PWM_TIMINT_IC_2_EN                                              WJ_PWM_TIMINT_IC_2_Msk

#define WJ_PWM_TIMINT_IC_3_Pos                                             (3U)
#define WJ_PWM_TIMINT_IC_3_Msk                                             (0x1U << WJ_PWM_TIMINT_IC_3_Pos)
#define WJ_PWM_TIMINT_IC_3_EN                                              WJ_PWM_TIMINT_IC_3_Msk

#define WJ_PWM_TIMINT_IC_4_Pos                                             (4U)
#define WJ_PWM_TIMINT_IC_4_Msk                                             (0x1U << WJ_PWM_TIMINT_IC_4_Pos)
#define WJ_PWM_TIMINT_IC_4_EN                                              WJ_PWM_TIMINT_IC_4_Msk

#define WJ_PWM_TIMINT_IC_5_Pos                                             (5U)
#define WJ_PWM_TIMINT_IC_5_Msk                                             (0x1U << WJ_PWM_TIMINT_IC_5_Pos)
#define WJ_PWM_TIMINT_IC_5_EN                                              WJ_PWM_TIMINT_IC_5_Msk

/*!  PWM time interrupt state Register, 	offset: 0xac */

#define WJ_PWM_TIMINT_IS_0_Pos                                             (0U)
#define WJ_PWM_TIMINT_IS_0_Msk                                             (0x1U << WJ_PWM_TIMINT_IS_0_Pos)
#define WJ_PWM_TIMINT_IS_0_EN                                              WJ_PWM_TIMINT_IS_0_Msk

#define WJ_PWM_TIMINT_IS_1_Pos                                             (1U)
#define WJ_PWM_TIMINT_IS_1_Msk                                             (0x1U << WJ_PWM_TIMINT_IS_1_Pos)
#define WJ_PWM_TIMINT_IS_1_EN                                              WJ_PWM_TIMINT_IS_1_Msk

#define WJ_PWM_TIMINT_IS_2_Pos                                             (2U)
#define WJ_PWM_TIMINT_IS_2_Msk                                             (0x1U << WJ_PWM_TIMINT_IS_2_Pos)
#define WJ_PWM_TIMINT_IS_2_EN                                              WJ_PWM_TIMINT_IS_2_Msk

#define WJ_PWM_TIMINT_IS_3_Pos                                             (3U)
#define WJ_PWM_TIMINT_IS_3_Msk                                             (0x1U << WJ_PWM_TIMINT_IS_3_Pos)
#define WJ_PWM_TIMINT_IS_3_EN                                              WJ_PWM_TIMINT_IS_3_Msk

#define WJ_PWM_TIMINT_IS_4_Pos                                             (4U)
#define WJ_PWM_TIMINT_IS_4_Msk                                             (0x1U << WJ_PWM_TIMINT_IS_4_Pos)
#define WJ_PWM_TIMINT_IS_4_EN                                              WJ_PWM_TIMINT_IS_4_Msk

#define WJ_PWM_TIMINT_IS_5_Pos                                             (5U)
#define WJ_PWM_TIMINT_IS_5_Msk                                             (0x1U << WJ_PWM_TIMINT_IS_5_Pos)
#define WJ_PWM_TIMINT_IS_5_EN                                              WJ_PWM_TIMINT_IS_5_Msk

/*!  PWM load value for the PWM counter Register, 	offset: 0xb0 */
#define WJ_PWM_TIME_LOAD_CH_0_Pos                                         (0U)
#define WJ_PWM_TIME_LOAD_CH_0_Msk                                         (0xFFFFU << WJ_PWM_TIME_LOAD_CH_0_Pos)

#define WJ_PWM_TIME_LOAD_CH_1_Pos                                         (16U)
#define WJ_PWM_TIME_LOAD_CH_1_Msk                                         (0xFFFFU << WJ_PWM_TIME_LOAD_CH_1_Pos)

/*!  PWM load value for the PWM counter Register, 	offset: 0xb4 */
#define WJ_PWM_TIME_LOAD_CH_2_Pos                                         (0U)
#define WJ_PWM_TIME_LOAD_CH_2_Msk                                         (0xFFFFU << WJ_PWM_TIME_LOAD_CH_2_Pos)

#define WJ_PWM_TIME_LOAD_CH_3_Pos                                         (16U)
#define WJ_PWM_TIME_LOAD_CH_3_Msk                                         (0xFFFFU << WJ_PWM_TIME_LOAD_CH_3_Pos)

/*!  PWM load value for the PWM counter Register, 	offset: 0xb8 */
#define WJ_PWM_TIME_LOAD_CH_4_Pos                                         (0U)
#define WJ_PWM_TIME_LOAD_CH_4_Msk                                         (0xFFFFU << WJ_PWM_TIME_LOAD_CH_4_Pos)

#define WJ_PWM_TIME_LOAD_CH_5_Pos                                         (16U)
#define WJ_PWM_TIME_LOAD_CH_5_Msk                                         (0xFFFFU << WJ_PWM_TIME_LOAD_CH_5_Pos)

/*!  PWM counter 0/1 value for the PWM Register, 	offset: 0xbc */
#define WJ_PWM_TIME_COUNT_CH_0_Pos                                        (0U)
#define WJ_PWM_TIME_COUNT_CH_0_Msk                                        (0xFFFFU << WJ_PWM_TIME_COUNT_CH_0_Pos)

#define WJ_PWM_TIME_COUNT_CH_1_Pos                                        (16U)
#define WJ_PWM_TIME_COUNT_CH_1_Msk                                        (0xFFFFU << WJ_PWM_TIME_COUNT_CH_1_Pos)

/*!  PWM counter 2/3 value for the PWM Register, 	offset: 0xc0 */
#define WJ_PWM_TIME_COUNT_CH_2_Pos                                        (0U)
#define WJ_PWM_TIME_COUNT_CH_2_Msk                                        (0xFFFFU << WJ_PWM_TIME_COUNT_CH_2_Pos)

#define WJ_PWM_TIME_COUNT_CH_3_Pos                                        (16U)
#define WJ_PWM_TIME_COUNT_CH_3_Msk                                        (0xFFFFU << WJ_PWM_TIME_COUNT_CH_3_Pos)

/*!  PWM counter 4/5 value for the PWM Register, 	offset: 0xc4 */
#define WJ_PWM_TIME_COUNT_CH_4_Pos                                        (0U)
#define WJ_PWM_TIME_COUNT_CH_4_Msk                                        (0xFFFFU << WJ_PWM_TIME_COUNT_CH_4_Pos)

#define WJ_PWM_TIME_COUNT_CH_5_Pos                                        (16U)
#define WJ_PWM_TIME_COUNT_CH_5_Msk                                        (0xFFFFU << WJ_PWM_TIME_COUNT_CH_5_Pos)

typedef struct {
    __IOM uint32_t     PWMCFG;              /* Offset: 0x000 (R/W)  PWM configure register */
    __IOM uint32_t     PWMINVERTTRIG;       /* Offset: 0x004 (R/W)  PWM signal is inverted register */
    __IOM uint32_t     PWM01TRIG;           /* Offset: 0x008 (R/W)  contain the trigger generate compare value */
    __IOM uint32_t     PWM23TRIG;           /* Offset: 0x00C (R/W)  contain the trigger generate compare value */
    __IOM uint32_t     PWM45TRIG;           /* Offset: 0x010 (R/W)  contain the trigger generate compare value */
    __IOM uint32_t     PWMINTEN1;           /* Offset: 0x014 (R/W)  interrupt enable */
    __IOM uint32_t     PWMINTEN2;           /* Offset: 0x018 (R/W)  interrupt enable */
    __IM  uint32_t     PWMRIS1;             /* Offset: 0x01C (R/ )  raw interrupt status */
    __IM  uint32_t     PWMRIS2;             /* Offset: 0x020 (R/ )  raw interrupt status */
    __IOM uint32_t     PWMIC1;              /* Offset: 0x024 (R/W)  interrupt clear */
    __IOM uint32_t     PWMIC2;              /* Offset: 0x028 (R/W)  interrupt clear */
    __IM  uint32_t     PWMIS1;              /* Offset: 0x02C (R/ )  interrupt status */
    __IM  uint32_t     PWMIS2;              /* Offset: 0x030 (R/ )  interrupt status */
    __IOM uint32_t     PWMCTL;              /* Offset: 0x034 (R/W)  configure the pwm generation blocks */
    __IOM uint32_t     PWM01LOAD;           /* Offset: 0x038 (R/W)  contain the load value of the PWM count */
    __IOM uint32_t     PWM23LOAD;           /* Offset: 0x03C (R/W)  contain the load value of the PWM count */
    __IOM uint32_t     PWM45LOAD;           /* Offset: 0x040 (R/W)  contain the load value of the PWM count */
    __IM  uint32_t     PWM01COUNT;          /* Offset: 0x044 (R/ )  contain the current value of the PWM count */
    __IM  uint32_t     PWM23COUNT;          /* Offset: 0x048 (R/ )  contain the current value of the PWM count */
    __IM  uint32_t     PWM45COUNT;          /* Offset: 0x04C (R/ )  contain the current value of the PWM count */
    __IOM uint32_t     PWM0CMP;             /* Offset: 0x050 (R/W)  contain a value to be compared against the counter */
    __IOM uint32_t     PWM1CMP;             /* Offset: 0x054 (R/W)  contain a value to be compared against the counter */
    __IOM uint32_t     PWM2CMP;             /* Offset: 0x058 (R/W)  contain a value to be compared against the counter */
    __IOM uint32_t     PWM3CMP;             /* Offset: 0x05C (N/A)  contain a value to be compared against the counter */
    __IOM uint32_t     PWM4CMP;             /* Offset: 0x060 (N/A)  contain a value to be compared against the counter */
    __IOM uint32_t     PWM5CMP;             /* Offset: 0x064 (N/A)  contain a value to be compared against the counter */
    __IOM uint32_t     PWM01DB;             /* Offset: 0x068 (R/W)  contain the number of clock ticks to delay */
    __IOM uint32_t     PWM23DB;             /* Offset: 0x06C (R/W)  contain the number of clock ticks to delay */
    __IOM uint32_t     PWM45DB;             /* Offset: 0x070 (N/A)  contain the number of clock ticks to delay */
    __IOM uint32_t     CAPCTL;              /* Offset: 0x074 (R/W)  input capture control */
    __IOM uint32_t     CAPINTEN;            /* Offset: 0x078 (R/W)  input capture interrupt enable */
    __IM  uint32_t     CAPRIS;              /* Offset: 0x07C (R/ )  input capture raw interrupt status */
    __IOM uint32_t     CAPIC;               /* Offset: 0x080 (R/W)  input capture interrupt clear */
    __IM  uint32_t     CAPIS;               /* Offset: 0x084 (R/ )  input capture interrupt status */
    __IM  uint32_t     CAP01T;              /* Offset: 0x088 (R/ )  input capture count value  */
    __IM  uint32_t     CAP23T;              /* Offset: 0x08C (R/ )  input capture count value  */
    __IM  uint32_t     CAP45T;              /* Offset: 0x090 (R/ )  input capture count value  */
    __IOM uint32_t     CAP01MATCH;          /* Offset: 0x094 (R/W)  input capture match value  */
    __IOM uint32_t     CAP23MATCH;          /* Offset: 0x098 (R/W)  input capture match value  */
    __IOM uint32_t     CAP45MATCH;          /* Offset: 0x09C (N/A)  input capture match value  */
    __IOM uint32_t     TIMINTEN;            /* Offset: 0x0A0 (R/W)  time interrupt enable */
    __IM  uint32_t     TIMRIS;              /* Offset: 0x0A4 (R/ )  time raw interrupt stats */
    __IOM uint32_t     TIMIC;               /* Offset: 0x0A8 (R/W)  time interrupt clear */
    __IM  uint32_t     TIMIS;               /* Offset: 0x0AC (R/ )  time interrupt status */
    __IOM uint32_t     TIM01LOAD;           /* Offset: 0x0B0 (R/W)  time load value */
    __IOM uint32_t     TIM23LOAD;           /* Offset: 0x0B4 (R/W)  time load value */
    __IOM uint32_t     TIM45LOAD;           /* Offset: 0x0B8 (R/W)  time load value */
    __IOM uint32_t     TIM01COUNT;          /* Offset: 0x0BC (R/W)  time current count time */
    __IOM uint32_t     TIM23COUNT;          /* Offset: 0x0C0 (R/W)  time current count time */
    __IOM uint32_t     TIM45COUNT;          /* Offset: 0x0C4 (R/W)  time current count time */
} wj_pwm_regs_t;

static inline void wj_pwm_reset_register(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG = 0U;
    pwm_base->PWMINVERTTRIG = 0U;
    pwm_base->PWM01TRIG = 0U;
    pwm_base->PWM23TRIG = 0U;
    pwm_base->PWM45TRIG = 0U;
    pwm_base->PWMINTEN1 = 0U;
    pwm_base->PWMINTEN2 = 0U;
    pwm_base->PWMIC1 = 0U;
    pwm_base->PWMIC2 = 0U;
    pwm_base->PWMCTL = 0U;
    pwm_base->PWM01LOAD = 0U;
    pwm_base->PWM23LOAD = 0U;
    pwm_base->PWM45LOAD = 0U;
    pwm_base->PWM0CMP = 0U;
    pwm_base->PWM1CMP = 0U;
    pwm_base->PWM2CMP = 0U;
    pwm_base->PWM3CMP = 0U;
    pwm_base->PWM4CMP = 0U;
    pwm_base->PWM5CMP = 0U;
    pwm_base->PWM01DB = 0U;
    pwm_base->PWM23DB = 0U;
    pwm_base->PWM45DB = 0U;
    pwm_base->CAPCTL = 0U;
    pwm_base->CAPINTEN = 0U;
    pwm_base->CAPIC = 0U;
    pwm_base->CAP01MATCH = 0U;
    pwm_base->CAP23MATCH = 0U;
    pwm_base->CAP45MATCH = 0U;
    pwm_base->TIMINTEN = 0U;
    pwm_base->TIMIC = 0U;
    pwm_base->TIM01LOAD = 0U;
    pwm_base->TIM23LOAD = 0U;
    pwm_base->TIM45LOAD = 0U;
    pwm_base->TIM01COUNT = 0U;
    pwm_base->TIM23COUNT = 0U;
    pwm_base->TIM45COUNT = 0U;
}

/**
 * \brief   Channel PWM configuration
*/
static inline void wj_pwm_config_output_ch_en(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->PWMCFG |= WJ_PWM_CONF_CH_EN(ch);
}
static inline void wj_pwm_config_output_ch_dis(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_CH_EN(ch);
}

static inline void wj_pwm_config_output_en(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    wj_pwm_config_output_ch_en(pwm_base, (ch << 1U));
    wj_pwm_config_output_ch_en(pwm_base, (ch << 1U) + 1U);
}
static inline void wj_pwm_config_output_dis(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    wj_pwm_config_output_ch_dis(pwm_base, (ch << 1U));
    wj_pwm_config_output_ch_dis(pwm_base, (ch << 1U) + 1U);
}

/**
 * \brief   Channel input capture configuration
*/
static inline void wj_pwm_config_input_capture_ch_en(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->PWMCFG |= WJ_PWM_CONF_IC_EN(ch);
}
static inline void wj_pwm_config_input_capture_ch_dis(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_IC_EN(ch);
}

/**
 * \brief   Channel 0 output compare configuration
*/
static inline void wj_pwm_config_output_compare_ch_0_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG |= WJ_PWM_CONF_OC_0_EN;
}
static inline void wj_pwm_config_output_compare_ch_0_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_OC_0_EN;
}
static inline uint32_t wj_pwm_config_get_output_compare_ch_0(wj_pwm_regs_t *pwm_base)
{
    return ((pwm_base->PWMCFG) & WJ_PWM_CONF_OC_0_Msk) >> WJ_PWM_CONF_OC_0_Pos;
}
/**
 * \brief   Channel 1 output compare configuration
*/
static inline void wj_pwm_config_output_compare_ch_1_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG |= WJ_PWM_CONF_OC_1_EN;
}
static inline void wj_pwm_config_output_compare_ch_1_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_OC_1_EN;
}
static inline uint32_t wj_pwm_config_get_output_compare_ch_1(wj_pwm_regs_t *pwm_base)
{
    return ((pwm_base->PWMCFG) & WJ_PWM_CONF_OC_1_Msk) >> WJ_PWM_CONF_OC_1_Pos;
}
/**
 * \brief   Channel 2 output compare configuration
*/
static inline void wj_pwm_config_output_compare_ch_2_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG |= WJ_PWM_CONF_OC_2_EN;
}
static inline void wj_pwm_config_output_compare_ch_2_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_OC_2_EN;
}
static inline uint32_t wj_pwm_config_get_output_compare_ch_2(wj_pwm_regs_t *pwm_base)
{
    return ((pwm_base->PWMCFG) & WJ_PWM_CONF_OC_2_Msk) >> WJ_PWM_CONF_OC_2_Pos;
}

/**
 * \brief   Counter frequency division selection
 *          111 : divide by 128
 *          110 : divide by 128
 *          101 : divide by 64
 *          100 : divide by 32
 *          011 : divide by 16
 *          010 : divide by 8
 *          001 : divide by 4
 *          000 : divide by 2
*/
static inline void wj_pwm_config_frq_div_sel_2(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_DIV_SEL_Msk;
    pwm_base->PWMCFG |= WJ_PWM_CONF_DIV_SEL_2;
}
static inline void wj_pwm_config_frq_div_sel_4(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_DIV_SEL_Msk;
    pwm_base->PWMCFG |= WJ_PWM_CONF_DIV_SEL_4;
}
static inline void wj_pwm_config_frq_div_sel_8(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_DIV_SEL_Msk;
    pwm_base->PWMCFG |= WJ_PWM_CONF_DIV_SEL_8;
}
static inline void wj_pwm_config_frq_div_sel_16(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_DIV_SEL_Msk;
    pwm_base->PWMCFG |= WJ_PWM_CONF_DIV_SEL_16;
}
static inline void wj_pwm_config_frq_div_sel_32(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_DIV_SEL_Msk;
    pwm_base->PWMCFG |= WJ_PWM_CONF_DIV_SEL_32;
}
static inline void wj_pwm_config_frq_div_sel_64(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_DIV_SEL_Msk;
    pwm_base->PWMCFG |= WJ_PWM_CONF_DIV_SEL_64;
}
static inline void wj_pwm_config_frq_div_sel_128(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_DIV_SEL_Msk;
    pwm_base->PWMCFG |= WJ_PWM_CONF_DIV_SEL_128;
}
static inline void wj_pwm_config_frq_div_select(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_DIV_SEL_Msk;
    pwm_base->PWMCFG |= (value << WJ_PWM_CONF_DIV_SEL_Pos);
}
static inline uint32_t wj_pwm_config_get_frq_div_sel(wj_pwm_regs_t *pwm_base)
{
    return ((pwm_base->PWMCFG) & WJ_PWM_CONF_DIV_SEL_Msk) >> WJ_PWM_CONF_DIV_SEL_Pos;
}

/**
 * \brief   Counter frequency division configuration
 *          1 : frequency division enable
 *          0 : frequency division disable, so the counter clock source is system clock
*/
static inline void wj_pwm_config_frq_div_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG |= WJ_PWM_CONF_DIV_EN;
}
static inline void wj_pwm_config_frq_div_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_DIV_EN;
}
static inline uint32_t wj_pwm_config_get_frq_div(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMCFG) & WJ_PWM_CONF_DIV_EN_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Count 0 hardware trigger configuration
 *          1 : hardware trigger enable
 *          0 : hardware trigger disable
*/
static inline void wj_pwm_config_hw_trigger_0_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG |= WJ_PWM_CONF_HT_0_EN;
}
static inline void wj_pwm_config_hw_trigger_0_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_HT_0_EN;
}
static inline uint32_t wj_pwm_config_get_hw_trigger_0(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMCFG) & WJ_PWM_CONF_HT_0_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Count 1 hardware trigger configuration
 *          1 : hardware trigger enable
 *          0 : hardware trigger disable
*/
static inline void wj_pwm_config_hw_trigger_1_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG |= WJ_PWM_CONF_HT_1_EN;
}
static inline void wj_pwm_config_hw_trigger_1_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_HT_1_EN;
}
static inline uint32_t wj_pwm_config_get_hw_trigger_1(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMCFG) & WJ_PWM_CONF_HT_1_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Count 2 hardware trigger configuration
 *          1 : hardware trigger enable
 *          0 : hardware trigger disable
*/
static inline void wj_pwm_config_hw_trigger_2_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG |= WJ_PWM_CONF_HT_2_EN;
}
static inline void wj_pwm_config_hw_trigger_2_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCFG &= ~WJ_PWM_CONF_HT_2_EN;
}
static inline uint32_t wj_pwm_config_get_hw_trigger_2(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMCFG) & WJ_PWM_CONF_HT_2_Msk) ? (uint32_t)1 : (uint32_t)0);
}


/**
 * \brief   Invert PWM signal
 * \param   msk PWM channel select(up to 11 channels) \ref wj_pwm_channel_t
 *          1 : PWM signal is inverted
 *          0 : PWM signal is not inverted
*/
static inline void wj_pwm_invtr_invert_set(wj_pwm_regs_t *pwm_base, uint32_t msk)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_INV_Msk;
    pwm_base->PWMINVERTTRIG |= msk;
}
static inline void wj_pwm_invtr_invert_clr(wj_pwm_regs_t *pwm_base, uint32_t msk)
{
    pwm_base->PWMINVERTTRIG &= ~msk;
}
static inline uint32_t wj_pwm_invtr_get_invert(wj_pwm_regs_t *pwm_base)
{
    return ((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_INV_Msk) >> WJ_PWM_INVTR_INV_Pos;
}

/**
 * \brief   Invert PWM trigger signal
 *          1 : PWM trigger signal is inverted
 *          0 : PWM trigger signal is not inverted
*/
static inline void wj_pwm_invtr_trigger_invert_set(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG |= WJ_PWM_INVTR_TR_INV_EN;
}
static inline void wj_pwm_invtr_trigger_invert_clr(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_TR_INV_EN;
}
static inline uint32_t wj_pwm_invtr_get_trigger_invert(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_TR_INV_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Trigger for counter=PWM0TRIGA when counter counts up
 *          1 : an ADC trigger pulse occurs when the counter counts up matching with PWM0TRIGA
 *          0 : no ADC trigger is output
*/
static inline void wj_pwm_invtr_trigger_0_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG |= WJ_PWM_INVTR_TR_UP_0_EN;
}
static inline void wj_pwm_invtr_trigger_0_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_TR_UP_0_EN;
}
static inline uint32_t wj_pwm_invtr_get_trigger_0_up(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_TR_UP_0_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Trigger for counter=PWM0TRIGA when counter counts down
 *          1 : an ADC trigger pulse occurs when the counter counts up matching with PWM0TRIGA
 *          0 : no ADC trigger is output
*/
static inline void wj_pwm_invtr_trigger_0_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG |= WJ_PWM_INVTR_TR_DW_0_EN;
}
static inline void wj_pwm_invtr_trigger_0_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_TR_DW_0_EN;
}
static inline uint32_t wj_pwm_invtr_get_trigger_0_down(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_TR_DW_0_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Trigger for counter=PWM1TRIGA when counter counts up
 *          1 : an ADC trigger pulse occurs when the counter counts up matching with PWM0TRIGA
 *          0 : no ADC trigger is output
*/
static inline void wj_pwm_invtr_trigger_1_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG |= WJ_PWM_INVTR_TR_UP_1_EN;
}
static inline void wj_pwm_invtr_trigger_1_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_TR_UP_1_EN;
}
static inline uint32_t wj_pwm_invtr_get_trigger_1_up(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_TR_UP_1_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Trigger for counter=PWM1TRIGA when counter counts down
 *          1 : an ADC trigger pulse occurs when the counter counts up matching with PWM0TRIGA
 *          0 : no ADC trigger is output
*/
static inline void wj_pwm_invtr_trigger_1_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG |= WJ_PWM_INVTR_TR_DW_1_EN;
}
static inline void wj_pwm_invtr_trigger_1_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_TR_DW_1_EN;
}
static inline uint32_t wj_pwm_invtr_get_trigger_1_down(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_TR_DW_1_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Trigger for counter=PWM2TRIGA when counter counts up
 *          1 : an ADC trigger pulse occurs when the counter counts up matching with PWM0TRIGA
 *          0 : no ADC trigger is output
*/
static inline void wj_pwm_invtr_trigger_2_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG |= WJ_PWM_INVTR_TR_UP_2_EN;
}
static inline void wj_pwm_invtr_trigger_2_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_TR_UP_2_EN;
}
static inline uint32_t wj_pwm_invtr_get_trigger_2_up(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_TR_UP_2_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Trigger for counter=PWM2TRIGA when counter counts down
 *          1 : an ADC trigger pulse occurs when the counter counts up matching with PWM0TRIGA
 *          0 : no ADC trigger is output
*/
static inline void wj_pwm_invtr_trigger_2_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG |= WJ_PWM_INVTR_TR_DW_2_EN;
}
static inline void wj_pwm_invtr_trigger_2_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_TR_DW_2_EN;
}
static inline uint32_t wj_pwm_invtr_get_trigger_2_down(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_TR_DW_2_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Trigger for counter=PWM3TRIGA when counter counts up
 *          1 : an ADC trigger pulse occurs when the counter counts up matching with PWM0TRIGA
 *          0 : no ADC trigger is output
*/
static inline void wj_pwm_invtr_trigger_3_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG |= WJ_PWM_INVTR_TR_UP_3_EN;
}
static inline void wj_pwm_invtr_trigger_3_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_TR_UP_3_EN;
}
static inline uint32_t wj_pwm_invtr_get_trigger_3_up(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_TR_UP_3_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Trigger for counter=PWM3TRIGA when counter counts down
 *          1 : an ADC trigger pulse occurs when the counter counts up matching with PWM0TRIGA
 *          0 : no ADC trigger is output
*/
static inline void wj_pwm_invtr_trigger_3_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG |= WJ_PWM_INVTR_TR_DW_3_EN;
}
static inline void wj_pwm_invtr_trigger_3_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_TR_DW_3_EN;
}
static inline uint32_t wj_pwm_invtr_get_trigger_3_down(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_TR_DW_3_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Trigger for counter=PWM4TRIGA when counter counts up
 *          1 : an ADC trigger pulse occurs when the counter counts up matching with PWM0TRIGA
 *          0 : no ADC trigger is output
*/
static inline void wj_pwm_invtr_trigger_4_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG |= WJ_PWM_INVTR_TR_UP_4_EN;
}
static inline void wj_pwm_invtr_trigger_4_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_TR_UP_4_EN;
}
static inline uint32_t wj_pwm_invtr_get_trigger_4_up(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_TR_UP_4_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Trigger for counter=PWM4TRIGA when counter counts down
 *          1 : an ADC trigger pulse occurs when the counter counts up matching with PWM0TRIGA
 *          0 : no ADC trigger is output
*/
static inline void wj_pwm_invtr_trigger_4_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG |= WJ_PWM_INVTR_TR_DW_4_EN;
}
static inline void wj_pwm_invtr_trigger_4_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_TR_DW_4_EN;
}
static inline uint32_t wj_pwm_invtr_get_trigger_4_down(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_TR_DW_4_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Trigger for counter=PWM5TRIGA when counter counts up
 *          1 : an ADC trigger pulse occurs when the counter counts up matching with PWM0TRIGA
 *          0 : no ADC trigger is output
*/
static inline void wj_pwm_invtr_trigger_5_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG |= WJ_PWM_INVTR_TR_UP_5_EN;
}
static inline void wj_pwm_invtr_trigger_5_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_TR_UP_5_EN;
}
static inline uint32_t wj_pwm_invtr_get_trigger_5_up(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_TR_UP_5_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Trigger for counter=PWM5TRIGA when counter counts down
 *          1 : an ADC trigger pulse occurs when the counter counts up matching with PWM0TRIGA
 *          0 : no ADC trigger is output
*/
static inline void wj_pwm_invtr_trigger_5_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG |= WJ_PWM_INVTR_TR_DW_5_EN;
}
static inline void wj_pwm_invtr_trigger_5_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINVERTTRIG &= ~WJ_PWM_INVTR_TR_DW_5_EN;
}
static inline uint32_t wj_pwm_invtr_get_trigger_5_down(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINVERTTRIG) & WJ_PWM_INVTR_TR_DW_5_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief    PWM channel 0 and 1 trigger compare value
 *           The value generating channel 0 and 1 trigger signal
*/
static inline uint32_t wj_pwm_trigger_read_ch_0_val(wj_pwm_regs_t *pwm_base)
{
    return ((pwm_base->PWM01TRIG & WJ_PWM_TRIG_COMP_VAL_0_Msk) >> WJ_PWM_TRIG_COMP_VAL_0_Pos);
}
static inline uint32_t wj_pwm_trigger_read_ch_1_val(wj_pwm_regs_t *pwm_base)
{
    return ((pwm_base->PWM01TRIG & WJ_PWM_TRIG_COMP_VAL_1_Msk) >> WJ_PWM_TRIG_COMP_VAL_1_Pos);
}
static inline void wj_pwm_trigger_write_ch_0_val(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->PWM01TRIG &= ~WJ_PWM_TRIG_COMP_VAL_0_Msk;
    pwm_base->PWM01TRIG |= (value << WJ_PWM_TRIG_COMP_VAL_0_Pos);
}
static inline void wj_pwm_trigger_write_ch_1_val(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->PWM01TRIG &= ~WJ_PWM_TRIG_COMP_VAL_1_Msk;
    pwm_base->PWM01TRIG |= (value << WJ_PWM_TRIG_COMP_VAL_1_Pos);
}

/**
 * \brief    PWM channel 2 and 3 trigger compare value
 *           The value generating channel 2 and 3 trigger signal
*/
static inline uint32_t wj_pwm_trigger_read_ch_2_val(wj_pwm_regs_t *pwm_base)
{
    return ((pwm_base->PWM23TRIG & WJ_PWM_TRIG_COMP_VAL_2_Msk) >> WJ_PWM_TRIG_COMP_VAL_2_Pos);
}
static inline uint32_t wj_pwm_trigger_read_ch_3_val(wj_pwm_regs_t *pwm_base)
{
    return ((pwm_base->PWM23TRIG & WJ_PWM_TRIG_COMP_VAL_3_Msk) >> WJ_PWM_TRIG_COMP_VAL_3_Pos);
}
static inline void wj_pwm_trigger_write_ch_2_val(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->PWM23TRIG &= ~WJ_PWM_TRIG_COMP_VAL_2_Msk;
    pwm_base->PWM23TRIG |= (value << WJ_PWM_TRIG_COMP_VAL_2_Pos);
}
static inline void wj_pwm_trigger_write_ch_3_val(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->PWM23TRIG &= ~WJ_PWM_TRIG_COMP_VAL_3_Msk;
    pwm_base->PWM23TRIG |= (value << WJ_PWM_TRIG_COMP_VAL_3_Pos);
}

/**
 * \brief    PWM channel 4 and 5 trigger compare value
 *           The value generating channel 4 and 5 trigger signal
*/
static inline uint32_t wj_pwm_trigger_read_ch_4_val(wj_pwm_regs_t *pwm_base)
{
    return ((pwm_base->PWM45TRIG & WJ_PWM_TRIG_COMP_VAL_4_Msk) >> WJ_PWM_TRIG_COMP_VAL_4_Pos);
}
static inline uint32_t wj_pwm_trigger_read_ch_5_val(wj_pwm_regs_t *pwm_base)
{
    return ((pwm_base->PWM45TRIG & WJ_PWM_TRIG_COMP_VAL_5_Msk) >> WJ_PWM_TRIG_COMP_VAL_5_Pos);
}
static inline void wj_pwm_trigger_write_ch_4_val(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->PWM45TRIG &= ~WJ_PWM_TRIG_COMP_VAL_4_Msk;
    pwm_base->PWM45TRIG |= (value << WJ_PWM_TRIG_COMP_VAL_4_Pos);
}
static inline void wj_pwm_trigger_write_ch_5_val(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->PWM45TRIG &= ~WJ_PWM_TRIG_COMP_VAL_5_Msk;
    pwm_base->PWM45TRIG |= (value << WJ_PWM_TRIG_COMP_VAL_5_Pos);
}

/**
 * \brief   Fault interrupt enable
 *          1 : the fault interrupt enable
 *          0 : the fault interrupt disable
*/
static inline void wj_pwm_intr_fault_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_FAULT_EN;
}
static inline void wj_pwm_intr_fault_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_FAULT_EN;
}
static inline uint32_t wj_pwm_intr_fault_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_FAULT_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_0_zero_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_0_ZERO_EN;
}
static inline void wj_pwm_intr_0_zero_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_0_ZERO_EN;
}
static inline uint32_t wj_pwm_intr_0_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_0_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_0_load_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_0_LOAD_EN;
}
static inline void wj_pwm_intr_0_load_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_0_LOAD_EN;
}
static inline uint32_t wj_pwm_intr_0_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_0_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_0_a_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_0_CA_UP_EN;
}
static inline void wj_pwm_intr_0_a_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_0_CA_UP_EN;
}
static inline uint32_t wj_pwm_intr_0_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_0_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_0_b_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_0_CB_UP_EN;
}
static inline void wj_pwm_intr_0_b_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_0_CB_UP_EN;
}
static inline uint32_t wj_pwm_intr_0_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_0_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_0_a_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_0_CA_DW_EN;
}
static inline void wj_pwm_intr_0_a_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_0_CA_DW_EN;
}
static inline uint32_t wj_pwm_intr_0_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_0_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_0_b_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_0_CB_DW_EN;
}
static inline void wj_pwm_intr_0_b_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_0_CB_DW_EN;
}
static inline uint32_t wj_pwm_intr_0_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_0_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_1_zero_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_1_ZERO_EN;
}
static inline void wj_pwm_intr_1_zero_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_1_ZERO_EN;
}
static inline uint32_t wj_pwm_intr_1_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_1_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_1_load_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_1_LOAD_EN;
}
static inline void wj_pwm_intr_1_load_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_1_LOAD_EN;
}
static inline uint32_t wj_pwm_intr_1_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_1_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_1_a_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_1_CA_UP_EN;
}
static inline void wj_pwm_intr_1_a_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_1_CA_UP_EN;
}
static inline uint32_t wj_pwm_intr_1_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_1_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_1_b_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_1_CB_UP_EN;
}
static inline void wj_pwm_intr_1_b_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_1_CB_UP_EN;
}
static inline uint32_t wj_pwm_intr_1_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_1_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_1_a_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_1_CA_DW_EN;
}
static inline void wj_pwm_intr_1_a_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_1_CA_DW_EN;
}
static inline uint32_t wj_pwm_intr_1_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_1_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_1_b_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_1_CB_DW_EN;
}
static inline void wj_pwm_intr_1_b_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_1_CB_DW_EN;
}
static inline uint32_t wj_pwm_intr_1_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_1_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_2_zero_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_2_ZERO_EN;
}
static inline void wj_pwm_intr_2_zero_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_2_ZERO_EN;
}
static inline uint32_t wj_pwm_intr_2_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_2_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_2_load_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_2_LOAD_EN;
}
static inline void wj_pwm_intr_2_load_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_2_LOAD_EN;
}
static inline uint32_t wj_pwm_intr_2_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_2_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_2_a_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_2_CA_UP_EN;
}
static inline void wj_pwm_intr_2_a_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_2_CA_UP_EN;
}
static inline uint32_t wj_pwm_intr_2_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_2_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_2_b_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_2_CB_UP_EN;
}
static inline void wj_pwm_intr_2_b_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_2_CB_UP_EN;
}
static inline uint32_t wj_pwm_intr_2_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_2_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_2_a_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_2_CA_DW_EN;
}
static inline void wj_pwm_intr_2_a_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_2_CA_DW_EN;
}
static inline uint32_t wj_pwm_intr_2_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_2_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_2_b_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 |= WJ_PWM_INTEN_2_CB_DW_EN;
}
static inline void wj_pwm_intr_2_b_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN1 &= ~WJ_PWM_INTEN_2_CB_DW_EN;
}
static inline uint32_t wj_pwm_intr_2_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN1) & WJ_PWM_INTEN_2_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_3_zero_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_3_ZERO_EN;
}
static inline void wj_pwm_intr_3_zero_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_3_ZERO_EN;
}
static inline uint32_t wj_pwm_intr_3_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_3_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_3_load_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_3_LOAD_EN;
}
static inline void wj_pwm_intr_3_load_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_3_LOAD_EN;
}
static inline uint32_t wj_pwm_intr_3_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_3_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_3_a_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_3_CA_UP_EN;
}
static inline void wj_pwm_intr_3_a_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_3_CA_UP_EN;
}
static inline uint32_t wj_pwm_intr_3_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_3_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_3_b_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_3_CB_UP_EN;
}
static inline void wj_pwm_intr_3_b_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_3_CB_UP_EN;
}
static inline uint32_t wj_pwm_intr_3_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_3_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_3_a_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_3_CA_DW_EN;
}
static inline void wj_pwm_intr_3_a_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_3_CA_DW_EN;
}
static inline uint32_t wj_pwm_intr_3_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_3_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_3_b_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_3_CB_DW_EN;
}
static inline void wj_pwm_intr_3_b_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_3_CB_DW_EN;
}
static inline uint32_t wj_pwm_intr_3_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_3_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_4_zero_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_4_ZERO_EN;
}
static inline void wj_pwm_intr_4_zero_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_4_ZERO_EN;
}
static inline uint32_t wj_pwm_intr_4_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_4_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_4_load_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_4_LOAD_EN;
}
static inline void wj_pwm_intr_4_load_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_4_LOAD_EN;
}
static inline uint32_t wj_pwm_intr_4_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_4_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_4_a_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_4_CA_UP_EN;
}
static inline void wj_pwm_intr_4_a_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_4_CA_UP_EN;
}
static inline uint32_t wj_pwm_intr_4_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_4_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_4_b_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_4_CB_UP_EN;
}
static inline void wj_pwm_intr_4_b_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_4_CB_UP_EN;
}
static inline uint32_t wj_pwm_intr_4_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_4_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_4_a_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_4_CA_DW_EN;
}
static inline void wj_pwm_intr_4_a_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_4_CA_DW_EN;
}
static inline uint32_t wj_pwm_intr_4_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_4_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_4_b_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_4_CB_DW_EN;
}
static inline void wj_pwm_intr_4_b_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_4_CB_DW_EN;
}
static inline uint32_t wj_pwm_intr_4_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_4_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_5_zero_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_5_ZERO_EN;
}
static inline void wj_pwm_intr_5_zero_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_5_ZERO_EN;
}
static inline uint32_t wj_pwm_intr_5_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_5_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_5_load_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_5_LOAD_EN;
}
static inline void wj_pwm_intr_5_load_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_5_LOAD_EN;
}
static inline uint32_t wj_pwm_intr_5_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_5_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_5_a_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_5_CA_UP_EN;
}
static inline void wj_pwm_intr_5_a_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_5_CA_UP_EN;
}
static inline uint32_t wj_pwm_intr_5_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_5_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_5_b_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_5_CB_UP_EN;
}
static inline void wj_pwm_intr_5_b_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_5_CB_UP_EN;
}
static inline uint32_t wj_pwm_intr_5_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_5_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_5_a_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_5_CA_DW_EN;
}
static inline void wj_pwm_intr_5_a_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_5_CA_DW_EN;
}
static inline uint32_t wj_pwm_intr_5_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_5_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intr_5_b_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 |= WJ_PWM_INTEN_5_CB_DW_EN;
}
static inline void wj_pwm_intr_5_b_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMINTEN2 &= ~WJ_PWM_INTEN_5_CB_DW_EN;
}
static inline uint32_t wj_pwm_intr_5_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMINTEN2) & WJ_PWM_INTEN_5_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Fault raw interrupt asserted
 *          1 : the fault raw interrupt asserted
 *          0 : the fault raw interrupt not asserted
*/
static inline uint32_t wj_pwm_intris_fault_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTRIS_FAULT_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_0_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_0_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_0_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_0_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_0_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_0_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_0_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_0_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_0_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_0_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_0_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_0_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_1_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_1_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_1_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_1_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_1_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_1_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_1_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_1_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_1_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_1_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_1_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_1_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_2_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_2_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_2_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_2_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_2_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_2_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_2_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_2_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_2_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_2_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_2_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS1) & WJ_PWM_INTEN_2_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_3_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_3_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_3_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_3_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_3_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_3_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_3_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_3_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_3_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_3_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_3_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_3_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_4_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_4_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_4_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_4_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_4_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_4_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_4_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_4_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_4_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_4_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_4_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_4_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_5_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_5_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_5_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_5_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_5_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_5_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_5_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_5_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_5_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_5_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Raw Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intris_5_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMRIS2) & WJ_PWM_INTEN_5_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Fault interrupt
 *          1 : clear the fault interrupt
 *          0 : not clear the fault interrupt
*/
static inline uint32_t wj_pwm_intic_fault_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_FAULT_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_0_zero_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_0_ZERO_EN;
}
static inline void wj_pwm_intic_0_zero_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_0_ZERO_EN;
}
static inline uint32_t wj_pwm_intic_0_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_0_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_0_load_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_0_LOAD_EN;
}
static inline void wj_pwm_intic_0_load_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_0_LOAD_EN;
}
static inline uint32_t wj_pwm_intic_0_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_0_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_0_a_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_0_CA_UP_EN;
}
static inline void wj_pwm_intic_0_a_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_0_CA_UP_EN;
}
static inline uint32_t wj_pwm_intic_0_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_0_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_0_b_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_0_CB_UP_EN;
}
static inline void wj_pwm_intic_0_b_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_0_CB_UP_EN;
}
static inline uint32_t wj_pwm_intic_0_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_0_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_0_a_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_0_CA_DW_EN;
}
static inline void wj_pwm_intic_0_a_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_0_CA_DW_EN;
}
static inline uint32_t wj_pwm_intic_0_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_0_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_0_b_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_0_CB_DW_EN;
}
static inline void wj_pwm_intic_0_b_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_0_CB_DW_EN;
}
static inline uint32_t wj_pwm_intic_0_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_0_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_1_zero_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_1_ZERO_EN;
}
static inline void wj_pwm_intic_1_zero_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_1_ZERO_EN;
}
static inline uint32_t wj_pwm_intic_1_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_1_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_1_load_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_1_LOAD_EN;
}
static inline void wj_pwm_intic_1_load_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_1_LOAD_EN;
}
static inline uint32_t wj_pwm_intic_1_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_1_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_1_a_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_1_CA_UP_EN;
}
static inline void wj_pwm_intic_1_a_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_1_CA_UP_EN;
}
static inline uint32_t wj_pwm_intic_1_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_1_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_1_b_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_1_CB_UP_EN;
}
static inline void wj_pwm_intic_1_b_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_1_CB_UP_EN;
}
static inline uint32_t wj_pwm_intic_1_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_1_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_1_a_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_1_CA_DW_EN;
}
static inline void wj_pwm_intic_1_a_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_1_CA_DW_EN;
}
static inline uint32_t wj_pwm_intic_1_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_1_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_1_b_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_1_CB_DW_EN;
}
static inline void wj_pwm_intic_1_b_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_1_CB_DW_EN;
}
static inline uint32_t wj_pwm_intic_1_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_1_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_2_zero_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_2_ZERO_EN;
}
static inline void wj_pwm_intic_2_zero_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_2_ZERO_EN;
}
static inline uint32_t wj_pwm_intic_2_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_2_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_2_load_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_2_LOAD_EN;
}
static inline void wj_pwm_intic_2_load_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_2_LOAD_EN;
}
static inline uint32_t wj_pwm_intic_2_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_2_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_2_a_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_2_CA_UP_EN;
}
static inline void wj_pwm_intic_2_a_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_2_CA_UP_EN;
}
static inline uint32_t wj_pwm_intic_2_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_2_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_2_b_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_2_CB_UP_EN;
}
static inline void wj_pwm_intic_2_b_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_2_CB_UP_EN;
}
static inline uint32_t wj_pwm_intic_2_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_2_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_2_a_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_2_CA_DW_EN;
}
static inline void wj_pwm_intic_2_a_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_2_CA_DW_EN;
}
static inline uint32_t wj_pwm_intic_2_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_2_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_2_b_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 |= WJ_PWM_INTIC_2_CB_DW_EN;
}
static inline void wj_pwm_intic_2_b_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC1 &= ~WJ_PWM_INTIC_2_CB_DW_EN;
}
static inline uint32_t wj_pwm_intic_2_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC1) & WJ_PWM_INTIC_2_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_3_zero_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_3_ZERO_EN;
}
static inline void wj_pwm_intic_3_zero_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_3_ZERO_EN;
}
static inline uint32_t wj_pwm_intic_3_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_3_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_3_load_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_3_LOAD_EN;
}
static inline void wj_pwm_intic_3_load_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_3_LOAD_EN;
}
static inline uint32_t wj_pwm_intic_3_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_3_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_3_a_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_3_CA_UP_EN;
}
static inline void wj_pwm_intic_3_a_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_3_CA_UP_EN;
}
static inline uint32_t wj_pwm_intic_3_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_3_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_3_b_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_3_CB_UP_EN;
}
static inline void wj_pwm_intic_3_b_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_3_CB_UP_EN;
}
static inline uint32_t wj_pwm_intic_3_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_3_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_3_a_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_3_CA_DW_EN;
}
static inline void wj_pwm_intic_3_a_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_3_CA_DW_EN;
}
static inline uint32_t wj_pwm_intic_3_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_3_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_3_b_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_3_CB_DW_EN;
}
static inline void wj_pwm_intic_3_b_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_3_CB_DW_EN;
}
static inline uint32_t wj_pwm_intic_3_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_3_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_4_zero_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_4_ZERO_EN;
}
static inline void wj_pwm_intic_4_zero_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_4_ZERO_EN;
}
static inline uint32_t wj_pwm_intic_4_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_4_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_4_load_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_4_LOAD_EN;
}
static inline void wj_pwm_intic_4_load_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_4_LOAD_EN;
}
static inline uint32_t wj_pwm_intic_4_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_4_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_4_a_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_4_CA_UP_EN;
}
static inline void wj_pwm_intic_4_a_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_4_CA_UP_EN;
}
static inline uint32_t wj_pwm_intic_4_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_4_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_4_b_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_4_CB_UP_EN;
}
static inline void wj_pwm_intic_4_b_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_4_CB_UP_EN;
}
static inline uint32_t wj_pwm_intic_4_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_4_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_4_a_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_4_CA_DW_EN;
}
static inline void wj_pwm_intic_4_a_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_4_CA_DW_EN;
}
static inline uint32_t wj_pwm_intic_4_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_4_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_4_b_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_4_CB_DW_EN;
}
static inline void wj_pwm_intic_4_b_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_4_CB_DW_EN;
}
static inline uint32_t wj_pwm_intic_4_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_4_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_5_zero_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_5_ZERO_EN;
}
static inline void wj_pwm_intic_5_zero_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_5_ZERO_EN;
}
static inline uint32_t wj_pwm_intic_5_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_5_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_5_load_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_5_LOAD_EN;
}
static inline void wj_pwm_intic_5_load_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_5_LOAD_EN;
}
static inline uint32_t wj_pwm_intic_5_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_5_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_5_a_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_5_CA_UP_EN;
}
static inline void wj_pwm_intic_5_a_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_5_CA_UP_EN;
}
static inline uint32_t wj_pwm_intic_5_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_5_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_5_b_up_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_5_CB_UP_EN;
}
static inline void wj_pwm_intic_5_b_up_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_5_CB_UP_EN;
}
static inline uint32_t wj_pwm_intic_5_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_5_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_5_a_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_5_CA_DW_EN;
}
static inline void wj_pwm_intic_5_a_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_5_CA_DW_EN;
}
static inline uint32_t wj_pwm_intic_5_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_5_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Clear Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline void wj_pwm_intic_5_b_down_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 |= WJ_PWM_INTIC_5_CB_DW_EN;
}
static inline void wj_pwm_intic_5_b_down_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMIC2 &= ~WJ_PWM_INTIC_5_CB_DW_EN;
}
static inline uint32_t wj_pwm_intic_5_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIC2) & WJ_PWM_INTIC_5_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Fault interrupt
 *          1 : clear the fault interrupt
 *          0 : not clear the fault interrupt
*/
static inline uint32_t wj_pwm_intis_fault_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_FAULT_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_0_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_0_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_0_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_0_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_0_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_0_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_0_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_0_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_0_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_0_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_0_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_0_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_1_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_1_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_1_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_1_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_1_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_1_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_1_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_1_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_1_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_1_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_1_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_1_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_2_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_2_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_2_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_2_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_2_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_2_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_2_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_2_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_2_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_2_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_2_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS1) & WJ_PWM_INTIS_2_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_3_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_3_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_3_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_3_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_3_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_3_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_3_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_3_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_3_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_3_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_3_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_3_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_4_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_4_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_4_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_4_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_4_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_4_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_4_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_4_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_4_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_4_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_4_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_4_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=0
 *          1 : a raw interrupt occurs when the counter is zero
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_5_zero_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_5_ZERO_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=Load
 *          1 : a raw interrupt occurs when the counter matches the values in the PWM01LOAD register value
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_5_load_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_5_LOAD_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_5_a_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_5_CA_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Up
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting up
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_5_b_up_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_5_CB_UP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator A Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_5_a_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_5_CA_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Interrupt for counter=comparator B Down
 *          1 : a raw interrupt occurs when the counter matches the value in the PWM0CMP register value when counting down
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_intis_5_b_down_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMIS2) & WJ_PWM_INTIS_5_CB_DW_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Counter mode
 *          1 : count-up/down mode
 *          0 : count-up mode
*/
static inline void wj_pwm_ctl_ch_up_mode(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->PWMCTL &= WJ_PWM_CTL_MODE_UP(ch);
}
static inline void wj_pwm_ctl_ch_up_dw_mode(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->PWMCTL |= WJ_PWM_CTL_MODE_UP_DW(ch);
}

/**
 * \brief   Synchronize mode
 *          00 : update register value when counter reach to zero
 *          01 : update register value when counter reach to load value
 *          10 : update register value when counter reach to zero and load value
 *          11 : not update
*/
static inline void wj_pwm_ctl_sync_0_zero(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_0_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_0_ZERO;
}
static inline void wj_pwm_ctl_sync_0_load(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_0_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_0_LOAD;
}
static inline void wj_pwm_ctl_sync_0_zero_load(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_0_Msk;
    pwm_base->PWMCTL &= WJ_PWM_CTL_SYNC_0_ZERO_LOAD;
}
static inline void wj_pwm_ctl_sync_0_no(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_0_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_0_NO;
}
static inline uint32_t wj_pwm_ctl_sync_0_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMCTL) & WJ_PWM_CTL_SYNC_0_Msk) >> WJ_PWM_CTL_SYNC_0_Pos);
}
static inline void wj_pwm_ctl_sync_1_zero(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_1_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_1_ZERO;
}
static inline void wj_pwm_ctl_sync_1_load(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_1_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_1_LOAD;
}
static inline void wj_pwm_ctl_sync_1_zero_load(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_1_Msk;
    pwm_base->PWMCTL &= WJ_PWM_CTL_SYNC_1_ZERO_LOAD;
}
static inline void wj_pwm_ctl_sync_1_no(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_1_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_1_NO;
}
static inline uint32_t wj_pwm_ctl_sync_1_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMCTL) & WJ_PWM_CTL_SYNC_1_Msk) >> WJ_PWM_CTL_SYNC_1_Pos);
}
static inline void wj_pwm_ctl_sync_2_zero(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_2_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_2_ZERO;
}
static inline void wj_pwm_ctl_sync_2_load(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_2_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_2_LOAD;
}
static inline void wj_pwm_ctl_sync_2_zero_load(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_2_Msk;
    pwm_base->PWMCTL &= WJ_PWM_CTL_SYNC_2_ZERO_LOAD;
}
static inline void wj_pwm_ctl_sync_2_no(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_2_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_2_NO;
}
static inline uint32_t wj_pwm_ctl_sync_2_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMCTL) & WJ_PWM_CTL_SYNC_2_Msk) >> WJ_PWM_CTL_SYNC_2_Pos);
}
static inline void wj_pwm_ctl_sync_3_zero(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_3_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_3_ZERO;
}
static inline void wj_pwm_ctl_sync_3_load(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_3_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_3_LOAD;
}
static inline void wj_pwm_ctl_sync_3_zero_load(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_3_Msk;
    pwm_base->PWMCTL &= WJ_PWM_CTL_SYNC_3_ZERO_LOAD;
}
static inline void wj_pwm_ctl_sync_3_no(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_3_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_3_NO;
}
static inline uint32_t wj_pwm_ctl_sync_3_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMCTL) & WJ_PWM_CTL_SYNC_3_Msk) >> WJ_PWM_CTL_SYNC_3_Pos);
}
static inline void wj_pwm_ctl_sync_4_zero(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_4_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_4_ZERO;
}
static inline void wj_pwm_ctl_sync_4_load(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_4_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_4_LOAD;
}
static inline void wj_pwm_ctl_sync_4_zero_load(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_4_Msk;
    pwm_base->PWMCTL &= WJ_PWM_CTL_SYNC_4_ZERO_LOAD;
}
static inline void wj_pwm_ctl_sync_4_no(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_4_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_4_NO;
}
static inline uint32_t wj_pwm_ctl_sync_4_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMCTL) & WJ_PWM_CTL_SYNC_4_Msk) >> WJ_PWM_CTL_SYNC_4_Pos);
}
static inline void wj_pwm_ctl_sync_5_zero(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_5_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_5_ZERO;
}
static inline void wj_pwm_ctl_sync_5_load(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_5_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_5_LOAD;
}
static inline void wj_pwm_ctl_sync_5_zero_load(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_5_Msk;
    pwm_base->PWMCTL &= WJ_PWM_CTL_SYNC_5_ZERO_LOAD;
}
static inline void wj_pwm_ctl_sync_5_no(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWMCTL &= ~WJ_PWM_CTL_SYNC_5_Msk;
    pwm_base->PWMCTL |= WJ_PWM_CTL_SYNC_5_NO;
}
static inline uint32_t wj_pwm_ctl_sync_5_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWMCTL) & WJ_PWM_CTL_SYNC_5_Msk) >> WJ_PWM_CTL_SYNC_5_Pos);
}
/**
 * \brief   Counter load n value
 *          These registers contain the load value for the PWM counter.
*/
static inline void wj_pwm_counter_write_load_ch(wj_pwm_regs_t *pwm_base, uint32_t ch, uint32_t value)
{
    *(&pwm_base->PWM01LOAD + (ch >> 1U)) &= ~WJ_PWM_COUNTER_LOAD_CH_Msk(ch);
    *(&pwm_base->PWM01LOAD + (ch >> 1U)) |= (value << WJ_PWM_COUNTER_LOAD_CH_Pos(ch));
}

/**
 * \brief   Counter n value
 *          These registers contain the current value of the PWM counter.
*/
static inline uint32_t wj_pwm_counter_read_count_ch_0(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM01COUNT) & WJ_PWM_COUNTER_COUNT_CH_0_Msk) >> WJ_PWM_COUNTER_COUNT_CH_0_Pos);
}
static inline uint32_t wj_pwm_counter_read_count_ch_1(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM01COUNT) & WJ_PWM_COUNTER_COUNT_CH_1_Msk) >> WJ_PWM_COUNTER_COUNT_CH_1_Pos);
}
static inline uint32_t wj_pwm_counter_read_count_ch_2(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM23COUNT) & WJ_PWM_COUNTER_COUNT_CH_2_Msk) >> WJ_PWM_COUNTER_COUNT_CH_2_Pos);
}
static inline uint32_t wj_pwm_counter_read_count_ch_3(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM23COUNT) & WJ_PWM_COUNTER_COUNT_CH_3_Msk) >> WJ_PWM_COUNTER_COUNT_CH_3_Pos);
}
static inline uint32_t wj_pwm_counter_read_count_ch_4(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM45COUNT) & WJ_PWM_COUNTER_COUNT_CH_4_Msk) >> WJ_PWM_COUNTER_COUNT_CH_4_Pos);
}
static inline uint32_t wj_pwm_counter_read_count_ch_5(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM45COUNT) & WJ_PWM_COUNTER_COUNT_CH_5_Msk) >> WJ_PWM_COUNTER_COUNT_CH_5_Pos);
}


/**
 * \brief   Comparator value
 *          These registers contain a/b value to be compared against the counter.
*/
static inline uint32_t wj_pwm_counter_read_compare_a_ch(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    return ((*(&pwm_base->PWM0CMP + ch) & WJ_PWM_COUNTER_COMP_CH_A_Msk) >> WJ_PWM_COUNTER_COMP_CH_A_Pos);
}
static inline uint32_t wj_pwm_counter_read_compare_b_ch(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    return ((*(&pwm_base->PWM0CMP + ch) & WJ_PWM_COUNTER_COMP_CH_B_Msk) >> WJ_PWM_COUNTER_COMP_CH_B_Pos);
}
static inline void wj_pwm_counter_write_compare_a_ch(wj_pwm_regs_t *pwm_base, uint32_t ch, uint32_t value)
{
    *(&pwm_base->PWM0CMP + ch) &= ~WJ_PWM_COUNTER_COMP_CH_A_Msk;
    *(&pwm_base->PWM0CMP + ch) |= (value << WJ_PWM_COUNTER_COMP_CH_A_Pos);
}
static inline void wj_pwm_counter_write_compare_b_ch(wj_pwm_regs_t *pwm_base, uint32_t ch, uint32_t value)
{
    *(&pwm_base->PWM0CMP + ch) &= ~WJ_PWM_COUNTER_COMP_CH_B_Msk;
    *(&pwm_base->PWM0CMP + ch) |= (value << WJ_PWM_COUNTER_COMP_CH_B_Pos);
}

/**
 * \brief   Dead-band delay
 *          These registers contains the number of clock ticks to delay.
*/
static inline uint32_t wj_pwm_read_deadband_delay_ch_0(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM01DB) & WJ_PWM_DEADBAND_CH_0_DELAY_Msk) >> WJ_PWM_DEADBAND_CH_0_DELAY_Pos);
}
static inline void wj_pwm_write_deadband_delay_ch_0(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->PWM01DB &= ~WJ_PWM_DEADBAND_CH_0_DELAY_Msk;
    pwm_base->PWM01DB |= (value << WJ_PWM_DEADBAND_CH_0_DELAY_Pos);
}
static inline uint32_t wj_pwm_read_deadband_delay_ch_1(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM01DB) & WJ_PWM_DEADBAND_CH_1_DELAY_Msk) >> WJ_PWM_DEADBAND_CH_1_DELAY_Pos);
}
static inline void wj_pwm_write_deadband_delay_ch_1(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->PWM01DB &= ~WJ_PWM_DEADBAND_CH_1_DELAY_Msk;
    pwm_base->PWM01DB |= (value << WJ_PWM_DEADBAND_CH_1_DELAY_Pos);
}
/**
 * \brief   Dead-band enable
 *          1 : insert dead-band into the output signals
 *          0 : pass the PWM signals through simply
*/
static inline void wj_pwm_deadband_ch_0_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWM01DB |= WJ_PWM_DEADBAND_CH_0_EN;
}
static inline void wj_pwm_deadband_ch_0_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWM01DB &= ~WJ_PWM_DEADBAND_CH_0_EN;
}
static inline uint32_t wj_pwm_deadband_ch_0_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM01DB) & WJ_PWM_DEADBAND_CH_0_EN_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_pwm_deadband_ch_1_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWM01DB |= WJ_PWM_DEADBAND_CH_1_EN;
}
static inline void wj_pwm_deadband_ch_1_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWM01DB &= ~WJ_PWM_DEADBAND_CH_1_EN;
}
static inline uint32_t wj_pwm_deadband_ch_1_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM01DB) & WJ_PWM_DEADBAND_CH_1_EN_Msk) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   Dead-band delay
 *          These registers contains the number of clock ticks to delay.
*/
static inline uint32_t wj_pwm_read_deadband_delay_ch_2(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM23DB) & WJ_PWM_DEADBAND_CH_2_DELAY_Msk) >> WJ_PWM_DEADBAND_CH_2_DELAY_Pos);
}
static inline void wj_pwm_write_deadband_delay_ch_2(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->PWM23DB &= ~WJ_PWM_DEADBAND_CH_2_DELAY_Msk;
    pwm_base->PWM23DB |= (value << WJ_PWM_DEADBAND_CH_2_DELAY_Pos);
}
static inline uint32_t wj_pwm_read_deadband_delay_ch_3(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM23DB) & WJ_PWM_DEADBAND_CH_3_DELAY_Msk) >> WJ_PWM_DEADBAND_CH_3_DELAY_Pos);
}
static inline void wj_pwm_write_deadband_delay_ch_3(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->PWM23DB &= ~WJ_PWM_DEADBAND_CH_3_DELAY_Msk;
    pwm_base->PWM23DB |= (value << WJ_PWM_DEADBAND_CH_3_DELAY_Pos);
}
/**
 * \brief   Dead-band enable
 *          1 : insert dead-band into the output signals
 *          0 : pass the PWM signals through simply
*/
static inline void wj_pwm_deadband_ch_2_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWM23DB |= WJ_PWM_DEADBAND_CH_2_EN;
}
static inline void wj_pwm_deadband_ch_2_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWM23DB &= ~WJ_PWM_DEADBAND_CH_2_EN;
}
static inline uint32_t wj_pwm_deadband_ch_2_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM23DB) & WJ_PWM_DEADBAND_CH_2_EN_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_pwm_deadband_ch_3_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWM23DB |= WJ_PWM_DEADBAND_CH_3_EN;
}
static inline void wj_pwm_deadband_ch_3_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWM23DB &= ~WJ_PWM_DEADBAND_CH_3_EN;
}
static inline uint32_t wj_pwm_deadband_ch_3_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM23DB) & WJ_PWM_DEADBAND_CH_3_EN_Msk) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   Dead-band delay
 *          These registers contains the number of clock ticks to delay.
*/
static inline uint32_t wj_pwm_read_deadband_delay_ch_4(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM45DB) & WJ_PWM_DEADBAND_CH_4_DELAY_Msk) >> WJ_PWM_DEADBAND_CH_4_DELAY_Pos);
}
static inline void wj_pwm_write_deadband_delay_ch_4(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->PWM45DB &= ~WJ_PWM_DEADBAND_CH_4_DELAY_Msk;
    pwm_base->PWM45DB |= (value << WJ_PWM_DEADBAND_CH_4_DELAY_Pos);
}
static inline uint32_t wj_pwm_read_deadband_delay_ch_5(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM45DB) & WJ_PWM_DEADBAND_CH_5_DELAY_Msk) >> WJ_PWM_DEADBAND_CH_5_DELAY_Pos);
}
static inline void wj_pwm_write_deadband_delay_ch_5(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->PWM45DB &= ~WJ_PWM_DEADBAND_CH_5_DELAY_Msk;
    pwm_base->PWM45DB |= (value << WJ_PWM_DEADBAND_CH_5_DELAY_Pos);
}
/**
 * \brief   Dead-band enable
 *          1 : insert dead-band into the output signals
 *          0 : pass the PWM signals through simply
*/
static inline void wj_pwm_deadband_ch_4_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWM45DB |= WJ_PWM_DEADBAND_CH_4_EN;
}
static inline void wj_pwm_deadband_ch_4_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWM45DB &= ~WJ_PWM_DEADBAND_CH_4_EN;
}
static inline uint32_t wj_pwm_deadband_ch_4_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM45DB) & WJ_PWM_DEADBAND_CH_4_EN_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_pwm_deadband_ch_5_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWM45DB |= WJ_PWM_DEADBAND_CH_5_EN;
}
static inline void wj_pwm_deadband_ch_5_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->PWM45DB &= ~WJ_PWM_DEADBAND_CH_5_EN;
}
static inline uint32_t wj_pwm_deadband_ch_5_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->PWM45DB) & WJ_PWM_DEADBAND_CH_5_EN_Msk) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   Capture mode
 *          1 : edge count mode
 *          0 : edge time mode
*/
static inline void wj_pwm_capctl_mode_ch_cnt(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->CAPCTL |= WJ_PWM_CAPCTL_MODE_CNT(ch);
}
static inline void wj_pwm_capctl_mode_ch_time(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->CAPCTL &= WJ_PWM_CAPCTL_MODE_TIME(ch);
}
static inline uint32_t wj_pwm_capctl_mode_ch_get_sta(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    return (((pwm_base->CAPCTL) & WJ_PWM_CAPCTL_MODE_Msk(ch)) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   Capture0 edge event mode
 *          00: posedge edge
 *          01 : negedge edge
 *          10 : reserved
 *          11 : both edge
*/
static inline void wj_pwm_capctl_event_ch_pose(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->CAPCTL &= ~WJ_PWM_CAPCTL_EVENT_Msk(ch);
    pwm_base->CAPCTL |= WJ_PWM_CAPCTL_EVENT_POSE(ch);
}
static inline void wj_pwm_capctl_event_ch_nege(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->CAPCTL &= ~WJ_PWM_CAPCTL_EVENT_Msk(ch);
    pwm_base->CAPCTL |= WJ_PWM_CAPCTL_EVENT_NEGE(ch);
}
static inline void wj_pwm_capctl_event_ch_both(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->CAPCTL &= ~WJ_PWM_CAPCTL_EVENT_Msk(ch);
    pwm_base->CAPCTL |= WJ_PWM_CAPCTL_EVENT_BOTH(ch);
}
static inline uint32_t wj_pwm_capctl_event_ch_get_sta(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    return ((pwm_base->CAPCTL) & WJ_PWM_CAPCTL_EVENT_Msk(ch)) >> WJ_PWM_CAPCTL_EVENT_Pos(ch);
}
/**
 * \brief   Capture edge count interrupt enable
 *          1 : interrupt enable
 *          0 : interrupt disable
*/

static inline void wj_pwm_capint_ie_cnt_ch_en(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->CAPINTEN |= WJ_PWM_CAPINT_IE_CNT_EN(ch);
}
static inline void wj_pwm_capint_ie_cnt_ch_dis(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->CAPINTEN &= ~WJ_PWM_CAPINT_IE_CNT_EN(ch);
}

/**
 * \brief   Capture edge time interrupt enable
 *          1 : interrupt enable
 *          0 : interrupt disable
*/
static inline void wj_pwm_capint_ie_time_ch_en(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->CAPINTEN |= WJ_PWM_CAPINT_IE_TIM_EN(ch);
}
static inline void wj_pwm_capint_ie_time_ch_dis(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->CAPINTEN &= ~WJ_PWM_CAPINT_IE_TIM_EN(ch);
}
/**
 * \brief   Capture edge count raw interrupt occurs
 *          This register shows the state of internal interrupt signal.
 *          1 : interrupt
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_capint_ris_cnt_ch_get_sta(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    return (((pwm_base->CAPRIS) & WJ_PWM_CAPINT_RIS_CNT_Msk(ch)) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   Capture edge time raw interrupt occurs
 *          This register shows the state of internal interrupt signal.
 *          1 : interrupt
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_capint_ris_time_ch_get_sta(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    return (((pwm_base->CAPRIS) & WJ_PWM_CAPINT_RIS_TIM_Msk(ch)) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   Capture edge count interrupt clear
 *          1 : clear interrupt
 *          0 : not clear interrupt
*/
static inline void wj_pwm_capint_ic_cnt_ch_en(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->CAPIC |= WJ_PWM_CAPINT_IC_CNT_EN(ch);
}
static inline void wj_pwm_capint_ic_cnt_ch_dis(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->CAPIC &= ~WJ_PWM_CAPINT_IC_CNT_EN(ch);
}
static inline uint32_t wj_pwm_capint_ic_cnt_ch_get_sta(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    return (((pwm_base->CAPIC) & WJ_PWM_CAPINT_IC_CNT_Msk(ch)) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   Capture edge time interrupt clear
 *          1 : clear interrupt
 *          0 : not clear interrupt
*/
static inline void wj_pwm_capint_ic_time_ch_en(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->CAPIC |= WJ_PWM_CAPINT_IC_TIM_EN(ch);
}
static inline void wj_pwm_capint_ic_time_ch_dis(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    pwm_base->CAPIC &= ~WJ_PWM_CAPINT_IC_TIM_EN(ch);
}
static inline uint32_t wj_pwm_capint_ic_time_ch_get_sta(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    return (((pwm_base->CAPIC) & WJ_PWM_CAPINT_IC_TIM_Msk(ch)) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   Capture edge count interrupt occurs
 *          1 : interrupt
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_capint_is_cnt_ch_get_sta(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    return (((pwm_base->CAPIS) & (WJ_PWM_CAPINT_IS_CNT_0_Msk << ch)) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   Capture edge time interrupt occurs
 *          1 : interrupt
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_capint_is_time_ch_get_sta(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    return (((pwm_base->CAPIS) & (WJ_PWM_CAPINT_IS_TIM_0_Msk << ch)) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   Input capture counter value
 *          This register contain the counter value when edge match.
*/

static inline uint32_t wj_pwm_capture_read_count_ch(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    return (((pwm_base->CAP01T + (ch >> 1U)) & WJ_PWM_CAPTURE_COUNT_CH_Msk(ch)) >> WJ_PWM_CAPTURE_COUNT_CH_Pos(ch));
}

/**
 * \brief   Capture match m value
 *          These registers contain the load value for the PWM counter.
*/
static inline uint32_t wj_pwm_capture_read_match_ch(wj_pwm_regs_t *pwm_base, uint32_t ch)
{
    return (((pwm_base->CAP01MATCH + (ch >> 1U)) & WJ_PWM_CAPTURE_MATCH_CH_Msk(ch)) >> WJ_PWM_CAPTURE_MATCH_CH_Pos(ch));

}
static inline void wj_pwm_capture_write_match_ch(wj_pwm_regs_t *pwm_base, uint32_t ch, uint32_t value)
{
    *(&pwm_base->CAP01MATCH + (ch >> 1U)) &= ~WJ_PWM_CAPTURE_MATCH_CH_Msk(ch);
    *(&pwm_base->CAP01MATCH + (ch >> 1U)) |= (value << WJ_PWM_CAPTURE_MATCH_CH_Pos(ch));
}

/**
 * \brief   A interrupt enable when counter mathc the CMPnmMATCH value.
 *          1 : interrupt enable
 *          0 : interrupt disable
*/
static inline void wj_pwm_timint_ie_ch_0_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMINTEN |= WJ_PWM_TIMINT_IE_0_EN;
}
static inline void wj_pwm_timint_ie_ch_0_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMINTEN &= ~WJ_PWM_TIMINT_IE_0_EN;
}
static inline uint32_t wj_pwm_timint_ie_ch_0_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMINTEN) & WJ_PWM_TIMINT_IE_0_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_pwm_timint_ie_ch_1_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMINTEN |= WJ_PWM_TIMINT_IE_1_EN;
}
static inline void wj_pwm_timint_ie_ch_1_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMINTEN &= ~WJ_PWM_TIMINT_IE_1_EN;
}
static inline uint32_t wj_pwm_timint_ie_ch_1_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMINTEN) & WJ_PWM_TIMINT_IE_1_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_pwm_timint_ie_ch_2_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMINTEN |= WJ_PWM_TIMINT_IE_2_EN;
}
static inline void wj_pwm_timint_ie_ch_2_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMINTEN &= ~WJ_PWM_TIMINT_IE_2_EN;
}
static inline uint32_t wj_pwm_timint_ie_ch_2_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMINTEN) & WJ_PWM_TIMINT_IE_2_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_pwm_timint_ie_ch_3_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMINTEN |= WJ_PWM_TIMINT_IE_3_EN;
}
static inline void wj_pwm_timint_ie_ch_3_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMINTEN &= ~WJ_PWM_TIMINT_IE_3_EN;
}
static inline uint32_t wj_pwm_timint_ie_ch_3_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMINTEN) & WJ_PWM_TIMINT_IE_3_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_pwm_timint_ie_ch_4_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMINTEN |= WJ_PWM_TIMINT_IE_4_EN;
}
static inline void wj_pwm_timint_ie_ch_4_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMINTEN &= ~WJ_PWM_TIMINT_IE_4_EN;
}
static inline uint32_t wj_pwm_timint_ie_ch_4_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMINTEN) & WJ_PWM_TIMINT_IE_4_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_pwm_timint_ie_ch_5_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMINTEN |= WJ_PWM_TIMINT_IE_5_EN;
}
static inline void wj_pwm_timint_ie_ch_5_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMINTEN &= ~WJ_PWM_TIMINT_IE_5_EN;
}
static inline uint32_t wj_pwm_timint_ie_ch_5_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMINTEN) & WJ_PWM_TIMINT_IE_5_Msk) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   A interrupt enable when counter mathc the CMPnmMATCH value.
 *          1 : interrupt enable
 *          0 : interrupt disable
*/
static inline uint32_t wj_pwm_timint_ris_ch_0_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMRIS) & WJ_PWM_TIMINT_RIS_0_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline uint32_t wj_pwm_timint_ris_ch_1_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMRIS) & WJ_PWM_TIMINT_RIS_1_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline uint32_t wj_pwm_timint_ris_ch_2_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMRIS) & WJ_PWM_TIMINT_RIS_2_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline uint32_t wj_pwm_timint_ris_ch_3_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMRIS) & WJ_PWM_TIMINT_RIS_3_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline uint32_t wj_pwm_timint_ris_ch_4_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMRIS) & WJ_PWM_TIMINT_RIS_4_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline uint32_t wj_pwm_timint_ris_ch_5_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMRIS) & WJ_PWM_TIMINT_RIS_5_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Timer interrupt
 *          This register clear internal interrupt signal.
 *          1 : clear interrupt
 *          0 : not clear interrupt
*/
static inline void wj_pwm_timint_ic_ch_0_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMIC |= WJ_PWM_TIMINT_IC_0_EN;
}
static inline void wj_pwm_timint_ic_ch_0_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMIC &= ~WJ_PWM_TIMINT_IC_0_EN;
}
static inline uint32_t wj_pwm_timint_ic_ch_0_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMIC) & WJ_PWM_TIMINT_IC_0_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_pwm_timint_ic_ch_1_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMIC |= WJ_PWM_TIMINT_IC_1_EN;
}
static inline void wj_pwm_timint_ic_ch_1_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMIC &= ~WJ_PWM_TIMINT_IC_1_EN;
}
static inline uint32_t wj_pwm_timint_ic_ch_1_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMIC) & WJ_PWM_TIMINT_IC_1_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_pwm_timint_ic_ch_2_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMIC |= WJ_PWM_TIMINT_IC_2_EN;
}
static inline void wj_pwm_timint_ic_ch_2_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMIC &= ~WJ_PWM_TIMINT_IC_2_EN;
}
static inline uint32_t wj_pwm_timint_ic_ch_2_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMIC) & WJ_PWM_TIMINT_IC_2_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_pwm_timint_ic_ch_3_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMIC |= WJ_PWM_TIMINT_IC_3_EN;
}
static inline void wj_pwm_timint_ic_ch_3_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMIC &= ~WJ_PWM_TIMINT_IC_3_EN;
}
static inline uint32_t wj_pwm_timint_ic_ch_3_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMIC) & WJ_PWM_TIMINT_IC_3_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_pwm_timint_ic_ch_4_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMIC |= WJ_PWM_TIMINT_IC_4_EN;
}
static inline void wj_pwm_timint_ic_ch_4_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMIC &= ~WJ_PWM_TIMINT_IC_4_EN;
}
static inline uint32_t wj_pwm_timint_ic_ch_4_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMIC) & WJ_PWM_TIMINT_IC_4_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_pwm_timint_ic_ch_5_en(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMIC |= WJ_PWM_TIMINT_IC_5_EN;
}
static inline void wj_pwm_timint_ic_ch_5_dis(wj_pwm_regs_t *pwm_base)
{
    pwm_base->TIMIC &= ~WJ_PWM_TIMINT_IC_5_EN;
}
static inline uint32_t wj_pwm_timint_ic_ch_5_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMIC) & WJ_PWM_TIMINT_IC_5_Msk) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   Timer interrupt occurs
 *          1 : interrupt
 *          0 : no interrupt
*/
static inline uint32_t wj_pwm_timint_is_ch_0_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMIS) & WJ_PWM_TIMINT_IS_0_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline uint32_t wj_pwm_timint_is_ch_1_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMIS) & WJ_PWM_TIMINT_IS_1_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline uint32_t wj_pwm_timint_is_ch_2_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMIS) & WJ_PWM_TIMINT_IS_2_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline uint32_t wj_pwm_timint_is_ch_3_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMIS) & WJ_PWM_TIMINT_IS_3_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline uint32_t wj_pwm_timint_is_ch_4_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMIS) & WJ_PWM_TIMINT_IS_4_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline uint32_t wj_pwm_timint_is_ch_5_get_sta(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIMIS) & WJ_PWM_TIMINT_IS_5_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   Time load n value
 *          These registers contain the load value for the PWM counter.
*/
static inline uint32_t wj_pwm_time_read_load_ch_0(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIM01LOAD) & WJ_PWM_TIME_LOAD_CH_0_Msk) >> WJ_PWM_TIME_LOAD_CH_0_Pos);
}
static inline void wj_pwm_time_write_load_ch_0(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->TIM01LOAD &= ~WJ_PWM_TIME_LOAD_CH_0_Msk;
    pwm_base->TIM01LOAD |= (value << WJ_PWM_TIME_LOAD_CH_0_Pos);
}
/**
 * \brief   Time load m value
 *          These registers contain the load value for the PWM counter.
*/
static inline uint32_t wj_pwm_time_read_load_ch_1(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIM01LOAD) & WJ_PWM_TIME_LOAD_CH_1_Msk) >> WJ_PWM_TIME_LOAD_CH_1_Pos);
}
static inline void wj_pwm_time_write_load_ch_1(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->TIM01LOAD &= ~WJ_PWM_TIME_LOAD_CH_1_Msk;
    pwm_base->TIM01LOAD |= (value << WJ_PWM_TIME_LOAD_CH_1_Pos);
}
/**
 * \brief   Time load n value
 *          These registers contain the load value for the PWM counter.
*/
static inline uint32_t wj_pwm_time_read_load_ch_2(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIM23LOAD) & WJ_PWM_TIME_LOAD_CH_2_Msk) >> WJ_PWM_TIME_LOAD_CH_2_Pos);
}
static inline void wj_pwm_time_write_load_ch_2(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->TIM23LOAD &= ~WJ_PWM_TIME_LOAD_CH_2_Msk;
    pwm_base->TIM23LOAD |= (value << WJ_PWM_TIME_LOAD_CH_2_Pos);
}
/**
 * \brief   Time load m value
 *          These registers contain the load value for the PWM counter.
*/
static inline uint32_t wj_pwm_time_read_load_ch_3(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIM23LOAD) & WJ_PWM_TIME_LOAD_CH_3_Msk) >> WJ_PWM_TIME_LOAD_CH_3_Pos);
}
static inline void wj_pwm_time_write_load_ch_3(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->TIM23LOAD &= ~WJ_PWM_TIME_LOAD_CH_3_Msk;
    pwm_base->TIM23LOAD |= (value << WJ_PWM_TIME_LOAD_CH_3_Pos);
}
/**
 * \brief   Time load n value
 *          These registers contain the load value for the PWM counter.
*/
static inline uint32_t wj_pwm_time_read_load_ch_4(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIM45LOAD) & WJ_PWM_TIME_LOAD_CH_4_Msk) >> WJ_PWM_TIME_LOAD_CH_4_Pos);
}
static inline void wj_pwm_time_write_load_ch_4(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->TIM45LOAD &= ~WJ_PWM_TIME_LOAD_CH_4_Msk;
    pwm_base->TIM45LOAD |= (value << WJ_PWM_TIME_LOAD_CH_4_Pos);
}
/**
 * \brief   Time load m value
 *          These registers contain the load value for the PWM counter.
*/
static inline uint32_t wj_pwm_time_read_load_ch_5(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIM45LOAD) & WJ_PWM_TIME_LOAD_CH_5_Msk) >> WJ_PWM_TIME_LOAD_CH_5_Pos);
}
static inline void wj_pwm_time_write_load_ch_5(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->TIM45LOAD &= ~WJ_PWM_TIME_LOAD_CH_5_Msk;
    pwm_base->TIM45LOAD |= (value << WJ_PWM_TIME_LOAD_CH_5_Pos);
}

/**
 * \brief   Time count n value
 *          These registers contain the count value for the PWM counter.
*/
static inline uint32_t wj_pwm_time_read_count_ch_0(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIM01COUNT) & WJ_PWM_TIME_COUNT_CH_0_Msk) >> WJ_PWM_TIME_COUNT_CH_0_Pos);
}
static inline void wj_pwm_time_write_count_ch_0(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->TIM01COUNT &= ~WJ_PWM_TIME_COUNT_CH_0_Msk;
    pwm_base->TIM01COUNT |= (value << WJ_PWM_TIME_COUNT_CH_0_Pos);
}
/**
 * \brief   Time count m value
 *          These registers contain the count value for the PWM counter.
*/
static inline uint32_t wj_pwm_time_read_count_ch_1(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIM01COUNT) & WJ_PWM_TIME_COUNT_CH_1_Msk) >> WJ_PWM_TIME_COUNT_CH_1_Pos);
}
static inline void wj_pwm_time_write_count_ch_1(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->TIM01COUNT &= ~WJ_PWM_TIME_COUNT_CH_1_Msk;
    pwm_base->TIM01COUNT |= (value << WJ_PWM_TIME_COUNT_CH_1_Pos);
}
/**
 * \brief   Time count n value
 *          These registers contain the count value for the PWM counter.
*/
static inline uint32_t wj_pwm_time_read_count_ch_2(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIM23COUNT) & WJ_PWM_TIME_COUNT_CH_2_Msk) >> WJ_PWM_TIME_COUNT_CH_2_Pos);
}
static inline void wj_pwm_time_write_count_ch_2(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->TIM23COUNT &= ~WJ_PWM_TIME_COUNT_CH_2_Msk;
    pwm_base->TIM23COUNT |= (value << WJ_PWM_TIME_COUNT_CH_2_Pos);
}
/**
 * \brief   Time count m value
 *          These registers contain the count value for the PWM counter.
*/
static inline uint32_t wj_pwm_time_read_count_ch_3(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIM23COUNT) & WJ_PWM_TIME_COUNT_CH_3_Msk) >> WJ_PWM_TIME_COUNT_CH_3_Pos);
}
static inline void wj_pwm_time_write_count_ch_3(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->TIM23COUNT &= ~WJ_PWM_TIME_COUNT_CH_3_Msk;
    pwm_base->TIM23COUNT |= (value << WJ_PWM_TIME_COUNT_CH_3_Pos);
}
/**
 * \brief   Time count n value
 *          These registers contain the count value for the PWM counter.
*/
static inline uint32_t wj_pwm_time_read_count_ch_4(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIM45COUNT) & WJ_PWM_TIME_COUNT_CH_4_Msk) >> WJ_PWM_TIME_COUNT_CH_4_Pos);
}
static inline void wj_pwm_time_write_count_ch_4(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->TIM45COUNT &= ~WJ_PWM_TIME_COUNT_CH_4_Msk;
    pwm_base->TIM45COUNT |= (value << WJ_PWM_TIME_COUNT_CH_4_Pos);
}
/**
 * \brief   Time count m value
 *          These registers contain the count value for the PWM counter.
*/
static inline uint32_t wj_pwm_time_read_count_ch_5(wj_pwm_regs_t *pwm_base)
{
    return (((pwm_base->TIM45COUNT) & WJ_PWM_TIME_COUNT_CH_5_Msk) >> WJ_PWM_TIME_COUNT_CH_5_Pos);
}
static inline void wj_pwm_time_write_count_ch_5(wj_pwm_regs_t *pwm_base, uint32_t value)
{
    pwm_base->TIM45COUNT &= ~WJ_PWM_TIME_COUNT_CH_5_Msk;
    pwm_base->TIM45COUNT |= (value << WJ_PWM_TIME_COUNT_CH_5_Pos);
}


#ifdef __cplusplus
}
#endif

#endif
