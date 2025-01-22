#include <csi_core.h>
#include <aos/aos.h>
#include <aos/cli.h>

void time_example_main(void)
{
    uint32_t cnt = 0;
    long long cur_time = 0;
    long long expect_time = 0;
    long long delay_19s_time = 0;
    const long long delay_1s_time = 1000;

    cnt = 10;
    printf("kernel systick is %u ms\n", 1000 / CONFIG_SYSTICK_HZ);
    cur_time = aos_now_ms();
    printf("now time = %llu ms before 19s....\n", cur_time);
    printf("print cnt every 1s for %u times\n", cnt);
    expect_time = cur_time;
    while (cnt--) {
        expect_time += 1000;
        aos_msleep(1000);
        printf("-----%u\n", cnt);
    }
    cnt = 3;
    printf("print cnt every 3s for %u times\n", cnt);
    while (cnt--) {
        expect_time += 3000;
        aos_msleep(3000);
        printf("-----%u\n", cnt);
    }
    delay_19s_time = aos_now_ms();
    printf("now time = %llu ms after 19s, will be delay  1s....\n", delay_19s_time);
    aos_msleep(1000);
    __DSB();
    cur_time = aos_now_ms();
    printf("now time = %llu ms after delay 1s \n", cur_time);
    if ((delay_19s_time - expect_time) < 0 || (cur_time - delay_19s_time - delay_1s_time) < 0) {
        printf("test kernel time fail\n");
        return;
    }
    if ((delay_19s_time - expect_time) < 200 && (cur_time - delay_19s_time - delay_1s_time) < 200) {
        printf("test kernel time successfully!\n");
    } else {
        printf("test kernel time fail\n");
    }
}
