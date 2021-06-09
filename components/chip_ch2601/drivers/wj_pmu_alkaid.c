/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_pmu_alkaid.c
 * @brief    CSI Source File for PMU Driver
 * @version  V1.0
 * @date     19. Feb 2020
 ******************************************************************************/
#include <csi_config.h>
#include <stdio.h>
#include <string.h>
#ifdef CONFIG_TEE_CA
#include <drv_tee.h>
#endif
#ifdef CONFIG_PM
#include <drv/io.h>
#include <drv/pm.h>
#include <drv/timer.h>
#include <soc.h>
#include <wj_pmu_alkaid.h>
#include <cd_qspi_ll.h>
#include <wj_ioctl_ll.h>
#include <dw_gpio_ll.h>
#define CONFIG_VIC_WAKEUP_BIT       31U
#define CSKY_SOC_WAKEUP_REG0        (WJ_IOC_BASE + 0x18)
#define CSKY_SOC_WAKEUP_LEVEL       (WJ_IOC_BASE + 0x78)

#define WAKEUP_IO       PA4
#ifdef CONFIG_CHIP_DANICA
#undef CONFIG_TEE_CA
#endif

typedef enum {
    WAIT_MODE = 0U,
    DOZE_MODE,
    STOP_MODE,
    STANDBY_MODE,
    SLEEP_MODE
} lpm_mode_e;

extern int32_t arch_do_cpu_save(void);
extern int32_t arch_do_cpu_resume(void);
extern int32_t arch_resume_context(void);

extern csi_timer_t tick_timer;

#ifndef CONFIG_TEE_CA
#define CONFIG_PMU_REGISTER_NUM_SAVE  19U
static uint32_t pmu_regs_saved[CONFIG_PMU_REGISTER_NUM_SAVE];
#endif

#define CONFIG_CSITIMER_REGISTER_NUM_SAVE    2U
static uint32_t csitimer_regs_saved[CONFIG_CSITIMER_REGISTER_NUM_SAVE];

#define CONFIG_CLIC_REGISTER_NUM_SAVE    (CONFIG_IRQ_NUM + 1U)
static uint32_t clic_regs_saved[CONFIG_CLIC_REGISTER_NUM_SAVE];

#define CONFIG_CPU_REGISTER_NUM_SAVE    60U
uint32_t g_arch_cpu_saved[CONFIG_CPU_REGISTER_NUM_SAVE];

#define CONFIG_IOCTRL_REGISTER_NUM_SAVE 19U
uint32_t io_regs_saved[CONFIG_IOCTRL_REGISTER_NUM_SAVE];
/* Driver Capabilities */

static void do_prepare_sleep_action(void)
{
    uint8_t i;
#ifndef CONFIG_TEE_CA
    wj_pmu_reg_t *pbase = (wj_pmu_reg_t *)WJ_PMU_BASE;

    pmu_regs_saved[0] = pbase->DFCC;
    pmu_regs_saved[1] = pbase->PCR;
    pmu_regs_saved[2] = pbase->WIME0;
    pmu_regs_saved[3] = pbase->SRC_CLKMD;
    pmu_regs_saved[4] = pbase->PDU_DCU_MODE;
    pmu_regs_saved[5] = pbase->MCLK_RATIO;

#endif

    /* save clic register */
    clic_regs_saved[0] = CLIC->CLICCFG;

    for (i = 0U; i < CONFIG_IRQ_NUM; i++) {
        clic_regs_saved[i + 1U] = *((volatile uint32_t *)&CLIC->CLICINT[i]);
    }

    /* save the timer0 register */
    volatile uint32_t *ibase = (uint32_t *)(DW_TIMER0_BASE);
    csitimer_regs_saved[0] = *((volatile uint32_t *)ibase);
    csitimer_regs_saved[1] = *((volatile uint32_t *)(ibase + 8U));

    /* save the ioctrl register */
    for (i = 0U; i < sizeof(io_regs_saved) / 4U; i++) {
        if (i < 17U) {
            io_regs_saved[i] = *((volatile uint32_t *)ibase + i);
        } else if (i == 17U) {
            io_regs_saved[i] = *((volatile uint32_t *)(DW_GPIO_BASE + 0x8U));
        }
    }
}

static void do_wakeup_sleep_action(void)
{
    uint8_t i;

#ifndef CONFIG_TEE_CA
    wj_pmu_reg_t *pbase = (wj_pmu_reg_t *)WJ_PMU_BASE;

    if (pbase->PDU_DCU_MODE & 0xd80U) {
        pbase->PDU_DCU_MODE = pmu_regs_saved[4];
        pbase->DFCC = 1U;

        while (pbase->DFCC & 0x1U);
    }

    pbase->PCR = pmu_regs_saved[1];
    pbase->WIME0 = pmu_regs_saved[2];
    pbase->SRC_CLKMD = pmu_regs_saved[3] & 0x3f00U;
    pbase->MODE_CHG_EN = 2U;

    while (pbase->MODE_CHG_EN);

    while ((pbase->CLKSTBR & 0x8U) != 0x8U);

    pbase->SRC_CLKMD = pmu_regs_saved[3] & 0xff00U;
    pbase->MODE_CHG_EN = 2U;

    while (pbase->MODE_CHG_EN);

    while ((pbase->CLKSTBR & 0x10U) != 0x10U);

    pbase->SRC_CLKMD = pmu_regs_saved[3];
    pbase->MODE_CHG_EN = 2U;

    while (pbase->MODE_CHG_EN);

    pbase->MCLK_RATIO = pmu_regs_saved[5] & 0xfU;
    pbase->MCLK_RATIO = pmu_regs_saved[5];

    while (pbase->MCLK_RATIO & 0x10U);

#endif

    /* resume clic register */
    CLIC->CLICCFG = clic_regs_saved[0];

    for (i = 0U; i < CONFIG_IRQ_NUM; i++) {
        *((volatile uint32_t *)&CLIC->CLICINT[i]) = clic_regs_saved[i + 1U];
    }

    /* resume the ioctrl register */
    volatile uint32_t *ibase = (uint32_t *)(DW_GPIO_BASE + 0x100U);

    for (i = 0U; i < sizeof(io_regs_saved) / 4U; i++) {
        if (i < 17U) {
            *((volatile uint32_t *)ibase + i) = io_regs_saved[i];
        } else if (i == 17U) {
            *((volatile uint32_t *)(DW_GPIO_BASE + 0x8)) = io_regs_saved[i];
        }
    }

    /* resume the timer0 register */
    volatile uint32_t *time_ibase = (uint32_t *)(DW_TIMER0_BASE);
    *((volatile uint32_t *)time_ibase) = csitimer_regs_saved[0];
    *((volatile uint32_t *)(time_ibase + 8U)) = csitimer_regs_saved[1];
}

static void pmu_action(csi_pm_dev_action_t action)
{
    switch (action) {
        case PM_DEV_SUSPEND:
            do_prepare_sleep_action();
            break;

        case PM_DEV_RESUME:
            do_wakeup_sleep_action();
            break;

        default:
            return;
    }
}

#ifdef CONFIG_TEE_CA
void resume_context_from_stop_mode(void)
{
    arch_do_cpu_resume();
}
#endif

#ifndef CONFIG_TEE_TA
static void set_resume_func(wj_pmu_reg_t *pmu_base, uint32_t *func)
{
    pmu_base->USER_DEFINE[0] = (uint32_t)func;
}
#endif

__attribute__((section(".ram.code"))) void soc_sleep(lpm_mode_e mode)
{
#ifdef CONFIG_TEE_CA
    tee_lpm_mode_e lpm_mode = 0U;

    if (mode == WAIT_MODE) {
        lpm_mode = TEE_LPM_MODE_WAIT;
    } else if (mode == DOZE_MODE) {
        lpm_mode = TEE_LPM_MODE_DOZE;
    } else if (mode == STOP_MODE) {
        lpm_mode = TEE_LPM_MODE_STOP;
    } else if (mode == STANDBY_MODE) {
        lpm_mode = TEE_LPM_MODE_STANDBY;
    } else {
        lpm_mode = TEE_LPM_MODE_WAIT;
    }

    csi_tee_enter_lpm((uint32_t)g_arch_cpu_saved, 0U, lpm_mode);

    if (mode == STOP_MODE) {
        resume_context_from_stop_mode();
    }

#else
    wj_pmu_reg_t *pmu_base = (wj_pmu_reg_t *)WJ_PMU_BASE;

    set_resume_func(pmu_base, (uint32_t *)&arch_resume_context);

    /* clear pend IRQ */
    for (uint8_t i = 0; i < 64; i++) {
        CLIC->CLICINT[i].IP = 0;
    }

    cd_qspi_regs_t *qspi_base = (cd_qspi_regs_t *)CD_QSPI_BASE;
    wj_ioctl_regs_t *ioctl_base = (wj_ioctl_regs_t *)WJ_IOC_BASE;
    dw_gpio_regs_t *gpio_base = (dw_gpio_regs_t*)DW_GPIO_BASE;

    ioctl_base->IO_PD0 = 0xffffffff;
    gpio_base->SWPORTA_DDR = 0;      ///< all gpio is input
    /* spiflash powerdown mode*/
    // while ((qspi_base->CFGR & (1 << 31)) == 0);
    // qspi_base->FCCR = 0xb9000001;
    // while (qspi_base->FCCR & 0x2);
    // while ((qspi_base->CFGR & (1 << 31)) == 0);

    ioctl_base->IO_QSPIDRV = 0X20;
    ioctl_base->IO_IIS456 = 0X2aa;
    wj_usb_phy_power_off(pmu_base);
    wj_codec_power_off(pmu_base);

    /* disable_els */
    uint32_t reg = pmu_base->SRC_CLKMD;
    reg &= ~(WJ_SRC_CLKMD_EHS_IE | WJ_SRC_CLKMD_EHS_OE | WJ_SRC_CLKMD_ILS_EN);
    pmu_base->SRC_CLKMD = reg;
    pmu_base->MODE_CHG_EN |= WJ_MODE_CHG_EN_DCU_MD_EN;

    while (pmu_base->MODE_CHG_EN & WJ_MODE_CHG_EN_DCU_MD_EN);

    if (mode == STOP_MODE) {
        wj_isram0_power_on_in_stop_mode(pmu_base);
        wj_isram1_power_on_in_stop_mode(pmu_base);
        wj_pmu_lp_stop_mode_enable(pmu_base);
        wj_pmu_allow_lp_mode(pmu_base);
        arch_do_cpu_save();
    } else if (mode == STANDBY_MODE) {
        soc_dcache_clean_invalid_all();
        wj_pmu_lp_standby_mode_enable(pmu_base);
        wj_pmu_allow_lp_mode(pmu_base);
        __STOP();
    }

#endif
}

/**
  \brief       choose the pmu mode to enter
  \param[in]   handle  pmu handle to operate.
  \param[in]   mode    \ref pmu_mode_e
  \return      error code
*/
csi_error_t soc_pm_enter_sleep(csi_pm_mode_t mode)
{
    csi_error_t ret = CSI_OK;

    switch (mode) {
        case PM_MODE_RUN:
            break;

        case PM_MODE_SLEEP_1:
            ret = CSI_UNSUPPORTED;
            break;

        case PM_MODE_SLEEP_2:
            ret = CSI_UNSUPPORTED;
            break;

        case PM_MODE_DEEP_SLEEP_1:
            pmu_action(PM_DEV_SUSPEND);
            soc_sleep(STOP_MODE);
            pmu_action(PM_DEV_RESUME);
            break;

        case PM_MODE_DEEP_SLEEP_2:
            pmu_action(PM_DEV_SUSPEND);
            soc_sleep(STANDBY_MODE);
            pmu_action(PM_DEV_RESUME);
            break;

        default:
            ret = CSI_ERROR;
    }

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

    if (wakeup_num >= 35U) {
        ret = CSI_ERROR;
    } else {

        wj_pmu_reg_t *pmu_base = (wj_pmu_reg_t *)WJ_PMU_BASE;

        if (enable) {
            csi_vic_set_wakeup_irq(CONFIG_VIC_WAKEUP_BIT);

            if (wakeup_num == WJ_IOCTL_Wakeupn) {
                putreg32(1U << WAKEUP_IO, (uint32_t *)CSKY_SOC_WAKEUP_REG0);
                putreg32(0U, (uint32_t *)CSKY_SOC_WAKEUP_LEVEL);
            }

            if (wakeup_num < 32U) {
                wj_set_wakeup_source0(pmu_base, wakeup_num);
            }
        } else {
            csi_vic_clear_wakeup_irq(CONFIG_VIC_WAKEUP_BIT);

            if (wakeup_num < 32U) {
                wj_clear_wakeup_source0(pmu_base, wakeup_num);
            }
        }

    }

    return ret;
}
#endif
