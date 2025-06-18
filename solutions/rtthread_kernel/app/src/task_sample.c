/*
 * Demo: thread(s)
 *
 * This demo creates two threads:
 *    1) create thread #1 dynamically, and delete automatically when the thread #1 finished;
 *    2) create thread #2 statically, and print counting numbers continuously.
 */

#include <rtthread.h>

#define THREAD_PRIORITY         25
#define THREAD_TIMESLICE        5

static char thread2_stack[CONFIG_APP_TASK_STACK_SIZE];
static struct rt_thread thread2;

/* thread handler */
static rt_thread_t tid1 = RT_NULL;


static void thread1_entry(void *parameter)
{
    rt_uint32_t count = 0;

    while (1) {
        /* thread #1 occupies low priority and prints counting numbers continuously */
        rt_kprintf("thread1 count: %d\n", count ++);
        rt_thread_mdelay(50);
        if (count == 5) {
            break;
        }
    }
    rt_kprintf("thread1 exit\n");
}


static void thread2_entry(void *param)
{
    rt_uint32_t count = 0;

    /* thread #2 occupies higher priority than that of thread #1 */
    for (count = 0; count < 10 ; count++) {
        rt_kprintf("thread2 count: %d\n", count);
    }
    rt_kprintf("thread2 exit\n");
}

int task_example_main(void)
{
    /* create thread #1 dynamically */
    tid1 = rt_thread_create("thread1",
                            thread1_entry, RT_NULL,
                            CONFIG_APP_TASK_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);

    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    /* create thread #2 statically */
    rt_thread_init(&thread2,
                   "thread2",
                   thread2_entry,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),
                   THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    rt_thread_startup(&thread2);

    return 0;
}

