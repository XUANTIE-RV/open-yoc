/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <string.h>
#include <syscfg.h>
#include <sysctrl.h>
#include <soc.h>
#include <csi_core.h>
#include <drv/gpio.h>
#include "codec_adrb.h"
#include <silan_pmu.h>
#include "silan_adev.h"
#include "sram_printf.h"
#include "silan_voice_adc.h"
#include "silan_syscfg_regs.h"

typedef enum {
    LPM_POLICY_NO_POWER_SAVE,
    LPM_POLICY_LOW_POWER,
    LPM_POLICY_DEEP_SLEEP,
} pm_policy_t;


#define CTR_SDR_PD_DSLEEP_REQ_Pos      (2U)
#define CTR_SDR_PD_DSLEEP_REQ_Msk      (0x1U << CTR_SDR_PD_DSLEEP_REQ_Pos)
#define CTR_SDR_PD_DSLEEP_ACK_Pos      (6U)
#define CTR_SDR_PD_DSLEEP_ACK_Msk      (0x1U << CTR_SDR_PD_DSLEEP_ACK_Pos)


#define __REG32(addr)             (*(volatile uint32_t*)(addr))

extern int32_t g_sram_tick;
extern int32_t g_sram_tick_conv;
extern void (*g_sram_vector[])(void);
extern uint32_t *g_sram_read_ptr;
extern uint32_t *g_sram_write_ptr;
extern codec_adrb_t g_sram_mic_ring;
void sdram_self_refresh_enter(void);
void sdram_self_refresh_exit(void);
void sram_silan_halt_dsp(void);
void sram_silan_run_dsp(void);
// void sram_dsp_cclk_onoff(int onoff);
void sram_reboot(void);
void sdram_flash_dsleep_mode_set(void);

extern int32_t sram_drv_gpio_input_init(int32_t gpio_pin);

SRAM_DATA vad_alg_t g_vad_alg = NULL;
SRAM_DATA int32_t g_rtc_irq_ret = 0;

SRAM_DATA uint32_t g_sram_clock = 160000000;

SRAM_TEXT void sram_rtc_irq_set(int32_t ret)
{
    g_rtc_irq_ret = ret;
}

SRAM_TEXT void sram_codec2_cclk_off(void)
{
    __REG32(SILAN_SYSCFG_REG2) &= ~(1 << 24); //clk disable
}
SRAM_TEXT void sram_deep_sleep_enter(void)
{
    silan_codec2_pwd_deal_save(NULL);
    sram_codec2_cclk_off();

    csi_vic_clear_pending_irq(GPIO2_IRQn);
    csi_vic_set_wakeup_irq(GPIO2_IRQn);

    csi_vic_set_wakeup_irq(RTC_IRQn);
    asm("wait");

    __disable_irq();
#if PRINT_DEBUG_EN == 0
    silan_set_sys_freq(SYSPLL_160M);
#endif
    sram_reboot();
}

SRAM_TEXT void sram_reboot(void)
{
    uint32_t sys_ctr2;

    __disable_irq();
    silan_sysctl_reg_t *sysctl = (silan_sysctl_reg_t *)SILAN_SYSCTRL_BASE;

    sysctl->SYS_LOCK = SYS_LOCK_UNLOCK_MAGIC;
    sysctl->WDT_TG = 5000;

    sys_ctr2 = sysctl->SYS_CTR2;
    sys_ctr2 |= SYS_CTL2_WDT_RESET_EN_Msk;
    sysctl->SYS_CTR2 = sys_ctr2;

    sysctl->SYS_LOCK = SYS_LOCK_LOCK_MAGIC;

    while(1);
}

SRAM_TEXT void sram_main(void *arg)
{
    int32_t clocks0, clocks1;
    pm_policy_t policy = (pm_policy_t)arg;

    __set_VBR((uint32_t)g_sram_vector);
    g_sram_tick = 0;
    csi_vic_disable_irq(10);

#if PRINT_DEBUG_EN == 0
    silan_set_sys_freq(SYSPLL_12M);
    g_sram_clock = 12000000;
#endif

    if (CORET->CTRL & (0x1 << 16)) {
        clocks0 = CORET->LOAD + (CORET->LOAD - CORET->VAL);
    } else {
        clocks0 = CORET->LOAD - CORET->VAL;
    }

    __enable_irq();

    sram_silan_halt_dsp();
    //Don't call in low power mode, DSP will unstable
    //sram_dsp_cclk_onoff(0);

    if(policy == LPM_POLICY_DEEP_SLEEP){
        sdram_flash_dsleep_mode_set();
        sram_deep_sleep_enter();
    } else {
#if SDRAM_ACCESS_EN == 0
        sdram_self_refresh_enter();
#endif
        while (1) {
            if (g_vad_alg){
                int vad_alg_ret = g_vad_alg();
                if (vad_alg_ret || g_rtc_irq_ret) {
                    sram_rtc_irq_set(0);
                    break;
                }
            } else {
                break;
            }
        }
    }
    __disable_irq();

    clocks1 = CORET->LOAD - CORET->VAL;
    g_sram_tick_conv = g_sram_tick * 160 / (g_sram_clock / 1000000)
        + (clocks1 - clocks0) * 160 / (g_sram_clock / 1000000) / CORET->LOAD;
#if PRINT_DEBUG_EN == 0
    silan_set_sys_freq(SYSPLL_160M);
#endif

#if SDRAM_ACCESS_EN == 0
    sdram_self_refresh_exit();
#endif
    //sram_dsp_cclk_onoff(1);
    sram_silan_run_dsp();

    csi_vic_enable_irq(10);
}

SRAM_TEXT __attribute__((naked)) void jump_to_sram(void *arg)
{
    asm("mfcr    r13, psr\n\r"
        "psrclr  ie\n\r"
        "mov     r12, sp\n\r"
        "lrw     sp, 0x20008000\n\r"
        "subi    sp, 16\n\r"
        "stw     r12, (sp)\n\r"
        "stw     r15, (sp, 4)\n\r"
        "mfcr    r12, vbr\n\r"
        "stw     r12, (sp, 8)\n\r"
        "stw     r13, (sp, 12)\n\r"
        "jbsr    sram_main\n\r"
        "ldw     r12, (sp, 12)\n\r"
        "mtcr    r12, epsr\n\r"
        "ldw     r12, (sp, 8)\n\r"
        "mtcr    r12, vbr\n\r"
        "ldw     r15, (sp, 4)\n\r"
        "mtcr    r15, epc\n\r"
        "ldw     r12, (sp)\n\r"
        "mov     sp, r12\n\r"
        "rte\n\r");
}

SRAM_TEXT void sdram_flash_dsleep_mode_set(void)
{
    csi_dcache_clean_invalid();
    csi_icache_invalid();
    csi_icache_disable();
    csi_dcache_disable();

    silan_syscfg_reg_t *syscfg = (silan_syscfg_reg_t *)SILAN_SYSCFG_BASE;
    syscfg->CTR_SDR_PD |= CTR_SDR_PD_DSLEEP_REQ_Msk;
    while (!(syscfg->CTR_SDR_PD & (CTR_SDR_PD_DSLEEP_ACK_Msk)));
}


SRAM_TEXT void sdram_self_refresh_enter(void)
{
    csi_dcache_clean_invalid();
    csi_icache_invalid();
    csi_icache_disable();
    csi_dcache_disable();

    silan_syscfg_reg_t *syscfg = (silan_syscfg_reg_t *)SILAN_SYSCFG_BASE;
    syscfg->CTR_SDR_PD |= CTR_SDR_PD_SREF_REQ_Msk;
    while (!(syscfg->CTR_SDR_PD & CTR_SDR_PD_SREF_ACK_Msk));
}

SRAM_TEXT void sdram_self_refresh_exit(void)
{
    silan_syscfg_reg_t *syscfg = (silan_syscfg_reg_t *)SILAN_SYSCFG_BASE;
    syscfg->CTR_SDR_PD &= ~CTR_SDR_PD_SREF_REQ_Msk;
    while (syscfg->CTR_SDR_PD & CTR_SDR_PD_SREF_ACK_Msk);

    csi_icache_enable();
    csi_dcache_enable();
}

SRAM_TEXT void sram_silan_halt_dsp(void)
{
    silan_syscfg_reg_t *syscfg = (silan_syscfg_reg_t *)SILAN_SYSCFG_BASE;
    syscfg->CTR_SOC0 |= CTR_SOC0_RUN_STALL_Msk;
}

SRAM_TEXT void sram_silan_run_dsp(void)
{
    silan_syscfg_reg_t *syscfg = (silan_syscfg_reg_t *)SILAN_SYSCFG_BASE;
    syscfg->CTR_SOC0 &= ~CTR_SOC0_RUN_STALL_Msk;
}

// SRAM_TEXT void sram_dsp_cclk_onoff(int onoff)
// {
//     //silan_syscfg_reg_t *syscfg = (silan_syscfg_reg_t *)SILAN_SYSCFG_BASE;
//     //syscfg->CTR_SOC0 |= CTR_SOC0_RUN_STALL_Msk;

//     if (onoff) {
//         __REG32(SILAN_SYSCFG_REG12) |= (1 << 3);
//     } else {
//         __REG32(SILAN_SYSCFG_REG12) &= ~(1 << 3);
//     }
// }
