/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_efuse_ll.h
 * @brief    head file for efusec
 * @version  V1.0
 * @date     19. Mar 2019
 ******************************************************************************/
#ifndef _WJ_EFUSE_LL_H_
#define _WJ_EFUSE_LL_H_
#include <soc.h>
#include <csi_core.h>
#include <drv/efuse.h>

#ifdef __cplusplus
extern "C" {
#endif

/*READ,offset 0x02*/
#define WJ_EFUSE_READ_Pos           (2U)
#define WJ_EFUSE_READ_MASK          (0X1U<<WJ_EFUSE_READ_Pos)
#define WJ_EFUSE_READ_EN            WJ_EFUSE_READ_MASK

/*RELOAD,offset 0x01*/
#define WJ_EFUSE_RELOAD_Pos           (1U)
#define WJ_EFUSE_RELOAD_MASK          (0X1U<<WJ_EFUSE_RELOAD_Pos)
#define WJ_EFUSE_RELOAD_EN            WJ_EFUSE_RELOAD_MASK

/*WEN,offset 0x00*/
#define WJ_EFUSE_WEN_Pos            (0U)
#define WJ_EFUSE_WEN_MASK           (0x1U<<WJ_EFUSE_WEN_Pos)
#define WJ_EFSUE_WEN_EN             WJ_EFUSE_WEN_MASK



typedef struct {
    __IOM uint32_t EFUSEC_CTRL;                 /*offset 0x000 R/W EFUSE controls registers*/
    __IOM uint32_t EFUSEC_ADDR;                 /*offset 0x004 R/W EFUSE indirectly reads and writes to the address register*/
    __IOM uint32_t EFUSEC_WDATA;                /*offset 0x008 R/W EFUSE programming writes data registers indirectly*/
    __IOM uint32_t EFUSEC_TIMER;                /*offset 0x00c R/W EFUSE sequence cycle control register*/
    __IOM uint32_t EFUSEC_STATUS;               /*offset 0x010 RO  EFUSE status register*/
    __IOM uint32_t EFUSEC_RDATA;                /*offset 0x014 R/W EFUSE reads the data register indirectly*/
    __IOM uint32_t LOCK_CTRL;                   /*offset 0x018 R/W EFUSE Indirectly reads the data register. EFUSE is a shadow register that holds locked information*/
    __IOM uint32_t RCOSC_TRIM;                  /*offset 0x01c R/W EFUSE indirect data read lock EFUSE stored information shadow registers EFUSE deposited on chip 32 k oscillator accuracy tuning parameters of shadow registers*/
    __IOM uint32_t CPU_JTG_EN;                  /*offset 0x020 R/W EFUSE stores shadow registers that are not allowed for debugging*/
    __IOM uint32_t PACKAGE;                     /*offset 0x024 R/W EFUSE holds the shadow register that encapsulates information*/
} wj_efuse_regs_t;

static inline void wj_efuse_write_enable(wj_efuse_regs_t *efuse)
{
    efuse->EFUSEC_CTRL = WJ_EFSUE_WEN_EN;
}

static inline void wj_efuse_read_enable(wj_efuse_regs_t *efuse, uint32_t addr)
{
    efuse->EFUSEC_ADDR = addr;
    efuse->EFUSEC_CTRL = WJ_EFUSE_READ_EN;
}

static inline void wj_efuse_write(wj_efuse_regs_t *efuse, uint32_t addr, uint32_t data)
{
    efuse->EFUSEC_ADDR = addr;
    efuse->EFUSEC_WDATA = data;
    wj_efuse_write_enable(efuse);
}

static inline void wj_efuse_read(wj_efuse_regs_t *efuse, uint8_t *data)
{
    *data = efuse->EFUSEC_RDATA;
}

static inline void wj_efuse_reaload(wj_efuse_regs_t *efuse)
{
    efuse->EFUSEC_CTRL = WJ_EFUSE_RELOAD_EN;
}

static inline uint32_t wj_efuse_lock_status(wj_efuse_regs_t *efuse)
{
    return efuse->LOCK_CTRL;
}

uint32_t wj_efuse_program_bytes(csi_efuse_t *efuse, uint32_t addr, uint32_t *data, uint32_t size);
uint32_t wj_efuse_read_bits(csi_efuse_t *efuse, uint32_t addr, uint8_t *data, uint32_t size);
int32_t wj_drv_efuse_read(csi_efuse_t *efuse, uint32_t addr, void *data, uint32_t size);
int32_t wj_drv_efuse_program(csi_efuse_t *efuse, uint32_t addr, const void *data, uint32_t size);


#ifdef __cplusplus
}

#endif

#endif