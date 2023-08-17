#ifndef LINK_VISUAL_STRUCT_H
#define LINK_VISUAL_STRUCT_H

#include "link_visual_enum.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 消息上报参数 */
typedef struct {
    char *topic;//消息主键
    char *message;//消息体
    int qos;//消息的服务质量(quality of service)
    int dev_id;
} lv_message_publish_param_s;

/* 设备认证信息 */
typedef struct {
    int dev_id;//0-主设备，>0-子设备,<0-illegal
    char *product_key;
    char *device_name;
    char *device_secret;
} lv_device_auth_s;

/* 视频参数结构体 */
typedef struct {
    lv_video_format_e format; //视频编码格式
    unsigned int fps;  //帧率
    unsigned int key_frame_interval_ms;//最大关键帧间隔时间，单位：毫秒
    unsigned int duration;//按LV_STREAM_CMD_STORAGE_RECORD_BY_FILE时的文件时长，单位s
} lv_video_param_s;

/* 音频参数结构体 */
typedef struct {
    lv_audio_format_e format;
    lv_audio_sample_rate_e sample_rate;
    lv_audio_sample_bits_e sample_bits;
    lv_audio_channel_e channel;
} lv_audio_param_s;

typedef struct {
    /*
     * bitrate_kbps为目标码率，单位为kbps;
     * SDK根据码流来预设定内部视音频缓冲区，设置过大会导致弱网下延迟增大，设置过小会导致弱网时丢帧频繁，画面卡顿
     * 内存分配大小：[0,1000) : 128KB ; [1000,2048) : 256KB ; [2048,4096) : 512KB ; [4096,6000) : 1MB ; [6144,∞) : 2MB;
     * */
    unsigned int bitrate_kbps;
    lv_video_param_s *video_param;//视频的相关参数配置
    lv_audio_param_s *audio_param;//音频的相关参数配置
} lv_stream_send_config_param_s;

typedef struct {
    struct {
        lv_stream_media_type_e type;
        unsigned int len;
        unsigned int timestamp_ms;
        char *p;
    } common;
    struct {
        lv_video_format_e format;
        int key_frame;
    } video;
    struct {
        lv_audio_format_e format;
    } audio;
} lv_stream_send_media_param_s;

/* 开始推流参数  */
typedef struct {
    struct {
        int service_id;//服务ID
        lv_stream_cmd_type_e stream_cmd_type;//推流命令类型
    } common;
    struct {
        int stream_type;//主、子码流等
    } live;
    struct {
        int pre_time;//预录事件录像的预录时间
    } pre;
    struct {
        unsigned int start_time;//播放当天0点的UTC时间,单位：s
        unsigned int stop_time;//播放当天24点的UTC时间,单位：s
        unsigned int seek_time;//播放的UTC时间相对于start_time的相对时间，即 seek_time + start_time = 播放的utc时间,单位：s
        lv_storage_record_type_e record_type;//录像类型
    } by_utc;
    struct {
        int stream_type;//主、子码流等
    } cloud;
    struct {
        int empty;//无作用的参数
    } voice;
    struct {
        unsigned int seek_time;//播放的时间相对于文件起始的相对时间
        char *file_name;//要点播的文件名
    } by_file;
} lv_start_push_stream_param_s;

/* 结束推流参数 */
typedef struct {
    int service_id;//服务ID
    lv_stream_cmd_type_e stream_cmd_type;//推流命令类型
} lv_stop_push_stream_param_s;

typedef struct {
    struct {
        int service_id;//服务ID
        lv_stream_cmd_type_e stream_cmd_type;//推流命令类型
        lv_on_push_streaming_cmd_type_e cmd_type;//命令类型
    } common;
    struct {
        unsigned int timestamp_ms;//seek的时间戳，单位：ms.
    } seek;
    struct {
        unsigned int speed;//倍速信息
        unsigned int key_only;//0-推送全数据帧，1-仅推送I帧
    } set_param;
} lv_on_push_stream_cmd_param_s;

typedef struct {
    int service_id;//服务ID
    lv_stream_cmd_type_e stream_cmd_type;//推流命令类型
    unsigned int len;
    unsigned int timestamp;
    lv_audio_param_s *audio_param;
    char *p;
} lv_on_push_streaming_data_param_s;

typedef struct {
    struct {
        int service_id;//服务ID
        lv_query_record_type_e type;//查询类型
    } common;
    struct {
        lv_storage_record_type_e type;//路线类型
        unsigned int start_time;//查询的开始时间，UTC时间，秒数
        unsigned int stop_time;//查询的结束时间，UTC时间，秒数
        unsigned int num; //录像查询的数量,等于0的时候 请求时间范围内的全部录像
    } by_day;
    struct {
        char* month;//查询的年月份，如201806
    } by_month;
} lv_query_record_param_s;

typedef struct {
    unsigned int start_time;                     // 录像开始时间，UTC时间，单位为秒
    unsigned int stop_time;                      // 录像结束时间，UTC时间，单位为秒
    unsigned int file_size;                         // 录像的文件大小，单位字节
    char *file_name;                // 录像的文件名
    lv_storage_record_type_e record_type;  //录像类型
} lv_query_record_response_day;                                //结构体数组

/* 录像查询列表结构体 */
typedef struct {
    struct {
        unsigned int num;                              //录像数量,days数组的长度
        lv_query_record_response_day *days; //
    } by_day;
    struct {
        int *months; //months为int数组，长度为31.int值为0表示无录像，非0表示有录像,当前月份不足31天部分也置为0（如2月30）
    } by_month;
} lv_query_record_response_param_s;

/* 抓图回复结构体 */
typedef struct {
    char *p;//报警事件的媒体数据,不大于1MB（大于时会返回错误），为空时仅上传其他信息
    unsigned int len;
} lv_trigger_picture_response_param_s;

/* 消息适配器内容 */
typedef struct {
    lv_message_adapter_type_s type;
    char *msg_id;
    unsigned int msg_id_len;
    char *service_name;
    unsigned int service_name_len;
    char *request;
    unsigned int request_len;
} lv_message_adapter_param_s;

typedef struct {
    int service_id;//服务ID
} lv_trigger_picture_param_s;

typedef struct {
    int service_id;//服务ID
    lv_cloud_event_type_e event_type;
    /* 口罩识别事件 */
    struct {
        void *reserved;
    } mask;
    /* 文件下载事件 */
    struct {
        lv_remote_file_type_e file_type;
        unsigned int file_size;
        char *file_name;
        char *url;
        char *md5;
    } file_download;
    struct {
        lv_remote_file_type_e file_type;
        char *file_name;
        char *url;
    } file_upload;
    struct {
        char *file_name;
    } file_delete;
    struct {
        char *encrypt_key;
    } encrypt_key;
} lv_cloud_event_param_s;

typedef struct {
    int result;//1-成功，!=1 -失败
    char *reason;//失败时的原因字符串
} lv_cloud_event_response_param_s;

/* 智能事件参数集 */
typedef struct {
    lv_intelligent_event_type_e type;//智能事件类型
    lv_media_format format;//智能事件的媒体数据类型，当前只支持图片类
    struct {
        char *p;
        unsigned int len;
    } media;//智能事件的媒体数据,不大于1MB（大于时会返回错误），为空时仅上传其他信息
    struct {
        char *p;
        unsigned int len;
    } addition_string;//智能事件的附加字符串信息，不大于2048B(大于时会截断），为空时仅上传其他信息
} lv_intelligent_alarm_param_s;

/* 报警事件参数集 */
typedef struct {
    lv_event_type_e type;
    struct {
        char *p;
        unsigned int len;
    } media; //报警事件的媒体数据,不大于1MB（大于时会返回错误），为空时仅上传其他信息
    struct {
        char *p;
        unsigned int len;
    } addition_string;//告警内容，格式为字符串；不大于1024个字节，超过会被截断；若物模型里AlarmEvent里无data字段，p传NULL即可；若有data字段但不需要使用，p则传空字符串
} lv_alarm_event_param_s;

/* 请求参数集 */
typedef struct {
    lv_device_request_type_e type;
} lv_device_request_param_s;

/* 功能强校验类型 */
typedef enum {
    LV_FEATURE_CHECK_CLOSE = 0,//被检验功能是关闭的
    /*
     * 直播预建联功能说明：
     *      在APP等准备开始观看时（例如APP进入到了设备列表页面），建立直播连接，此时仅交互少量心跳数据
     *      APP开始观看时，正式发送音视频数据；
     *      该功能能够提高首帧的速度
     * NOTICE: APP需要同时打开预建联功能；
     * NOTICE: V2.1.6开始支持该功能，若打开了预建联功能，不能降级成不支持预建联的SDK（V2.1.6之前的版本）
     * 0 - 关闭， >0 - 打开
     * */
    LV_FEATURE_CHECK_LIVE_PRE_PUBLISH = 1000,//支持功能：直播预建联功能
    LV_FEATURE_CHECK_PRE_EVENT = 10000,//支持功能：预录事件录像
} lv_feature_check_e ;

/* SDK异步处理结果通知内容 */
typedef struct {
    lv_async_result_type_e type;
    struct {
        int service_id;
        lv_picture_upload_result_e result;
        char *picture_id;
    } picture_upload;
} lv_async_result_data_s;

#ifdef __cplusplus
}
#endif

#endif //LINK_VISUAL_STRUCT_H
