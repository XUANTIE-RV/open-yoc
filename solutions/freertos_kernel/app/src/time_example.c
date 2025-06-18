#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define  FREERTOS_SYSTICK      (1000 / CONFIG_SYSTICK_HZ)

void time_example_main(void)
{
    uint32_t cnt = 0;
    long long cur_time = 0;
    long long expect_time = 0;
    long long delay_19s_time = 0;
    const long long delay_1s_time = 1000;

    cnt = 10;
    printf("kernel systick is %u ms\n", FREERTOS_SYSTICK);
    cur_time = xTaskGetTickCount() * FREERTOS_SYSTICK;
    printf("now time = %llu ms before 19s....\n", cur_time);
    printf("print cnt every 1s for %u times\n", cnt);
    expect_time = cur_time;
    while (cnt--) {
        expect_time += 1000;
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("-----%u\n", cnt);
    }
    cnt = 3;
    printf("print cnt every 3s for %u times\n", cnt);
    while (cnt--) {
        expect_time += 3000;
        vTaskDelay(pdMS_TO_TICKS(3000));
        printf("-----%u\n", cnt);
    }
    delay_19s_time = xTaskGetTickCount() * FREERTOS_SYSTICK;
    printf("now time = %llu ms after 19s, will be delay  1s....\n", delay_19s_time);
    vTaskDelay(pdMS_TO_TICKS(1000));
    __DSB();
    __ISB();
    cur_time = xTaskGetTickCount() * FREERTOS_SYSTICK;
    printf("now time = %llu ms after delay 1s \n", cur_time);
    long long err_19s = delay_19s_time - expect_time;
    long long err_1s = cur_time - delay_19s_time - delay_1s_time;
    printf("err_19s = %lld, err_1s = %lld\r\n", err_19s, err_1s);
    if (abs(err_19s) < 200 && abs(err_1s) < 200) {
        printf("test kernel time successfully!\n");
    } else {
        printf("test kernel time1 fail\n");
    }
}

