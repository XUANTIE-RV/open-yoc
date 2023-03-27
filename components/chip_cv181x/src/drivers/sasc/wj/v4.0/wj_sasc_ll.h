/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_sasc_ll.h
 * @brief    header file for sasc ll driver
 * @version  V1.0
 * @date     10. May 2020
 ******************************************************************************/

#ifndef _WJ_SASC_LL_H_
#define _WJ_SASC_LL_H_

#include <stdio.h>
#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SASC register bit definitions
 */

/* QSPIC_CAR, offset: 0x00 */
#define WJ_SASC_QSPIC_CAR_Pos                    (0U)
#define WJ_SASC_QSPIC_CAR_Msk                    (0x3U << WJ_SASC_QSPIC_CAR_Pos)
#define WJ_SASC_QSPIC_CAR_UNSEC_UNSUPER          (0x0U << WJ_SASC_QSPIC_CAR_Pos)
#define WJ_SASC_QSPIC_CAR_UNSEC_SUPER            (0x1U << WJ_SASC_QSPIC_CAR_Pos)
#define WJ_SASC_QSPIC_CAR_SEC_UNSUPER            (0x2U << WJ_SASC_QSPIC_CAR_Pos)
#define WJ_SASC_QSPIC_CAR_SEC_SUPER              (0x3U << WJ_SASC_QSPIC_CAR_Pos)

/* RM_CAR, offset: 0x00 */
#define WJ_SASC_RM_CAR_Pos                       (0U)
#define WJ_SASC_RM_CAR_Msk                       (0x3U << WJ_SASC_RM_CAR_Pos)
#define WJ_SASC_RM_CAR_UNSEC_UNSUPER             (0x0U << WJ_SASC_RM_CAR_Pos)
#define WJ_SASC_RM_CAR_UNSEC_SUPER               (0x1U << WJ_SASC_RM_CAR_Pos)
#define WJ_SASC_RM_CAR_SEC_UNSUPER               (0x2U << WJ_SASC_RM_CAR_Pos)
#define WJ_SASC_RM_CAR_SEC_SUPER                 (0x3U << WJ_SASC_RM_CAR_Pos)

/* QSPIC_CR, offset: 0x04 */
#define WJ_SASC_QSPIC_CR_Pos                     (0U)
#define WJ_SASC_QSPIC_CR_Msk                     (0x1U << WJ_SASC_QSPIC_CR_Pos)
#define WJ_SASC_QSPIC_CR_EN                      WJ_SASC_QSPIC_CR_Msk

/* RM_CR, offset: 0x04 */
#define WJ_SASC_RM_CR_Pos                        (0U)
#define WJ_SASC_RM_CR_Msk                        (0x1U << WJ_SASC_RM_CR_Pos)
#define WJ_SASC_RM_CR_EN                         WJ_SASC_RM_CR_Msk

/* QSPIC_APx(x=0/1/2), offset: 0x08/0x10/0x18 */
#define WJ_SASC_QSPIC_AP_Pos                     (0U)
#define WJ_SASC_QSPIC_AP_Msk                     (0x3U << WJ_SASC_QSPIC_AP_Pos)
#define WJ_SASC_QSPIC_AP_RW                      (0x0U << WJ_SASC_QSPIC_AP_Pos)
#define WJ_SASC_QSPIC_AP_RO                      (0x1U << WJ_SASC_QSPIC_AP_Pos)
#define WJ_SASC_QSPIC_AP_WO                      (0x2U << WJ_SASC_QSPIC_AP_Pos)
#define WJ_SASC_QSPIC_AP_DENY                    (0x3U << WJ_SASC_QSPIC_AP_Pos)

/* RM_APx(x=0/1/2), offset: 0x08/0x10/0x18 */
#define WJ_SASC_RM_AP_Pos                        (0U)
#define WJ_SASC_RM_AP_Msk                        (0x3U << WJ_SASC_RM_AP_Pos)
#define WJ_SASC_RM_AP_RW                         (0x0U << WJ_SASC_RM_AP_Pos)
#define WJ_SASC_RM_AP_RO                         (0x1U << WJ_SASC_RM_AP_Pos)
#define WJ_SASC_RM_AP_WO                         (0x2U << WJ_SASC_RM_AP_Pos)
#define WJ_SASC_RM_AP_DENY                       (0x3U << WJ_SASC_RM_AP_Pos)

/* QSPIC_CDx(x=0/1/2), offset: 0x0C/0x14/0x1c */
#define WJ_SASC_QSPIC_CD_Pos                     (0U)
#define WJ_SASC_QSPIC_CD_Msk                     (0x3U << WJ_SASC_QSPIC_CD_Pos)
#define WJ_SASC_QSPIC_CD_DI                      (0x0U << WJ_SASC_QSPIC_CD_Pos)
#define WJ_SASC_QSPIC_CD_DO                      (0x1U << WJ_SASC_QSPIC_CD_Pos)
#define WJ_SASC_QSPIC_CD_IO                      (0x2U << WJ_SASC_QSPIC_CD_Pos)
#define WJ_SASC_QSPIC_CD_DENY                    (0x3U << WJ_SASC_QSPIC_CD_Pos)

/* RM_CDx(x=0/1/2), offset: 0x0C/0x14/0x1c */
#define WJ_SASC_RM_CD_Pos                        (0U)
#define WJ_SASC_RM_CD_Msk                        (0x3U << WJ_SASC_RM_CD_Pos)
#define WJ_SASC_RM_CD_DI                         (0x0U << WJ_SASC_RM_CD_Pos)
#define WJ_SASC_RM_CD_DO                         (0x1U << WJ_SASC_RM_CD_Pos)
#define WJ_SASC_RM_CD_IO                         (0x2U << WJ_SASC_RM_CD_Pos)
#define WJ_SASC_RM_CD_DENY                       (0x3U << WJ_SASC_RM_CD_Pos)

/* QSPIC_RGx(x=0/1/2/3/4/5/6/7), offset: 0x20/0x24/0x28/0x2C/0x30/0x34/0x38/0x3C */
#define WJ_SASC_QSPIC_REG_RG_BA_Pos              (8U)
#define WJ_SASC_QSPIC_REG_RG_BA_Msk              (0x7FFU << WJ_SASC_QSPIC_REG_RG_BA_Pos)

#define WJ_SASC_QSPIC_REG_RG_SZ_Pos              (0U)
#define WJ_SASC_QSPIC_REG_RG_SZ_Msk              (0xFU << WJ_SASC_QSPIC_REG_RG_SZ_Pos)

/* RM_RGx(x=0/1/2/3/4/5/6/7), offset: 0x20/0x24/0x28/0x2C/0x30/0x34/0x38/0x3C */
#define WJ_SASC_RM_REG_RG_BA_Pos                 (8U)
#define WJ_SASC_RM_REG_RG_BA_Msk                 (0x7FFU << WJ_SASC_RM_REG_RG_BA_Pos)

#define WJ_SASC_RM_REG_RG_SZ_Pos                 (0U)
#define WJ_SASC_RM_REG_RG_SZ_Msk                 (0x1FU << WJ_SASC_RM_REG_RG_SZ_Pos)

typedef struct {
    __IOM uint32_t QSPIC_CAR;                  /* Offset: 0x000 (W/R)  flash configure attributes register */
    __IOM uint32_t QSPIC_CR;                   /* Offset: 0x004 (W/R)  flash control register */
    __IOM uint32_t QSPIC_AP0;                  /* Offset: 0x008 (W/R)  flash ap configuration register for unsecurity user */
    __IOM uint32_t QSPIC_CD0;                  /* Offset: 0x00c (W/R)  flash cd configuration register for unsecurity user */
    __IOM uint32_t QSPIC_AP1;                  /* Offset: 0x010 (W/R)  flash ap configuration register for unsecurity super */
    __IOM uint32_t QSPIC_CD1;                  /* Offset: 0x014 (W/R)  flash cd configuration register for unsecurity super */
    __IOM uint32_t QSPIC_AP2;                  /* Offset: 0x018 (W/R)  flash ap configuration register for security user */
    __IOM uint32_t QSPIC_CD2;                  /* Offset: 0x01c (W/R)  flash cd configuration register for security user */
    __IOM uint32_t QSPIC_RG[8];                /* Offset: 0x020 (W/R)  flash base address and size register */
} wj_sasc_qspic_regs_t;

typedef struct {
    __IOM uint32_t RM_CAR;                   /* Offset: 0x000 (W/R)  sram configure attributes register */
    __IOM uint32_t RM_CR;                    /* Offset: 0x004 (W/R)  sram control register */
    __IOM uint32_t RM_AP0;                   /* Offset: 0x008 (W/R)  sram ap configuration register for unsecurity user */
    __IOM uint32_t RM_CD0;                   /* Offset: 0x00c (W/R)  sram cd configuration register for unsecurity user */
    __IOM uint32_t RM_AP1;                   /* Offset: 0x010 (W/R)  sram ap configuration register for unsecurity super */
    __IOM uint32_t RM_CD1;                   /* Offset: 0x014 (W/R)  sram cd configuration register for unsecurity super */
    __IOM uint32_t RM_AP2;                   /* Offset: 0x018 (W/R)  sram ap configuration register for security user */
    __IOM uint32_t RM_CD2;                   /* Offset: 0x01c (W/R)  sram cd configuration register for security user */
    __IOM uint32_t RM_RG[8];                 /* Offset: 0x020 (W/R)  sram base address and size register */

} wj_sasc_sram_regs_t;

static inline void wj_sasc_qspic_set_unsec_unsuper_mode(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CAR &= ~(WJ_SASC_QSPIC_CAR_Msk << (region << 1U));
    sasc_base->QSPIC_CAR |= (WJ_SASC_QSPIC_CAR_UNSEC_UNSUPER << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_super_mode(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CAR &= ~(WJ_SASC_QSPIC_CAR_Msk << (region << 1U));
    sasc_base->QSPIC_CAR |= (WJ_SASC_QSPIC_CAR_UNSEC_SUPER << (region << 1U));
}

static inline void wj_sasc_qspic_set_sec_unsuper_mode(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CAR &= ~(WJ_SASC_QSPIC_CAR_Msk << (region << 1U));
    sasc_base->QSPIC_CAR |= (WJ_SASC_QSPIC_CAR_SEC_UNSUPER << (region << 1U));
}

static inline void wj_sasc_qspic_set_sec_super_mode(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CAR &= ~(WJ_SASC_QSPIC_CAR_Msk << (region << 1U));
    sasc_base->QSPIC_CAR |= (WJ_SASC_QSPIC_CAR_SEC_SUPER << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_unsuper_mode(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CAR &= ~(WJ_SASC_RM_CAR_Msk << (region << 1U));
    sasc_base->RM_CAR |= (WJ_SASC_RM_CAR_UNSEC_UNSUPER << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_super_mode(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CAR &= ~(WJ_SASC_RM_CAR_Msk << (region << 1U));
    sasc_base->RM_CAR |= (WJ_SASC_RM_CAR_UNSEC_SUPER << (region << 1U));
}

static inline void wj_sasc_sram_set_sec_unsuper_mode(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CAR &= ~(WJ_SASC_QSPIC_CAR_Msk << (region << 1U));
    sasc_base->RM_CAR |= (WJ_SASC_RM_CAR_SEC_UNSUPER << (region << 1U));
}

static inline void wj_sasc_sram_set_sec_super_mode(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CAR &= ~(WJ_SASC_QSPIC_CAR_Msk << (region << 1U));
    sasc_base->RM_CAR |= (WJ_SASC_RM_CAR_SEC_SUPER << (region << 1U));
}

static inline void wj_sasc_qspic_enable(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CR |= (WJ_SASC_QSPIC_CR_EN << region);
}

static inline void wj_sasc_qspic_disable(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CR &= ~(WJ_SASC_QSPIC_CR_EN << region);
}

static inline void wj_sasc_sram_enable(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CR |= (WJ_SASC_RM_CR_EN << region);
}

static inline void wj_sasc_sram_disable(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CR &= ~(WJ_SASC_RM_CR_EN << region);
}

static inline void wj_sasc_qspic_set_unsec_unsuper_ap_rw(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_AP0 &= ~(WJ_SASC_QSPIC_AP_Msk << (region << 1U));
    sasc_base->QSPIC_AP0 |= (WJ_SASC_QSPIC_AP_RW << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_unsuper_ap_ro(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_AP0 &= ~(WJ_SASC_QSPIC_AP_Msk << (region << 1U));
    sasc_base->QSPIC_AP0 |= (WJ_SASC_QSPIC_AP_RO << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_unsuper_ap_wo(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_AP0 &= ~(WJ_SASC_QSPIC_AP_Msk << (region << 1U));
    sasc_base->QSPIC_AP0 |= (WJ_SASC_QSPIC_AP_WO << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_unsuper_ap_deny(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_AP0 &= ~(WJ_SASC_QSPIC_AP_Msk << (region << 1U));
    sasc_base->QSPIC_AP0 |= (WJ_SASC_QSPIC_AP_DENY << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_unsuper_ap_rw(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_AP0 &= ~(WJ_SASC_RM_AP_Msk << (region << 1U));
    sasc_base->RM_AP0 |= (WJ_SASC_RM_AP_RW << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_unsuper_ap_ro(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_AP0 &= ~(WJ_SASC_RM_AP_Msk << (region << 1U));
    sasc_base->RM_AP0 |= (WJ_SASC_RM_AP_RO << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_unsuper_ap_wo(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_AP0 &= ~(WJ_SASC_RM_AP_Msk << (region << 1U));
    sasc_base->RM_AP0 |= (WJ_SASC_RM_AP_WO << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_unsuper_ap_deny(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_AP0 &= ~(WJ_SASC_RM_AP_Msk << (region << 1U));
    sasc_base->RM_AP0 |= (WJ_SASC_RM_AP_DENY << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_super_ap_rw(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_AP1 &= ~(WJ_SASC_QSPIC_AP_Msk << (region << 1U));
    sasc_base->QSPIC_AP1 |= (WJ_SASC_QSPIC_AP_RW << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_super_ap_ro(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_AP1 &= ~(WJ_SASC_QSPIC_AP_Msk << (region << 1U));
    sasc_base->QSPIC_AP1 |= (WJ_SASC_QSPIC_AP_RO << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_super_ap_wo(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_AP1 &= ~(WJ_SASC_QSPIC_AP_Msk << (region << 1U));
    sasc_base->QSPIC_AP1 |= (WJ_SASC_QSPIC_AP_WO << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_super_ap_deny(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_AP1 &= ~(WJ_SASC_QSPIC_AP_Msk << (region << 1U));
    sasc_base->QSPIC_AP1 |= (WJ_SASC_QSPIC_AP_DENY << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_super_ap_rw(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_AP1 &= ~(WJ_SASC_RM_AP_Msk << (region << 1U));
    sasc_base->RM_AP1 |= (WJ_SASC_RM_AP_RW << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_super_ap_ro(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_AP1 &= ~(WJ_SASC_RM_AP_Msk << (region << 1U));
    sasc_base->RM_AP1 |= (WJ_SASC_RM_AP_RO << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_super_ap_wo(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_AP1 &= ~(WJ_SASC_RM_AP_Msk << (region << 1U));
    sasc_base->RM_AP1 |= (WJ_SASC_RM_AP_WO << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_super_ap_deny(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_AP1 &= ~(WJ_SASC_RM_AP_Msk << (region << 1U));
    sasc_base->RM_AP1 |= (WJ_SASC_RM_AP_DENY << (region << 1U));
}

static inline void wj_sasc_qspic_set_sec_unsuper_ap_rw(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_AP2 &= ~(WJ_SASC_QSPIC_AP_Msk << (region << 1U));
    sasc_base->QSPIC_AP2 |= (WJ_SASC_QSPIC_AP_RW << (region << 1U));
}

static inline void wj_sasc_qspic_set_sec_unsuper_ap_ro(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_AP2 &= ~(WJ_SASC_QSPIC_AP_Msk << (region << 1U));
    sasc_base->QSPIC_AP2 |= (WJ_SASC_QSPIC_AP_RO << (region << 1U));
}

static inline void wj_sasc_qspic_set_sec_unsuper_ap_wo(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_AP2 &= ~(WJ_SASC_QSPIC_AP_Msk << (region << 1U));
    sasc_base->QSPIC_AP2 |= (WJ_SASC_QSPIC_AP_WO << (region << 1U));
}

static inline void wj_sasc_qspic_set_sec_unsuper_ap_deny(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_AP2 &= ~(WJ_SASC_QSPIC_AP_Msk << (region << 1U));
    sasc_base->QSPIC_AP2 |= (WJ_SASC_QSPIC_AP_DENY << (region << 1U));
}

static inline void wj_sasc_sram_set_sec_unsuper_ap_rw(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_AP2 &= ~(WJ_SASC_RM_AP_Msk << (region << 1U));
    sasc_base->RM_AP2 |= (WJ_SASC_RM_AP_RW << (region << 1U));
}

static inline void wj_sasc_sram_set_sec_unsuper_ap_ro(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_AP2 &= ~(WJ_SASC_RM_AP_Msk << (region << 1U));
    sasc_base->RM_AP2 |= (WJ_SASC_RM_AP_RO << (region << 1U));
}

static inline void wj_sasc_sram_set_sec_unsuper_ap_wo(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_AP2 &= ~(WJ_SASC_RM_AP_Msk << (region << 1U));
    sasc_base->RM_AP2 |= (WJ_SASC_RM_AP_WO << (region << 1U));
}

static inline void wj_sasc_sram_set_sec_unsuper_ap_deny(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_AP2 &= ~(WJ_SASC_RM_AP_Msk << (region << 1U));
    sasc_base->RM_AP2 |= (WJ_SASC_RM_AP_DENY << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_unsuper_cd_di(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CD0 &= ~(WJ_SASC_QSPIC_CD_Msk << (region << 1U));
    sasc_base->QSPIC_CD0 |= (WJ_SASC_QSPIC_CD_DI << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_unsuper_cd_do(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CD0 &= ~(WJ_SASC_QSPIC_CD_Msk << (region << 1U));
    sasc_base->QSPIC_CD0 |= (WJ_SASC_QSPIC_CD_DO << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_unsuper_cd_io(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CD0 &= ~(WJ_SASC_QSPIC_CD_Msk << (region << 1U));
    sasc_base->QSPIC_CD0 |= (WJ_SASC_QSPIC_CD_IO << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_unsuper_cd_deny(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CD0 &= ~(WJ_SASC_QSPIC_CD_Msk << (region << 1U));
    sasc_base->QSPIC_CD0 |= (WJ_SASC_QSPIC_CD_DENY << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_unsuper_cd_di(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CD0 &= ~(WJ_SASC_RM_CD_Msk << (region << 1U));
    sasc_base->RM_CD0 |= (WJ_SASC_RM_CD_DI << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_unsuper_cd_do(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CD0 &= ~(WJ_SASC_RM_CD_Msk << (region << 1U));
    sasc_base->RM_CD0 |= (WJ_SASC_RM_CD_DO << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_unsuper_cd_io(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CD0 &= ~(WJ_SASC_RM_CD_Msk << (region << 1U));
    sasc_base->RM_CD0 |= (WJ_SASC_RM_CD_IO << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_unsuper_cd_deny(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CD0 &= ~(WJ_SASC_RM_CD_Msk << (region << 1U));
    sasc_base->RM_CD0 |= (WJ_SASC_RM_CD_DENY << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_super_cd_di(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CD1 &= ~(WJ_SASC_QSPIC_CD_Msk << (region << 1U));
    sasc_base->QSPIC_CD1 |= (WJ_SASC_QSPIC_CD_DI << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_super_cd_do(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CD1 &= ~(WJ_SASC_QSPIC_CD_Msk << (region << 1U));
    sasc_base->QSPIC_CD1 |= (WJ_SASC_QSPIC_CD_DO << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_super_cd_io(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CD1 &= ~(WJ_SASC_QSPIC_CD_Msk << (region << 1U));
    sasc_base->QSPIC_CD1 |= (WJ_SASC_QSPIC_CD_IO << (region << 1U));
}

static inline void wj_sasc_qspic_set_unsec_super_cd_deny(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CD1 &= ~(WJ_SASC_QSPIC_CD_Msk << (region << 1U));
    sasc_base->QSPIC_CD1 |= (WJ_SASC_QSPIC_CD_DENY << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_super_cd_di(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CD1 &= ~(WJ_SASC_RM_CD_Msk << (region << 1U));
    sasc_base->RM_CD1 |= (WJ_SASC_RM_CD_DI << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_super_cd_do(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CD1 &= ~(WJ_SASC_RM_CD_Msk << (region << 1U));
    sasc_base->RM_CD1 |= (WJ_SASC_RM_CD_DO << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_super_cd_io(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CD1 &= ~(WJ_SASC_RM_CD_Msk << (region << 1U));
    sasc_base->RM_CD1 |= (WJ_SASC_RM_CD_IO << (region << 1U));
}

static inline void wj_sasc_sram_set_unsec_super_cd_deny(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CD1 &= ~(WJ_SASC_RM_CD_Msk << (region << 1U));
    sasc_base->RM_CD1 |= (WJ_SASC_RM_CD_DENY << (region << 1U));
}

static inline void wj_sasc_qspic_set_sec_unsuper_cd_di(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CD2 &= ~(WJ_SASC_QSPIC_CD_Msk << (region << 1U));
    sasc_base->QSPIC_CD2 |= (WJ_SASC_QSPIC_CD_DI << (region << 1U));
}

static inline void wj_sasc_qspic_set_sec_unsuper_cd_do(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CD2 &= ~(WJ_SASC_QSPIC_CD_Msk << (region << 1U));
    sasc_base->QSPIC_CD2 |= (WJ_SASC_QSPIC_CD_DO << (region << 1U));
}

static inline void wj_sasc_qspic_set_sec_unsuper_cd_io(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CD2 &= ~(WJ_SASC_QSPIC_CD_Msk << (region << 1U));
    sasc_base->QSPIC_CD2 |= (WJ_SASC_QSPIC_CD_IO << (region << 1U));
}

static inline void wj_sasc_qspic_set_sec_unsuper_cd_deny(wj_sasc_qspic_regs_t *sasc_base, uint8_t region)
{
    sasc_base->QSPIC_CD2 &= ~(WJ_SASC_QSPIC_CD_Msk << (region << 1U));
    sasc_base->QSPIC_CD2 |= (WJ_SASC_QSPIC_CD_DENY  << (region << 1U));
}

static inline void wj_sasc_sram_set_sec_unsuper_cd_di(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CD2 &= ~(WJ_SASC_RM_CD_Msk << (region << 1U));
    sasc_base->RM_CD2 |= (WJ_SASC_RM_CD_DI << (region << 1U));
}

static inline void wj_sasc_sram_set_sec_unsuper_cd_do(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CD2 &= ~(WJ_SASC_RM_CD_Msk << (region << 1U));
    sasc_base->RM_CD2 |= (WJ_SASC_RM_CD_DO << (region << 1U));
}

static inline void wj_sasc_sram_set_sec_unsuper_cd_io(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CD2 &= ~(WJ_SASC_RM_CD_Msk << (region << 1U));
    sasc_base->RM_CD2 |= (WJ_SASC_RM_CD_IO << (region << 1U));
}

static inline void wj_sasc_sram_set_sec_unsuper_cd_deny(wj_sasc_sram_regs_t *sasc_base, uint8_t region)
{
    sasc_base->RM_CD2 &= ~(WJ_SASC_RM_CD_Msk << (region << 1U));
    sasc_base->RM_CD2 |= (WJ_SASC_RM_CD_DENY  << (region << 1U));
}

static inline void wj_sasc_qspic_config_addr(wj_sasc_qspic_regs_t *sasc_base, uint8_t region, uint32_t addr)
{
    sasc_base->QSPIC_RG[region] &= ~WJ_SASC_QSPIC_REG_RG_BA_Msk;
    sasc_base->QSPIC_RG[region] |= (addr << WJ_SASC_QSPIC_REG_RG_BA_Pos);
}

static inline void wj_sasc_sram_config_addr(wj_sasc_sram_regs_t *sasc_base, uint8_t region, uint32_t addr)
{
    sasc_base->RM_RG[region] &= ~WJ_SASC_RM_REG_RG_BA_Msk;
    sasc_base->RM_RG[region] |= (addr << WJ_SASC_RM_REG_RG_BA_Pos);
}

static inline void wj_sasc_qspic_config_size(wj_sasc_qspic_regs_t *sasc_base, uint8_t region, uint32_t size)
{
    sasc_base->QSPIC_RG[region] &= ~WJ_SASC_QSPIC_REG_RG_SZ_Msk;
    sasc_base->QSPIC_RG[region] |= (size << WJ_SASC_QSPIC_REG_RG_SZ_Pos);
}

static inline void wj_sasc_sram_config_size(wj_sasc_sram_regs_t *sasc_base, uint8_t region, uint32_t size)
{
    sasc_base->RM_RG[region] &= ~WJ_SASC_RM_REG_RG_SZ_Msk;
    sasc_base->RM_RG[region] |= (size << WJ_SASC_RM_REG_RG_SZ_Pos);
}

#ifdef __cplusplus
}
#endif

#endif /* _WJ_SASC_LL_H_ */
