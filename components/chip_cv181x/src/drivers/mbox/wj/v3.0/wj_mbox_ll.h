/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_mbox_ll.h
 * @brief    header file for wj mbox v3 ll driver
 * @version  V1.0
 * @date     15. Apr 2020
 ******************************************************************************/

#ifndef _WJ_mbox_LL_H_
#define _WJ_mbox_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_NUM                                   (2U)
#define CHN_NUM                                   (1U)
#define INFO_NUM                                  (8U)
#define WJ_MBOX_MAX_MESSAGE_LENGTH                (INFO_NUM * 4U)
#define WJ_MBOX_SEND_MAX_MESSAGE_LENGTH           ((INFO_NUM - 1U) * 4U)
#define WJ_MBOX_CHANNEL_OFFSET(idx)               ((WJ_MBOX_MAX_MESSAGE_LENGTH >> 2) * idx)

#define MBOX_ACK                                  (0X7975UL)

#define ACK_BIT                                   (WJ_MBOX_SEND_MAX_MESSAGE_LENGTH >> 2)
#define BIT(n)                                    (1UL << (n))
#define BIT_MASK(n)                               (BIT(n) - 1U)


#define WJ_CPU0                                   (0U)
#define WJ_CPU1                                   (1U)
#define WJ_CPU2                                   (2U)
#define WJ_CPU3                                   (3U)

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


typedef struct {
    __IOM uint32_t INTR_GEN;
    __IOM uint32_t MBOX_INFO[INFO_NUM];
} wj_mbox_ch_t;

typedef struct {
    __IM uint32_t INTR_STA;
    __OM uint32_t INTR_CLR;
    __IOM uint32_t INTR_RAW;
    __IOM uint32_t INTR_MASK[CHN_NUM];
    __IOM wj_mbox_ch_t MBOX_CH[CHN_NUM];
} wj_mbox_cpu_t;

typedef struct {
    __IOM wj_mbox_cpu_t MBOX_CPU[CPU_NUM];
    __IM uint32_t IP_ID;
} wj_mbox_regs_t;

static inline uint32_t wj_read_cpu_interrupt_state(wj_mbox_regs_t *mbox_base, uint32_t cpu_id)
{
    return mbox_base->MBOX_CPU[cpu_id].INTR_STA;
}

static inline void wj_clear_cpu_ch_interrupt(wj_mbox_regs_t *mbox_base, uint32_t cpu_id, uint32_t ch)
{
    mbox_base->MBOX_CPU[cpu_id].INTR_CLR = (uint32_t)((uint32_t)1U << ch);
}

static inline uint32_t wj_read_cpu_raw_interrupt_state(wj_mbox_regs_t *mbox_base, uint32_t cpu_id)
{
    return mbox_base->MBOX_CPU[cpu_id].INTR_RAW;
}

static inline uint32_t wj_read_cpu_ch_interrupt_mask(wj_mbox_regs_t *mbox_base, uint32_t cpu_id, uint32_t ch)
{
    return mbox_base->MBOX_CPU[cpu_id].INTR_MASK[ch];
}

static inline void wj_write_cpu_ch_interrupt_mask(wj_mbox_regs_t *mbox_base, uint32_t cpu_id, uint32_t ch, uint32_t state)
{
    mbox_base->MBOX_CPU[cpu_id].INTR_MASK[ch] = state;
}

static inline void wj_write_gen_cpu_ch_interrupt(wj_mbox_regs_t *mbox_base, uint32_t cpu_id, uint32_t ch, uint32_t state)
{
    mbox_base->MBOX_CPU[cpu_id].MBOX_CH[ch].INTR_GEN = state;
}

static inline uint32_t wj_read_gen_cpu_ch_interrupt(wj_mbox_regs_t *mbox_base, uint32_t cpu_id, uint32_t ch)
{
    return mbox_base->MBOX_CPU[cpu_id].MBOX_CH[ch].INTR_GEN;
}

static inline uint32_t wj_read_cpu_ch_info(wj_mbox_regs_t *mbox_base, uint32_t cpu_id, uint32_t ch, uint32_t info_id)
{
    return mbox_base->MBOX_CPU[cpu_id].MBOX_CH[ch].MBOX_INFO[info_id];
}

static inline void wj_write_cpu_ch_info(wj_mbox_regs_t *mbox_base, uint32_t cpu_id, uint32_t ch, uint32_t info_id, uint32_t data)
{
    mbox_base->MBOX_CPU[cpu_id].MBOX_CH[ch].MBOX_INFO[info_id] = data;
}

static inline uint32_t wj_read_ip_id(wj_mbox_regs_t *mbox_base)
{
    return mbox_base->IP_ID;
}

#ifdef __cplusplus
}
#endif

#endif /* _WJ_MBOX_LL_H_ */

