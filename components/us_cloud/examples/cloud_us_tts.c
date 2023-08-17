/**
 * @file cloud_us_tts.h
 * @brief KWS Engine C/C++ API Header
 * @authors jiangchuang@unisound.com
 * @copyright 2023-2023 Unisound AI Technology Co., Ltd. All rights reserved.
 */

#include <stdbool.h>
#include <aos/debug.h>
#include <ulog/ulog.h>

#include <av/media.h>
#include <av/avutil/named_straightfifo.h>
#include "uni_tts.h"
#include "cloud_us_tts.h"

typedef enum {
    TTS_STATE_IDLE = 0,
    TTS_STATE_RUN,
    TTS_STATE_PLAY
} tts_state_t;

#define TAG "UsTtsCloud"

static tts_state_t g_tts_state       = TTS_STATE_IDLE;

static nsfifo_t             *g_aui_fifo     = NULL;
static char                 *g_us_tts_fifo = NULL;

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

static int nsfifo_is_reof(nsfifo_t *fifo)
{
    int     ret;
    uint8_t reof;
    uint8_t weof;
    ret = nsfifo_get_eof(fifo, &reof, &weof);

    return (ret == 0) ? reof : 1;
}

static int _tts_cb(void *usr_ptr, UniTtsState state, const uint8_t *data, int data_len) {
  int total_len;
  int   reof;
  char *pos;
  total_len = data_len;

  aui_t *aui = (aui_t *)usr_ptr;
  if (state != US_TTS_RUNNING) {
    nsfifo_set_eof(g_aui_fifo, 0, 1); // set weof
    g_tts_state = TTS_STATE_IDLE;
    return 0;
  }

  if (TTS_STATE_RUN == g_tts_state) {
      if (g_aui_fifo) {
          nsfifo_set_eof(g_aui_fifo, 0, 1); // set weof
      }

      nsfifo_reset(g_aui_fifo);

      if (aui->cb.tts_cb) {
          aui->cb.tts_cb(g_us_tts_fifo, strlen(g_us_tts_fifo), aui->cb.tts_priv);
      }

      g_tts_state = TTS_STATE_PLAY;
      LOGD(TAG, "%s: g_tts_state set to %d", __FUNCTION__, g_tts_state);
  }

  while (total_len > 0) {
      reof = nsfifo_is_reof(g_aui_fifo); /** peer read reach to end */
      if (reof) {
          // LOGD(TAG, "named fifo read eof");
          break;
      }

      int len = nsfifo_get_wpos(g_aui_fifo, &pos, 500);
      if (len <= 0) {
          continue;
      }

      len = MIN(len, total_len);
      memcpy(pos, (char *)data + (data_len - total_len), len);
      nsfifo_set_wpos(g_aui_fifo, len);

      total_len -= len;
  }

  return 0;
}

static int us_start_tts(aui_t *aui)
{
    nsfifo_set_eof(g_aui_fifo, 0, 1); // set weof
    g_tts_state = TTS_STATE_IDLE;
    return 0;
}

static int us_stop_tts(aui_t *aui)
{
    g_tts_state = TTS_STATE_IDLE;

    TtsStop();

    return 0;
}

/**
 * 对文本进行tts播放
 */
static int us_req_tts(aui_t *aui, const char *text)
{
    int8_t retry_cnt = 0;

    LOGD(TAG, "Enter %s", __FUNCTION__);

    if (!text || strlen(text) == 0) {
        LOGD(TAG, "text input none");
        return -3;
    }

    if (TtsPlayString((char*)text, _tts_cb, aui) != 0) {
       LOGD(TAG, "uni play tts fail");
       return -1;
    }

    g_tts_state = TTS_STATE_RUN;

    while (TTS_STATE_RUN == g_tts_state) {
        aos_msleep(100);
        retry_cnt++;
        if (50 == retry_cnt) { // wait 50 * 100ms = 5s
            LOGE(TAG, "%s timeout!", __FUNCTION__);
            break;
        }
    }


    return 0;
}

static int us_tts_init(aui_t *aui)
{
    if (aui == NULL) {
        LOGE(TAG, "us tts init");
        return -1;
    }

    g_us_tts_fifo = "fifo://ustts"; //"mp3"

    g_aui_fifo = nsfifo_open(g_us_tts_fifo, O_CREAT, 10 * 16 * 1024);

    TtsCreate("/data/config_file");
    return 0;
}

static aui_tts_cls_t g_tts_cls = {
    .init    = us_tts_init,
    .start   = us_start_tts,
    .stop    = us_stop_tts,
    .req_tts = us_req_tts
};

void aui_us_tts_register(aui_t *aui, aui_tts_cb_t cb, void *priv)
{
    aos_check_param(aui);
    aui_cloud_tts_register(aui, &g_tts_cls, cb, priv);
}

//通用注册接口
void aui_tts_register(aui_t *aui, aui_tts_cb_t cb, void *priv)
{
    aos_check_param(aui);
    aui_cloud_tts_register(aui, &g_tts_cls, cb, priv);
}
