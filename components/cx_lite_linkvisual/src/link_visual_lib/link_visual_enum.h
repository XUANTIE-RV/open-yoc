#ifndef LINK_VISUAL_ENUM_H_
#define LINK_VISUAL_ENUM_H_

#include <unistd.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LV_STORAGE_RECORD_PLAN = 0, //计划录像
    LV_STORAGE_RECORD_ALARM = 1, //报警录像
    LV_STORAGE_RECORD_INITIATIVE = 2, // 主动录像
    LV_STORAGE_RECORD_ANY = 99,       //所有类型。
} lv_storage_record_type_e; //如果不在这个范围内，则为用户自定义录像类型，由APP和设备侧自主协商含义，SDK不再明确列出具体类型.

typedef enum {
    LV_STREAM_CMD_LIVE = 0, //直播
    /*
     * @NOTICE:
     * 点播时（LV_STREAM_CMD_STORAGE_RECORD_BY_UTC_TIME/LV_STREAM_CMD_STORAGE_RECORD_BY_FILE)，
     * 时间戳和发流速率应严格按照推荐的方式值发送，发帧速度建议：
     *   全帧倍数（<4倍）：
     *   按照实际播放速度*1.1系数来发帧，例如：视频文件帧率是25fps，每帧时间戳pts间隔差应稳定在40ms， 建议发帧速率：1/2倍为13fps、1倍为27fps、2倍为55fps。
     *
     *   抽帧倍数（>=4倍）：
     *   抽帧倍数播放只需要发送I帧，按照实际播放速度*1.1系数来发帧，例如：视频文件帧率是25fps，GOP大小为50，即I帧pts间隔差值为2S，建议发I帧速率：4倍为2.2fps、8倍为4.4fps、16倍为8.8fps。
     *
     * */
    LV_STREAM_CMD_STORAGE_RECORD_BY_UTC_TIME = 2,//按UTC时间播放设备存储录像
    LV_STREAM_CMD_PRE_EVENT_RECORD = 3,//事件录像（预录）,LV_SDK_FEATURE_CHECK_PRE_EVENT打开时才会回调
    LV_STREAM_CMD_VOICE = 4,//对讲
    LV_STREAM_CMD_CLOUD_STORAGE = 5,//云存储，目前暂不可用
    LV_STREAM_CMD_STORAGE_RECORD_BY_FILE = 6,//按文件名播放设备存储录像
    LV_STREAM_CMD_PERFORMANCE = 7,//开发者无需处理
    LV_STREAM_CMD_MAX,
} lv_stream_cmd_type_e;

typedef enum {
    LV_QUERY_RECORD_BY_DAY = 0,//查询当天的录像
    LV_QUERY_RECORD_BY_MONTH,//查询当月的录像
} lv_query_record_type_e;

/* 收到来自远程的指令 */
typedef enum {
    LV_STORAGE_RECORD_START = 0,//开始播放，对于录像点播有效
    LV_STORAGE_RECORD_PAUSE,//暂停，对于录像点播有效
    LV_STORAGE_RECORD_UNPAUSE,// 继续播放，对于录像点播有效
    LV_STORAGE_RECORD_SEEK,// 定位，对于录像点播有效
    LV_STORAGE_RECORD_STOP,//停止，对于录像点播有效
    LV_STORAGE_RECORD_SET_PARAM,//设置点播倍速等参数信息
    LV_LIVE_REQUEST_I_FRAME,//强制编码I帧，对于直播有效
    LV_VOICE_DATA,
} lv_on_push_streaming_cmd_type_e;

/* 发送给远程的指令 */
typedef enum {
    LV_STORAGE_RECORD_COMPLETE,//录像点播已经播放完成
    LV_PRE_EVENT_RECORD_COMPLETE,//事件录像的预录数据已经完成
} lv_push_stream_cmd_s;

/* 视频格式  */
typedef enum {
    // 编码类型切换后需保证首帧为I帧
    LV_VIDEO_FORMAT_H264 = 0, //AVC
    LV_VIDEO_FORMAT_H265 = 1, //HEVC
} lv_video_format_e;

/* 音频格式  */
typedef enum {
    //不支持同一设备切换音频编码类型，不支持切换编码参数
    LV_AUDIO_FORMAT_PCM = 0,//音频对讲功能不支持PCM
    LV_AUDIO_FORMAT_G711A = 1,
    LV_AUDIO_FORMAT_AAC = 2,
    LV_AUDIO_FORMAT_G711U = 3,
} lv_audio_format_e;

/* 音频采样率  */
typedef enum {
    LV_AUDIO_SAMPLE_RATE_96000 = 0,
    LV_AUDIO_SAMPLE_RATE_88200 = 1,
    LV_AUDIO_SAMPLE_RATE_64000 = 2,
    LV_AUDIO_SAMPLE_RATE_48000 = 3,
    LV_AUDIO_SAMPLE_RATE_44100 = 4,
    LV_AUDIO_SAMPLE_RATE_32000 = 5,
    LV_AUDIO_SAMPLE_RATE_24000 = 6,
    LV_AUDIO_SAMPLE_RATE_22050 = 7,
    LV_AUDIO_SAMPLE_RATE_16000 = 8,
    LV_AUDIO_SAMPLE_RATE_12000 = 9,
    LV_AUDIO_SAMPLE_RATE_11025 = 10,
    LV_AUDIO_SAMPLE_RATE_8000 = 11,
    LV_AUDIO_SAMPLE_RATE_7350 = 12,
} lv_audio_sample_rate_e;

/* 音频位宽  */
typedef enum {
    LV_AUDIO_SAMPLE_BITS_8BIT  = 0,
    LV_AUDIO_SAMPLE_BITS_16BIT = 1,
} lv_audio_sample_bits_e;

/* 音频声道  */
typedef enum {
    LV_AUDIO_CHANNEL_MONO = 0,
    LV_AUDIO_CHANNEL_STEREO = 1,
} lv_audio_channel_e;

/* 媒体封装或编码类型 */
typedef enum {
    LV_MEDIA_JPEG = 0,
    LV_MEDIA_PNG,
} lv_media_format;

/* 事件类型 */
typedef enum {
    /* 普通事件 */
    LV_EVENT_MOVEMENT = 1, //移动侦测
    LV_EVENT_SOUND = 2, //声音侦测
    LV_EVENT_HUMAN = 3,  //人形侦测
    LV_EVENT_PET = 4, //宠物侦测
    LV_EVENT_CROSS_LINE = 5, //越界侦测
    LV_EVENT_REGIONAL_INVASION = 6, //区域入侵侦测
    LV_EVENT_FALL = 7, //跌倒检测
    LV_EVENT_FACE = 8, //人脸检测
    LV_EVENT_SMILING = 9, //笑脸检测
    LV_EVENT_ABNORMAL_SOUND = 10, //异响侦测
    LV_EVENT_CRY = 11, //哭声侦测
    LV_EVENT_LAUGH = 12, //笑声侦测
    LV_EVENT_MAX
} lv_event_type_e;

/* 智能事件类型 */
typedef enum {
    LV_INTELLIGENT_EVENT_MOVING_CHECK = 1,//移动侦测
    LV_INTELLIGENT_EVENT_SOUND_CHECK = 2,//声音侦测
    LV_INTELLIGENT_EVENT_HUMAN_CHECK = 3,//人形侦测
    LV_INTELLIGENT_EVENT_PET_CHECK = 4,//宠物侦测
    LV_INTELLIGENT_EVENT_CROSS_LINE_CHECK = 5,//越界侦测
    LV_INTELLIGENT_EVENT_REGIONAL_INVASION = 6,//区域入侵侦测
    LV_INTELLIGENT_EVENT_FALL_CHECK = 7,//跌倒检测
    LV_INTELLIGENT_EVENT_FACE_CHECK = 8,//人脸检测
    LV_INTELLIGENT_EVENT_SMILING_CHECK = 9,//笑脸检测
    LV_INTELLIGENT_EVENT_ABNORMAL_SOUND_CHECK = 10,//异响侦测
    LV_INTELLIGENT_EVENT_CRY_CHECK = 11,//哭声侦测
    LV_INTELLIGENT_EVENT_LAUGH_CHECK = 12,//笑声侦测
    LV_INTELLIGENT_EVENT_ILLEGAL_PARKING = 10001,//违章停车
    LV_INTELLIGENT_EVENT_ILLEGAL_SALE = 10002,//占道经营
    LV_INTELLIGENT_EVENT_MOTORCYCLE_RECOGNITION = 10003,//摩托车识别
    LV_INTELLIGENT_EVENT_PEDESTRIAN_RECOGNITION = 10004,//行人识别
    LV_INTELLIGENT_EVENT_VEHICLES_RECOGNITION = 10005,//车辆识别
    LV_INTELLIGENT_EVENT_DELIVER_SALE = 10006,//到店经营
    LV_INTELLIGENT_EVENT_FACE_RECOGNITION = 10007,//人脸识别
    LV_INTELLIGENT_EVENT_FACE_DETECT = 10008,//人脸检测
    LV_INTELLIGENT_EVENT_PERSON_VEHICLE_DETECTION = 10009,//人车检测
    LV_INTELLIGENT_EVENT_IPC_OCCLUSION_DETECTION = 10010,//摄像头遮挡检测
    LV_INTELLIGENT_EVENT_IPC_MOVE_DETECTION = 10011,//摄像头移动检测
    LV_INTELLIGENT_EVENT_KEY_AREA_OCCUPY = 10012,//重点区域占用
    LV_INTELLIGENT_EVENT_REGIONAL_INVASION_GW = 11001,//区域入侵
    LV_INTELLIGENT_EVENT_CLIMBING_DETECT = 11002,//攀高检测
    LV_INTELLIGENT_EVENT_ARISE_DETECT = 11003,//起身检测
    LV_INTELLIGENT_EVENT_ABSENT_DETECT = 11004,//离岗检测
    LV_INTELLIGENT_EVENT_LOITERING_DETECT = 11005,//人员逗留检测
    LV_INTELLIGENT_EVENT_CROSS_LINE_DETECT = 11006,//拌线检测
    LV_INTELLIGENT_EVENT_RETROGRADE_DETECT = 11007,//逆行检测
    LV_INTELLIGENT_EVENT_QUICKLY_MOVING = 11008,//快速移动
    LV_INTELLIGENT_EVENT_GOODS_MOVED = 11009,//物品移动
    LV_INTELLIGENT_EVENT_GOODS_LEFT = 11010,//物品遗留
    LV_INTELLIGENT_EVENT_CROWD_DENSITY = 11011,//人群密度估计
    LV_INTELLIGENT_EVENT_CROWD_GATHERED = 11012,//人群聚集
    LV_INTELLIGENT_EVENT_CROWD_DISPERSED = 11013,//人群发散
    LV_INTELLIGENT_EVENT_STRENUOUS_EXERCISE = 11014,//剧烈运动
    LV_INTELLIGENT_EVENT_FALL_DETECT = 11015,//跌倒检测
    LV_INTELLIGENT_EVENT_KID_TRACK = 11016,//小孩防走失
    LV_INTELLIGENT_EVENT_MASK_DETECT = 11017,//口罩识别
    LV_INTELLIGENT_EVENT_PET_DETECT = 11018,//宠物检测
    LV_INTELLIGENT_EVENT_HUMAN_BODY_FACE_DETECT = 11022,//人体人脸检测
    LV_INTELLIGENT_EVENT_ELECTRICAL_BICYCLE_DETECT = 11023,//电瓶车识别
    LV_INTELLIGENT_EVENT_FALLING_OBJECTS_DETECT = 11027,//高空抛物检测
    LV_INTELLIGENT_EVENT_ILLEGAL_NON_MOTOR_VEHICLE_PARKING = 12001,//非机动车乱停
    LV_INTELLIGENT_EVENT_GARBAGE_EXPOSURE = 12002,//垃圾暴露
    LV_INTELLIGENT_EVENT_HANGING_ALONG_THE_STREET = 12003,//沿街晾挂
    LV_INTELLIGENT_EVENT_FIRE_DETECT = 13001,//火灾检测
    LV_INTELLIGENT_EVENT_FIRE_CHANNEL_OCCUPANCY = 13002,//消防通道占用
    LV_INTELLIGENT_EVENT_SMOKE_DETECT = 13003,//吸烟检测
    LV_INTELLIGENT_EVENT_PASSENGER_FLOW = 14001,//客流统计
} lv_intelligent_event_type_e;

/* 云端事件类型 */
typedef enum {
    LV_CLOUD_EVENT_MASK = 0,//检测口罩
    LV_CLOUD_EVENT_DOWNLOAD_FILE = 1,//下载文件
    LV_CLOUD_EVENT_UPLOAD_FILE = 2,//上传文件
    LV_CLOUD_EVENT_DELETE_FILE = 3,//删除文件
    LV_CLOUD_EVENT_ENCRYPT_KEY = 4,//秘钥
} lv_cloud_event_type_e;

/* SDK日志等级 */
typedef enum {
    LV_LOG_ERROR = 2,
    LV_LOG_WARN = 3,
    LV_LOG_INFO = 4,
    LV_LOG_DEBUG = 5,
    LV_LOG_VERBOSE = 6,
    LV_LOG_MAX = 7,
} lv_log_level_e;

/* SDK日志输出定向*/
typedef enum {
    LV_LOG_DESTINATION_FILE,//写文件，未实现；需写文件请使用 LV_LOG_DESTINATION_USER_DEFINE
    LV_LOG_DESTINATION_STDOUT,//直接向stdout输出日志
    LV_LOG_DESTINATION_USER_DEFINE,//将日志消息放入回调函数 lv_log_cb 中。可在回调函数中实现写文件功能。
    LV_LOG_DESTINATION_MAX
} lv_log_destination;

/* SDK的函数返回值枚举量 */
typedef enum {
    LV_WARN_BUF_FULL = 1,
    LV_ERROR_NONE = 0,
    LV_ERROR_DEFAULT = -1,
    LV_ERROR_ILLEGAL_INPUT = -2,
} lv_error_e;

/* 远程文件类型 */
typedef enum {
    LV_REMOTE_FILE_OTHERS = 0,
    LV_REMOTE_FILE_VOICE_RECORD = 1,
    LV_REMOTE_FILE_FACE_PICTURE = 2,
    LV_REMOTE_FILE_ALARM_IMAGE = 3,
    LV_REMOTE_FILE_VOICE_MESSAGE = 4,
    LV_REMOTE_FILE_MY_RINGTONE = 5,
} lv_remote_file_type_e ;

/* 消息适配器类型 */
typedef enum {
    LV_MESSAGE_ADAPTER_TYPE_TSL_PROPERTY = 0, //物模型属性消息，目前未使用
    LV_MESSAGE_ADAPTER_TYPE_TSL_SERVICE, //物模型服务消息
    LV_MESSAGE_ADAPTER_TYPE_LINK_VISUAL, // LinkVisual自定义消息
    LV_MESSAGE_ADAPTER_TYPE_CONNECTED, //上线信息
} lv_message_adapter_type_s;

/* SDK控制类型 */
typedef enum {
    LV_CONTROL_LOG_LEVEL = 0,
    LV_CONTROL_STREAM_AUTO_CHECK,
    LV_CONTROL_STREAM_AUTO_SAVE,
    LV_CONTROL_DEVELOP_TEST_PING,//用于验证SDK能否完成PING功能
} lv_control_type_e;

/* 流的媒体数据类型 */
typedef enum {
    LV_STREAM_MEDIA_VIDEO = 0,
    LV_STREAM_MEDIA_AUDIO,
} lv_stream_media_type_e;

/* dns的服务器类型 */
typedef enum {
    LV_DNS_SYSTEM = 0,
    LV_DNS_EXTERNAL = 1,
    LV_DNS_SYSTEM_AND_EXTERNAL = 2,
} lv_dns_mode_e;

/* 设备主动请求类型 */
typedef enum {
    LV_DEVICE_REQUEST_ENCRYPT_KEY = 0,//秘钥
} lv_device_request_type_e;

typedef enum {
    LV_ASYNC_RESULT_PICTURE_UPLOAD = 0
} lv_async_result_type_e;

/* 报警图片结果 */
typedef enum {
    LV_PICTURE_UPLOAD_SUCCESS = 0,
    LV_PICTURE_UPLOAD_FAILED = 1,
} lv_picture_upload_result_e;

#ifdef __cplusplus
}
#endif

#endif // LINK_VISUAL_ENUM_H_
