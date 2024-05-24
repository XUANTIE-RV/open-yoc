#include <aos/aos.h>
#include <aos/cli.h>

void example_main()
{
    uint32_t cnt = 0;
    long long cur_count = 0;
    long long uwTickCount = 0;
    long long delay_19s_tick_count = 0;
    uint32_t delay_1s_tick = CONFIG_SYSTICK_HZ;
    uint32_t delay_3s_tick = 3 * CONFIG_SYSTICK_HZ;

    cnt = 10;
    printf("kernel systick is %u ms\n", 1000 / CONFIG_SYSTICK_HZ);
    cur_count = aos_sys_tick_get();
    printf("current tick count = %llu before 19s....\n", cur_count);
    printf("print cnt every 1s for %u times\n", cnt);
    uwTickCount = cur_count;
    while (cnt--) {
        uwTickCount += delay_1s_tick;
        aos_msleep(1000);
        printf("-----%u\n", cnt);
    }
    cnt = 3;
    printf("print cnt every 3s for %u times\n", cnt);
    while (cnt--) {
        uwTickCount += delay_3s_tick;
        aos_msleep(3000);
        printf("-----%u\n", cnt);
    }
    delay_19s_tick_count = aos_sys_tick_get();
    printf("tick count = %llu after 19s, will be delay  1s....\n", delay_19s_tick_count);
    aos_msleep(1000);
    cur_count = aos_sys_tick_get();
    printf("tick count = %llu after delay 1s \n", cur_count);
    if ((delay_19s_tick_count - uwTickCount) < 0 ||
        (cur_count - delay_19s_tick_count - delay_1s_tick) < 0) {
        printf("test kernel time fail\n");
        return;
    }
    if ((delay_19s_tick_count - uwTickCount) < 3 &&
        (cur_count - delay_19s_tick_count - delay_1s_tick) < 2) {
        printf("test kernel time successfully!\n");
    } else {
        printf("test kernel time fail\n");
    }
}
