/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_efusec.h
 * @brief    head file for ck efusec
 * @version  V1.0
 * @date     19. Mar 2019
 ******************************************************************************/
#ifndef _CK_EFUSEC_H_
#define _CK_EFUSEC_H_

#include "drv/efusec.h"
#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EFUSEC_CTRL_PROGRAM_ENABLE          (1 << 0)
#define EFUSEC_CTRL_RELOAD_ENABLE           (1 << 1)
#define EFUSEC_CTRL_READ_ENABLE             (1 << 2)

#define EFUSEC_STA_PROGRAM_DONE             (1 << 0)
#define EFUSEC_STA_RELOAD_DONE              (1 << 1)
#define EFUSEC_STA_READ_DONE                (1 << 2)
#define EFUSEC_STA_BUSY                     (1 << 3)

#define LOCK_CTRL_ENABLE_LOCK0              (1 << 0)  /* all shadow regs can't be programmed */
#define LOCK_CTRL_ENABLE_LOCK1              (1 << 1)  /* all eFuse can't be programmed */

typedef struct {
    __IOM uint32_t EFUSEC_CTRL;        /* Offset: 0x000 (R/W) efusec controller control register */
    __IOM uint32_t EFUSEC_ADDR;        /* Offset: 0x004 (R/W) efuse address which will be programmed */
    __IOM uint32_t EFUSEC_WDATA;       /* Offset: 0x008 (R/W) efuse data which will be programmed */
    __IOM uint32_t EFUSEC_TIMER;       /* Offset: 0x00c (R/W) configure needed clock cycles before program or read efuse  */
    __IOM uint32_t EFUSEC_STA;         /* Offset: 0x010 (R/W) efusec controller status register */
    __IOM uint32_t EFUSEC_RDATA;       /* Offset: 0x014 (R/W) Data out from eFuse */
    __IOM uint32_t LOCK_CTRL;          /* Offset: 0x018 (R/W) store rcosc trim message which are read from eFuse1 address 0x1F4 's bit 0 to bit 5 */
    __IOM uint32_t RCOSC_TRIM_REG;     /* Offset: 0x01c (R/W) store rcosc trim message which are read from Efuse0 address 0x0C to address 0x0F */
    __IOM uint32_t CPU_JTG_EN;         /* Offset: 0x020 (R/W) store debug enable message which are read from eFuse1 address address 0x1F8's bit 0 */
    __IOM uint32_t PACKAGE;            /* Offset: 0x024 (R/W) store package message which are read from eFuse1 address 0x1FC's bit 0 to bit 3 */
} ck_efusec_reg_t;

#ifdef __cplusplus
}
#endif

#endif
