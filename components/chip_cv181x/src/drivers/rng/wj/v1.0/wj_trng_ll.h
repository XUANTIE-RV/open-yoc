/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_trng_ll.h
 * @brief
 * @version
 * @date     2020-03-04
 ******************************************************************************/

#ifndef _WJ_TRNG_LL_H_
#define _WJ_TRNG_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TCR, offset: 0x00 */
#define WJ_TRNG_TCR_LPMD_Pos                        (2U)
#define WJ_TRNG_TCR_LPMD_Msk                        (0x1U << WJ_TRNG_TCR_LPMD_Pos)
#define WJ_TRNG_TCR_LPMD_EN                         WJ_TRNG_TCR_LPMD_Msk

#define WJ_TRNG_TCR_EN_Pos                          (1U)
#define WJ_TRNG_TCR_EN_Msk                          (0x1U << WJ_TRNG_TCR_EN_Pos)
#define WJ_TRNG_TCR_EN                              WJ_TRNG_TCR_EN_Msk

#define WJ_TRNG_TCR_DVLD_Pos                        (0U)
#define WJ_TRNG_TCR_DVLD_Msk                        (0x1U << WJ_TRNG_TCR_DVLD_Pos)
#define WJ_TRNG_TCR_DVLD_EN                         WJ_TRNG_TCR_DVLD_Msk

typedef struct {
    __IOM uint32_t  TCR;             /* Offset: 0x000 (W/R)  RBG Control Register */
    __IM  uint32_t  TDR;             /* Offset: 0x004 ( /R)  RBG Data Register */
} wj_trng_regs_t;

static inline void wj_trng_enable_low_power(wj_trng_regs_t *trng)
{
    trng->TCR |= WJ_TRNG_TCR_LPMD_EN;
}

static inline void wj_trng_disable_low_power(wj_trng_regs_t *trng)
{
    trng->TCR &= ~WJ_TRNG_TCR_LPMD_EN;
}

static inline void wj_trng_enable_module(wj_trng_regs_t *trng)
{
    trng->TCR |= WJ_TRNG_TCR_EN;
}

static inline void wj_trng_disable_module(wj_trng_regs_t *trng)
{
    trng->TCR &= ~WJ_TRNG_TCR_EN;
}

static inline uint32_t wj_trng_data_valid(wj_trng_regs_t *trng)
{
    uint32_t state;
    
    if((trng->TCR & WJ_TRNG_TCR_DVLD_Msk) == WJ_TRNG_TCR_DVLD_Msk){
        state = 1U;
    }else{
        state = 0U;
    }

    return state;
}

static inline uint32_t wj_trng_get_data(wj_trng_regs_t *trng)
{
    return trng->TDR;
}

#ifdef __cplusplus
}
#endif

#endif  /* _WJ_TRNG_LL_H_*/
