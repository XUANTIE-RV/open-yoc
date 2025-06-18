#include <aos/aos.h>
#include <aos/cli.h>

#define TAG                         "soc_message_q"
#define MSG_NUM                     5
#define QUEUE_BUF_LEN               512
#define MAX_MSG_LEN                 64
#define TIMEOUT                     100

static aos_queue_t test_queue;
static char queue_buf[QUEUE_BUF_LEN] = {0};


static void send_entry(void *arg)
{
    uint32_t i = 0;
    int ret = -1;
    int message_size;

    LOGI(TAG, "send_entry start .");

    char abuf0[] = "test is message 0";
    char abuf1[] = "test is message 1";
    char abuf2[] = "test is message 2";
    char abuf3[] = "test is message 3";
    char abuf4[] = "test is message 4";
    char error_buf[] = "this is an error";
    char *buf_p[MSG_NUM + 1] = {abuf0, abuf1, abuf2, abuf3, abuf4, error_buf};

    while (i < 5) {
        if (buf_p[i] == NULL) {
            break;
        }
        message_size = strlen(buf_p[i]);
        ret = aos_queue_send(&test_queue, buf_p[i], message_size);
        if (ret) {
            LOGE(TAG, "aos_queue_send fail .");
            break;
        }
        i++;
        uint32_t count = aos_queue_get_count(&test_queue);
        LOGI(TAG, "send_entry:number of queued mesages : %u .", count);
        aos_msleep(10);
    }
    aos_task_exit(0);
}

static void recv_entry(void *arg)
{
    int  ret = -1;
    int loop_times = 0;
    char Readbuf[64] = {0};
    size_t size = 0;

    LOGI(TAG, "send_entry start .");
    aos_msleep(100);
    while (1) {
        ret = aos_queue_recv(&test_queue, TIMEOUT, Readbuf, &size);
        if (ret) {
            if (loop_times==5) {
                LOGI(TAG, "queue recv finish .");
            } else {
                LOGE(TAG, "in expected, fail to recv message,error:%x\n", ret);
            }
            break;
        }
        LOGI(TAG, "recv message:%s, recv message size:%u\n", Readbuf, size);
        uint32_t count = aos_queue_get_count(&test_queue);
        LOGI(TAG, "number of queued mesages : %u .", count);
        loop_times++;
        aos_msleep(10);
    }
    aos_msleep(100);
    aos_queue_free(&test_queue);
    aos_task_exit(0);
}

void message_example_main(void)
{
    int ret = -1;
    aos_task_t send_handle;
    aos_task_t recv_handle;

    ret = aos_queue_new(&test_queue, queue_buf, QUEUE_BUF_LEN, MAX_MSG_LEN);
    if (ret) {
        LOGE(TAG, "aos_queue_new error");
        return;
    }
    ret = aos_task_new_ext(&send_handle, "send_task", send_entry, NULL, CONFIG_APP_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI);
    if (ret) {
        LOGE(TAG, "fail to create task.");
        aos_queue_free(&test_queue);
        return;
    }
    ret = aos_task_new_ext(&recv_handle, "recv_task", recv_entry, NULL, CONFIG_APP_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI);
    if (ret) {
        aos_task_delete(&send_handle);
        aos_queue_free(&test_queue);
        LOGE(TAG, "fail to create task.");
        return;
    }
}