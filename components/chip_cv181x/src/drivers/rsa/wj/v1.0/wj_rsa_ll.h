/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_rsa_ll.h
 * @brief    header file for rsa ll driver
 * @version  V1.0
 * @date     22. JAN 2020
 ******************************************************************************/

#ifndef __WJ_RSA_LL_H__
#define __WJ_RSA_LL_H__

#include <stdio.h>
#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RSA_KEY_LEN     2048U
#define RSA_KEY_BYTE    (RSA_KEY_LEN >> 3U)
#define RSA_KEY_WORD    (RSA_KEY_LEN >> 5U)

#define BN_MAX_BITS     ((RSA_KEY_LEN << 1U) + 32U)
#define BN_MAX_BYTES    ((BN_MAX_BITS + 7U) >> 3U)
#define BN_MAX_WORDS    ((BN_MAX_BYTES + 3U) >> 2U)

#define MAX_RSA_LP_CNT  10000U

#define GET_KEY_BYTE(k) (k >> 3U)
#define GET_KEY_WORD(k) (k >> 5U)

#define UINT32_TO_UINT64(data)     ((uint64_t)(((uint64_t)(data)) & 0x00000000ffffffffU))
#define UINT64L_TO_UINT32(data)    ((uint32_t)(((uint64_t)(data)) & 0x00000000ffffffffU))
#define UINT64H_TO_UINT32(data)    ((uint32_t)((((uint64_t)(data)) >> 32U) & 0x00000000ffffffffU))

#define PKCS1_PADDING           0x01U
#define NO_PADDING              0x02U

#define MD5_PADDING             0x00U
#define SHA1_PADDING            0x01U
#define SHA256_PADDING          0x03U

#define MD5_HASH_SZ             16U
#define SHA1_HASH_SZ            20U
#define SHA256_HASH_SZ          32U

#define RAS_CALCULATE_Q         0x6U
#define RSA_ENABLE_MODULE       0x3U
#define RSA_ENDIAN_MODE         0x8U
#define RSA_RESET               0x1U
#define RSA_CAL_Q_DONE_OFFSET   0x5U


#define   RSA_RFM_OFFSET        0X100U
#define   RSA_RFD_OFFSET        0X200U
#define   RSA_RFC_OFFSET        0X300U
#define   RSA_RFB_OFFSET        0X400U
#define   RSA_RFR_OFFSET        0X500U


/* MWID Offset 0x000 */
#define WJ_RSA_MWID_MWID_Pos            (0U)
#define WJ_RSA_MWID_MWID_Msk            (0x1FU << WJ_RSA_MWID_MWID_Pos)

/*DWID Offset 0x004*/
#define WJ_RSA_DWID_DWID_Pos            (0U)
#define WJ_RSA_DWID_DWID_Msk            (0x7FFU << WJ_RSA_DWID_DWID_Pos)

/*BWID Offset 0x008*/
#define WJ_RSA_BWID_DWID_Pos            (0U)
#define WJ_RSA_BWID_DWID_Msk            (0x1FU << WJ_RSA_BWID_DWID_Pos)

/* CTRL Offset 0x00C */
#define WJ_RSA_CTRL_RSA_START_Pos        (0U)
#define WJ_RSA_CTRL_RSA_START_Msk        (0x1U << WJ_RSA_CTRL_RSA_START_Pos)
#define WJ_RSA_CTRL_RSA_START_EN         WJ_RSA_CTRL_RSA_START_Msk

#define WJ_RSA_CTRL_RSA_EN_Pos           (1U)
#define WJ_RSA_CTRL_RSA_EN_Msk           (0x1U << WJ_RSA_CTRL_RSA_EN_Pos)
#define WJ_RSA_CTRL_RSA_EN_EN            WJ_RSA_CTRL_RSA_EN_Msk

#define WJ_RSA_CTRL_CAL_EN_Pos           (2U)
#define WJ_RSA_CTRL_CAL_EN_Msk           (0x1U << WJ_RSA_CTRL_CAL_EN_Pos)
#define WJ_RSA_CTRL_CAL_EN_EN            WJ_RSA_CTRL_CAL_EN_Msk

#define WJ_RSA_CTRL_Endian_Pos           (3U)
#define WJ_RSA_CTRL_Endian_Msk           (0x1U << WJ_RSA_CTRL_Endian_Pos)
#define WJ_RSA_CTRL_Endian_EN            WJ_RSA_CTRL_Endian_Msk

/* RST Offset 0x010*/
#define WJ_RSA_RST_RST_Pos               (0U)
#define WJ_RSA_RST_RST_Msk               (0x1U << WJ_RSA_RST_RST_Pos)
#define WJ_RSA_RST_RST_EN                WJ_RSA_RST_RST_Msk

/* LP_CNT  Offset 0x014*/
#define WJ_RSA_LP_CNT_LP_CNT_Pos         (0U)
#define WJ_RSA_LP_CNT_LP_CNT_Msk         (0x7FFU << WJ_RSA_LP_CNT_LP_CNT_Pos)
#define WJ_RSA_LP_CNT_LP_CNT_EN          WJ_RSA_LP_CNT_LP_CNT_Msk

/* Q Offset 0x018*/
#define WJ_RSA_Q1_Q_Pos                  (0U)
#define WJ_RSA_Q1_Q_Msk                  (0xFFFFFFFFU << WJ_RSA_Q1_Q_Pos
#define WJ_RSA_Q1_Q_EN                   WJ_RSA_Q1_Q_Msk

/* Q Offset 0x01C*/
#define WJ_RSA_Q2_Q_Pos                  (0U)
#define WJ_RSA_Q2_Q_Msk                  (0xFFFFFFFFU << WJ_RSA_Q2_Q_Pos
#define WJ_RSA_Q2_Q_EN                   WJ_RSA_Q2_Q_Msk

/* RSA_IRSR  Offset 0x020*/
#define WJ_RSA_IRSR_RSA_DONE_Pos         (0U)
#define WJ_RSA_IRSR_RSA_DONE_Msk         (0x1U << WJ_RSA_IRSR_RSA_DONE_Pos)
#define WJ_RSA_IRSR_RSA_DONE_EN          WJ_RSA_IRSR_RSA_DONE_Msk

#define WJ_RSA_IRSR_Abnormal1_Pos        (1U)
#define WJ_RSA_IRSR_Abnormal1_Msk        (0x1U << WJ_RSA_IRSR_Abnormal1_Pos)
#define WJ_RSA_IRSR_Abnormal1_EN         WJ_RSA_IRSR_Abnormal1_Msk

#define WJ_RSA_IRSR_Abnormal2_Pos        (2U)
#define WJ_RSA_IRSR_Abnormal2_Msk        (0x1U << WJ_RSA_IRSR_Abnormal2_Pos)
#define WJ_RSA_IRSR_Abnormal2_EN         WJ_RSA_IRSR_Abnormal2_Msk

#define WJ_RSA_IRSR_Abnormal3_Pos        (3U)
#define WJ_RSA_IRSR_Abnormal3_Msk        (0x1U << WJ_RSA_IRSR_Abnormal3_Pos)
#define WJ_RSA_IRSR_Abnormal3_EN         WJ_RSA_IRSR_Abnormal3_Msk

#define WJ_RSA_IRSR_Abnormal4_Pos        (4U)
#define WJ_RSA_IRSR_Abnormal4_Msk        (0x1U << WJ_RSA_IRSR_Abnormal4_Pos)
#define WJ_RSA_IRSR_Abnormal4_EN         WJ_RSA_IRSR_Abnormal4_Msk

#define WJ_RSA_IRSR_CAL_Q_DONE_Pos       (5U)
#define WJ_RSA_IRSR_CAL_Q_DONE_Msk       (0x1U << WJ_RSA_IRSR_CAL_Q_DONE_Pos)
#define WJ_RSA_IRSR_CAL_Q_DONE_EN        WJ_RSA_IRSR_CAL_Q_DONE_Msk

/* RSA_IMR  Offset 0x020*/
#define WJ_RSA_IMR_RSA_DONE_Pos          (0U)
#define WJ_RSA_IMR_RSA_DONE_Msk          (0x1U << WJ_RSA_IMR_RSA_DONE_Pos)
#define WJ_RSA_IMR_RSA_DONE_EN           WJ_RSA_IMR_RSA_DONE_Msk

#define WJ_RSA_IMR_Abnormal1_Pos         (1U)
#define WJ_RSA_IMR_Abnormal1_Msk         (0x1U << WJ_RSA_IMR_Abnormal1_Pos)
#define WJ_RSA_IMR_Abnormal1_EN          WJ_RSA_IMR_Abnormal1_Msk

#define WJ_RSA_IMR_Abnormal2_Pos         (2U)
#define WJ_RSA_IMR_Abnormal2_Msk         (0x1U << WJ_RSA_IMR_Abnormal2_Pos)
#define WJ_RSA_IMR_Abnormal2_EN          WJ_RSA_IMR_Abnormal2_Msk

#define WJ_RSA_IMR_Abnormal3_Pos         (3U)
#define WJ_RSA_IMR_Abnormal3_Msk         (0x1U << WJ_RSA_IMR_Abnormal3_Pos)
#define WJ_RSA_IMR_Abnormal3_EN          WJ_RSA_IMR_Abnormal3_Msk

#define WJ_RSA_IMR_Abnormal4_Pos         (4U)
#define WJ_RSA_IMR_Abnormal4_Msk         (0x1U << WJ_RSA_IMR_Abnormal4_Pos)
#define WJ_RSA_IMR_Abnormal4_EN          WJ_RSA_IMR_Abnormal4_Msk

#define WJ_RSA_IMR_CAL_Q_DONE_Pos        (5U)
#define WJ_RSA_IMR_CAL_Q_DONE_Msk        (0x1U << WJ_RSA_IMR_CAL_Q_DONE_Pos)
#define WJ_RSA_IMR_CAL_Q_DONE_EN         WJ_RSA_IMR_CAL_Q_DONE_Msk

typedef struct bignum {
    uint32_t pdata[BN_MAX_WORDS];
    uint32_t words;
} bignum_t;

typedef struct {
    __IOM uint32_t RSA_MWID;         /* Offset 0x000(R/W) */
    __IOM uint32_t RSA_DWID;         /* Offset 0x004(R/W) */
    __IOM uint32_t RSA_BWID;         /* Offset 0x008(R/W) */
    __IOM uint32_t RSA_CTRL;         /* Offset 0x00c(R/W) */
    __IOM uint32_t RSA_RST;          /* Offset 0x010(R/W) */
    __IOM uint32_t RSA_LP_CNT;       /* Offset 0x014(R/W) */
    __IOM uint32_t RSA_Q1;           /* Offset 0x018(R/W) */
    __IOM uint32_t RSA_Q2;           /* Offset 0x01c(R/W) */
    __IOM uint32_t RSA_IRSR;         /* Offset 0x020(R/W) */
    __IOM uint32_t RSA_IMR;          /* Offset 0x024(R/W) */
} wj_rsa_regs_t;

static inline void  wj_rsa_set_mwid(wj_rsa_regs_t *addr, uint32_t value)
{
    addr->RSA_MWID = 0U;
    addr->RSA_MWID = value;
}

static inline uint32_t  wj_rsa_get_mwid(wj_rsa_regs_t *addr)
{
    return (addr->RSA_MWID & WJ_RSA_MWID_MWID_Msk);
}

static inline void  wj_rsa_set_dwid(wj_rsa_regs_t *addr, uint32_t value)
{
    addr->RSA_DWID = 0U;
    addr->RSA_DWID = value;
}

static inline uint32_t  wj_rsa_get_dwid(wj_rsa_regs_t *addr)
{
    return (addr->RSA_DWID & WJ_RSA_DWID_DWID_Msk);
}

static inline void  wj_rsa_set_bwid(wj_rsa_regs_t *addr, uint32_t value)
{
    addr->RSA_BWID = 0U;
    addr->RSA_BWID = value;
}

static inline uint32_t  wj_rsa_get_bwid(wj_rsa_regs_t *addr)
{
    return (addr->RSA_BWID & WJ_RSA_BWID_DWID_Msk);
}

static inline void  wj_rsa_en_ctrl_rsa_start(wj_rsa_regs_t *addr)
{
    addr->RSA_CTRL |= WJ_RSA_CTRL_RSA_START_EN;
}

static inline uint32_t wj_rsa_get_ctrl_rsa_start(wj_rsa_regs_t *addr)
{
    return (addr->RSA_CTRL & WJ_RSA_CTRL_RSA_START_Msk);
}

static inline void  wj_rsa_en_ctrl_rsa_en(wj_rsa_regs_t *addr)
{
    addr->RSA_CTRL |= WJ_RSA_CTRL_RSA_EN_EN;
}

static inline void  wj_rsa_dis_ctrl_rsa_en(wj_rsa_regs_t *addr)
{
    addr->RSA_CTRL &= ~WJ_RSA_CTRL_RSA_EN_EN;
}

static inline uint32_t wj_rsa_get_ctrl_rsa_en(wj_rsa_regs_t *addr)
{
    return (addr->RSA_CTRL & WJ_RSA_CTRL_RSA_EN_Msk);
}


static inline void  wj_rsa_en_ctrl_cal_en(wj_rsa_regs_t *addr)
{
    addr->RSA_CTRL |= WJ_RSA_CTRL_CAL_EN_EN;
}

static inline uint32_t wj_rsa_get_ctrl_cal_en(wj_rsa_regs_t *addr)
{
    return (addr->RSA_CTRL & WJ_RSA_CTRL_CAL_EN_Msk);
}


static inline void  wj_rsa_en_ctrl_big_endian(wj_rsa_regs_t *addr)
{
    addr->RSA_CTRL &= ~WJ_RSA_CTRL_Endian_EN;
}

static inline void  wj_rsa_en_ctrl_little_endian(wj_rsa_regs_t *addr)
{

    addr->RSA_CTRL |= WJ_RSA_CTRL_Endian_EN;
}

static inline uint32_t wj_rsa_get_ctrl_endian(wj_rsa_regs_t *addr)
{
    return (addr->RSA_CTRL & WJ_RSA_CTRL_Endian_Msk);
}

static inline void  wj_rsa_en_rst_rst(wj_rsa_regs_t *addr)
{
    addr->RSA_RST |= WJ_RSA_RST_RST_EN;
}

static inline void  wj_rsa_dis_rst_rst(wj_rsa_regs_t *addr)
{
    addr->RSA_RST &= ~WJ_RSA_RST_RST_EN;
}

static inline uint32_t wj_rsa_get_rst_rst(wj_rsa_regs_t *addr)
{
    return (addr->RSA_RST & WJ_RSA_RST_RST_Msk);
}

static inline uint32_t  wj_rsa_get_lp_cnt(wj_rsa_regs_t *addr)
{
    return (addr->RSA_LP_CNT & WJ_RSA_LP_CNT_LP_CNT_Msk);
}

static inline uint32_t  wj_rsa_get_q1(wj_rsa_regs_t *addr)
{
    return addr->RSA_Q1 ;
}

static inline uint32_t  wj_rsa_get_q2(wj_rsa_regs_t *addr)
{
    return addr->RSA_Q2 ;
}

static inline void wj_rsa_set_irsr(wj_rsa_regs_t *addr, uint32_t value)
{
    addr->RSA_IRSR = value;
}
static inline uint32_t wj_rsa_get_irsr(wj_rsa_regs_t *addr)
{
    return (addr->RSA_IRSR);
}

static inline void  wj_rsa_en_irsr_rsa_done(wj_rsa_regs_t *addr)
{
    addr->RSA_IRSR |= WJ_RSA_IRSR_RSA_DONE_EN;
}

static inline void  wj_rsa_dis_irsr_rsa_done(wj_rsa_regs_t *addr)
{
    addr->RSA_IRSR &= ~WJ_RSA_IRSR_RSA_DONE_EN;
}

static inline uint32_t wj_rsa_get_irsr_rsa_done(wj_rsa_regs_t *addr)
{
    return (addr->RSA_IRSR & WJ_RSA_IRSR_RSA_DONE_Msk);
}

static inline void  wj_rsa_en_irsr_abnormal1(wj_rsa_regs_t *addr)
{
    addr->RSA_IRSR |= WJ_RSA_IRSR_Abnormal1_EN;
}

static inline void  wj_rsa_dis_irsr_abnormal1(wj_rsa_regs_t *addr)
{
    addr->RSA_IRSR &= ~WJ_RSA_IRSR_Abnormal1_EN;
}

static inline uint32_t wj_rsa_get_irsr_abnormal1(wj_rsa_regs_t *addr)
{
    return (addr->RSA_IRSR & WJ_RSA_IRSR_Abnormal1_Msk);
}


static inline void  wj_rsa_en_irsr_abnormal2(wj_rsa_regs_t *addr)
{
    addr->RSA_IRSR |= WJ_RSA_IRSR_Abnormal2_EN;
}

static inline void  wj_rsa_dis_irsr_abnormal2(wj_rsa_regs_t *addr)
{
    addr->RSA_IRSR &= ~WJ_RSA_IRSR_Abnormal2_EN;
}

static inline uint32_t wj_rsa_get_irsr_abnormal2(wj_rsa_regs_t *addr)
{
    return (addr->RSA_IRSR & WJ_RSA_IRSR_Abnormal2_Msk);
}


static inline void  wj_rsa_en_irsr_abnormal3(wj_rsa_regs_t *addr)
{
    addr->RSA_IRSR |= WJ_RSA_IRSR_Abnormal3_EN;
}

static inline void  wj_rsa_dis_irsr_abnormal3(wj_rsa_regs_t *addr)
{
    addr->RSA_IRSR &= ~WJ_RSA_IRSR_Abnormal3_EN;
}

static inline uint32_t wj_rsa_get_irsr_abnormal3(wj_rsa_regs_t *addr)
{
    return (addr->RSA_IRSR & WJ_RSA_IRSR_Abnormal3_Msk);
}


static inline void  wj_rsa_en_irsr_abnormal4(wj_rsa_regs_t *addr)
{
    addr->RSA_IRSR |= WJ_RSA_IRSR_Abnormal4_EN;
}

static inline void  wj_rsa_dis_irsr_abnormal4(wj_rsa_regs_t *addr)
{
    addr->RSA_IRSR &= ~WJ_RSA_IRSR_Abnormal4_EN;
}

static inline uint32_t wj_rsa_get_irsr_abnormal4(wj_rsa_regs_t *addr)
{
    return (addr->RSA_IRSR & WJ_RSA_IRSR_Abnormal4_Msk);
}

static inline void  wj_rsa_en_irsr_cal_q_done(wj_rsa_regs_t *addr)
{
    addr->RSA_IRSR |= WJ_RSA_IRSR_CAL_Q_DONE_EN;
}

static inline void  wj_rsa_dis_irsr_cal_q_done(wj_rsa_regs_t *addr)
{
    addr->RSA_IRSR &= ~WJ_RSA_IRSR_CAL_Q_DONE_EN;
}

static inline uint32_t wj_rsa_get_irsr_cal_q_done(wj_rsa_regs_t *addr)
{
    return (addr->RSA_IRSR & WJ_RSA_IRSR_CAL_Q_DONE_Msk);
}

static inline void wj_rsa_reset_imr(wj_rsa_regs_t *addr, uint32_t value)
{
    addr->RSA_IMR &= ~value;
}
static inline void wj_rsa_set_imr(wj_rsa_regs_t *addr, uint32_t value)
{
    addr->RSA_IMR = value;
}

static inline void  wj_rsa_en_imr_rsa_done(wj_rsa_regs_t *addr)
{
    addr->RSA_IMR |= WJ_RSA_IMR_RSA_DONE_EN;
}

static inline void  wj_rsa_dis_imr_rsa_done(wj_rsa_regs_t *addr)
{
    addr->RSA_IMR &= ~WJ_RSA_IMR_RSA_DONE_EN;
}

static inline uint32_t wj_rsa_get_imr_rsa_done(wj_rsa_regs_t *addr)
{
    return (addr->RSA_IMR & WJ_RSA_IMR_RSA_DONE_Msk);
}

static inline void  wj_rsa_en_imr_abnormal1(wj_rsa_regs_t *addr)
{
    addr->RSA_IMR |= WJ_RSA_IMR_Abnormal1_EN;
}

static inline void  wj_rsa_dis_imr_abnormal1(wj_rsa_regs_t *addr)
{
    addr->RSA_IMR &= ~WJ_RSA_IMR_Abnormal1_EN;
}

static inline uint32_t wj_rsa_get_imr_abnormal1(wj_rsa_regs_t *addr)
{
    return (addr->RSA_IMR & WJ_RSA_IMR_Abnormal1_Msk);
}


static inline void  wj_rsa_en_imr_abnormal2(wj_rsa_regs_t *addr)
{
    addr->RSA_IMR |= WJ_RSA_IMR_Abnormal2_EN;
}

static inline void  wj_rsa_dis_imr_abnormal2(wj_rsa_regs_t *addr)
{
    addr->RSA_IMR &= ~WJ_RSA_IMR_Abnormal2_EN;
}

static inline uint32_t wj_rsa_get_imr_abnormal2(wj_rsa_regs_t *addr)
{
    return (addr->RSA_IMR & WJ_RSA_IMR_Abnormal2_Msk);
}


static inline void  wj_rsa_en_imr_abnormal3(wj_rsa_regs_t *addr)
{
    addr->RSA_IMR |= WJ_RSA_IMR_Abnormal3_EN;
}

static inline void  wj_rsa_dis_imr_abnormal3(wj_rsa_regs_t *addr)
{
    addr->RSA_IMR &= ~WJ_RSA_IMR_Abnormal3_EN;
}

static inline uint32_t wj_rsa_get_imr_abnormal3(wj_rsa_regs_t *addr)
{
    return (addr->RSA_IMR & WJ_RSA_IMR_Abnormal3_Msk);
}


static inline void  wj_rsa_en_imr_abnormal4(wj_rsa_regs_t *addr)
{
    addr->RSA_IMR |= WJ_RSA_IMR_Abnormal4_EN;
}

static inline void  wj_rsa_dis_imr_abnormal4(wj_rsa_regs_t *addr)
{
    addr->RSA_IMR &= ~WJ_RSA_IMR_Abnormal4_EN;
}

static inline uint32_t wj_rsa_get_imr_abnormal4(wj_rsa_regs_t *addr)
{
    return (addr->RSA_IMR & WJ_RSA_IMR_Abnormal4_Msk);
}


static inline void  wj_rsa_en_imr_cal_q_done(wj_rsa_regs_t *addr)
{
    addr->RSA_IMR |= WJ_RSA_IMR_CAL_Q_DONE_EN;
}

static inline void  wj_rsa_dis_imr_cal_q_done(wj_rsa_regs_t *addr)
{
    addr->RSA_IMR &= ~WJ_RSA_IMR_CAL_Q_DONE_EN;
}

static inline uint32_t wj_rsa_get_imr_cal_q_done(wj_rsa_regs_t *addr)
{
    return (addr->RSA_IMR & WJ_RSA_IMR_CAL_Q_DONE_Msk);
}


#ifdef __cplusplus
}
#endif

#endif /* __WJ_RSA_LL_H__ */
