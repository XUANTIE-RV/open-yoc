#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/kernel.h>
#include <aos/log.h>

#include "dspalg_cxc_msg.h"
#include "dspalg_ringbuf.h"

#include "huwen_yv_ap.h"

#define TAG "michw"

static aos_task_t       task_handle;
static char *           g_pcm_recv_buffer = NULL; /* 接收buffer，双通道 */
static char *           g_pcm_mic_buffer  = NULL; /* 给mic层的数据,单通道 */
static aos_sem_t        g_sem_cxc_msg_event;
static silan_cxc_mesg_t g_cxc_msg_cur = {
    0,
};

static mic_event_t g_mic_evt_cb = NULL;
static mic_t *     g_mic_handle = NULL;

static struct _yv_ctrl {
    int yv_en;
    int yv_pcm_en;
    int yv_wake_sim_flag;
} g_yv_ctrl = {1, 0, 0};

/*************************
 * 调试命令处理
*************************/
static int         g_yv_debug      = 0;
static mic_event_t g_yv_event_hook = NULL;

void huwen_yv_debug_hook(mic_event_t hook, int dbg_level)
{
    g_yv_event_hook = hook;
    g_yv_debug      = dbg_level;
}

static int yv_debug_hook_call(mic_event_id_t evt_id, void *data, int len)
{
    switch (g_yv_debug) {
        case 1:
            if (g_yv_event_hook) {
                g_yv_event_hook(NULL, evt_id, data, len);
            }
            return 0;
        case 2:
            if (g_yv_event_hook) {
                g_yv_event_hook(NULL, evt_id, data, len);
            }
            return 1; /* 走正常的交互流程 */
        default:
            return 1;
    }

    return 1;
}

static int pcm_convert_dual2mono(char *dual, int dual_byte, char *mono, int mono_byte)
{
    int i;

    if (dual_byte != (mono_byte * 2) || dual == NULL || mono == NULL) {
        LOGE(TAG, "pcm conv param");
        return -1;
    }

    short *short_dual   = (short *)dual;
    short *short_mono   = (short *)mono;
    int    sample_count = mono_byte / 2;

    for (i = 0; i < sample_count; i++) {
        short_mono[i] = short_dual[i * 2];
    }

    return 0;
}

static void yv_mic_event_call(mic_event_id_t evt_id, void *data, int size)
{
    /* g_yv_debug == 1 屏蔽所有DSP事件 */
    if (g_mic_evt_cb && (g_yv_debug != 1)) {
        g_mic_evt_cb(g_mic_handle, evt_id, data, size);
    }
}

/*************************
 * 交互任务处理
*************************/
#define READ_BLOCK_SIZE 4096
static void michw_thread(void *arg)
{
    int       ret       = 0;
    int       kwid      = 0;
    int       wake_count = 0;
    long long wake_time = 0;

    while (1) {
        aos_sem_wait(&g_sem_cxc_msg_event, 40 /*ms*/);

        /* 唤醒事件检查 */
        if (g_cxc_msg_cur.main_cmd) {
#if 0
            printf("mesg{%d,%d,%d,%d,0x%08x}\n",
                g_cxc_msg_cur.main_cmd, g_cxc_msg_cur.mode,
                g_cxc_msg_cur.sub_cmd, g_cxc_msg_cur.argc, *((int *)g_cxc_msg_cur.argv));
#endif
            g_cxc_msg_cur.main_cmd = 0;

            /* 唤醒事件回调 */
            memcpy(&kwid, g_cxc_msg_cur.argv, sizeof(int));
            if (kwid == 0) {
                wake_time = aos_now_ms();
                yv_mic_event_call(MIC_EVENT_SESSION_START, &kwid, sizeof(int));
                LOGI(TAG, "wakeup %d", ++wake_count);
            }
        } else {
            /* 断句检查 */
            if (wake_time != 0) {
                /* 断句模拟 */
                if (aos_now_ms() - wake_time > 5000) {
                    wake_time = 0;
                    yv_mic_event_call(MIC_EVENT_SESSION_STOP, NULL, 0);
                }
            }
        }

        /* 数据检查 */
        int data_len = ring_buf_fullness(DEV_DSP);
        if (data_len >= READ_BLOCK_SIZE) {
            ret = ring_buf_rd_data(DEV_DSP, g_pcm_recv_buffer, READ_BLOCK_SIZE);
            if (ret == READ_BLOCK_SIZE) {
                ret = yv_debug_hook_call(MIC_EVENT_PCM_DATA, g_pcm_recv_buffer, READ_BLOCK_SIZE);

                /* PCM数据回调 */
                if (ret && g_yv_ctrl.yv_pcm_en) {
                    pcm_convert_dual2mono(g_pcm_recv_buffer, READ_BLOCK_SIZE, g_pcm_mic_buffer,
                                          READ_BLOCK_SIZE / 2);
                    yv_mic_event_call(MIC_EVENT_PCM_DATA, g_pcm_mic_buffer, READ_BLOCK_SIZE / 2);
                }
            } else {
                LOGW(TAG, "ring_buf read error, %d, %d", data_len, ret);
            }
        }
    } /* while */
}

/* DSP CXC中断 消息事件 */
static void cxc_msg_hdl(silan_cxc_mesg_t *mesg)
{
    g_cxc_msg_cur = *mesg;
    aos_sem_signal(&g_sem_cxc_msg_event);
    //printf("post %d\n", g_cxc_msg_cur.main_cmd);
}

int huwen_yv_init(mic_event_t cb, mic_t *mic)
{
    int ret;

    if (g_pcm_recv_buffer != NULL) {
        LOGW(TAG, "dup init");
        return 0;
    }

    silan_cxc_init(cxc_msg_hdl);
    ring_buf_init(DEV_DSP);
    g_pcm_recv_buffer = malloc(ring_buf_size(DEV_DSP));
    g_pcm_mic_buffer  = malloc(ring_buf_size(DEV_DSP) / 2);

    aos_sem_new(&g_sem_cxc_msg_event, 0);

    ret = aos_task_new_ext(&task_handle, "michw", michw_thread, NULL, 2048, AOS_DEFAULT_APP_PRI - 4);

    if (ret < 0) {
        free(g_pcm_recv_buffer);
        g_pcm_recv_buffer = NULL;
        LOGE(TAG, "create task failed.");
    }

    g_mic_evt_cb = cb;
    g_mic_handle = mic;

    return ret;
}

/*************************
 * yv API 实现
*************************/
int huwen_yv_kws_enable(int flag)
{
    g_yv_ctrl.yv_en = flag;
    return 0;
}

int huwen_yv_pcm_enable(int flag)
{
    g_yv_ctrl.yv_pcm_en = flag;
    return 0;
}
int huwen_yv_wake_trigger(int flag)
{
    g_yv_ctrl.yv_wake_sim_flag = flag;
    return 0;
}
