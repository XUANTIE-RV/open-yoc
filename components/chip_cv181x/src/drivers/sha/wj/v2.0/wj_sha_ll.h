/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_sha_ll.h
 * @brief    header file for sha driver
 * @version  V1.0
 * @date     9. April 2020
 ******************************************************************************/
#ifndef _WJ_SHA_H_
#define _WJ_SHA_H_

#include <stdio.h>
#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! SHA CONTROL Reg, 	offset: 0x00 */
#define WJ_SHA_CTRL_MODE_LEN_Pos                                      (0U)
#define WJ_SHA_CTRL_MODE_LEN_Msk                                      (0x7U << WJ_SHA_CTRL_MODE_LEN_Pos)
#define WJ_SHA_CTRL_MODE_LEN_1                                        (0x1U << WJ_SHA_CTRL_MODE_LEN_Pos)
#define WJ_SHA_CTRL_MODE_LEN_256                                      (0x2U << WJ_SHA_CTRL_MODE_LEN_Pos)
#define WJ_SHA_CTRL_MODE_LEN_224                                      (0x3U << WJ_SHA_CTRL_MODE_LEN_Pos)
#define WJ_SHA_CTRL_MODE_LEN_512                                      (0x4U << WJ_SHA_CTRL_MODE_LEN_Pos)
#define WJ_SHA_CTRL_MODE_LEN_384                                      (0x5U << WJ_SHA_CTRL_MODE_LEN_Pos)
#define WJ_SHA_CTRL_MODE_LEN_512_256                                  (0x6U << WJ_SHA_CTRL_MODE_LEN_Pos)
#define WJ_SHA_CTRL_MODE_LEN_512_224                                  (0x7U << WJ_SHA_CTRL_MODE_LEN_Pos)
#define WJ_SHA_CTRL_INITIAL_Pos                                       (3U)
#define WJ_SHA_CTRL_INITIAL_Msk                                       (0x1U << WJ_SHA_CTRL_INITIAL_Pos)
#define WJ_SHA_CTRL_INITIAL_EN                                        WJ_SHA_CTRL_INITIAL_Msk
#define WJ_SHA_CTRL_INTERRUPT_Pos                                     (4U)
#define WJ_SHA_CTRL_INTERRUPT_Msk                                     (0x1U << WJ_SHA_CTRL_INTERRUPT_Pos)
#define WJ_SHA_CTRL_INTERRUPT_EN                                      WJ_SHA_CTRL_INTERRUPT_Msk
#define WJ_SHA_CTRL_ENDIAN_MODE_Pos                                   (5U)
#define WJ_SHA_CTRL_ENDIAN_MODE_Msk                                   (0x1U << WJ_SHA_CTRL_ENDIAN_MODE_Pos)
#define WJ_SHA_CTRL_ENDIAN_MODE_EN                                    WJ_SHA_CTRL_ENDIAN_MODE_Msk
#define WJ_SHA_CTRL_CALCULATE_Pos                                     (6U)
#define WJ_SHA_CTRL_CALCULATE_Msk                                     (0x1U << WJ_SHA_CTRL_CALCULATE_Pos)
#define WJ_SHA_CTRL_CALCULATE_EN                                      WJ_SHA_CTRL_CALCULATE_Msk

/*! SHA INTSTATE Reg, 	    offset: 0x04 */
#define WJ_SHA_INTSTATE_INT_DONE_Pos                                  (0U)
#define WJ_SHA_INTSTATE_INT_DONE_Msk                                  (0x1U << WJ_SHA_INTSTATE_INT_DONE_Pos)
#define WJ_SHA_INTSTATE_INT_DONE_EN                                   WJ_SHA_INTSTATE_INT_DONE_Msk

#define WJ_SHA_INTSTATE_INT_ERROR_Pos                                 (1U)
#define WJ_SHA_INTSTATE_INT_ERROR_Msk                                 (0x1U << WJ_SHA_INTSTATE_INT_ERROR_Pos)
#define WJ_SHA_INTSTATE_INT_ERROR_EN                                  WJ_SHA_INTSTATE_INT_ERROR_Msk

typedef struct {
    __IOM uint32_t SHA_CON;                     /* Offset: 0x000 (R/W)  Control register */
    __IOM uint32_t SHA_INTSTATE;                /* Offset: 0x004 (R/W)  Instatus register */
    __IOM uint32_t SHA_HL[16];                  /* Offset: 0x008 (R/W)  HL register */
    __IOM uint32_t SHA_DATA1[16];               /* Offset: 0x048 (R/W)  DATA1 register */
    __IOM uint32_t SHA_DATA2[16];               /* Offset: 0x088 (R/W)  DATA2 register (only for SHA-384, SHA-512)*/
} wj_sha_regs_t;

/**
 * \brief   Control
 *          CPU writes this bit to enable the SHA to deal with thedata after cpu give the data and address.
 *          It is cleared by SHA when encoding the entire message.
 *          CPU can read this bit to verify whether the encoding is complete after setting it.
*/
static inline void wj_sha_ctrl_start_cal(wj_sha_regs_t *sha_base)
{
    sha_base->SHA_CON |= (WJ_SHA_CTRL_CALCULATE_EN);
}
static inline void wj_sha_ctrl_stop_cal(wj_sha_regs_t *sha_base)
{
    sha_base->SHA_CON &= ~(WJ_SHA_CTRL_CALCULATE_EN);
}
static inline uint32_t wj_sha_ctrl_get_cal(wj_sha_regs_t *sha_base)
{
    return (((sha_base->SHA_CON) & WJ_SHA_CTRL_CALCULATE_Msk) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_sha_ctrl_endian_big(wj_sha_regs_t *sha_base)
{
    sha_base->SHA_CON &= ~(WJ_SHA_CTRL_ENDIAN_MODE_EN);
}
static inline void wj_sha_ctrl_endian_little(wj_sha_regs_t *sha_base)
{
    sha_base->SHA_CON |= (WJ_SHA_CTRL_ENDIAN_MODE_EN);
}
static inline uint32_t wj_sha_ctrl_get_endian(wj_sha_regs_t *sha_base)
{
    return (((sha_base->SHA_CON) & WJ_SHA_CTRL_ENDIAN_MODE_Msk) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_sha_ctrl_interrupt_en(wj_sha_regs_t *sha_base)
{
    sha_base->SHA_CON |= (WJ_SHA_CTRL_INTERRUPT_EN);
}
static inline void wj_sha_ctrl_interrupt_dis(wj_sha_regs_t *sha_base)
{
    sha_base->SHA_CON &= ~(WJ_SHA_CTRL_INTERRUPT_EN);
}
static inline uint32_t wj_sha_ctrl_get_interrupt(wj_sha_regs_t *sha_base)
{
    return (((sha_base->SHA_CON) & WJ_SHA_CTRL_INTERRUPT_Msk) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_sha_mode_en_initial(wj_sha_regs_t *sha_base)
{
    sha_base->SHA_CON |= (WJ_SHA_CTRL_INITIAL_EN);
}
static inline void wj_sha_mode_dis_initial(wj_sha_regs_t *sha_base)
{
    sha_base->SHA_CON &= ~(WJ_SHA_CTRL_INITIAL_EN);
}
static inline uint32_t wj_sha_mode_get_initial(wj_sha_regs_t *sha_base)
{
    return (((sha_base->SHA_CON) & WJ_SHA_CTRL_INITIAL_Msk) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   Mode
 *          3’b001: SHA-1
 *          3’b010: SHA-256
 *          3’b011: SHA-224
 *          3’b100: SHA-512
 *          3’b101: SHA-384
 *          3’b110: SHA-512/256
 *          3’b111: SHA-512/224
*/
static inline void wj_sha_mode_sel(wj_sha_regs_t *sha_base, uint32_t mode)
{
    sha_base->SHA_CON &= ~WJ_SHA_CTRL_MODE_LEN_Msk;
    sha_base->SHA_CON |= (mode << WJ_SHA_CTRL_MODE_LEN_Pos);
}
static inline void wj_sha_mode_sha_config(wj_sha_regs_t *sha_base, uint32_t mode)
{
    sha_base->SHA_CON &= ~WJ_SHA_CTRL_MODE_LEN_Msk;
    sha_base->SHA_CON |= (mode << WJ_SHA_CTRL_MODE_LEN_Pos);
}
static inline uint32_t wj_sha_mode_get_len(wj_sha_regs_t *sha_base)
{
    return (((sha_base->SHA_CON) & WJ_SHA_CTRL_MODE_LEN_Msk) >> WJ_SHA_CTRL_MODE_LEN_Pos);
}

/**
 * \brief   INTSTATE
*/
static inline void wj_sha_state_set_int_done(wj_sha_regs_t *sha_base)
{
    sha_base->SHA_INTSTATE |= (WJ_SHA_INTSTATE_INT_DONE_EN);
}
static inline void wj_sha_state_clr_int_done(wj_sha_regs_t *sha_base)
{
    sha_base->SHA_INTSTATE &= ~(WJ_SHA_INTSTATE_INT_DONE_EN);
}
static inline uint32_t wj_sha_state_get_int_done(wj_sha_regs_t *sha_base)
{
    return (((sha_base->SHA_INTSTATE) & WJ_SHA_INTSTATE_INT_DONE_Msk) ? (uint32_t)1 : (uint32_t)0);
}

static inline uint32_t wj_sha_intstate_get_int_error(wj_sha_regs_t *sha_base)
{
    return (((sha_base->SHA_INTSTATE) & WJ_SHA_INTSTATE_INT_ERROR_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   HL
 *          The hash value.
*/
static inline uint32_t *wj_sha_get_hash_address(wj_sha_regs_t *sha_base)
{
    return ((uint32_t *)sha_base->SHA_HL);
}
static inline void wj_sha_write_hash_value(wj_sha_regs_t *sha_base, uint32_t *p_data)
{
    uint32_t i;

    for (i = 0U; i < 16U; i++) {
        sha_base->SHA_HL[i] = *(p_data + i);
    }
}
static inline void wj_sha_clear_hash_value(wj_sha_regs_t *sha_base)
{
    uint32_t i;

    for (i = 0U; i < 16U; i++) {
        sha_base->SHA_HL[i] = 0U;
    }
}
/**
 * \brief   DATA1
 *          The result value.
*/
static inline uint32_t *wj_sha_read_data1_value(wj_sha_regs_t *sha_base)
{
    return ((uint32_t *)sha_base->SHA_DATA1);
}
static inline void wj_sha_write_data1_value(wj_sha_regs_t *sha_base, uint32_t *p_data)
{
    uint32_t i;

    for (i = 0U; i < 16U; i++) {
        sha_base->SHA_DATA1[i] = *(p_data + i);
    }
}
static inline void wj_sha_clear_data1_value(wj_sha_regs_t *sha_base)
{
    uint32_t i;

    for (i = 0U; i < 16U; i++) {
        sha_base->SHA_DATA1[i] = 0U;
    }
}
/**
 * \brief   DATA2
 *          The result value.
*/
static inline uint32_t *wj_sha_read_data2_value(wj_sha_regs_t *sha_base)
{
    return ((uint32_t *)sha_base->SHA_DATA2);
}
static inline void wj_sha_write_data2_value(wj_sha_regs_t *sha_base, uint32_t *p_data)
{
    uint32_t i;

    for (i = 0U; i < 16U; i++) {
        sha_base->SHA_DATA2[i] = *(p_data + i);
    }
}
static inline void wj_sha_clear_data2_value(wj_sha_regs_t *sha_base)
{
    uint32_t i;

    for (i = 0U; i < 16U; i++) {
        sha_base->SHA_DATA2[i] = 0U;
    }
}

/**
 * \brief   DATA all
 *          The result value.
*/
static inline uint32_t *wj_sha_read_data_value(wj_sha_regs_t *sha_base)
{
    return ((uint32_t *)wj_sha_read_data1_value(sha_base));
}
static inline void wj_sha_write_data_value(wj_sha_regs_t *sha_base, uint32_t *p_data)
{
    wj_sha_write_data1_value(sha_base, p_data);
    wj_sha_write_data2_value(sha_base, (p_data + 16));
}
static inline void wj_sha_clear_data_value(wj_sha_regs_t *sha_base)
{
    wj_sha_clear_data1_value(sha_base);
    wj_sha_clear_data2_value(sha_base);
}

#ifdef __cplusplus
}
#endif

#endif
