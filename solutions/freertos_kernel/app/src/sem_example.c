#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define TASK_PRIO_TEST        7
#define WAIT_FOREVER          -1

static QueueHandle_t g_usSem;
static TaskHandle_t g_TestTask01;
static TaskHandle_t g_TestTask02;

static void Example_SemTask1(void)
{
    int uwRet;

    printf("Example_SemTask1 try get sem g_usSem ,timeout 100ms.\n");
    uwRet = xSemaphoreTake(g_usSem, pdMS_TO_TICKS(100));
    if (pdPASS == uwRet) {
        xSemaphoreGive(g_usSem);
        printf("Example_SemTask1 post sem g_usSem .\n");
    } else {
        printf("Example_SemTask1 timeout and try get sem g_usSem wait forever.\n");
        uwRet = xSemaphoreTake(g_usSem, WAIT_FOREVER);
        if (pdPASS == uwRet) {
            xSemaphoreGive(g_usSem);
            vTaskDelete(NULL);
        }
        printf("Example_SemTask1 wait_forever and get sem g_usSem .\n");
    }

    vTaskDelete(NULL);
}

static void Example_SemTask2(void)
{
    int uwRet;

    printf("Example_SemTask2 try get sem g_usSem wait forever.\n");

    uwRet = xSemaphoreTake(g_usSem, WAIT_FOREVER);
    if (pdPASS == uwRet) {
        printf("Example_SemTask2 get sem g_usSem and then delay 200ms .\n");
    }

    vTaskDelay(pdMS_TO_TICKS(200));

    printf("Example_SemTask2 post sem g_usSem .\n");

    xSemaphoreGive(g_usSem);

    vTaskDelete(NULL);
}

void sem_example_main(void)
{
    BaseType_t ret;

    g_usSem = xSemaphoreCreateCounting(1, 0);
    vTaskSuspendAll();
    ret = xTaskCreate((TaskFunction_t)Example_SemTask1, "Example_TaskHi", CONFIG_APP_TASK_STACK_SIZE, NULL, TASK_PRIO_TEST, &g_TestTask01);
    if (ret != pdPASS) {
        xTaskResumeAll();
        printf("fail to create task1.\n");
        goto failure;
    }

    ret = xTaskCreate((TaskFunction_t)Example_SemTask2, "Example_TaskLo", CONFIG_APP_TASK_STACK_SIZE, NULL, TASK_PRIO_TEST-1, &g_TestTask02);
    if (ret != pdPASS) {
        vTaskDelete(g_TestTask01);
        xTaskResumeAll();
        printf("fail to create task2.\n");
        goto failure;
    }
    xTaskResumeAll();
    xSemaphoreGive(g_usSem);
    vTaskDelay(pdMS_TO_TICKS(400));
    printf("test kernel semaphore successfully!\n");
failure:
    vSemaphoreDelete(g_usSem);
}
