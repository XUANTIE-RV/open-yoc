/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>

#include <yoc/mic.h>
#include <yoc/mic_port.h>

#include <cxvision/cxvision.h>

#include "voice_thead.h"
#include "dispatch_process.h"

#define TAG "VioceTHEAD"

#define MIN(x, y) ((x) > (y) ? (y) : (x))

#define FRAME_SIZE ((16000 / 1000) * (16 / 8) * 20) /* 640 */

static voice_t g_voice_priv;

using SessionMessageT = posto::Message<thead::voice::proto::SessionMsg>;
using AlgCmdMessageT  = posto::Message<thead::voice::proto::AlgCmdMsg>;

static std::shared_ptr<posto::Participant>              participant_;
static std::shared_ptr<posto::Reader<SessionMessageT> > reader_;
static std::shared_ptr<posto::Writer<AlgCmdMessageT> >  algcmd_writer_;

static int voice_set_push2talk(int mode)
{
    auto msg = std::make_shared<AlgCmdMessageT>();

    msg->body().set_cmd(thead::voice::proto::PUSH2TALK_CMD);
    msg->body().set_push2talk_mode(mode);

    algcmd_writer_->Write(msg);

    return 0;
}

static void voice_session_sub(void)
{
    reader_
        = participant_->CreateReader<SessionMessageT>("SessionMsg", [](const std::shared_ptr<SessionMessageT> &msg) {
              mic_kws_t      result    = { 0 };
              int            data_size = 0;
              mic_event_id_t evt_id    = MIC_EVENT_INVALID;

              LOGI(TAG, "voice_session_sub msg sub cmd id = %d", msg->body().cmd_id());

              switch (msg->body().cmd_id()) {
                  case thead::voice::proto::BEGIN:
                      evt_id             = MIC_EVENT_SESSION_START;
                      g_voice_priv.state = VOICE_STATE_BUSY;

                      memcpy(result.word, msg->body().kws_word().c_str(), 32);
                      result.id    = msg->body().kws_id();
                      result.score = msg->body().kws_score();
                      data_size    = sizeof(mic_kws_t);

                      break;

                  case thead::voice::proto::END:
                  case thead::voice::proto::TIMEOUT:
                      evt_id             = MIC_EVENT_SESSION_STOP;
                      g_voice_priv.state = VOICE_STATE_IDLE;
                      break;

                  case thead::voice::proto::WWV:
                      aos_sem_signal(&g_voice_priv.kws_sem);
                      return;

                  default:
                      break;
              }

              g_voice_priv.event_cb(g_voice_priv.mic, evt_id, (void *)&result, data_size);
          });
}

static void plugin_task_entry(void *arg)
{
    static const std::string json_str = R"({
      "pipeline_0": {
        "data_input": {
          "plugin": "DataInput",
          "props": {
              "chn_num": "3",
              "interleaved": "0",
              "pcm_bits": "16",
              "sample_rate": "16000",
              "frame_ms": "10"
          },
          "next": ["pre_process", "record_process#0"]
        },
        "pre_process": {
          "plugin": "PreProcess",
          "next": ["inference", "post_process#0", "vad_process#0", "record_process#1"],
          "thread": {
            "priority": "28",
            "stack_size": "32768"
          }
        },
        "inference": {
          "device_id": "0",
          "plugin": "Inference",
          "props": {
            "model_path": "models/xxxx/yyyy"
          },
          "next": ["post_process#1", "vad_process#1"],
          "thread": {
            "priority": "28",
            "stack_size": "32768"
          }
        },
        "vad_process": {
          "plugin": "VadProc",
          "next": "post_process#2"
        },
        "dispatch_process": {
          "plugin": "DispatchProc"
        },
        "record_process": {
          "plugin": "RecordProc"
        },
        "post_process": {
          "plugin": "PostProcess",
          "next": "dispatch_process"
        }
      }
    })";

    // Global initialization of CxVision
    cx::Init();

    LOGD(TAG, "Start cx::Init success.\n");

    cx::GraphManager graphMgr(json_str);

    if (!graphMgr.Start()) {
        LOGD(TAG, "Start graphs failed.\n");
    }

    LOGD(TAG, "Start graphs success.\n");

    participant_ = posto::Domain::CreateParticipant("cmd_consumer");

    voice_session_sub();

    /* publish to kws about control cmd (such as play_state, push2talk) */
    algcmd_writer_ = participant_->CreateWriter<AlgCmdMessageT>("AlgCmdMsg");

    char *pcm_data = (char *)aos_malloc_check(FRAME_SIZE);
    int data_size = 0;

    while (g_voice_priv.task_running) {
        aos_sem_wait(&g_voice_priv.pcm_sem, AOS_WAIT_FOREVER);

        while ((data_size = voice_get_pcm_data(pcm_data, FRAME_SIZE)) > 0 || g_voice_priv.state == VOICE_STATE_BUSY) {
            if (data_size > 0) {
                g_voice_priv.event_cb(g_voice_priv.mic, MIC_EVENT_PCM_DATA, pcm_data, data_size);
            }
            aos_msleep(20);
        }
    }

    aos_task_exit(0);
}

static int mic_adaptor_init(mic_t *mic, mic_event_t event)
{
    g_voice_priv.event_cb = event;

    g_voice_priv.mic = mic;

    int ret = aos_sem_new(&g_voice_priv.pcm_sem, 0);
    if (ret < 0) {
        return -1;
    }

  return 0;
}

static int mic_adaptor_deinit(mic_t *mic)
{
    aos_sem_free(&g_voice_priv.pcm_sem);
    return 0;
}


static int mic_adaptor_start(mic_t *mic)
{
    g_voice_priv.task_running = 1;

    aos_task_new_ext(&g_voice_priv.plugin_task, "voice_thead", &plugin_task_entry, NULL, 1024 * 8, AOS_DEFAULT_APP_PRI);

  return 0;
}

static int mic_adaptor_stop(mic_t *mic)
{
    g_voice_priv.task_running = 0;

    return 0;
}

static int mic_adaptor_pcm_data_control(mic_t *mic, int enable)
{
    LOGD(TAG, "pcm_control_update enable %d", enable);
    if (enable) {
        aos_sem_signal(&g_voice_priv.pcm_sem);
    }

    return 0;
}

static int mic_adaptor_set_push2talk(mic_t *mic, int mode)
{
    int ret = -1;

    ret = voice_set_push2talk(mode);

    return ret;
}

static int mic_adaptor_wakeup_notify_play_status(mic_t *mic, int play_status, int timeout)
{
    auto msg = std::make_shared<AlgCmdMessageT>();

    msg->body().set_cmd(thead::voice::proto::PLAYSTATE_CMD);
    msg->body().set_wake_ack_play_status(play_status);
    msg->body().set_wake_ack_play_timeout(timeout);

    algcmd_writer_->Write(msg);

    return 0;
}

static mic_ops_t voice_ops = {
    .init   = mic_adaptor_init,
    .deinit = mic_adaptor_deinit,

    .start = mic_adaptor_start,
    .stop  = mic_adaptor_stop,

    .pcm_data_control = mic_adaptor_pcm_data_control,

    .set_push2talk      = mic_adaptor_set_push2talk,
    .notify_play_status = mic_adaptor_wakeup_notify_play_status,
};

void aui_mic_register(void)
{
    mic_ops_register(&voice_ops);
}
