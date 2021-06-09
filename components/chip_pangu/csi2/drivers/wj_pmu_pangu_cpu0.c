/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_pmu_pangu_cpu0.c
 * @brief    CSI Source File for PMU Driver
 * @version  V1.0
 * @date     20. Aug 2020
 ******************************************************************************/
#include <csi_config.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    WAIT_MODE = 0U,
    DOZE_MODE,
    STOP_MODE,
    STANDBY_MODE,
    SLEEP_MODE
} lpm_mode_e;

#ifdef CONFIG_PM
#include <drv/io.h>
#include <drv/pm.h>
#include <soc.h>
#include <wj_pmu_pangu_cpu0.h>

#define RUN_ADDR            0x18000000
#define STORAGE_ADDR        0x8023000

#define PMU_WKUPMASK 0x30000018
#define PMU_LPCR     0x30000014
#define PMU_DLC_IFR  0x30000210
#define PREG32(addr) *((volatile unsigned int *)addr)

extern int32_t arch_do_cpu_save(void);
extern int32_t arch_do_cpu_resume(void);
extern int32_t arch_resume_context(void);

void resume_context_from_stop_mode(void)
{
    *(volatile uint32_t *)0x31000014 = 0x1;
    *(volatile uint32_t *)0x1a000100 = 0x2000000;
    *(volatile uint32_t *)0x1a000054 = 0x9;
    *(volatile uint32_t *)0x1a000000 = 0x1c1168;
    *(volatile uint32_t *)0x1a000004 = 0x029e949f;
    *(volatile uint32_t *)0x1a00000c = 0x3009;

    while ((*(volatile uint32_t *)0x1a00000c) & 0x01);
}

static void set_resume_func(wj_pmu_reg_cpr0_t *pmu_base, uint32_t *func)
{
    /* enable the lp bootloadr secure */
    *(volatile uint32_t *)(WJ_TIPC_BASE + 4) = 0x100;
    wj_pmu_write_lpbootaddr_804_0(pmu_base, (uint32_t)func - (uint32_t)RUN_ADDR + (uint32_t)STORAGE_ADDR);
}

void sdram_auto_flash_enable(int flag)
{
    if (flag) {
        *(volatile uint32_t *)(0x1a00000c) = 0x3002;

        while ((*(volatile uint32_t *)(0x1a00000c) & 0x800) != 0x800);

        printf("SDRAM into SelfRefresh, CPU 0 will into wait state\n\n");
    } else {
        *(volatile uint32_t *)(0x1a00000c) = 0x3009;

        while ((*(volatile uint32_t *)(0x1a00000c) & 0x800)  != 0);

        // printf("SDRAM into Auto Refresh, SDRAM OK\n");
    }
}
void soc_sleep(lpm_mode_e mode)
{
#ifdef CONFIG_ARCH_CSKY

    while ((*(volatile uint32_t *)(0x30000208)) != 0);

    // PREG32(PMU_WKUPMASK) = 0xF;
    // config PMU lowpower
    PREG32(PMU_LPCR) = 0x1;
    //config CPU clear pmu intr
    PREG32(PMU_DLC_IFR) = 0x1f;

    while ((*(volatile uint32_t *)(PMU_DLC_IFR)) != 0);

    PREG32(0xe000e280) = 0x10040;
    PREG32(0xe000e284) = 0x400;
    PREG32(0xE000E140) = 0x40;
    PREG32(0xE000E144) = 0x400;

    VIC->ICPR[0] = 0xFFFFFFFF; /* clear pend IRQ */
#endif

    if (mode == WAIT_MODE) {
        wj_pmu_write_change_volt_dis(PMU_REG_BASE);
        wj_pmu_write_lpen_en(PMU_REG_BASE);
        __WFI();
    } else if (mode == DOZE_MODE) {
        wj_pmu_write_change_volt_en(PMU_REG_BASE);
        wj_pmu_write_lpen_en(PMU_REG_BASE);
        __WFI();
    } else if (mode == STANDBY_MODE) {
        set_resume_func(PMU_REG_BASE, (uint32_t *)&arch_resume_context);
        wj_pmu_write_lpen_en(PMU_REG_BASE);
        __STOP();
    } else {
        __WFI();
    }

}

static uint32_t s_irq_state;

/**
  \brief       choose the pmu mode to enter
  \param[in]   handle  pmu handle to operate.
  \param[in]   mode    \ref pmu_mode_e
  \return      error code
*/
csi_error_t soc_pm_enter_sleep(csi_pm_mode_t mode)
{
    csi_error_t ret = CSI_OK;

#ifdef CONFIG_ARCH_CSKY
    VIC->ICPR[0] = 0xFFFFFFFF; /* clear pend IRQ */
#endif

    s_irq_state = csi_irq_save();

    switch (mode) {
        case PM_MODE_RUN:
            break;

        case PM_MODE_SLEEP_1:
            soc_sleep(WAIT_MODE);
            break;

        case PM_MODE_SLEEP_2:
            soc_sleep(DOZE_MODE);
            break;

        case PM_MODE_DEEP_SLEEP_2:
            soc_sleep(STANDBY_MODE);
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    csi_irq_restore(s_irq_state);

    return ret;
}

/**
  \brief       Config the wakeup source.
  \param[in]   wakeup_num wakeup source num
  \param[in]   enable  flag control the wakeup source is enable or not
  \return      error code
*/
csi_error_t soc_pm_config_wakeup_source(uint32_t wakeup_num, bool enable)
{
    csi_error_t ret = CSI_OK;

    if (wakeup_num > 3U) {
        ret = CSI_ERROR;
    } else {

        if (enable) {
            csi_vic_set_wakeup_irq(WJ_PMU_IRQn);
            PREG32(0x30000008) = 0;         //must close cpu1&2

            if (0U == wakeup_num) {
                wj_pmu_write_gpio_mask_en(PMU_REG_BASE);
            } else if (1U == wakeup_num) {
                wj_pmu_write_aontim_mask_en(PMU_REG_BASE);
            } else if (2U == wakeup_num) {
                wj_pmu_write_rtc_mask_en(PMU_REG_BASE);
            } else if (3U == wakeup_num) {
                wj_pmu_write_codec_wt_mask_en(PMU_REG_BASE);
            }
        } else {
            csi_vic_clear_wakeup_irq(WJ_PMU_IRQn);
            PREG32(0x30000008) = 3;

            if (0U == wakeup_num) {
                wj_pmu_write_gpio_mask_dis(PMU_REG_BASE);
            } else if (1U == wakeup_num) {
                wj_pmu_write_aontim_mask_dis(PMU_REG_BASE);
            } else if (2U == wakeup_num) {
                wj_pmu_write_rtc_mask_dis(PMU_REG_BASE);
            } else if (3U == wakeup_num) {
                wj_pmu_write_codec_wt_mask_dis(PMU_REG_BASE);
            }
        }
    }

    return ret;
}

#endif
