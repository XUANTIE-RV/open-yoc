/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_sasc_v2.h
 * @brief    head file for ck sasc
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_SASC_H_
#define _CK_SASC_H_

#include "drv/sasc.h"
#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REGION_USED 1
#define EFLASH_ADDR_START   0x10000000
#define EFLASH_ADDR_END     0x1003f7ff
#define EFLASH_SECTOR_SIZE  0x200
#define EFLASH_REG_BASE     0x40005038

#define IS_EFLASH_ADDR(addr) \
    ((addr >= EFLASH_ADDR_START) && (addr <= EFLASH_ADDR_END))

#define SRAM_ADDR_START     0x20000000
#define SRAM_ADDR_END       0x2000ffff

#define SRAM0_ADDR_START     0x20000000
#define SRAM0_ADDR_END       0x20003fff

#define SRAM1_ADDR_START     0x20004000
#define SRAM1_ADDR_END       0x20007fff

#define SRAM2_ADDR_START     0x20008000
#define SRAM2_ADDR_END       0x2000ffff

#define IS_SRAM_ADDR(addr) \
    ((addr >= SRAM_ADDR_START) && (addr <= SRAM_ADDR_END))

#define IS_SRAM0_ADDR(addr) \
    ((addr >= SRAM0_ADDR_START) && (addr <= SRAM0_ADDR_END))

#define IS_SRAM1_ADDR(addr) \
    ((addr >= SRAM1_ADDR_START) && (addr <= SRAM1_ADDR_END))

#define IS_SRAM2_ADDR(addr) \
    ((addr >= SRAM2_ADDR_START) && (addr <= SRAM2_ADDR_END))

#define SRAM_REG_BASE     0x40009000

#define REGION_MAX_NUM  8

typedef struct {
    __IOM uint32_t EFC_CAR;                  /* Offset: 0x000 (W/R)  flash configure attributes register */
    __IOM uint32_t EFC_CR;                   /* Offset: 0x004 (W/R)  flash control register */
    __IOM uint32_t EFC_RG[8];                /* Offset: 0x008 (W/R)  flash base address and size register */
    __IOM uint32_t EFC_CFG0;                 /* Offset: 0x00c (W/R)  flash configuration register */
    __IOM uint32_t EFC_CFG1;                 /* Offset: 0x00c (W/R)  flash configuration register */
} ck_sasc_eflash_reg_t;

typedef struct {
    __IOM uint32_t RM_CAR;                   /* Offset: 0x000 (W/R)  sram configure attributes register */
    __IOM uint32_t RM_CR;                    /* Offset: 0x004 (W/R)  sram control register */
    __IOM uint32_t RM_RG[8];                 /* Offset: 0x008 (W/R)  sram base address and size register */
    __IOM uint32_t RM_CFG0;                  /* Offset: 0x00c (W/R)  sram configuration register */
    __IOM uint32_t RM_CFG1;                  /* Offset: 0x00c (W/R)  sram configuration register */
} ck_sasc_sram_reg_t;

#ifdef __cplusplus
}
#endif

#endif
