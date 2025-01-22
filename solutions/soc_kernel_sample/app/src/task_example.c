#include <aos/aos.h>
#include <aos/cli.h>

#define TAG  "soc_task_test"

static aos_task_t task_hi_handle = NULL;
static aos_task_t task_lo_handle = NULL;

static void task_hi_task()
{
    int ret = -1;

    LOGI(TAG, "task_hi_task start");
    aos_msleep(10);
    LOGI(TAG, "task_hi_task delay end,start to suspend self");
    ret = aos_task_suspend(&task_hi_handle);
    if (ret) {
        LOGE(TAG, "suspend self fail");
    } else {
        LOGI(TAG, "suspend self successful");
    }
    aos_task_exit(0);
    LOGE(TAG, "aos_task_exit fail");
}

static void task_lo_task()
{
    int ret = -1;

    LOGI(TAG, "task_lo_task start");
    aos_msleep(50);
    ret = aos_task_resume(&task_hi_handle);
    if (ret) {
        LOGE(TAG, "resume  task_hi_task fail");
    } else {
        LOGI(TAG, "resume  task_hi_task success");
    }
    aos_task_exit(0);
    LOGE(TAG, "aos_task_exit fail");
}


void task_example_main(void)
{
    int ret = -1;

    ret = aos_task_new_ext(&task_hi_handle, "task_hi task", task_hi_task, NULL, CONFIG_APP_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI - 1);
    if (ret) {
        LOGE(TAG, "fail to create task.");
        return;
    }
    ret = aos_task_new_ext(&task_lo_handle, "task_lo task", task_lo_task, NULL, CONFIG_APP_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI);
    if (ret) {
        LOGE(TAG, "fail to create task.");
        aos_task_delete(&task_hi_handle);
        return;
    }
}
