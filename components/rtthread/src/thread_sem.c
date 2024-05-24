/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <string.h>

#include <rthw.h>
#include <rtthread.h>

rt_err_t rt_thread_sem_create(rt_thread_t thread, const char *name,
                              rt_size_t count)
{
    rt_err_t ret = RT_EOK;

    if (thread == NULL)
    {
        return -RT_ERROR;
    }

    thread->thread_sem_obj = rt_sem_create(name, count, RT_IPC_FLAG_PRIO);
    if (thread->thread_sem_obj == RT_NULL)
    {
        ret = -RT_ERROR;
    }

    return ret;
}

rt_err_t rt_thread_sem_del(rt_thread_t thread)
{
    RT_ASSERT(thread);

    return rt_sem_delete(thread->thread_sem_obj);
}

rt_err_t rt_thread_sem_give(rt_thread_t thread)
{
    RT_ASSERT(thread);

    return rt_sem_release(thread->thread_sem_obj);
}

rt_err_t rt_thread_sem_take(rt_int32_t timeout)
{
    return rt_sem_take(rt_thread_self()->thread_sem_obj, timeout);
}

rt_err_t rt_thread_sem_count_set(rt_thread_t thread, rt_uint32_t count)
{
    RT_ASSERT(thread);
    RT_ASSERT(thread->thread_sem_obj);

    rt_base_t level;

    level = rt_hw_interrupt_disable();

    thread->thread_sem_obj->value = count;

    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

rt_err_t rt_thread_sem_count_get(rt_thread_t thread, rt_uint32_t *count)
{
    RT_ASSERT(thread);
    RT_ASSERT(thread->thread_sem_obj);
    RT_ASSERT(count);

    rt_base_t level;

    level = rt_hw_interrupt_disable();

    *count =  thread->thread_sem_obj->value;

    rt_hw_interrupt_enable(level);

    return RT_EOK;
}
