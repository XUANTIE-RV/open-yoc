/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_bmu_ll.h
 * @brief    header file for wj bmu ll driver
 * @version  V1.0
 * @date     22. Jan 2021
 ******************************************************************************/

#ifndef _WJ_BMU_LL_H_
#define _WJ_BMU_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    __IOM uint32_t BMU_CFG0;            /* Offset: 0x000 (R/W) BMU CFG0 register.          */
    __IOM uint32_t BMU_CFG1;            /* Offset: 0x004 (R/W) BMU CFG1 register.          */
    __IOM uint32_t BMU_CFG2;            /* Offset: 0x008 (R/W) BMU CFG2 register.          */
    __IOM uint32_t BMU_CFG3;            /* Offset: 0x00C (R/W) BMU CFG3 register.          */
    __IOM uint32_t BMU_CFG4;            /* Offset: 0x010 (R/W) BMU CFG4 register.          */
    __IOM uint32_t BMU_CFG5;            /* Offset: 0x014 (R/W) BMU CFG5 register.          */
    __IOM uint32_t BMU_CFG6[4];         /* Offset: 0x018 (R/W) BMU CFG6 register.          */
    __IOM uint32_t BMU_FLT;             /* Offset: 0x028 (R/W) BMU CFG0 register.          */
    __IOM uint32_t BMU_CFG10[4];        /* Offset: 0x02C (R/W) BMU CFG10 register.         */
    __IOM uint32_t BMU_CFG14;           /* Offset: 0x03C (R/W) BMU CFG14 register.         */
    __IOM uint32_t BMU_RD_STS0;         /* Offset: 0x040 (R/W) BMU RD STS0 register.       */
    __IOM uint32_t BMU_RD_STS1;         /* Offset: 0x044 (R/W) BMU RD STS1 register.       */
    __IOM uint32_t BMU_RD_STS2;         /* Offset: 0x048 (R/W) BMU RD STS2 register.       */
    __IOM uint32_t BMU_RD_STS3;         /* Offset: 0x04C (R/W) BMU RD STS3 register.       */
    __IOM uint32_t BMU_WD_STS0;         /* Offset: 0x050 (R/W) BMU WD STS0 register.       */
    __IOM uint32_t BMU_WD_STS1;         /* Offset: 0x054 (R/W) BMU WD STS1 register.       */
    __IOM uint32_t BMU_WD_STS2;         /* Offset: 0x058 (R/W) BMU WD STS2 register.       */
    __IOM uint32_t BMU_WD_STS3;         /* Offset: 0x05c (R/W) BMU WD STS3 register.       */
    __IOM uint32_t BMU_VRD_STS0;        /* Offset: 0x060 (R/W) BMU VRD STS0 register.      */
    __IOM uint32_t BMU_VRD_STS1;        /* Offset: 0x064 (R/W) BMU VRD STS1 register.      */
    __IOM uint32_t RECEIVE[2];
    __IOM uint32_t BMU_VWR_STS0;        /* Offset: 0x070 (R/W) BMU VWR STS0 register.      */
    __IOM uint32_t BMU_VWR_STS1;        /* Offset: 0x074 (R/W) BMU VWR STS1 register.      */
    __IOM uint32_t RECEIVE1[2];
    __IOM uint32_t BMU_IR_STS0;         /* Offset: 0x080 (R/W) BMU IR STS0 register.       */
    __IOM uint32_t BMU_IR_STS1;         /* Offset: 0x084 (R/W) BMU IR STS1 register.       */
    __IOM uint32_t BMU_VERSION0;        /* Offset: 0x088 (R/W) BMU VERSION0 register.      */
    __IOM uint32_t BMU_VERSION1;        /* Offset: 0x08C (R/W) BMU VERSION1 register.      */
    __IOM uint32_t BMU_OSTD_STS;        /* Offset: 0x090 (R/W) BMU OSTD STS register.      */
    __IOM uint32_t BMU_OSTD_CFG;        /* Offset: 0x094 (R/W) BMU OSTD CFG register.      */
} wj_bmu_regs_t;

/* Offset: 0x000 (R/W) BMU CFG0 register.          */
#define WJ_BMU_CFG0_SW_RST_Pos                  0U
#define WJ_BMU_CFG0_SW_RST_Msk                  (0x1U << WJ_BMU_CFG0_SW_RST_Pos)
#define WJ_BMU_CFG0_SW_RST_EN                   WJ_BMU_CFG0_SW_RST_Msk

#define WJ_BMU_CFG0_CTL_MONITOR_Pos             1U
#define WJ_BMU_CFG0_CTL_MONITOR_Msk             (0x1U << WJ_BMU_CFG0_CTL_MONITOR_Pos)
#define WJ_BMU_CFG0_CTL_MONITOR_EN              WJ_BMU_CFG0_CTL_MONITOR_Msk

#define WJ_BMU_CFG0_MON_CLK_FREE_Pos            2U
#define WJ_BMU_CFG0_MON_CLK_FREE_Msk            (0x1U << WJ_BMU_CFG0_MON_CLK_FREE_Pos)
#define WJ_BMU_CFG0_MON_CLK_FREE_EN             WJ_BMU_CFG0_MON_CLK_FREE_Msk

#define WJ_BMU_CFG0_INT_SRC_SEL_Pos             3U
#define WJ_BMU_CFG0_INT_SRC_SEL_Msk             (0x1FU << WJ_BMU_CFG0_INT_SRC_SEL_Pos)
#define WJ_BMU_CFG0_INT_SRC_SEL_ADD_RANGE_HIT   (0x0U  << WJ_BMU_CFG0_INT_SRC_SEL_Pos)
#define WJ_BMU_CFG0_INT_SRC_SEL_MON_PERIOD_EXP  (0x1U  << WJ_BMU_CFG0_INT_SRC_SEL_Pos)
#define WJ_BMU_CFG0_INT_SRC_SEL_W_TAR_WD_OCCUR  (0x2U  << WJ_BMU_CFG0_INT_SRC_SEL_Pos)
#define WJ_BMU_CFG0_INT_SRC_SEL_ERR_RES_HAPED   (0x3U  << WJ_BMU_CFG0_INT_SRC_SEL_Pos)
#define WJ_BMU_CFG0_INT_SRC_SEL_COUNT_OVERFLOW  (0x4U  << WJ_BMU_CFG0_INT_SRC_SEL_Pos)

#define WJ_BMU_CFG0_TRIG_MODE_Pos               8U
#define WJ_BMU_CFG0_TRIG_MODE_Msk               (0x1U << WJ_BMU_CFG0_TRIG_MODE_Pos)
#define WJ_BMU_CFG0_TRIG_MODE_SINGLE            WJ_BMU_CFG0_TRIG_MODE_Msk

#define WJ_BMU_CFG0_PERIOD_MON_Pos              12U
#define WJ_BMU_CFG0_PERIOD_MON_Msk              (0xFU << WJ_BMU_CFG0_INT_SRC_SEL_Pos)
#define WJ_BMU_CFG0_PERIOD_MON_DIV4             (0x1U << WJ_BMU_CFG0_INT_SRC_SEL_Pos)
#define WJ_BMU_CFG0_PERIOD_MON_DIV8             (0x2U << WJ_BMU_CFG0_INT_SRC_SEL_Pos)
#define WJ_BMU_CFG0_PERIOD_MON_DIV16            (0x4U << WJ_BMU_CFG0_INT_SRC_SEL_Pos)
#define WJ_BMU_CFG0_PERIOD_MON_DIV32            (0x8U << WJ_BMU_CFG0_INT_SRC_SEL_Pos)

#define WJ_BMU_CFG0_ADD_ALN_FLT_Pos             16U
#define WJ_BMU_CFG0_ADD_ALN_FLT_Msk             (0xFU << WJ_BMU_CFG0_INT_SRC_SEL_Pos)
#define WJ_BMU_CFG0_ADD_ALN_FLT_16B             (0x1U << WJ_BMU_CFG0_INT_SRC_SEL_Pos)
#define WJ_BMU_CFG0_ADD_ALN_FLT_32B             (0x2U << WJ_BMU_CFG0_INT_SRC_SEL_Pos)
#define WJ_BMU_CFG0_ADD_ALN_FLT_64B             (0x4U << WJ_BMU_CFG0_INT_SRC_SEL_Pos)
#define WJ_BMU_CFG0_ADD_ALN_FLT_128B            (0x8U << WJ_BMU_CFG0_INT_SRC_SEL_Pos)

/* Offset: 0x004 (R/W) BMU CFG1 register.          */
#define WJ_BMU_CFG1_WR_DURA_TH_Pos              0U
#define WJ_BMU_CFG1_WR_DURA_TH_Msk              (0xFFFFU << WJ_BMU_CFG1_WR_DURA_TH_Pos)
#define WJ_BMU_CFG1_WR_DURA_TH_EN               WJ_BMU_CFG1_WR_DURA_TH_Msk

#define WJ_BMU_CFG1_RD_DURA_TH_Pos              16U
#define WJ_BMU_CFG1_RD_DURA_TH_Msk              (0xFFFFU << WJ_BMU_CFG1_RD_DURA_TH_Pos)
#define WJ_BMU_CFG1_RD_DURA_TH_EN               WJ_BMU_CFG1_RD_DURA_TH_Msk

/* Offset: 0x008 (R/W) BMU CFG2 register.          */
#define WJ_BMU_CFG2_ID_Pos                     0U
#define WJ_BMU_CFG2_ID_Msk                     (0xFFFFU << WJ_BMU_CFG1_WR_DURA_TH_Pos)
#define WJ_BMU_CFG2_ID_ENABLE                  (0x1U << WJ_BMU_CFG1_WR_DURA_TH_Pos)

#define WJ_BMU_CFG2_ID_TARGET_Pos              16U
#define WJ_BMU_CFG2_ID_TARGET_Msk              (0xFFFFU << WJ_BMU_CFG1_RD_DURA_TH_Pos)
#define WJ_BMU_CFG2_ID_TARGET                  WJ_BMU_CFG1_RD_DURA_TH_Msk

/* Offset: 0x00C (R/W) BMU CFG3 register.          */
#define WJ_BMU_CFG3_PERIOD_MON_Pos             0U
#define WJ_BMU_CFG3_PERIOD_MON_Msk             (0xFFFFFFFFUL << WJ_BMU_CFG1_WR_DURA_TH_Pos)
#define WJ_BMU_CFG3_PERIOD_MON                 WJ_BMU_CFG1_WR_DURA_TH_Msk

/* Offset: 0x010 (R/W) BMU CFG4 register.          */
#define WJ_BMU_CFG4_ADDR_ALN_FLT_Pos             0U
#define WJ_BMU_CFG4_ADDR_ALN_FLT_Msk             (0x3U << WJ_BMU_CFG4_ADDR_ALN_FLT_Pos)
#define WJ_BMU_CFG4_ADDR_ALN_FLT_COUNTER0        (0x1U << WJ_BMU_CFG4_ADDR_ALN_FLT_Pos)
#define WJ_BMU_CFG4_ADDR_ALN_FLT_COUNTER1        (0x2U << WJ_BMU_CFG4_ADDR_ALN_FLT_Pos)

#define WJ_BMU_CFG4_ADDR_RANGE_FLT_Pos           4U
#define WJ_BMU_CFG4_ADDR_RANGE_FLT_Msk           (0x3U << WJ_BMU_CFG4_ADDR_RANGE_FLT_Pos)
#define WJ_BMU_CFG4_ADDR_RANGE_FLT_COUNTER0      (0x1U << WJ_BMU_CFG4_ADDR_RANGE_FLT_Pos)
#define WJ_BMU_CFG4_ADDR_RANGE_FLT_COUNTER1      (0x2U << WJ_BMU_CFG4_ADDR_RANGE_FLT_Pos)

#define WJ_BMU_CFG4_SIZE_FLT_Pos                 8U
#define WJ_BMU_CFG4_SIZE_FLT_Msk                 (0x3U << WJ_BMU_CFG4_ADDR_RANGE_FLT_Pos)
#define WJ_BMU_CFG4_SIZE_FLT_COUNTER0            (0x1U << WJ_BMU_CFG4_ADDR_RANGE_FLT_Pos)
#define WJ_BMU_CFG4_SIZE_FLT_COUNTER1            (0x2U << WJ_BMU_CFG4_ADDR_RANGE_FLT_Pos)

#define WJ_BMU_CFG4_LEN_FLT_Pos                  12U
#define WJ_BMU_CFG4_LEN_FLT_Msk                  (0x3U << WJ_BMU_CFG4_LEN_FLT_Pos)
#define WJ_BMU_CFG4_LEN_FLT_COUNTER0             (0x1U << WJ_BMU_CFG4_LEN_FLT_Pos)
#define WJ_BMU_CFG4_LEN_FLT_COUNTER1             (0x2U << WJ_BMU_CFG4_LEN_FLT_Pos)

#define WJ_BMU_CFG4_SIZE_FLT_COUNT0_Pos          16U
#define WJ_BMU_CFG4_SIZE_FLT_COUNT0_Msk          (0x7U << WJ_BMU_CFG4_SIZE_FLT_COUNT0_Pos)
#define WJ_BMU_CFG4_SIZE_FLT_COUNT1_Pos          19U
#define WJ_BMU_CFG4_SIZE_FLT_COUNT1_Msk          (0x7U << WJ_BMU_CFG4_SIZE_FLT_COUNT1_Pos)

/* Offset: 0x014 (R/W) BMU CFG5 register.          */
#define WJ_BMU_CFG5_LEN_FLT_COUNTER0_Pos         0U
#define WJ_BMU_CFG5_LEN_FLT_COUNTER0_Msk         (0xFFU << WJ_BMU_CFG5_LEN_FLT_COUNTER0_Pos)
#define WJ_BMU_CFG5_LEN_FLT_COUNTER1_Pos         19U
#define WJ_BMU_CFG5_LEN_FLT_COUNTER1_Msk         (0xFFU << WJ_BMU_CFG5_LEN_FLT_COUNTER1_Pos)

typedef enum {
    BMU_FILTER_16B_ALIGN,
    BMU_FILTER_32B_ALIGN,
    BMU_FILTER_64B_ALIGN,
    BMU_FILTER_128B_ALIGN,
    BMU_FILTER_NOT_16B_ALIGN,
} bmu_align_t;

typedef enum {
    BMU_MONITOR_PERIOD_DIV0 = 0,
    BMU_MONITOR_PERIOD_DIV4 = 1,
    BMU_MONITOR_PERIOD_DIV8 = 2,
    BMU_MONITOR_PERIOD_DIV16 = 4,
    BMU_MONITOR_PERIOD_DIV32 = 8,
} bmu_monitor_period_div_t;

typedef enum {
    BUM_SIZE_FILTER_1_BYTE,
    BUM_SIZE_FILTER_2_BYTE,
    BUM_SIZE_FILTER_4_BYTE,
    BUM_SIZE_FILTER_8_BYTE,
    BUM_SIZE_FILTER_16_BYTE,
    BUM_SIZE_FILTER_32_BYTE,
    BUM_SIZE_FILTER_64_BYTE,
} bmu_filter_size_t;

/*counter0: read cmd num*/
static inline void wj_bmu_counter0_addr_align_filter(wj_bmu_regs_t *bmu_base, bmu_align_t align_filter)
{
    uint32_t val = bmu_base->BMU_CFG0;

    if (align_filter == BMU_FILTER_NOT_16B_ALIGN) {
        bmu_base->BMU_CFG0 |= 0xfU << 16;
        return;
    }

    val &= ~(0xfU << 16);
    val |= (1U << align_filter) << 16U;
    bmu_base->BMU_CFG0 = val;
}

/*counter1: read byte num*/
static inline void wj_bmu_counter1_addr_align_filter(wj_bmu_regs_t *bmu_base, bmu_align_t align_filter)
{
    uint32_t val = bmu_base->BMU_CFG0;

    if (align_filter == BMU_FILTER_NOT_16B_ALIGN) {
        bmu_base->BMU_CFG0 |= 0xfU << 20U;
        return;
    }

    val &= ~(0xfU << 20U);
    val |= (1U << align_filter) << 20U;
    bmu_base->BMU_CFG0 = val;
}

static inline void wj_bmu_set_period_monitor_mode(wj_bmu_regs_t *bmu_base)
{
    bmu_base->BMU_CFG0 &= ~WJ_BMU_CFG0_TRIG_MODE_SINGLE;
}

static inline void wj_bmu_set_single_monitor_mode(wj_bmu_regs_t *bmu_base)
{
    bmu_base->BMU_CFG0 |= WJ_BMU_CFG0_TRIG_MODE_SINGLE;
}

static inline void wj_bmu_bmu_monitor_interrupt(wj_bmu_regs_t *bmu_base, uint32_t mask)
{
    mask &= 0x1f;
    bmu_base->BMU_CFG0 |= (mask << 3U);
}

static inline void wj_bmu_bmu_monitor_disable_interrupt(wj_bmu_regs_t *bmu_base)
{
    bmu_base->BMU_CFG0 &= ~(0x1f << 3U);
}


static inline void wj_bmu_bmu_monitor_reset(wj_bmu_regs_t *bmu_base)
{
    bmu_base->BMU_CFG0 &= ~WJ_BMU_CFG0_SW_RST_EN;
    bmu_base->BMU_CFG0 |= WJ_BMU_CFG0_SW_RST_EN;
}

static inline void wj_bmu_bmu_monitor_enable(wj_bmu_regs_t *bmu_base)
{
    bmu_base->BMU_CFG0 |= WJ_BMU_CFG0_CTL_MONITOR_EN;
}

static inline void wj_bmu_bmu_monitor_disable(wj_bmu_regs_t *bmu_base)
{
    bmu_base->BMU_CFG0 &= ~WJ_BMU_CFG0_CTL_MONITOR_EN;
}

/*The threshold for read cmd duration, can't set 0.*/
static inline void wj_bmu_read_cmd_dura_threshold(wj_bmu_regs_t *bmu_base, uint16_t val)
{
    uint32_t reg = bmu_base->BMU_CFG1;
    reg &= ~(0xffff << 16);
    reg |= val << 16;
    bmu_base->BMU_CFG1 = reg;
}

/*The threshold for write cmd duration, can't set 0.*/
static inline void wj_bmu_write_cmd_dura_threshold(wj_bmu_regs_t *bmu_base, uint16_t val)
{
    uint32_t reg = bmu_base->BMU_CFG1;
    reg &= ~(0xffff);
    reg |= val;
    bmu_base->BMU_CFG1 = reg;
}

static inline void wj_bmu_compare_monitor_id_enable(wj_bmu_regs_t *bmu_base)
{
    bmu_base->BMU_CFG2 |= WJ_BMU_CFG2_ID_ENABLE;
}

static inline void wj_bmu_compare_monitor_id_disable(wj_bmu_regs_t *bmu_base)
{
    bmu_base->BMU_CFG2 &= ~WJ_BMU_CFG2_ID_ENABLE;
}

static inline void wj_bmu_set_monitor_id(wj_bmu_regs_t *bmu_base, uint16_t target_id)
{
    uint32_t val = target_id << WJ_BMU_CFG2_ID_TARGET_Pos;
    bmu_base->BMU_CFG2 |= val;
}

/*period unit apb clk cycles*/
static inline void wj_bmu_set_monitor_period(wj_bmu_regs_t *bmu_base, uint32_t val)
{
    bmu_base->BMU_CFG3 = val;
}

static inline void wj_bmu_set_monitor_period_div(wj_bmu_regs_t *bmu_base, bmu_monitor_period_div_t div)
{
    uint32_t val = bmu_base->BMU_CFG0;
    val &= ~(0xf << 12);
    val |= div << 12;
    bmu_base->BMU_CFG0 = val;
}

static inline void wj_bmu_counter0_size_filter(wj_bmu_regs_t *bmu_base, bmu_filter_size_t size)
{
    uint32_t reg = bmu_base->BMU_CFG4;
    reg &= ~(7 << 16);
    reg |= size << 16;
    bmu_base->BMU_CFG4 = reg;
}

static inline void wj_bmu_counter1_size_filter(wj_bmu_regs_t *bmu_base, uint8_t val)
{
    val &= 7;
    uint32_t reg = bmu_base->BMU_CFG4;
    reg &= ~(7 << 18);
    reg |= val << 18;
    bmu_base->BMU_CFG4 = reg;
}

static inline void wj_bmu_counter0_length_filter_enable(wj_bmu_regs_t *bmu_base, bool en)
{
    if (en) {
        bmu_base->BMU_CFG4 |= WJ_BMU_CFG4_LEN_FLT_COUNTER0;
    } else {
        bmu_base->BMU_CFG4 &= ~WJ_BMU_CFG4_LEN_FLT_COUNTER0;
    }
}

static inline void wj_bmu_counter1_length_filter_enable(wj_bmu_regs_t *bmu_base, bool en)
{
    if (en) {
        bmu_base->BMU_CFG4 |= WJ_BMU_CFG4_LEN_FLT_COUNTER1;
    } else {
        bmu_base->BMU_CFG4 &= ~WJ_BMU_CFG4_LEN_FLT_COUNTER1;
    }
}

static inline void wj_bmu_counter0_size_filter_enable(wj_bmu_regs_t *bmu_base, bool en)
{
    if (en) {
        bmu_base->BMU_CFG4 |= WJ_BMU_CFG4_SIZE_FLT_COUNTER0;
    } else {
        bmu_base->BMU_CFG4 &= ~WJ_BMU_CFG4_SIZE_FLT_COUNTER0;
    }
}

static inline void wj_bmu_counter1_size_filter_enable(wj_bmu_regs_t *bmu_base, bool en)
{
    if (en) {
        bmu_base->BMU_CFG4 |= WJ_BMU_CFG4_SIZE_FLT_COUNTER1;
    } else {
        bmu_base->BMU_CFG4 &= ~WJ_BMU_CFG4_SIZE_FLT_COUNTER1;
    }
}

static inline void wj_bmu_counter0_addr_range_filter_enable(wj_bmu_regs_t *bmu_base, bool en)
{
    if (en) {
        bmu_base->BMU_CFG4 |= WJ_BMU_CFG4_ADDR_RANGE_FLT_COUNTER0;
    } else {
        bmu_base->BMU_CFG4 &= ~WJ_BMU_CFG4_ADDR_RANGE_FLT_COUNTER0;
    }
}

static inline void wj_bmu_counter1_addr_range_filter_enable(wj_bmu_regs_t *bmu_base, bool en)
{
    if (en) {
        bmu_base->BMU_CFG4 |= WJ_BMU_CFG4_ADDR_RANGE_FLT_COUNTER1;
    } else {
        bmu_base->BMU_CFG4 &= ~WJ_BMU_CFG4_ADDR_RANGE_FLT_COUNTER1;
    }
}

static inline void wj_bmu_counter0_alignment_filter_enable(wj_bmu_regs_t *bmu_base, bool en)
{
    if (en) {
        bmu_base->BMU_CFG4 |= WJ_BMU_CFG4_ADDR_ALN_FLT_COUNTER0;
    } else {
        bmu_base->BMU_CFG4 &= ~WJ_BMU_CFG4_ADDR_ALN_FLT_COUNTER0;
    }
}

static inline void wj_bmu_counter1_alignment_filter_enable(wj_bmu_regs_t *bmu_base, bool en)
{
    if (en) {
        bmu_base->BMU_CFG4 |= WJ_BMU_CFG4_ADDR_ALN_FLT_COUNTER1;
    } else {
        bmu_base->BMU_CFG4 &= ~WJ_BMU_CFG4_ADDR_ALN_FLT_COUNTER1;
    }
}

static inline void wj_bmu_counter0_length_filter(wj_bmu_regs_t *bmu_base, uint8_t val)
{
    uint32_t reg = bmu_base->BMU_CFG5;
    reg &= ~0xff;
    reg |= val;
    bmu_base->BMU_CFG5 = reg;
}

static inline void wj_bmu_counter1_length_filter(wj_bmu_regs_t *bmu_base, uint8_t val)
{
    uint32_t reg = bmu_base->BMU_CFG5;
    reg &= ~(0xff << 8);
    reg |= val << 8;
    bmu_base->BMU_CFG5 = reg;
}

static inline uint32_t wj_bmu_read_version0(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_VERSION0;
}

static inline uint32_t wj_bmu_read_version1(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_VERSION1;
}

static inline uint32_t wj_bmu_read_address_width(wj_bmu_regs_t *bmu_base)
{
    return (bmu_base->BMU_VERSION0 & 0xffU);
}

static inline void wj_bmu_counter0_addr_filter(wj_bmu_regs_t *bmu_base, unsigned long lower_addr, unsigned long upper_addr)
{
    volatile uint32_t addrmsb = wj_bmu_read_address_width(bmu_base);
    volatile unsigned long addr_msk = 1;
    addr_msk = addr_msk << (addrmsb + 1);
    addr_msk -= 1;
    bmu_base->BMU_CFG6[0] = (uint32_t)(upper_addr & addr_msk);

    if (addrmsb > 32U) {
        bmu_base->BMU_CFG6[1] = (uint32_t)(upper_addr >> 32U);
    }

    bmu_base->BMU_CFG10[0] = (uint32_t)(lower_addr & addr_msk);

    if (addrmsb > 32U) {
        bmu_base->BMU_CFG10[1] = (uint32_t)(lower_addr >> 32U);
    }
}

static inline void wj_bmu_counter1_addr_filter(wj_bmu_regs_t *bmu_base, unsigned long lower_addr, unsigned long upper_addr)
{
    uint32_t addrmsb = wj_bmu_read_address_width(bmu_base);

    if (addrmsb <= 32) {
        bmu_base->BMU_CFG6[1] = (uint32_t)upper_addr;
    } else {
        bmu_base->BMU_CFG6[1] |= (uint32_t)(upper_addr << (addrmsb - 32U));
        bmu_base->BMU_CFG6[2] = (uint32_t)(upper_addr >> (32U - (addrmsb - 32U)));
    }

    if (addrmsb <= 32) {
        bmu_base->BMU_CFG10[1] = (uint32_t)lower_addr;
    } else {
        bmu_base->BMU_CFG10[1] |= (uint32_t)(lower_addr << (addrmsb - 32U));
        bmu_base->BMU_CFG10[2] = (uint32_t)(lower_addr >> (32U - (addrmsb - 32U)));
    }
}

/*cmd_filter 8, 4, 2.	为了节约资源，每n个命令选一个记录, 0不舍弃*/
static inline void wj_bmu_read_cmd_over_th_filter_enable(wj_bmu_regs_t *bmu_base, uint8_t cmd_filter, bool en)
{
    uint32_t reg = bmu_base->BMU_FLT;
    reg &= ~7;

    if (en) {
        reg |= 1 << 8 | cmd_filter / 2;
    } else {
        reg &= ~(1 << 8);
    }

    bmu_base->BMU_FLT = reg;
}

/*cmd_filter 8, 4, 2.	为了节约资源，每n个命令选一个记录, 0不舍弃*/
static inline void wj_bmu_write_cmd_over_th_filter_enable(wj_bmu_regs_t *bmu_base, uint8_t cmd_filter, bool en)
{
    uint32_t reg = bmu_base->BMU_FLT;
    reg &= ~(7 << 4);

    if (en) {
        reg |= 1 << 9 | (cmd_filter / 2) << 4;
    } else {
        reg &= ~(1 << 9);
    }

    bmu_base->BMU_FLT = reg;
}

static inline void wj_bmu_set_write_compare_data(wj_bmu_regs_t *bmu_base, uint32_t val)
{
    bmu_base->BMU_CFG14 = val;
}

static inline uint32_t wj_bmu_total_read_duration_cnt(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_RD_STS0;
}

static inline uint32_t wj_bmu_total_read_transaction_num(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_RD_STS1;
}

static inline uint32_t wj_bmu_total_read_byte_num(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_RD_STS2;
}

static inline uint32_t wj_bmu_total_read_cmd_duration_over_threshod(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_RD_STS3;
}

static inline uint32_t wj_bmu_total_write_duration_cnt(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_WD_STS0;
}

static inline uint32_t wj_bmu_total_write_transaction_num(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_WD_STS1;
}

static inline uint32_t wj_bmu_total_write_byte_num(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_WD_STS2;
}

static inline uint32_t wj_bmu_total_write_cmd_duration_over_threshod(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_WD_STS3;
}

/*可变读命令计数器0, cnt8_2*/
static inline uint32_t wj_bmu_filter_read_cmd_counter0(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_VRD_STS0;
}

/*可变读命令计数器1, cnt8_1*/
static inline uint32_t wj_bmu_filter_read_cmd_counter1(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_VRD_STS1;
}

/*可变写命令计数器0, cnt9_0*/
static inline uint32_t wj_bmu_filter_write_cmd_counter0(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_VWR_STS0;
}

/*可变写命令计数器1, cnt9_1*/
static inline uint32_t wj_bmu_filter_write_cmd_counter1(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_VWR_STS1;
}

/*indicate the number of read transaction which not finished in last monitor period*/
static inline uint32_t wj_bmu_rg_sta_ir_rostd_cnt(wj_bmu_regs_t *bmu_base)
{
    return (bmu_base->BMU_IR_STS0 & (0xff << 24)) >> 24;
}

/*indicate the number of write transaction which not finished in last monitor period*/
static inline uint32_t wj_bmu_rg_sta_ir_wostd_cnt(wj_bmu_regs_t *bmu_base)
{
    return (bmu_base->BMU_IR_STS0 & (0xff << 16)) >> 16;
}

static inline uint32_t wj_bmu_apb_int_sta(wj_bmu_regs_t *bmu_base)
{
    return (bmu_base->BMU_IR_STS0 & (0x3ff << 5)) >> 5;
}

static inline void wj_bmu_int_sta_clear(wj_bmu_regs_t *bmu_base)
{
    bmu_base->BMU_IR_STS0 |= 1;
}

static inline uint16_t wj_bmu_write_error_resp_id(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_IR_STS1 & 0xffff;
}

static inline uint16_t wj_bmu_read_error_resp_id(wj_bmu_regs_t *bmu_base)
{
    return (bmu_base->BMU_IR_STS1 & (0xffff << 16)) >> 16;
}

/*8192 cycle or 4096 cycle, true 8192, false 4096*/
static inline void wj_bmu_ostd_cnt_mode_8192(wj_bmu_regs_t *bmu_base)
{
    bmu_base->BMU_OSTD_CFG &= ~(1 << 16);
}

static inline void wj_bmu_ostd_cnt_mode_4096(wj_bmu_regs_t *bmu_base)
{
    bmu_base->BMU_OSTD_CFG |= 1 << 16;
}

static inline uint32_t wj_bmu_rg_sta_wr_avg_ostd_dbg(wj_bmu_regs_t *bmu_base)
{
    return (bmu_base->BMU_OSTD_CFG & (0xff << 8)) >> 8;
}

static inline uint32_t wj_bmu_rg_sta_rd_avg_ostd_dbg(wj_bmu_regs_t *bmu_base)
{
    return bmu_base->BMU_OSTD_CFG & 0xff;
}

#define BMU_MONITOR_EXPIRED_INT 0
#define BMU_WRITE_TARGET_WDATA_OCCUR_INT 1
#define BMU_ERROR_RESPONSE_HAPPENDED_INT 2
#define BMU_COUNTER_OVERFLOW_INT         4

#define BMU_TIMER_EXPIRED_INT_STA                              (1 << 0)
#define BMU_TARGET_WDATA_OCCUR_INT_STA                         (1 << 1)
#define BMU_WRITE_ERROR_RESP_OCCUR_INT_STA                     (1 << 2)
#define BMU_WRITE_DURATION_WITH_THRESHOLD_FIFO_FULL_INT_STA    (1 << 3)
#define BMU_WRITE_DURATION_COUNTER_FULL_INT_STA                (1 << 4)
/*write transactions from last monitor period still not finished in current monitor period*/
#define BMU_WRITE_CROSS_MONITOR_PERION_INT_STA                 (1 << 5)
#define BMU_READ_DURATION_WITH_THRESHOLD_FIFO_FULL_INT_STA     (1 << 6)
#define BMU_READ_DURATION_COUNTER_FULL_INT_STA                 (1 << 7)
/*read transactions from last monitor period still not finished in current monitor period*/
#define BMU_READ_CROSS_MONITOR_PERION_INT_STA                  (1 << 8)
#define BMU_READ_ERROR_RESP_OCCUR_INT_STA                      (1 << 9)

#ifdef __cplusplus
}
#endif

#endif  /* _BMU_H_*/
