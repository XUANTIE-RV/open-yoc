/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if !(defined(CONFIG_BOARD_AMP_LOAD_FW) && CONFIG_BOARD_AMP_LOAD_FW)

#include <stdio.h>
#include <memory>

#include <ulog/ulog.h>
#include <cxvision/cxvision.h>

#include "ly_kws.h"

#define TAG "infer"

#define MIN_WAKE_INTERV 15 /*15*20ms=300ms*/

#define CHN_NUM    3

using SspOutMessageT   = thead::voice::proto::SspOutMsg;
using InferOutMessageT = thead::voice::proto::InferOutMsg;

using AlgCmdMessageT   = posto::Message<thead::voice::proto::AlgCmdMsg>;

namespace cpt
{

class Inference : public cx::PluginBase
{
public:
    Inference();
    bool Init(const std::map<std::string, std::string> &props) override;
    bool DeInit() override;
    bool Process(const std::vector<cx::BufferPtr> &data_vec) override;

private:
    int inited_flag_;
    int output_data_[CHN_NUM][2];

    int kws_chn_id_;
    int ref_chn_id_;

    int16_t *frame_buf_;
    size_t frame_buf_len_;

    void *temp_buf_;
    size_t temp_buf_len_;

    int merged_flag_;
    int kws_proc_cnt_;

    int max_score_;

    int push2talk_;

    kws_info_t info_skws;
    kws_handle_t handle_skws;
    int ret_info;

    std::shared_ptr<posto::Participant>              participant_;
    std::shared_ptr<posto::Reader<AlgCmdMessageT> >  algcmd_reader_;
};

Inference::Inference()
{
    inited_flag_ = 0;

    kws_chn_id_= 0;
    ref_chn_id_= 0;
    frame_buf_ = nullptr;
    frame_buf_len_ = 0;
    temp_buf_ = nullptr;
    temp_buf_len_ = 0;
    merged_flag_= 0;
    kws_proc_cnt_= 0;
    max_score_= 0;
    push2talk_= 0;
    participant_ = nullptr;
    algcmd_reader_ = nullptr;
    handle_skws = nullptr;
    ret_info = 0;
}

bool Inference::Init(const std::map<std::string, std::string> &props)
{
    handle_skws = aie_register(skws_fsmn);
    ret_info = aie_get_kws_info(handle_skws, &info_skws);
    const auto &iter = props.find("model_path");

    if (iter != props.end()) {
        LOGD(TAG, "model_path: %s\n", iter->second.c_str());
    }

    for(int i = 0; i < CHN_NUM; i++) {
        for(int j = 0; j < 2; j ++) {
            output_data_[i][j] = 0;
        }
    }

    frame_buf_ = (int16_t *)malloc(320 * CHN_NUM * 2);


    ref_chn_id_ = CHN_NUM - 1;

    participant_ = posto::Domain::CreateParticipant("cmd_producer");

    algcmd_reader_
        = participant_->CreateReader<AlgCmdMessageT>("AlgCmdMsg", [this](const std::shared_ptr<AlgCmdMessageT> &msg) {
              LOGD(TAG, "AlgCmdMsg got, cmd: %d", msg->body().cmd());
              switch (msg->body().cmd()) {
                  case thead::voice::proto::PUSH2TALK_CMD:
                      push2talk_ = msg->body().ivalue();
                      break;

                  default:
                      break;
              }
          });

    return true;
}

bool Inference::DeInit()
{
    if (temp_buf_) {
        free(temp_buf_);
    }

    return true;
}

bool Inference::Process(const std::vector<cx::BufferPtr> &data_vec)
{
    auto iMemory = data_vec.at(0)->GetMemory(0);
    int16_t *data_in = (int16_t *)iMemory->data();

    auto iMeta = data_vec.at(0)->GetMetadata<SspOutMessageT>("ssp_param");

    if (inited_flag_ == 0) {
        int ret_init = aie_kws_init(handle_skws, iMeta->chn_num(), &frame_buf_len_, &temp_buf_len_);
        int temp_buf_byte_len = temp_buf_len_ * iMeta->chn_num() * sizeof(int16_t);
        temp_buf_ = malloc(temp_buf_byte_len);
        LOGD(TAG, "kws init. chn_num: %d, frame %d, temp:%p(%ld), ret_info %d, ret_init %d\n", 
             iMeta->chn_num(), iMeta->frame(), temp_buf_, temp_buf_byte_len, ret_info, ret_init);
        inited_flag_ = 1;
    }

    // 2 10ms frames merge to 20ms frame
    merged_flag_ = (merged_flag_ + 1) % 2;

    if (merged_flag_) {
        for (int i = 0; i < iMeta->chn_num(); i++) {
            memcpy(&frame_buf_[iMeta->frame() * i * 2], &data_in[iMeta->frame() * i], iMeta->frame() * sizeof(int16_t));
        }

        return true;
    } else {
        for (int i = 0; i < iMeta->chn_num(); i++) {
            memcpy(&frame_buf_[iMeta->frame() * i * 2 + iMeta->frame()], &data_in[iMeta->frame() * i], iMeta->frame() * sizeof(int16_t));
        }
    }

    // process kws
    aie_kws_run(handle_skws, frame_buf_, frame_buf_len_, temp_buf_, temp_buf_len_);
    aie_kws_postprocess(handle_skws, temp_buf_, output_data_);

    int kws_state = 0;
    int score = 0;

    int first_wkup = 0;

    // check whether the kws is ref chn
    // if (output_data_[ref_chn_id_][1]) {
    //   kws_state = 0;
    // } else {
    // choice one kwsed chn
    for (int i = 0; i < iMeta->chn_num(); i++) {
        if (output_data_[i][0] >= 0) {
            LOGD(TAG, "    chn %d wakeup: [%d %d]", i, output_data_[i][0], output_data_[i][1]);

            if (output_data_[i][1] > score) {
                score = output_data_[i][1];
                kws_chn_id_ = i;
            }

            kws_state  = 1;
        }
    }

    // }

    //连续唤醒检查
    if (kws_state == 1 && kws_proc_cnt_ == 0) {
        kws_proc_cnt_  = MIN_WAKE_INTERV;
        max_score_ = score;
        first_wkup = 1;
    } else {
        if (kws_proc_cnt_ > 0) {
            kws_proc_cnt_ --;
        }

        if (max_score_ < score) {
            max_score_ = score;
        } else {
            kws_state = 0;
        }
    }

    // //唤醒处理
    if (kws_state == 1 || push2talk_ == 1) {
        if (push2talk_) {
            first_wkup = 1;
            push2talk_ = 0;
        }

        auto oMeta = std::make_shared<InferOutMessageT>();
        oMeta->set_kws_chn(kws_chn_id_);
        oMeta->set_kws_id(output_data_[kws_chn_id_][0]);
        oMeta->set_kws_score(max_score_);
        oMeta->set_first_wakeup(first_wkup);

        auto output = std::make_shared<cx::Buffer>();
        output->SetMetadata("kws_param", oMeta);

        Send(0, output);
        Send(1, output);
    }

    return true;
}

CX_REGISTER_PLUGIN(Inference);

}  // namespace cpt

#endif