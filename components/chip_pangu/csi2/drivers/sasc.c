/*
 *    Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/*******************************************************
 * @file       sasc.c
 * @brief      source file for rtc csi driver
 * @version    V4.0
 * @date       6. Aug 2020
 * ******************************************************/


#include <string.h>
#include <stdio.h>
#include <soc.h>
#include <drv/sasc.h>
#include "wj_sasc_ll.h"

#define QSPIFLASH_ADDR_START   (CD_QSPIMEM_BASE)
#define QSPIFLASH_ADDR_END     ((CD_QSPIMEM_BASE) + (0x7fffffU))
#define QSPIFLASH_SECTOR_SIZE  0x1000U
#define QSPIFLASH_REG_BASE     ((CD_QSPIC0_BASE) + (0x23CU))

#define IS_QSPIFLASH_ADDR(addr) \
    (((addr) >= (QSPIFLASH_ADDR_START)) && ((addr) <= (QSPIFLASH_ADDR_END)))

#define SRAM0_ADDR_START     WJ_CK804_TCM0_BASE
#define SRAM0_ADDR_END       ((((WJ_CK804_TCM0_BASE) + (WJ_CK804_TCM_SIZE)) - (1U)))
#define SRAM1_ADDR_START     (WJ_CK804_TCM1_BASE)
#define SRAM1_ADDR_END       ((((WJ_CK804_TCM1_BASE) + (WJ_CK804_TCM_SIZE))) - (1U))
#define SRAM2_ADDR_START     (WJ_CK804_TCM2_BASE)
#define SRAM2_ADDR_END       (((WJ_CK804_TCM2_BASE) + (WJ_CK804_TCM_SIZE)) - (1U))
#define SRAM3_ADDR_START     (WJ_CK804_TCM3_BASE)
#define SRAM3_ADDR_END       (((WJ_CK804_TCM3_BASE) + (WJ_CK804_TCM_SIZE)) - (1U))
#define SRAM4_ADDR_START     (WJ_CK804_TCM4_BASE)
#define SRAM4_ADDR_END       (((WJ_CK804_TCM4_BASE) + (WJ_CK804_TCM_SIZE)) - (1U))
#define SRAM5_ADDR_START     (WJ_CK804_TCM5_BASE)
#define SRAM5_ADDR_END       (((WJ_CK804_TCM5_BASE) + (WJ_CK804_TCM_SIZE)) - (1U))
#define SRAM6_ADDR_START     (WJ_CK804_TCM6_BASE)
#define SRAM6_ADDR_END       (((WJ_CK804_TCM6_BASE) + (WJ_CK804_TCM_SIZE)) - (1U))
#define SRAM7_ADDR_START     (WJ_CK804_TCM7_BASE)
#define SRAM7_ADDR_END       (((WJ_CK804_TCM7_BASE) + (WJ_CK804_TCM_SIZE)) - (1U))


#define IS_SRAM_ADDR(addr) \
    ((((addr) >= (SRAM0_ADDR_START)) && ((addr) <= (SRAM0_ADDR_END))) \
     || (((addr) >= (SRAM1_ADDR_START)) && (((addr)) <= (SRAM1_ADDR_END))) \
     || (((addr) >= (SRAM2_ADDR_START)) && (((addr)) <= (SRAM2_ADDR_END ))) \
     || (((addr) >= (SRAM3_ADDR_START)) && ((addr) <= (SRAM3_ADDR_END))) \
     || (((addr) >= (SRAM4_ADDR_START)) && ((addr) <= (SRAM4_ADDR_END))) \
     || (((addr) >= (SRAM5_ADDR_START)) && ((addr) <= (SRAM5_ADDR_END))) \
     || (((addr) >= (SRAM6_ADDR_START)) && ((addr) <= (SRAM6_ADDR_END))) \
     || (((addr) >= (SRAM7_ADDR_START)) && ((addr) <= (SRAM7_ADDR_END))))

#define IS_SRAM0_ADDR(addr) \
    (((addr) >= (SRAM0_ADDR_START)) && ((addr) <= (SRAM0_ADDR_END)))

#define IS_SRAM1_ADDR(addr) \
    (((addr) >= (SRAM1_ADDR_START)) && ((addr) <= (SRAM1_ADDR_END)))

#define IS_SRAM2_ADDR(addr) \
    (((addr) >= (SRAM2_ADDR_START)) && ((addr) <= (SRAM2_ADDR_END)))

#define IS_SRAM3_ADDR(addr) \
    (((addr) >= (SRAM3_ADDR_START)) && ((addr) <= (SRAM3_ADDR_END)))

#define IS_SRAM4_ADDR(addr) \
    (((addr) >= (SRAM4_ADDR_START)) && ((addr) <= (SRAM4_ADDR_END)))

#define IS_SRAM5_ADDR(addr) \
    (((addr) >= (SRAM5_ADDR_START)) && ((addr) <= (SRAM5_ADDR_END)))

#define IS_SRAM6_ADDR(addr) \
    (((addr) >= (SRAM6_ADDR_START)) && ((addr) <= (SRAM6_ADDR_END)))

#define IS_SRAM7_ADDR(addr) \
    (((addr) >= (SRAM7_ADDR_START)) && ((addr) <= (SRAM7_ADDR_END)))

#define SRAM_REG_BASE       WJ_SASC_BASE

static wj_sasc_qspic_regs_t *flash_base = (wj_sasc_qspic_regs_t *)QSPIFLASH_REG_BASE;
static wj_sasc_sram_regs_t *sram_base = (wj_sasc_sram_regs_t *)SRAM_REG_BASE;

csi_error_t csi_sasc_ram_config(uint8_t region_id, uint32_t base_addr, csi_sasc_ram_size_t size, csi_sasc_attr_t attr)
{
    uint32_t sram_addr_base;
    uint32_t s_sram_base;
    uint32_t region_addr;

    csi_error_t ret = CSI_OK;

    if ((!(IS_SRAM_ADDR(base_addr)))) {
        ret = CSI_ERROR;
    } else {
        if ((IS_SRAM0_ADDR(base_addr))) {
            sram_addr_base = SRAM0_ADDR_START;
            s_sram_base = SRAM_REG_BASE;
        } else if ((IS_SRAM1_ADDR(base_addr))) {
            sram_addr_base = SRAM1_ADDR_START;
            s_sram_base = SRAM_REG_BASE + 0x100U;
        } else if ((IS_SRAM2_ADDR(base_addr))) {
            sram_addr_base = SRAM2_ADDR_START;
            s_sram_base = SRAM_REG_BASE + 0x200U;
        } else if ((IS_SRAM3_ADDR(base_addr))) {
            sram_addr_base = SRAM3_ADDR_START;
            s_sram_base = SRAM_REG_BASE + 0x300U;
        } else if ((IS_SRAM4_ADDR(base_addr))) {
            sram_addr_base = SRAM4_ADDR_START;
            s_sram_base = SRAM_REG_BASE + 0x400U;
        } else if ((IS_SRAM5_ADDR(base_addr))) {
            sram_addr_base = SRAM5_ADDR_START;
            s_sram_base = SRAM_REG_BASE + 0x500U;
        } else if ((IS_SRAM6_ADDR(base_addr))) {
            sram_addr_base = SRAM6_ADDR_START;
            s_sram_base = SRAM_REG_BASE + 0x600U;
        } else {
            sram_addr_base = SRAM7_ADDR_START;
            s_sram_base = SRAM_REG_BASE + 0x700U;
        }

        sram_base = (wj_sasc_sram_regs_t *)s_sram_base;

        if (attr.is_secure) {
            wj_sasc_sram_set_sec_super_mode(sram_base, region_id);
        } else {
            wj_sasc_sram_set_unsec_super_mode(sram_base, region_id);
        }

        switch (attr.user_ap) {
            case SASC_RW:
                wj_sasc_sram_set_unsec_unsuper_ap_rw(sram_base, region_id);
                wj_sasc_sram_set_sec_unsuper_ap_rw(sram_base, region_id);
                break;

            case SASC_RO:
                wj_sasc_sram_set_unsec_unsuper_ap_ro(sram_base, region_id);
                wj_sasc_sram_set_sec_unsuper_ap_ro(sram_base, region_id);
                break;

            case SASC_WO:
                wj_sasc_sram_set_unsec_unsuper_ap_wo(sram_base, region_id);
                wj_sasc_sram_set_sec_unsuper_ap_wo(sram_base, region_id);
                break;

            case SASC_AP_DENY:
                wj_sasc_sram_set_unsec_unsuper_ap_deny(sram_base, region_id);
                wj_sasc_sram_set_sec_unsuper_ap_deny(sram_base, region_id);
                break;
        }

        switch (attr.super_ap) {
            case SASC_RW:
                wj_sasc_sram_set_unsec_super_ap_rw(sram_base, region_id);
                break;

            case SASC_RO:
                wj_sasc_sram_set_unsec_super_ap_ro(sram_base, region_id);
                break;

            case SASC_WO:
                wj_sasc_sram_set_unsec_super_ap_wo(sram_base, region_id);
                break;

            case SASC_AP_DENY:
                wj_sasc_sram_set_unsec_super_ap_deny(sram_base, region_id);
                break;
        }

        switch (attr.user_di) {
            case SASC_DI:
                wj_sasc_sram_set_unsec_unsuper_cd_di(sram_base, region_id);
                wj_sasc_sram_set_sec_unsuper_cd_di(sram_base, region_id);
                break;

            case SASC_DO:
                wj_sasc_sram_set_unsec_unsuper_cd_do(sram_base, region_id);
                wj_sasc_sram_set_sec_unsuper_cd_do(sram_base, region_id);
                break;

            case SASC_IO:
                wj_sasc_sram_set_unsec_unsuper_cd_io(sram_base, region_id);
                wj_sasc_sram_set_sec_unsuper_cd_io(sram_base, region_id);
                break;

            case SASC_DI_DENY:
                wj_sasc_sram_set_unsec_unsuper_cd_deny(sram_base, region_id);
                wj_sasc_sram_set_sec_unsuper_cd_deny(sram_base, region_id);
                break;
        }

        switch (attr.super_di) {
            case SASC_DI:
                wj_sasc_sram_set_unsec_super_cd_di(sram_base, region_id);
                break;

            case SASC_DO:
                wj_sasc_sram_set_unsec_super_cd_do(sram_base, region_id);
                break;

            case SASC_IO:
                wj_sasc_sram_set_unsec_super_cd_io(sram_base, region_id);
                break;

            case SASC_DI_DENY:
                wj_sasc_sram_set_unsec_super_cd_deny(sram_base, region_id);
                break;

        }

        wj_sasc_sram_enable(sram_base, region_id);
        region_addr = (base_addr - sram_addr_base) / 4;
        wj_sasc_sram_config_addr(sram_base, region_id, region_addr);
        wj_sasc_sram_config_size(sram_base, region_id, size);
    }

    return ret;
}

csi_error_t csi_sasc_flash_config(uint8_t region_id, uint32_t base_addr, csi_sasc_flash_size_t size, csi_sasc_attr_t attr)
{
    uint32_t region_addr;
    csi_error_t ret = CSI_OK;

    if ((!(IS_QSPIFLASH_ADDR(base_addr)))) {
        ret = CSI_ERROR;
    } else {
        if (attr.is_secure) {
            wj_sasc_qspic_set_sec_super_mode(flash_base, region_id);
        } else {
            wj_sasc_qspic_set_unsec_super_mode(flash_base, region_id);
        }

        switch (attr.user_ap) {
            case SASC_RW:
                wj_sasc_qspic_set_unsec_unsuper_ap_rw(flash_base, region_id);
                wj_sasc_qspic_set_sec_unsuper_ap_rw(flash_base, region_id);
                break;

            case SASC_RO:
                wj_sasc_qspic_set_unsec_unsuper_ap_ro(flash_base, region_id);
                wj_sasc_qspic_set_sec_unsuper_ap_ro(flash_base, region_id);
                break;

            case SASC_WO:
                wj_sasc_qspic_set_unsec_unsuper_ap_wo(flash_base, region_id);
                wj_sasc_qspic_set_sec_unsuper_ap_wo(flash_base, region_id);
                break;

            case SASC_AP_DENY:
                wj_sasc_qspic_set_unsec_unsuper_ap_deny(flash_base, region_id);
                wj_sasc_qspic_set_sec_unsuper_ap_deny(flash_base, region_id);
                break;
        }

        switch (attr.super_ap) {
            case SASC_RW:
                wj_sasc_qspic_set_unsec_super_ap_rw(flash_base, region_id);
                break;

            case SASC_RO:
                wj_sasc_qspic_set_unsec_super_ap_ro(flash_base, region_id);
                break;

            case SASC_WO:
                wj_sasc_qspic_set_unsec_super_ap_wo(flash_base, region_id);
                break;

            case SASC_AP_DENY:
                wj_sasc_qspic_set_unsec_super_ap_deny(flash_base, region_id);
                break;
        }

        switch (attr.user_di) {
            case SASC_DI:
                wj_sasc_qspic_set_unsec_unsuper_cd_di(flash_base, region_id);
                wj_sasc_qspic_set_sec_unsuper_cd_di(flash_base, region_id);
                break;

            case SASC_DO:
                wj_sasc_qspic_set_unsec_unsuper_cd_do(flash_base, region_id);
                wj_sasc_qspic_set_sec_unsuper_cd_do(flash_base, region_id);
                break;

            case SASC_IO:
                wj_sasc_qspic_set_unsec_unsuper_cd_io(flash_base, region_id);
                wj_sasc_qspic_set_sec_unsuper_cd_io(flash_base, region_id);
                break;

            case SASC_DI_DENY:
                wj_sasc_qspic_set_unsec_unsuper_cd_deny(flash_base, region_id);
                wj_sasc_qspic_set_sec_unsuper_cd_deny(flash_base, region_id);
                break;
        }

        switch (attr.super_di) {
            case SASC_DI:
                wj_sasc_qspic_set_unsec_super_cd_di(flash_base, region_id);
                break;

            case SASC_DO:
                wj_sasc_qspic_set_unsec_super_cd_do(flash_base, region_id);
                break;

            case SASC_IO:
                wj_sasc_qspic_set_unsec_super_cd_io(flash_base, region_id);
                break;

            case SASC_DI_DENY:
                wj_sasc_qspic_set_unsec_super_cd_deny(flash_base, region_id);
                break;

        }

        wj_sasc_qspic_enable(flash_base, region_id);
        wj_sasc_qspic_config_addr(flash_base, region_id, base_addr);
        wj_sasc_qspic_config_size(flash_base, region_id, size);
        region_addr = (base_addr - QSPIFLASH_ADDR_START) / QSPIFLASH_SECTOR_SIZE;
        wj_sasc_qspic_config_addr(flash_base, region_id, region_addr);
        wj_sasc_qspic_config_size(flash_base, region_id, size);
    }

    return ret;
}

csi_error_t csi_sasc_ram_enable(uint8_t region_id)
{
    wj_sasc_sram_enable(sram_base, region_id);
    return  CSI_OK;
}

csi_error_t csi_sasc_flash_enable(uint8_t region_id)
{
    wj_sasc_qspic_enable(flash_base, region_id);
    return  CSI_OK;
}

csi_error_t csi_sasc_ram_disable(uint8_t region_id)
{
    wj_sasc_sram_disable(sram_base, region_id);
    return  CSI_OK;
}

csi_error_t csi_sasc_flash_disable(uint8_t region_id)
{
    wj_sasc_qspic_disable(flash_base, region_id);
    return  CSI_OK;
}
