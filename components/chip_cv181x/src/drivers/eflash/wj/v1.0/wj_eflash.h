/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     wj_eflash.h
 * @brief    header file for eflash driver
 * @version  V1.0
 * @date     21. May 2020
 ******************************************************************************/

#ifndef _WJ_EFLASH_H_
#define _WJ_EFLASH_H_
#include <soc.h>
#include <csi_core.h>
#include <drv/eflash.h>

#ifdef __cplusplus
extern "C" {
#endif
#define EFLASH_SECTOR_SIZE   0x200
#define EFLASH_ERASED_VALUE  0xff
#define EFLASH_PAGE_SIZE     0
#define EFLASH_ADDR_START    0x10000000UL
#define EFLASH_ADDR_END      0x1003f7ffUL
/*
EFLASH register bit definition
*/

/*NVR,offset 0x00*/
#define WJ_ELFASH_NVR_Pos               (0U)
#define WJ_EFLASH_NVR_MASK              (0x1U<<WJ_ELFASH_NVR_Pos)
#define WJ_EFLASH_NVR_EN                WJ_EFLASH_NVR_MASK

/*PE,offset 0x10*/
#define WJ_EFLASH_PE_Pos                (0U)
#define WJ_EFLASH_PE_MASK               (0x1U<<WJ_EFLASH_PE_Pos)
#define WJ_EFLASH_PE_EN                 WJ_EFLASH_PE_MASK

/*WE,offset 0x18*/
#define WJ_EFLASH_WE_Pos                (0U)
#define WJ_EFLASH_WE_MASK               (0x1U<<WJ_EFLASH_WE_Pos)
#define WJ_EFLASH_WE_EN                 WJ_EFLASH_WE_MASK

/*EFC_CAR,offset 0x38*/
#define WJ_EFLASH_EFC_CAR1_Pos          (0U)
#define WJ_EFLASH_EFC_CAR1_MASK         (0x1U<<WJ_EFLASH_EFC_CAR1_Pos)
#define WJ_EFLASH_EFC_CAR1              WJ_EFLASH_EFC_CAR1_MASK

#define WJ_EFLASH_EFC_CAR2_Pos          (1U)
#define WJ_EFLASH_EFC_CAR2_MASK         (0x1U<<WJ_EFLASH_EFC_CAR2_Pos)
#define WJ_EFLASH_EFC_CAR2              WJ_EFLASH_EFC_CAR2_MASK

#define WJ_EFLASH_EFC_CAR3_Pos          (2U)
#define WJ_EFLASH_EFC_CAR3_MASK         (0x1U<<WJ_EFLASH_EFC_CAR3_Pos)
#define WJ_EFLASH_EFC_CAR3              WJ_EFLASH_EFC_CAR3_MASK

#define WJ_EFLASH_EFC_CAR4_Pos          (4U)
#define WJ_EFLASH_EFC_CAR4_MASK         (0x4U<<WJ_EFLASH_EFC_CAR4_Pos)
#define WJ_EFLASH_EFC_CAR4              WJ_EFLASH_EFC_CAR4_MASK

#define WJ_EFLASH_EFC_CAR5_Pos          (5U)
#define WJ_EFLASH_EFC_CAR5_MASK         (0x1U<<WJ_EFLASH_EFC_CAR5_Pos)
#define WJ_EFLASH_EFC_CAR5              WJ_EFLASH_EFC_CAR5_MASK

#define WJ_EFLASH_EFC_CAR6_Pos          (6U)
#define WJ_EFLASH_EFC_CAR6_MASK         (0x1U<<WJ_EFLASH_EFC_CAR6_Pos)
#define WJ_EFLASH_EFC_CAR6              WJ_EFLASH_EFC_CAR6_MASK

#define WJ_EFLASH_EFC_CAR7_Pos          (7U)
#define WJ_EFLASH_EFC_CAR7_MASK         (0x1U<<WJ_EFLASH_EFC_CAR7_Pos)
#define WJ_EFLASH_EFC_CAR7              WJ_EFLASH_EFC_CAR7_MASK

/*EFC_CR,offset 0x3c*/
#define WJ_EFLASH_EFC_CR1_Pos           (0U)
#define WJ_EFLASH_EFC_CR1_MASK          (0x1u<<WJ_EFLASH_EFC_CR1_Pos)
#define WJ_EFLASH_EFC_CR1_EN            WJ_EFLASH_EFC_CR1_MASK

#define WJ_EFLASH_EFC_CR2_Pos           (1U)
#define WJ_EFLASH_EFC_CR2_MASK          (0x1u<<WJ_EFLASH_EFC_CR2_Pos)
#define WJ_EFLASH_EFC_CR2_EN            WJ_EFLASH_EFC_CR2_MASK

#define WJ_EFLASH_EFC_CR3_Pos           (2U)
#define WJ_EFLASH_EFC_CR3_MASK          (0x1u<<WJ_EFLASH_EFC_CR3_Pos)
#define WJ_EFLASH_EFC_CR3_EN            WJ_EFLASH_EFC_CR3_MASK

#define WJ_EFLASH_EFC_CR4_Pos           (3U)
#define WJ_EFLASH_EFC_CR4_MASK          (0x1u<<WJ_EFLASH_EFC_CR4_Pos)
#define WJ_EFLASH_EFC_CR4_EN            WJ_EFLASH_EFC_CR4_MASK

#define WJ_EFLASH_EFC_CR5_Pos           (4U)
#define WJ_EFLASH_EFC_CR5_MASK          (0x1u<<WJ_EFLASH_EFC_CR5_Pos)
#define WJ_EFLASH_EFC_CR5_EN            WJ_EFLASH_EFC_CR5_MASK

#define WJ_EFLASH_EFC_CR6_Pos           (6U)
#define WJ_EFLASH_EFC_CR6_MASK          (0x1u<<WJ_EFLASH_EFC_CR6_Pos)
#define WJ_EFLASH_EFC_CR6_EN            WJ_EFLASH_EFC_CR6_MASK

#define WJ_EFLASH_EFC_CR7_Pos           (7U)
#define WJ_EFLASH_EFC_CR7_MASK          (0x1u<<WJ_EFLASH_EFC_CR7_Pos)
#define WJ_EFLASH_EFC_CR7_EN            WJ_EFLASH_EFC_CR7_MASK

/*EFC_CFG,offset 0x60*/
#define WJ_EFLASH_EFC_CFG_Pos           (0U)
#define WJ_EFLASH_EFC_CFG_MASK          (0x1U<<WJ_EFLASH_EFC_CFG_Pos)

typedef struct {
    __IOM uint32_t EFLASH_NVR;                     /*offst 0x000 R/W     Non-Volatile Register Sector select*/
    __IOM uint32_t EFLASH_ADDR;                    /*offst 0x004 R/W     Address register*/
    uint32_t EFLASH_Reserved1;                     /*offst 0x008 R/W     Reserved*/
    uint32_t EFLASH_Reserved2;                     /*offst 0x00c R/W     Reserved*/
    __IOM uint32_t EFLASH_PE;                      /*offst 0x010 R/W     Page erase enable register*/
    uint32_t EFLASH_Reserved3;                     /*offst 0x014 R/W     Reserved*/
    __IOM uint32_t EFLASH_WE;                      /*offst 0x018 R/W     Write enbale register*/
    __IOM uint32_t EFLASH_WDATA;                   /*offst 0x01c R/W     Write data register*/
    __IOM uint32_t EFLASH_TRC;                     /*offst 0x020 R/W     Read cycle time register*/
    __IOM uint32_t EFLASH_TNVS;                    /*offst 0x024 R/W     PROG/ERASE/CEb/NVR/RDN/Address to Web setup time register*/
    __IOM uint32_t EFLASH_TRGS;                    /*offst 0x028 R/W     Web low to PROG2 high setup time register*/
    __IOM uint32_t EFLASH_TPROG;                   /*offst 0x02c R/W     Byte program time register*/
    __IOM uint32_t EFLASH_TRCV;                    /*offst 0x030 R/W     Program/sector-erase/chip-erase recovery time register*/
    __IOM uint32_t EFLASH_TERASE;                  /*offst 0x034 R/W     Sector/chip erase time register*/
    __IOM uint32_t EFLASH_EFC_CAR;                 /*offst 0x038 R/W     Flash configure attributes reister*/
    __IOM uint32_t EFLASH_EFC_CR;                  /*offst 0x03c R/W     Flash control register*/
    __IOM uint32_t EFLASH_EFC_RG0;                 /*offst 0x040 R/W     Flash region 0 base address and size register*/
    __IOM uint32_t EFLASH_EFC_RG1;                 /*offst 0x044 R/W     Flash region 1 base address and size register*/
    __IOM uint32_t EFLASH_EFC_RG2;                 /*offst 0x048 R/W     Flash region 2 base address and size register*/
    __IOM uint32_t EFLASH_EFC_RG3;                 /*offst 0x04c R/W     Flash region 3 base address and size register*/
    __IOM uint32_t EFLASH_EFC_RG4;                 /*offst 0x050 R/W     Flash region 4 base address and size register*/
    __IOM uint32_t EFLASH_EFC_RG5;                 /*offst 0x05c R/W     Flash region 5 base address and size register*/
    __IOM uint32_t EFLASH_EFC_RG6;                 /*offst 0x058 R/W     Flash region 6 base address and size register*/
    __IOM uint32_t EFLASH_EFC_RG7;                 /*offst 0x05c R/W     Flash region 7 base address and size register*/
    __IOM uint32_t EFLASH_EFC_CFG;                 /*offst 0x060 R/W     Flash Configuration register*/

} wj_eflash_regs_t;

static inline void wj_select_EN(wj_eflash_regs_t *eflash_base)
{
    eflash_base->EFLASH_NVR = WJ_EFLASH_NVR_EN;
}
static inline void wj_eflash_erase_enbale(wj_eflash_regs_t *eflash_base)
{
    eflash_base->EFLASH_PE = WJ_EFLASH_PE_EN;
}
static inline void wj_eflash_write_enbale(wj_eflash_regs_t *eflash_base)
{
    eflash_base->EFLASH_WE = WJ_EFLASH_WE_EN;
}
static inline void wj_edflash_write_data(wj_eflash_regs_t *eflash_base, uint32_t data)
{
    eflash_base->EFLASH_WDATA = data;
}
static inline void wj_eflash_write_address(wj_eflash_regs_t *elfash_base, uint32_t address)
{
    elfash_base->EFLASH_ADDR = address;
}

#ifdef __cplusplus
}
#endif

#endif