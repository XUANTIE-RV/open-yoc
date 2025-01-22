
/*
 * Demo: mutex(es)
 *
 * This demo demonstrates how the mutex manage the shared resource.
 */

#include <rtthread.h>

#define THREAD_PRIORITY         8
#define THREAD_TIMESLICE        5

static rt_mutex_t dynamic_mutex = RT_NULL;
static rt_uint8_t number1, number2 = 0;

static char thread1_stack[CONFIG_APP_TASK_STACK_SIZE];
static struct rt_thread thread1;

static char thread2_stack[CONFIG_APP_TASK_STACK_SIZE];
static struct rt_thread thread2;

static void rt_thread_entry1(void *parameter)
{
    while (1) {
        rt_mutex_take(dynamic_mutex, RT_WAITING_FOREVER);
        number1++;
        rt_thread_mdelay(10);
        number2++;
        rt_mutex_release(dynamic_mutex);
        if (number1 != number2) {
            rt_kprintf("not protect.number1 = %d, mumber2 = %d \n", number1, number2);
        } else {
            rt_kprintf("rt_thread_entry1 mutex protect ,number1 = mumber2 is %d\n", number1);
        }
        if (number1 >= 20) {
            break;
        }
    }
    rt_mutex_delete(dynamic_mutex);
    rt_kprintf("rt_thread_entry1 exit\n");
}


static void rt_thread_entry2(void *parameter)
{
    while (1) {
        rt_mutex_take(dynamic_mutex, RT_WAITING_FOREVER);
        if (number1 != number2) {
            rt_kprintf("not protect.number1 = %d, mumber2 = %d \n", number1, number2);
        } else {
            rt_kprintf("rt_thread_entry2 mutex protect ,number1 = mumber2 is %d\n", number1);
        }

        number1++;
        number2++;
        rt_thread_mdelay(10);
        rt_mutex_release(dynamic_mutex);

        if (number1 >= 10)
            break;
    }
    rt_kprintf("rt_thread_entry2 exit\n");
}

int mutex_example_main(void)
{
    dynamic_mutex = rt_mutex_create("dmutex", RT_IPC_FLAG_PRIO);
    if (dynamic_mutex == RT_NULL) {
        rt_kprintf("create dynamic mutex failed.\n");
        return -1;
    }

    rt_thread_init(&thread1,
                   "thread1",
                   rt_thread_entry1,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),
                   THREAD_PRIORITY, THREAD_TIMESLICE);
    rt_thread_startup(&thread1);

    rt_thread_init(&thread2,
                   "thread2",
                   rt_thread_entry2,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),
                   THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    rt_thread_startup(&thread2);
    return 0;
}
