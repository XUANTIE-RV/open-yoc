/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_mbox_ll.h
 * @brief    header file for wj mbox v3 ll driver
 * @version  V1.0
 * @date     15. Apr 2020
 ******************************************************************************/

#ifndef _WJ_MBOX_LL_H_
#define _WJ_MBOX_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INFO_NUM                                  (16U)
#define WJ_MBOX_MAX_MESSAGE_LENGTH                (INFO_NUM * 4U)
#define WJ_MBOX_SEND_MAX_MESSAGE_LENGTH           ((INFO_NUM - 1U) * 4U)

#define MBOX_ACK                                  (0X7975UL)

#define ACK_BIT                                   (WJ_MBOX_SEND_MAX_MESSAGE_LENGTH >> 2)
#define BIT(n)                                    (1UL << (n))
#define BIT_MASK(n)                               (BIT(n) - 1U)

#define WJ_GEN_INFO_0_Pos                         (0U)
#define WJ_GEN_INFO_0_Msk                         (0x1U << WJ_GEN_INFO_0_Pos)
#define WJ_GEN_INFO_0                             WJ_GEN_INFO_0_Msk

#define WJ_GEN_INFO_1_Pos                         (1U)
#define WJ_GEN_INFO_1_Msk                         (0x1U << WJ_GEN_INFO_1_Pos)
#define WJ_GEN_INFO_1                             WJ_GEN_INFO_1_Msk

#define WJ_GEN_INFO_2_Pos                         (2U)
#define WJ_GEN_INFO_2_Msk                         (0x1U << WJ_GEN_INFO_2_Pos)
#define WJ_GEN_INFO_2                             WJ_GEN_INFO_2_Msk

#define WJ_GEN_INFO_3_Pos                         (3U)
#define WJ_GEN_INFO_3_Msk                         (0x1U << WJ_GEN_INFO_3_Pos)
#define WJ_GEN_INFO_3                             WJ_GEN_INFO_3_Msk

#define WJ_GEN_INFO_4_Pos                         (4U)
#define WJ_GEN_INFO_4_Msk                         (0x1U << WJ_GEN_INFO_4_Pos)
#define WJ_GEN_INFO_4                             WJ_GEN_INFO_4_Msk

#define WJ_GEN_INFO_5_Pos                         (5U)
#define WJ_GEN_INFO_5_Msk                         (0x1U << WJ_GEN_INFO_5_Pos)
#define WJ_GEN_INFO_5                             WJ_GEN_INFO_5_Msk

#define WJ_GEN_INFO_6_Pos                         (6U)
#define WJ_GEN_INFO_6_Msk                         (0x1U << WJ_GEN_INFO_6_Pos)
#define WJ_GEN_INFO_6                             WJ_GEN_INFO_6_Msk

#define WJ_GEN_INFO_7_Pos                         (7U)
#define WJ_GEN_INFO_7_Msk                         (0x1U << WJ_GEN_INFO_7_Pos)
#define WJ_GEN_INFO_7                             WJ_GEN_INFO_7_Msk

#define WJ_GEN_INFO_8_Pos                         (8U)
#define WJ_GEN_INFO_8_Msk                         (8x1U << WJ_GEN_INFO_8_Pos)
#define WJ_GEN_INFO_8                             WJ_GEN_INFO_8_Msk

#define WJ_GEN_INFO_9_Pos                         (9U)
#define WJ_GEN_INFO_9_Msk                         (0x1U << WJ_GEN_INFO_9_Pos)
#define WJ_GEN_INFO_9                             WJ_GEN_INFO_9_Msk

#define WJ_GEN_INFO_10_Pos                        (10U)
#define WJ_GEN_INFO_10_Msk                        (0x1U << WJ_GEN_INFO_10_Pos)
#define WJ_GEN_INFO_10                            WJ_GEN_INFO_10_Msk

#define WJ_GEN_INFO_11_Pos                        (11U)
#define WJ_GEN_INFO_11_Msk                        (0x1U << WJ_GEN_INFO_11_Pos)
#define WJ_GEN_INFO_11                            WJ_GEN_INFO_11_Msk

#define WJ_GEN_INFO_12_Pos                        (12U)
#define WJ_GEN_INFO_12_Msk                        (0x1U << WJ_GEN_INFO_12_Pos)
#define WJ_GEN_INFO_12                            WJ_GEN_INFO_12_Msk

#define WJ_GEN_INFO_13_Pos                        (13U)
#define WJ_GEN_INFO_13_Msk                        (0x1U << WJ_GEN_INFO_13_Pos)
#define WJ_GEN_INFO_13                            WJ_GEN_INFO_13_Msk

#define WJ_GEN_INFO_14_Pos                        (14U)
#define WJ_GEN_INFO_14_Msk                        (0x1U << WJ_GEN_INFO_14_Pos)
#define WJ_GEN_INFO_14                            WJ_GEN_INFO_14_Msk

#define WJ_GEN_INFO_15_Pos                        (15U)
#define WJ_GEN_INFO_15_Msk                        (0x1U << WJ_GEN_INFO_15_Pos)
#define WJ_GEN_INFO_15                            WJ_GEN_INFO_15_Msk

typedef struct {
    __IOM uint32_t INTGR;
    __IOM uint32_t INTCR;
    __IOM uint32_t INTMR;
    __IOM uint32_t INTRSR;
    __IOM uint32_t INTMSR;
    __IOM uint32_t INTENB;
} wj_mbox_ch_t;

typedef struct {
    wj_mbox_ch_t  MBOX_CH[2];
    __IOM uint32_t MBOX_INFO0[16];
    __IOM uint32_t MBOX_INFO1[16];
} wj_mbox_regs_t;

static inline uint32_t wj_mbox_enable_irq(wj_mbox_regs_t *mbox_base, uint32_t cpu_id)
{
    mbox_base->MBOX_CH[cpu_id].INTENB = 0xFFFFU;
}

static inline uint32_t wj_mbox_disable_irq(wj_mbox_regs_t *mbox_base, uint32_t cpu_id)
{
    mbox_base->MBOX_CH[cpu_id].INTENB = 0U;
}

static inline uint32_t wj_read_cpu_interrupt_state(wj_mbox_regs_t *mbox_base, uint32_t cpu_id)
{
    if (cpu_id == 0U) {
        return mbox_base->MBOX_CH[1].INTMSR;
    }

    if (cpu_id == 1U) {
        return mbox_base->MBOX_CH[0].INTMSR;
    }
}

static inline void wj_clear_cpu_ch_interrupt(wj_mbox_regs_t *mbox_base, uint32_t cpu_id, uint32_t ch)
{
    if (cpu_id == 0U) {
        mbox_base->MBOX_CH[1].INTCR = ch;
    }

    if (cpu_id == 1U) {
        mbox_base->MBOX_CH[0].INTCR = ch;
    }
}

static inline uint32_t wj_read_cpu_raw_interrupt_state(wj_mbox_regs_t *mbox_base, uint32_t cpu_id)
{
    if (cpu_id == 0U) {
        return mbox_base->MBOX_CH[1].INTRSR;
    }

    if (cpu_id == 1U) {
        return mbox_base->MBOX_CH[0].INTRSR;
    }
}

static inline uint32_t wj_read_cpu_interrupt_mask(wj_mbox_regs_t *mbox_base, uint32_t cpu_id)
{
    return mbox_base->MBOX_CH[cpu_id].INTMR;
}

static inline void wj_write_cpu_ch_interrupt_mask(wj_mbox_regs_t *mbox_base, uint32_t cpu_id, uint32_t state)
{
    mbox_base->MBOX_CH[cpu_id].INTMR = state;
}

static inline void wj_write_gen_cpu_ch_interrupt(wj_mbox_regs_t *mbox_base, uint32_t cpu_id, uint32_t state)
{
    mbox_base->MBOX_CH[cpu_id].INTGR = state;
}

static inline uint32_t wj_read_gen_cpu_ch_interrupt(wj_mbox_regs_t *mbox_base, uint32_t cpu_id)
{
    if (cpu_id == 0U) {
        return mbox_base->MBOX_CH[1].INTGR;
    }

    if (cpu_id == 1U) {
        return mbox_base->MBOX_CH[0].INTGR;
    }
}

static inline uint32_t wj_read_cpu_ch_info(wj_mbox_regs_t *mbox_base, uint32_t cpu_id, uint32_t info_id)
{
    if (cpu_id == 0U) {
        return mbox_base->MBOX_INFO1[info_id];
    }

    if (cpu_id == 1U) {
        return mbox_base->MBOX_INFO0[info_id];
    }
}

static inline void wj_write_cpu_ch_info(wj_mbox_regs_t *mbox_base, uint32_t cpu_id, uint32_t info_id, uint32_t data)
{
    if (cpu_id == 0U) {
        mbox_base->MBOX_INFO0[info_id] = data;
    }

    if (cpu_id == 1U) {
        mbox_base->MBOX_INFO1[info_id] = data;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* _WJ_MBOX_LL_H_ */

