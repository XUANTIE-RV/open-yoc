/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_sasc_v4.c
 * @brief    CSI Source File
 * @version  V1.0
 * @date     26. Jan 2018
 ******************************************************************************/

#include <csi_config.h>
#include <stdio.h>
#include <drv/sasc.h>
#ifdef CONFIG_CHIP_PANGU
#include <ck_sasc_v4_pangu.h>
#else
#include <ck_sasc_v4.h>
#endif
static uint8_t region[2][REGION_MAX_NUM] = {0x0};
static uint32_t s_sram_base;
static uint32_t sram_addr_offset;
static uint32_t sram_addr_base;
static ck_sasc_qspiflash_reg_t *fbase = (ck_sasc_qspiflash_reg_t *)QSPIFLASH_REG_BASE;
static ck_sasc_sram_reg_t *sbase = (ck_sasc_sram_reg_t *)SRAM_REG_BASE;

//
// Functions
//

static int alloc_region(uint8_t idx)
{
    uint8_t i;

    for (i = 0; i < REGION_MAX_NUM; i++) {
        if (region[idx][i] != REGION_USED) {
            region[idx][i] = REGION_USED;
            return i;
        }
    }

    return -1;
}

static inline void qspiflash_region_security_config(uint8_t region_idx, uint8_t is_security)
{
    uint32_t temp = fbase->QSPIC_CAR;
    temp &= ~(REG_WIDTH << (region_idx << 1));
    temp |= ((1 | (is_security << 0x1)) << (region_idx << 1));
    fbase->QSPIC_CAR = temp;
}

static inline void qspiflash_region_cd_config(uint8_t region_idx, sasc_cd_e ucd)
{
    uint32_t temp = fbase->QSPIC_CD0;
    temp &= ~(REG_WIDTH << (region_idx << 1));
    temp |= (ucd << (region_idx * 2));
    fbase->QSPIC_CD0 = temp;

    temp = fbase->QSPIC_CD2;
    temp &= ~(REG_WIDTH << (region_idx << 1));
    temp |= (ucd << (region_idx * 2));
    fbase->QSPIC_CD2 = temp;
}

static inline void qspiflash_region_ap_config(uint8_t region_idx, sasc_ap_e sap, sasc_ap_e uap)
{
    uint32_t temp = fbase->QSPIC_AP0;
    temp &= ~(REG_WIDTH << (region_idx << 1));
    temp |= (uap << (region_idx << 1));
    fbase->QSPIC_AP0 = temp;

    temp = fbase->QSPIC_AP2;
    temp &= ~(REG_WIDTH << (region_idx << 1));
    temp |= (uap << (region_idx << 1));
    fbase->QSPIC_AP2 = temp;

    temp = fbase->QSPIC_AP1;
    temp &= ~(REG_WIDTH << (region_idx << 1));
    temp |= (sap << (region_idx << 1));
    fbase->QSPIC_AP1 = temp;
}

static inline int qspiflash_region_address_config(uint8_t region_idx, uint32_t addr, sasc_size_e size)
{
    uint32_t region_addr = (addr - QSPIFLASH_ADDR_START) / QSPIFLASH_SECTOR_SIZE;
    fbase->QSPIC_RG[region_idx] = region_addr << 8 | size;
    return 0;
}

static inline void sram_region_security_config(uint8_t region_idx, uint8_t is_security)
{
    uint32_t temp = sbase->RM_CAR;
    temp &= ~(REG_WIDTH << (region_idx << 1));
    temp |= ((0x1 | (is_security << 1)) << (region_idx << 1));
    sbase->RM_CAR = temp;
}


static inline void sram_region_cd_config(uint8_t region_idx, sasc_cd_e ucd)
{
    uint32_t temp = sbase->RM_CD0;
    temp &= ~(REG_WIDTH << (region_idx << 1));
    temp |= (ucd << (region_idx << 1));
    sbase->RM_CD0 = temp;

    temp = sbase->RM_CD2;
    temp &= ~(REG_WIDTH << (region_idx << 1));
    temp |= (ucd << (region_idx << 1));
    sbase->RM_CD2 = temp;
}

static inline void sram_region_ap_config(uint8_t region_idx, sasc_ap_e sap, sasc_ap_e uap)
{
    uint32_t temp = sbase->RM_AP0;
    temp &= ~(REG_WIDTH << (region_idx << 1));
    temp |= (uap << (region_idx << 1));
    sbase->RM_AP0 = temp;

    temp = sbase->RM_AP2;
    temp &= ~(REG_WIDTH << (region_idx << 1));
    temp |= (uap << (region_idx << 1));
    sbase->RM_AP2 = temp;

    temp = sbase->RM_AP1;
    temp &= ~(REG_WIDTH << (region_idx << 1));
    temp |= (sap << (region_idx << 1));
    sbase->RM_AP1 = temp;
}

static inline void qspiflash_region_active_config(uint8_t region_idx)
{
    fbase->QSPIC_CR |= (1 << region_idx);
}

static inline void sram_region_active_config(uint8_t region_idx)
{
    sbase->RM_CR |= (1 << region_idx);
}

static inline int sram_region_address_config(uint8_t region_idx, uint32_t addr, sasc_size_e size)
{
    uint32_t region_addr = (addr - sram_addr_base) / 4;
    sbase->RM_RG[region_idx] = region_addr << sram_addr_offset | size;
    return 0;
}

static int32_t qspiflash_region_config(uint32_t addr, sasc_size_e size, sasc_ap_e sap, sasc_ap_e uap, sasc_cd_e ucd, uint8_t is_security)
{
    uint8_t region_idx = alloc_region(0);

    qspiflash_region_security_config(region_idx, is_security);
    qspiflash_region_address_config(region_idx, addr, size);
    qspiflash_region_cd_config(region_idx, ucd);
    qspiflash_region_ap_config(region_idx, sap, uap);
    qspiflash_region_active_config(region_idx);
    return 0;

}

static int32_t sram_region_config(uint32_t addr, sasc_size_e size, sasc_ap_e sap, sasc_ap_e uap, sasc_cd_e ucd, uint8_t is_security)
{
    uint8_t region_idx = alloc_region(1);

    if (IS_SRAM0_ADDR(addr)) {
        s_sram_base = SRAM_REG_BASE;
        sram_addr_offset = 8;
        sram_addr_base = SRAM0_ADDR_START;
    } else if (IS_SRAM1_ADDR(addr)) {
        s_sram_base = SRAM_REG_BASE + 0x100;
        sram_addr_offset = 8;
        sram_addr_base = SRAM1_ADDR_START;
    } else if (IS_SRAM2_ADDR(addr)) {
        s_sram_base = SRAM_REG_BASE + 0x200;
        sram_addr_offset = 8;
        sram_addr_base = SRAM2_ADDR_START;
    } else if (IS_SRAM3_ADDR(addr)) {
        s_sram_base = SRAM_REG_BASE + 0x300;
        sram_addr_offset = 8;
        sram_addr_base = SRAM3_ADDR_START;
#ifdef CONFIG_CHIP_PANGU
    } else if (IS_SRAM4_ADDR(addr)) {
        s_sram_base = SRAM_REG_BASE + 0x400;
        sram_addr_offset = 8;
        sram_addr_base = SRAM4_ADDR_START;
    } else if (IS_SRAM5_ADDR(addr)) {
        s_sram_base = SRAM_REG_BASE + 0x500;
        sram_addr_offset = 8;
        sram_addr_base = SRAM5_ADDR_START;
    } else if (IS_SRAM6_ADDR(addr)) {
        s_sram_base = SRAM_REG_BASE + 0x600;
        sram_addr_offset = 8;
        sram_addr_base = SRAM6_ADDR_START;
    } else if (IS_SRAM7_ADDR(addr)) {
        s_sram_base = SRAM_REG_BASE + 0x700;
        sram_addr_offset = 8;
        sram_addr_base = SRAM7_ADDR_START;
#endif
    }

    sbase = (ck_sasc_sram_reg_t *)s_sram_base;
    sram_region_security_config(region_idx, is_security);
    sram_region_address_config(region_idx, addr, size);
    sram_region_cd_config(region_idx, ucd);
    sram_region_ap_config(region_idx, sap, uap);
    sram_region_active_config(region_idx);
    return 0;
}
/**
  \brief       Config the sasc region attribute.
  \param[in]   handle  sasc handle to operate.
  \param[in]   addr  config region base address.
  \param[in]   size  config region addr.
  \param[in]   sap   super mode ap.
  \param[in]   uap   user mode ap.
  \param[in]   scd   super mode cd.
  \param[in]   ucd   user mode cd.
  \param[in]   is_security  config the region is security or not.
  \return      error code
*/
int32_t drv_sasc_config_region(uint32_t addr, sasc_size_e size, sasc_ap_e sap, sasc_ap_e uap, sasc_cd_e ucd, uint8_t is_security)
{
    uint32_t ret;

    if (IS_QSPIFLASH_ADDR(addr)) {
        ret = qspiflash_region_config(addr, size - SASC_FLASH_1S, sap, uap, ucd, is_security);
    } else if (IS_SRAM_ADDR(addr)) {
        ret = sram_region_config(addr, size, sap, uap, ucd, is_security);
    } else {
        return -1;
    }

    return ret;
}
