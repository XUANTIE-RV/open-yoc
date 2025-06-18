#include <aos/aos.h>
#include <aos/cli.h>

#define TAG  "soc_timer_test"
#define TIMER_ARRAY_LEN            20

static long long call_time[TIMER_ARRAY_LEN] = {0};
static int call_times1 = 0;
static int call_times2 = 0;


#define PERIOD_MODE     1
#define ONCE_MODE       0
#define NO_RUN          0

static void timer1_cb()
{
    call_times1++;
}

static void timer2_cb()
{
    long long  ms;
    ms = aos_now_ms();
    if (call_times2 < TIMER_ARRAY_LEN) {
        call_time[call_times2++] = ms;
    };
}

void timer_example_main(void)
{
    int ret = -1;
    aos_timer_t timer1;
    aos_timer_t timer2;

    ret = aos_timer_new_ext(&timer1, timer1_cb, NULL, 100, ONCE_MODE, NO_RUN);
    if (ret) {
        LOGE(TAG, "timer1 fail");
        return;
    }
    ret = aos_timer_new_ext(&timer2, timer2_cb, NULL, 100,  PERIOD_MODE, NO_RUN);
    if (ret) {
        aos_timer_free(&timer1);
        LOGE(TAG, "timer2 fail");
        return;
    }
    LOGI(TAG, "timer1 start");
    call_times1 = 0;
    ret = aos_timer_start(&timer1);
    if (ret) {
        LOGE(TAG, "timer1 start fail");
        goto failure;
    }
    LOGI(TAG, "delay 1000 ms");
    aos_msleep(1000);
    LOGI(TAG, "timer1 stop");
    aos_timer_stop(&timer1);
    if (call_times1 == 1) {
        LOGI(TAG, "timer1 call success");
    } else {
        LOGE(TAG, "timer1 call fail");
        goto failure;
    }
    LOGI(TAG, "timer2 start");
    call_times2 = 0;
    ret = aos_timer_start(&timer2);
    if (ret) {
        LOGE(TAG, "timer2 start fail");
        goto failure;
    }
    LOGI(TAG, "delay 1010 ms");
    aos_msleep(1010);
    LOGI(TAG, "timer2 stop");
    ret = aos_timer_stop(&timer2);
    if (ret) {
        LOGE(TAG, "timer2 stop fail");
        goto failure;
    }
    if (call_times2 == 10) {
        LOGI(TAG, "every 100ms call once");
        for (int i = 0; i<call_times2; i++) {
            LOGI(TAG, "the %d trigger time of the timer is  %llu ms",i, call_time[i]);
        }
    } else {
        LOGE(TAG, "timer2 fail, trigger times:%d\n", call_times2);
        goto failure;
    }
    LOGI(TAG, "timer test success");
failure:
    aos_timer_free(&timer1);
    aos_timer_free(&timer2);
}