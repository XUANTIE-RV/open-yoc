/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_adc_ll.h
 * @brief
 * @version
 * @date     2020-05-09
 ******************************************************************************/

#ifndef _WJ_ADC_LL_H_
#define _WJ_ADC_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IP_ID_OFFSET    0xFC
/* ADC_CTRL, offset: 0x00 */
#define WJ_ADC_CTRL_ADC_START_Pos                      (0U)
#define WJ_ADC_CTRL_ADC_START_Msk                      (0x1U << WJ_ADC_CTRL_ADC_START_Pos)
#define WJ_ADC_CTRL_ADC_START_EN                       WJ_ADC_CTRL_ADC_START_Msk

#define WJ_ADC_CTRL_ADC_STOP_Pos                       (1U)
#define WJ_ADC_CTRL_ADC_STOP_Msk                       (0x1U << WJ_ADC_CTRL_ADC_STOP_Pos)
#define WJ_ADC_CTRL_ADC_STOP_EN                        WJ_ADC_CTRL_ADC_STOP_Msk

#define WJ_ADC_CTRL_ADC_DATA_READY_Pos                 (16U)
#define WJ_ADC_CTRL_ADC_DATA_READY_Msk                 (0x1U << WJ_ADC_CTRL_ADC_DATA_READY_Pos)

#define WJ_ADC_CTRL_ADC_DATA_OVERFLOW_Pos              (17U)
#define WJ_ADC_CTRL_ADC_DATA_OVERFLOW_Msk              (0x1U << WJ_ADC_CTRL_ADC_DATA_OVERFLOW_Pos)

#define WJ_ADC_CTRL_ADC_DATA_IDLE_Pos                  (18U)
#define WJ_ADC_CTRL_ADC_DATA_IDLE_Msk                  (0x1U << WJ_ADC_CTRL_ADC_DATA_IDLE_Pos)

/* CHANNEL_SEL, offset: 0x04 */
#define WJ_ADC_CHANNEL_SEL_Pos                         (0U)
#define WJ_ADC_CHANNEL_SEL_Msk                         (0xFFFFU << WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN1		                   (0U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN2		                   (1U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN3		                   (2U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN4		                   (3U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN5		                   (4U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN6		                   (5U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN7		                   (6U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN8		                   (7U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN9		                   (8U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN10	                   (9U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN11	                   (10<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN12	                   (11<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN13	                   (12<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN14	                   (13<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN15	                   (14<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN16	                   (15<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN1_9	                   (0U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN2_10	                   (1U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN3_11	                   (2U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN4_12	                   (3U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN5_13	                   (4U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN6_14	                   (5U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN7_15	                   (6U<<WJ_ADC_CHANNEL_SEL_Pos)
#define WJ_ADC_CHANNEL_SEL_VIN8_16	                   (7U<<WJ_ADC_CHANNEL_SEL_Pos)\

#define WJ_ADC_DIRECTION_Pos                           (16U)
#define WJ_ADC_DIRECTION_Msk                           (0x1U << WJ_ADC_DIRECTION_Pos)
#define WJ_ADC_DIRECTION_BIG                           WJ_ADC_DIRECTION_Msk
#define WJ_ADC_DIRECTION_LITTE                         (0U)

/* ADC_CONFIG0, offset: 0x08 */
#define WJ_ADC_CONFIG0_LOW_POWER_MODE_Pos              (0U)
#define WJ_ADC_CONFIG0_LOW_POWER_MODE_Msk              (0x1U << WJ_ADC_CONFIG0_LOW_POWER_MODE_Pos)
#define WJ_ADC_CONFIG0_LOW_POWER_MODE_EN               WJ_ADC_CONFIG0_LOW_POWER_MODE_Msk

#define WJ_ADC_CONFIG0_WAIT_MODE_Pos                   (1U)
#define WJ_ADC_CONFIG0_WAIT_MODE_Msk                   (0x1U << WJ_ADC_CONFIG0_WAIT_MODE_Pos)
#define WJ_ADC_CONFIG0_WAIT_MODE_EN                    WJ_ADC_CONFIG0_WAIT_MODE_Msk

#define WJ_ADC_CONFIG0_WORK_MODE_Pos                   (2U)
#define WJ_ADC_CONFIG0_WORK_MODE_Msk                   (0x3U << WJ_ADC_CONFIG0_WORK_MODE_Pos)
#define WJ_ADC_CONFIG0_WORK_MODE_SINGLE                (0x0U << WJ_ADC_CONFIG0_WORK_MODE_Pos)
#define WJ_ADC_CONFIG0_WORK_MODE_CONTINUE              (0x1U << WJ_ADC_CONFIG0_WORK_MODE_Pos)
#define WJ_ADC_CONFIG0_WORK_MODE_SCAN                  (0x2U << WJ_ADC_CONFIG0_WORK_MODE_Pos)

#define WJ_ADC_CONFIG0_CLK_DIV_Pos                     (4U)
#define WJ_ADC_CONFIG0_CLK_DIV_Msk                     (0xFU << WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_2	                   (0U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_4	                   (1U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_6	                   (2U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_8	                   (3U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_10	                   (4U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_12	                   (5U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_14	                   (6U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_16	                   (7U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_18	                   (8U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_20	                   (9U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_22	                   (10U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_24	                   (11U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_26	                   (12U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_28	                   (13U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_30	                   (14U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)
#define WJ_ADC_CONFIG0_CLK_DIV_1_32	                   (15U<< WJ_ADC_CONFIG0_CLK_DIV_Pos)

#define WJ_ADC_CONFIG0_CONV_NUM_Pos                    (8U)
#define WJ_ADC_CONFIG0_CONV_NUM_Msk                    (0x7U << WJ_ADC_CONFIG0_CONV_NUM_Pos)
#define WJ_ADC_CONFIG0_CONV_NUM_0	                   (0U<< WJ_ADC_CONFIG0_CONV_NUM_Pos)
#define WJ_ADC_CONFIG0_CONV_NUM_1	                   (1U<< WJ_ADC_CONFIG0_CONV_NUM_Pos)
#define WJ_ADC_CONFIG0_CONV_NUM_7	                   (7U<< WJ_ADC_CONFIG0_CONV_NUM_Pos)

#define WJ_ADC_CONFIG0_CONVERT_TYPE_Pos                (12U)
#define WJ_ADC_CONFIG0_CONVERT_TYPE_Msk                (0x1U << WJ_ADC_CONFIG0_CONVERT_TYPE_Pos)
#define WJ_ADC_CONFIG0_CONVERT_TYPE_SINGEL	           (0U<< WJ_ADC_CONFIG0_CONVERT_TYPE_Pos)
#define WJ_ADC_CONFIG0_CONVERT_TYPE_DIFFERENTIAL       (1U<< WJ_ADC_CONFIG0_CONVERT_TYPE_Pos)

#define WJ_ADC_CONFIG0_CONVERT_TYPE_Pos                (12U)
#define WJ_ADC_CONFIG0_CONVERT_TYPE_Msk                (0x1U << WJ_ADC_CONFIG0_CONVERT_TYPE_Pos)
#define WJ_ADC_CONFIG0_CONVERT_TYPE_SINGEL	           (0U<< WJ_ADC_CONFIG0_CONVERT_TYPE_Pos)
#define WJ_ADC_CONFIG0_CONVERT_TYPE_DIFFERENTIAL       (1U<< WJ_ADC_CONFIG0_CONVERT_TYPE_Pos)

#define WJ_ADC_CONFIG0_DATA_TYPE_Pos                   (13U)
#define WJ_ADC_CONFIG0_DATA_TYPE_Msk                   (0x1U << WJ_ADC_CONFIG0_DATA_TYPE_Pos)
#define WJ_ADC_CONFIG0_DATA_TYPE_UNSIGNED              (0U<< WJ_ADC_CONFIG0_CONVERT_TYPE_Pos)
#define WJ_ADC_CONFIG0_DATA_TYPE_SIGNED                (1U<< WJ_ADC_CONFIG0_CONVERT_TYPE_Pos)

#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos          (16U)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Msk          (0xFU << WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_1	            (0U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_2	            (1U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_3	            (2U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_4	            (3U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_5	            (4U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_6	            (5U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_7	            (6U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_8	            (7U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_9	            (8U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_10            (9U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_11            (10U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_12            (11U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_13            (12U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_14            (13U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_15            (14U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)
#define WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_16            (15U<< WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos)

/* ADC_CONFIG1, offset: 0x0C */
#define WJ_ADC_SAMPLE_CYCLE_Pos                        (0U)
#define WJ_ADC_SAMPLE_CYCLE_Msk                        (0xFU << WJ_ADC_SAMPLE_CYCLE_Pos)

#define WJ_ADC_CONVERT_CYCLE_Pos                       (4U)
#define WJ_ADC_CONVERT_CYCLE_Msk                       (0xFU << WJ_ADC_CONVERT_CYCLE_Pos)

/* ADC_INT_EN, offset: 0x10 */
#define WJ_ADC_EN_ONE_CONVERT_DONE_Pos                 (0U)
#define WJ_ADC_EN_ONE_CONVERT_DONE_Msk                 (0x1U << WJ_ADC_EN_ONE_CONVERT_DONE_Pos)
#define WJ_ADC_EN_ONE_CONVERT_DONE_EN                  WJ_ADC_EN_ONE_CONVERT_DONE_Msk

#define WJ_ADC_EN_ONE_CHANNNEL_DONE_Pos                 (1U)
#define WJ_ADC_EN_ONE_CHANNNEL_DONE_Msk                 (0x1U << WJ_ADC_EN_ONE_CHANNNEL_DONE_Pos)
#define WJ_ADC_EN_ONE_CHANNNEL_DONE_EN                  WJ_ADC_EN_ONE_CHANNNEL_DONE_Msk

#define WJ_ADC_EN_ONE_SEQUENCE_DONE_Pos                (2U)
#define WJ_ADC_EN_ONE_SEQUENCE_DONE_Msk                (0x1U << WJ_ADC_EN_ONE_SEQUENCE_DONE_Pos)
#define WJ_ADC_EN_ONE_SEQUENCE_DONE_EN                 WJ_ADC_EN_ONE_SEQUENCE_DONE_Msk

#define WJ_ADC_EN_ONE_OVERFLOW_Pos                     (3U)
#define WJ_ADC_EN_ONE_OVERFLOW_Msk                     (0x1U << WJ_ADC_EN_ONE_OVERFLOW_Pos)
#define WJ_ADC_EN_ONE_OVERFLOW_EN                      WJ_ADC_EN_ONE_OVERFLOW_Msk

#define WJ_ADC_EN_WATCHDOG_Pos                         (4U)
#define WJ_ADC_EN_WATCHDOG_Msk                         (0x1U << WJ_ADC_EN_WATCHDOG_Pos)
#define WJ_ADC_EN_WATCHDOG_EN                          WJ_ADC_EN_WATCHDOG_Msk

/* INT_STATUS, offset: 0x14 */
#define WJ_ADC_INT_ONE_CONVERT_DONE_Pos                 (0U)
#define WJ_ADC_INT_ONE_CONVERT_DONE_Msk                 (0x1U << WJ_ADC_INT_ONE_CONVERT_DONE_Pos)
#define WJ_ADC_INT_ONE_CONVERT_DONE_EN                  WJ_ADC_EN_ONE_CONVERT_DONE_Msk

#define WJ_ADC_INT_ONE_CONNNEL_DONE_Pos                 (1U)
#define WJ_ADC_INT_ONE_CONNNEL_DONE_Msk                 (0x1U << WJ_ADC_INT_ONE_CONNNEL_DONE_Pos)
#define WJ_ADC_INT_ONE_CONNNEL_DONE_EN                  WJ_ADC_INT_ONE_CONNNEL_DONE_Msk

#define WJ_ADC_INT_ONE_SEQUENCE_DONE_Pos                (2U)
#define WJ_ADC_INT_ONE_SEQUENCE_DONE_Msk                (0x1U << WJ_ADC_INT_ONE_SEQUENCE_DONE_Pos)
#define WJ_ADC_INT_ONE_SEQUENCE_DONE_EN                 WJ_ADC_EN_ONE_SEQUENCE_DONE_Msk

#define WJ_ADC_INT_ONE_OVERFLOW_Pos                     (3U)
#define WJ_ADC_INT_ONE_OVERFLOW_Msk                     (0x1U << WJ_ADC_INT_ONE_OVERFLOW_Pos)
#define WJ_ADC_INT_ONE_OVERFLOW_EN                      WJ_ADC_EN_ONE_OVERFLOW_Msk

#define WJ_ADC_INT_WATCHDOG_Pos                         (4U)
#define WJ_ADC_INT_WATCHDOG_Msk                         (0x1U << WJ_ADC_INT_WATCHDOG_Pos)
#define WJ_ADC_INT_WATCHDOG_EN                          WJ_ADC_EN_WATCHDOG_Msk

/* ADC_DATA, offset: 0x18 */
#define WJ_ADC_DATA_Pos                                (0U)
#define WJ_ADC_DATA_Msk                                (0xFFFU << WJ_ADC_DATA_Pos)

/* ADC_CMP_DATA, offset: 0x18 */
#define WJ_ADC_CMP_DATA_L_Pos                          (0U)
#define WJ_ADC_CMP_DATA_L_Msk                          (0xFFFU << WJ_ADC_CMP_DATA_L_Pos)

#define WJ_ADC_CMP_DATA_H_Pos                          (12U)
#define WJ_ADC_CMP_DATA_H_Msk                          (0xFFFU << WJ_ADC_CMP_DATA_H_Pos)

/* ADC_ANA_REGISTER, offset: 0x30 */
#define WJ_ADC_EN_Pos                             (0U)
#define WJ_ADC_EN_Msk                             (0x1U << WJ_ADC_EN_Pos)
#define WJ_ADC_EN                                 WJ_ADC_EN_Msk

#define WJ_ADC_RSTN_Pos                           (1U)
#define WJ_ADC_RSTN_Msk                           (0x1U << WJ_ADC_RSTN_Pos)
#define WJ_ADC_RSTN_RESET                         (0U)
#define WJ_ADC_RSTN_ACTIVE                        WJ_ADC_RSTN_Msk

#define WJ_ADC_LATCH_M_Pos                        (2U)
#define WJ_ADC_LATCH_M_Msk                        (0x1U << WJ_ADC_LATCH_M_Pos)
#define WJ_ADC_LATCH_M_STATIC                     (0U)
#define WJ_ADC_LATCH_M_DYNAMIC                    WJ_ADC_LATCH_M_Msk

#define WJ_ADC_ENV_REFINT_Pos                     (3U)
#define WJ_ADC_ENV_REFINT_Msk                     (0x1U << WJ_ADC_ENV_REFINT_Pos)
#define WJ_ADC_ENV_REFINT_EXTERNAL                (0U)
#define WJ_ADC_ENV_REFINT_INTERNAL                WJ_ADC_ENV_REFINT_Msk

#define WJ_ADC_BIAS_C_Pos                         (4U)
#define WJ_ADC_BIAS_C_Msk                         (0x7U << WJ_ADC_ENV_REFINT_Pos)
#define WJ_ADC_BIAS_C_0_25                        (0U)
#define WJ_ADC_BIAS_C_0_50                        (0x1U << WJ_ADC_ENV_REFINT_Pos)
#define WJ_ADC_BIAS_C_0_75                        (0x2U << WJ_ADC_ENV_REFINT_Pos)
#define WJ_ADC_BIAS_C_1                           (0x3U << WJ_ADC_ENV_REFINT_Pos)
#define WJ_ADC_BIAS_C_1_25                        (0x4U << WJ_ADC_ENV_REFINT_Pos)
#define WJ_ADC_BIAS_C_1_50                        (0x5U << WJ_ADC_ENV_REFINT_Pos)
#define WJ_ADC_BIAS_C_1_75                        (0x6U << WJ_ADC_ENV_REFINT_Pos)
#define WJ_ADC_BIAS_C_2                           (0x7U << WJ_ADC_ENV_REFINT_Pos)

#define WJ_ADC_TTRIM_Pos                         (8U)
#define WJ_ADC_TTRIM_Msk                         (0xfU << WJ_ADC_IE_OSSCEDIE_Pos)

#define WJ_ADC_VTRIM_Pos                         (12U)
#define WJ_ADC_VTRIM_Msk                         (0xfU << WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_381                       (0b0   <<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_398                       (0b1   <<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_415                       (0b10  <<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_432                       (0b11  <<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_449                       (0b100 <<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_466                       (0b101 <<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_483                       (0b110 <<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_5                         (0b111 <<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_517                       (0b1000<<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_534                       (0b1001<<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_551                       (0b1010<<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_568                       (0b1011<<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_585                       (0b1100<<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_602                       (0b1101<<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_619                       (0b1110<<WJ_ADC_IE_OSSCEDIE_Pos)
#define WJ_ADC_VTRIM_1_636                       (0b1111<<WJ_ADC_IE_OSSCEDIE_Pos)


/* IP_ID, offset: 0xFC */
#define WJ_ADC_IP_ID_Pos                         (0U)
#define WJ_ADC_IP_ID_Msk                         (0xFFFFFFFFU << WJ_ADC_IP_ID_Pos)


typedef struct {
    __IOM uint32_t ADC_CTRL;
    __IOM uint32_t CHANNEL_SEL;
    __IOM uint32_t ADC_CONFIG0;
    __IOM uint32_t ADC_CONFIG1;
    __IOM uint32_t ADC_INT_EN;
    __IOM uint32_t ADC_INT_STATUS;
    __IOM uint32_t ADC_DATA;
    __IOM uint32_t ADC_CMP_VALUE;
    __IOM uint32_t RESERVER[4];
    __IOM uint32_t ADC_ANA_REGISTER;
} wj_adc_regs_t;


static inline void wj_adc_en_start(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CTRL &= ~WJ_ADC_CTRL_ADC_START_Msk;
    adc_base->ADC_CTRL |= WJ_ADC_CTRL_ADC_START_EN;
}

static inline uint32_t wj_adc_get_start(wj_adc_regs_t *adc_base)
{
    return (adc_base->ADC_CTRL & WJ_ADC_CTRL_ADC_START_Msk);
}
static inline void wj_adc_en_stop(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CTRL &= ~WJ_ADC_CTRL_ADC_STOP_Msk;
    adc_base->ADC_CTRL |= WJ_ADC_CTRL_ADC_STOP_EN;
}
static inline uint32_t wj_adc_get_stop(wj_adc_regs_t *adc_base)
{
    return (adc_base->ADC_CTRL & WJ_ADC_CTRL_ADC_STOP_Msk);
}

static inline uint32_t wj_adc_get_data_ready(wj_adc_regs_t *adc_base)
{
    return (adc_base->ADC_CTRL & WJ_ADC_CTRL_ADC_DATA_READY_Msk);
}

static inline uint32_t wj_adc_get_overflow(wj_adc_regs_t *adc_base)
{
    return (adc_base->ADC_CTRL & WJ_ADC_CTRL_ADC_DATA_OVERFLOW_Msk);
}

static inline uint32_t wj_adc_get_idle(wj_adc_regs_t *adc_base)
{
    return (adc_base->ADC_CTRL & WJ_ADC_CTRL_ADC_DATA_IDLE_Msk);
}

static inline void wj_adc_set_sel_channel(wj_adc_regs_t *adc_base, uint32_t value)
{
    // adc_base->CHANNEL_SEL &= ~WJ_ADC_CHANNEL_SEL_Msk;
    adc_base->CHANNEL_SEL |= (WJ_ADC_CHANNEL_SEL_Msk & value);
}

static inline void wj_adc_reset_sel_channel(wj_adc_regs_t *adc_base, uint32_t value)
{
    uint32_t temp = (adc_base->CHANNEL_SEL & WJ_ADC_CHANNEL_SEL_Msk);
    adc_base->CHANNEL_SEL &= ~WJ_ADC_CHANNEL_SEL_Msk;
    adc_base->CHANNEL_SEL |= (temp & (~value));
}

static inline uint32_t wj_adc_get_sel_channel(wj_adc_regs_t *adc_base)
{
    return (adc_base->CHANNEL_SEL & WJ_ADC_CHANNEL_SEL_Msk);
}

static inline void wj_adc_set_big_end(wj_adc_regs_t *adc_base)
{
    adc_base->CHANNEL_SEL &= ~WJ_ADC_DIRECTION_Msk;
    adc_base->CHANNEL_SEL |= WJ_ADC_DIRECTION_BIG;
}

static inline void wj_adc_set_little_end(wj_adc_regs_t *adc_base)
{
    adc_base->CHANNEL_SEL &= ~WJ_ADC_DIRECTION_Msk;
    adc_base->CHANNEL_SEL |= WJ_ADC_DIRECTION_LITTE;
}

static inline uint32_t wj_adc_get_end_type(wj_adc_regs_t *adc_base)
{
    return (adc_base->CHANNEL_SEL & WJ_ADC_DIRECTION_Msk);
}

static inline void wj_adc_en_low_power_mode(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CONFIG0 &= ~WJ_ADC_CONFIG0_LOW_POWER_MODE_Msk;
    adc_base->ADC_CONFIG0 |= WJ_ADC_CONFIG0_LOW_POWER_MODE_EN;
}

static inline void wj_adc_dis_low_power_mode(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CONFIG0 &= ~WJ_ADC_CONFIG0_LOW_POWER_MODE_Msk;
}

static inline void wj_adc_en_wait_mode(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CONFIG0 &= ~WJ_ADC_CONFIG0_WAIT_MODE_Msk;
    adc_base->ADC_CONFIG0 |= WJ_ADC_CONFIG0_WAIT_MODE_EN;
}

static inline void wj_adc_dis_wait_mode(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CONFIG0 &= ~WJ_ADC_CONFIG0_WAIT_MODE_Msk;
}

static inline void wj_adc_set_work_mode(wj_adc_regs_t *adc_base, uint32_t value)
{
    adc_base->ADC_CONFIG0 &= ~WJ_ADC_CONFIG0_WORK_MODE_Msk;
    adc_base->ADC_CONFIG0 |= (WJ_ADC_CHANNEL_SEL_Msk & value);
}

static inline uint32_t wj_adc_get_work_mode(wj_adc_regs_t *adc_base)
{
    return (adc_base->ADC_CONFIG0 & WJ_ADC_CONFIG0_WORK_MODE_Msk);
}

static inline void wj_adc_set_clk_div(wj_adc_regs_t *adc_base, uint32_t value)
{
    adc_base->ADC_CONFIG0 &= ~WJ_ADC_CONFIG0_CLK_DIV_Msk;
    adc_base->ADC_CONFIG0 |= (WJ_ADC_CONFIG0_CLK_DIV_Msk & value);
}
static inline uint32_t wj_adc_get_clk_div(wj_adc_regs_t *adc_base)
{
    return (adc_base->ADC_CONFIG0 & WJ_ADC_CONFIG0_CLK_DIV_Msk);
}

static inline void wj_adc_set_convert_num(wj_adc_regs_t *adc_base, uint32_t value)
{
    adc_base->ADC_CONFIG0 &= ~WJ_ADC_CONFIG0_CONV_NUM_Msk;
    adc_base->ADC_CONFIG0 |= (WJ_ADC_CONFIG0_CONV_NUM_Msk & value);
}

static inline void wj_adc_set_single_mode(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CONFIG0 &= ~WJ_ADC_CONFIG0_CONVERT_TYPE_Msk;
}

static inline void wj_adc_set_diff_mode(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CONFIG0 |= WJ_ADC_CONFIG0_CONVERT_TYPE_DIFFERENTIAL ;
}

static inline void wj_adc_set_signed_data(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CONFIG0 |= WJ_ADC_CONFIG0_DATA_TYPE_SIGNED;
}

static inline void wj_adc_set_unsigned_data(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_CONFIG0 &= ~WJ_ADC_CONFIG0_DATA_TYPE_Msk;
}

static inline void wj_adc_set_single_channel_num(wj_adc_regs_t *adc_base, uint32_t value)
{
    value <<= WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Pos;
    adc_base->ADC_CONFIG0 &= ~WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Msk;
    adc_base->ADC_CONFIG0 |= (value & WJ_ADC_CONFIG0_SINGLE_CHANNEL_NUM_Msk);
}

static inline void wj_adc_set_convert_cycle(wj_adc_regs_t *adc_base, uint32_t value)
{
    adc_base->ADC_CONFIG1 &= ~WJ_ADC_SAMPLE_CYCLE_Msk;
    adc_base->ADC_CONFIG1 |= (WJ_ADC_SAMPLE_CYCLE_Msk & value);
}

static inline void wj_adc_set_sample_cycle(wj_adc_regs_t *adc_base, uint32_t value)
{
    adc_base->ADC_CONFIG1 &= ~WJ_ADC_SAMPLE_CYCLE_Msk;
    adc_base->ADC_CONFIG1 |= (WJ_ADC_SAMPLE_CYCLE_Msk & value);
}

static inline void wj_adc_en_int_one_convert_done(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_INT_EN |= WJ_ADC_EN_ONE_CONVERT_DONE_EN;
}

static inline void wj_adc_dis_int_one_convert_done(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_INT_EN &= ~WJ_ADC_EN_ONE_CONVERT_DONE_EN;
}

static inline void wj_adc_en_int_one_channel_done(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_INT_EN |= WJ_ADC_EN_ONE_CHANNNEL_DONE_EN;
}

static inline void wj_adc_dis_int_one_channel_done(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_INT_EN &= ~WJ_ADC_EN_ONE_CHANNNEL_DONE_EN;
}

static inline void wj_adc_en_int_overflow(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_INT_EN |= WJ_ADC_EN_ONE_OVERFLOW_EN;
}

static inline void wj_adc_dis_int_overflow(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_INT_EN &= ~WJ_ADC_EN_ONE_OVERFLOW_EN;
}

static inline void wj_adc_en_int_watchdog(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_INT_EN |= WJ_ADC_EN_WATCHDOG_EN;
}

static inline void wj_adc_dis_int_watchdog(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_INT_EN &= ~WJ_ADC_EN_WATCHDOG_EN;
}

static inline uint32_t wj_adc_get_int_flag(wj_adc_regs_t *adc_base)
{
    return adc_base->ADC_INT_STATUS & (WJ_ADC_INT_ONE_CONVERT_DONE_Msk | WJ_ADC_INT_ONE_CONNNEL_DONE_Msk | WJ_ADC_INT_ONE_SEQUENCE_DONE_Msk | WJ_ADC_INT_ONE_OVERFLOW_Msk | WJ_ADC_INT_WATCHDOG_Msk);
}

static inline void wj_adc_clear_int_flag(wj_adc_regs_t *adc_base, uint32_t value)
{
    adc_base->ADC_INT_STATUS |= value;
}

static inline uint32_t wj_adc_get_data(wj_adc_regs_t *adc_base)
{
    return (adc_base->ADC_DATA & WJ_ADC_DATA_Msk);
}

static inline void wj_adc_set_cmp_l_data(wj_adc_regs_t *adc_base, uint32_t value)
{
    adc_base->ADC_CONFIG0 &= ~WJ_ADC_CMP_DATA_L_Msk;
    adc_base->ADC_CONFIG0 |= (value & WJ_ADC_CMP_DATA_L_Msk) ;
}

static inline void wj_adc_set_cmp_h_data(wj_adc_regs_t *adc_base, uint32_t value)
{
    adc_base->ADC_CONFIG0 &= ~WJ_ADC_CMP_DATA_H_Msk;
    adc_base->ADC_CONFIG0 |= (value & WJ_ADC_CMP_DATA_H_Msk) ;
}

static inline uint32_t wj_adc_get_cmp_l_data(wj_adc_regs_t *adc_base)
{
    return (adc_base->ADC_CMP_VALUE & WJ_ADC_CMP_DATA_L_Msk);
}

static inline uint32_t wj_adc_get_cmp_h_data(wj_adc_regs_t *adc_base)
{
    return (adc_base->ADC_CMP_VALUE & WJ_ADC_CMP_DATA_H_Msk);
}

static inline void wj_adc_en(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ANA_REGISTER |= WJ_ADC_EN;
}

static inline void wj_adc_dis(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ANA_REGISTER &= ~WJ_ADC_EN_Msk;
}

static inline void wj_adc_reset(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ANA_REGISTER |= WJ_ADC_RSTN_ACTIVE;
}

static inline void wj_adc_set_static(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ANA_REGISTER &= ~WJ_ADC_LATCH_M_Msk;
}

static inline void wj_adc_set_dynamic(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ANA_REGISTER |= WJ_ADC_LATCH_M_DYNAMIC;
}


static inline void wj_adc_set_external_ref(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ANA_REGISTER &= ~WJ_ADC_LATCH_M_Msk;
}

static inline void wj_adc_set_internal_ref(wj_adc_regs_t *adc_base)
{
    adc_base->ADC_ANA_REGISTER |= WJ_ADC_ENV_REFINT_INTERNAL;
}

static inline void wj_adc_set_circuit_percent(wj_adc_regs_t *adc_base, uint32_t value)
{
    adc_base->ADC_ANA_REGISTER &= ~WJ_ADC_BIAS_C_Msk;
    adc_base->ADC_ANA_REGISTER |= (value & WJ_ADC_BIAS_C_Msk);
}

static inline uint32_t wj_adc_get_ip_id(wj_adc_regs_t *adc_base)
{
    return (*((uint32_t *)adc_base + IP_ID_OFFSET)&WJ_ADC_IP_ID_Msk);
}

#ifdef __cplusplus
}
#endif

#endif  /* _WJ_ADC_LL_H_*/
