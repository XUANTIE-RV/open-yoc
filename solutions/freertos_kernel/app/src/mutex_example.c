#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define MUTEX_TASK_HI_PRIO              7
#define MUTEX_TASK_LO_PRIO              6
#define WAIT_FOREVER                    -1

static SemaphoreHandle_t  g_Testmux01;
static TaskHandle_t g_TestTask01;
static TaskHandle_t g_TestTask02;

static void Example_MutexTask1()
{
    BaseType_t uwRet;

    printf("task1 try to get  mutex, wait 100ms.\n");
    uwRet = xSemaphoreTake(g_Testmux01, pdMS_TO_TICKS(100));
    if (uwRet == pdPASS) {
        printf("task1 get mutex g_Testmux01.\n");
        xSemaphoreGive(g_Testmux01);
    } else {
        printf("task1 timeout and try to get  mutex, wait forever.\n");
        uwRet = xSemaphoreTake(g_Testmux01, WAIT_FOREVER);
        if (uwRet == pdPASS) {
            printf("task1 wait forever,get mutex g_Testmux01.\n");
            xSemaphoreGive(g_Testmux01);
        }
    }
    vTaskDelete(NULL);
}

static void Example_MutexTask2()
{
    printf("task2 try to get  mutex, wait forever.\n");
    xSemaphoreTake(g_Testmux01, WAIT_FOREVER);

    printf("task2 get mutex g_Testmux01 and suspend 100 ms.\n");
    vTaskDelay(pdMS_TO_TICKS(100));

    printf("task2 resumed and post the g_Testmux01\n");
    xSemaphoreGive(g_Testmux01);

    vTaskDelete(NULL);
}

void mutex_example_main(void)
{
    BaseType_t ret;

    g_Testmux01 = xSemaphoreCreateMutex();

    vTaskSuspendAll();
    ret = xTaskCreate((TaskFunction_t)Example_MutexTask1, "MutexTsk1", CONFIG_APP_TASK_STACK_SIZE, NULL, MUTEX_TASK_LO_PRIO, &g_TestTask01);
    if (ret != pdPASS) {
        xTaskResumeAll();
        printf("fail to task1 create.\n");
        goto failure;
    }

    ret = xTaskCreate((TaskFunction_t)Example_MutexTask2, "MutexTsk2", CONFIG_APP_TASK_STACK_SIZE, NULL, MUTEX_TASK_HI_PRIO, &g_TestTask02);
    if (ret != pdPASS) {
        xTaskResumeAll();
        vTaskDelete(g_TestTask01);
        printf("fail to create task2.\n");
        goto failure;
    }
    xTaskResumeAll();
    vTaskDelay(pdMS_TO_TICKS(500));
    printf("test kernel mutex successfully!\n");
failure:
    vSemaphoreDelete(g_Testmux01);
}
