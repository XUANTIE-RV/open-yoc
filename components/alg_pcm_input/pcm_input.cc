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

#define TAG "data_input"

// cxvision message
using DataInputMessageT = thead::voice::proto::DataInputMsg;
// pub/sub message
using RecordMessageT    = posto::Message<thead::voice::proto::RecordMsg>;

namespace cpt
{

class DataInput : public cx::PluginBase
{
public:
    DataInput();
    static void data_input(void *arg);

    bool Init(const std::map<std::string, std::string> &props) override;
    bool DeInit() override;
    bool Process(const std::vector<cx::BufferPtr> &data_vec) override
    {
        return true;
    }

private:
    void config(const std::map<std::string, std::string> &props);

    aos_task_t task_;
    std::shared_ptr<posto::Participant>             participant_;
    std::shared_ptr<posto::Reader<RecordMessageT> > reader_;

    bool start_record_;

    int chn_num;
    int interleaved;
    int format; /* example:16 bit, 24 bit */
    int sample_rate;
    int frame_ms;
    int peroid_size; /* Single frame single channel sample count */
};

DataInput::DataInput()
{
    task_         = NULL;
    participant_  = NULL;
    reader_       = NULL;
    start_record_ = false;
    chn_num       = 0;
    interleaved   = 0;
    format        = 0; 
    sample_rate   = 0;
    frame_ms      = 0;
    peroid_size   = 0;
}

extern "C"
{
    int voice_pcm_acquire(void *data, int len);
    int voice_pcm_acquire_init(int bit_format, int sample_rate, int frame_ms, int chn_num);
}

void DataInput::data_input(void *arg)
{

    DataInput *self         = static_cast<DataInput *>(arg);
    int        capture_byte = 0;

    voice_pcm_acquire_register(&g_pcm_acquire_ops);

    capture_byte = voice_pcm_acquire_init(self->format, self->sample_rate, self->frame_ms, self->chn_num);

    LOGD(TAG, "Go to the algorithm process");

    /* capture main loop */
    while (1) {
        /* malloc and get mic data from audio, send to algrithm process */
        auto     oMemory      = cx::MemoryHelper::Malloc(capture_byte);
        int16_t *capture_buf  = (int16_t *)oMemory->data();

        int rlen = voice_pcm_acquire(capture_buf, capture_byte);

        if (rlen <= 0) {
            continue;
        }

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

        if (self->start_record_ == true) {
            self->Send(1, output);
        }
    }

    aos_task_exit(0);
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

    LOGI(TAG,
         "chn_num %d, frame_ms %d, rate %d, bits %d, interleaved %d",
         this->chn_num,
         this->frame_ms,
         this->sample_rate,
         this->format,
         this->interleaved);
}

bool DataInput::Init(const std::map<std::string, std::string> &props)
{
    // param config
    config(props);

    start_record_ = false;

    participant_ = posto::Domain::CreateParticipant("cmd_consumer");

    reader_
        = participant_->CreateReader<RecordMessageT>("RecordMsg", [this](const std::shared_ptr<RecordMessageT> &msg) {
              // LOGD(TAG, "Message got, cmd_id: %d\n", msg->body().cmd_id());
              switch (msg->body().cmd()) {
                  case thead::voice::proto::START:
                      start_record_ = true;
                      break;

                  case thead::voice::proto::STOP:
                      start_record_ = false;
                      break;

                  default:
                      break;
              }
          });

    aos_task_new_ext(&task_, "PcmInput", &data_input, this, 1024 * 8, AOS_DEFAULT_APP_PRI - 4);
    return true;
}

bool DataInput::DeInit()
{
    return true;
}

CX_REGISTER_PLUGIN(DataInput);

} // namespace cpt
