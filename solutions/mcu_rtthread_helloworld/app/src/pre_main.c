/*
 * Copyright (C) 2018-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <rtthread.h>
#include <rthw.h>

extern void rt_hw_board_init(void);
int rtthread_startup(void)
{
#ifdef RT_USING_SMP
    rt_hw_spin_lock_init(&_cpus_lock);
#endif
    rt_hw_local_irq_disable();

    /* board level initialization
     * NOTE: please initialize heap inside board initialization.
     */
    rt_hw_board_init();

    /* timer system initialization */
    rt_system_timer_init();

    /* scheduler system initialization */
    rt_system_scheduler_init();

#ifdef RT_USING_SIGNALS
    /* signal system initialization */
    rt_system_signal_init();
#endif /* RT_USING_SIGNALS */

    /* timer thread initialization */
    rt_system_timer_thread_init();

    /* idle thread initialization */
    rt_thread_idle_init();

#ifdef RT_USING_SMP
    rt_hw_spin_lock(&_cpus_lock);
#endif /* RT_USING_SMP */
    return 0;
}

extern int main(void);
static void application_task_entry(void *arg)
{
    main();
}

/* overide pre_main in chip component */
int pre_main(void)
{
    rtthread_startup();
    /* RT-Thread components initialization */
    extern void rt_components_init(void);
    rt_components_init();

    int prio = RT_THREAD_PRIORITY_MAX / 2;
    rt_thread_t tid = rt_thread_create("app_task", application_task_entry, NULL, CONFIG_INIT_TASK_STACK_SIZE, prio, 10u);
    if (tid == RT_NULL) {
        // printf("task create error\r\n");
        while (1);
    }
    rt_thread_startup(tid);

    /* kernel start */
    rt_system_scheduler_start();

    return 0;
}


