/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/12/23     Bernard      The first version
 * 2018/12/27     Jesven       Add secondary cpu boot
 * 2023/12/27     Steven       Adapted to Xuantie C907MP
 */

#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>
#include <csi_core.h>
#include <soc.h>
#include <asm/riscv_csr.h>

enum C907_MP
{
    C907_CORE0 = 0,
    C907_CORE1,
    C907_CORE2,
    C907_CORE3
};

/**
 * @brief    This function is used to suspend a specified kernel software interrupt.
 *
 * @note     Calling this function will trigger inter core interrupts.
 *
 * @param    id is the ID of the specific physical core.
 *
 */
void clint_ipi_send(uint64_t id)
{
    CLINT_Type *clint = (CLINT_Type *)CORET_BASE;

#if defined(CONFIG_RISCV_SMODE) && CONFIG_RISCV_SMODE
    switch (id)
    {
    case C907_CORE0:
        clint->SSIP0 |= (uint32_t)0x1;
        break;
    case C907_CORE1:
        clint->SSIP1 |= (uint32_t)0x1;
        break;
    case C907_CORE2:
        clint->SSIP2 |= (uint32_t)0x1;
        break;
    case C907_CORE3:
        clint->SSIP3 |= (uint32_t)0x1;
        break;
    default:
        break;
    }
#else
    switch (id)
    {
    case C907_CORE0:
        clint->MSIP0 |= (uint32_t)0x1;
        break;
    case C907_CORE1:
        clint->MSIP1 |= (uint32_t)0x1;
        break;
    case C907_CORE2:
        clint->MSIP2 |= (uint32_t)0x1;
        break;
    case C907_CORE3:
        clint->MSIP3 |= (uint32_t)0x1;
        break;
    default:
        break;
    }
#endif
}

/**
 * @brief    This function is used to clear the specified kernel software interrupt.
 *
 * @note     Calling this function will clear the inter core interrupt suspend flag.
 *
 * @param    id is the ID of the specific physical core.
 *
 */
void clint_ipi_clear(uint64_t id)
{
    CLINT_Type *clint = (CLINT_Type *)CORET_BASE;

#if defined(CONFIG_RISCV_SMODE) && CONFIG_RISCV_SMODE
    switch (id)
    {
    case C907_CORE0:
        clint->SSIP0 &= ~(uint32_t)0x1;
        break;
    case C907_CORE1:
        clint->SSIP1 &= ~(uint32_t)0x1;
        break;
    case C907_CORE2:
        clint->SSIP2 &= ~(uint32_t)0x1;
        break;
    case C907_CORE3:
        clint->SSIP3 &= ~(uint32_t)0x1;
        break;
    default:
        break;
    }
#else
    switch (id)
    {
    case C907_CORE0:
        clint->MSIP0 &= ~(uint32_t)0x1;
        break;
    case C907_CORE1:
        clint->MSIP1 &= ~(uint32_t)0x1;
        break;
    case C907_CORE2:
        clint->MSIP2 &= ~(uint32_t)0x1;
        break;
    case C907_CORE3:
        clint->MSIP3 &= ~(uint32_t)0x1;
        break;
    default:
        break;
    }
#endif
}

/**
 * @brief    This function is used to configure timers for specific physical cores.
 *
 * @note     Calling this function will configure Heartbeat Time.
 *
 * @param    NULL.
 *
 */
int rt_hw_tick_init(void)
{
    return 0;
}

/**
 * @brief    This function is used to enable software interrupts for specific physical cores.
 *
 * @note     Calling this function will enable software interruption.
 *
 * @param    NULL.
 *
 */
int rt_hw_clint_ipi_enable(void)
{
    /* Set the Machine-Software bit in MIE */
    csr_set(0x304, SR_MIE);
    return 0;
}

/**
 * @brief    This function is used to initialize the interrupt management function unique to the physical core.
 *
 * @note     Calling this function will close the specific core external interrupt and set the trigger threshold to 0.
 *
 * @param    NULL.
 *
 */
void rt_hw_scondary_interrupt_init(void)
{
    int i;
    int core_id = 0;
    core_id = rt_hw_cpu_id();

    if (core_id == C907_CORE0)
    {
        return;
    }
    else if (core_id == C907_CORE1)
    {
        for (i = 0; i < 32; i++)
        {
            PLIC->PLIC_H1_MIE[i] = 0;
            PLIC->PLIC_H1_SIE[i] = 0;
        }

        /* set hart threshold 0, enable all interrupt */
        PLIC->PLIC_H1_MTH = 0;
        PLIC->PLIC_H1_STH = 0;
    }
    else if (core_id == C907_CORE2)
    {
        for (i = 0; i < 32; i++)
        {
            PLIC->PLIC_H2_MIE[i] = 0;
            PLIC->PLIC_H2_SIE[i] = 0;
        }

        /* set hart threshold 0, enable all interrupt */
        PLIC->PLIC_H2_MTH = 0;
        PLIC->PLIC_H2_STH = 0;
    }
    else if (core_id == C907_CORE3)
    {
        for (i = 0; i < 32; i++)
        {
            PLIC->PLIC_H3_MIE[i] = 0;
            PLIC->PLIC_H3_SIE[i] = 0;
        }

        /* set hart threshold 0, enable all interrupt */
        PLIC->PLIC_H3_MTH = 0;
        PLIC->PLIC_H3_STH = 0;
    }
    else
    {
        rt_kprintf("Illegal CPU core, ID = %d", core_id);
        while (1)
            ;
    }

    /* enable MEIE & MSIE */
    uint32_t mie = __get_MIE();
    mie |= (1 << 11 | 1 << 3);
#if CONFIG_ECC_L1_ENABLE
    mie |= (1 << 16);
#endif
    __set_MIE(mie);
}

/**
 * @brief    Software interrupt handling function.
 *
 * @note     This function is used to respond to inter core interrupts.
 *
 * @param    NULL.
 *
 */
void ipi_irq(void)
{
    int core_id = 0;
    core_id = rt_hw_cpu_id();
    clint_ipi_clear(core_id);
    rt_schedule();
}
