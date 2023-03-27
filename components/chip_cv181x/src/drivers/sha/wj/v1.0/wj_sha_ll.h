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

/*! SHA Control Reg, 	offset: 0x00 */
#define WJ_SHA_CTRL_ENABLE_Pos                                      (0U)
#define WJ_SHA_CTRL_ENABLE_Msk                                      (0x1U << WJ_SHA_CTRL_ENABLE_Pos)
#define WJ_SHA_CTRL_ENABLE_EN                                       WJ_SHA_CTRL_ENABLE_Msk

/*! SHA Mode Reg, 	    offset: 0x04 */
#define WJ_SHA_MODE_LEN_Pos                                         (0U)
#define WJ_SHA_MODE_LEN_Msk                                         (0x7U << WJ_SHA_MODE_LEN_Pos)
#define WJ_SHA_MODE_LEN_1                                           (0x1U << WJ_SHA_MODE_LEN_Pos)
#define WJ_SHA_MODE_LEN_256                                         (0x2U << WJ_SHA_MODE_LEN_Pos)
#define WJ_SHA_MODE_LEN_224                                         (0x3U << WJ_SHA_MODE_LEN_Pos)
#define WJ_SHA_MODE_LEN_512                                         (0x4U << WJ_SHA_MODE_LEN_Pos)
#define WJ_SHA_MODE_LEN_384                                         (0x5U << WJ_SHA_MODE_LEN_Pos)
#define WJ_SHA_MODE_LEN_512_256                                     (0x6U << WJ_SHA_MODE_LEN_Pos)
#define WJ_SHA_MODE_LEN_512_224                                     (0x7U << WJ_SHA_MODE_LEN_Pos)

#define WJ_SHA_MODE_EN_INIT_Pos                                     (3U)
#define WJ_SHA_MODE_EN_INIT_Msk                                     (0x1U << WJ_SHA_MODE_EN_INIT_Pos)
#define WJ_SHA_MODE_EN_INIT_EN                                      WJ_SHA_MODE_EN_INIT_Msk

#define WJ_SHA_MODE_EN_LOOP_Pos                                     (4U)
#define WJ_SHA_MODE_EN_LOOP_Msk                                     (0x1U << WJ_SHA_MODE_EN_LOOP_Pos)
#define WJ_SHA_MODE_EN_LOOP_EN                                      WJ_SHA_MODE_EN_LOOP_Msk

#define WJ_SHA_MODE_LOOP_LEN_Pos                                    (5U)
#define WJ_SHA_MODE_LOOP_LEN_Msk                                    (0xFFFFFU << WJ_SHA_MODE_LOOP_LEN_Pos)

#define WJ_SHA_MODE_WITHOUT_Pos                                     (25U)
#define WJ_SHA_MODE_WITHOUT_Msk                                     (0x1U << WJ_SHA_MODE_WITHOUT_Pos)
#define WJ_SHA_MODE_WITHOUT_EN                                      WJ_SHA_MODE_WITHOUT_Msk

/*! SHA IntState Reg, 	 offset: 0x08 */
#define WJ_SHA_INTSTATE_DONE_Pos                                    (0U)
#define WJ_SHA_INTSTATE_DONE_Msk                                    (0x1U << WJ_SHA_INTSTATE_DONE_Pos)
#define WJ_SHA_INTSTATE_DONE_EN                                     WJ_SHA_INTSTATE_DONE_Msk

#define WJ_SHA_INTSTATE_ERR_Pos                                     (1U)
#define WJ_SHA_INTSTATE_ERR_Msk                                     (0x1U << WJ_SHA_INTSTATE_ERR_Pos)
#define WJ_SHA_INTSTATE_ERR_EN                                      WJ_SHA_INTSTATE_ERR_Msk

/*! SHA BaseAddr Reg, 	 offset: 0x0c */
#define WJ_SHA_BASE_ADDR_Pos                                        (0U)
#define WJ_SHA_BASE_ADDR_Msk                                        (0x1U << WJ_SHA_BASE_ADDR_Pos)
#define WJ_SHA_BASE_ADDR_EN                                         WJ_SHA_BASE_ADDR_Msk

typedef struct {
    __OM  uint32_t CTRL;                    /*!< Offset: 0x000 ( /W)  Control register */
    __IOM uint32_t MODE;                    /*!< Offset: 0x004 (R/W)  Mode register */
    __IOM uint32_t INTSTATE;                /*!< Offset: 0x008 (R/W)  Instatus register */
    __IOM uint32_t BASEADDR;                /*!< Offset: 0x00c (R/W)  Baseaddr register */
    __IOM uint32_t DESTADDR;                /*!< Offset: 0x010 (R/W)  Dest addr register */
    __IOM uint32_t COUNTER[4];              /*!< Offset: 0x014 (R/W)  count0~3 register */
    __IOM uint32_t HL[16];                  /*!< Offset: 0x024 (R/W)  H0L~H7H register */
} wj_sha_regs_t;

/**
 * \brief   Control
 *          CPU writes this bit to enable the SHA to deal with thedata after cpu give the data and address.
 *          It is cleared by SHA when encoding the entire message.
 *          CPU can read this bit to verify whether the encoding is complete after setting it.
*/
static inline void wj_sha_ctrl_start_cal(wj_sha_regs_t *sha_base)
{
    sha_base->CTRL |= (WJ_SHA_CTRL_ENABLE_EN);
}
static inline void wj_sha_ctrl_stop_cal(wj_sha_regs_t *sha_base)
{
    sha_base->CTRL &= ~(WJ_SHA_CTRL_ENABLE_EN);
}
static inline uint32_t wj_sha_ctrl_get_cal(wj_sha_regs_t *sha_base)
{
    return (((sha_base->CTRL) & WJ_SHA_CTRL_ENABLE_EN) ? (uint32_t)1 : (uint32_t)0);
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
    sha_base->MODE &= ~WJ_SHA_MODE_LEN_Msk;
    sha_base->MODE |= (mode << WJ_SHA_MODE_LEN_Pos);
}
static inline void wj_sha_mode_sha_1(wj_sha_regs_t *sha_base)
{
    sha_base->MODE &= ~WJ_SHA_MODE_LEN_Msk;
    sha_base->MODE |= (WJ_SHA_MODE_LEN_1);
}
static inline void wj_sha_mode_sha_256(wj_sha_regs_t *sha_base)
{
    sha_base->MODE &= ~WJ_SHA_MODE_LEN_Msk;
    sha_base->MODE |= (WJ_SHA_MODE_LEN_256);
}
static inline void wj_sha_mode_sha_224(wj_sha_regs_t *sha_base)
{
    sha_base->MODE &= ~WJ_SHA_MODE_LEN_Msk;
    sha_base->MODE |= (WJ_SHA_MODE_LEN_224);
}
static inline void wj_sha_mode_sha_512(wj_sha_regs_t *sha_base)
{
    sha_base->MODE &= ~WJ_SHA_MODE_LEN_Msk;
    sha_base->MODE |= (WJ_SHA_MODE_LEN_512);
}
static inline void wj_sha_mode_sha_384(wj_sha_regs_t *sha_base)
{
    sha_base->MODE &= ~WJ_SHA_MODE_LEN_Msk;
    sha_base->MODE |= (WJ_SHA_MODE_LEN_384);
}
static inline void wj_sha_mode_sha_512_256(wj_sha_regs_t *sha_base)
{
    sha_base->MODE &= ~WJ_SHA_MODE_LEN_Msk;
    sha_base->MODE |= (WJ_SHA_MODE_LEN_512_256);
}
static inline void wj_sha_mode_sha_512_224(wj_sha_regs_t *sha_base)
{
    sha_base->MODE &= ~WJ_SHA_MODE_LEN_Msk;
    sha_base->MODE |= (WJ_SHA_MODE_LEN_512_224);
}
static inline void wj_sha_mode_sha_config(wj_sha_regs_t *sha_base, uint32_t mode)
{
    sha_base->MODE &= ~WJ_SHA_MODE_LEN_Msk;
    sha_base->MODE |= (mode << WJ_SHA_MODE_LEN_Pos);
}

static inline uint32_t wj_sha_mode_get_len(wj_sha_regs_t *sha_base)
{
    return (((sha_base->MODE) & WJ_SHA_MODE_LEN_Msk) >> WJ_SHA_MODE_LEN_Pos);
}
/**
 * \brief   EnableLoop
 *          CPU sets this bit to encode the message with the initial value of A-H
 *          (A-E when Mask=3’b010) which is also written by CPU instead of standard initial value.
 *          It is used when user divides one message to several messages.
 *          Store the last digest and write it to A-H when continuing a new message.
*/
static inline void wj_sha_mode_en_initial(wj_sha_regs_t *sha_base)
{
    sha_base->MODE |= (WJ_SHA_MODE_EN_INIT_EN);
}
static inline void wj_sha_mode_dis_initial(wj_sha_regs_t *sha_base)
{
    sha_base->MODE &= ~(WJ_SHA_MODE_EN_INIT_EN);
}
static inline uint32_t wj_sha_mode_get_initial(wj_sha_regs_t *sha_base)
{
    return (((sha_base->MODE) & WJ_SHA_MODE_EN_INIT_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   LoopLength
 *          If the message is too long, store the data in a loop.
 *          CPU should write the LoopLength field.
 *          When reach the edge of the memory.
 *          Store the data in the start address ‘baseaddr’.
*/
static inline void wj_sha_mode_en_loop(wj_sha_regs_t *sha_base)
{
    sha_base->MODE |= (WJ_SHA_MODE_EN_LOOP_EN);
}
static inline void wj_sha_mode_dis_loop(wj_sha_regs_t *sha_base)
{
    sha_base->MODE &= ~(WJ_SHA_MODE_EN_LOOP_EN);
}
static inline uint32_t wj_sha_mode_get_loop(wj_sha_regs_t *sha_base)
{
    return (((sha_base->MODE) & WJ_SHA_MODE_EN_LOOP_Msk) ? (uint32_t)1 : (uint32_t)0);
}

/**
 * \brief   LoopLength:
 *          It is the length of loop in 1024 bits.
 *          The length of the address is LoopLength * 1024 bit.
 */
static inline uint32_t wj_sha_mode_read_looplength(wj_sha_regs_t *sha_base)
{
    return (sha_base->MODE & WJ_SHA_MODE_LOOP_LEN_Msk) >> WJ_SHA_MODE_LOOP_LEN_Pos;
}
static inline void wj_sha_mode_write_looplength(wj_sha_regs_t *sha_base, uint32_t value)
{
    sha_base->MODE &= ~WJ_SHA_MODE_LOOP_LEN_Msk;
    sha_base->MODE |= (value << WJ_SHA_MODE_LOOP_LEN_Pos);
}
/**
 * \brief   Without_count:
 *          This is set when message is without count.
 *          It means that this is not an integrated message, maybe just part of the message,
 *          but it should be the multiple of the block length(512/1024),
 *          so user do not need to write the length of the message to the memory after massage.
 *          This feature is used when users do not want the sha to pad the message so they can
 *          add the message with using the internal result in H0-H7.
 *          This is ususlly used with before EnableInitial.
*/
static inline void wj_sha_mode_en_without_count(wj_sha_regs_t *sha_base)
{
    sha_base->MODE |= (WJ_SHA_MODE_WITHOUT_EN);
}
static inline void wj_sha_mode_dis_without_count(wj_sha_regs_t *sha_base)
{
    sha_base->MODE &= ~(WJ_SHA_MODE_WITHOUT_EN);
}
static inline uint32_t wj_sha_mode_get_without_count(wj_sha_regs_t *sha_base)
{
    return (((sha_base->MODE) & WJ_SHA_MODE_WITHOUT_Msk) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_sha_mode_all_clear(wj_sha_regs_t *sha_base)
{
    sha_base->MODE &= ~(WJ_SHA_MODE_LEN_Msk | WJ_SHA_MODE_EN_INIT_Msk | WJ_SHA_MODE_EN_LOOP_Msk |
                        WJ_SHA_MODE_LOOP_LEN_Msk | WJ_SHA_INTSTATE_DONE_Msk);
}
/**
 * \brief   Int_Done
 *          This bit is set by SHA after finishing encoding the message and writing the digest to the base adddress.
 *          CPU can clean it to start a new message encoding.
 *          Cleaned by written.
*/
static inline void wj_sha_state_set_int_done(wj_sha_regs_t *sha_base)
{
    sha_base->INTSTATE |= (WJ_SHA_INTSTATE_DONE_EN);
}
static inline void wj_sha_state_clr_int_done(wj_sha_regs_t *sha_base)
{
    sha_base->INTSTATE &= ~(WJ_SHA_INTSTATE_DONE_EN);
}
static inline uint32_t wj_sha_state_get_int_done(wj_sha_regs_t *sha_base)
{
    return (((sha_base->INTSTATE) & WJ_SHA_INTSTATE_DONE_Msk) ? (uint32_t)1 : (uint32_t)0);
}
/**
 * \brief   Int_Error
 *          This bit is set by SHA when ahb master resp errors.
 *          Cleaned by CPU. Cleaned by written
*/
static inline void wj_sha_state_set_int_error(wj_sha_regs_t *sha_base)
{
    sha_base->INTSTATE |= (WJ_SHA_INTSTATE_ERR_EN);
}
static inline void wj_sha_state_clr_int_error(wj_sha_regs_t *sha_base)
{
    sha_base->INTSTATE &= ~(WJ_SHA_INTSTATE_ERR_EN);
}
static inline uint32_t wj_sha_state_get_int_error(wj_sha_regs_t *sha_base)
{
    return (((sha_base->INTSTATE) & WJ_SHA_INTSTATE_ERR_Msk) ? (uint32_t)1 : (uint32_t)0);
}
static inline void wj_sha_state_all_clear(wj_sha_regs_t *sha_base)
{
    sha_base->INTSTATE &= ~(WJ_SHA_INTSTATE_DONE_Msk | WJ_SHA_INTSTATE_ERR_Msk);
}
/**
 * \brief   BaseAddr
 *          The base address of the message data,
 *          cpu writes it at the beginning.
*/
static inline uint32_t wj_sha_read_base_addr(wj_sha_regs_t *sha_base)
{
    return (sha_base->BASEADDR);
}
static inline void wj_sha_write_base_addr(wj_sha_regs_t *sha_base, uint32_t *p_addr)
{
    sha_base->BASEADDR = (uint32_t)p_addr;
}

/**
 * \brief   DestAddr
 *          The dest address of the message data,
 *          cpu writes it at the beginning.
*/
static inline uint32_t wj_sha_read_dest_addr(wj_sha_regs_t *sha_base)
{
    return (sha_base->DESTADDR);
}
static inline void wj_sha_write_dest_addr(wj_sha_regs_t *sha_base, uint32_t *p_addr)
{
    sha_base->DESTADDR = (uint32_t)p_addr;
}

/**
 * \brief   Count
 *          The total count of the message data in bit
*/
static inline uint32_t *wj_sha_read_count(wj_sha_regs_t *sha_base)
{
    return ((uint32_t *)sha_base->COUNTER);
}
static inline void wj_sha_write_count(wj_sha_regs_t *sha_base, uint32_t *p_data)
{
    uint32_t i;

    for (i = 0U; i < 4U; i++) {
        sha_base->COUNTER[i] = *(p_data + i);
    }

    memcpy((uint8_t *)sha_base->COUNTER, (uint8_t *)p_data, sizeof(sha_base->COUNTER));
}

/**
 * \brief   HL
 *          The hash value.
*/
static inline uint32_t *wj_sha_read_hash_value(wj_sha_regs_t *sha_base)
{
    return ((uint32_t *)sha_base->HL);
}
static inline void wj_sha_write_hash_value(wj_sha_regs_t *sha_base, uint32_t *p_data)
{
    uint32_t i;

    for (i = 0U; i < 16U; i++) {
        sha_base->HL[i] = *(p_data + i);
    }
}
static inline void wj_sha_clear_hash_value(wj_sha_regs_t *sha_base)
{
    uint32_t i;

    for (i = 0U; i < 16U; i++) {
        sha_base->HL[i] = 0U;
    }
}

#ifdef __cplusplus
}
#endif

#endif
