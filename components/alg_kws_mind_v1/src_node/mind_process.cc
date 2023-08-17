/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <board.h>

#if !(defined(CONFIG_BOARD_AMP_LOAD_FW) && CONFIG_BOARD_AMP_LOAD_FW)

#include <stdio.h>
#include <memory>

#include <aos/kernel.h>
#include <aos/ringbuffer.h>
#include <cxvision/cxvision.h>
#include <ulog/ulog.h>

#include "mit/mit_rtos.h"
#include "alg_audio.h"

#include "mind_process.h"

#define TAG "KWSMinD"

#if defined(CONFIG_ASR_RECORD) && CONFIG_ASR_RECORD
#define ASR_RECORD_CACHE_SIZE (25 * 1024)
#endif

// cxvision message
using DataInputMessageT = thead::voice::proto::DataInputMsg;
using SspOutMessageT    = thead::voice::proto::SspOutMsg;
using KwsOutMessageT    = thead::voice::proto::KwsOutMsg;

// pub/sub message
using AlgCmdMessageT  = posto::Message<thead::voice::proto::AlgCmdMsg>;
using SessionMessageT = posto::Message<thead::voice::proto::SessionMsg>;
using RecordMessageT  = posto::Message<thead::voice::proto::RecordMsg>;

namespace cpt
{

class MindProcess : public cx::PluginBase
{
public:
    MindProcess();
    bool Init(const std::map<std::string, std::string> &props) override;
    bool DeInit() override;
    bool Process(const std::vector<cx::BufferPtr> &data_vec) override;

private:
    int vad_cnt_;
    int session_start_;
    int cmd_exe_push2talk_;     /* 0:就绪状态 1:开始p2t 2:p2t交互状态 3:结束p2t */
    int push2talk_mode_;
    int cmd_exe_get_doa_;       /* 0:就绪状态 1:获取DOA */
    int start_record_;
    int asr_enable_status_;

#if defined(CONFIG_VAD_TAG) && CONFIG_VAD_TAG
    mic_vad_buf_t *vad_out_data;
#else
    char vad_out_data[5120];
#endif

#if defined(CONFIG_ASR_RECORD) && CONFIG_ASR_RECORD
    dev_ringbuf_t asr_cache;
    void *asr_cache_buffer;
#endif

    std::shared_ptr<posto::Participant>              participant_;
    std::shared_ptr<posto::Writer<SessionMessageT> > writer_;
    std::shared_ptr<posto::Reader<RecordMessageT> >  rec_reader_;
    std::shared_ptr<posto::Reader<AlgCmdMessageT> >  algcmd_reader_;

private:
    int                 vad_proc(int16_t *data_in, void **out, size_t *vad_len);
    audio_wakeup_info_t kws_proc(int16_t *data_in, int frame, int16_t **data_out, size_t *out_len);
    int                 get_kws_data(void **data, size_t *data_len);

    void KwsSendMsg(uint8_t *buf, size_t buf_len, thead::voice::proto::DataType type);
    int record_chn_count_; /* 0:不录制 3:录制3路 5:录制5路 6: 录制6路 */
    int vad_send_count_;
};

MindProcess::MindProcess()
{
    memset(vad_out_data, 0, sizeof(vad_out_data));
    vad_cnt_       = 0;
    session_start_ = 0;
    cmd_exe_push2talk_     = 0;
    push2talk_mode_ = 0;
    start_record_  = 0;
    participant_   = NULL;
    writer_        = NULL;
    rec_reader_    = NULL;
    algcmd_reader_ = NULL;
    vad_send_count_ = 0;

#ifdef CONFIG_FEOUTAEC
    audio_set_linear_aec_en_status(0);
#endif

}

int MindProcess::get_kws_data(void **data, size_t *data_len)
{
    aos_check_param(data);

    /* wwv data ready */
    int   kws_data_len;
    char *kws_data = audio_get_kws_data(&kws_data_len);
    if (kws_data != NULL) {
        *data     = kws_data;
        *data_len = (size_t)kws_data_len;
        LOGD(TAG, "kws get %p %d bytes data", kws_data, kws_data_len);
        return 0;
    }

    *data     = NULL;
    *data_len = 0;
    return -1;
}

audio_wakeup_info_t MindProcess::kws_proc(int16_t *data_in, int frame, int16_t **data_out, size_t *out_len)
{
    aos_check_param(data_in);

    return audio_process(data_in, frame, data_out, out_len);
}

int MindProcess::vad_proc(int16_t *data_in, void **out, size_t *vad_len)
{
    static int vad_detected = 0;
    int        vad_stat     = VOICE_VADSTAT_SILENCE;

    aos_check_param((vad_len != NULL) && (out != NULL) && (vad_len != NULL));

    *vad_len = 0;
    *out     = NULL;

    /* VAD state handle */
    int vadres = audio_get_vad();
    switch (vadres) {
        case -1:
            break;
        case 0:
            /* enter next state */
            if (AU_STATUS_WAKE == audio_get_status()) {
                audio_set_status(AU_STATUS_ASR);
                vad_stat = VOICE_VADSTAT_ASR;
            }
            vad_detected = 1;
            break;
        case 1:
            vad_detected = 0;
            break;
        case 2:
            vad_detected = 0;
            break;
        default:;
    }

    int    vad_data_len;
    int    vad_data_finished = 0;
    char  *vad_data;

#if defined(CONFIG_VAD_TAG) && CONFIG_VAD_TAG
    short *frame_tags;

    int ret = audio_get_tagged_vad_data(&vad_data, &vad_data_len, &frame_tags, &vad_data_finished);
    if (ret == 0) {
        static short last_tag = 0; // remember last tag to smooth out 10 ms tag to 20 ms tag

        if (!vad_out_data) {
            vad_out_data = aos_zalloc_check(sizeof(mic_vad_buf_t) + 10 * 20 * 32);
        }

        vad_out_data->len = vad_data_len;

        for (int i = 0; i < vad_data_len / 10 / 32; i++) {

            if (i % 2) {
                vad_out_data->vad_tags[i / 2] = frame_tags[i] + frame_tags[i - 1] + last_tag >= 2 ? 1 : 0;
                last_tag                      = frame_tags[i];
            }
        }

        memcpy(vad_out_data->data, vad_data, vad_data_len);
        *vad_len = sizeof(mic_vad_buf_t) + vad_data_len;

        vad_stat = VOICE_VAD_PCM_DATA;
        *out = vad_out_data;
    }
#else
    int ret = audio_get_vad_data(&vad_data, &vad_data_len, &vad_data_finished);
    if (ret == 0) {
        memcpy(vad_out_data, vad_data, vad_data_len);
        *vad_len = vad_data_len;
        *out = vad_out_data;
    }
#endif

    if (1 == vad_data_finished) {
        LOGD(TAG,
             "vad_data_finished vad_stat %d audio_stat %d, vad_detected %d",
             vad_stat,
             audio_get_status(),
             vad_detected);
    }

    if (audio_get_status() == AU_STATUS_FINI) {
        vad_stat = VOICE_VADSTAT_ASR_FINI;
        LOGD(TAG, "VAD finish");

        /* 进入下一个状态 */
        audio_set_status(AU_STATUS_READY);
    }

    return vad_stat != VOICE_VADSTAT_SILENCE ? vad_stat : (vad_detected ? VOICE_VADSTAT_VOICE : VOICE_VADSTAT_SILENCE);
}

bool MindProcess::Init(const std::map<std::string, std::string> &props)
{
    const auto &iter = props.find("model_path");
    if (iter != props.end()) {
        LOGD(TAG, "model_path: %s", iter->second.c_str());
    }

    participant_ = posto::Domain::CreateParticipant("cmd_producer");
    writer_      = participant_->CreateWriter<SessionMessageT>("SessionMsg");

    rec_reader_
        = participant_->CreateReader<RecordMessageT>("RecordMsg", [this](const std::shared_ptr<RecordMessageT> &msg) {
              LOGD(TAG, "RecordMsg got, cmd_id: %d", msg->body().cmd());
              switch (msg->body().cmd()) {
                  case thead::voice::proto::START:
                      start_record_ = 1;
                      record_chn_count_ = msg->body().record_chn_count();
#ifdef CONFIG_FEOUTAEC
                      /* 录制六路时为了避免FEOUTAEC(该数据为5路)问题, 强制关闭FEOUTAEC */
                      if (record_chn_count_ == 6) {
                          audio_set_linear_aec_en_status(0);
                      }
#endif
                      break;

                  case thead::voice::proto::STOP:
                      start_record_ = 0;
                      record_chn_count_ = 0;
                      break;

                  default:
                      break;
              }
          });

    algcmd_reader_
        = participant_->CreateReader<AlgCmdMessageT>("AlgCmdMsg", [this](const std::shared_ptr<AlgCmdMessageT> &msg) {
              LOGD(TAG, "AlgCmdMsg got, cmd: %d iv=%d iv2=%d strv=[%s]", msg->body().cmd(), msg->body().ivalue(),
                                                                         msg->body().ivalue2(), msg->body().strvalue().c_str());
              switch (msg->body().cmd()) {
                  case thead::voice::proto::PUSH2TALK_CMD: {
                      int p2tmode = msg->body().ivalue();
                      if (p2tmode) {
                          cmd_exe_push2talk_ = 1;
                          push2talk_mode_ = p2tmode;
                      } else {
                          cmd_exe_push2talk_ = 3;
                          push2talk_mode_ = 0;
                      }
                  } break;

                  case thead::voice::proto::PLAYSTATE_CMD:
                      audio_wakeup_voice_stat(msg->body().ivalue(), 20, msg->body().ivalue2());
                      break;
                  case thead::voice::proto::WAKEUP_LEVEL_CMD:
                      audio_set_wakeup_level(msg->body().strvalue().c_str(), msg->body().ivalue());
                      break;
                  case thead::voice::proto::START_DOA_CMD:
                      cmd_exe_get_doa_ = 1;
                      break;
#ifdef CONFIG_FEOUTAEC
                  case thead::voice::proto::ENABLE_LINEAR_AEC_DATA_CMD:
                      audio_set_linear_aec_en_status(msg->body().ivalue());
                      break;
#endif
                  case thead::voice::proto::ENABLE_ASR:
                      asr_enable_status_ = (bool)msg->body().ivalue();
                      break;
                  default:
                      break;
              }
          });

    vad_cnt_ = 0;
    record_chn_count_ = 0;

#if defined(CONFIG_ASR_RECORD) && CONFIG_ASR_RECORD
    this->asr_cache_buffer = malloc(ASR_RECORD_CACHE_SIZE);
    ringbuffer_create(&this->asr_cache, (char *)this->asr_cache_buffer, ASR_RECORD_CACHE_SIZE);
#endif

    asr_enable_status_ = true;

    audio_alginit();

    return true;
}

bool MindProcess::DeInit()
{
    asr_enable_status_ = false;
    record_chn_count_ = 0;
#if defined(CONFIG_ASR_RECORD) && CONFIG_ASR_RECORD
    free(this->asr_cache_buffer);
    ringbuffer_destroy(&this->asr_cache);
#endif
    return true;
}

void MindProcess::KwsSendMsg(uint8_t *buf, size_t buf_len, thead::voice::proto::DataType type)
{
    auto     oMemory = cx::MemoryHelper::Malloc(buf_len);
    uint8_t *data    = (uint8_t *)oMemory->data();

    memcpy(data, (uint8_t *)buf, buf_len);

    auto oMeta = std::make_shared<KwsOutMessageT>();
    oMeta->set_type(type);
    oMeta->set_buf_len(buf_len);

    auto output = std::make_shared<cx::Buffer>();
    output->AddMemory(oMemory);
    output->SetMetadata("dispatch_param", oMeta);

    Send(1, output);
}

bool MindProcess::Process(const std::vector<cx::BufferPtr> &data_vec)
{
    char  *vad_out;
    size_t vad_len = 0, kws_rec_len = 0;
    char  *kws_wwv_data;
    size_t kws_wwv_len = 0;

    int  pub_flag = 0;
    auto msg      = std::make_shared<SessionMessageT>();

    //算法输入数据，要求交织的2Mic+1Ref
    auto     iMemory          = data_vec.at(0)->GetMemory(0);
    int16_t *interleaved_buf_ = (int16_t *)iMemory->data();

    auto iMeta = data_vec.at(0)->GetMetadata<DataInputMessageT>("alsa_param");

    /* 单通道采样点数量 */
    int frame_sample_count = iMeta->frame();

    /* 单通道字节数 */
    int frame_byte = frame_sample_count * (iMeta->format() / 8);

    /* 算法PCM输入通道数(2mic+1ref) */
    int pcm_chn_num = iMeta->chn_num();

    // vad process
    int vad_state = vad_proc(interleaved_buf_, (void **)&vad_out, &vad_len);

    if (vad_state != VOICE_VADSTAT_SILENCE || vad_len > 0) {
        int to_send = 1;
        /* vad end detected, stop the silence timer */
        if (vad_state == VOICE_VADSTAT_ASR_FINI) {
            session_start_ = 0;
            //if (audio_get_status() != AU_STATUS_READY) {
                pub_flag       = 1;
                msg->body().set_vad_send_count(vad_send_count_);
                msg->body().set_cmd_id(thead::voice::proto::END);
                vad_send_count_ = 0;
                LOGD(TAG, "Vad end, send END msg, status %d", audio_get_status());
            //}
        } else if (vad_state == VOICE_VADSTAT_ASR) {
            LOGD(TAG, "vad state=VOICE_VADSTAT_ASR");
        }

        /* control the vad voice event freq to around 1/s */
        if (vad_state == VOICE_VADSTAT_VOICE) {
            vad_cnt_ = (vad_cnt_ + 1) % (1000 / 20);

            if (vad_cnt_ != 0 && vad_len == 0) {
                to_send = 0;
            }
        }

        if (to_send && session_start_) {
            if (vad_len > 0) {
#if defined(CONFIG_ASR_RECORD) && CONFIG_ASR_RECORD
                /* clear asr cache if record stoped */
                if (!start_record_ && ringbuffer_available_read_space(&this->asr_cache))
                {
                    LOGI(TAG, "clear asr cache");
                    while(ringbuffer_available_read_space(&this->asr_cache))
                    {
                        uint8_t tmp;
                        ringbuffer_read(&this->asr_cache, &tmp, 1);
                    }
                }

                /* cache all asr data if record ongoing */
                if (start_record_ && record_chn_count_ >= 6)
                {
                    if (ringbuffer_available_write_space(&this->asr_cache) >= vad_len)
                    {
                        ringbuffer_write(&this->asr_cache, (uint8_t *)vad_out, vad_len);
                    }
                    else
                    {
                        LOGE(TAG, "asr recode overflow");
                    }
                }
#endif

#if defined(CONFIG_ALG_ASR_LYEVA) && CONFIG_ALG_ASR_LYEVA
            if (asr_enable_status_) {
                /* ASR数据从端口2输出 */
                auto     oMemory      = cx::MemoryHelper::Malloc(vad_len);
                int16_t *asr_buffer  = (int16_t *)oMemory->data();

                memcpy(asr_buffer, vad_out, vad_len);
                auto oMeta = std::make_shared<DataInputMessageT>();
                oMeta->set_chn_num(1);
                oMeta->set_format(iMeta->format());
                oMeta->set_sample_rate(iMeta->sample_rate());
                oMeta->set_frame(vad_len/(iMeta->format() / 8));
                auto output = std::make_shared<cx::Buffer>();
                output->AddMemory(oMemory);
                output->SetMetadata("alsa_param", oMeta);
                vad_send_count_++;
                Send(0, output);
            }
#endif

                // cloud data send
                // LOGD(TAG, "CXSend ASR data to dispatch node (%d)", vad_len);
                KwsSendMsg((uint8_t *)&vad_out[0], vad_len, thead::voice::proto::TYPE_PCM);
            }
        }
    } else if (vad_state == VOICE_VADSTAT_SILENCE) {
        vad_cnt_ = 0;
    }

    // kws process
    int16_t            *kws_rec_data = (int16_t *)malloc(frame_byte * 2);
    audio_wakeup_info_t wakeup_info
        = kws_proc(interleaved_buf_, frame_sample_count, (int16_t **)&kws_rec_data, &kws_rec_len);

    // p2t process
    if (cmd_exe_push2talk_ == 1) {
        /* 有p2t命令来，无论是否唤醒状态，都重新发送唤醒事件 */
        audio_set_p2t_mode(push2talk_mode_);
        audio_set_status(AU_STATUS_WAKE);

        cmd_exe_push2talk_ = 2; /* 进入p2t交互状态 */

        static char word_p2t[] = "p2t";
        wakeup_info.wakeup_type = AUDIO_WAKEUP_TYPE_P2T;
        wakeup_info.kws_score   = 100;
        wakeup_info.kws_word = word_p2t;

    } else if (cmd_exe_push2talk_ == 3 ) {
        /* 设置为P2T就绪状态 */
        cmd_exe_push2talk_ = 0;

        /* 收到P2T结束命令，发送断句事件 */
        if (audio_get_status() != AU_STATUS_READY && audio_get_status() != AU_STATUS_FINI) {
            audio_set_p2t_mode(0);
            pub_flag       = 1;
            msg->body().set_cmd_id(thead::voice::proto::END);
            LOGD(TAG, "P2T end, send END msg, status %d", audio_get_status());
        }

        audio_set_status(AU_STATUS_FINI);
    }

    if (wakeup_info.wakeup_type) {
        LOGD(TAG,
             "wakeup. type=0x%x, id=0x%x word=%s score=%d vad=%d",
             wakeup_info.wakeup_type,
             wakeup_info.kws_id,
             wakeup_info.kws_word,
             wakeup_info.kws_score,
             wakeup_info.vad_st);

        session_start_ = 1;
        pub_flag       = 1;
        msg->body().set_cmd_id(thead::voice::proto::BEGIN);
        msg->body().set_wakeup_type(wakeup_info.wakeup_type);
        msg->body().set_kws_id(wakeup_info.kws_id);
        msg->body().set_kws_score(wakeup_info.kws_score);
        msg->body().set_kws_doa(0); // 历史遗留无效参数
        msg->body().set_kws_word(wakeup_info.kws_word);
    }

    if (wakeup_info.vad_st) {
        pub_flag = 1;
        if (wakeup_info.vad_st == 1) {
            msg->body().set_cmd_id(thead::voice::proto::VAD_BEGIN);
        } else {
            msg->body().set_cmd_id(thead::voice::proto::VAD_END);
        }
    }

    /* 提取唤醒词数据 */
    if (get_kws_data((void **)&kws_wwv_data, &kws_wwv_len) == 0) {
        LOGD(TAG, "get_kws_data ok kws_len %d", kws_wwv_len);
        if (kws_wwv_len > 0) {
            // wwv data send
            LOGD(TAG, "CXSend KWS data to dispatch node (%d)", kws_wwv_len);
            KwsSendMsg((uint8_t *)kws_wwv_data, kws_wwv_len, thead::voice::proto::TYPE_KWS);

            pub_flag = 1;
            msg->body().set_cmd_id(thead::voice::proto::WWV);
        }
    }

    /* 声源定位 */
    if (cmd_exe_get_doa_ == 1 && pub_flag == 0) {
        int doa = audio_get_doa();
        msg->body().set_cmd_id(thead::voice::proto::DOA);
        msg->body().set_kws_doa(doa);
        pub_flag = 1;
        cmd_exe_get_doa_ = 0;
    }

    /* ASR结果 */
    const char * asr_result = audio_get_asr_result();
    if (asr_result) {
        msg->body().set_cmd_id(thead::voice::proto::ASR);
        msg->body().set_kws_word(asr_result);
        pub_flag = 1;
        audio_clear_asr_result();
    }

    // pub msg to sub
    if (pub_flag) {
        LOGD(TAG, "PUB SessionMsg: cmd_id %d", msg->body().cmd_id());
        writer_->Write(msg);
    }

    /* 准备录音或FEOUTAEC数据,5路数据录音,必须为交织格式 */
    int      rec_chn_num  = pcm_chn_num + 2;
#if defined(CONFIG_ASR_RECORD) && CONFIG_ASR_RECORD
    if (record_chn_count_ >= 6)
    {
        /* 增加第六路为ASR数据 */
        rec_chn_num += 1;
    }
#endif

    int      create_5ch_pcm_data = 0; /* 录音或FEOUTAEC模式都需要创建5路数据 */

#ifdef CONFIG_FEOUTAEC
    if (audio_get_linear_aec_en_status()) {
        create_5ch_pcm_data = 1;
    }
#endif

    /* 仅处理5路或者6路音频数据录制 */
    if ( start_record_ && record_chn_count_ >=5 ) {
        create_5ch_pcm_data = 1;
    }

    if (create_5ch_pcm_data) {
        auto     rec_oMemory  = cx::MemoryHelper::Malloc(frame_byte * rec_chn_num);
        int16_t  *rec_data_out = (int16_t *)rec_oMemory->data();

        /* 5 chn interleaved data to record (mic1, mic2, ref, aec1, aec2) */
        for (int i = 0; i < frame_sample_count; i++) {

            /* 原始PCM数据放前3路 */
            for (int j = 0; j < pcm_chn_num; j++) {
                rec_data_out[i * rec_chn_num + j] = interleaved_buf_[i * pcm_chn_num + j];
            }
            /* 算法处理后数据放后2路 */
            for (int j = 0; j < 2; j++) {
                rec_data_out[i * rec_chn_num + j + 3] = kws_rec_data[i * 2 + j];
            }

#if defined(CONFIG_ASR_RECORD) && CONFIG_ASR_RECORD
            /* ASR数据放置第六路，需要注意ASR数据并不与前五路数据帧同步 */
            if (record_chn_count_ >= 6)
            {
                uint16_t asr_data = 0;
                ringbuffer_read(&this->asr_cache, (uint8_t *)&asr_data, 2);
                rec_data_out[i * rec_chn_num + 5] = asr_data;
            }
#endif
        }
#if defined(CONFIG_ASR_RECORD) && CONFIG_ASR_RECORD
        if (start_record_) {
            /* send message to record process */
            auto oMeta = std::make_shared<SspOutMessageT>();
            oMeta->set_chn_num(rec_chn_num);
            oMeta->set_vad_res(0); /* reserve */
            oMeta->set_sample_rate(iMeta->sample_rate());
            oMeta->set_frame(frame_sample_count);

            auto output = std::make_shared<cx::Buffer>();
            output->AddMemory(rec_oMemory);
            output->SetMetadata("ssp_param", oMeta);
            //发送到record_process#1节点
            Send(2, output);
        }
#endif
#ifdef CONFIG_FEOUTAEC
        if (audio_get_linear_aec_en_status()) {
            auto oMeta = std::make_shared<KwsOutMessageT>();
            oMeta->set_type(thead::voice::proto::TYPE_FEAEC);
            oMeta->set_buf_len(frame_byte * rec_chn_num);

            auto output = std::make_shared<cx::Buffer>();
            output->AddMemory(rec_oMemory);
            output->SetMetadata("dispatch_param", oMeta);
            /* 发送到dispatch_process节点 */
             Send(1, output);
        }
#endif
    }

    free(kws_rec_data);

    return true;
}

CX_REGISTER_PLUGIN(MindProcess);

} // namespace cpt

#endif /*#if defined(CONFIG_CHIP_D1) || defined(CONFIG_CHIP_BL606P) || defined(CONFIG_CHIP_LIGHT)*/
