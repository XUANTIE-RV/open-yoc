/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_efusec_v2.h
 * @brief    head file for ck efusec_v2
 * @version  V1.0
 * @date     19. Mar 2019
 ******************************************************************************/
#ifndef _CK_EFUSEC_V2_H_
#define _CK_EFUSEC_V2_H_

#include "drv_efusec.h"
#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EFUSEC_CTRL_PROGRAM_ENABLE          (1 << 0)
#define EFUSEC_CTRL_READ_ENABLE             (1 << 0)
#define EFUSEC_STA_BUSY                     (1 << 0)

#define EFUSEC_WP_A_MAGIC           (0x376128a6)
#define EFUSEC_WP_B_MAGIC           (0xea5238c9)

typedef struct {
    __IOM uint32_t RDATA;        /* Offset: 0x000 (R/W) data out from efuse register */
    __IOM uint32_t ADDR;         /* Offset: 0x004 (R/W) read and write efuse address */
    __IOM uint32_t REN;          /* Offset: 0x008 (R/W) read enable */
    __IOM uint32_t WEN;          /* Offset: 0x00c (R/W) write enable  */
    __IOM uint32_t IDLE;         /* Offset: 0x010 (R/W) idle of efuse */
    __IOM uint32_t WTIME;        /* Offset: 0x014 (R/W) write timing configure register */
    __IOM uint32_t WP_A;         /* Offset: 0x018 (R/W) write protection value A */
    __IOM uint32_t WP_B;         /* Offset: 0x01c (R/W) write protection value B */
    __IOM uint32_t WP_CNT;       /* Offset: 0x020 (R/W) the cycles of set WP_B */
    __IOM uint32_t WP_CLR;       /* Offset: 0x024 (R/W) clear the write protection */
    __IOM uint32_t AVD_PAD;      /* Offset: 0x024 (R/W) ACDD pad control register */
} ck_efusec_v2_reg_t;

#ifdef __cplusplus
}
#endif

#endif
