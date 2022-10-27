/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <memory>

#include <cxvision/cxvision.h>
#include "ulog/ulog.h"
#include <aos/aos.h>

#define TAG "post"

using SspOutMessageT   = thead::voice::proto::SspOutMsg;
using InferOutMessageT = thead::voice::proto::InferOutMsg;
using VadOutMessageT   = thead::voice::proto::VadOutMsg;
using KwsOutMessageT   = thead::voice::proto::KwsOutMsg;


using SessionMessageT    = posto::Message<thead::voice::proto::SessionMsg>;

namespace cpt {

class PostProcess : public cx::PluginBase {
public:
  PostProcess();
private:
  int kws_chn;

  std::shared_ptr<posto::Participant> participant_;
  std::shared_ptr<posto::Writer<SessionMessageT>> writer_;

  void KwsSendMsg(uint8_t *buf, size_t buf_len, thead::voice::proto::DataType type);

public:
  bool Init(const std::map<std::string, std::string>& props) override;
  bool DeInit() override;
  bool Process(const std::vector<cx::BufferPtr>& data_vec) override;
};

void PostProcess::KwsSendMsg(uint8_t *buf, size_t buf_len, thead::voice::proto::DataType type)
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

    Send(0, output);
}

PostProcess::PostProcess()
{
  kws_chn = 0;
}

bool PostProcess::Init(const std::map<std::string, std::string>& props) {
  const auto& iter = props.find("max_time_ms");
  if (iter != props.end()) {
    LOGD(TAG, "max_time_ms: %s\n", iter->second.c_str());
  }

  participant_ = posto::Domain::CreateParticipant("cmd_producer");
  writer_ = participant_->CreateWriter<SessionMessageT>("SessionMsg");

  kws_chn = 0;

  return true;
}

bool PostProcess::DeInit() {

  return true;
}

bool PostProcess::Process(const std::vector<cx::BufferPtr>& data_vec) {
  auto ptr0 = data_vec.at(0); // ssp data
  auto ptr1 = data_vec.at(1); // kws info
  auto ptr2 = data_vec.at(2); // vad info

  int pub_flag = 0;

  auto msg = std::make_shared<SessionMessageT>();

  // ssp data
  if (ptr0) {
    auto iMemory0 = ptr0->GetMemory(0);
    auto iMeta0 = ptr0->GetMetadata<SspOutMessageT>("ssp_param");

    int16_t *data = (int16_t *)iMemory0->data();

    /*上云数据发送给dispatch*/
    KwsSendMsg((uint8_t *)&data[kws_chn * iMeta0->frame()], iMeta0->frame() * sizeof(int16_t), thead::voice::proto::TYPE_PCM);

    return true;
  }

  // kws state
  if (ptr1) {
    auto iMeta1 = ptr1->GetMetadata<InferOutMessageT>("kws_param");
    kws_chn = iMeta1->kws_chn();

    if (iMeta1->first_wakeup() == false) {
      return true;
    }

    LOGD(TAG, "  Port[1].Meta[\"kws_param\"]: kws_chn %d, kws_id %d, kws_score %d\n", 
          iMeta1->kws_chn(), iMeta1->kws_id(), iMeta1->kws_score());

    pub_flag = 1;
    msg->body().set_cmd_id(thead::voice::proto::BEGIN);
    msg->body().set_kws_id(iMeta1->kws_id());
    msg->body().set_kws_score(iMeta1->kws_score());
    msg->body().set_kws_word("nihaoxinbao");
  }

  // vad state
  if (ptr2) {
    auto iMeta2 = ptr2->GetMetadata<VadOutMessageT>("vad_param");

    pub_flag = 1;
    msg->body().set_cmd_id(thead::voice::proto::END);
    LOGD(TAG, "  Port[2].Meta[\"vad_param\"]: vad %d\n", iMeta2->vad_status());
  }

  // 发布唤醒和断句消息
  if (pub_flag) {
    LOGD(TAG, "PUB SessionMsg: cmd_id %d", msg->body().cmd_id());
    writer_->Write(msg);
  }

  return true;
}

CX_REGISTER_PLUGIN(PostProcess);

}  // namespace cpt
