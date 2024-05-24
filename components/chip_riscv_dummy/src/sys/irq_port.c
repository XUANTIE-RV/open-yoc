/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <csi_config.h>
#include <stdbool.h>
#include <csi_core.h>
#include <soc.h>

void soc_irq_enable(uint32_t irq_num)
{
    csi_vic_enable_irq((int32_t)irq_num);
}

void soc_irq_disable(uint32_t irq_num)
{
    csi_vic_disable_irq((int32_t)irq_num);
}

bool soc_irq_is_enabled(uint32_t irq_num)
{
    bool ret;

    if (csi_vic_get_enabled_irq((int32_t)irq_num)) {
        ret = true;
    } else {
        ret = false;
    }

    return ret;
}

void soc_irq_priority(uint32_t irq_num, uint32_t priority)
{
    csi_vic_set_prio((int32_t)irq_num, priority);
}

/**
 * @brief get external irq number only 
 * @return irq no 
 */
uint32_t soc_irq_get_irq_num(void)
{
#if CONFIG_CPU_E9XX
    return (__get_MCAUSE() & 0x3FFU);
#else
#if defined(CONFIG_RISCV_SMODE) && CONFIG_RISCV_SMODE
    uint32_t num = PLIC_Hn_MSCLAIM_VAL(&PLIC->PLIC_H0_SCLAIM, csi_get_cpu_id());
#else
    uint32_t num = PLIC_Hn_MSCLAIM_VAL(&PLIC->PLIC_H0_MCLAIM, csi_get_cpu_id());
#endif
    return num;
#endif /* end exx */
}

void soc_irq_end(uint32_t irq_num)
{
#if CONFIG_CPU_E9XX
    //TODO:
#else
#if defined(CONFIG_RISCV_SMODE) && CONFIG_RISCV_SMODE
    PLIC_Hn_MSCLAIM_VAL(&PLIC->PLIC_H0_SCLAIM, csi_get_cpu_id()) = irq_num;
#else
    PLIC_Hn_MSCLAIM_VAL(&PLIC->PLIC_H0_MCLAIM, csi_get_cpu_id()) = irq_num;
#endif
#endif /* end exx */
}
