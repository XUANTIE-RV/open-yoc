/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     osr_trng_ll.h
 * @brief
 * @version
 * @date     2020-03-03
 ******************************************************************************/

#ifndef _OSR_TRNG_LL_H_
#define _OSR_TRNG_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RBG_CR, offset: 0x00 */
#define OSR_TRNG_RBG_CR_IRQEN_Pos                        (24U)
#define OSR_TRNG_RBG_CR_IRQEN_Msk                        (0x1U << OSR_TRNG_RBG_CR_IRQEN_Pos)
#define OSR_TRNG_RBG_CR_IRQEN                            OSR_TRNG_RBG_CR_IRQEN_Msk

#define OSR_TRNG_RBG_CR_TOIEN_Pos                        (19U)
#define OSR_TRNG_RBG_CR_TOIEN_Msk                        (0x1U << OSR_TRNG_RBG_CR_TOIEN_Pos)
#define OSR_TRNG_RBG_CR_TOIEN                            OSR_TRNG_RBG_CR_TOIEN_Msk

#define OSR_TRNG_RBG_CR_ERIEN_Pos                        (18U)
#define OSR_TRNG_RBG_CR_ERIEN_Msk                        (0x1U << OSR_TRNG_RBG_CR_ERIEN_Pos)
#define OSR_TRNG_RBG_CR_ERIEN                            OSR_TRNG_RBG_CR_ERIEN_Msk

#define OSR_TRNG_RBG_CR_DIEN_Pos                         (17U)
#define OSR_TRNG_RBG_CR_DIEN_Msk                         (0x1U << OSR_TRNG_RBG_CR_DIEN_Pos)
#define OSR_TRNG_RBG_CR_DIEN                             OSR_TRNG_RBG_CR_DIEN_Msk

#define OSR_TRNG_RBG_CR_HTEN_Pos                         (16U)
#define OSR_TRNG_RBG_CR_HTEN_Msk                         (0x1U << OSR_TRNG_RBG_CR_HTEN_Pos)
#define OSR_TRNG_RBG_CR_HTEN                             OSR_TRNG_RBG_CR_HTEN_Msk

#define OSR_TRNG_RBG_CR_ESSEL_Pos                        (1U)
#define OSR_TRNG_RBG_CR_ESSEL_Msk                        (0x1U << OSR_TRNG_RBG_CR_ESSEL_Pos)
#define OSR_TRNG_RBG_CR_ESSEL_RO                         (0x1U << OSR_TRNG_RBG_CR_ESSEL_Pos)
#define OSR_TRNG_RBG_CR_ESSEL_TERO                       (0x0U << OSR_TRNG_RBG_CR_ESSEL_Pos)

#define OSR_TRNG_RBG_CR_RBGEN_Pos                        (0U)
#define OSR_TRNG_RBG_CR_RBGEN_Msk                        (0x1U << OSR_TRNG_RBG_CR_RBGEN_Pos)
#define OSR_TRNG_RBG_CR_RBGEN                            OSR_TRNG_RBG_CR_RBGEN_Msk

/* RBG_RTCR, offset: 0x04 */
#define OSR_TRNG_RBG_RTCR_MSEL_Pos                       (0U)
#define OSR_TRNG_RBG_RTCR_MSEL_Msk                       (0x1U << OSR_TRNG_RBG_RTCR_MSEL_Pos)
#define OSR_TRNG_RBG_RTCR_MSEL_EN                        OSR_TRNG_RBG_RTCR_MSEL_Msk

/* RBG_SR, offset: 0x08 */
#define OSR_TRNG_RBG_SR_Pos                              (0U)
#define OSR_TRNG_RBG_SR_Msk                              (0xFU << OSR_TRNG_RBG_SR_Pos)
#define OSR_TRNG_RBG_SR_TTO                              (0x8U << OSR_TRNG_RBG_SR_Pos)
#define OSR_TRNG_RBG_SR_ERERR                            (0x4U << OSR_TRNG_RBG_SR_Pos)
#define OSR_TRNG_RBG_SR_DRDY                             (0x2U << OSR_TRNG_RBG_SR_Pos)
#define OSR_TRNG_RBG_SR_HTF                              (0x1U << OSR_TRNG_RBG_SR_Pos)
#define OSR_TRNG_RBG_SR_REGISTER                         (0xFU << OSR_TRNG_RBG_SR_Pos)

/* RBG_FIFO_CR, offset: 0x20 */
#define OSR_TRNG_RBG_FIFO_CR_TFTV_Pos                    (16U)
#define OSR_TRNG_RBG_FIFO_CR_TFTV_Msk                    (0x7FU << OSR_TRNG_RBG_FIFO_CR_TFTV_Pos)

#define OSR_TRNG_RBG_FIFO_CR_DFTV_Pos                    (0U)
#define OSR_TRNG_RBG_FIFO_CR_DFTV_Msk                    (0x7FU << OSR_TRNG_RBG_FIFO_CR_DFTV_Pos)

/* RBG_FIFO_SR, offset: 0x24 */
#define OSR_TRNG_RBG_FIFO_SR_TFE_Pos                     (24U)
#define OSR_TRNG_RBG_FIFO_SR_TFE_Msk                     (0x1U << OSR_TRNG_RBG_FIFO_SR_TFE_Pos)
#define OSR_TRNG_RBG_FIFO_SR_TFE                         OSR_TRNG_RBG_FIFO_SR_TFE_Msk

#define OSR_TRNG_RBG_FIFO_SR_TFCNT_Pos                   (16U)
#define OSR_TRNG_RBG_FIFO_SR_TFCNT_Msk                   (0xFFU << OSR_TRNG_RBG_FIFO_SR_TFCNT_Pos)


#define OSR_TRNG_RBG_FIFO_SR_DFE_Pos                     (8U)
#define OSR_TRNG_RBG_FIFO_SR_DFE_Msk                     (0x1U << OSR_TRNG_RBG_FIFO_SR_DFE_Pos)
#define OSR_TRNG_RBG_FIFO_SR_DFE_EN                      OSR_TRNG_RBG_FIFO_SR_DFE_Msk

#define OSR_TRNG_RBG_FIFO_SR_DFCNT_Pos                   (0U)
#define OSR_TRNG_RBG_FIFO_SR_DFCNT_Msk                   (0xFFU << OSR_TRNG_RBG_FIFO_SR_DFCNT_Pos)

/* RBG_RCLK_CR, offset: 0x60 */
#define OSR_TRNG_RBG_RCLK_CR_DRC_Pos                     (1U)
#define OSR_TRNG_RBG_RCLK_CR_DRC_Msk                     (0x7U << OSR_TRNG_RBG_FIFO_SR_TFE_Pos)

#define OSR_TRNG_RBG_RCLK_CR_DFCNT_Pos                   (0U)
#define OSR_TRNG_RBG_RCLK_CR_DFCNT_Msk                   (0x1U << OSR_TRNG_RBG_RCLK_CR_DFCNT_Pos)
#define OSR_TRNG_RBG_RCLK_CR_DFCNT_EN                    OSR_TRNG_RBG_RCLK_CR_DFCNT_Msk

/* RBG_HT_SR, offset: 0x70 */
#define OSR_TRNG_RBG_HT_SR_Pos                           (0U)
#define OSR_TRNG_RBG_HT_SR_Msk                           (0x3FFFFFFU << OSR_TRNG_RBG_HT_SR_Pos)
#define OSR_TRNG_RBG_HT_SR_RCTF                          (0x2000U << OSR_TRNG_RBG_HT_SR_Pos)
#define OSR_TRNG_RBG_HT_SR_APTF                          (0x1000U << OSR_TRNG_RBG_HT_SR_Pos)
#define OSR_TRNG_RBG_HT_SR_RCTBF                         (0x40U << OSR_TRNG_RBG_HT_SR_Pos)
#define OSR_TRNG_RBG_HT_SR_APTBBF                        (0x20U << OSR_TRNG_RBG_HT_SR_Pos)
#define OSR_TRNG_RBG_HT_SR_APTNBB                        (0x10U << OSR_TRNG_RBG_HT_SR_Pos)
#define OSR_TRNG_RBG_HT_SR_DBF                           (0x1U << OSR_TRNG_RBG_HT_SR_Pos)

/* RBG_RO_CR2, offset: 0x84 */
#define OSR_TRNG_RO_CR2_FSEL_Pos                         (0U)
#define OSR_TRNG_RO_CR2_FSEL_Msk                         (0x3U << OSR_TRNG_RO_CR2_FSEL_Pos)

/* RBG_TERO_CR, offset: 0x90 */
#define OSR_TRNG_TERO_CR_TEROEN_Pos                      (24U)
#define OSR_TRNG_TERO_CR_TEROEN_Msk                      (0xFFU << OSR_TRNG_TERO_CR_TEROEN_Pos)

#define OSR_TRNG_TERO_CR_OTV_Pos                         (16U)
#define OSR_TRNG_TERO_CR_OTV_Msk                         (0xFFU << OSR_TRNG_TERO_CR_OTV_Pos)

#define OSR_TRNG_TERO_CR_OM_Pos                          (11U)
#define OSR_TRNG_TERO_CR_OM_Msk                          (0x1U << OSR_TRNG_TERO_CR_OM_Pos)
#define OSR_TRNG_TERO_CR_OM_RANDOM_NUMBER                (0x0U << OSR_TRNG_TERO_CR_OM_Pos)
#define OSR_TRNG_TERO_CR_OM_OSCILLATION                  (0x1U << OSR_TRNG_TERO_CR_OM_Pos)

#define OSR_TRNG_TERO_CR_OSEL_Pos                        (8U)
#define OSR_TRNG_TERO_CR_OSEL_Msk                        (0x3U << OSR_TRNG_TERO_CR_OSEL_Pos)

#define OSR_TRNG_TERO_CR_TEROSEL_Pos                     (0U)
#define OSR_TRNG_TERO_CR_TEROSEL_Msk                     (0x3U << OSR_TRNG_TERO_CR_TEROSEL_Pos)

/* TRO_CR, offset: 0xA0 */
#define OSR_TRNG_TRO_CR_NUMSP_Pos                        (16U)
#define OSR_TRNG_TRO_CR_NUMSP_Msk                        (0xFFU << OSR_TRNG_TRO_CR_NUMSP_Pos)

#define OSR_TRNG_TRO_CR_ROEN_Pos                         (9U)
#define OSR_TRNG_TRO_CR_ROEN_Msk                         (0x3U << OSR_TRNG_TRO_CR_ROEN_Pos)

#define OSR_TRNG_TRO_CR_CEN_Pos                          (8U)
#define OSR_TRNG_TRO_CR_CEN_Msk                          (0x1U << OSR_TRNG_TERO_CR_OM_Pos)
#define OSR_TRNG_TRO_CR_CEN                              OSR_TRNG_TRO_CR_CEN_Msk

#define OSR_TRNG_TRO_CR_FSEL_Pos                         (0U)
#define OSR_TRNG_TRO_CR_FSEL_Msk                         (0x3U << OSR_TRNG_TRO_CR_FSEL_Pos)

/* TRO_SR, offset: 0xA4 */
#define OSR_TRNG_TRO_SR_Pos                              (0U)
#define OSR_TRNG_TRO_SR_Msk                              (0x1U << OSR_TRNG_TRO_SR_Pos)
#define OSR_TRNG_TRO_SR_DR                               OSR_TRNG_TRO_SR_Msk

/* TTERO_CR, offset: 0xB0 */
#define OSR_TRNG_TTERO_CR_COTV_Pos                       (24U)
#define OSR_TRNG_TTERO_CR_COTV_Msk                       (0xFFU << OSR_TRNG_TTERO_CR_COTV_Pos)

#define OSR_TRNG_TTERO_CR_TEROEN_Pos                     (9U)
#define OSR_TRNG_TTERO_CR_TEROEN_Msk                     (0x3U << OSR_TRNG_TTERO_CR_TEROEN_Pos)

#define OSR_TRNG_TTERO_CR_TEN_Pos                        (8U)
#define OSR_TRNG_TTERO_CR_TEN_Msk                        (0x1U << OSR_TRNG_TTERO_CR_TEN_Pos)
#define OSR_TRNG_TTERO_CR_TEN_EN                         OSR_TRNG_TTERO_CR_TEN_Msk

#define OSR_TRNG_TTERO_CR_MS_Pos                         (1U)
#define OSR_TRNG_TTERO_CR_MS_Msk                         (0x1U << OSR_TRNG_TTERO_CR_MS_Pos)
#define OSR_TRNG_TTERO_CR_MS_RANDOM_NUMBER               (0x0U << OSR_TRNG_TTERO_CR_MS_Pos)
#define OSR_TRNG_TTERO_CR_MS_OSC                         (0x1U << OSR_TRNG_TTERO_CR_MS_Pos)

#define OSR_TRNG_TTERO_CR_EN_Pos                         (0U)
#define OSR_TRNG_TTERO_CR_EN_Msk                         (0x1U << OSR_TRNG_TTERO_CR_EN_Pos)
#define OSR_TRNG_TTERO_CR_EN                             OSR_TRNG_TTERO_CR_EN_Msk

/* TTERO_CR2, offset: 0xB4 */
#define OSR_TRNG_TTERO_CR2_LBTERO0_Pos                   (16U)
#define OSR_TRNG_TTERO_CR2_LBTERO0_Msk                   (0xFFU << OSR_TRNG_TTERO_CR2_LBTERO0_Pos)

#define OSR_TRNG_TTERO_CR2_LBTERO1_Pos                   (0U)
#define OSR_TRNG_TTERO_CR2_LBTERO1_Msk                   (0xFFU << OSR_TRNG_TTERO_CR2_LBTERO1_Pos)

/* TTERO_CR3, offset: 0xB8 */
#define OSR_TRNG_TTERO_CR3_UBTERO0_Pos                   (16U)
#define OSR_TRNG_TTERO_CR3_UBTERO0_Msk                   (0xFFU << OSR_TRNG_TTERO_CR3_UBTERO0_Pos)

#define OSR_TRNG_TTERO_CR3_UBTERO1_Pos                   (0U)
#define OSR_TRNG_TTERO_CR3_UBTERO1_Msk                   (0xFFU << OSR_TRNG_TTERO_CR3_UBTERO1_Pos)

/* TTERO_SR, offset: 0xD0 */
#define OSR_TRNG_TTERO_SR_Pos                            (0U)
#define OSR_TRNG_TTERO_SR_Msk                            (0x1U << OSR_TRNG_TTERO_SR_Pos)
#define OSR_TRNG_TTERO_SR_DR                             OSR_TRNG_TTERO_SR_Msk

typedef struct {
    __IOM uint32_t  RBG_CR;               /* Offset: 0x000 (W/R)  RBG control register */
    __IOM uint32_t  RBG_RTCR;             /* Offset: 0x004 (W/R)  RBG mode selection register */
    __IOM uint32_t  RBG_SR;               /* Offset: 0x008 (W/R)  RBG status register */
    __IM  uint32_t  RBG_DR;               /* Offset: 0x00c ( /R)  Random data register */
    uint32_t  RESERVED[4];
    __IOM uint32_t  RBG_FIFO_CR;          /* Offset: 0x020 (W/R)  FIFO control register */
    __IM  uint32_t  RBG_FIFO_SR;          /* Offset: 0x024 ( /R)  FIFO status register */
    uint32_t  RESERVED1[14];
    __IOM uint32_t  RBG_RCLK_CR;          /* Offset: 0x060 (W/R)  Random clock control register*/
    uint32_t  RESERVED2[3];
    __IM  uint32_t  RBG_HT_SR;            /* Offset: 0x070 (/R)   Health test status register */
    uint32_t  RESERVED3[3];
    __IOM uint32_t  RO_CR;                /* Offset: 0x080 (W/R)  RO control register */
    __IOM uint32_t  RO_CR2;               /* Offset: 0x084 (W/R)  RO RNG control register 2 */
    uint32_t  RESERVED4[2];
    __IOM uint32_t  TERO_CR;              /* Offset: 0x090 (W/R)  TERO control register */
    __IOM uint32_t  TERO_WD_CR;           /* Offset: 0x094 (W/R)  TERO RNG watchdog control register */
    uint32_t  RESERVED5[2];
    __IOM uint32_t  TRO_CR;               /* Offset: 0x0A0 (W/R)  Test RO control register */
    __IM  uint32_t  TRO_SR;               /* Offset: 0x0A4 ( /R)  Test RO status register */
    __IM  uint32_t  TRO_DR;               /* Offset: 0x0A8 ( /R)  Test RO data register */
    uint32_t  RESERVED6;
    __IOM uint32_t  TTERO_CR;             /* Offset: 0x0B0 ( /R)  Test TERO control register */
    __IOM uint32_t  TTERO_CR2;            /* Offset: 0x0B4 (W/R)  Test TERO control register 2 */
    __IOM uint32_t  TTERO_CR3;            /* Offset: 0x0B8 (W/R)  Test TERO control register 3 */
    uint32_t  RESERVED7;
    __IOM uint32_t  TTERO_CR4;            /* Offset: 0x0C0 (W/R)  Test TERO control register 4 */
    __IOM uint32_t  TTERO_CR5;            /* Offset: 0x0C4 (W/R)  Test TERO control register 5 */
    __IOM uint32_t  TTERO_CR6;            /* Offset: 0x0C8 (W/R)  Test TERO control register 6 */
    __IOM uint32_t  TTERO_CR7;            /* Offset: 0x0CC (W/R)  Test TERO control register 7 */
    __IM  uint32_t  TTERO_SR;             /* Offset: 0x0D0 ( /R)  Test TERO status register */
    __IM  uint32_t  TTERO_DR;             /* Offset: 0x0D4 ( /R)  Test TERO data register */
    uint32_t  RESERVED8[2];
    __IM  uint32_t  TTERO_SR1;            /* Offset: 0x0E0 ( /R)  Test TERO status register 1 */
    __IM  uint32_t  TTERO_SR2;            /* Offset: 0x0E4 ( /R)  Test TERO status register 2 */
    __IM  uint32_t  TTERO_SR3;            /* Offset: 0x0E8 ( /R)  Test TERO status register 3 */
    __IM  uint32_t  TTERO_SR4;            /* Offset: 0x0EC ( /R)  Test TERO status register 4 */
} osr_trng_regs_t;

static inline void csr_trng_enable_global_irq(osr_trng_regs_t *trng)
{
    trng->RBG_CR |= OSR_TRNG_RBG_CR_IRQEN;
}

static inline void csr_trng_disable_global_irq(osr_trng_regs_t *trng)
{
    trng->RBG_CR &= ~OSR_TRNG_RBG_CR_IRQEN;
}

static inline void csr_trng_enable_timeout_irq(osr_trng_regs_t *trng)
{
    trng->RBG_CR |= OSR_TRNG_RBG_CR_TOIEN;
}

static inline void csr_trng_disable_timeout_irq(osr_trng_regs_t *trng)
{
    trng->RBG_CR &= ~OSR_TRNG_RBG_CR_TOIEN;
}

static inline void csr_trng_enable_empty_read_irq(osr_trng_regs_t *trng)
{
    trng->RBG_CR |= OSR_TRNG_RBG_CR_ERIEN;
}

static inline void csr_trng_disable_empty_read_irq(osr_trng_regs_t *trng)
{
    trng->RBG_CR &= ~OSR_TRNG_RBG_CR_ERIEN;
}

static inline void csr_trng_enable_data_irq(osr_trng_regs_t *trng)
{
    trng->RBG_CR |= OSR_TRNG_RBG_CR_DIEN;
}

static inline void csr_trng_disable_data_irq(osr_trng_regs_t *trng)
{
    trng->RBG_CR &= ~OSR_TRNG_RBG_CR_DIEN;
}

static inline void csr_trng_enable_health_test_irq(osr_trng_regs_t *trng)
{
    trng->RBG_CR |= OSR_TRNG_RBG_CR_HTEN;
}

static inline void csr_trng_disable_health_test_irq(osr_trng_regs_t *trng)
{
    trng->RBG_CR &= ~OSR_TRNG_RBG_CR_HTEN;
}

static inline void csr_trng_set_ro_entropy_source(osr_trng_regs_t *trng)
{
    trng->RBG_CR |= OSR_TRNG_RBG_CR_ESSEL_RO;
}


static inline void csr_trng_set_tero_entropy_source(osr_trng_regs_t *trng)
{
    trng->RBG_CR &= ~OSR_TRNG_RBG_CR_ESSEL_Msk;
    trng->RBG_CR |= OSR_TRNG_RBG_CR_ESSEL_TERO;
}

static inline void csr_trng_enable_rbg(osr_trng_regs_t *trng)
{
    trng->RBG_CR |= OSR_TRNG_RBG_CR_RBGEN;
}

static inline void csr_trng_disable_rbg(osr_trng_regs_t *trng)
{
    trng->RBG_CR &= ~OSR_TRNG_RBG_CR_RBGEN;
}

static inline void csr_trng_enable_post_processing(osr_trng_regs_t *trng)
{
    trng->RBG_RTCR |= OSR_TRNG_RBG_RTCR_MSEL_EN;
}

static inline void csr_trng_disable_post_processing(osr_trng_regs_t *trng)
{
    trng->RBG_RTCR &= ~OSR_TRNG_RBG_RTCR_MSEL_EN;
}

static inline uint32_t csr_trng_get_status(osr_trng_regs_t *trng)
{
    return trng->RBG_SR & OSR_TRNG_RBG_SR_Msk;
}

static inline void csr_trng_clear_all_status(osr_trng_regs_t *trng)
{
    trng->RBG_SR = 0U;
}

static inline void csr_trng_register_sr(osr_trng_regs_t *trng)
{
    trng->RBG_SR = OSR_TRNG_RBG_SR_REGISTER;
}

static inline uint32_t csr_trng_get_random_data(osr_trng_regs_t *trng)
{
    return trng->RBG_DR;
}

static inline void csr_trng_config_trng_fifo_threshold(osr_trng_regs_t *trng, uint32_t threshold)
{
    trng->RBG_FIFO_CR &= ~OSR_TRNG_RBG_FIFO_CR_TFTV_Msk;
    trng->RBG_FIFO_CR |= (threshold << OSR_TRNG_RBG_FIFO_CR_TFTV_Pos);
}

static inline void csr_trng_config_drng_fifo_threshold(osr_trng_regs_t *trng, uint32_t threshold)
{
    trng->RBG_FIFO_CR &= OSR_TRNG_RBG_FIFO_CR_DFTV_Msk;
    trng->RBG_FIFO_CR |= (threshold << OSR_TRNG_RBG_FIFO_CR_DFTV_Pos);
}

static inline uint32_t csr_trng_get_fifo_status(osr_trng_regs_t *trng)
{
    return trng->RBG_FIFO_SR;
}

static inline uint32_t csr_trng_get_trbg_fifo_count(osr_trng_regs_t *trng)
{
    return (trng->RBG_FIFO_SR & OSR_TRNG_RBG_FIFO_SR_TFCNT_Msk) >> OSR_TRNG_RBG_FIFO_SR_TFCNT_Pos ;
}

static inline uint32_t csr_trng_get_trbg_fifo_tfestate(osr_trng_regs_t *trng)
{
    return trng->RBG_FIFO_SR & OSR_TRNG_RBG_FIFO_SR_TFE_Msk;
}

static inline uint32_t csr_trng_get_drbg_fifo_count(osr_trng_regs_t *trng)
{
    return (trng->RBG_FIFO_SR & OSR_TRNG_RBG_FIFO_SR_DFCNT_Msk) >> OSR_TRNG_RBG_FIFO_SR_DFCNT_Pos ;
}

static inline void csr_trng_config_random_clock_duty_ratio(osr_trng_regs_t *trng, uint32_t idx)
{
    trng->RBG_RCLK_CR &= ~OSR_TRNG_RBG_RCLK_CR_DRC_Msk;
    trng->RBG_RCLK_CR |= (idx << OSR_TRNG_RBG_RCLK_CR_DRC_Pos);
}

static inline void csr_trng_enable_random_clock(osr_trng_regs_t *trng)
{
    trng->RBG_RCLK_CR |= OSR_TRNG_RBG_RCLK_CR_DFCNT_EN;
}

static inline void csr_trng_disable_random_clock(osr_trng_regs_t *trng)
{
    trng->RBG_RCLK_CR &= ~OSR_TRNG_RBG_RCLK_CR_DFCNT_EN;
}

static inline uint32_t csr_trng_get_health_test_status(osr_trng_regs_t *trng)
{
    return trng->RBG_HT_SR;
}

static inline void csr_trng_config_ro_source(osr_trng_regs_t *trng, uint32_t msk)
{
    trng->RO_CR = msk;
}

static inline uint32_t csr_trng_get_ro_source(osr_trng_regs_t *trng)
{
    return trng->RO_CR;
}

static inline void csr_trng_config_ro_freq(osr_trng_regs_t *trng, uint32_t sel)
{
    trng->RO_CR2 &= ~OSR_TRNG_RO_CR2_FSEL_Msk;
    trng->RO_CR2 |= (sel << OSR_TRNG_RO_CR2_FSEL_Pos);
}

static inline void csr_trng_config_tero_source(osr_trng_regs_t *trng, uint32_t msk)
{
    trng->TERO_CR &= ~OSR_TRNG_TERO_CR_TEROEN_Msk;
    trng->TERO_CR |= (msk << OSR_TRNG_TERO_CR_TEROEN_Pos);
}

static inline uint32_t csr_trng_get_tero_source(osr_trng_regs_t *trng)
{
    return trng->TERO_CR & OSR_TRNG_TERO_CR_TEROEN_Msk;
}

static inline void csr_trng_config_otv(osr_trng_regs_t *trng, uint32_t otv)
{
    trng->TERO_CR &= ~OSR_TRNG_TERO_CR_OTV_Msk;
    trng->TERO_CR |= (otv << OSR_TRNG_TERO_CR_OTV_Pos);
}

static inline void csr_trng_set_om_random_number(osr_trng_regs_t *trng)
{
    trng->TERO_CR &= ~OSR_TRNG_TERO_CR_OM_Msk;
    trng->TERO_CR |= OSR_TRNG_TERO_CR_OM_RANDOM_NUMBER;
}

static inline void csr_trng_set_om_random_oscillation(osr_trng_regs_t *trng)
{
    trng->TERO_CR &= ~OSR_TRNG_TERO_CR_OM_Msk;
    trng->TERO_CR |= OSR_TRNG_TERO_CR_OM_RANDOM_NUMBER;
}

static inline void csr_trng_config_osel(osr_trng_regs_t *trng, uint32_t sel)
{
    trng->TERO_CR &= ~OSR_TRNG_TERO_CR_OSEL_Msk;
    trng->TERO_CR |= (sel << OSR_TRNG_TERO_CR_OSEL_Pos);
}

static inline void csr_trng_config_tero_sel(osr_trng_regs_t *trng, uint32_t sel)
{
    trng->TERO_CR &= ~OSR_TRNG_TERO_CR_TEROSEL_Msk;
    trng->TERO_CR |= (sel << OSR_TRNG_TERO_CR_OSEL_Pos);
}

static inline void csr_trng_config_watchdog_threshold(osr_trng_regs_t *trng, uint32_t threshold)
{
    trng->TERO_WD_CR = threshold;
}

static inline void csr_trng_config_test_ro_sample_point(osr_trng_regs_t *trng, uint32_t value)
{
    trng->TRO_CR &= ~OSR_TRNG_TRO_CR_NUMSP_Msk;
    trng->TRO_CR |= (value << OSR_TRNG_TRO_CR_NUMSP_Pos);
}

static inline void csr_trng_enable_test_ro(osr_trng_regs_t *trng, uint32_t idx)
{
    trng->TRO_CR &= ~OSR_TRNG_TRO_CR_ROEN_Msk;
    trng->TRO_CR |= (idx << OSR_TRNG_TRO_CR_ROEN_Pos);
}

static inline void csr_trng_disable_test_ro(osr_trng_regs_t *trng, uint32_t idx)
{
    trng->TRO_CR &= ~(idx << OSR_TRNG_TRO_CR_ROEN_Pos);
}

static inline void csr_trng_enable_test_ro_clock(osr_trng_regs_t *trng)
{
    trng->TRO_CR |= OSR_TRNG_TRO_CR_CEN;
}

static inline void csr_trng_disable_test_ro_clock(osr_trng_regs_t *trng)
{
    trng->TRO_CR &= ~OSR_TRNG_TRO_CR_CEN;
}

static inline void csr_trng_config_test_ro_freq(osr_trng_regs_t *trng, uint32_t sel)
{
    trng->TRO_CR &= ~OSR_TRNG_TRO_CR_FSEL_Msk;
    trng->TRO_CR |= (sel << OSR_TRNG_TRO_CR_FSEL_Pos);
}

static inline uint32_t csr_trng_get_test_ro_status(osr_trng_regs_t *trng)
{
    return trng->TRO_SR;
}

static inline uint32_t csr_trng_get_test_random_data(osr_trng_regs_t *trng)
{
    return trng->TRO_DR;
}

static inline void csr_trng_config_test_tero_threshold_value(osr_trng_regs_t *trng, uint32_t value)
{
    trng->TTERO_CR &= ~OSR_TRNG_TTERO_CR_COTV_Msk;
    trng->TTERO_CR |= (value << OSR_TRNG_TTERO_CR_COTV_Pos);
}

static inline void csr_trng_enable_test_tero(osr_trng_regs_t *trng, uint32_t idx)
{
    trng->TTERO_CR &= ~OSR_TRNG_TTERO_CR_TEROEN_Msk;
    trng->TTERO_CR |= (idx << OSR_TRNG_TTERO_CR_TEROEN_Pos);
}

static inline void csr_trng_disable_test_tero(osr_trng_regs_t *trng, uint32_t idx)
{
    trng->TTERO_CR &= ~(idx << OSR_TRNG_TTERO_CR_TEROEN_Pos);
}

static inline void csr_trng_enable_test_tero_threshold_ctrl(osr_trng_regs_t *trng)
{
    trng->TTERO_CR |= OSR_TRNG_TTERO_CR_TEN_EN;
}

static inline void csr_trng_disable_test_tero_threshold_ctrl(osr_trng_regs_t *trng)
{
    trng->TTERO_CR &= ~OSR_TRNG_TTERO_CR_TEN_EN;
}

static inline void csr_trng_set_test_tero_random_number(osr_trng_regs_t *trng)
{
    trng->TTERO_CR &= ~OSR_TRNG_TTERO_CR_MS_Msk;
    trng->TTERO_CR |= OSR_TRNG_TTERO_CR_MS_RANDOM_NUMBER;
}

static inline void csr_trng_set_test_tero_oscillation(osr_trng_regs_t *trng)
{
    trng->TTERO_CR &= ~OSR_TRNG_TTERO_CR_MS_Msk;
    trng->TTERO_CR |= OSR_TRNG_TTERO_CR_MS_OSC;
}

static inline void csr_trng_enable_test_tero_state_mechine(osr_trng_regs_t *trng)
{
    trng->TTERO_CR |= OSR_TRNG_TTERO_CR_EN;
}

static inline void csr_trng_disable_test_tero_state_mechine(osr_trng_regs_t *trng)
{
    trng->TTERO_CR &= ~OSR_TRNG_TTERO_CR_EN;
}

static inline void csr_trng_config_test_tero0_bound(osr_trng_regs_t *trng, uint32_t lower, uint32_t upper)
{
    trng->TTERO_CR2 &= ~OSR_TRNG_TTERO_CR2_LBTERO0_Msk;
    trng->TTERO_CR2 |= (lower << OSR_TRNG_TTERO_CR2_LBTERO0_Pos);
    trng->TTERO_CR3 &= ~OSR_TRNG_TTERO_CR3_UBTERO0_Msk;
    trng->TTERO_CR3 |= (upper << OSR_TRNG_TTERO_CR3_UBTERO0_Pos);
}

static inline void csr_trng_config_test_tero1_bound(osr_trng_regs_t *trng, uint32_t lower, uint32_t upper)
{
    trng->TTERO_CR2 &= ~OSR_TRNG_TTERO_CR2_LBTERO0_Msk;
    trng->TTERO_CR2 |= (lower << OSR_TRNG_TTERO_CR2_LBTERO0_Pos);
    trng->TTERO_CR3 &= ~OSR_TRNG_TTERO_CR3_UBTERO1_Msk;
    trng->TTERO_CR3 |= (upper << OSR_TRNG_TTERO_CR3_UBTERO1_Pos);
}

static inline void csr_trng_config_test_tero0_seed_msb(osr_trng_regs_t *trng, uint32_t value)
{
    trng->TTERO_CR4 = value;
}

static inline void csr_trng_config_test_tero0_seed_lsb(osr_trng_regs_t *trng, uint32_t value)
{
    trng->TTERO_CR5 = value;
}

static inline void csr_trng_config_test_tero1_seed_msb(osr_trng_regs_t *trng, uint32_t value)
{
    trng->TTERO_CR6 = value;
}

static inline void csr_trng_config_test_tero1_seed_lsb(osr_trng_regs_t *trng, uint32_t value)
{
    trng->TTERO_CR7 = value;
}

#endif  /* _OSR_TRNG_LL_H_*/
