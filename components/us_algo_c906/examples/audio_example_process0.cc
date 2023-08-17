/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifdef CONFIG_ALG_ENABLE_EXAMPLE_PROCESS0

#include <stdio.h>
#include <memory>

#include <cxvision/cxvision.h>
#include <ulog/ulog.h>
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>
#include "signalProcess.h"
#include "wakeup_sync_api.h"
#include "aceAsr.h"
#include "uni_nlu.h"

#include "audio_example_process0.h"

//#define _ENABLE_ALG_RECORD

#define TAG "AudioSample"
#define ASR_MAX_FRAME (1000 / 20 * 10) //每一帧20ms,一共10s的帧

// cxvision message
/* 输入/输出音频数据消息 */
using DataInputMessageT = thead::voice::proto::DataInputMsg;
/* 录音音频数据 */
using SspOutMessageT    = thead::voice::proto::SspOutMsg;
using KwsOutMessageT    = thead::voice::proto::KwsOutMsg;

// pub/sub message
using AlgCmdMessageT  = posto::Message<thead::voice::proto::AlgCmdMsg>;
using SessionMessageT = posto::Message<thead::voice::proto::SessionMsg>;
using RecordMessageT  = posto::Message<thead::voice::proto::RecordMsg>;



namespace cpt
{
  static int _sgp_cb(void *usr_ptr, const char *data, int size);
  static int _wku_cb(void *usr_ptr, const char *json, int bytes);
  static int _asr_cb(void *usr_ptr, char *json, int bytes);

class AudioExampleProcess0 : public cx::PluginBase
{
public:
    AudioExampleProcess0();
    bool Init(const std::map<std::string, std::string> &props) override;
    bool DeInit() override;
    bool Process(const std::vector<cx::BufferPtr> &data_vec) override;
    bool SendSgpRst(const char *data, int size);
    bool SendWkuRst(const char *json, int bytes);
    bool SendAsrRst(const char *json, int bytes);
private:
    std::shared_ptr<posto::Participant>              participant_;
    std::shared_ptr<posto::Writer<SessionMessageT> > writer_;
    std::shared_ptr<posto::Reader<AlgCmdMessageT> >  algcmd_reader0_;
    void *sgp_hdl;
    void *wakeup_hdl;
    TGEngine asr_hdl;
    bool start_asr_;
    int asr_frame_num;
    int ssp_num;
#ifdef _ENABLE_ALG_RECORD
    std::shared_ptr<posto::Reader<RecordMessageT> >  record_reader1_;
    int start_record_;
    int record_chn_count_; /* 0:不录制 3:录制3路 5:录制5路 6: 录制6路 */
#endif
};

AudioExampleProcess0::AudioExampleProcess0()
{
    participant_    = NULL;
    writer_         = NULL;
    algcmd_reader0_ = NULL;
    sgp_hdl = NULL;
    wakeup_hdl = NULL;
    asr_hdl = NULL;
    start_asr_ = false;
    asr_frame_num = 0;
    ssp_num = 0;
#ifdef _ENABLE_ALG_RECORD
    record_reader1_   = NULL;
    start_record_     = 0;
    record_chn_count_ = 0;
#endif
}

bool AudioExampleProcess0::SendSgpRst(const char *data, int size)
{
  if (ssp_num++ % 500 == 0) {
    LOGW(TAG, "ssp data size:%d, cnt:%d", size, ssp_num);
  }

  if (!start_asr_) {
    return true;
  }

  auto     oMemory = cx::MemoryHelper::Malloc(size);
  uint8_t *p_data    = (uint8_t *)oMemory->data();
  
  memcpy(p_data, data, size);
  
  auto oMeta = std::make_shared<KwsOutMessageT>();
  oMeta->set_type(thead::voice::proto::TYPE_PCM);
  oMeta->set_buf_len(size);

  auto output = std::make_shared<cx::Buffer>();
  output->AddMemory(oMemory);
  output->SetMetadata("dispatch_param", oMeta);

  Send(2, output);

  return true;
}

bool AudioExampleProcess0::SendWkuRst(const char *json, int bytes)
{
  char *pingyin = NULL;
  int index = 0;
  float score = 0;
  auto msg      = std::make_shared<SessionMessageT>();
  LOGW(TAG, "WkuCb:%s",json);

  pingyin = NluParseWkuRst(json, &score, &index);
  if (pingyin) {
    msg->body().set_cmd_id(thead::voice::proto::BEGIN);
    msg->body().set_wakeup_type(1);
    msg->body().set_kws_score((int)score);
    msg->body().set_kws_word(pingyin);
    msg->body().set_kws_id(index);
    /* 通过发布（Pub）SessionMessage，将识别结果发布给应用节点 */
    writer_->Write(msg);
    free(pingyin);

    /* 开启asr */
    start_asr_ = true;
    asr_frame_num = 0;
    TGEngineStart(asr_hdl, _asr_cb, static_cast<void*>(this));
  }
  return true;
}

bool AudioExampleProcess0::SendAsrRst(const char *json, int bytes)
{
  char *rst = NULL;
  float score = 0;
  auto msg      = std::make_shared<SessionMessageT>();

  LOGW(TAG,"AsrCb:%s",json);
  rst = NluParseLasr(json, &score);
  if (rst) {
    msg->body().set_cmd_id(thead::voice::proto::ASR);
    msg->body().set_kws_word(rst);
    /* 通过发布（Pub）SessionMessage，将识别结果发布给应用节点 */
    writer_->Write(msg);
    free(rst);

    /* 结束asr */
    start_asr_ = false;
    TGEngineStop(asr_hdl);
    /* 停止在线asr */
    msg->body().set_cmd_id(thead::voice::proto::END);
    writer_->Write(msg);
    return true;
  }

  return false;
}


static int _sgp_cb(void *usr_ptr, const char *data, int size) {
  AudioExampleProcess0 *p = static_cast<AudioExampleProcess0*>(usr_ptr);
  p->SendSgpRst(data, size);
  return 0;
}

static int _wku_cb(void *usr_ptr, const char *json, int bytes) {
  AudioExampleProcess0 *p = static_cast<AudioExampleProcess0*>(usr_ptr);
  p->SendWkuRst(json, bytes);
  return 0;
}

static int _asr_cb(void *usr_ptr, char *json, int bytes) {
  AudioExampleProcess0 *p = static_cast<AudioExampleProcess0*>(usr_ptr);
  p->SendAsrRst(json, bytes);
  return 0;
}

bool AudioExampleProcess0::Init(const std::map<std::string, std::string> &props)
{
    const auto &iter = props.find("model_path");
    if (iter != props.end()) {
        LOGD(TAG, "model_path: %s", iter->second.c_str());
    }

    LOGD(TAG, "AudioExampleProcess0::Init");

    /* 初始化 Pub，Sub 相关接口*/
    participant_ = posto::Domain::CreateParticipant("cmd_producer");

    /* 初始化SessionMsg消息发布接口 */
    writer_      = participant_->CreateWriter<SessionMessageT>("SessionMsg");

    /* 初始化AlgCmdMessage消息订阅接口 */
    algcmd_reader0_
        = participant_->CreateReader<AlgCmdMessageT>("AlgCmdMsg", [this](const std::shared_ptr<AlgCmdMessageT> &msg) {
              LOGD(TAG, "AlgCmdMsg got, cmd: %d iv=%d iv2=%d strv=[%s]", msg->body().cmd(), msg->body().ivalue(),
                                                                         msg->body().ivalue2(), msg->body().strvalue().c_str());
              switch (msg->body().cmd()) {
                  case thead::voice::proto::PUSH2TALK_CMD: {
                      int p2tmode = msg->body().ivalue();
                      /* TODO: 设置常唤醒模式 */
                      (void)p2tmode;
                  } break;  
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
                      start_record_ = 0;
                      record_chn_count_ = 0;
                      break;

                  default:
                      break;
              }
          });
#endif
    /* TODO: 音频算法初始化 */
    sgp_hdl = sgp_new(NULL);
    sgp_register_processed_voice(sgp_hdl, static_cast<void*>(this), _sgp_cb);
    wakeup_hdl = wku_new();
    asr_hdl = TGEngineCreate();
    sgp_start(sgp_hdl);
    wku_start(wakeup_hdl, _wku_cb, static_cast<void*>(this));
    return true;
}

bool AudioExampleProcess0::DeInit()
{
    /* TODO: 算法去初始化 */
    sgp_delete(sgp_hdl);
    wku_delete(wakeup_hdl);
    TGEngineDestroy(asr_hdl);
    return true;
}

bool AudioExampleProcess0::Process(const std::vector<cx::BufferPtr> &data_vec)
{
    auto msg      = std::make_shared<SessionMessageT>();

    /* 获取输入的音频数据*/
    auto     iMemory          = data_vec.at(0)->GetMemory(0);
    int16_t *interleaved_buf_ = (int16_t *)iMemory->data();

    /* 获取输入音频数据参数 */
    auto iMeta = data_vec.at(0)->GetMetadata<DataInputMessageT>("alsa_param");

    /* 单个通道采样点数量 */
    int frame_sample_count = iMeta->frame();

    /* 音频数据采用通道数 */
    int pcm_chn_num = iMeta->chn_num();

    /* 音频帧数据格式，16bit/24bit */
    int frame_format = iMeta->format();

    /* 单通道字节数 */
    int frame_byte = frame_sample_count * (frame_format / 8);

    //LOGD(TAG, "alsa params frame_sample_cout %d, pcm_chn_num %d, frame_format %d\n", \
    //        frame_sample_count, pcm_chn_num, frame_format);

    /* TODO: 算法处理 */
    // reslut <- alg_process(interleaved_buf_, frame_byte)
    /* 判断asr是否超时10s */
    if (start_asr_ && asr_frame_num ++ > ASR_MAX_FRAME) {
      start_asr_ = false;
      TGEngineStop(asr_hdl);

      /* 停止在线asr */
      msg->body().set_cmd_id(thead::voice::proto::END);
      writer_->Write(msg);
    }

    sgp_feed(sgp_hdl, (const char*)interleaved_buf_, frame_byte * pcm_chn_num);
//    static int cnt = 0;
//    LOGD(TAG, "process cnt: %d", cnt++);
    /* 多算法节点部署 */
#ifdef CONFIG_ALG_ENABLE_EXAMPLE_PROCESS1
    /* 准备发送给算法节点1的音频数据，实际长度按照上述流程的处理后结果为准 */
    if (frame_byte)
    {
        auto     oMemory      = cx::MemoryHelper::Malloc(frame_byte);
        int16_t *frame_process0  = (int16_t *)oMemory->data();
        /* 填充音频数据至frame_process0 */
        (void)frame_process0;

        /* 通过输出端口0，发送音频数据至算法节点1的输入端口0 */
        auto oMeta = std::make_shared<DataInputMessageT>();
        oMeta->set_chn_num(1);
        oMeta->set_format(iMeta->format());
        oMeta->set_sample_rate(iMeta->sample_rate());
        oMeta->set_frame(frame_sample_count);
        auto output = std::make_shared<cx::Buffer>();
        output->AddMemory(oMemory);
        output->SetMetadata("alsa_param", oMeta);
        Send(0, output);
    }
#else
    /* 示例输出 */
//    const char *result = "{\"header\":{\"namespace\":\"NluModel\",\"version\":\"1.0.0\"}," \
//            "\"payload\":{\"asrresult\":\"打开空调\","\
//            "\"semantics\":[{\"domain\":\"acc\",\"intent\":\"ctl_acc_power_ctrl_on\"}]}}";
//    //LOGD(TAG, "this is example result %s\n", result);

//    if (0) {
//        msg->body().set_cmd_id(thead::voice::proto::ASR);
//        msg->body().set_kws_word(result);
//        /* 通过发布（Pub）SessionMessage，将识别结果发布给应用节点 */
//        writer_->Write(msg);
//    }
#endif

#ifdef _ENABLE_ALG_RECORD
    if (start_record_) {
        /* 根据录制的通道数，准备需要录制的音频数据 */
        auto     rec_oMemory  = cx::MemoryHelper::Malloc(frame_byte * record_chn_count_);
        int16_t  *rec_data_out = (int16_t *)rec_oMemory->data();
        /* TODO：填充录音音频数据，需要注意数据格式为交织格式 */

        /* 发送录音音频数据至Record节点 */
        auto oMeta = std::make_shared<SspOutMessageT>();
        oMeta->set_chn_num(record_chn_count_);
        oMeta->set_vad_res(0); /* reserve */
        oMeta->set_sample_rate(iMeta->sample_rate());
        oMeta->set_frame(frame_sample_count);
        auto output = std::make_shared<cx::Buffer>();
        output->AddMemory(rec_oMemory);
        output->SetMetadata("ssp_param", oMeta);
        /* 算法节点默认输出端口1，对接Record节点的输入端口1 */
        Send(1, output);
    }
#endif

    (void)frame_sample_count;
    (void)pcm_chn_num;
    (void)interleaved_buf_;
    (void)frame_byte;

    return true;
}

CX_REGISTER_PLUGIN(AudioExampleProcess0);

} // namespace cpt

#endif
