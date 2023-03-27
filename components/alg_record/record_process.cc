/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <memory>

#include <aos/kv.h>
#include <cxvision/cxvision.h>
#include <ulog/ulog.h>
#include <aos/ringbuffer.h>
#include <aos/aos.h>

#include "record_process.h"

#define TAG "Record"

#define FA_READ_CNT    450                 // 10ms per time, send 4.5s data
#define FA_RINGBUF_LEN (16 * 2 * 3 * 3000) /* store fault wakeup data: 3sec, 3chn */

// cxvision message
using DataInputMessageT = thead::voice::proto::DataInputMsg;
using SspOutMessageT    = thead::voice::proto::SspOutMsg;
// pub/sub message
using SessionMessageT   = posto::Message<thead::voice::proto::SessionMsg>;

namespace cpt
{

class RecordProc : public cx::PluginBase
{

public:
    RecordProc();
    bool Init(const std::map<std::string, std::string> &props) override;
    bool DeInit() override;
    bool Process(const std::vector<cx::BufferPtr> &data_vec) override;

private:
    std::shared_ptr<posto::Participant>              participant_;
    std::shared_ptr<posto::Reader<SessionMessageT> > session_reader_;

    int wakeup_ringbuf_rec_;

    dev_ringbuf_t pcm_rb_;
    int           pcm_read_cnt_;

    dev_ringbuf_t ssp_rb_;
    int           ssp_read_cnt_;

private:
    int push_to_history_buffer(dev_ringbuf_t *ringbuffer, uint8_t *data, uint32_t length);
};

RecordProc::RecordProc()
{
    ssp_read_cnt_ = 0;
    wakeup_ringbuf_rec_ = 0;
    pcm_read_cnt_ = 0;
    participant_ = NULL;
    session_reader_ = NULL;
}

int RecordProc::push_to_history_buffer(dev_ringbuf_t *ringbuffer, uint8_t *data, uint32_t length)
{
    uint32_t tmplen = 0;

    tmplen = ringbuffer->length - ringbuffer->widx;
    if (length <= tmplen) {
        memcpy((void *)&ringbuffer->buffer[ringbuffer->widx], (void *)data, length);
    } else {
        memcpy((void *)&ringbuffer->buffer[ringbuffer->widx], (void *)data, tmplen);
        memcpy((void *)ringbuffer->buffer, (uint8_t *)data + tmplen, length - tmplen);
    }

    ringbuffer->widx = (ringbuffer->widx + length) % (ringbuffer->length + 1);

    return length;
}

bool RecordProc::Init(const std::map<std::string, std::string> &props)
{
    wakeup_ringbuf_rec_ = 0;

    pcm_read_cnt_ = FA_READ_CNT + 1;
    ssp_read_cnt_ = FA_READ_CNT + 1;

    // store previous 4s data to buffer
    aos_kv_getint("wakeup_rec", &wakeup_ringbuf_rec_);
    if (wakeup_ringbuf_rec_) {
        char *pcm_ringbuff = (char *)malloc(FA_RINGBUF_LEN);
        ringbuffer_create(&pcm_rb_, pcm_ringbuff, FA_RINGBUF_LEN);

        char *ssp_ringbuff = (char *)malloc(FA_RINGBUF_LEN);
        ringbuffer_create(&ssp_rb_, ssp_ringbuff, FA_RINGBUF_LEN);
    }

    participant_    = posto::Domain::CreateParticipant("cmd_consumer");
    session_reader_ = participant_->CreateReader<SessionMessageT>(
        "SessionMsg", [this](const std::shared_ptr<SessionMessageT> &msg) {
            LOGD(TAG, "SessionMsg got, cmd_id: %d", msg->body().cmd_id());
            switch (msg->body().cmd_id()) {
                case thead::voice::proto::BEGIN:
                    pcm_read_cnt_ = 0;
                    ssp_read_cnt_ = 0;
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

bool RecordProc::DeInit()
{

    return true;
}

bool RecordProc::Process(const std::vector<cx::BufferPtr> &data_vec)
{
    /*
     * usb 和 ws 只支持 3路和5路两种数据保存方式
     */

    /*
     * 3路数据保存
     */
    if (record_get_chncnt() == 3) {
        auto ptr0 = data_vec.at(0); // pcm data
        if (ptr0) {
            auto iMemory0 = ptr0->GetMemory(0);
            auto iMeta0   = ptr0->GetMetadata<DataInputMessageT>("alsa_param");

            int len = iMeta0->frame();
            int chn = iMeta0->chn_num();
            int pcm_record_len = len * chn * iMeta0->format() / 8;

            int16_t *pcm_record_data = (int16_t *)iMemory0->data();

            if (wakeup_ringbuf_rec_) {
                // store to ringbuffer
                push_to_history_buffer(&pcm_rb_, (uint8_t *)pcm_record_data, pcm_record_len);

                // if wakeup, send the ringbuffer data to server
                if (pcm_read_cnt_ == 0) {
                    pcm_rb_.ridx = (pcm_rb_.widx + pcm_record_len) % (pcm_rb_.length + 1);
                    LOGD(TAG, "read_first pcm_rb_.widx: %d ridx: %d\n", pcm_rb_.widx, pcm_rb_.ridx);
                }

                if (pcm_read_cnt_ <= FA_READ_CNT) {
                    pcm_read_cnt_++;

                    int read_len = ringbuffer_read(&pcm_rb_, (uint8_t *)pcm_record_data, pcm_record_len);
                    if (read_len != 0) {
                        rec_copy_data(0, (uint8_t *)pcm_record_data, pcm_record_len);
                    }
                }
            } else {
                rec_copy_data(0, (uint8_t *)pcm_record_data, pcm_record_len);
            }
        }
    }

    /*
     * 5/6路数据保存
     */
    if (record_get_chncnt() == 5 || record_get_chncnt() == 6) {
        auto ptr1 = data_vec.at(1); // ssp data
        // ssp data
        if (ptr1) {
            auto iMemory1 = ptr1->GetMemory(0);
            auto iMeta1   = ptr1->GetMetadata<SspOutMessageT>("ssp_param");

            int alg_rec_num = iMeta1->chn_num();

            int pcm_record_len = iMeta1->frame() * alg_rec_num * sizeof(int16_t);

            int16_t *pcm_record_data = (int16_t *)iMemory1->data();

            if (wakeup_ringbuf_rec_) {
                // store to ringbuffer
                push_to_history_buffer(&ssp_rb_, (uint8_t *)pcm_record_data, pcm_record_len);

                // if wakeup, send the ringbuffer data to server
                if (ssp_read_cnt_ == 0) {
                    ssp_rb_.ridx = (ssp_rb_.widx + pcm_record_len) % (ssp_rb_.length + 1);
                    LOGD(TAG, "read_first ssp_rb_.widx: %d ridx: %d\n", ssp_rb_.widx, ssp_rb_.ridx);
                }

                if (ssp_read_cnt_ <= FA_READ_CNT) {
                    ssp_read_cnt_++;

                    int read_len = ringbuffer_read(&ssp_rb_, (uint8_t *)pcm_record_data, pcm_record_len);
                    if (read_len != 0) {
                        rec_copy_data(0, (uint8_t *)pcm_record_data, pcm_record_len);
                    }
                }
            } else {
                rec_copy_data(0, (uint8_t *)pcm_record_data, pcm_record_len);
            }
        }
    }

    return true;
}

CX_REGISTER_PLUGIN(RecordProc);

} // namespace cpt

/*********************
 * 录制状态消息发布
 *********************/
#include <cxvision/cxvision.h>
#ifdef __cplusplus
extern "C"
{
#endif
using RecordMessageT = posto::Message<thead::voice::proto::RecordMsg>;
static std::shared_ptr<posto::Participant>             g_record_participant = NULL;
static std::shared_ptr<posto::Writer<RecordMessageT> > g_record_writer      = NULL;

void record_msg_publish(int start)
{
    if (g_record_participant == NULL) {
        g_record_participant = posto::Domain::CreateParticipant("cmd_producer");
        g_record_writer      = g_record_participant->CreateWriter<RecordMessageT>("RecordMsg");
    }

    auto msg = std::make_shared<RecordMessageT>();
    if (start) {
        msg->body().set_cmd(thead::voice::proto::START);
        msg->body().set_record_chn_count(record_get_chncnt());
    } else {
        msg->body().set_cmd(thead::voice::proto::STOP);
        msg->body().set_record_chn_count(0);
    }
    g_record_writer->Write(msg);
}

static int g_chncnt = 0;

void record_set_chncnt(int chncnt)
{
    g_chncnt = chncnt;
}

int record_get_chncnt()
{
    return g_chncnt;
}

#ifdef __cplusplus
}
#endif
