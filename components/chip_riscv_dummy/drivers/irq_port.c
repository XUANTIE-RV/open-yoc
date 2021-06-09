/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     irq_port.c
 * @brief    CSI Source File for irq port
 * @version  V1.0
 * @date     17. Mar 2020
 ******************************************************************************/

#include <csi_config.h>
#include <stdbool.h>
#include <csi_core.h>

void soc_irq_enable(uint32_t irq_num)
{
#ifdef CONFIG_SYSTEM_SECURE
    csi_vic_enable_sirq((int32_t)irq_num);
#else
#ifdef __arm__
    NVIC_EnableIRQ(irq_num);
#else
    csi_vic_enable_irq((int32_t)irq_num);
#endif
#endif
}

void soc_irq_disable(uint32_t irq_num)
{
#ifdef CONFIG_SYSTEM_SECURE
    csi_vic_disable_sirq((int32_t)irq_num);
#else
#ifdef __arm__
    NVIC_DisableIRQ(irq_num);
#else
    csi_vic_disable_irq((int32_t)irq_num);
#endif
#endif
}

bool soc_irq_is_enabled(uint32_t irq_num)
{
    bool ret;

#ifdef __arm__
    if (__NVIC_GetEnableIRQ(irq_num)) {
#else
    if (csi_vic_get_enabled_irq((int32_t)irq_num)) {
#endif
        ret = true;
    } else {
        ret = false;
    }

    return ret;
}

void soc_irq_priority(uint32_t irq_num, uint32_t priority)
{
#ifdef __arm__
    __NVIC_SetPriority(irq_num, priority);
#else
    csi_vic_set_prio((int32_t)irq_num, priority);
#endif
}

void soc_irq_enable_wakeup(uint32_t irq_num)
{
#ifdef __arm__
#else
    csi_vic_set_wakeup_irq((int32_t)irq_num);
#endif
}

void soc_irq_disable_wakeup(uint32_t irq_num)
{
#ifdef __arm__
#else
    csi_vic_clear_wakeup_irq((int32_t)irq_num);
#endif
}

uint32_t soc_irq_get_irq_num(void)
{
#ifdef __CSKY__
    return ((__get_PSR() >> 16U) & 0xFFU) - 32U;
#elif defined(__riscv)
    return (__get_MCAUSE() & 0x3FFU);
#elif defined(__arm__)
    return (__get_IPSR() & 0x1FFU) - 16U;
#endif
}
