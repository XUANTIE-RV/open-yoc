/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-03-14     WangShun     first version
 */

#include <rtthread.h>

rt_atomic_t rt_hw_atomic_exchange(volatile rt_atomic_t *ptr, rt_atomic_t val)
{
    rt_base_t level;
    rt_atomic_t temp;
    level = rt_hw_interrupt_disable();
    temp = *ptr;
    *ptr = val;
    rt_hw_interrupt_enable(level);
    return temp;
}

rt_atomic_t rt_hw_atomic_add(volatile rt_atomic_t *ptr, rt_atomic_t val)
{
    rt_base_t level;
    rt_atomic_t temp;
    level = rt_hw_interrupt_disable();
    temp = *ptr;
    *ptr += val;
    rt_hw_interrupt_enable(level);
    return temp;
}

rt_atomic_t rt_hw_atomic_sub(volatile rt_atomic_t *ptr, rt_atomic_t val)
{
    rt_base_t level;
    rt_atomic_t temp;
    level = rt_hw_interrupt_disable();
    temp = *ptr;
    *ptr -= val;
    rt_hw_interrupt_enable(level);
    return temp;
}

rt_atomic_t rt_hw_atomic_xor(volatile rt_atomic_t *ptr, rt_atomic_t val)
{
    rt_base_t level;
    rt_atomic_t temp;
    level = rt_hw_interrupt_disable();
    temp = *ptr;
    *ptr = (*ptr) ^ val;
    rt_hw_interrupt_enable(level);
    return temp;
}

rt_atomic_t rt_hw_atomic_and(volatile rt_atomic_t *ptr, rt_atomic_t val)
{
    rt_base_t level;
    rt_atomic_t temp;
    level = rt_hw_interrupt_disable();
    temp = *ptr;
    *ptr = (*ptr) & val;
    rt_hw_interrupt_enable(level);
    return temp;
}

rt_atomic_t rt_hw_atomic_or(volatile rt_atomic_t *ptr, rt_atomic_t val)
{
    rt_base_t level;
    rt_atomic_t temp;
    level = rt_hw_interrupt_disable();
    temp = *ptr;
    *ptr = (*ptr) | val;
    rt_hw_interrupt_enable(level);
    return temp;
}

rt_atomic_t rt_hw_atomic_load(volatile rt_atomic_t *ptr)
{
    rt_base_t level;
    rt_atomic_t temp;
    level = rt_hw_interrupt_disable();
    temp = *ptr;
    rt_hw_interrupt_enable(level);
    return temp;
}

void rt_hw_atomic_store(volatile rt_atomic_t *ptr, rt_atomic_t val)
{
    rt_base_t level;
    level = rt_hw_interrupt_disable();
    *ptr = val;
    rt_hw_interrupt_enable(level);
}

rt_atomic_t rt_hw_atomic_flag_test_and_set(volatile rt_atomic_t *ptr)
{
    rt_base_t level;
    rt_atomic_t temp;
    level = rt_hw_interrupt_disable();
    if (*ptr == 0)
    {
        temp = 0;
        *ptr = 1;
    }
    else
        temp = 1;
    rt_hw_interrupt_enable(level);
    return temp;
}

void rt_hw_atomic_flag_clear(volatile rt_atomic_t *ptr)
{
    rt_base_t level;
    level = rt_hw_interrupt_disable();
    *ptr = 0;
    rt_hw_interrupt_enable(level);
}

rt_atomic_t rt_hw_atomic_compare_exchange_strong(volatile rt_atomic_t *ptr, rt_atomic_t *old, rt_atomic_t desired)
{
    rt_base_t level;
    rt_atomic_t temp;
    level = rt_hw_interrupt_disable();
    if ((*ptr) != (*old))
    {
        *old = *ptr;
        temp = 0;
    }
    else
    {
        *ptr = desired;
        temp = 1;
    }
    rt_hw_interrupt_enable(level);
    return temp;
}
