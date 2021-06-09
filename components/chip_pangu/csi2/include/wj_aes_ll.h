/*
 *	Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/*******************************************************
 * @file 	wj_aes_ll.h
 * @brief	header file for aes ll driver
 * @version V1.0
 * @date	9. April 2020
 * ******************************************************/

#ifndef _WJ_AES_LL_H_
#define _WJ_AES_LL_H_

#include <stdio.h>
#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*! AES Control Reg, 	offset: 0x40 */
#define WJ_AES_CTRL_START_WORK_Pos                                    (0U)
#define WJ_AES_CTRL_START_WORK_Msk                                    (0x1U << WJ_AES_CTRL_START_WORK_Pos)
#define WJ_AES_CTRL_START_WORK_EN                                     WJ_AES_CTRL_START_WORK_Msk

#define WJ_AES_CTRL_KEY_INT_Pos                                       (1U)
#define WJ_AES_CTRL_KEY_INT_Msk                                       (0x1U << WJ_AES_CTRL_KEY_INT_Pos)
#define WJ_AES_CTRL_KEY_INT_EN                                        WJ_AES_CTRL_KEY_INT_Msk

#define WJ_AES_CTRL_DATA_INT_Pos                                      (2U)
#define WJ_AES_CTRL_DATA_INT_Msk                                      (0x1U << WJ_AES_CTRL_DATA_INT_Pos)
#define WJ_AES_CTRL_DATA_INT_EN                                       WJ_AES_CTRL_DATA_INT_Msk

#define WJ_AES_CTRL_MODE_SEL_Pos                                      (3U)
#define WJ_AES_CTRL_MODE_SEL_Msk                                      (0x1U << WJ_AES_CTRL_MODE_SEL_Pos)
#define WJ_AES_CTRL_MODE_ECB_EN                                       (0x0U << WJ_AES_CTRL_MODE_SEL_Pos)
#define WJ_AES_CTRL_MODE_CBC_EN                                       (0x1U << WJ_AES_CTRL_MODE_SEL_Pos)

#define WJ_AES_CTRL_KEY_LEN_Pos                                       (4U)
#define WJ_AES_CTRL_KEY_LEN_Msk                                       (0x3U << WJ_AES_CTRL_KEY_LEN_Pos)
#define WJ_AES_CTRL_KEY_128_Bit                                       (0x0U << WJ_AES_CTRL_KEY_LEN_Pos)
#define WJ_AES_CTRL_KEY_192_Bit                                       (0x1U << WJ_AES_CTRL_KEY_LEN_Pos)
#define WJ_AES_CTRL_KEY_256_Bit                                       (0x2U << WJ_AES_CTRL_KEY_LEN_Pos)

#define WJ_AES_CTRL_OPT_CODE_Pos                                      (6U)
#define WJ_AES_CTRL_OPT_CODE_Msk                                      (0x3U << WJ_AES_CTRL_OPT_CODE_Pos)
#define WJ_AES_CTRL_OPT_CODE_ENC                                      (0x0U << WJ_AES_CTRL_OPT_CODE_Pos)
#define WJ_AES_CTRL_OPT_CODE_DEC                                      (0x1U << WJ_AES_CTRL_OPT_CODE_Pos)
#define WJ_AES_CTRL_OPT_CODE_KEY                                      (0x2U << WJ_AES_CTRL_OPT_CODE_Pos)
#define WJ_AES_CTRL_OPT_CODE_Rav                                      (0x3U << WJ_AES_CTRL_OPT_CODE_Pos)

#define WJ_AES_CTRL_BIG_ENDIAN_Pos                                    (8U)
#define WJ_AES_CTRL_BIG_ENDIAN_Msk                                    (0x1U << WJ_AES_CTRL_BIG_ENDIAN_Pos)
#define WJ_AES_CTRL_BIG_ENDIAN_EN                                     WJ_AES_CTRL_BIG_ENDIAN_Msk

#define WJ_AES_CTRL_BIT_ENDIAN_Pos                                    (9U)
#define WJ_AES_CTRL_BIT_ENDIAN_Msk                                    (0x1U << WJ_AES_CTRL_BIT_ENDIAN_Pos)
#define WJ_AES_CTRL_BIT_ENDIAN_EN                                     WJ_AES_CTRL_BIT_ENDIAN_Msk

/*! AES State Reg, 	    offset: 0x44 */
#define WJ_AES_STA_BUSY_Pos                                          (0U)
#define WJ_AES_STA_BUSY_Msk                                          (0x1U << WJ_AES_STA_BUSY_Pos)
#define WJ_AES_STA_BUSY_EN                                           WJ_AES_STA_BUSY_Msk

#define WJ_AES_STA_KEY_INT_FLG_Pos                                   (1U)
#define WJ_AES_STA_KEY_INT_FLG_Msk                                   (0x1U << WJ_AES_STA_KEY_INT_FLG_Pos)
#define WJ_AES_STA_KEY_INT_FLG_EN                                    WJ_AES_STA_KEY_INT_FLG_Msk

#define WJ_AES_STA_DATA_INT_FLG_Pos                                  (2U)
#define WJ_AES_STA_DATA_INT_FLG_Msk                                  (0x1U << WJ_AES_STA_DATA_INT_FLG_Pos)
#define WJ_AES_STA_DATA_INT_FLG_EN                                   WJ_AES_STA_DATA_INT_FLG_Msk


typedef struct {
    __IOM uint32_t DATAIN[4];                                       /* Offset: 0x000 (R/W) Data input 0~127         */
    __IOM uint32_t KEY[8];                                          /* Offset: 0x010 (R/W) Key 0~255                */
    __IOM uint32_t IV[4];                                           /* Offset: 0x030 (R/W) Initial Vector: 0~127    */
    __IOM uint32_t CTRL;                                            /* Offset: 0x040 (R/W) AES Control Register     */
    __IOM uint32_t STATE;                                           /* Offset: 0x044 (R/W) AES State Register       */
    __IOM uint32_t DATAOUT[4];                                      /* Offset: 0x048 (R/W) Data Output 0~127        */
} wj_aes_regs_t;


static inline uint32_t wj_aes_read_data_in(wj_aes_regs_t *aes_base, uint32_t idx)
{
    return (aes_base->DATAIN[idx]);
}
static inline void wj_aes_write_data_in(wj_aes_regs_t *aes_base, uint32_t idx, uint32_t value)
{
    aes_base->DATAIN[idx] = value;
}

static inline uint32_t wj_aes_read_key(wj_aes_regs_t *aes_base, uint32_t idx)
{
    return (aes_base->KEY[idx]);
}
static inline void wj_aes_write_key(wj_aes_regs_t *aes_base, uint32_t idx, uint32_t value)
{
    aes_base->KEY[idx] = value;
}

static inline uint32_t wj_aes_read_iv(wj_aes_regs_t *aes_base, uint32_t idx)
{
    return (aes_base->IV[idx]);
}
static inline void wj_aes_write_iv(wj_aes_regs_t *aes_base, uint32_t idx, uint32_t value)
{
    aes_base->IV[idx] = value;
}

static inline void wj_aes_start_work(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL |= (WJ_AES_CTRL_START_WORK_EN);
}
static inline void wj_aes_stop_work(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL &= ~(WJ_AES_CTRL_START_WORK_EN);
}
static inline uint32_t wj_aes_get_work_sta(wj_aes_regs_t *aes_base)
{
    return (((aes_base->CTRL) & WJ_AES_CTRL_START_WORK_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_aes_key_int_en(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL |= (WJ_AES_CTRL_KEY_INT_EN);
}
static inline void wj_aes_key_int_dis(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL &= ~(WJ_AES_CTRL_KEY_INT_EN);
}
static inline uint32_t wj_aes_get_key_int(wj_aes_regs_t *aes_base)
{
    return (((aes_base->CTRL) & WJ_AES_CTRL_KEY_INT_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_aes_data_int_en(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL |= (WJ_AES_CTRL_DATA_INT_EN);
}
static inline void wj_aes_data_int_dis(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL &= ~(WJ_AES_CTRL_DATA_INT_EN);
}
static inline uint32_t wj_aes_get_data_int(wj_aes_regs_t *aes_base)
{
    return (((aes_base->CTRL) & WJ_AES_CTRL_DATA_INT_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_aes_mode_ecb(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL |= (WJ_AES_CTRL_MODE_ECB_EN);
}
static inline void wj_aes_mode_cbc(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL |= (WJ_AES_CTRL_MODE_CBC_EN);
}
static inline uint32_t wj_aes_get_mode(wj_aes_regs_t *aes_base)
{
    return (((aes_base->CTRL) & WJ_AES_CTRL_MODE_SEL_Msk) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_aes_key_len_128bit(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL &= ~WJ_AES_CTRL_KEY_LEN_Msk;
    aes_base->CTRL |= (WJ_AES_CTRL_KEY_128_Bit);
}
static inline void wj_aes_key_len_192bit(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL &= ~WJ_AES_CTRL_KEY_LEN_Msk;
    aes_base->CTRL |= (WJ_AES_CTRL_KEY_192_Bit);
}
static inline void wj_aes_key_len_256bit(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL &= ~WJ_AES_CTRL_KEY_LEN_Msk;
    aes_base->CTRL |= (WJ_AES_CTRL_KEY_256_Bit);
}
static inline uint32_t wj_aes_get_key_len(wj_aes_regs_t *aes_base)
{
    return (((aes_base->CTRL) & WJ_AES_CTRL_KEY_LEN_Msk) >> WJ_AES_CTRL_KEY_LEN_Pos);
}

static inline void wj_aes_opt_code_encrypt(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL &= ~WJ_AES_CTRL_OPT_CODE_Msk;
    aes_base->CTRL |= (WJ_AES_CTRL_OPT_CODE_ENC);
}
static inline void wj_aes_opt_code_decrypt(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL &= ~WJ_AES_CTRL_OPT_CODE_Msk;
    aes_base->CTRL |= (WJ_AES_CTRL_OPT_CODE_DEC);
}
static inline void wj_aes_opt_code_key_expand(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL &= ~WJ_AES_CTRL_OPT_CODE_Msk;
    aes_base->CTRL |= (WJ_AES_CTRL_OPT_CODE_KEY);
}
static inline uint32_t wj_aes_get_opt_code(wj_aes_regs_t *aes_base)
{
    return (((aes_base->CTRL) & WJ_AES_CTRL_OPT_CODE_Msk) >> WJ_AES_CTRL_OPT_CODE_Pos);
}

static inline void wj_aes_big_endian_big(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL |= (WJ_AES_CTRL_BIG_ENDIAN_EN);
}
static inline void wj_aes_big_endian_little(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL &= ~(WJ_AES_CTRL_BIG_ENDIAN_EN);
}
static inline uint32_t wj_aes_get_big_endian(wj_aes_regs_t *aes_base)
{
    return (((aes_base->CTRL) & WJ_AES_CTRL_BIG_ENDIAN_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_aes_bit_endian_big(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL |= (WJ_AES_CTRL_BIT_ENDIAN_EN);
}
static inline void wj_aes_bit_endian_little(wj_aes_regs_t *aes_base)
{
    aes_base->CTRL &= ~(WJ_AES_CTRL_BIT_ENDIAN_EN);
}
static inline uint32_t wj_aes_get_bit_endian(wj_aes_regs_t *aes_base)
{
    return (((aes_base->CTRL) & WJ_AES_CTRL_BIT_ENDIAN_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline uint32_t wj_aes_get_busy_sta(wj_aes_regs_t *aes_base)
{
    return (((aes_base->STATE) & WJ_AES_STA_BUSY_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_aes_clear_key_int_flag(wj_aes_regs_t *aes_base)
{
    aes_base->STATE &= ~(WJ_AES_STA_KEY_INT_FLG_EN);
}
static inline uint32_t wj_aes_get_key_int_flag(wj_aes_regs_t *aes_base)
{
    return (((aes_base->STATE) & WJ_AES_STA_KEY_INT_FLG_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline void wj_aes_clear_data_int_flag(wj_aes_regs_t *aes_base)
{
    aes_base->STATE &= ~(WJ_AES_STA_DATA_INT_FLG_EN);
}
static inline uint32_t wj_aes_get_data_int_flag(wj_aes_regs_t *aes_base)
{
    return (((aes_base->STATE) & WJ_AES_STA_DATA_INT_FLG_EN) ? (uint32_t)1 : (uint32_t)0);
}

static inline uint32_t wj_aes_read_data_out(wj_aes_regs_t *aes_base, uint32_t idx)
{
    return (aes_base->DATAOUT[idx]);
}
static inline void wj_aes_write_data_out(wj_aes_regs_t *aes_base, uint32_t idx, uint32_t value)
{
    aes_base->DATAOUT[idx] = value;
}

#ifdef __cplusplus
}
#endif

#endif /* _WJ_AES_LL_H_ */
