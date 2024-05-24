/*
 * Copyright (C) 2019-2024 Alibaba Group Holding Limited
 */

#if defined(CONFIG_SMP) && CONFIG_SMP
#include <aos/aos.h>
#include <aos/cli.h>

static int g_count;
static aos_spinlock_t spinlock;

static void thread_entry(void *parameter)
{
    int id;

    while (1) {
        id = aos_get_cur_cpu_id();
        aos_spin_lock(&spinlock);
        printf("[%s] in %d core, count:%d \r\n", aos_task_name(), id, g_count++);
        aos_spin_unlock(&spinlock);
        aos_msleep(1000);
    }
}

void smp_example(void)
{
    aos_spin_lock_init(&spinlock);
    for (int i = 0; i < CONFIG_NR_CPUS; i++) {
        char buf[32];
        aos_task_t task;
        snprintf(buf, sizeof(buf), "thread_%d", i);
        aos_task_create_ext(&task, buf, thread_entry, NULL, NULL, 2048, AOS_DEFAULT_APP_PRI, AOS_TASK_AUTORUN, i);
    }
}

#endif /*CONFIG_SMP*/
