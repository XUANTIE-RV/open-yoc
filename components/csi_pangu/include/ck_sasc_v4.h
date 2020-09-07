/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_sasc_v4.h
 * @brief    head file for ck sasc
 * @version  V1.0
 * @date     26. Jan 2018
 ******************************************************************************/
#ifndef _CK_SASC_V4_H_
#define _CK_SASC_V4_H_

#include <drv/sasc.h>
#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define REG_WIDTH   0x3
#define REGION_USED 1
#define QSPIFLASH_ADDR_START   0x18000000
#define QSPIFLASH_ADDR_END     0x1fffffff
#define QSPIFLASH_SECTOR_SIZE  0x1000
#define QSPIFLASH_REG_BASE     0x5002c23c

#define IS_QSPIFLASH_ADDR(addr) \
    ((addr >= QSPIFLASH_ADDR_START) && (addr <= QSPIFLASH_ADDR_END))


#define SRAM_ADDR_START      0x20000000
#define SRAM_ADDR_END        0x2002ffff

#define SRAM0_ADDR_START     0x20000000
#define SRAM0_ADDR_END       0x2000ffff

#define SRAM1_ADDR_START     0x20010000
#define SRAM1_ADDR_END       0x2001ffff

#define SRAM2_ADDR_START     0x20020000
#define SRAM2_ADDR_END       0x2002ffff

#define SRAM3_ADDR_START     0x08000000
#define SRAM3_ADDR_END       0x0800ffff

#define IS_SRAM_ADDR(addr) \
    (((addr >= SRAM_ADDR_START) && (addr <= SRAM_ADDR_END)) || ((addr >= SRAM3_ADDR_START) && (addr <= SRAM3_ADDR_END)))

#define IS_SRAM0_ADDR(addr) \
    ((addr >= SRAM0_ADDR_START) && (addr <= SRAM0_ADDR_END))

#define IS_SRAM1_ADDR(addr) \
    ((addr >= SRAM1_ADDR_START) && (addr <= SRAM1_ADDR_END))

#define IS_SRAM2_ADDR(addr) \
    ((addr >= SRAM2_ADDR_START) && (addr <= SRAM2_ADDR_END))

#define IS_SRAM3_ADDR(addr) \
    ((addr >= SRAM3_ADDR_START) && (addr <= SRAM3_ADDR_END))

#define SRAM_REG_BASE       0x40206000

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
