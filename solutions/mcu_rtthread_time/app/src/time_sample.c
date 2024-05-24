#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <rtthread.h>
#include <rthw.h>
#include <rtdef.h>

void example_main(void *arg)
{
    rt_uint32_t uwTickCount = 0;
    rt_uint32_t cnt = 0;
    rt_tick_t cur_count = 0;
    rt_tick_t delay_1s_tick  = CONFIG_SYSTICK_HZ;
    rt_tick_t delay_3s_tick  = 3*CONFIG_SYSTICK_HZ;
    rt_tick_t delay_19s_tick_count  = 0;

    cnt = 10;
    rt_kprintf("current  kernel systick %d ms\n", 1000/CONFIG_SYSTICK_HZ);
    cur_count = rt_tick_get();
    rt_kprintf("current tick count = %u,  will delay 19s....\n", cur_count);
    rt_kprintf("print cnt every 1s for %lu times\n", cnt);
    uwTickCount = cur_count;
    while (cnt--) {
        uwTickCount += delay_1s_tick;
        rt_thread_delay(delay_1s_tick);
        rt_kprintf("-----%lu\n", cnt);
    }
    cnt = 3;
    rt_kprintf("print cnt every 3s for %lu times\n", cnt);
    while (cnt--) {
        uwTickCount += delay_3s_tick;
        rt_thread_delay(delay_3s_tick);
        rt_kprintf("-----%lu\n", cnt);
    }
    delay_19s_tick_count = rt_tick_get();
    rt_kprintf("tick cont = %u after 19s, will delay 1s....\n", delay_19s_tick_count);
    rt_thread_delay(delay_1s_tick);
    cur_count = rt_tick_get();
    rt_kprintf("tick cont = %u after delay 1s\n", cur_count);

    if ((delay_19s_tick_count- uwTickCount) < 3 && (cur_count - delay_19s_tick_count-delay_1s_tick) < 2) {
        rt_kprintf("rtt time successfully!\n");
    } else {
        rt_kprintf("rtt time fail\n");
    }
}