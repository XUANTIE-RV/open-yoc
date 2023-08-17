/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <memory>

#include <cxvision/cxvision.h>
#include <ulog/ulog.h>
#include <aos/ringbuffer.h>
#include <aos/aos.h>

#include "dispatch_ringbuf.h"

#define TAG "Dispatch"

#define FRAME_SIZE ((16000 / 1000) * (16 / 8) * 20) /* 640 */
#define PCM_RINGBUF_LEN      (FRAME_SIZE * 150)  /* store cloud data: 3sec 1chn */
#define KWS_RINGBUF_LEN      (FRAME_SIZE * 80)   /* store wakeup data: 1.6sec 1chn */
#define FEAEC_RINGBUF_LEN     (FRAME_SIZE * 25 * 5)   /* 5chn, 0.5s per chn */

// cxvision message
using KwsOutMessageT    = thead::voice::proto::KwsOutMsg;
// pub/sub message
using SessionMessageT   = posto::Message<thead::voice::proto::SessionMsg>;

namespace cpt {

class DispatchProc : public cx::PluginBase {

public:
  DispatchProc();
  bool Init(const std::map<std::string, std::string>& props) override;
  bool DeInit() override;
  bool Process(const std::vector<cx::BufferPtr>& data_vec) override;

private:
  std::shared_ptr<posto::Participant> participant_;
  std::shared_ptr<posto::Reader<SessionMessageT>> session_reader_;
};

DispatchProc::DispatchProc()
{
  ;
}

bool DispatchProc::Init(const std::map<std::string, std::string>& props) {

  /* ringbuffer group init */
  dispatch_ringbuffer_init();

  // sub session
  participant_ = posto::Domain::CreateParticipant("cmd_consumer");
  session_reader_ = participant_->CreateReader<SessionMessageT>("SessionMsg",
      [this] (const std::shared_ptr<SessionMessageT>& msg) {
        LOGD(TAG, "SessionMsg got, cmd_id: %d", msg->body().cmd_id());
        switch (msg->body().cmd_id()) {
            case thead::voice::proto::BEGIN:
            dispatch_ringbuffer_clear(TYPE_PCM);
            break;

            case thead::voice::proto::END:
            case thead::voice::proto::TIMEOUT:
            break;

            default:
            break;
        }
    });

  return true;
}

bool DispatchProc::DeInit() {
  dispatch_ringbuffer_destory(TYPE_KWS);
  dispatch_ringbuffer_destory(TYPE_PCM);
  dispatch_ringbuffer_destory(TYPE_FEAEC);

  return true;
}

bool DispatchProc::Process(const std::vector<cx::BufferPtr>& data_vec) {
  auto ptr0 = data_vec.at(0);

  if (ptr0) {
    auto iMemory0 = ptr0->GetMemory(0);
    auto iMeta0   = ptr0->GetMetadata<KwsOutMessageT>("dispatch_param");

    int len  = iMeta0->buf_len();

    data_type_e type = TYPE_MAX;
    switch(iMeta0->type()) {
      case thead::voice::proto::TYPE_PCM:
        type = TYPE_PCM;
        dispatch_ringbuffer_create(TYPE_PCM, PCM_RINGBUF_LEN);
        break;
      case thead::voice::proto::TYPE_KWS:
        type = TYPE_KWS;
        dispatch_ringbuffer_create(TYPE_KWS, KWS_RINGBUF_LEN);
        break;
      case thead::voice::proto::TYPE_FEAEC:
        type = TYPE_FEAEC;
        dispatch_ringbuffer_create(TYPE_FEAEC, FEAEC_RINGBUF_LEN); /* 处理程序中初始化,用到了才创建申请内存 */
        break;
      default: ;
    }

    if (type < TYPE_MAX) {
      dispatch_ringbuffer_write(type, (uint8_t *)iMemory0->data(), len);
    }
  }

  return true;
}

CX_REGISTER_PLUGIN(DispatchProc);

}  // namespace cpt
