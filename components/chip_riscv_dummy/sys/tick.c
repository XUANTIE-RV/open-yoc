/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     tick.c
 * @brief    Source File for tick
 * @version  V1.0
 * @date     7. April 2020
 ******************************************************************************/

#include <soc.h>
#include <csi_config.h>
#include <sys_clk.h>
#include <drv/common.h>
#include <drv/irq.h>
#include <drv/tick.h>
#include <drv/porting.h>
#include <drv/timer.h>

#define __WEAK         __attribute__((weak))

static csi_dev_t tick_dev;

static volatile uint32_t csi_tick = 0U;
static volatile uint32_t last_time_ms = 0U;
static volatile uint64_t last_time_us = 0U;
static volatile uint64_t timer_init_value = 0U;

void csi_tick_increase(void)
{
    csi_tick++;
}

void csi_tick_uninit(void)
{
    csi_irq_disable(tick_dev.irq_num);
    csi_irq_detach(tick_dev.irq_num);
}

uint32_t csi_tick_get(void)
{
    return csi_tick;
}

#if __riscv_xlen == 64
static void tick_irq_handler(csi_timer_t *timer_handle, void *arg)
{
    csi_tick_increase();
    csi_clint_config(CORET_BASE, (soc_get_coretim_freq()/ CONFIG_SYSTICK_HZ), CORET_IRQn);
#ifndef CONFIG_KERNEL_NONE
    extern void aos_sys_tick_handler(void);
    aos_sys_tick_handler();
#endif
}

csi_error_t csi_tick_init(void)
{
    CLINT_Type *clint = (CLINT_Type *)CORET_BASE;

    csi_tick = 0U;
    tick_dev.irq_num = (uint8_t)CORET_IRQn;
    timer_init_value = csi_clint_get_value();
    csi_plic_set_prio(PLIC_BASE, CORET_IRQn, 31U);
    csi_irq_attach((uint32_t)tick_dev.irq_num, &tick_irq_handler, &tick_dev);
#if defined(CONFIG_RISCV_SMODE) && CONFIG_RISCV_SMODE
    clint->STIMECMPH0 = 0;
    clint->STIMECMPL0 = 0;
#else
    clint->MTIMECMPH0 = 0;
    clint->MTIMECMPL0 = 0;
#endif
    csi_clint_config(CORET_BASE, (soc_get_coretim_freq() / CONFIG_SYSTICK_HZ), CORET_IRQn);
    csi_irq_enable((uint32_t)tick_dev.irq_num);

    return CSI_OK;
}

uint32_t csi_tick_get_ms(void)
{
    uint32_t time;

    time = (uint32_t)((csi_clint_get_value() - timer_init_value) * 1000U / (uint64_t)soc_get_coretim_freq());
    last_time_ms = time;
    return time;
}

uint64_t csi_tick_get_us(void)
{
    uint64_t time;

    time = (csi_clint_get_value() - timer_init_value) * 1000U * 1000U / (uint64_t)soc_get_coretim_freq();
    last_time_us = time;
    return time;
}

static void _mdelay(void)
{
    uint64_t start = csi_clint_get_value();
    uint64_t cur;
    uint32_t cnt = (soc_get_coretim_freq() / 1000U);

    while (1) {
        cur = csi_clint_get_value();

        if (start > cur) {
            if ((start - cur) >= cnt) {
                break;
            }
        } else {
            if (cur - start >= cnt) {
                break;
            }
        }
    }
}

static void _10udelay(void)
{
    uint64_t start = csi_clint_get_value();
    uint32_t cnt = (soc_get_coretim_freq() / 1000U / 100U);

    while (1) {
        uint64_t cur = csi_clint_get_value();

        if (start > cur) {
            if ((start - cur) >= cnt) {
                break;
            }
        } else {
            if (cur - start >= cnt) {
                break;
            }
        }
    }
}

#else

static void tick_irq_handler(void *arg)
{
    csi_tick_increase();
    csi_coret_config(soc_get_coretim_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);
#ifndef CONFIG_KERNEL_NONE
    extern void aos_sys_tick_handler(void);
    aos_sys_tick_handler();
#endif
}

csi_error_t csi_tick_init(void)
{
    tick_dev.irq_num = CORET_IRQn;
    timer_init_value = ((uint64_t)csi_coret_get_valueh() << 32U) | csi_coret_get_value();
    csi_vic_set_prio(CORET_IRQn, 31U);
    csi_irq_attach(tick_dev.irq_num, &tick_irq_handler, &tick_dev);
    CORET->MTIMECMP = 0;
    csi_coret_config((soc_get_coretim_freq() / CONFIG_SYSTICK_HZ), CORET_IRQn);
    csi_irq_enable(tick_dev.irq_num);
    return CSI_OK;
}

uint32_t csi_tick_get_ms(void)
{
    uint32_t time;
    time = ((((uint64_t)csi_coret_get_valueh() << 32U) | csi_coret_get_value()) - timer_init_value) / (soc_get_coretim_freq() / 1000U);
    last_time_ms = time;
    return time;
}

uint64_t csi_tick_get_us(void)
{
    uint64_t time;
    time = ((((uint64_t)csi_coret_get_valueh() << 32U) | csi_coret_get_value()) - timer_init_value) / (soc_get_coretim_freq() / 1000000U);
    last_time_us = time;
    return time;
}

void _mdelay(void)
{
    unsigned long long start, cur, delta;
    uint32_t startl = csi_coret_get_value();
    uint32_t starth = csi_coret_get_valueh();
    uint32_t curl, curh;
    uint32_t cnt = (soc_get_coretim_freq() / 1000U);
    start = ((unsigned long long)starth << 32U) | startl;

    while (1) {
        curl = csi_coret_get_value();
        curh = csi_coret_get_valueh();
        cur = ((unsigned long long)curh << 32U) | curl;
        delta = cur - start;

        if (delta >= cnt) {
            return;
        }
    }
}

static void _10udelay(void)
{
    unsigned long long start, cur;
    uint32_t startl = csi_coret_get_value();
    uint32_t starth = csi_coret_get_valueh();
    uint32_t curl, curh;
    uint32_t cnt = (soc_get_coretim_freq() / 1000U / 100U);
    start = ((unsigned long long)starth << 32U) | startl;

    while (1) {
        curl = csi_coret_get_value();
        curh = csi_coret_get_valueh();
        cur = ((unsigned long long)curh << 32U) | curl;
        if (start > cur) {
            if ((start - cur) >= cnt) {
                break;
            }
        } else {
            if (cur - start >= cnt) {
                break;
            }
        }
    }
}
#endif /*__riscv_xlen*/

__WEAK void mdelay(uint32_t ms)
{
    while (ms) {
        ms--;
        _mdelay();
    }
}

/**
 * Ps: At least delay over 10us
*/
void udelay(uint32_t us)
{
    us /= 10U;

    while (us) {
        us--;
        _10udelay();
    }
}

