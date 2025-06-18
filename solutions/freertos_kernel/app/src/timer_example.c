#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>

#define PERIOD_MODE     pdTRUE
#define NO_PERIOD_MODE  pdFALSE

static void Timer1_Callback(TimerHandle_t xTimer);
static void Timer2_Callback(TimerHandle_t xTimer);
static uint32_t g_timercount1 = 0;
static uint32_t g_timercount2 = 0;

static void Timer1_Callback(TimerHandle_t xTimer)
{
    unsigned long tick_last1;

    tick_last1 = (unsigned long)xTaskGetTickCount();
    g_timercount1++;
    printf("g_timercount1=%u\n", g_timercount1);
    printf("tick_last1 = %lu\n", tick_last1);
}

static void Timer2_Callback(TimerHandle_t xTimer)
{
    unsigned long tick_last2;

    g_timercount2++;
    tick_last2 = (unsigned long)xTaskGetTickCount();
    printf("g_timercount2=%u\n", g_timercount2);
    printf("tick_last2 = %lu\n", tick_last2);
}

void timer_example_main(void)
{
    BaseType_t ret;
    TimerHandle_t timer01;
    TimerHandle_t timer02;

    printf("start to create timer\n");
    timer01 = xTimerCreate("Timer01", pdMS_TO_TICKS(500), NO_PERIOD_MODE, (void*)1, (TimerCallbackFunction_t)Timer1_Callback);
    if (NULL == timer01) {
        printf("create Timer01 fail\n");
    }
    timer02 = xTimerCreate("Timer02", pdMS_TO_TICKS(100), PERIOD_MODE, (void*)2, (TimerCallbackFunction_t)Timer2_Callback);
    if (NULL == timer02) {
        xTimerDelete(timer01, 0);
        printf("create Timer02 fail\n");
    }
    ret = xTimerStart(timer01, 0);
    if (ret != pdPASS) {
        printf("start Timer1 failed\n");
        goto failure;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
    ret = xTimerStop(timer01, 0);
    if (ret != pdPASS) {
        printf("stop Timer1 failed\n");
        goto failure;
    }
    ret = xTimerStart(timer01, 0);
    if (ret != pdPASS) {
        printf("start Timer1 failed\n");
        goto failure;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("start Timer2\n");
    xTimerStart(timer02, 0);
    vTaskDelay(pdMS_TO_TICKS(1010));
    ret = xTimerStop(timer02, 0);
    if (ret != pdPASS) {
        printf("stop Timer2 failed\n");
    }
    printf("test kernel timer successfully!\n");

failure:
    xTimerDelete(timer01, 0);
    xTimerDelete(timer02, 0);
}
