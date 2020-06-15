/*
 * Copyright (C) 2020 FishSemi Holding Limited
 */
/******************************************************************************
 * @file     ck_pmu.c
 * @brief    CSI Source File for PMU Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <drv/pmu.h>
#include <drv/eflash.h>
#include <ck_pmu.h>
#include <soc.h>

#include "drv/irq.h"

#define ERR_PMU(errno) (CSI_DRV_ERRNO_PMU_BASE | errno)

#define TOP_PWR_BASE                CSKY_PWR_BASE
#define TOP_PWR_AP_UNIT_PD_CTL      (TOP_PWR_BASE + 0x204)
#define TOP_PWR_CK802_CTL0          (TOP_PWR_BASE + 0x22c)
#define TOP_PWR_SLPCTL_AP_M4        (TOP_PWR_BASE + 0x360)

#define TOP_PWR_AP_M4_PD_MK         (1 << 3)
#define TOP_PWR_AP_M4_AU_PU_MK      (1 << 6)
#define TOP_PWR_AP_M4_AU_PD_MK      (1 << 7)
#define TOP_PWR_AP_M4_PD_CPU_SEL    (1 << 8)

#define TOP_PWR_SLP_LPMD_B_MK       (1 << 12)
#define TOP_PWR_CK802_SLP_EN        (1 << 14)

#define TOP_PWR_AP_M4_SLP_EN        (1 << 0)
#define TOP_PWR_AP_M4_SLP_MK        (1 << 1)
#define TOP_PWR_AP_M4_DS_SLP_EN     (1 << 2)

#define putreg32(v,a)        (*(volatile uint32_t *)(a) = (v))
#define getreg32(a)          (*(volatile uint32_t *)(a))

void modifyreg32(unsigned int addr, uint32_t clearbits, uint32_t setbits)
{
    uint32_t   regval;

    regval  = getreg32(addr);
    regval &= ~clearbits;
    regval |= setbits;
    putreg32(regval, addr);
}

static void U1RXD_IRQHandler(void)
{
}

pmu_handle_t csi_pmu_initialize(int32_t idx, pmu_event_cb_t cb_event)
{
    /* Mask AP M4 effect to DS */

    putreg32(TOP_PWR_AP_M4_SLP_MK << 16 |
            TOP_PWR_AP_M4_SLP_MK, TOP_PWR_SLPCTL_AP_M4);

    /* Unmask CK802 LPMD_B effect to DS */

    modifyreg32(TOP_PWR_CK802_CTL0, TOP_PWR_SLP_LPMD_B_MK, 0);

    /* Which CPU should watch when set core power down, 1 = CK802 */

    putreg32(TOP_PWR_AP_M4_PD_CPU_SEL << 16 |
            TOP_PWR_AP_M4_PD_CPU_SEL, TOP_PWR_AP_UNIT_PD_CTL);

    /* Forbid the AP power down, AP will power down following SP */

    putreg32(TOP_PWR_AP_M4_AU_PD_MK << 16 |
            TOP_PWR_AP_M4_AU_PD_MK, TOP_PWR_AP_UNIT_PD_CTL);

    drv_irq_register(SLP_U1RXD_ACT_IRQn, U1RXD_IRQHandler);
    drv_irq_enable(SLP_U1RXD_ACT_IRQn);

    return 0;
}

static void up_cpu_lp(bool pwr_sleep, bool ds_sleep)
{
    /* Allow PWR_SLEEP (VDDMAIN ON)? */

    if (pwr_sleep)
        modifyreg32(TOP_PWR_CK802_CTL0, 0, TOP_PWR_CK802_SLP_EN);
    else
        modifyreg32(TOP_PWR_CK802_CTL0, TOP_PWR_CK802_SLP_EN, 0);

    /* Allow DS_SLEEP (VDDMAIN OFF)? */

    if (ds_sleep)
        putreg32(TOP_PWR_AP_M4_DS_SLP_EN << 16 |
                TOP_PWR_AP_M4_DS_SLP_EN, TOP_PWR_SLPCTL_AP_M4);
    else
        putreg32(TOP_PWR_AP_M4_DS_SLP_EN << 16, TOP_PWR_SLPCTL_AP_M4);
}

int32_t csi_pmu_enter_sleep(pmu_handle_t handle, pmu_mode_e mode)
{
    switch (mode) {
        case PMU_MODE_RUN:
            break;

        case PMU_MODE_SHUTDOWN:
            return ERR_PMU(DRV_ERROR_UNSUPPORTED);

        case PMU_MODE_SLEEP:
            up_cpu_lp(false, false);
            __WFI();
            break;

        case PMU_MODE_DOZE:
            up_cpu_lp(false, false);
            __DOZE();
            break;

        case PMU_MODE_DORMANT:
            up_cpu_lp(true, false);
            __STOP();
            break;

        case PMU_MODE_STANDBY:
            up_cpu_lp(true, true);
            __STOP();
            break;

        default:
            return ERR_PMU(DRV_ERROR_PARAMETER);
    }
    return 0;
}
