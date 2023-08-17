#ifndef LINK_VISUAL_API_H
#define LINK_VISUAL_API_H

#include "link_visual_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------——-- SDK业务相关的消息通知 ----------------------------*/
/**
* @brief SDK需要发送消息(一般指MQTT)
*
* @param [IN] lv_message_publish_param_s: 消息上报参数
*
* @retval < 0 : Fail.
* @retval  0 : Success.
*
*/
typedef int (*lv_message_publish_cb)(const lv_message_publish_param_s *message);

/**
 * @brief 通知设备开始推流
 * 
 * @param [IN] lv_device_auth_s: 设备认证信息
 * @param [IN] lv_start_push_stream_param_s: 开始推流的参数
 *
 * @retval < 0 : Fail.
 * @retval  0 : Success.
 * 
 * @notice: type == LV_STREAM_CMD_LIVE时，需要调用lv_stream_send_video()/lv_stream_send_audio()直接开始推流；
 *          type == LV_STREAM_CMD_STORAGE_RECORD_BY_FILE/LV_STREAM_CMD_STORAGE_RECORD_BY_UTC_TIME时，
 *          需要等待lv_on_push_streaming_cmd_cb()中的消息通知;当lv_on_push_streaming_cmd_cb()中通知LV_STORAGE_RECORD_START时，
 *          调用lv_stream_send_video()/lv_stream_send_audio()开始推流
 * @see lv_stream_start_service() lv_on_push_streaming_cmd_cb()
 */
typedef int (*lv_start_push_streaming_cb)(const lv_device_auth_s *auth, const lv_start_push_stream_param_s *param);

/**
 * @brief 通知设备停止推流
 * 
 * @param [IN] lv_device_auth_s: 设备认证信息
 * @param [IN] lv_stop_push_stream_param_s: 停止推流的参数
 * 
 * @retval < 0 : Fail.
 * @retval  0 : Success.
 */
typedef int (*lv_stop_push_streaming_cb)(const lv_device_auth_s *auth, const lv_stop_push_stream_param_s *param);

/**
 * @brief 推送直播/存储录像流的过程中，需要支持的命令
 * 
 * @param [IN] lv_device_auth_s: 设备认证信息
 * @param [IN] lv_on_push_streaming_cmd_param_s: 推流过程中的参数
 * 
 * @retval < 0 : Fail.
 * @retval  0 : Success.
 */
typedef int (*lv_on_push_streaming_cmd_cb)(const lv_device_auth_s *auth, const lv_on_push_stream_cmd_param_s *param);

/**
 * @brief 对接过程中，收到的数据
 *
 * @param [IN] lv_device_auth_s: 设备认证信息
 * @param [IN] lv_on_push_streaming_data_param_s: 推流过程中的参数
 *
 * @retval < 0 : Fail.
 * @retval  0 : Success.
 */
typedef int (*lv_on_push_streaming_data_cb)(const lv_device_auth_s *auth, const lv_on_push_streaming_data_param_s *param);

/**
 * @brief 查询存储录像列表
 *
 * @param [IN] lv_device_auth_s: 设备认证信息
 * @param [IN] lv_query_record_param_s: 录像查询参数
 * 
 * @return void
 */
typedef void (*lv_query_record_cb)(const lv_device_auth_s *auth, const lv_query_record_param_s *param);

/**
 * @brief 主动
 *
 * @param [IN] lv_device_auth_s: 设备认证信息
 * @param [IN] lv_query_record_param_s: 录像查询参数
 *
 * @return void
 */
typedef void (*lv_trigger_picture_cb)(const lv_device_auth_s *auth, const lv_trigger_picture_param_s *param);

/**
 * @brief 云端事件通知
 *
 * @param [IN] type: 事件类型
 * @param [IN] param: 事件附加参数
 *
 * @retval < 0 : Fail.
 * @retval  0 : Success.
 */
typedef int (*lv_cloud_event_cb)(const lv_device_auth_s *auth, const lv_cloud_event_param_s *param);

/**
 * @brief 功能强校验
 * @notice 请按照实际的功能对接情况进行返回：
 *          预录功能已经实现时，返回LV_SDK_FEATURE_CHECK_PRE_EVENT；否则返回LV_SDK_FEATURE_CHECK_CLOSE
 *
 *
 * @retval LV_FEATURE_CHECK_PRE_EVENT : 预录功能已实现.
 * @retval LV_FEATURE_CHECK_CLOSE : 预录功能未实现
 */
typedef int (*lv_feature_check_cb)(void);

/**
 * @brief 日志信息回调
 *
 * @param [IN] level  : 日志级别
 * @param [IN] file_name  : 日志的源文件名称
 * @param [IN] line  : 日志的源文件行数
 * @param [IN] fmt  : 日志打印可变参数
 *
 * @return void
 */
typedef void (*lv_log_cb)(lv_log_level_e level, const char *file_name, int line, const char *fmt, ...);

/**
 * @brief SDK异步处理结果通知
 *
 * @param auth      : 设备认证信息
 * @param result    : 异步结果
 *
 * @return void
 */
typedef void (*lv_async_result_cb)(const lv_device_auth_s *auth, const lv_async_result_data_s *result);

/*-------------------------SDK配置相关内容----------------------------*/
#define PATH_NAME_LEN       (32)

/* 配置类参数结构体 */
typedef struct {
    /* 设备类型设置:
     * 1. 单IPC设备，device_type = 0,
     * 2. NVR+IPC设备，device_type = 1；
     * NOTICE：请查看结构体 lv_device_auth_s:
     *  device_type = 0时,只接受一个dev_id = 0的设备(IPC)
     *  device_type = 1时,只接受一个dev_id = 0的设备(NVR),接受不超过sub_num个dev_id > 0的设备(IPC) */
    unsigned int device_type;
    /* 最大子设备数量配置 */
    /* device_type = 0时,sub_num不生效；device_type = 1时，取值范围：[1，128] */
    unsigned int sub_num;

    /* SDK的日志配置 */
    lv_log_level_e log_level;
    lv_log_destination log_dest;

    /* 点播源在多客户端观看时，无论是云端转发还是P2P，客户端观看的码流都是独立的，不会进行分发
     * storage_record_source_solo_num为单设备最大点播路数，
     * storage_record_source_solo_num值范围[1,8]，默认值为1
     * storage_record_source_num为支持的点播最大路数；路数越多，网络带宽、内存占用越高
     * storage_record_source_num值范围[storage_record_source_solo_num, 256]，该参数仅对NVR生效
     * */
    unsigned int storage_record_source_solo_num;
    unsigned int storage_record_source_num;

    /*
     * lv_post_intelligent_alarm/lv_post_alarm_image/lv_post_trigger_picture会对图片数据进行拷贝（如有），
     * image_size_max用于设置总的图片拷贝内存值，单位：B。
     * 若单张图片超过总内存限制，则直接丢弃图片；若单张图片超过剩余内存限制，则从旧到新丢弃图片，直至新图片可被拷贝。丢弃后的行为见具体API。
     * */
    unsigned int image_size_max;
    /*
     * lv_post_intelligent_alarm/lv_post_alarm_image/lv_post_trigger_picture会异步上传图片，
     * image_parallel用于设置图片并发上传数量，值范围[1,8]。
     * 并发越大，发送越快，内存消耗越高，带宽占用越大。WIFI设备建议为[2,3]，有线IPC建议为[2,4]，有线NVR建议为[4,8]
     * */
    unsigned int image_parallel;

    /* 码流自检查功能，能帮助开发者发现码流本身问题,调试过程中请打开。
     * 0 - 关闭， >0 - 打开 */
    unsigned int stream_auto_check;
    /* 码流数据自动保存功能，需要排查码流兼容性等问题才需要打开,在stream_auto_check打开后可使用
     * 0 - 关闭， >0 - 打开 */
    unsigned int stream_auto_save;
    /* 码流数据自动保存的路径，路径需保证存在且可写，路径名末尾需要含有"/"，如 "/tmp/" */
    char stream_auto_save_path[PATH_NAME_LEN + 1];

    /* 设备取证服务功能（Device Attestation Service, 缩写为das)，0-开启，1-关闭 */
    unsigned int das_close;

    /*
     * DNS服务器配置
     * @Notice： dns_mode的值
     *              = LV_DNS_SYSTEM:仅读取系统配置/etc/resolv.conf中的DNS服务器配置
     *              = LV_DNS_EXTERNAL:仅读取外部配置dns_num和dns_servers中的DNS服务器配置
     *              = LV_DNS_SYSTEM_AND_EXTERNAL:读取系统配置和外部配置
     *  @Notice：
     *      若未成功读取到DNS服务器配置，则使用内置的IP: 223.5.5.5/223.6.6.6/8.8.8.8
     *      总读取到的DNS配置不超过10个（系统配置优先/按读取顺序）
     *      推荐使用LV_DNS_SYSTEM_AND_EXTERNAL，除系统自动获取的DNS配置外，外部另配置数个公共、可靠的服务器；推荐总配置3个以上
     * */
    lv_dns_mode_e dns_mode;
    unsigned int dns_num;
    char **dns_servers;//字符串数组
} lv_init_config_s;

/* 回调类参数结构体 */
typedef struct {
    /* 消息通道 */
    lv_message_publish_cb message_publish_cb;

    /* SDK的日志回调（如有） */
    lv_log_cb log_cb;

    /* 音视频推流服务 */
    lv_start_push_streaming_cb start_push_streaming_cb;
    lv_stop_push_streaming_cb stop_push_streaming_cb;
    lv_on_push_streaming_cmd_cb on_push_streaming_cmd_cb;
    lv_on_push_streaming_data_cb on_push_streaming_data_cb;

    /* 存储录像查询命令 */
    lv_query_record_cb query_storage_record_cb;

    /* 主动抓图命令 */
    lv_trigger_picture_cb trigger_picture_cb;

    /* 云端事件通知 */
    lv_cloud_event_cb cloud_event_cb;

    /* 功能强校验 */
    lv_feature_check_cb feature_check_cb;

    /* 异步结果通知 */
    lv_async_result_cb async_result_cb;
} lv_init_callback_s;

/* 配置参数结构体 */
typedef struct {
    int todo;
} lv_init_system_s;

/*------------------------- SDK功能接口 ----------------------------*/
/**
 * @brief SDK初始化
 * 
 * @param [IN] config: SDK配置参数集合
 * @param [IN] callback: SDK回调参数集合
 * @param [IN] system: SDK系统参数集合
 * 
 * @return lv_error_e
 */
int lv_init(const lv_init_config_s *config, const lv_init_callback_s *callback, const lv_init_system_s *system);

/**
 * @brief SDK销毁
 * 
 * @param [IN] void
 * 
 * @return lv_error_e
 */
int lv_destroy(void);

/**
 * @brief 消息适配器，将消息注入该函数中，由SDK代为处理
 *
 * @param [IN] lv_device_auth_s: 设备认证信息
 * @param [IN] lv_message_adapter_property_s: 消息内容入参
 *
 * @return lv_error_e
 */
int lv_message_adapter(const lv_device_auth_s *auth, const lv_message_adapter_param_s *param);

/**
 * @brief 在发送实际视音频数据前发送视音频相关配置,用于直播推流和存储录像播放
 * 
 * @param [IN] service_id: 服务ID，来自回调 lv_start_push_streaming_cb
 * @param [IN] lv_stream_send_config_param_s: 配置参数集合
 * @notice 通知开始推流、强制I帧请求后，需要调用此API。
 * 
 * @return lv_error_e
 * @see lv_start_push_streaming_cb()
 */
int lv_stream_send_config(int service_id, const lv_stream_send_config_param_s *param);

/**
 * @brief 发送音视频数据
 * 
 * @param [IN] service_id: 服务ID
 * @param [IN] lv_stream_send_media_param_s： 音视频参数
 * 
 * @return lv_error_e
 */
int lv_stream_send_media(int service_id, const lv_stream_send_media_param_s *param);

/**
 * @brief 卡录像点播模式时（LV_STREAM_CMD_STORAGE_RECORD_BY_UTC_TIME），在最后一个文件播放结束后调用
 * 含预录事件录像功能时（LV_STREAM_CMD_PRE_EVENT_RECORD），在预录缓冲数据消耗完第一次切换为实时码流时调用
 *
 * @param [IN] service_id: 服务ID
 * @param [IN] lv_push_stream_cmd_s: 命令名
 *
 * @return lv_error_e
 *
 */
int lv_stream_send_cmd(int service_id, lv_push_stream_cmd_s cmd);

/**
 * @brief 智能报警事件图片上传，通过该接口上报的图片和事件是绑定在一起的
 *
 * @param [IN] param: 结构体lv_intelligent_alarm_param_s指针
 *
 * @notice:
 * 1. 调用间隔短于云端设定值，返回失败
 * 2. 附加字符串大于2048B时会被截断，但不会返回失败
 * 3. 单张图片数据最大为5MB（含），超过则返回失败
 * 4. 本接口为异步接口，存在图片数据时，SDK会进行图片数据的拷贝，拷贝受到image_size_max的限制；
 * 5. 事件也会进行拷贝，不受限制。事件和图片随后加入任务队列中
 * 6. 任务队列遵循FIFO进行处理，并发处理量为image_parallel
 * 7. 任务处理时，先上传图片。如果有图片数据，则进行图片上传，否则进入下一步。如果图片上传失败，不会进行重传，进入下一步
 * 8. 上报事件，如果事件上报失败，不会进行重传。
 * 9. 上传结果会回调至lv_async_result_cb
 *
 *
 * 异步上报的结果会反馈到lv_async_result_cb。
 *
 * @return lv_error_e
 */
int lv_post_intelligent_alarm(const lv_device_auth_s *auth, const lv_intelligent_alarm_param_s *param, int *service_id);

/**
 * @brief 报警事件图片上传或抓图上传，通过该接口上报的图片和事件是绑定在一起的
 * @notice 报警事件上传时，推荐使用lv_post_intelligent_alarm，该接口将逐渐废弃

 * @notice:
 * 1. 调用间隔短于云端设定值，返回失败
 * 2. 附加字符串大于2048B时会被截断，但不会返回失败
 * 3. 单张图片数据最大为5MB（含），超过则返回失败
 * 4. 本接口为异步接口，存在图片数据时，SDK会进行图片数据的拷贝，拷贝受到image_size_max的限制；
 * 5. 事件也会进行拷贝，不受限制。事件和图片随后加入任务队列中
 * 6. 任务队列遵循FIFO进行处理，并发处理量为image_parallel
 * 7. 任务处理时，先上传图片。如果有图片数据，则进行图片上传，否则进入下一步。如果图片上传失败，不会进行重传，进入下一步
 * 8. 上报事件，如果事件上报失败，不会进行重传。
 * 9. 上传结果会回调至lv_async_result_cb
 *
 *
 * @return lv_error_e
 * @see lv_trigger_pic_capture_cb()
 */
int lv_post_alarm_image(const lv_device_auth_s *auth, const lv_alarm_event_param_s *param, int *service_id);

/**
 * @brief 卡录像查询回复
 *
 * @param [IN] service_id: 服务ID
 * @param [IN] lv_query_record_response_s: 回复内容
 *
 * @return lv_error_e
 *
 */
int lv_post_query_record(int service_id, const lv_query_record_response_param_s *response);

/**
 * @brief 抓图回复
 *
 * @param [IN] service_id: 服务ID
 * @param [IN] lv_trigger_picture_response_param_s: 回复内容
 *
 * @notice:
 * 1. 本接口为异步接口，存在图片数据时，SDK会进行图片数据的拷贝，拷贝受到image_size_max的限制；
 * 2. 图片上传随后加入任务队列中
 * 3. 任务队列遵循FIFO进行处理，并发处理量为image_parallel
 * 4. 上传图片。如果有图片数据，则进行图片上传，否则不做操作。如果图片上传失败，不会进行重传
 *
 * @return lv_error_e
 *
 */
int lv_post_trigger_picture(int service_id, const lv_trigger_picture_response_param_s *response);


/**
 * @brief 云端事件回复
 *
 * @param [IN] service_id: 服务ID
 * @param [IN] lv_cloud_event_response_param_s: 回复内容
 *
 *
 * @notice:
 *  目前仅 LV_CLOUD_EVENT_DELETE_FILE 需要回复
 *
 * @return lv_error_e
 *
 */
int lv_post_cloud_event(int service_id, const lv_cloud_event_response_param_s *response);

/**
 * @brief 设备主动发起功能请求
 *
 * @return lv_error_e
 */
int lv_post_device_request(const lv_device_auth_s *auth, const lv_device_request_param_s *param, int *service_id);

/*
 * @brief 用于动态调节SDK的某些功能，如日志级别，方便调试。
 *
 * @param [IN] type: 功能类型,见enum lv_control_type_e.
 * @param [IN] ...: 可变长参数，参数类型与功能类型有关
 *              type:LV_CONTROL_LOG_LEVEL, 参数：int log_level;实时生效
 *              type:LV_CONTROL_STREAM_AUTO_CHECK，参数：unsigned int flag；已启动的流不会生效，新启动的流会生效
 *              type:LV_CONTROL_STREAM_AUTO_SAVE，参数：unsigned int flag，char *save_path；需要关闭时，
 *                          save_path参数不会被使用。已启动的流不会生效，新启动的流会生效
 *              type:LV_DEVELOP_TEST_PING, 参数：unsigned int times ,const char *address;
 *                  未避免此类型被滥用，SDK会限制调用次数为3次，重新初始化SDK即可重置次数
 *
 * @return lv_error_e
 */
int lv_control(lv_control_type_e type, ...);


#ifdef __cplusplus
}
#endif
#endif /* LINK_VISUAL_API_H */
