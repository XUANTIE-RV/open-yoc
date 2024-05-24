/*
 * Copyright (C) 2023-2024 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <rtthread.h>
#include <rthw.h>

static int g_count;
rt_hw_spinlock_t spinlock;

static void thread_entry(void *parameter)
{
    int id;

    while (1) {
        id = rt_hw_cpu_id();
        rt_hw_spin_lock(&spinlock);
        printf("[%s] in %d core, count:%d \r\n", rt_thread_self()->parent.name, id, g_count++);
        rt_hw_spin_unlock(&spinlock);
        rt_thread_mdelay(1000);
    }
}

void smp_example(void)
{
    rt_hw_spin_lock_init(&spinlock);
    for (long i = 0; i < CONFIG_NR_CPUS; i++) {
        char buf[32];
        rt_thread_t tid;
        snprintf(buf, sizeof(buf), "thread_%ld", i);
        tid = rt_thread_create(buf, thread_entry, RT_NULL, 8192, 32, 5);
        rt_thread_control(tid, RT_THREAD_CTRL_BIND_CPU, (void *)i);
        if (tid != RT_NULL) {
            rt_thread_startup(tid);
        }
    }
}
