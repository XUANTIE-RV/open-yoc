/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/**
 * 文件用于模拟IPC的功能，仅用于demo测试使用
 * 开发者不需要过多关注，实际产品不会使用到该部分代码
 */

#ifndef PROJECT_NORMAL_IPC_H
#define PROJECT_NORMAL_IPC_H


/**
 * 文件用于模拟IPC输出视音频流
 * 开发者不需要过多关注，实际产品不会使用到该部分代码
 */

#include "link_visual_struct.h"
#include "ipc_defs.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif



#define NormalIpcMediaFileLen (256)
typedef  struct {
    char source_file[NormalIpcMediaFileLen + 1];
} NormalIpcMediaFile;

typedef enum {
    NORMAL_IPC_MEDIA_START = 0,
    NORMAL_IPC_MEDIA_PAUSE,
    NORMAL_IPC_MEDIA_UNPAUSE,
    NORMAL_IPC_MEDIA_STOP,
    NORMAL_IPC_MEDIA_SEEK,
    NORMAL_IPC_MEDIA_REQUEST_I_FRAME,
    NORMAL_IPC_MEDIA_SET_PARAM,
} NormalIpcMediaCmd;

typedef struct {
    int service_id;
    unsigned int stream_type;
    unsigned int seek_timestamp_ms;
    unsigned int base_time_ms;
    unsigned int speed;
    unsigned int key_only;
    char source_file[NormalIpcMediaFileLen + 1];
} NormalIpcMediaParam;

typedef struct {//单个IPC需要配置的源
    /* 模拟实时直播的源 */
    unsigned int live_num;//num最大值为2，主码流和子码流
    NormalIpcMediaFile *live_source;

    /* 模拟本地录像的源 */
    // unsigned int vod_num;
    // NormalIpcMediaFile *vod_source;

    /* 模拟图片源 */
    char *picture_source;

    /* 每隔一段时间，触发一次移动报警抓图 */
    unsigned int picture_interval_s;

    /* 设备认证信息 */
    lv_device_auth_s *auth;
} IpcUnitParam;

typedef struct {
    unsigned int ipc_num;//IPC的数量，大于0

    IpcUnitParam *ipc_series;//结构体数组

    /* 视频帧数据回调 */
    VideoData video_handler;

    /* 音频帧数据回调 */
    AudioData audio_handler;

    /* 报警图片数据回调 */
    PictureData picture_handler;

    /* 属性设置回调 */
    PropertyData set_property_handler;

    RecordData query_record_handler;
    /* 点播消息回调 */
    VodCmdData vod_cmd_handler;
} NormalIpcConfig;


/* 开启虚拟IPC */
int normal_ipc_start(const NormalIpcConfig *config);

/* 停止虚拟IPC */
void normal_ipc_stop();

/* 直播控制（开启、切换主子码流） */
int normal_ipc_live(const lv_device_auth_s *auth, NormalIpcMediaCmd cmd, const NormalIpcMediaParam *param);

/* 获取直播流参数 */
int normal_ipc_live_get_media_param(const lv_device_auth_s *auth, int service_id, lv_audio_param_s *aparam, lv_video_param_s *vparam);

/* 获取当前点播文件的配置 */
int normal_ipc_vod_get_media_param(const lv_device_auth_s *auth, lv_video_param_s *vparam, lv_audio_param_s *aparam);

/* 按utc点播控制（打开、关闭文件、暂停、seek) */
int normal_ipc_vod_by_utc(const lv_device_auth_s *auth, NormalIpcMediaCmd cmd, const NormalIpcMediaParam *param);

/* 按file name点播控制（打开、关闭文件、暂停、seek) */
int normal_ipc_vod_by_file(const lv_device_auth_s *auth, NormalIpcMediaCmd cmd, const NormalIpcMediaParam *param);

/* 获取要上报点播文件的信息 */
void normal_ipc_report_vod_list(const lv_device_auth_s *auth, const lv_query_record_param_s *param);

/* 设置属性 */
int normal_ipc_set_property(const lv_device_auth_s *auth, const char *key, const char *value);

/* 获取所有属性值 */
void normal_ipc_get_all_property(const lv_device_auth_s *auth);

/* 获取一次抓图的图像数据 */
int normal_ipc_capture(const lv_device_auth_s *auth, unsigned char **data, unsigned int *data_len);




#ifdef __cplusplus
}
#endif

#endif //PROJECT_NORMAL_IPC_H
