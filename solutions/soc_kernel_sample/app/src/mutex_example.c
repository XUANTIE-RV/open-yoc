#include <aos/aos.h>
#include <aos/cli.h>

#define TAG "soc_mutex_test"

static aos_mutex_t g_mutex;

static void mutex_task1()
{
    int ret = -1;
    int timeout = 100;

    LOGI(TAG, "delay 10ms");
    aos_msleep(10);
    LOGI(TAG, "task1 lock");
    ret = aos_mutex_lock(&g_mutex, timeout);
    if (ret) {
        if (-ETIMEDOUT == ret) {
            LOGI(TAG, "timeout and try to get  mutex, wait forever");
            ret = aos_mutex_lock(&g_mutex, AOS_WAIT_FOREVER);
            if (!ret) {
                aos_mutex_unlock(&g_mutex);
                LOGI(TAG, "task1 unlock");
            }
        } else {
            LOGE(TAG, "aos_mutex_lock fail");
        }
    } else {
        aos_mutex_unlock(&g_mutex);
        LOGI(TAG, "task1 unlock");
    }
    aos_task_exit(0);
}

static void mutex_task2()
{
    int ret = -1;

    LOGI(TAG, "mutex_task2 lock");
    ret = aos_mutex_lock(&g_mutex, AOS_WAIT_FOREVER);
    if (ret) {
        LOGE(TAG, "aos_mutex_lock fail");
        aos_task_exit(0);
    }
    LOGI(TAG, "delay 200ms");
    aos_msleep(200);
    ret = aos_mutex_unlock(&g_mutex);
    if (ret) {
        LOGE(TAG, "aos_mutex_unlock fail");
    } else {
        LOGI(TAG, "mutex_task2 unlock");
    }
    aos_task_exit(0);
}

void mutex_example_main(void)
{
    int ret = -1;
    aos_task_t mutex_task1_handle;
    aos_task_t mutex_task2_handle;

    ret = aos_mutex_new(&g_mutex);
    if (ret) {
        LOGE(TAG, "create event error");
        return;
    }
    ret = aos_task_new_ext(&mutex_task1_handle, "mutex_task1", mutex_task1, NULL, CONFIG_APP_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI);
    if (ret) {
        LOGE(TAG, "fail to create task.");
        aos_mutex_free(&g_mutex);
        return;
    }

    ret = aos_task_new_ext(&mutex_task2_handle, "mutex_task1", mutex_task2, NULL, CONFIG_APP_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI - 1);
    if (ret) {
        aos_task_delete(&mutex_task1_handle);
        LOGE(TAG, "fail to create task.");
        aos_mutex_free(&g_mutex);
        return;
    }
    aos_msleep(400);
    aos_mutex_free(&g_mutex);
}
