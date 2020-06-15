/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <soc.h>
#include <drv/rtc.h>
#include <drv/pmu.h>

#include <aos/list.h>
#include <aos/types.h>
#include <yoc/lpm.h>
#include <syscfg.h>
#include <sysctrl.h>

#include "silan_voice_adc.h"
#include "silan_pmu.h"

/* Constants identifying power state categories */
#define SYS_PM_ACTIVE_STATE     0 /* SOC and CPU are in active state */
#define SYS_PM_LOW_POWER_STATE  1 /* CPU low power state */
#define SYS_PM_DEEP_SLEEP       2 /* SOC low power state */

#define SYS_PM_NOT_HANDLED      SYS_PM_ACTIVE_STATE

#define MIN_TIME_TO_SLEEP       50      //ms
#define MIN_TIME_TO_SUSPEND     10000   //ms

extern void mdelay(uint32_t ms);
extern void jump_to_sram(void *arg);
extern int32_t g_sram_tick_conv;
static uint32_t codec1_regs[102];

/******************************************
// SOC interfaces
******************************************/
void lpm_clk_shutdown()
{
    silan_syscfg_reg_t *syscfg = (silan_syscfg_reg_t *)SILAN_SYSCFG_BASE;
    syscfg->CTR_REG2 &= ~(1<<3);    //ispdif
    syscfg->CTR_REG2 &= ~(1<<11);   //sd
    syscfg->CTR_REG2 &= ~(1<<15);   //sdio
    syscfg->CTR_REG2 &= ~(1<<19);   //otp
    syscfg->CTR_REG2 &= ~(1<<23);   //codec1 cfg no effect
    syscfg->CTR_REG2 &= ~(1<<24);   //codec2 cfg no effect
    syscfg->CTR_REG2 &= ~(1<<25);   //dsp_wdog no effect
    syscfg->CTR_REG2 &= ~(1<<26);   //mcu_wdog no effect
    syscfg->CTR_REG2 &= ~(1<<27);   //risc_wdog no effect ?
    syscfg->CTR_REG3 &= ~(1<<3);    //pwm
    // syscfg->CTR_REG3 &= ~(1<<7);    //adc
    syscfg->CTR_REG3 &= ~(1<<23);   //timer
    syscfg->CTR_REG3 &= ~(1<<25);   //usbfs
    syscfg->CTR_REG3 &= ~(1<<30);   //pdp
    syscfg->CTR_REG12 &= ~(1<<14);  //dsp_hclk
    syscfg->CTR_REG12 &= ~(1<<4);   //mcu_cclk
    syscfg->CTR_REG5 &= ~(1<<6);    //ch1_bclk_en
    syscfg->CTR_REG5 &= ~(1<<7);    //ch1_mclk_en
    syscfg->CTR_REG5 &= ~(1<<14);   //ch2_bclk_en
    syscfg->CTR_REG5 &= ~(1<<15);   //ch2_mclk_en
    syscfg->CTR_REG5 &= ~(1<<22);   //ch3_bclk_en
    syscfg->CTR_REG5 &= ~(1<<23);   //ch3_mclk_en
    syscfg->CTR_REG5 &= ~(1<<30);   //ch4_bclk_en
    syscfg->CTR_REG5 &= ~(1<<31);   //ch4_mclk_en
    syscfg->CTR_REG3 &= ~(1<<30);   //pdp

    silan_sysctl_reg_t *sysctrl = (silan_sysctl_reg_t *)SILAN_SYSCTRL_BASE;
    sysctrl->AUDIO_PLL_CTRL &= ~(1<<24);    //apll
    sysctrl->AUDIO_PLL_CTRL |= 1<<20;   //apll
    sysctrl->AUDIO_PLL_CTRL |= 1<<21;   //apll
    sysctrl->AUDIO_PLL_CTRL |= 1<<22;   //apll
    sysctrl->SSCG_CTRL |= 1<<11;        //sscg
}

static void sys_disable_modules_clock(pm_ctx_t *pm_ctx)
{
    drv_clk_disable(CLK_SDMMC);
    drv_clk_disable(CLK_SDIO);
    drv_clk_disable(CLK_USBHS);
    drv_clk_disable(CLK_USBFS);
    drv_clk_disable(CLK_SDMAC);
    drv_clk_disable(CLK_OSPDIF);
    drv_clk_disable(CLK_ISPDIF);
    drv_clk_disable(CLK_I1_I2S);
    drv_clk_disable(CLK_I3_I2S);
    drv_clk_disable(CLK_O1_I2S);
    drv_clk_disable(CLK_O2_I2S);
    drv_clk_disable(CLK_O3_I2S);
    drv_clk_disable(CLK_PCM_I2S);
    drv_clk_disable(CLK_PDM);
    drv_clk_disable(CLK_SPI);
    drv_clk_disable(CLK_UART1);
#if PRINT_DEBUG_EN == 0
    drv_clk_disable(CLK_UART2); //del for printf
#endif
    drv_clk_disable(CLK_UART3);
    drv_clk_disable(CLK_UART4);
    drv_clk_disable(CLK_I2C1);
    drv_clk_disable(CLK_I2C2);
    drv_clk_disable(CLK_IIR);
#if SRAM_TIMER_EN == 0
    drv_clk_disable(CLK_TIMER);
#endif
    drv_clk_disable(CLK_PDP);
    drv_clk_disable(CLK_PWM);
    if (pm_ctx->policy == LPM_POLICY_DEEP_SLEEP) {
        drv_clk_disable(CLK_I2_I2S);
        drv_clk_disable(CLK_ADMAC);
        drv_clk_disable(CLK_CXC);
        /* PMU clk关闭后GPIO唤不醒,故保留 */
        /* drv_clk_disable(CLK_PMU); */
        drv_clk_disable(CLK_SSP);
    }

    if ((pm_ctx->lpm_dev_mask & LPM_DEV_MASK_GENERAL_ADC) == 0) {
        drv_clk_disable(CLK_ADC);
    }

    if ((pm_ctx->lpm_dev_mask & LPM_DEV_MASK_GENERAL_GPIO1) == 0) {
        drv_clk_disable(CLK_GPIO1);
    }

    if ((pm_ctx->lpm_dev_mask & LPM_DEV_MASK_GENERAL_GPIO2) == 0) {
        drv_clk_disable(CLK_GPIO2);
    }
}

static void sys_modules_clock_save(pm_ctx_t *pm_ctx, uint32_t *reg0, uint32_t *reg1)
{
    silan_syscfg_reg_t *syscfg = (silan_syscfg_reg_t *)SILAN_SYSCFG_BASE;
    *reg0 = syscfg->CTR_REG0;
    *reg1 = syscfg->CTR_REG1;

    sys_disable_modules_clock(pm_ctx);
}

static void sys_modules_clock_restore(pm_ctx_t *pm_ctx, uint32_t reg0, uint32_t reg1)
{
    silan_syscfg_reg_t *syscfg = (silan_syscfg_reg_t *)SILAN_SYSCFG_BASE;
    syscfg->CTR_REG0 = reg0;
    syscfg->CTR_REG1 = reg1;
}

int sys_soc_init(pm_ctx_t *pm_ctx)
{
    pm_config_policy(LPM_POLICY_NO_POWER_SAVE);

    return 0;
}

int sys_soc_suspend(pm_ctx_t *pm_ctx)
{
    //uint32_t ticks = pm_ctx->suspend_tick;
    uint32_t flags, reg0, reg1;

    /* 通知DSP 准备进入睡眠 */
    voice_set_wake_flag(-1);
    /* 等待dsp进入等待，再关闭clk，保证唤醒后能立即处理数据 */
    mdelay(20);
    sys_modules_clock_save(pm_ctx, &reg0, &reg1);

    flags = csi_irq_save();
    silan_codec1_pwd_deal_save(codec1_regs);
    silan_codec1_cclk_config(0);
    jump_to_sram((void *)pm_ctx->policy);
    sys_modules_clock_restore(pm_ctx, reg0, reg1);
    silan_codec1_cclk_config(1);
    silan_codec1_pwd_deal_restore(codec1_regs);
    csi_irq_restore(flags);

    /* 通知DSP 唤醒 */
    voice_set_wake_flag(1);

    return 0;
}

int sys_soc_resume(pm_ctx_t *pm_ctx, int pm_state)
{
    return g_sram_tick_conv;
}
