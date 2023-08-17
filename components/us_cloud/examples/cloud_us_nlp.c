/**
 * @file cloud_us_nlp.h
 * @brief KWS Engine C/C++ API Header
 * @authors jiangchuang@unisound.com
 * @copyright 2023-2023 Unisound AI Technology Co., Ltd. All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include "aos/kernel.h"
#include <yoc/aui_cloud.h>
#include <ulog/ulog.h>

#include "uni_rasr.h"
#include "cloud_us_nlp.h"

#define TAG "UsCloud"
#define MAX_RASR_MSG_NUM 6

typedef struct {
  char *msg;
  int msg_size;
} RasrRstMsg;

typedef struct {
  aos_task_t asr_task;
  aos_queue_t asr_queue;
  RasrHandle asr_hdl;
} UsRasr;

static UsRasr g_us_rasr = {0};

static void us_nlp_deinit(void) {
  char *que_buf = NULL;

  if (g_us_rasr.asr_hdl) {
    RasrDestroy(g_us_rasr.asr_hdl);
    g_us_rasr.asr_hdl = NULL;
  }

  que_buf = aos_queue_buf_ptr(&g_us_rasr.asr_queue);
  if (que_buf) {
    aos_queue_free(&g_us_rasr.asr_queue);
    free(que_buf);
  }

  aos_task_delete(&g_us_rasr.asr_task);
  return;
}

static int _us_asr_cb(void *arg, char *data, int size) {
  RasrRstMsg msg = {0};
  msg.msg = malloc(size);
  if (!msg.msg) {
    LOGE(TAG, "malloc asr rst size = %d fail", size);
    return 0;
  }
  strcpy(msg.msg, data);
  msg.msg_size = size;
  aos_queue_send(&g_us_rasr.asr_queue, &msg, sizeof(msg));
  return 0;
}

static void rasr_task(void *priv) {
  RasrRstMsg rasr_msg;
  size_t msg_size = 0;
  aui_audio_req_ack_info_t req_ack_info = {0};
  aui_t *aui = (aui_t *)priv;

  while (1) {
    if (aos_queue_recv(&g_us_rasr.asr_queue, AOS_WAIT_FOREVER, &rasr_msg, &msg_size) != 0) {
      LOGE(TAG, "asr queue recv fail");
      continue;
    }

    LOGD(TAG, "%s normal asr\n", __func__);
    req_ack_info.data_type = AUI_AUDIO_REQ_TYPE_NLP;
    req_ack_info.data     = rasr_msg.msg;
    req_ack_info.len      = rasr_msg.msg_size;

    if (aui->cb.nlp_cb) {
        aui->cb.nlp_cb((void *)&req_ack_info, sizeof(aui_audio_req_ack_info_t), aui->cb.nlp_priv);
    }

    if (rasr_msg.msg) {
      free(rasr_msg.msg);
    }
  }
}


/** init only once with multiple talks */
static int us_nlp_init(aui_t *aui)
{
    aos_check_return_einval(aui);

    if (g_us_rasr.asr_hdl) {
        return 0;
    }

  do {
    /* 创建在线识别模块, 并注册识别结果回调 */
    g_us_rasr.asr_hdl = RasrCreate("/data/config_file", _us_asr_cb, NULL);
    if (g_us_rasr.asr_hdl == NULL) {
        LOGE(TAG, "rasr create fail");
        return 0;
    }
    if (aos_queue_create(&g_us_rasr.asr_queue, MAX_RASR_MSG_NUM * sizeof(RasrRstMsg), sizeof(RasrRstMsg), 0) != 0) {
      LOGE(TAG, "rasr create queue fail");
      break;
    }
    if (aos_task_new_ext(&g_us_rasr.asr_task, "us_rasr", rasr_task, aui, 16 * 1024, 25) != 0) {
      LOGE(TAG, "rasr create task fail");
      break;
    }
    LOGI(TAG, "us nlp init succ");
    return 0;
  } while (0);

  us_nlp_deinit();
  return -1;
}

static int us_start_pcm(aui_t *aui)
{
    aos_check_return_einval(aui);

    if (aui->audio_req_type == AUI_AUDIO_REQ_TYPE_WWV) {
      return 0;
    }

    if (g_us_rasr.asr_hdl) {
      LOGD(TAG, "nui_things_start success");
      RasrStart();
    }

    return 0;
}

static int us_push_pcm(aui_t *aui, void *data, size_t size)
{
    aos_check_return_einval(aui && data && size);

    if (aui->audio_req_type == AUI_AUDIO_REQ_TYPE_WWV) {
      return 0;
    }

    if (g_us_rasr.asr_hdl) {
      RasrAcquist(data, size);
    }

    return 0;
}

static int us_stop_pcm(aui_t *aui)
{
    aos_check_return_einval(aui);

    if (aui->audio_req_type == AUI_AUDIO_REQ_TYPE_WWV) {
      return 0;
    }

    if (g_us_rasr.asr_hdl) {
      RasrStop();
    }

    return 0;
}

static int us_force_stop(aui_t *aui)
{
    aos_check_return_einval(aui);
    if (aui->audio_req_type == AUI_AUDIO_REQ_TYPE_WWV) {
      return 0;
    }

    if (g_us_rasr.asr_hdl) {
      RasrStop();
    }

    return 0;
}

static aui_nlp_cls_t us_nlp_cls = {
    .init           = us_nlp_init,
    .start          = us_start_pcm,
    .push_data      = us_push_pcm,
    .stop_push_data = us_stop_pcm,
    .push_text      = NULL,
    .stop           = us_force_stop
};

void aui_us_nlp_register(aui_t *aui, aui_nlp_cb_t cb, void *priv)
{
    aos_check_param(aui);
    aui_cloud_nlp_register(aui, &us_nlp_cls, cb, priv);
}

//通用注册接口
void aui_nlp_register(aui_t *aui, aui_nlp_cb_t cb, void *priv)
{
    aos_check_param(aui);
    aui_cloud_nlp_register(aui, &us_nlp_cls, cb, priv);
}
