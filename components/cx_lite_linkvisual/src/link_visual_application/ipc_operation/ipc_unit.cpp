/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/**
 * 文件用于模拟IPC的功能，仅用于demo测试使用
 * 开发者不需要过多关注，实际产品不会使用到该部分代码
 */

#include "ipc_unit.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "ulog/ulog.h"
#include "link_visual_struct.h"
#include "dirent.h"
#include "lv_internal_api.h"

#define TAG "IPC_UNIT"

IpcUnit::IpcUnit(VideoData video, AudioData audio, PictureData pic, PropertyData property, RecordData record, VodCmdData vod_cmd) {
#ifdef DUMMY_IPC
    live_main_ = new MediaParse(videoBufferHandler, audioBufferHandler, vodCmdHandler, this);
    live_sub_ = new MediaParse(videoBufferHandler, audioBufferHandler, vodCmdHandler, this);
    vod_current_ = new MediaParse(videoBufferHandler, audioBufferHandler, vodCmdHandler, this);
    pic_ = new PictureParser(pictureBufferHandler, this);
#else
    live_main_ = new MediaParse(videoBufferHandler, audioBufferHandler, vodCmdHandler, this);
    live_sub_ = new MediaParse(videoBufferHandler, audioBufferHandler, vodCmdHandler, this);
    pic_ = new PictureParser(pictureBufferHandler, this);
#endif
    property_ = new PropertyParser(propertyHandler, this);

    video_data_ = video;
    audio_data_ = audio;
    pic_data_ = pic;
    property_data_ = property;
    record_data_ = record;
    vod_cmd_data_ = vod_cmd;
#ifdef DUMMY_IPC
    assert(video_data_);
    assert(audio_data_);
    assert(pic_data_);
    assert(property_data_);
#endif
    assert(record_data_);
    start_ = false;
    ipc_vod = new IpcVodUnit(video, audio, property, record, vod_cmd);
    dev_id_ = -1;

    check_day_start_time = 0;
    vod_current_ = nullptr;
}

IpcUnit::~IpcUnit() {
    delete pic_;
#ifdef DUMMY_IPC
    delete vod_current_;
#endif
    delete property_;
    delete live_main_;
    delete live_sub_;

    delete ipc_vod;
}

int IpcUnit::SetDevAuth(const lv_device_auth_s *auth) {
    if (!(auth && auth->product_key && auth->device_secret && auth->device_name)) {
        return -1;
    }

    auth_pk_ = auth->product_key;
    auth_dn_ = auth->device_name;
    auth_ds_ = auth->device_secret;
    dev_id_ = auth->dev_id;

    return 0;
}

bool IpcUnit::DevAuthCheck(const lv_device_auth_s *auth) {
    if (!auth) {
        return false;
    }

    if (dev_id_ == auth->dev_id &&
        (auth_pk_ == auth->product_key) &&
        (auth_dn_ == auth->device_name) &&
        (auth_ds_ == auth->device_secret)) {
        return true;
    }

    return false;
}

int IpcUnit::Start(const std::vector<std::string> &live,
                   const std::vector<std::string> &vod,
                   const std::string &pic,
                   unsigned int picture_interval_s) {
    if (start_) {
        return 0;
    }

#ifdef DUMMY_IPC
    if (live.size() != kMaxNum || vod.size() != kMaxNum) {
        return -1;
    }
    if (!(checkStreamFileGroupExist(live) &&
        checkStreamFileGroupExist(vod) &&
        checkFileExist(pic))) {
        return -1;
    }
#else
    // checkStreamFileGroupExist(vod);
#endif
    /* 直播主子码流和抓图默认打开 */
    live_main_->StartLive(0, live[0]);
    live_sub_->StartLive(0, live[1]);
    // pic_->Start(picture_interval_s, pic);

    /* 点播保存下要播放的文件名 */
    vod_file_ = vod;

    start_ = true;
    return 0;
}

void IpcUnit::Stop() {

}

void IpcUnit::LiveStart(int stream_type, int service_id) {
#ifndef DUMMY_IPC
    int ret;
    ret = linkvisual_start_video_encode(1);
    if(ret < 0) {
        return;
    }
    ret = linkvisual_start_audio_encode(1);
    if(ret < 0) {
        return;
    }
#endif
    if (stream_type == 0) {
        live_main_->SetServiceId(service_id);
        live_main_->RequestIFrame();
        live_main_->live_running_flag = 1;
    } else {
        live_sub_->SetServiceId(service_id);
        live_sub_->RequestIFrame();
        live_sub_->live_running_flag = 1;
    }
}

void IpcUnit::LiveStop(int service_id) {
    if (live_main_->GetServiceId() == service_id) {
        live_main_->SetServiceId(0);
#ifndef DUMMY_IPC
        linkvisual_start_video_encode(0);
        linkvisual_start_audio_encode(0);
#endif
        return;
    }

    if (live_sub_->GetServiceId() == service_id) {
        live_sub_->SetServiceId(0);
#ifndef DUMMY_IPC
        linkvisual_start_video_encode(0);
        linkvisual_start_audio_encode(0);
#endif
        return;
    }

    LOGE(TAG, "LiveStop failed, service id =%d", service_id);
}

void IpcUnit::LiveRequestIFrame(int service_id) {
    if (live_main_->GetServiceId() == service_id) {
        live_main_->RequestIFrame();
        return;
    }

    if (live_sub_->GetServiceId() == service_id) {
        live_sub_->RequestIFrame();
        return;
    }

    LOGE(TAG, "LiveRequestIFrame failed, service id =%d", service_id);
}

int IpcUnit::LiveGetParam(int service_id, lv_audio_param_s *aparam, lv_video_param_s *vparam) {
    if (!vparam || !aparam) {
        return -1;
    }

    // if (live_main_->GetServiceId() == service_id) {
    //     live_main_->GetParam(*vparam, *aparam);
    //     return 0;
    // }

    // if (live_sub_->GetServiceId() == service_id) {
    //     live_sub_->GetParam(*vparam, *aparam);
    //     return 0;
    // }

    // //音频
    // if (live_main_->GetServiceId() > 0) {
    //     live_main_->GetParam(*vparam, *aparam);
    //     return 0;
    // }
    // if (live_sub_->GetServiceId() > service_id) {
    //     live_sub_->GetParam(*vparam, *aparam);
    //     return 0;
    // }
    live_main_->GetParam(*vparam, *aparam);
    return -1;
}

int IpcUnit::VodGetParam(lv_audio_param_s *aparam, lv_video_param_s *vparam) {
    if (!vparam || !aparam) {
        return -1;
    }
#ifdef DUMMY_IPC
    vod_current_->GetParam(*vparam, *aparam);
#else
    ipc_vod->GetParam(*aparam, *vparam);
#endif
    return 0;
}

void IpcUnit::VodStop() {
#ifdef DUMMY_IPC
    vod_current_->Stop();
#else
    ipc_vod->Stop();
#endif
}

void IpcUnit::VodPause(bool pause) {
#ifdef DUMMY_IPC
    vod_current_->Pause(pause);
#else
    ipc_vod->Pause(pause);
#endif
}

int IpcUnit::VodSeek(int service_id, unsigned int timestamp_ms) {
#ifndef DUMMY_IPC
    ipc_vod->Start(service_id, timestamp_ms);
#else
    double duration = 0;
    unsigned int timestamp_file_ms = 0;
    unsigned int i;
    for (i = 0; i < vod_file_.size(); i ++) {
        MediaParse::GetDuration(vod_file_[i], duration);
        timestamp_file_ms += (unsigned int)duration * 1000;
        if (timestamp_ms <= timestamp_file_ms) {
            break;
        }
    }
    if (timestamp_ms > timestamp_file_ms) {
        return -1;
    }

    /* 第一次打开 */
    if (!vod_current_->GetStatus()) {
        vod_current_->Pause(true);
        vod_current_->StartVod(service_id,vod_file_[i], (timestamp_file_ms - (unsigned int) duration * 1000));
        vod_current_->Seek(timestamp_ms - (timestamp_file_ms - (unsigned int)duration * 1000));
    } else {
        vod_current_->Pause(true);//先暂停当前流
        if (vod_file_[i] != vod_current_->GetStreamPrefix()) {
            vod_current_->Stop();
            vod_current_->StartVod(service_id, vod_file_[i], (timestamp_file_ms - (unsigned int) duration * 1000));
        }
        vod_current_->Seek(timestamp_ms - (timestamp_file_ms - (unsigned int)duration * 1000));
        vod_current_->Pause(false);
    }
#endif
    return 0;
}

int IpcUnit::VodSeekFile(int service_id, unsigned int timestamp_ms, const std::string &file) {
    if (file.empty()) {//非第一次打开
        vod_current_->Seek(timestamp_ms);
    } else {//第一次打开
        unsigned int i;
        for (i = 0; i < vod_file_.size(); i ++) {
            if (file == vod_file_[i]) {
                break;
            }
        }
        if (i >= vod_file_.size()) {
            LOGE(TAG, "Query file failed");
            return -1;
        }

        vod_current_->Pause(true);
#ifndef DUMMY_IPC
        vod_current_->StartVod(service_id, 0);
#else 
        vod_current_->StartVod(service_id, file, 0);
#endif
        vod_current_->Seek(0);

    }
    return 0;
}

void IpcUnit::VodSetParam(unsigned int speed, unsigned int key_only) {
#ifdef DUMMY_IPC
    vod_current_->SetSpeed(speed, key_only);
#else
    ipc_vod->SetParam(speed, key_only);
#endif
}

void IpcUnit::VodQueryList(const lv_query_record_param_s *param) {
    if (!param) {
        return;
    }
#ifndef DUMMY_IPC
    ipc_vod->QueryList(param);
#else
    lv_query_record_response_param_s *response = new lv_query_record_response_param_s;
    memset(response, 0, sizeof(lv_query_record_response_param_s));

    if (param->common.type == LV_QUERY_RECORD_BY_DAY) {
        int answer_num = vod_file_.size();
        response->by_day.num = answer_num;
        response->by_day.days = new lv_query_record_response_day[answer_num];
        memset(response->by_day.days, 0, sizeof(lv_query_record_response_day) * answer_num);
        double duration = 0;
        for (int i = 0; i < answer_num; i ++) {
            MediaParse::GetDuration(vod_file_[i], duration);
            response->by_day.days[i].file_size = 1024;//demo简单赋值
            response->by_day.days[i].record_type = LV_STORAGE_RECORD_INITIATIVE;
            response->by_day.days[i].file_name = new char [vod_file_[i].size() + 1];
            memcpy(response->by_day.days[i].file_name, vod_file_[i].c_str(), vod_file_[i].size());
            response->by_day.days[i].file_name[vod_file_[i].size()] = '\0';
            if (i == 0) {
                response->by_day.days[i].start_time = param->by_day.start_time;
                response->by_day.days[i].stop_time = param->by_day.start_time + (int)duration;
            } else {
                response->by_day.days[i].start_time = response->by_day.days[i - 1].stop_time;
                response->by_day.days[i].stop_time = response->by_day.days[i - 1].stop_time + (int)duration;
            }

        }
        record_data_(param->common.service_id, response);
    } else if (param->common.type == LV_QUERY_RECORD_BY_MONTH) {
        response->by_month.months = new int[31];
        memset(response->by_month.months, 0, 31 * sizeof(int));
        /* demo设置部分天数为有录像 */
        response->by_month.months[0] = 1;
        response->by_month.months[10] = 1;
        response->by_month.months[27] = 1;
        record_data_(param->common.service_id, response);
    }

    for (unsigned int i = 0; i < response->by_day.num; i++) {
        delete [] response->by_day.days[i].file_name;
    }
    delete [] response->by_month.months;
    delete [] response->by_day.days;
    delete response;
#endif
}

void IpcUnit::SetProperty(const std::string &key, const std::string &value) {
    property_->SetProperty(key, value);
}

void IpcUnit::GetAllProperty() {
    property_->GetAllProperty();
}

int IpcUnit::GetPicture(unsigned char **data, unsigned int *data_len) {
    if (data && data_len) {
        *data = pic_->GetPictureData();
        *data_len = pic_->GetPictureLen();
        return 0;
    }

    return -1;
}

bool IpcUnit::checkStreamFileExist(const std::string &stream_prefix) {
    std::string stream_index = stream_prefix + ".index";
    std::string stream_param = stream_prefix + ".meta";

    return checkFileExist(stream_index) &&
        checkFileExist(stream_param) &&
        checkFileExist(stream_prefix);
}

bool IpcUnit::checkStreamFileGroupExist(const std::vector<std::string> &stream_group) {
    for (std::vector<std::string>::const_iterator search = stream_group.begin(); search != stream_group.end(); ++ search) {
        if (!checkStreamFileExist(*search)) {
            return false;
        }
    }
    return true;
}

bool IpcUnit::checkFileExist(const std::string &file_name) {
    FILE *fp = NULL;
    fp = fopen(file_name.c_str(), "r");
    if (!fp) {
        LOGE(TAG, "Check files failed: %s", file_name.c_str());
        return false;
    }
    fclose(fp);
    return true;
}

int IpcUnit::videoBufferHandler(void *arg, lv_video_format_e format, unsigned char *buffer, unsigned int buffer_size,
                               unsigned int timestamp_ms, int nal_type, int service_id) {
    //LOGD(TAG, "service_id:%d, present_time:%u nal_type:%d ", service_id, timestamp_ms, nal_type);
    int ret = -1;
    if (!arg) {
        return ret;
    }

    IpcUnit *unit = (IpcUnit *)arg;
    if (service_id > 0) {//service_id大于0再往外抛出数据
        ret = unit->video_data_(service_id, format, buffer, buffer_size, timestamp_ms, nal_type);
    }
    return ret;
}

int IpcUnit::audioBufferHandler(void *arg, lv_audio_format_e format, unsigned char *buffer, unsigned int buffer_size,
                               unsigned int timestamp_ms, int service_id) {
    //printf("service_id:%d, present_time:%u  buffer_size:%u\n", service_id, timestamp_ms, buffer_size);
    int ret = -1;
    if (!arg) {
        return ret;
    }

    IpcUnit *unit = (IpcUnit *)arg;
    if (service_id) {//service_id大于0再往外抛出数据
        ret = unit->audio_data_(service_id, format, buffer, buffer_size, timestamp_ms);
    }
    return ret;
}

int IpcUnit::vodCmdHandler(void *arg, int vod_cmd, int param, int service_id) {
    if (!arg) {
        return -1;
    }

    IpcUnit *unit = (IpcUnit *)arg;
    if (service_id) {//service_id大于0再往外抛出数据
        unit->vod_cmd_data_(service_id, vod_cmd, param);
    }
    return 0;
}

void IpcUnit::pictureBufferHandler(void *arg, unsigned char *buffer, unsigned int buffer_len, const char *id) {
    if (!arg) {
        return;
    }

    IpcUnit *unit = (IpcUnit *)arg;
    lv_device_auth_s auth = {0};
    auth.dev_id = unit->dev_id_;
    auth.product_key = (char *)unit->auth_pk_.c_str();
    auth.device_name = (char *)unit->auth_dn_.c_str();
    auth.device_secret = (char *)unit->auth_ds_.c_str();
    unit->pic_data_(&auth, buffer, buffer_len);
}

void IpcUnit::propertyHandler(void *arg, const std::string &value) {
    if (!arg) {
        return;
    }

    IpcUnit *unit = (IpcUnit *)arg;
    lv_device_auth_s auth = {0};
    auth.dev_id = unit->dev_id_;
    auth.product_key = (char *)unit->auth_pk_.c_str();
    auth.device_name = (char *)unit->auth_dn_.c_str();
    auth.device_secret = (char *)unit->auth_ds_.c_str();
    unit->property_data_(&auth, value.c_str());
}
