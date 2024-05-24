/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <soc.h>
#include <csi_config.h>
#include <sys_clk.h>
#include <drv/common.h>
#include <drv/irq.h>
#include <drv/tick.h>
#include <drv/porting.h>
#include <drv/timer.h>

#define __WEAK         __attribute__((weak))

static volatile uint32_t csi_tick = 0U;
static volatile uint32_t last_time_ms = 0U;
static volatile uint64_t last_time_us = 0U;
static volatile uint64_t timer_init_value = 0U;

#if CONFIG_CPU_E9XX
static csi_dev_t tick_dev;
#endif

void csi_tick_increase(void)
{
    csi_tick++;
}

uint32_t csi_tick_get(void)
{
    return csi_tick;
}

void tick_irq_handler(void *arg)
{
    csi_tick_increase();
    csi_coret_config((soc_get_coretim_freq() / CONFIG_SYSTICK_HZ), CORET_IRQn);
#if CONFIG_AOS_OSAL
    extern void aos_sys_tick_handler(void);
    aos_sys_tick_handler();
#else
#ifdef CONFIG_KERNEL_FREERTOS
    extern void xPortSysTickHandler(void);
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_RTTHREAD)
    extern void rt_tick_increase(void);
    rt_tick_increase();
#else
#endif
#endif /* end CONFIG_AOS_OSAL */
}

csi_error_t csi_tick_init(void)
{
#if CONFIG_CPU_E9XX
    tick_dev.irq_num = CORET_IRQn;
    csi_vic_set_prio(tick_dev.irq_num, 31U);
    csi_irq_attach(tick_dev.irq_num, &tick_irq_handler, &tick_dev);
#endif

    csi_tick = 0U;
    timer_init_value = csi_coret_get_value2();
    csi_coret_reset_value2();
    csi_coret_config((soc_get_coretim_freq() / CONFIG_SYSTICK_HZ), CORET_IRQn);
    csi_coret_irq_enable();

    return CSI_OK;
}

void csi_tick_uninit(void)
{
    csi_coret_irq_disable();
#if CONFIG_CPU_E9XX
    csi_irq_detach(tick_dev.irq_num);
#endif
}

uint32_t csi_tick_get_ms(void)
{
    uint32_t time;

    time = (uint32_t)((csi_coret_get_value2() - timer_init_value) * 1000U / (uint64_t)soc_get_coretim_freq());
    last_time_ms = time;
    return time;
}

uint64_t csi_tick_get_us(void)
{
    uint64_t time;

    time = (csi_coret_get_value2() - timer_init_value) * 1000U * 1000U / (uint64_t)soc_get_coretim_freq();
    last_time_us = time;
    return time;
}

static void _mdelay(void)
{
    uint64_t start = csi_coret_get_value2();
    uint64_t cur;
    uint32_t cnt = (soc_get_coretim_freq() / 1000U);

    while (1) {
        cur = csi_coret_get_value2();

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
    uint64_t cur;
    uint64_t start = csi_coret_get_value2();
    uint32_t cnt = (soc_get_coretim_freq() / 1000U / 100U);

    while (1) {
        cur = csi_coret_get_value2();

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

