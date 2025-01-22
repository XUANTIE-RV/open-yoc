#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define QUEUE_LEN             5
#define QUEUE_SIZE            50
#define MSG_NUM               5

static TaskHandle_t uwTask1;
static TaskHandle_t uwTask2;
static QueueHandle_t g_uwQueue;

static char abuf0[] = "test is message 0";
static char abuf1[] = "test is message 1";
static char abuf2[] = "test is message 2";
static char abuf3[] = "test is message 3";
static char abuf4[] = "test is message 4";
static char error_buf[] = "this is an error";

static void send_Entry(void *arg)
{
    uint32_t i = 0;
    int  uwRet = 0;
    uint32_t count = 0;

    printf("send_Entry\n");
    char *buf_p[MSG_NUM + 1] = {abuf0, abuf1, abuf2, abuf3, abuf4, error_buf};
    while (i < 5) {
        uwRet = xQueueSend(g_uwQueue, buf_p[i], 0);
        if (uwRet != pdPASS) {
            printf("fail to send message,error:%x\n", uwRet);
        }
        i++;
        count = uxQueueMessagesWaiting(g_uwQueue);
        printf("send_Entry:number of queued mesages : %u\n", count);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelete(NULL);
}

static void recv_Entry(void *arg)
{
    int   uwRet = 0;
    char Readbuf[64] = {0};
    uint32_t count = 0;

    printf("recv_Entry\n");
    vTaskDelay(pdMS_TO_TICKS(100));
    while (1) {
        uwRet = xQueueReceive(g_uwQueue, Readbuf, 0);
        if (uwRet != pdPASS) {
            if (0 == count) {
                printf("message queue recv finish\n");
                break;
            }
            printf("in expected, fail to recv message,error:%x\n", uwRet);
            break;
        }
        printf("recv message:%s\n", Readbuf);
        count = uxQueueMessagesWaiting(g_uwQueue);
        printf("recv_Entry:number of queued mesages : %u\n", count);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vQueueDelete(g_uwQueue);
    printf("delete the queue successfully!\n");
    vTaskDelete(NULL);
}

void message_example_main(void)
{
    BaseType_t ret;

    vTaskSuspendAll();
    g_uwQueue = xQueueCreate(QUEUE_LEN, QUEUE_SIZE);
    ret = xTaskCreate((TaskFunction_t)send_Entry, "send_task", CONFIG_APP_TASK_STACK_SIZE, NULL, 9, &uwTask1);
    if (ret != pdPASS) {
        printf("fail to create task1!\n");
        goto failure;
    }
    ret =  xTaskCreate((TaskFunction_t)recv_Entry, "recv_task", CONFIG_APP_TASK_STACK_SIZE, NULL, 9, &uwTask2);
    if (ret != pdPASS) {
        vTaskDelete(uwTask1);
        printf("fail to create task2!\n");
        goto failure;
    }
failure:
    xTaskResumeAll();
}






