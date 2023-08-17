/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <board.h>

#if !(defined(CONFIG_BOARD_AMP_LOAD_FW) && CONFIG_BOARD_AMP_LOAD_FW)

#include <stdio.h>
#include <memory>

#include <aos/aos.h>
#include <cxvision/cxvision.h>
#include <ulog/ulog.h>

#include "alg_register.h"
#include "ly_preprocess.h"
#include "ly_asr.h"
#include "aie_mem_pool.h"
#include "asr_nlu.h"

// #define _ENABLE_ALG_RECORD

#define TAG              "AudioSample"
#define ASR_CHANNEL_NUM_ 1
#define FRAME_COUNT_MAX_ 32 * 10 + 3 // (FRAME_COUNT_MAX_ - 3) % 32 = 0
#define MAX_TEXT_LEN     60  // 20 * 3 byte
#define NLU_MSG_LEN      1024
#define ASR_CACHE_NEED   34 // 30帧 = 300ms
#define AGC_MODE_        1
#define AGC_GAIN_        30
#define VAD_MODE_        2
#define VAD_TIME_        50 // 500ms
#define SAMPLE_RATE      16000
#define VAD_COUNT        10
// cxvision message
/* 输入/输出音频数据消息 */
using DataInputMessageT = thead::voice::proto::DataInputMsg;
/* 录音音频数据 */
using SspOutMessageT = thead::voice::proto::SspOutMsg;

// pub/sub message
using AlgCmdMessageT  = posto::Message<thead::voice::proto::AlgCmdMsg>;
using SessionMessageT = posto::Message<thead::voice::proto::SessionMsg>;
using RecordMessageT  = posto::Message<thead::voice::proto::RecordMsg>;

namespace cpt
{
class LyevaASRProcess : public cx::PluginBase
{
public:
    LyevaASRProcess();
    bool Init(const std::map<std::string, std::string> &props) override;
    bool DeInit() override;
    bool Process(const std::vector<cx::BufferPtr> &data_vec) override;

    void SetASR(asr_wenet_handle *asr_handle);
    void SetStride(int stride);
    void SetDecodeWindow(int decoding_window);
    void SetFrameBuf(int16_t *frame_buf, size_t frame_buf_len, int16_t *agc_buf, int16_t *vad_buff);
    void SetVad(void *vad);
    void SetAgc(void *agc);

private:
    bool                                             ScoreAndSend();
    std::shared_ptr<posto::Participant>              participant_;
    std::shared_ptr<posto::Writer<SessionMessageT> > writer_;
    std::shared_ptr<posto::Reader<SessionMessageT> > session_reader_;
    std::shared_ptr<posto::Reader<AlgCmdMessageT> >  algcmd_reader0_;

    asr_wenet_handle *asr_handle_;
    int               decoding_window_; // 特征提取窗长
    int               stride_;          // 特征提取窗移
    int16_t *         frame_buf_;
    float *           frame_feature_buf_; // 无需分配空间
    size_t            frame_buf_len_; // 一帧采样点数
    int               vad_state_;
    int               asr_feature_count_;
    int               vad_enable_;
    int *              top_index;
    int               is_detected_phy;
    aos_mutex_t       asr_lock_;
    int               vad_send_count_;
    vad_handle *      vad_;
    int16_t           vad_out_;
    int16_t *         agc_buf_;
    agc_handle *      agc_;
    int               vad_start;
    int               vad_off_count;
    int16_t *         vad_buff_;

#ifdef _ENABLE_ALG_RECORD
    std::shared_ptr<posto::Reader<RecordMessageT> > record_reader1_;
    int                                             start_record_;
    int record_chn_count_; /* 0:不录制 3:录制3路 5:录制5路 6: 录制6路 */
#endif
};

void LyevaASRProcess::SetASR(asr_wenet_handle *asr_handle)
{
    asr_handle_ = asr_handle;
}

void LyevaASRProcess::SetStride(int stride)
{
    stride_ = stride;
}

void LyevaASRProcess::SetDecodeWindow(int decoding_window)
{
    decoding_window_ = decoding_window;
}

void LyevaASRProcess::SetFrameBuf(int16_t *frame_buf, size_t frame_buf_len, int16_t *agc_buf, int16_t *vad_buff)
{
    frame_buf_     = frame_buf;
    frame_buf_len_ = frame_buf_len;
    agc_buf_       = agc_buf;
    vad_buff_      = vad_buff;
}

void LyevaASRProcess::SetVad(vad_handle *vad)
{
    vad_ = vad;
}

void LyevaASRProcess::SetAgc(agc_handle *agc)
{
    agc_ = agc;
}

LyevaASRProcess::LyevaASRProcess()
{
    participant_    = NULL;
    writer_         = NULL;
    algcmd_reader0_ = NULL;

    asr_handle_        = nullptr; // no free
    decoding_window_   = 0;
    stride_            = 0;
    frame_buf_         = nullptr; // no free
    frame_feature_buf_ = nullptr;
    frame_buf_len_     = 0;
    asr_feature_count_ = ASR_CACHE_NEED; // 确保300ms缓存
    vad_enable_          = 1; // 使用nnvad 需要初始化为1
    is_detected_phy    = 0;
    vad_               = nullptr;
    vad_out_           = 0;
    agc_buf_           = nullptr; // agc_buf_len == frame_buf_len
    agc_               = nullptr;
    top_index          = nullptr;
    vad_start          = 0;
    vad_off_count       = 0;
    vad_buff_            = nullptr;

#ifdef _ENABLE_ALG_RECORD
    record_reader1_   = NULL;
    start_record_     = 0;
    record_chn_count_ = 0;
#endif
}

bool LyevaASRProcess::ScoreAndSend()
{
    auto   msg = std::make_shared<SessionMessageT>();
    char * asr_text;
    size_t text_len;
    asr_text = (char *)malloc(MAX_TEXT_LEN * sizeof(char));
    if (!asr_text) {
        LOGE(TAG, "asr_text malloc fail\n");
        return true;
    }
    memset(asr_text, 0, MAX_TEXT_LEN * sizeof(char));

    char *nlu_result = (char *)malloc(1024);
    if (!nlu_result) {
        free(asr_text);
        LOGE(TAG, "nlu_result malloc fail\n");
        return true;
    }

    memset(nlu_result, 0, NLU_MSG_LEN);

    aos_mutex_lock(&asr_lock_, AOS_WAIT_FOREVER);
    int ret = aie_asr_search_and_rescoring(asr_handle_, &asr_text, &text_len);
    if (ret) {
        LOGE(TAG, "rescore failed! %d", ret);
        return true;
    }
    aie_nnvad_reset(vad_);
    // aie_vad_reset(vad_);

    // aie_agc_reset(agc_);
    aie_asr_reset(asr_handle_);
    asr_feature_count_ = ASR_CACHE_NEED;
    aos_mutex_unlock(&asr_lock_);

    if (text_len <= 1) {
        LOGD(TAG, "asr result is empty\n");
        free(asr_text);
        free(nlu_result);
        return true;
    }

    int err = asr_nlu_get(asr_text, nlu_result, NLU_MSG_LEN);
    if (!err) {
        msg->body().set_cmd_id(thead::voice::proto::ASR);
        msg->body().set_kws_word(nlu_result);
        /* 通过发布（Pub）SessionMessage，将识别结果发布给应用节点 */
        writer_->Write(msg);
    }
    free(asr_text);
    free(nlu_result);
    return true;
}

void _asr_init(void *arg)
{
    LyevaASRProcess * asr = (LyevaASRProcess *)arg;
    asr_info_t        info_asr;
    size_t            frame_buf_len;
    int16_t *         frame_buf;
    int16_t *         agc_buf;
    int16_t *         vad_buff;
    int               decoding_window;
    int               stride;
    int               ret;
    int               mem_pool_limit[] = { 500 * 1024 };
    int               mem_pool_size[]  = { 1600 * 1024 };
    asr_init_params_t asr_params       = { ASR_CHANNEL_NUM_};
    mem_pool_paprams_t pool_params = {1, mem_pool_limit, mem_pool_size};
    aie_global_mem_pool_init(pool_params);
    
    vad_handle *      vad_handle = aie_register(nnvad);
    agc_handle *      agc_handle = aie_register(agc);
    asr_wenet_handle *asr_handle = aie_register(asr_wenet);

    // TODO: Check Value;
    ret = aie_agc_init(agc_handle, SAMPLE_RATE, AGC_MODE_, AGC_GAIN_);
    if (ret) {
        LOGE(TAG, "agc init fail %d", ret);
        return;
    }

    ret = aie_asr_init(asr_handle, asr_params, &frame_buf_len);
    if (ret) {
        LOGE(TAG, "asr init fail %d", ret);
        return;
    }

    ret = aie_get_asr_info(asr_handle, &info_asr);
    if (ret) {
        LOGE(TAG, "get asr info fail %d", ret);
        return;
    }

    decoding_window = info_asr.decoding_window;
    stride          = info_asr.stride;

    ret = aie_nnvad_init(vad_handle, 0, VAD_MODE_, SAMPLE_RATE);
    // ret = aie_vad_init(vad_handle, VAD_TIME_, VAD_MODE_, SAMPLE_RATE);
    if (ret) {
        LOGE(TAG, "vad init fail %d", ret);
        return;
    }

    frame_buf = (int16_t *)malloc(frame_buf_len * ASR_CHANNEL_NUM_ * sizeof(int16_t));
    if (frame_buf == NULL) {
        LOGE(TAG, "frame alloc fail");
        return;
    }
    agc_buf = (int16_t *)malloc(frame_buf_len * ASR_CHANNEL_NUM_ * sizeof(int16_t));
    if (agc_buf == NULL) {
        LOGE(TAG, "agc_buf alloc fail");
        return;
    }

    vad_buff = (int16_t *)malloc(VAD_COUNT * sizeof(int16_t));
    if (vad_buff == NULL) {
        LOGE(TAG, "vad_buff alloc fail");
        return;
    }
    memset(vad_buff, 0, VAD_COUNT * sizeof(int16_t));


    ret = asr_nlu_init();
    if (ret) {
        LOGE(TAG, "nlu init fail %d", ret);
        return;
    }

    asr->SetASR(asr_handle);
    asr->SetDecodeWindow(decoding_window);
    asr->SetStride(stride);
    asr->SetFrameBuf(frame_buf, frame_buf_len, agc_buf, vad_buff);
    asr->SetVad(vad_handle);
    asr->SetAgc(agc_handle);
    LOGD(TAG, "ASR::Init");

    aos_task_exit(0);
}

bool LyevaASRProcess::Init(const std::map<std::string, std::string> &props)
{
    const auto &iter = props.find("model_path");
    if (iter != props.end()) {
        LOGD(TAG, "model_path: %s", iter->second.c_str());
    }

    LOGD(TAG, "LyevaASRProcess::Init");

    /* 初始化 Pub，Sub 相关接口*/
    participant_ = posto::Domain::CreateParticipant("cmd_producer");

    /* 初始化SessionMsg消息发布接口 */
    writer_ = participant_->CreateWriter<SessionMessageT>("SessionMsg");

    /* 初始化AlgCmdMessage消息订阅接口 */
    algcmd_reader0_
        = participant_->CreateReader<AlgCmdMessageT>("AlgCmdMsg", [this](const std::shared_ptr<AlgCmdMessageT> &msg) {
              LOGD(TAG, "AlgCmdMsg got, cmd: %d iv=%d iv2=%d strv=[%s]", msg->body().cmd(), msg->body().ivalue(),
                   msg->body().ivalue2(), msg->body().strvalue().c_str());
              switch (msg->body().cmd()) {
                  case thead::voice::proto::PUSH2TALK_CMD: {
                      int p2tmode = msg->body().ivalue();
                      (void)p2tmode;
                  } break;
                  default:
                      break;
              }
          });

    session_reader_ = participant_->CreateReader<SessionMessageT>(
        "SessionMsg", [this](const std::shared_ptr<SessionMessageT> &msg) {
            LOGD(TAG, "SessionMsg got,cmd_id: %d", msg->body().cmd_id());

            switch (msg->body().cmd_id()) {
                 case thead::voice::proto::BEGIN:
                        // aie_vad_reset(vad_);
                        aie_nnvad_reset(vad_);
                        // aie_agc_reset(agc_);
                        aie_asr_reset(asr_handle_);
                        asr_feature_count_ = ASR_CACHE_NEED;
                        // last_vad = 0;
                // case thead::voice::proto::WWV:
                //     vad_state_ = 1;
                //     aos_mutex_lock(&asr_lock_, AOS_WAIT_FOREVER);
                //     aie_asr_reset(asr_handle_);
                //     asr_feature_count_ = 0;
                //     aos_mutex_unlock(&asr_lock_);
                 break;

                // case thead::voice::proto::END:
                // case thead::voice::proto::TIMEOUT:
                //     vad_send_count_ = msg->body().vad_send_count();
                //     vad_state_      = 0;
                //     break;
                default:
                    break;
            }
        });

#ifdef _ENABLE_ALG_RECORD
    /* 初始化RecordMessage消息订阅接口 */
    record_reader1_
        = participant_->CreateReader<RecordMessageT>("RecordMsg", [this](const std::shared_ptr<RecordMessageT> &msg) {
              LOGD(TAG, "RecordMsg got, cmd_id: %d", msg->body().cmd());
              switch (msg->body().cmd()) {
                  /* 开始录音 */
                  case thead::voice::proto::START:
                      /* 记录开始录音 */
                      start_record_ = 1;
                      /* 记录录音通道数 */
                      record_chn_count_ = msg->body().record_chn_count();
                      break;

                  /* 停止录音 */
                  case thead::voice::proto::STOP:
                      start_record_     = 0;
                      record_chn_count_ = 0;
                      break;

                  default:
                      break;
              }
          });
#endif

    aos_mutex_new(&asr_lock_);

    // ASR初始化需要一个比较大的栈空间，独立一个任务进行
    aos_task_t asr_task_;
    aos_task_new_ext(&asr_task_, "asr_task", _asr_init, this, 1024 * 16, AOS_DEFAULT_APP_PRI);

    return true;
}

bool LyevaASRProcess::DeInit()
{
    /* TODO: 算法去初始化 */
    aos_mutex_free(&asr_lock_);
    asr_nlu_deinit();
    return true;
}

bool LyevaASRProcess::Process(const std::vector<cx::BufferPtr> &data_vec)
{
    /* 获取输入的音频数据*/
    auto     iMemory   = data_vec.at(0)->GetMemory(0);
    int16_t *data_buf_ = (int16_t *)iMemory->data();

    /* 获取输入音频数据参数 */
    auto iMeta = data_vec.at(0)->GetMetadata<DataInputMessageT>("alsa_param");

    /* 单个通道采样点数量 */
    size_t frame_sample_count = iMeta->frame();

    /* 音频数据采用通道数 */
    int pcm_chn_num = iMeta->chn_num();

    /* 音频帧数据格式，16bit/24bit */
    int frame_format = iMeta->format();

    /* 单通道字节数 */
    int frame_byte = frame_sample_count * (frame_format / 8);

    (void)frame_byte;

    /* TODO: 算法处理 */
    aos_assert(pcm_chn_num == ASR_CHANNEL_NUM_);
    int asr_offset = 0;
    // 每10ms进行特征提取

//***************************************************nnvad***************************************//
    for (size_t seq = 0; seq < frame_sample_count / frame_buf_len_; seq++) {
        for (size_t j = 0; j < frame_buf_len_; j++) {
            frame_buf_[j] = data_buf_[j + seq * frame_buf_len_]; // 均为单通道
        }

        aie_agc_run(agc_, frame_buf_, frame_buf_len_, agc_buf_);
        // for (size_t j = 0; j < frame_buf_len_; j++) {
        //     if(frame_buf_[j] > 4095) // 32767 / 8
        //         frame_buf_[j] = 32767;
        //     else if(frame_buf_[j] < -4095)
        //         frame_buf_[j] = -32768;
        //     else
        //         frame_buf_[j] = frame_buf_[j] * 8; // 均为单通道
        // }

        aie_asr_feature_compute(asr_handle_, agc_buf_, frame_buf_len_, &frame_feature_buf_);

        aie_nnvad_run(vad_, frame_feature_buf_, frame_buf_len_, &vad_out_, vad_enable_);

        memmove(&vad_buff_[1], vad_buff_, (VAD_COUNT - 1) * sizeof(int16_t));

        vad_buff_[0] = vad_out_;
        // LOGD(TAG, "BUF %d %d %d %d %d", vad_buff_[0],vad_buff_[1],vad_buff_[2],vad_buff_[3],vad_buff_[4]);
        int sum_tmp = 0;
        for (int i = 0; i < VAD_COUNT; i++)
            sum_tmp += vad_buff_[i];
        if (sum_tmp > VAD_COUNT - 2) {
        // if(vad_out_ == 1 && last_vad == 0) {
            LOGE(TAG, "Vad Begining");
            // aie_asr_reset(asr_handle_);
            aie_asr_reset_remain_caches(asr_handle_);
            vad_start = 1;
            asr_feature_count_ = ASR_CACHE_NEED;
            vad_enable_ = 0;
            memset(vad_buff_, 0, VAD_COUNT * sizeof(int16_t));
        }
        // last_vad = vad_out_;

        asr_feature_count_++;
        if ((asr_feature_count_ - (decoding_window_ - stride_)) % stride_ == 0
            && asr_feature_count_ > (decoding_window_ - stride_))
        {
            // if (vad_enable_)
            //     aie_asr_run(asr_handle_);
            if (vad_enable_ == 0) {
                aie_asr_run_as_vad(asr_handle_, &asr_offset, &top_index);
                int top_sum = 0;
                for (int i = 0; i < asr_offset; i ++)
                    top_sum += top_index[i];
                
                if(top_sum == 0 && is_detected_phy == 0) {

                    // do nothing  ctc 
                }
                else {
                    is_detected_phy = 1;
                    top_sum = 0;
                    int zero_count = 10;
                    for (int i = 0; i < zero_count; i ++)
                        top_sum += top_index[asr_offset - zero_count + i];
                    // LOGD(TAG, "asr_offset%d", asr_offset);
                    if(top_sum == 0) {
                        vad_enable_ = 1;
                        is_detected_phy = 0;
                        vad_start = 0;
                        ScoreAndSend();
                    }
                }
            }
        }
        if  (vad_enable_ == 0 && asr_feature_count_ > FRAME_COUNT_MAX_ ) {
            LOGD(TAG, "Time out 3000ms");
            vad_enable_ = 1;
            vad_start = 0;
            ScoreAndSend();

        }

#ifdef _ENABLE_ALG_RECORD
    if (start_record_) {
        /* 根据录制的通道数，准备需要录制的音频数据 */
        auto     rec_oMemory  = cx::MemoryHelper::Malloc(320 *  record_chn_count_);
        int16_t *rec_data_out = (int16_t *)rec_oMemory->data();

        /* 填充ASR数据 */
        for (size_t i = 0; i < frame_buf_len_; i++) {
            rec_data_out[i *  3] = data_buf_[i + seq * frame_buf_len_]; // 均为单通道
            rec_data_out[i * 3 + 1] = data_buf_[i + seq * frame_buf_len_] * 8;
            rec_data_out[i * 3 + 2] = agc_buf_[i];
        }


        /* 发送录音音频数据至Record节点 */
        auto oMeta = std::make_shared<SspOutMessageT>();
        oMeta->set_chn_num(record_chn_count_);
        oMeta->set_vad_res(0); /* reserve */
        oMeta->set_sample_rate(iMeta->sample_rate());
        oMeta->set_frame(160);
        auto output = std::make_shared<cx::Buffer>();
        output->AddMemory(rec_oMemory);
        output->SetMetadata("ssp_param", oMeta);
        /* 算法节点默认输出端口1，对接Record节点的输入端口1 */
        Send(0, output);
    }
#endif
    }

    return true;
}

CX_REGISTER_PLUGIN(LyevaASRProcess);

} // namespace cpt

// #endif

#endif
