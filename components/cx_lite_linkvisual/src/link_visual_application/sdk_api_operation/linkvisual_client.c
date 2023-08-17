/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include "link_visual_struct.h"
#include "link_visual_enum.h"
#include "link_visual_api.h"
#include "iot_import.h"
#include "linkkit_client.h"
#include "sdk_assistant.h"

#include "linkvisual_client.h"
#ifdef DUMMY_IPC
#include "dummy_ipc.h"
#else
#include "normal_ipc.h"
#endif // DUMMY_IPC
#include "time.h"
#include "ulog/ulog.h"
#include "cJSON.h"
#include <cx/cloud_lv_service.h>
#include "lv_internal_api.h"

#define TAG "linkvisual_client"

static void linkvisual_client_set_property_handler(const lv_device_auth_s *auth, const char *value);
static lv_set_property_handler lv_prop_cb =  linkvisual_client_set_property_handler;

// #ifdef DUMMY_IPC
typedef struct
{
    int live_main_service_id;
    int live_sub_service_id;
    int vod_utc_service_id;
    int vod_file_service_id;
    int voice_intercom_service_id;
} IpcService;
static IpcService *g_ipc_service = NULL;
static lv_device_auth_s *g_ipc_auth = NULL;
static unsigned int g_ipc_num = 0;
// #endif // DUMMY_IPC

static void query_storage_record_cb(const lv_device_auth_s *auth, const lv_query_record_param_s *param)
{
    // LOGD(TAG, "query_storage_record_cb");
#ifdef DUMMY_IPC
    dummy_ipc_report_vod_list(auth, param);
#else
    normal_ipc_report_vod_list(auth, param);
#endif // DUMMY_IPC
}

static void trigger_picture_cb(const lv_device_auth_s *auth, const lv_trigger_picture_param_s *param)
{
    lv_trigger_picture_response_param_s response = {0};
#ifdef DUMMY_IPC
    if (dummy_ipc_capture(auth, (unsigned char **)(&response.p), &(response.len)) == 0)
    {
        lv_post_trigger_picture(param->service_id, &response);
    }
#else
    if (normal_ipc_capture(auth, (unsigned char **)(&response.p), &(response.len)) == 0)
    {
        lv_post_trigger_picture(param->service_id, &response);
    }
#endif
}

static int cloud_event_cb(const lv_device_auth_s *auth, const lv_cloud_event_param_s *param)
{
    LOGD(TAG, "cloud_event_cb: %d ", param->event_type);
    if (param->event_type == LV_CLOUD_EVENT_DOWNLOAD_FILE) {
        LOGD(TAG, "cloud_event_cb %d %u %s %s %s",
               param->file_download.file_type, param->file_download.file_size, param->file_download.file_name, param->file_download.url, param->file_download.md5);
    } else if (param->event_type == LV_CLOUD_EVENT_UPLOAD_FILE) {
        LOGD(TAG, "cloud_event_cb %d %s %s", param->file_upload.file_type, param->file_upload.file_name, param->file_upload.url);
    } else if (param->event_type == LV_CLOUD_EVENT_DELETE_FILE) {
        LOGD(TAG, "cloud_event_cb %s", param->file_delete.file_name);
#if 0 //回复示例
        lv_cloud_event_response_param_s response;
        response.reason = "http request error";//自定义内容
        response.result = 0;
        lv_post_cloud_event(param->service_id, &response);
#endif
    } else if (param->event_type == LV_CLOUD_EVENT_ENCRYPT_KEY) {
        LOGD(TAG, "cloud_event_cb %s\n", param->encrypt_key.encrypt_key);
    }
    return 0;
}

static int feature_check_cb(void)
{
#if 1
    /* demo未实现预录事件录像，未启用预建联功能 */
    return LV_FEATURE_CHECK_CLOSE;
#else
    return LV_FEATURE_CHECK_LIVE_PUBLISH + LV_FEATURE_CHECK_PRE_EVENT;
#endif
}

static void async_result_cb(const lv_device_auth_s *auth, const lv_async_result_data_s *result) {
    LOGD(TAG, "async_result_cb, dev_id = %d, type = %d \n", auth->dev_id, result->type);
    if (result->type == LV_ASYNC_RESULT_PICTURE_UPLOAD) {
        LOGD(TAG, "async_result_cb, service_id = %d picture id = %s, picture result = %d \n",
            result->picture_upload.service_id, result->picture_upload.picture_id, result->picture_upload.result);
    }
}

static int on_push_streaming_cmd_cb(const lv_device_auth_s *auth, const lv_on_push_stream_cmd_param_s *param) {
    LOGD(TAG, "on_push_streaming_cmd_cb service_id:%d, stream_cmd_type:%d cmd:%d %d",
           param->common.service_id, param->common.stream_cmd_type, param->common.cmd_type, param->seek.timestamp_ms);

#ifdef DUMMY_IPC
    unsigned int i = lvDeviceAuthCompares(auth, g_ipc_num, g_ipc_auth);
    if (i >= g_ipc_num) {
        LOGE(TAG, "No ipc");
        return -1;
    }

    DummyIpcMediaParam ipc_param = {0};
    ipc_param.service_id = param->common.service_id;
    if (param->common.cmd_type == LV_LIVE_REQUEST_I_FRAME) {
        lv_video_param_s video_param;
        lv_audio_param_s audio_param;
        memset(&video_param, 0, sizeof(lv_video_param_s));
        memset(&audio_param, 0, sizeof(lv_audio_param_s));
        dummy_ipc_live_get_media_param(auth, param->common.service_id, &audio_param, &video_param);
        lv_stream_send_config_param_s config_param = {0};
        config_param.audio_param = &audio_param;
        config_param.video_param = &video_param;
        config_param.bitrate_kbps = 1000;
        lv_stream_send_config(param->common.service_id, &config_param);
        dummy_ipc_live(auth, DUMMY_IPC_MEDIA_REQUEST_I_FRAME, &ipc_param);
    } else if (param->common.cmd_type == LV_STORAGE_RECORD_SEEK) {
        ipc_param.seek_timestamp_ms = param->seek.timestamp_ms;
        if (param->common.service_id == g_ipc_service[i].vod_utc_service_id) {
            dummy_ipc_vod_by_utc(auth, DUMMY_IPC_MEDIA_SEEK, &ipc_param);
        } else {
            dummy_ipc_vod_by_file(auth, DUMMY_IPC_MEDIA_SEEK, &ipc_param);
        }
    } else if (param->common.cmd_type == LV_STORAGE_RECORD_PAUSE) {
        if (param->common.service_id == g_ipc_service[i].vod_utc_service_id) {
            dummy_ipc_vod_by_utc(auth, DUMMY_IPC_MEDIA_PAUSE, &ipc_param);
        } else {
            dummy_ipc_vod_by_file(auth, DUMMY_IPC_MEDIA_PAUSE, &ipc_param);
        }
    } else if (param->common.cmd_type == LV_STORAGE_RECORD_UNPAUSE) {
        if (param->common.service_id == g_ipc_service[i].vod_utc_service_id) {
            dummy_ipc_vod_by_utc(auth, DUMMY_IPC_MEDIA_UNPAUSE, &ipc_param);
        } else {
            dummy_ipc_vod_by_file(auth, DUMMY_IPC_MEDIA_UNPAUSE, &ipc_param);
        }
    } else if (param->common.cmd_type == LV_STORAGE_RECORD_START) {
        if (param->common.service_id == g_ipc_service[i].vod_utc_service_id) {
            dummy_ipc_vod_by_utc(auth, DUMMY_IPC_MEDIA_UNPAUSE, &ipc_param);
        } else {
            dummy_ipc_vod_by_file(auth, DUMMY_IPC_MEDIA_UNPAUSE, &ipc_param);
        }
    } else if (param->common.cmd_type == LV_STORAGE_RECORD_SET_PARAM) {
        LOGD(TAG, "on_push_streaming_cmd_cb set param: %u %u", param->set_param.key_only, param->set_param.speed);
        ipc_param.key_only = param->set_param.key_only;
        ipc_param.speed = param->set_param.speed;
        if (param->common.service_id == g_ipc_service[i].vod_utc_service_id) {
            dummy_ipc_vod_by_utc(auth, DUMMY_IPC_MEDIA_SET_PARAM, &ipc_param);
        } else {
            dummy_ipc_vod_by_file(auth, DUMMY_IPC_MEDIA_SET_PARAM, &ipc_param);
        }
    }
#else
    unsigned int i = lvDeviceAuthCompares(auth, g_ipc_num, g_ipc_auth);
    if (i >= g_ipc_num)
    {
        LOGE(TAG, "No ipc");
        return -1;
    }

    NormalIpcMediaParam ipc_param = {0};
    ipc_param.service_id = param->common.service_id;
    if (param->common.cmd_type == LV_LIVE_REQUEST_I_FRAME)
    {
        // lv_video_param_s video_param;
        // lv_audio_param_s audio_param;
        // memset(&video_param, 0, sizeof(lv_video_param_s));
        // memset(&audio_param, 0, sizeof(lv_audio_param_s));
        // normal_ipc_live_get_media_param(auth, param->common.service_id, &audio_param, &video_param);
        // lv_stream_send_config_param_s config_param = {0};
        // config_param.audio_param = &audio_param;
        // config_param.video_param = &video_param;
        // config_param.bitrate_kbps = 1000;
        // lv_stream_send_config(param->common.service_id, &config_param);
        normal_ipc_live(auth, NORMAL_IPC_MEDIA_REQUEST_I_FRAME, &ipc_param);
    }
    else if (param->common.cmd_type == LV_STORAGE_RECORD_SEEK)
    {
        ipc_param.seek_timestamp_ms = param->seek.timestamp_ms;
        if (param->common.service_id == g_ipc_service[i].vod_utc_service_id)
        {
            normal_ipc_vod_by_utc(auth, NORMAL_IPC_MEDIA_SEEK, &ipc_param);
        }
        else
        {
            normal_ipc_vod_by_file(auth, NORMAL_IPC_MEDIA_SEEK, &ipc_param);
        }
    }
    else if (param->common.cmd_type == LV_STORAGE_RECORD_PAUSE)
    {
        if (param->common.service_id == g_ipc_service[i].vod_utc_service_id)
        {
            normal_ipc_vod_by_utc(auth, NORMAL_IPC_MEDIA_PAUSE, &ipc_param);
        }
        else
        {
            normal_ipc_vod_by_file(auth, NORMAL_IPC_MEDIA_PAUSE, &ipc_param);
        }
    }
    else if (param->common.cmd_type == LV_STORAGE_RECORD_UNPAUSE)
    {
        if (param->common.service_id == g_ipc_service[i].vod_utc_service_id)
        {
            normal_ipc_vod_by_utc(auth, NORMAL_IPC_MEDIA_UNPAUSE, &ipc_param);
        }
        else
        {
            normal_ipc_vod_by_file(auth, NORMAL_IPC_MEDIA_UNPAUSE, &ipc_param);
        }
    }
    else if (param->common.cmd_type == LV_STORAGE_RECORD_START)
    {
        if (param->common.service_id == g_ipc_service[i].vod_utc_service_id)
        {
            normal_ipc_vod_by_utc(auth, NORMAL_IPC_MEDIA_UNPAUSE, &ipc_param);
        }
        else
        {
            normal_ipc_vod_by_file(auth, NORMAL_IPC_MEDIA_UNPAUSE, &ipc_param);
        }
    }
    else if (param->common.cmd_type == LV_STORAGE_RECORD_SET_PARAM)
    {
        LOGD(TAG, "on_push_streaming_cmd_cb set param: %u %u", param->set_param.key_only, param->set_param.speed);
        ipc_param.key_only = param->set_param.key_only;
        ipc_param.speed = param->set_param.speed;
        if (param->common.service_id == g_ipc_service[i].vod_utc_service_id)
        {
            normal_ipc_vod_by_utc(auth, NORMAL_IPC_MEDIA_SET_PARAM, &ipc_param);
        }
        else
        {
            normal_ipc_vod_by_file(auth, NORMAL_IPC_MEDIA_SET_PARAM, &ipc_param);
        }
    }
#endif // DUMMY_IPC

    return 0;
}

static int on_push_streaming_data_cb(const lv_device_auth_s *auth, const lv_on_push_streaming_data_param_s *param) {
    // printf("Receive voice data, param = %d %d %d %d, size = %u, timestamp = %u\n",
    //        param->audio_param->format, param->audio_param->channel, param->audio_param->sample_bits, param->audio_param->sample_rate, param->len, param->timestamp);
    // if(linkvisual_get_intercom_dir() == LV_INTERCOM_DIR_TO_CLOUD)
    //     return 0;
    linkvisual_audio_play(param);
    return 0;
}

static int start_push_streaming_cb(const lv_device_auth_s *auth, const lv_start_push_stream_param_s *param) {
    LOGD(TAG, "start_push_streaming_cb:%d %d", param->common.service_id, param->common.stream_cmd_type);

#ifdef DUMMY_IPC
    unsigned int i = lvDeviceAuthCompares(auth, g_ipc_num, g_ipc_auth);
    if (i >= g_ipc_num) {
        LOGE(TAG, "No ipc");
        return -1;
    }

    DummyIpcMediaParam ipc_param = {0};
    ipc_param.service_id = param->common.service_id;
    if (param->common.stream_cmd_type == LV_STREAM_CMD_LIVE) {
        ipc_param.stream_type = param->live.stream_type;
        dummy_ipc_live(auth, DUMMY_IPC_MEDIA_START, &ipc_param);
        if (param->live.stream_type == 0) {
            g_ipc_service[i].live_main_service_id = param->common.service_id;
        } else {
            g_ipc_service[i].live_sub_service_id = param->common.service_id;
        }
    } else if (param->common.stream_cmd_type == LV_STREAM_CMD_STORAGE_RECORD_BY_UTC_TIME) {
        printf("Record type: %d\n", param->by_utc.record_type);
        ipc_param.seek_timestamp_ms = (param->by_utc.seek_time - ipc_param.base_time_ms) * 1000;
        dummy_ipc_vod_by_utc(auth, DUMMY_IPC_MEDIA_SEEK, &ipc_param);
        g_ipc_service[i].vod_utc_service_id = param->common.service_id;
    } else if (param->common.stream_cmd_type == LV_STREAM_CMD_STORAGE_RECORD_BY_FILE) {
        printf("File name: %s, seek_time = %d\n", param->by_file.file_name, param->by_file.seek_time);
        memcpy(&ipc_param.source_file, param->by_file.file_name, strlen(param->by_file.file_name) > DummyIpcMediaFileLen ? DummyIpcMediaFileLen : strlen(param->by_file.file_name));
        ipc_param.seek_timestamp_ms = param->by_file.seek_time * 1000;
        dummy_ipc_vod_by_file(auth, DUMMY_IPC_MEDIA_SEEK, &ipc_param);
        g_ipc_service[i].vod_file_service_id = param->common.service_id;
    } else if (param->common.stream_cmd_type == LV_STREAM_CMD_VOICE) {
        g_ipc_service[i].voice_intercom_service_id = param->common.service_id;
    } else {
        printf("Stream type is not support\n");
        return -1;
    }

    lv_video_param_s video_param;
    lv_audio_param_s audio_param;
    memset(&video_param, 0, sizeof(lv_video_param_s));
    memset(&audio_param, 0, sizeof(lv_audio_param_s));
    if (param->common.stream_cmd_type == LV_STREAM_CMD_LIVE || param->common.stream_cmd_type == LV_STREAM_CMD_VOICE) {
        dummy_ipc_live_get_media_param(auth, param->common.service_id, &audio_param, &video_param);
    } else {
        dummy_ipc_vod_get_media_param(auth, &video_param, &audio_param);
    }

    lv_stream_send_config_param_s config_param = {0};
    config_param.audio_param = &audio_param;
    config_param.video_param = &video_param;
    config_param.bitrate_kbps = 1000;
    lv_stream_send_config(param->common.service_id, &config_param);
#else 
    unsigned int i = lvDeviceAuthCompares(auth, g_ipc_num, g_ipc_auth);
    if (i >= g_ipc_num)
    {
        LOGE(TAG, "No ipc");
        return -1;
    }
    
    NormalIpcMediaParam ipc_param = {0};
    ipc_param.service_id = param->common.service_id;
	
    lv_video_param_s video_param;
    lv_audio_param_s audio_param;
    memset(&video_param, 0, sizeof(lv_video_param_s));
    memset(&audio_param, 0, sizeof(lv_audio_param_s));
    if (param->common.stream_cmd_type == LV_STREAM_CMD_LIVE || param->common.stream_cmd_type == LV_STREAM_CMD_VOICE)
    {
        normal_ipc_live_get_media_param(auth, param->common.service_id, &audio_param, &video_param);
    }
    else
    {
        normal_ipc_vod_get_media_param(auth, &video_param, &audio_param);
    }

    lv_stream_send_config_param_s config_param = {0};
    config_param.audio_param = &audio_param;
    config_param.video_param = &video_param;
    config_param.bitrate_kbps = 1024;
    lv_stream_send_config(param->common.service_id, &config_param);
    if (param->common.stream_cmd_type == LV_STREAM_CMD_LIVE)
    {
        ipc_param.stream_type = param->live.stream_type;
        normal_ipc_live(auth, NORMAL_IPC_MEDIA_START, &ipc_param);
        if (param->live.stream_type == 0)
        {
            g_ipc_service[i].live_main_service_id = param->common.service_id;
        }
        else
        {
            g_ipc_service[i].live_sub_service_id = param->common.service_id;
        }
    }
    else if (param->common.stream_cmd_type == LV_STREAM_CMD_STORAGE_RECORD_BY_UTC_TIME)
    {
        LOGD(TAG, "Record type: %d", param->by_utc.record_type);
        ipc_param.seek_timestamp_ms = (param->by_utc.seek_time - ipc_param.base_time_ms) * 1000;
        normal_ipc_vod_by_utc(auth, NORMAL_IPC_MEDIA_SEEK, &ipc_param);
        g_ipc_service[i].vod_utc_service_id = param->common.service_id;
    }
    else if (param->common.stream_cmd_type == LV_STREAM_CMD_STORAGE_RECORD_BY_FILE)
    {
        LOGD(TAG, "File name: %s, seek_time = %d", param->by_file.file_name, param->by_file.seek_time);
        memcpy(&ipc_param.source_file, param->by_file.file_name, strlen(param->by_file.file_name) > NormalIpcMediaFileLen ? NormalIpcMediaFileLen : strlen(param->by_file.file_name));
        normal_ipc_vod_by_file(auth, NORMAL_IPC_MEDIA_SEEK, &ipc_param);
        g_ipc_service[i].vod_file_service_id = param->common.service_id;
    }
    else if (param->common.stream_cmd_type == LV_STREAM_CMD_VOICE)
    {
        g_ipc_service[i].voice_intercom_service_id = param->common.service_id;
    }
    else
    {
        LOGE(TAG, "Stream type is not support");
        return -1;
    }
#endif // DUMMY_IPC

    return 0;
}

static int stop_push_streaming_cb(const lv_device_auth_s *auth, const lv_stop_push_stream_param_s *param) {
    LOGD(TAG, "stop_push_streaming_cb:%d %d\n", param->service_id, param->stream_cmd_type);

#ifdef DUMMY_IPC
    unsigned int i = lvDeviceAuthCompares(auth, g_ipc_num, g_ipc_auth);
    if (i >= g_ipc_num) {
        LOGE(TAG, "No ipc");
        return -1;
    }

    DummyIpcMediaParam ipc_param = {0};
    ipc_param.service_id = param->service_id;
    if (param->service_id == g_ipc_service[i].live_main_service_id) {
        g_ipc_service[i].live_main_service_id = 0;
        dummy_ipc_live(auth, DUMMY_IPC_MEDIA_STOP, &ipc_param);
    } else if (param->service_id == g_ipc_service[i].live_sub_service_id) {
        g_ipc_service[i].live_sub_service_id = 0;
        dummy_ipc_live(auth, DUMMY_IPC_MEDIA_STOP, &ipc_param);
    } else if (param->service_id == g_ipc_service[i].vod_utc_service_id) {
        g_ipc_service[i].vod_utc_service_id = 0;
        dummy_ipc_vod_by_utc(auth, DUMMY_IPC_MEDIA_STOP, &ipc_param);
    } else if (param->service_id == g_ipc_service[i].vod_file_service_id) {
        g_ipc_service[i].vod_file_service_id = 0;
        dummy_ipc_vod_by_file(auth, DUMMY_IPC_MEDIA_STOP, &ipc_param);
    } else if (param->service_id == g_ipc_service[i].voice_intercom_service_id) {
        g_ipc_service[i].voice_intercom_service_id = 0;
    }
#else
    unsigned int i = lvDeviceAuthCompares(auth, g_ipc_num, g_ipc_auth);
    if (i >= g_ipc_num)
    {
        LOGE(TAG, "No ipc");
        return -1;
    }

    NormalIpcMediaParam ipc_param = {0};
    ipc_param.service_id = param->service_id;
    if (param->service_id == g_ipc_service[i].live_main_service_id)
    {
        g_ipc_service[i].live_main_service_id = 0;
       normal_ipc_live(auth, NORMAL_IPC_MEDIA_STOP, &ipc_param);
    }
    else if (param->service_id == g_ipc_service[i].live_sub_service_id)
    {
        g_ipc_service[i].live_sub_service_id = 0;
       normal_ipc_live(auth, NORMAL_IPC_MEDIA_STOP, &ipc_param);
    }
    else if (param->service_id == g_ipc_service[i].vod_utc_service_id)
    {
        g_ipc_service[i].vod_utc_service_id = 0;
       normal_ipc_vod_by_utc(auth, NORMAL_IPC_MEDIA_STOP, &ipc_param);
    }
    else if (param->service_id == g_ipc_service[i].vod_file_service_id)
    {
        g_ipc_service[i].vod_file_service_id = 0;
       normal_ipc_vod_by_file(auth, NORMAL_IPC_MEDIA_STOP, &ipc_param);
    }
    else if (param->service_id == g_ipc_service[i].voice_intercom_service_id)
    {
        g_ipc_service[i].voice_intercom_service_id = 0;
    }
#endif // DUMMY_IPC

    return 0;
}

#define kTimevalLen 32
#define kTimeLen 64
static int g_logger_level = LV_LOG_ERROR;
static const char* LOGGER_LEVEL_NAME[] = {"LV-E", "LV-W", "LV-I", "LV-D", "LV-V"};

static void log_print(lv_log_level_e level, const char *file_name, int line, const char *fmt, ...)
{
    if (level < LV_LOG_ERROR || level >= LV_LOG_MAX) {
        return;
    }
    if (level > g_logger_level) {
        return;
    }

    //notice:连续调用time和gettimeofday，在跨秒的时候时间戳可能会有问题，仅用于日志打印
    // char date[kTimevalLen] = {0};
    // // struct tm *tm = cx_util_localtime();
    // long long ms = aos_now_ms();
    // snprintf(date, kTimevalLen, "%4d.%03d", (int)(ms / 1000), (int)(ms % 1000));
    // printf("\r\n[%s|%s]", date, LOGGER_LEVEL_NAME[level - LV_LOG_ERROR]);
    // va_list args;
    // va_start(args, fmt);
    // vprintf(fmt, args);
    // va_end(args);
    // printf("\r\n");

    char *tmp_buff = malloc(2048);
    va_list args;
    va_start(args, fmt);
    vsnprintf(tmp_buff, 2048, fmt, args);
    va_end(args);
    LOGD(LOGGER_LEVEL_NAME[level - LV_LOG_ERROR], "%s", tmp_buff);
    free(tmp_buff);
}

/**
 * linkvisual_demo_init负责LinkVisual相关回调的注册。
 * 1. 开发者需要注册相关认证信息和回调函数，并进行初始化
 * 2. 根据回调函数中的信息，完成音视频流的上传
 * 3. 当前文件主要用于打印回调函数的命令，并模拟IPC的行为进行了音视频的传输
 */
static int g_init = 0;
int linkvisual_client_init(unsigned int device_type, int sub_num, lv_log_level_e log_level)
{
    LOGD(TAG, "before init linkvisual");

    if (g_init)
    {
        LOGE(TAG, "linkvisual_demo_init already init");
        return 0;
    }

    lv_init_config_s config;
    memset(&config, 0, sizeof(lv_init_config_s));
    lv_init_callback_s callback;
    memset(&callback, 0, sizeof(lv_init_callback_s));
    lv_init_system_s system;
    memset(&system, 0, sizeof(lv_init_system_s));

    /* SDK的类型配置 */
    config.device_type = device_type;
    config.sub_num = sub_num;

    /* SDK的日志配置 */
    config.log_level = log_level;
    config.log_dest = LV_LOG_DESTINATION_USER_DEFINE;
    g_logger_level = log_level;

    /* 码流路数限制 */
    config.storage_record_source_solo_num = 1;
    config.storage_record_source_num = 4; //该参数仅对NVR有效

    /* 图片性能控制 */
    config.image_size_max = 1024 * 1024; //内存申请不超过1M
    config.image_parallel = 2;           //2路并发

    /* 码流检查功能 */
    config.stream_auto_check = 1;
#if 1 /* 码流保存为文件功能，按需使用 */
    config.stream_auto_save = 0;
#else
    config.stream_auto_save = 1;
    char *path = "/tmp/";
    memcpy(config.stream_auto_save_path, path, strlen(path));
#endif

    /* das默认开启 */
    config.das_close = 0;


#if 1
    config.dns_mode = LV_DNS_SYSTEM;
#else
    /*
     * DNS配置默认使用系统内置和外部设置的
    */
    config.dns_mode = LV_DNS_SYSTEM_AND_EXTERNAL;
    config.dns_num = 3;
    const char *dns_ip_1 = "223.5.5.5";
    const char *dns_ip_2 = "223.6.6.6";
    const char *dns_ip_3 = "8.8.8.8";
    char *dns_servers[] = {dns_ip_1, dns_ip_2, dns_ip_3};
    config.dns_servers = dns_servers;
#endif

    callback.message_publish_cb = linkkit_message_publish_cb;

    //音视频推流服务
    callback.start_push_streaming_cb = start_push_streaming_cb;
    callback.stop_push_streaming_cb = stop_push_streaming_cb;
    callback.on_push_streaming_cmd_cb = on_push_streaming_cmd_cb;
    callback.on_push_streaming_data_cb = on_push_streaming_data_cb;
    callback.log_cb = log_print;

    //获取存储录像录像列表
    callback.query_storage_record_cb = query_storage_record_cb;

    callback.trigger_picture_cb = trigger_picture_cb;

    /* 云端事件通知 */
    callback.cloud_event_cb = cloud_event_cb;

    callback.feature_check_cb = feature_check_cb;

    callback.async_result_cb = async_result_cb;

    //先准备好LinkVisual相关资源
    int ret = lv_init(&config, &callback, &system);
    if (ret < 0) {
        LOGE(TAG, "lv_init failed, result = %d", ret);
        return -1;
    }
#if 0
    //测试SDK能否正常使用PING功能
    lv_control(LV_CONTROL_DEVELOP_TEST_PING, 3, "baidu.com");
#endif

    g_init = 1;

     LOGD(TAG, "after init linkvisual");

    return 0;
}

void linkvisual_client_destroy()
{
    LOGD(TAG, "before destroy linkvisual");

    if (!g_init) {
        LOGE(TAG, "linkvisual_demo_destroy is not init");
        return;
    }

    lv_destroy();

    g_init = 0;

    LOGD(TAG, "after destroy linkvisual");
}

int linkvisual_client_video_handler(int service_id, lv_video_format_e format, unsigned char *buffer, unsigned int buffer_size,
                                     unsigned int present_time, int nal_type) {
    //printf("video service_id:%d, format:%d, present_time:%u nal_type:%d size %u\n", service_id, format, present_time, nal_type, buffer_size);
    lv_stream_send_media_param_s param = {{0}};
    param.common.type = LV_STREAM_MEDIA_VIDEO;
    param.common.p = (char *)buffer;
    param.common.len = buffer_size;
    param.common.timestamp_ms = present_time;
    param.video.format = format;
    param.video.key_frame = nal_type;
    return lv_stream_send_media(service_id, &param);
}

/* 音频帧数据回调 */
int linkvisual_client_audio_handler(int service_id,
                                     lv_audio_format_e format,
                                     unsigned char *buffer,
                                     unsigned int buffer_size,
                                     unsigned int present_time)
{
    //LOGD(TAG, "audio service_id:%d, present_time:%u buffer_size:%u", service_id, present_time, buffer_size);
    int ret;
    lv_stream_send_media_param_s param = {{0}};
    param.common.type = LV_STREAM_MEDIA_AUDIO;
    param.common.p = (char *)buffer;
    param.common.len = buffer_size;
    param.common.timestamp_ms = present_time;
    param.audio.format = format;

    for (unsigned int i = 0; i < g_ipc_num; i++)
    { //对讲开启时，多发送一份对讲的音频
        if (g_ipc_service[i].live_main_service_id == service_id || g_ipc_service[i].live_sub_service_id == service_id)
        {
            if (g_ipc_service[i].voice_intercom_service_id > 0)
            {
#if !CONFIG_INTERCOM_FULL_DUPLEX
                if(linkvisual_get_intercom_dir() == LV_INTERCOM_DIR_FROM_CLOUD) {
                    memset(buffer, 0, buffer_size);
                }
#endif
                lv_stream_send_media(g_ipc_service[i].voice_intercom_service_id, &param);
            }
        }
    }
    ret = lv_stream_send_media(service_id, &param);
    return ret;
}

int linkvisual_snapshot_upload(unsigned char *buff, int len, lv_media_format type)
{
    if(type < LV_MEDIA_JPEG || type > LV_MEDIA_PNG) {
        LOGE("%s unknown type", __func__);
    }
    lv_device_auth_s auth;
    linkkit_get_auth(0, &auth, NULL);
    linkvisual_client_picture_handler(&auth, buff, len);
    return 0;
}

/* 报警图片数据回调 */
void linkvisual_client_picture_handler(const lv_device_auth_s *auth, unsigned char *buffer, unsigned int buffer_size)
{
    static char *data = "test";
#if 0
    lv_alarm_event_param_s param_1;
    int service_id_1;
    memset(&param_1, 0, sizeof(lv_alarm_event_param_s));
    param_1.type = LV_EVENT_SOUND;
    param_1.media.p = (char *)buffer;
    param_1.media.len = buffer_size;
    param_1.addition_string.p = data;
    param_1.addition_string.len = strlen(data);
    lv_post_alarm_image(auth, &param_1, &service_id_1);
    printf("lv_post_alarm_image, service id = %d\n", service_id_1);
#else
    lv_intelligent_alarm_param_s param_2;
    int service_id_2;
    memset(&param_2, 0, sizeof(lv_intelligent_alarm_param_s));
    param_2.type = LV_INTELLIGENT_EVENT_MOVING_CHECK;
    param_2.media.p = (char *)buffer;
    param_2.media.len = buffer_size;
    param_2.addition_string.p = data;
    param_2.addition_string.len = strlen(data);
    param_2.format = LV_MEDIA_JPEG; //该字段目前无效
    lv_post_intelligent_alarm(auth, &param_2, &service_id_2);
    LOGD(TAG, "lv_post_intelligent_alarm, service id = %d", service_id_2);
#endif
}

int linkvisual_client_vod_handler(int service_id, int vod_cmd, int param) {
    //目前只定义了点播完成的回调
    lv_stream_send_cmd(service_id, LV_STORAGE_RECORD_COMPLETE);
    return 0;
}

void linkvisual_client_query_record(int service_id, const lv_query_record_response_param_s *param)
{
    lv_post_query_record(service_id, param);
}

/* 属性设置回调 */
static void linkvisual_client_set_property_handler(const lv_device_auth_s *auth, const char *value) 
{
    cJSON *root = cJSON_Parse(value);
    cJSON *item = cJSON_GetObjectItem(root, "StreamVideoQuality");
    int quality = item->valueint;
    linkvisual_set_live_clarity(quality);
    IOT_Linkkit_Report(auth->dev_id, ITM_MSG_POST_PROPERTY, (unsigned char *)value, strlen(value));
}

void linkvisual_client_control_test()
{
    /* 一个十分简易的命令行解析程序，请按照示例命令来使用 */
#define CMD_LINE_MAX (128)
    char str[CMD_LINE_MAX] = {0};
    gets(str);

    char *key = strtok(str, " ");
    if (!key) {
        return;
    }
    char *value = strtok(NULL, " ");
    if (!value) {
        return;
    }

    if (!strcmp(key, "-l")) {// 日志级别设置，使用示例： -l 3
        int log_level = value[0] - '0';
        lv_control(LV_CONTROL_LOG_LEVEL, log_level);
    } else if (!strcmp(key, "-c")) {// 码流自检功能，使用示例： -c 0
        int check = value[0] - '0';
        lv_control(LV_CONTROL_STREAM_AUTO_CHECK, check);
    } else if (!strcmp(key, "-s")) {// 码流自动功能，使用示例： -s 0
        int save = value[0] - '0';
        const char *path = "/tmp/";// 需要打开时，使用默认的保存路径
        lv_control(LV_CONTROL_STREAM_AUTO_SAVE, save, path);
    } else {
        return;
    }
}

int linkvisual_client_assistant_start(unsigned int device_type, const lv_device_auth_s *main, unsigned int sub_num, const lv_device_auth_s *sub) {
// #ifdef DUMMY_IPC
    if (device_type == 0) {//ipc单设备
        g_ipc_service = (IpcService *)malloc(sizeof(IpcService));
        if (!g_ipc_service) {
            return -1;
        }
        memset(g_ipc_service, 0, sizeof(IpcService));
        g_ipc_auth = lvDeviceAuthCopy(main, 1);
        if (!g_ipc_auth) {
            return -1;
        }
        g_ipc_num = 1;
    } else {//主设备NVR+子设备IPC
        if (sub_num && sub) {
            g_ipc_service = (IpcService *)malloc(sizeof(IpcService) * sub_num);
            if (!g_ipc_service) {
                return -1;
            }
            memset(g_ipc_service, 0, sizeof(IpcService) * sub_num);
            g_ipc_auth = lvDeviceAuthCopy(sub, sub_num);
            if (!g_ipc_auth) {
                return -1;
            }
            g_ipc_num = sub_num;
        }
    }
// #endif
    return 0;
}

void linkvisual_client_assistant_stop() {
// #ifdef DUMMY_IPC
    lvDeviceAuthDelete(g_ipc_auth, g_ipc_num);
    g_ipc_auth = NULL;
    g_ipc_num = 0;
    if (g_ipc_service) {
        free(g_ipc_service);
    }
// #endif
}

void iot_register_set_property_callback(lv_set_property_handler cb)
{
    lv_prop_cb = cb;
}

lv_set_property_handler linkvisual_get_property_handler(void)
{
    return lv_prop_cb;
}

cx_lv_callback_cb lv_event_cb = NULL;
void lv_event_callback_register(cx_lv_callback_cb cb)
{
    lv_event_cb = cb;
}
