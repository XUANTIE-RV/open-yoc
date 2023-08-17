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

#include "voice_mind.h"
#include "dispatch_process.h"

#define TAG "VioceMinD"

#define MIN(x, y) ((x) > (y) ? (y) : (x))

#define FRAME_SIZE       ((16000 / 1000) * (16 / 8) * 20) /* 640Byte 20ms*/
#define MAX_KWS_DATA_LEN (64 * 1024)                      /* 2s PCM */

static voice_t g_voice_priv;

using SessionMessageT = posto::Message<thead::voice::proto::SessionMsg>;
using AlgCmdMessageT  = posto::Message<thead::voice::proto::AlgCmdMsg>;

static std::shared_ptr<posto::Participant>              participant_;
static std::shared_ptr<posto::Reader<SessionMessageT> > reader_;
static std::shared_ptr<posto::Writer<AlgCmdMessageT> >  algcmd_writer_;

static void voice_session_sub(void)
{
    reader_
        = participant_->CreateReader<SessionMessageT>("SessionMsg", [](const std::shared_ptr<SessionMessageT> &msg) {
              mic_kws_t      result    = { MIC_WAKEUP_TYPE_NONE, 0, 0, 0, "" };
              int            data_size = 0;
              mic_event_id_t evt_id    = MIC_EVENT_INVALID;

              LOGI(TAG, "SessionMsg got,cmd_id: %d", msg->body().cmd_id());

              switch (msg->body().cmd_id()) {
                  case thead::voice::proto::BEGIN:
                      evt_id = MIC_EVENT_SESSION_START;

                      strlcpy(result.word, msg->body().kws_word().c_str(), 32);
                      result.type  = (mic_wakeup_type_t)msg->body().wakeup_type();
                      result.id    = msg->body().kws_id();
                      result.score = msg->body().kws_score();
                      result.doa   = msg->body().kws_doa();
                      data_size    = sizeof(mic_kws_t);
                      g_voice_priv.event_cb(g_voice_priv.mic, evt_id, (void *)&result, data_size);
                      break;

                  case thead::voice::proto::END:
                  case thead::voice::proto::TIMEOUT:
                      evt_id = MIC_EVENT_SESSION_STOP;
                      g_voice_priv.event_cb(g_voice_priv.mic, evt_id, (void *)&result, data_size);
                      break;

                  case thead::voice::proto::WWV: {
                      if (g_voice_priv.kws_data == NULL) {
                          g_voice_priv.kws_data = (char *)aos_malloc_check(MAX_KWS_DATA_LEN);
                      }

                      evt_id                    = MIC_EVENT_KWS_DATA;
                      data_size                 = voice_get_kws_data(g_voice_priv.kws_data, MAX_KWS_DATA_LEN);
                      g_voice_priv.kws_data_len = data_size;
                      g_voice_priv.event_cb(g_voice_priv.mic, evt_id, (void *)g_voice_priv.kws_data, data_size);
                  } break;
                  case thead::voice::proto::DOA: {
                      evt_id = MIC_EVENT_SESSION_DOA;
                      LOGI(TAG, "SessionMsg got, doa: %d", msg->body().kws_doa());
                      g_voice_priv.event_cb(g_voice_priv.mic, evt_id, (void *)(long)msg->body().kws_doa(), 0);
                  } break;
                  case thead::voice::proto::ASR: {
                      evt_id                 = MIC_EVENT_LOCAL_ASR;
                      data_size              = msg->body().kws_word().length() + 1;
                      const char *asr_result = msg->body().kws_word().c_str();
                      // LOGI(TAG, "SessionMsg got, asr: %s %d", asr_result, data_size);
                      g_voice_priv.event_cb(g_voice_priv.mic, evt_id, (void *)asr_result, data_size);
                  } break;
                  case thead::voice::proto::VAD_BEGIN:
                      evt_id = MIC_EVENT_VAD_BEGIN;
                      g_voice_priv.event_cb(g_voice_priv.mic, evt_id, (void *)&result, data_size);
                      break;
                  case thead::voice::proto::VAD_END:
                      evt_id = MIC_EVENT_VAD_END;
                      g_voice_priv.event_cb(g_voice_priv.mic, evt_id, (void *)&result, data_size);
                      break;
                  default:
                      break;
              }
          });
}


static void plugin_task_entry(void *arg)
{
#if defined(CONFIG_ALG_ASR_LYEVA) && CONFIG_ALG_ASR_LYEVA
    static const std::string json_str = R"({
      "pipeline_0": {
        "data_input": {
          "plugin": "DataInput",
          "props": {
              "chn_num": "3",
              "interleaved": "0",
              "pcm_bits": "16",
              "sample_rate": "16000",
              "frame_ms": "20"
          },
          "next": ["mind_process", "record_process#0"]
        },
        "record_process": {
          "plugin": "RecordProc"
        },
        "dispatch_process": {
          "plugin": "DispatchProc"
        },
        "mind_process": {
          "device_id": "0",
          "plugin": "MindProcess",
          "next": [ "lyeva_asr_process","dispatch_process","record_process#1"],
          "thread": {
            "priority": "28",
            "stack_size": "16384"
          }
        },
        "lyeva_asr_process": {
          "device_id": "0",
          "plugin": "LyevaASRProcess",
          "next": ["record_process#1"],
          "thread": {
            "priority": "28",
            "stack_size": "16384"
          }
        }
      }
    })";
#elif defined(CONFIG_ALG_US) && CONFIG_ALG_US
    static const std::string json_str = R"({
      "pipeline_0": {
        "data_input": {
          "plugin": "DataInput",
          "props": {
              "chn_num": "3",
              "interleaved": "0",
              "pcm_bits": "16",
              "sample_rate": "16000",
              "frame_ms": "20"
          },
          "next": ["audio_example_process0", "record_process#0"]
        },
        "record_process": {
          "plugin": "RecordProc"
        },
        "audio_example_process0": {
          "device_id": "0",
          "plugin": "AudioExampleProcess0",
          "next": ["audio_example_process1", "record_process#1", "dispatch_process"],
          "thread": {
            "priority": "29",
            "stack_size": "40960"
          }
        },
        "audio_example_process1": {
          "device_id": "0",
          "plugin": "AudioExampleProcess1",
          "thread": {
            "priority": "29",
            "stack_size": "40960"
          }
        },
        "dispatch_process": {
          "plugin": "DispatchProc"
        }
      }
    })";
#else
    static const std::string json_str = R"({
      "pipeline_0": {
        "data_input": {
          "plugin": "DataInput",
          "props": {
              "chn_num": "3",
              "interleaved": "0",
              "pcm_bits": "16",
              "sample_rate": "16000",
              "frame_ms": "20"
          },
          "next": ["mind_process", "record_process#0"]
        },
        "record_process": {
          "plugin": "RecordProc"
        },
        "dispatch_process": {
          "plugin": "DispatchProc"
        },
        "mind_process": {
          "device_id": "0",
          "plugin": "MindProcess",
          "next": ["record_process#1", "dispatch_process"],
          "thread": {
            "priority": "28",
            "stack_size": "40960"
          }
        }
      }
    })";
#endif

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

    /* mic_adaptor_start wait thsi signal */
    aos_sem_signal(&g_voice_priv.start_sem);

    char *pcm_data  = (char *)aos_malloc_check(FRAME_SIZE);
    int   data_size = 0;

    while (g_voice_priv.task_running) {
        aos_sem_wait(&g_voice_priv.pcm_sem, AOS_WAIT_FOREVER);

        while (g_voice_priv.pcm_output_en) {
            data_size = voice_get_pcm_data(pcm_data, FRAME_SIZE);
            if (data_size > 0) {
                g_voice_priv.event_cb(g_voice_priv.mic, MIC_EVENT_PCM_DATA, pcm_data, data_size);
            } else {
                aos_msleep(20);
            }
        }
    }

    g_voice_priv.task_exit = 1;
    if (g_voice_priv.kws_data) {
        aos_free(g_voice_priv.kws_data);
    }
    aos_task_exit(0);
}

static int mic_adaptor_init(mic_t *mic, mic_event_t event)
{
    g_voice_priv.event_cb = event; // aui_mic_imp.c:mic_event_hdl

    g_voice_priv.mic           = mic;
    g_voice_priv.pcm_output_en = 0;

    aos_sem_new(&g_voice_priv.pcm_sem, 0);
    aos_sem_new(&g_voice_priv.start_sem, 0);

    return 0;
}

static int mic_adaptor_deinit(mic_t *mic)
{
    /* The voice service runs throughout its lifecycle and does not need to be released. */
#if 0
    aos_check_return_einval(!g_voice_priv.task_running);

    aos_sem_free(&g_voice_priv.pcm_sem);
#endif
    return 0;
}

static int mic_adaptor_start(mic_t *mic)
{
    if (g_voice_priv.task_running) {
      return 0;
    }

    g_voice_priv.task_running = 1;
    g_voice_priv.task_exit    = 0;
    g_voice_priv.task_start   = 0;

    aos_task_new_ext(&g_voice_priv.plugin_task, "voice_mind", &plugin_task_entry, NULL, 2 * 1024 * 8, 11);
    aos_sem_wait(&g_voice_priv.start_sem, AOS_WAIT_FOREVER);
    g_voice_priv.task_start   = 1;

    return 0;
}

static int mic_adaptor_stop(mic_t *mic)
{
    /* The voice service runs throughout its lifecycle and does not need to be released. */
#if 0
    aos_check_return_einval(g_voice_priv.task_running);

    g_voice_priv.task_running = 0;

    aos_sem_signal(&g_voice_priv.pcm_sem);

    while (!g_voice_priv.task_exit) {
        aos_msleep(20);
    };
#endif

    return 0;
}

static int mic_adaptor_pcm_data_control(mic_t *mic, int enable)
{
    LOGD(TAG, "pcm_control_update enable %d", enable);
    g_voice_priv.pcm_output_en = enable;

    if (enable) {
        aos_sem_signal(&g_voice_priv.pcm_sem);
    }

    return 0;
}

static int mic_adaptor_set_push2talk(mic_t *mic, int mode)
{
    if (g_voice_priv.task_start == 0) {
        return -1;
    }

    auto msg = std::make_shared<AlgCmdMessageT>();

    msg->body().set_cmd(thead::voice::proto::PUSH2TALK_CMD);
    msg->body().set_ivalue(mode);

    algcmd_writer_->Write(msg);

    return 0;
}

static int mic_adaptor_wakeup_notify_play_status(mic_t *mic, int play_status, int timeout)
{
    if (g_voice_priv.task_start == 0) {
        return -1;
    }

    auto msg = std::make_shared<AlgCmdMessageT>();

    msg->body().set_cmd(thead::voice::proto::PLAYSTATE_CMD);
    msg->body().set_ivalue(play_status);
    msg->body().set_ivalue2(timeout);

    algcmd_writer_->Write(msg);

    return 0;
}

static int mic_adaptor_set_wakup_level(mic_t *mic, char *wakeup_word, int level)
{
    if (g_voice_priv.task_start == 0) {
        return -1;
    }

    auto msg = std::make_shared<AlgCmdMessageT>();

    msg->body().set_cmd(thead::voice::proto::WAKEUP_LEVEL_CMD);
    msg->body().set_ivalue(level);
    msg->body().set_strvalue(wakeup_word);

    algcmd_writer_->Write(msg);

    return 0;
}

static int mic_adaptor_start_doa(mic_t *mic)
{
    if (g_voice_priv.task_start == 0) {
        return -1;
    }

    auto msg = std::make_shared<AlgCmdMessageT>();

    msg->body().set_cmd(thead::voice::proto::START_DOA_CMD);

    algcmd_writer_->Write(msg);
    return 0;
}

static int mic_adaptor_enable_linear_aec_data(mic_t *mic, int enable)
{
    if (g_voice_priv.task_start == 0) {
        return -1;
    }

    auto msg = std::make_shared<AlgCmdMessageT>();

    msg->body().set_cmd(thead::voice::proto::ENABLE_LINEAR_AEC_DATA_CMD);
    msg->body().set_ivalue(enable);

    algcmd_writer_->Write(msg);
    return 0;
}

static int mic_adaptor_asr(mic_t *mic, int enable)
{
    if (g_voice_priv.task_start == 0) {
        return -1;
    }

    auto msg = std::make_shared<AlgCmdMessageT>();

    msg->body().set_cmd(thead::voice::proto::ENABLE_ASR);
    msg->body().set_ivalue(enable);

    algcmd_writer_->Write(msg);
    return 0;
}

static mic_ops_t voice_ops = {
    .init   = mic_adaptor_init,
    .deinit = mic_adaptor_deinit,

    .start = mic_adaptor_start,
    .stop  = mic_adaptor_stop,

    .pcm_data_control = mic_adaptor_pcm_data_control,

    .set_push2talk          = mic_adaptor_set_push2talk,
    .notify_play_status     = mic_adaptor_wakeup_notify_play_status,
    .set_wakeup_level       = mic_adaptor_set_wakup_level,
    .start_doa              = mic_adaptor_start_doa,
    .enable_linear_aec_data = mic_adaptor_enable_linear_aec_data,
    .enable_asr             = mic_adaptor_asr,
};

void aui_mic_register(void)
{
    mic_ops_register(&voice_ops);
}
