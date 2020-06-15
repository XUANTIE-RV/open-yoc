/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     ck_eflash.h
 * @brief    head file for ck eflash
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_EFLASH_H_
#define _CK_EFLASH_H_

#include "drv/eflash.h"
#include "soc.h"

#define EFLASH_SECTOR_SIZE  0x200
#define EFLASH_ERASED_VALUE 0xff
#define EFLASH_PROGRAM_UINT 0x4
#define EFLASH_PAGE_SIZE    0

typedef struct {
    uint32_t RESERVED1[1];
    __IOM uint32_t ADDR;                 /* Offset: 0x004 (R/W) operate addr register */
    uint32_t RESERVED2[2];
    __IOM uint32_t PE;                   /* Offset: 0x010 (R/W) page erase enable register */
    uint32_t RESERVED3[1];
    __IOM uint32_t WE;                   /* Offset: 0x018 (R/W) writer enable register */
    __IOM uint32_t WDATA;                /* Offset: 0x01c (R/W) writer data register */
    __IOM uint32_t TRC;                  /* Offset: 0x020 (R/W) read cycle time register */
    __IOM uint32_t TNVS;                 /* Offset: 0x024 (R/W) address to web setup time register */
    __IOM uint32_t TPGS;                 /* Offset: 0x028 (R/W) web low to prog2 high setup register */
    __IOM uint32_t TPROG;                /* Offset: 0x02c (R/W) byte program time register */
    __IOM uint32_t TRCV;                 /* Offset: 0x030 (R/W) recovery time register */
    __IOM uint32_t TERASE;               /* Offset: 0x034 (R/W) erase time register */
} ck_eflash_reg_t;

#endif
