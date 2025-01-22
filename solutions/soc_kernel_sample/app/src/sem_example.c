#include <aos/aos.h>
#include <aos/cli.h>

#define TAG                         "soc_sem_test"

static aos_sem_t g_sem;

static void task_lo_task()
{
    int ret = -1;

    LOGI(TAG, "task_lo_task start, try to wait 100ms");
    aos_msleep(10);
    ret = aos_sem_wait(&g_sem, 100);
    if (ret) {
        if (ret == -ETIMEDOUT) {
            LOGI(TAG, "aos_sem_wait timeout, will wait forever");
            ret = aos_sem_wait(&g_sem, AOS_WAIT_FOREVER);
            if (!ret) {
                aos_sem_signal(&g_sem);
                LOGI(TAG, "sem signal");
            }
        } else {
            LOGE(TAG, "aos_sem_wait fail");
        }
    } else {
        aos_sem_signal(&g_sem);
        LOGI(TAG, "sem signal");
    }
    aos_task_exit(0);
}

static void task_hi_task()
{
    int ret = -1;

    LOGI(TAG, "task_hi_task start,get sem wait forever");
    ret = aos_sem_wait(&g_sem, AOS_WAIT_FOREVER);
    if (!ret) {
        LOGI(TAG, "sem wait");
    } else {
        LOGE(TAG, "wait sem fail");
    }
    LOGI(TAG, "will delay 200ms");
    aos_msleep(200);
    aos_sem_signal(&g_sem);
    LOGI(TAG, "sem signal");
    aos_task_exit(0);
}


void sem_example_main(void)
{
    int ret = -1;
    aos_task_t task_hi_handle = NULL;
    aos_task_t task_lo_handle = NULL;

    ret = aos_sem_new(&g_sem, 1);
    if (ret) {
        LOGE(TAG, "aos_sem_new fail.");
        return;
    }
    ret = aos_task_new_ext(&task_hi_handle, "task_hi task", task_hi_task, NULL, CONFIG_APP_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI-1);
    if (ret) {
        LOGE(TAG, "fail to create task.");
        aos_sem_free(&g_sem);
        return;
    }
    ret = aos_task_new_ext(&task_lo_handle, "task_lo task", task_lo_task, NULL, CONFIG_APP_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI);
    if (ret) {
        LOGE(TAG, "fail to create task.");
        aos_task_delete(&task_hi_handle);
        aos_sem_free(&g_sem);
        return;
    }
    aos_msleep(300);
    aos_sem_free(&g_sem);
}
