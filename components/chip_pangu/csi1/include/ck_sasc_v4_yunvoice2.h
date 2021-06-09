/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     ck_sasc_v4_yunvoice2.h
 * @brief    head file for ck sasc
 * @version  V1.0
 * @date     06. Mar 2019
 ******************************************************************************/
#ifndef _CK_SASC_V4_YUNVOCIE2_H_
#define _CK_SASC_V4_YUNVOCIE2_H_

#include <drv/sasc.h>
#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define REG_WIDTH   0x3
#define REGION_USED 1
#define QSPIFLASH_ADDR_START   CSKY_QSPIMEM_BASE
#define QSPIFLASH_ADDR_END     CSKY_QSPIMEM_BASE + 0x7fffff
#define QSPIFLASH_SECTOR_SIZE  0x1000
#define QSPIFLASH_REG_BASE     (CSKY_QSPIC0_BASE + 0x23C)

#define IS_QSPIFLASH_ADDR(addr) \
    ((addr >= QSPIFLASH_ADDR_START) && (addr <= QSPIFLASH_ADDR_END))

#ifdef __CK804__
#define SRAM0_ADDR_START     CSKY_CK804_TCM0_BASE
#define SRAM0_ADDR_END       (CSKY_CK804_TCM0_BASE + CSKY_CK804_TCM_SIZE - 1)
#define SRAM1_ADDR_START     CSKY_CK804_TCM1_BASE
#define SRAM1_ADDR_END       (CSKY_CK804_TCM1_BASE + CSKY_CK804_TCM_SIZE - 1)
#define SRAM2_ADDR_START     CSKY_CK804_TCM2_BASE
#define SRAM2_ADDR_END       (CSKY_CK804_TCM2_BASE + CSKY_CK804_TCM_SIZE - 1)
#define SRAM3_ADDR_START     CSKY_CK804_TCM3_BASE
#define SRAM3_ADDR_END       (CSKY_CK804_TCM3_BASE + CSKY_CK804_TCM_SIZE - 1)
#define SRAM4_ADDR_START     CSKY_CK804_TCM4_BASE
#define SRAM4_ADDR_END       (CSKY_CK804_TCM4_BASE + CSKY_CK804_TCM_SIZE - 1)
#define SRAM5_ADDR_START     CSKY_CK804_TCM5_BASE
#define SRAM5_ADDR_END       (CSKY_CK804_TCM5_BASE + CSKY_CK804_TCM_SIZE - 1)
#define SRAM6_ADDR_START     CSKY_CK804_TCM6_BASE
#define SRAM6_ADDR_END       (CSKY_CK804_TCM6_BASE + CSKY_CK804_TCM_SIZE - 1)
#define SRAM7_ADDR_START     CSKY_CK804_TCM7_BASE
#define SRAM7_ADDR_END       (CSKY_CK804_TCM7_BASE + CSKY_CK804_TCM_SIZE - 1)
#endif


#define IS_SRAM_ADDR(addr) \
    (((addr >= SRAM0_ADDR_START) && (addr <= SRAM0_ADDR_END)) \
  || ((addr >= SRAM1_ADDR_START) && (addr <= SRAM1_ADDR_END)) \
  || ((addr >= SRAM2_ADDR_START) && (addr <= SRAM2_ADDR_END)) \
  || ((addr >= SRAM3_ADDR_START) && (addr <= SRAM3_ADDR_END)) \
  || ((addr >= SRAM4_ADDR_START) && (addr <= SRAM4_ADDR_END)) \
  || ((addr >= SRAM5_ADDR_START) && (addr <= SRAM5_ADDR_END)) \
  || ((addr >= SRAM6_ADDR_START) && (addr <= SRAM6_ADDR_END)) \
  || ((addr >= SRAM7_ADDR_START) && (addr <= SRAM7_ADDR_END)))

#define IS_SRAM0_ADDR(addr) \
    ((addr >= SRAM0_ADDR_START) && (addr <= SRAM0_ADDR_END))

#define IS_SRAM1_ADDR(addr) \
    ((addr >= SRAM1_ADDR_START) && (addr <= SRAM1_ADDR_END))

#define IS_SRAM2_ADDR(addr) \
    ((addr >= SRAM2_ADDR_START) && (addr <= SRAM2_ADDR_END))

#define IS_SRAM3_ADDR(addr) \
    ((addr >= SRAM3_ADDR_START) && (addr <= SRAM3_ADDR_END))

#define IS_SRAM4_ADDR(addr) \
    ((addr >= SRAM4_ADDR_START) && (addr <= SRAM4_ADDR_END))

#define IS_SRAM5_ADDR(addr) \
    ((addr >= SRAM5_ADDR_START) && (addr <= SRAM5_ADDR_END))

#define IS_SRAM6_ADDR(addr) \
    ((addr >= SRAM6_ADDR_START) && (addr <= SRAM6_ADDR_END))

#define IS_SRAM7_ADDR(addr) \
    ((addr >= SRAM7_ADDR_START) && (addr <= SRAM7_ADDR_END))

#define SRAM_REG_BASE       CSKY_SASC_BASE

#define REGION_MAX_NUM  8

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
} ck_sasc_qspiflash_reg_t;

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
} ck_sasc_sram_reg_t;

#ifdef __cplusplus
}
#endif

#endif
