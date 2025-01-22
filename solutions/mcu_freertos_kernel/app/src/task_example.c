#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define TSK_PRIOR_HI          11
#define TSK_PRIOR_LO          10
#define WAIT_FOREVER          -1

static TaskHandle_t g_uwTskHi;
static TaskHandle_t g_uwTskLo;

void Example_TaskHi()
{
    printf("Enter TaskHi Handler.\r\n");
    vTaskDelay(pdMS_TO_TICKS(100));
    printf("Example_TaskHi:TaskHi vTaskDelay Done and suspend self.\r\n");
    vTaskSuspend(NULL);
    printf("Example_TaskHi:test kernel task successfully!\n");
    vTaskDelete(NULL);
    printf("Example_TaskHi:Fail to delete TaskHi itself. \n");
}


void Example_TaskLo()
{
    printf("Example_TaskLo:Enter TaskLo Handler.\r\n");
    vTaskDelay(pdMS_TO_TICKS(100));
    printf("Example_TaskLo:resume Example_TaskHi\r\n");
    vTaskResume(g_uwTskHi);
    vTaskDelete(NULL);
    printf("Example_TaskLo:Fail to delete TaskLo itself. \n");
}

void task_example_main(void)
{
    BaseType_t ret;

    vTaskSuspendAll();
    ret = xTaskCreate((TaskFunction_t)Example_TaskHi, "Example_TaskHi", CONFIG_APP_TASK_STACK_SIZE, NULL, TSK_PRIOR_HI, &g_uwTskHi);
    if (ret != pdPASS) {
        printf("fail to task1 create.\n");
        goto failure;
    }
    ret = xTaskCreate((TaskFunction_t)Example_TaskLo, "Example_TaskLo", CONFIG_APP_TASK_STACK_SIZE, NULL, TSK_PRIOR_LO, &g_uwTskLo);
    if (ret != pdPASS) {
        vTaskDelete(g_uwTskHi);
        printf("fail to create task2.\n");
        goto failure;
    }
failure:
    xTaskResumeAll();
    printf("Fail to delete example task itself. \n");
}
