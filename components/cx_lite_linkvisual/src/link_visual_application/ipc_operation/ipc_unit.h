/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/**
 * 文件用于模拟IPC的功能，仅用于demo测试使用
 * 开发者不需要过多关注，实际产品不会使用到该部分代码
 */

#ifndef  PROJECT_DUMMY_IPC_UNIT_H
#define  PROJECT_DUMMY_IPC_UNIT_H

#include <vector>
#include <string>

#include "media_parser.h"
#include "picture_parser.h"
#include "ipc_defs.h"
#include "property_parser.h"
#include "link_visual_struct.h"
#include "ipc_vod.h"

class IpcUnit {
public:
    IpcUnit(VideoData video, AudioData audio, PictureData pic, PropertyData property, RecordData record, VodCmdData vod_cmd);
    ~IpcUnit();

    int SetDevAuth(const lv_device_auth_s *auth);

    bool DevAuthCheck(const lv_device_auth_s *auth);

    int Start(const std::vector<std::string> &live, const std::vector<std::string> &vod, const std::string &pic, unsigned int picture_interval_s);

    void Stop();

    void LiveStart(int stream_type, int service_id);

    void LiveStop(int service_id);

    void LiveRequestIFrame(int service_id);

    int LiveGetParam(int service_id, lv_audio_param_s *aparam, lv_video_param_s *vparam);

    int VodGetParam(lv_audio_param_s *aparam, lv_video_param_s *vparam);

    void VodStop();

    void VodPause(bool pause);

    int VodSeek(int service_id, unsigned int timestamp_ms);

    int VodSeekFile(int service_id, unsigned int timestamp_ms, const std::string &file);

    void VodSetParam(unsigned int speed, unsigned int key_only);

    void VodQueryList(const lv_query_record_param_s *param);

    void SetProperty(const std::string &key, const std::string &value);

    void GetAllProperty();

    int GetPicture(unsigned char **data, unsigned int *data_len);

private:
    static bool checkStreamFileExist(const std::string &stream_prefix);
    static bool checkStreamFileGroupExist(const std::vector<std::string> &stream_group);
    static bool checkFileExist(const std::string &file_name);
    static int videoBufferHandler(void *arg, lv_video_format_e format, unsigned char *buffer, unsigned int buffer_size,
                                   unsigned int timestamp_ms, int nal_type, int service_id);
    static int audioBufferHandler(void *arg, lv_audio_format_e format, unsigned char *buffer, unsigned int buffer_size,
                                   unsigned int timestamp_ms, int service_id);
    static void pictureBufferHandler(void *arg, unsigned char *buffer, unsigned int buffer_len, const char *id);
    static void propertyHandler(void *arg, const std::string &value);
    static int vodCmdHandler(void *arg, int vod_cmd, int param, int service_id);

private:
    std::vector<std::string> vod_file_;
    unsigned int check_day_start_time;
    MediaParse *vod_current_;
    MediaParse *live_main_;
    MediaParse *live_sub_;
    PictureParser *pic_;
    PropertyParser *property_;

    VideoData video_data_;
    AudioData audio_data_;
    PictureData pic_data_;
    PropertyData property_data_;
    RecordData record_data_;
    VodCmdData vod_cmd_data_;
    bool start_;

    std::string auth_pk_;
    std::string auth_dn_;
    std::string auth_ds_;
    int dev_id_;
private:
    static const unsigned int kMaxNum = 2;

    IpcVodUnit *ipc_vod;
};





#endif // PROJECT_DUMMY_IPC_UNIT_H
