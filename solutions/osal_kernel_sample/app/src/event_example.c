#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <aos/aos.h>
#include <debug/dbg.h>
#include <aos/cli.h>

#define TAG                           "soc_event_test"
#define event_wait                    0x00001001

static aos_event_t event;

static void example_event()
{
    int ret;
    unsigned int flags = 0;

    LOGI(TAG, "Example_Event waits event 0x%x", event_wait);
    ret =  aos_event_get(&event, event_wait, AOS_EVENT_OR_CLEAR, &flags, AOS_WAIT_FOREVER);
    if (ret) {
        LOGE(TAG, "aos_event_get fail.");
    }

    if ((flags&event_wait) == event_wait) {
        LOGI(TAG, "reads event :0x%x.",flags);
    } else {
        LOGE(TAG, "reads event fail.");
    }

    LOGI(TAG, "test kernel event successfully!");
    aos_event_free(&event);
    aos_task_exit(0);
}

void event_example_main(void)
{
    int ret = -1;
    aos_task_t event_handle;

    ret = aos_event_new(&event, 0);
    if (ret) {
        LOGE(TAG, "create event error");
        return;
    }
    ret = aos_task_new_ext(&event_handle, "example_event_task", example_event, NULL, CONFIG_APP_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI);
    if (ret) {
        LOGE(TAG, "fail to create task.");
        aos_event_free(&event);
        return;
    }

    LOGI(TAG, "example_k_event writes event .");

    ret = aos_event_set(&event, event_wait, AOS_EVENT_OR);
    if (ret) {
        LOGE(TAG, "aos_event_set fail.");
        aos_event_free(&event);
    }
}