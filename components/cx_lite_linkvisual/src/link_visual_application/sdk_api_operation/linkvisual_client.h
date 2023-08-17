/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef PROJECT_LINKVISUAL_DEMO_H
#define PROJECT_LINKVISUAL_DEMO_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "sdk_assistant.h"
#include "link_visual_enum.h"
#include <cx/cloud_lv_service.h>
// #include "cx_common.h"

typedef void (*lv_set_property_handler)(const lv_device_auth_s *auth, const char *value);
/**
* 文件用于描述LinkVisual的启动、结束过程，以及生命周期过程中对音视频的传输过程
*/

/* linkvisual资源初始化 */
int linkvisual_client_init(unsigned int device_type, int sub_num, lv_log_level_e log_level);

/* linkvisual资源销毁 */
void linkvisual_client_destroy();

int linkvisual_client_assistant_start(unsigned int device_type, const lv_device_auth_s *main, unsigned int sub_num, const lv_device_auth_s *sub);

void linkvisual_client_assistant_stop();

/* 来自IPC的视频帧数据回调 */
int linkvisual_client_video_handler(int service_id, lv_video_format_e format, unsigned char *buffer, unsigned int buffer_size,
                                     unsigned int present_time, int nal_type);

/* 来自IPC的音频帧数据回调 */
int linkvisual_client_audio_handler(int service_id,
                                     lv_audio_format_e format,
                                     unsigned char *buffer,
                                     unsigned int buffer_size,
                                     unsigned int present_time);

/* 来自IPC的报警图片数据回调 */
void linkvisual_client_picture_handler(const lv_device_auth_s *auth, unsigned char *buffer, unsigned int buffer_size);


/* 来自IPC的点播命令回调 */
int linkvisual_client_vod_handler(int service_id, int vod_cmd, int param);

/* 来自IPC的录像列表查询回调 */
void linkvisual_client_query_record(int service_id, const lv_query_record_response_param_s* param);

/* 调试过程中使用，可以读取交互式命令，改变部分SDK的行为，可改变的行为请参考：lv_control_type_e */
void linkvisual_client_control_test();

/* link visual 抓拍上传接口*/
int linkvisual_snapshot_upload(unsigned char *buff, int len, lv_media_format type);

void iot_register_set_property_callback(lv_set_property_handler cb);

lv_set_property_handler linkvisual_get_property_handler(void);

/* 注册 linkvisual 消息用户接口 */
void lv_event_callback_register(cx_lv_callback_cb cb);

#if defined(__cplusplus)
}
#endif
#endif // PROJECT_LINKVISUAL_DEMO_H