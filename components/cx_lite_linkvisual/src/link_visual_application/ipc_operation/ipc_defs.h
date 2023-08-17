/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

/**
 * 文件用于模拟IPC的功能，仅用于demo测试使用
 * 开发者不需要过多关注，实际产品不会使用到该部分代码
 */


#ifndef PROJECT_IPC_DEFS_H
#define PROJECT_IPC_DEFS_H

#include "link_visual_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 视频帧数据回调 */
typedef int (*VideoData)(int service_id, lv_video_format_e format, unsigned char *buffer, unsigned int buffer_size,
                          unsigned int timestamp_ms, int nal_type);

/* 音频帧数据回调 */
typedef int (*AudioData)(int service_id, lv_audio_format_e format, unsigned char *buffer, unsigned int buffer_size, unsigned int timestamp_ms);

/* 录像列表查询回调 */
typedef void (*RecordData)(int service_id, const lv_query_record_response_param_s* param);

/* 报警图片数据回调 */
typedef void (*PictureData)(const lv_device_auth_s *auth, unsigned char *buffer, unsigned int buffer_size);

/* 属性设置回调 */
typedef void (*PropertyData)(const lv_device_auth_s *auth, const char *value);

/* 录像播放完成回调 */
typedef int (*VodCmdData)(int service_id, int vod_cmd, int param);

#ifdef __cplusplus
}
#endif


#endif //PROJECT_IPC_DEFS_H
