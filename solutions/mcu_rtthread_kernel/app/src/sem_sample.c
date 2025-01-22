/*
 * Demo: semaphore
 * This demo creates one semaphore and two threads:
 *    1) thread #1: release the semaphore
 *    2) thread #2: receive the semaphore
 */

#include <rtthread.h>

#define THREAD_PRIORITY         25
#define THREAD_TIMESLICE        5

static rt_sem_t dynamic_sem = RT_NULL;

static char thread1_stack[CONFIG_APP_TASK_STACK_SIZE];
static struct rt_thread thread1;

static char thread2_stack[CONFIG_APP_TASK_STACK_SIZE];
static struct rt_thread thread2;

static void rt_thread1_entry(void *parameter)
{
    static rt_uint8_t count = 0;

    while (1) {
        if (count <= 100) {
            count++;
        } else
            break;

        /* count release semaphore every 10 counts */
        if (0 == (count % 10)) {
            rt_kprintf("thread1 release a dynamic semaphore.\n");
            rt_sem_release(dynamic_sem);
        }
    }
    rt_kprintf("rt_thread1_entry exit\n");
}



static void rt_thread2_entry(void *parameter)
{
    static rt_err_t result;
    static rt_uint8_t number = 0;
    while (1) {
        /* permanently wait for the semaphore; once obtain the semaphore, perform the number self-add operation */
        result = rt_sem_take(dynamic_sem, RT_WAITING_FOREVER);
        if (result != RT_EOK) {
            rt_kprintf("thread2 take a dynamic semaphore, failed.\n");
            break;
        } else {
            number++;
            rt_kprintf("thread2 take a dynamic semaphore. number = %d\n", number);
            if (number == 10) {
                break;
            }
        }
    }
    rt_thread_mdelay(10);
    rt_kprintf("rt_thread2_entry exit\n");
    rt_sem_delete(dynamic_sem);
}

int sem_example_main(void)
{
    /* create semaphtore and its initial value is 0 */
    dynamic_sem = rt_sem_create("dsem", 0, RT_IPC_FLAG_PRIO);
    if (dynamic_sem == RT_NULL) {
        rt_kprintf("create dynamic semaphore failed.\n");
        return -1;
    } else {
        rt_kprintf("create done. dynamic semaphore value = 0.\n");
    }

    rt_thread_init(&thread1,
                   "thread1",
                   rt_thread1_entry,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),
                   THREAD_PRIORITY, THREAD_TIMESLICE);
    rt_thread_startup(&thread1);

    rt_thread_init(&thread2,
                   "thread2",
                   rt_thread2_entry,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),
                   THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    rt_thread_startup(&thread2);

    return 0;
}
