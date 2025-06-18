#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <event_groups.h>

#define event_wait                0x00001001
#define EVENT_TASK_PRIO           6
#define ClearOnExit               1
#define WaiteForAllBit            1
#define TimeOut                   100
#define WAIT_FOREVER              -1

static TaskHandle_t g_TestTask01;
static EventGroupHandle_t example_k_event_t;

static void Example_Event()
{
    EventBits_t ret;
    EventBits_t get_flags;

    printf("Example_Event waits event 0x%x \n", event_wait);
    get_flags = xEventGroupWaitBits(example_k_event_t, event_wait, ClearOnExit, WaiteForAllBit, TimeOut);
    if ((get_flags&event_wait) == event_wait) {
        printf("Example_Event,reads event :0x%lx\n", (long)get_flags);
    } else {
        printf("Example_Event,reads event timeout\n");
    }
    ret = xEventGroupGetBits(example_k_event_t);
    if ((ret&event_wait)!= 0) {
        printf("Example_Event fail to get event\n");
    }

    vTaskDelete(NULL);
}

void event_example_main(void)
{
    BaseType_t ret;
    EventBits_t ret_bits;

    example_k_event_t = xEventGroupCreate();
    ret = xTaskCreate((TaskFunction_t)Example_Event, "Example_Event_task", CONFIG_APP_TASK_STACK_SIZE, NULL, EVENT_TASK_PRIO, &g_TestTask01);
    if (ret != pdPASS) {
        printf("fail to create task.\n");
        goto failure;
    }
    printf("example_k_event writes event .\n");
    ret_bits = xEventGroupSetBits(example_k_event_t, event_wait);
    if ((ret_bits&event_wait) != event_wait) {
        printf("can not set\n");
        goto failure;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    printf("test kernel event successfully!\n");
failure:
    vEventGroupDelete(example_k_event_t);
}









