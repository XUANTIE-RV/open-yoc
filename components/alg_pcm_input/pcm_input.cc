/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <memory>

#include <aos/kernel.h>
#include <ulog/ulog.h>

#include <cxvision/cxvision.h>

#include "pcm_input_internal.h"
#include "yoc/pcm_input.h"

#define TAG "data_input"

extern "C" {
void pcm_input_register();
}

// cxvision message
using DataInputMessageT = thead::voice::proto::DataInputMsg;
// pub/sub message
using RecordMessageT = posto::Message<thead::voice::proto::RecordMsg>;

namespace cpt
{

class DataInput : public cx::PluginBase
{
public:
    DataInput();
    static void pcm_data_cb(void *data, unsigned int len, void *arg);

    bool Init(const std::map<std::string, std::string> &props) override;
    bool DeInit() override;
    bool Process(const std::vector<cx::BufferPtr> &data_vec) override
    {
        return true;
    }

private:
    void                                            config(const std::map<std::string, std::string> &props);
    std::shared_ptr<posto::Participant>             participant_;
    std::shared_ptr<posto::Reader<RecordMessageT> > reader_;

    int record_chn_count_; /* 0:不录制 3:录制3路 5:录制5路 */

    int chn_num;
    int interleaved;
    int format; /* example:16 bit, 24 bit */
    int sample_rate;
    int frame_ms;
    int peroid_size; /* Single frame single channel sample count */
    int capture_byte;
};

DataInput::DataInput()
{
    participant_      = NULL;
    reader_           = NULL;
    record_chn_count_ = 0;
    chn_num           = 0;
    interleaved       = 0;
    format            = 0;
    sample_rate       = 0;
    frame_ms          = 0;
    peroid_size       = 0;
}

void DataInput::pcm_data_cb(void *data, unsigned int len, void *arg)
{
    DataInput *self        = static_cast<DataInput *>(arg);
    auto       oMemory     = cx::MemoryHelper::Malloc(self->capture_byte);
    int16_t *  capture_buf = (int16_t *)oMemory->data();
    memcpy(capture_buf, data, len);

    /* send the message included mic data and params to algrithm process */
    auto oMeta = std::make_shared<DataInputMessageT>();

    oMeta->set_chn_num(self->chn_num);
    oMeta->set_format(self->format);
    oMeta->set_sample_rate(self->sample_rate);
    oMeta->set_frame(self->peroid_size);

    auto output = std::make_shared<cx::Buffer>();
    output->AddMemory(oMemory);
    output->SetMetadata("alsa_param", oMeta);

    self->Send(0, output);

    if (self->record_chn_count_ == 3) {
        //发送到record_process#0节点
        self->Send(1, output);
    }
}

void DataInput::config(const std::map<std::string, std::string> &props)
{
    for (auto iter = props.begin(); iter != props.end(); ++iter) {
        if (strcmp(iter->first.c_str(), "chn_num") == 0) {
            this->chn_num = atoi(iter->second.c_str());
        } else if (strcmp(iter->first.c_str(), "frame_ms") == 0) {
            this->frame_ms = atoi(iter->second.c_str());
        } else if (strcmp(iter->first.c_str(), "interleaved") == 0) {
            this->interleaved = atoi(iter->second.c_str());
        } else if (strcmp(iter->first.c_str(), "sample_rate") == 0) {
            this->sample_rate = atoi(iter->second.c_str());
        } else if (strcmp(iter->first.c_str(), "pcm_bits") == 0) {
            this->format = atoi(iter->second.c_str());
        }
    }

    this->peroid_size = this->frame_ms * (this->sample_rate / 1000);

    LOGI(TAG, "chn_num %d, frame_ms %d, rate %d, bits %d, interleaved %d", this->chn_num, this->frame_ms,
         this->sample_rate, this->format, this->interleaved);
}

bool DataInput::Init(const std::map<std::string, std::string> &props)
{
    int err;
    // param config
    config(props);

    record_chn_count_ = 0;

    participant_ = posto::Domain::CreateParticipant("cmd_consumer");

    reader_
        = participant_->CreateReader<RecordMessageT>("RecordMsg", [this](const std::shared_ptr<RecordMessageT> &msg) {
              // LOGD(TAG, "Message got, cmd_id: %d\n", msg->body().cmd_id());
              switch (msg->body().cmd()) {
                  case thead::voice::proto::START:
                      record_chn_count_ = msg->body().record_chn_count();
                      break;

                  case thead::voice::proto::STOP:
                      record_chn_count_ = 0;
                      break;

                  default:
                      break;
              }
          });

    pcm_input_register();

    this->capture_byte = pcm_input_init(this->format, this->sample_rate, this->frame_ms, this->chn_num);
    if (this->capture_byte < 0) {
        LOGE(TAG, "Capture init failed %d\n", this->capture_byte);
        return false;
    }

    err = pcm_input_cb_register(pcm_data_cb, (void *)this);
    if (err) {
        LOGE(TAG, "Pcm cb register failed %d\n", err);
        return false;
    }

    return true;
}

bool DataInput::DeInit()
{
    int err = 0;
    err     = pcm_input_cb_unregister(pcm_data_cb);
    if (err) {
        LOGE(TAG, "Pcm cb unregister failed %d\n", err);
        return false;
    }
    return true;
}

CX_REGISTER_PLUGIN(DataInput);

} // namespace cpt
