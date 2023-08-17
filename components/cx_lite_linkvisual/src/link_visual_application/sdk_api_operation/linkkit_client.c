/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "linkkit_client.h"

#include <stdio.h>

#include "link_visual_api.h"
#include "link_visual_enum.h"
#include "exports/iot_export_linkkit.h"
#include "iot_import.h"
#include "cJSON.h"
#include "sdk_assistant.h"
#include "ulog/ulog.h"
#ifdef DUMMY_IPC
#include "dummy_ipc.h"
#else
#include "normal_ipc.h"
#endif


#define TAG "linkkit_client"

static iot_msg_cb iot_msg_callback = NULL;

static int g_master_dev_id = -1;
static int g_running = 0;
static void *g_thread = NULL;
static int g_thread_not_quit = 0;
static int g_connect = 0;
static int g_property[128] = {0};


static void *g_sub_thread = NULL;
static int g_sub_thread_not_quit = 0;
static void *g_sem = NULL;
static unsigned int g_sub_num = 0;//保存子设备的信息
static iotx_linkkit_dev_meta_info_t *g_sub = NULL;

static iotx_linkkit_dev_meta_info_t *g_master_dev_info = NULL;

/* 这个字符串数组用于说明LinkVisual需要处理的物模型服务 */
static char *link_visual_service[] = {
    "TriggerPicCapture",//触发设备抓图
    "StartVoiceIntercom",//开始语音对讲
    "StopVoiceIntercom",//停止语音对讲
    "StartVod",//开始录像观看
    "StartVodByTime",//开始录像按时间观看
    "StopVod",//停止录像观看
    "QueryRecordTimeList",//查询录像列表
    "QueryRecordList",//查询录像列表
    "StartP2PStreaming",//开始P2P直播
    "StartPushStreaming",//开始直播
    "StopPushStreaming",//停止直播
    "QueryMonthRecord",//按月查询卡录像列表
};


/* 以下两个extern的函数在linkkit或者ali-smartliving中定义 */
extern int iotx_dm_get_triple_by_devid(int devid, char **product_key, char **device_name, char **device_secret);
void linkkit_get_auth(int dev_id, lv_device_auth_s *auth, iotx_linkkit_dev_meta_info_t *sub_dev)
{
    if (dev_id > 0) {
        /* 此处请注意：需要使用devid准确查询出实际的三元组 */
        char *product_key = NULL;
        char *device_name = NULL;
        char *device_secret= NULL;
        iotx_dm_get_triple_by_devid(dev_id, &product_key, &device_name, &device_secret);
        auth->dev_id = dev_id;
        auth->product_key = product_key;
        auth->device_name = device_name;
        auth->device_secret = device_secret;
    } else if (dev_id == 0) {
        /* Notice:这里本应该也使用iotx_dm_get_triple_by_devid进行查询，方便代码统一。
         * 但此函数查询devid=0时，会丢失掉device_secret数据(有bug)
         * 此处改为直接使用主设备的三元组信息，不进行查询 */
        auth->dev_id = dev_id;
        auth->product_key = g_master_dev_info->product_key;
        auth->device_name = g_master_dev_info->device_name;
#ifdef LINKKIT_DYNAMIC_REGISTER
        HAL_GetDeviceSecret(g_master_dev_info->device_secret);
#endif
        auth->device_secret = g_master_dev_info->device_secret;
    }
}


static void *sub_online(void *args) {
    while (g_running) {
        HAL_SemaphoreWait(g_sem, PLATFORM_WAIT_INFINITE);//无限等待
        if (!g_running) {//方便退出
            break;
        }

        LOGD(TAG, "Start login sub dev");
        for (unsigned int i = 0; i < g_sub_num; i++) {
            iotx_linkkit_dev_meta_info_t meta;
            memset(&meta, 0, sizeof(iotx_linkkit_dev_meta_info_t));
            string_safe_copy(meta.product_key, g_sub[i].product_key, PRODUCT_KEY_MAXLEN - 1);
            string_safe_copy(meta.device_name, g_sub[i].device_name, DEVICE_NAME_MAXLEN - 1);
            string_safe_copy(meta.device_secret, g_sub[i].device_secret, DEVICE_SECRET_MAXLEN - 1);
            string_safe_copy(meta.product_secret, g_sub[i].product_secret, PRODUCT_SECRET_MAXLEN - 1);
            int devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_SLAVE, &meta);
            if (devid == FAIL_RETURN) {
                LOGE(TAG, "subdev open Failed");
                break;
            }

            int res = IOT_Linkkit_Connect(devid);
            if (res == FAIL_RETURN) {
                LOGE(TAG, "subdev connect Failed");
                break;
            }

            res = IOT_Linkkit_Report(devid, ITM_MSG_LOGIN, NULL, 0);
            if (res == FAIL_RETURN) {
                LOGE(TAG, "subdev login Failed");
                break;
            }

            // 子设备上线消息同步给LinkVisual
            lv_device_auth_s auth;
            iotx_linkkit_dev_meta_info_t sub_dev = {0};
            linkkit_get_auth(devid, &auth, &sub_dev);
            lv_message_adapter_param_s in = {0};
            in.type = LV_MESSAGE_ADAPTER_TYPE_CONNECTED;
            lv_message_adapter(&auth, &in);

            //所有IPC子设备需要上线后上报IPC的属性
            //每次SDK启动只需要上报一次，SDK因网络等情况离线不需要重复上报。
            if (!g_property[devid]) {//demo未做严格的越界处理，开发者可以根据实际需要来改动。
                g_property[devid] = 1;
#ifdef DUMMY_IPC
            dummy_ipc_get_all_property(&auth);
#else
            normal_ipc_get_all_property(&auth);
#endif
            }
        }
    }

    g_sub_thread_not_quit = 0;
    return NULL;
}

static int user_connected_event_handler(void) {
    LOGD(TAG, "Cloud Connected");
    g_connect = 1;

    /**
     * Linkkit连接后，上报设备的属性值。
     * 当APP查询设备属性时，会直接从云端获取到设备上报的属性值，而不会下发查询指令。
     * 对于设备自身会变化的属性值（存储使用量等），设备可以主动隔一段时间进行上报。
     */


    lv_device_auth_s auth;
    iotx_linkkit_dev_meta_info_t sub_dev = {0};
    linkkit_get_auth(0, &auth, &sub_dev);//这个回调只有主设备才会进入

    /* IPC开机获取和上报所有属性值；子设备不存在时，说明是单IPC，上报属性；子设备存在时，说明是NVR，NVR没有IPC的属性，不需要上报 */
    if (g_sub_num == 0) {
        //物模型属性建议批量上报，可以降低上报的数据条数。批量数据格式举例：{"AlarmFrequencyLevel":0,"AlarmSwitch":0}
        //每次SDK启动只需要上报一次，SDK因网络等情况离线不需要重复上报
        if (!g_property[0]) {
            g_property[0] = 1;
#ifdef DUMMY_IPC
            dummy_ipc_get_all_property(&auth);
#else
            normal_ipc_get_all_property(&auth);
#endif // DUMMY_IPC
        }
    }

    /* Linkkit连接后，查询下ntp服务器的时间戳，用于同步服务器时间。查询结果在user_timestamp_reply_handler中 */
    IOT_Linkkit_Query(g_master_dev_id, ITM_MSG_QUERY_TIMESTAMP, NULL, 0);

    // linkkit上线消息同步给LinkVisual
    lv_message_adapter_param_s in = {0};
    in.type = LV_MESSAGE_ADAPTER_TYPE_CONNECTED;
    lv_message_adapter(&auth, &in);

    /* 含子设备时，主设备上线后，逐个上线子设备;当网络等原因导致主设备反复上下线时，也需要重新上线子设备 */
    if (g_sub_num && g_sub) {
        HAL_SemaphorePost(g_sem);
    }
    return 0;
}

static int user_disconnected_event_handler(void) {
    LOGD(TAG, "Cloud Disconnected");
    g_connect = 0;
    return 0;
}

static int user_service_request_handler(const int devid, const char *id, const int id_len,
                                            const char *serviceid, const int serviceid_len,
                                            const char *request, const int request_len,
                                            char **response, int *response_len) {
    LOGD(TAG, "Service Request Received, Devid: %d, ID %.*s, Service ID: %.*s, Payload: %s",
             devid, id_len, id, serviceid_len, serviceid, request);

    /* 部分物模型服务消息由LinkVisual处理，部分需要自行处理。 */
    int link_visual_process = 0;
    for (unsigned int i = 0; i < sizeof(link_visual_service)/sizeof(link_visual_service[0]); i++) {
        /* 这里需要根据字符串的长度来判断 */
        if (!strncmp(serviceid, link_visual_service[i], strlen(link_visual_service[i]))) {
            link_visual_process = 1;
            break;
        }
    }

    if (link_visual_process) {
        /* ISV将某些服务类消息交由LinkVisual来处理，不需要处理response */
        /* 此处请注意：需要使用devid准确查询出实际的三元组 */
        lv_device_auth_s auth;
        iotx_linkkit_dev_meta_info_t sub_dev = {0};
        linkkit_get_auth(devid, &auth, &sub_dev);

        lv_message_adapter_param_s in = {0};
        in.type = LV_MESSAGE_ADAPTER_TYPE_TSL_SERVICE;
        in.msg_id = (char *)id;
        in.msg_id_len = id_len;
        in.service_name = (char *)serviceid;
        in.service_name_len = serviceid_len;
        in.request = (char *)request;
        in.request_len = request_len;
        int ret = lv_message_adapter(&auth, &in);
        if (ret < 0) {
            LOGE(TAG, "LinkVisual process service request failed, ret = %d", ret);
            return -1;
        }
    } else {
        /* 非LinkVisual处理的消息示例 */
        if (!strncmp(serviceid, "PTZActionControl", (serviceid_len > 0)?serviceid_len:0)) {
            cJSON *root = cJSON_Parse(request);
            if (root == NULL) {
                LOGE(TAG, "JSON Parse Error");
                return -1;
            }

            cJSON *child = cJSON_GetObjectItem(root, "ActionType");
            if (!child) {
                LOGE(TAG, "JSON Parse Error");
                cJSON_Delete(root);
                return -1;
            }
            int action_type = child->valueint;

            child = cJSON_GetObjectItem(root, "Step");
            if (!child) {
                LOGE(TAG, "JSON Parse Error");
                cJSON_Delete(root);
                return -1;
            }
            int step = child->valueint;

            cJSON_Delete(root);
            LOGD(TAG, "PTZActionControl %d %d", action_type, step);
        }
    }

    return 0;
}

static int user_property_set_handler(const int devid, const char *request, const int request_len) {
    LOGD(TAG, "Property Set Received, Devid: %d, Request: %s\n", devid, request);

    /* 此处请注意：需要使用devid准确查询出实际的三元组 */
    lv_device_auth_s auth;
    iotx_linkkit_dev_meta_info_t sub_dev = {0};
    linkkit_get_auth(devid, &auth, &sub_dev);

    cJSON *root = cJSON_Parse(request);
    if (!root) {
        LOGE(TAG, "value parse error");
        return -1;
    }
    cJSON *child = root->child;

    /* 设置属性 */
#ifdef DUMMY_IPC
    if (child->type == cJSON_Number) {
        char value[32] = {0};
        snprintf(value, 32, "%d", child->valueint);
        dummy_ipc_set_property(&auth, child->string, value);
    } else if (child->type == cJSON_String) {
        dummy_ipc_set_property(&auth, child->string, child->valuestring);
    }
#else
    if (child->type == cJSON_Number) {
        char value[32] = {0};
        snprintf(value, 32, "%d", child->valueint);
        normal_ipc_set_property(&auth, child->string, value);
    } else if (child->type == cJSON_String) {
        normal_ipc_set_property(&auth, child->string, child->valuestring);
    }
#endif // DUMMY_IPC
    cJSON_Delete(root);

    return 0;
}

static int user_timestamp_reply_handler(const char *timestamp) {
    LOGD(TAG, "Current Timestamp: %s ", timestamp);//时间戳为字符串格式，单位：毫秒

    return 0;
}

static int user_fota_handler(int type, const char *version) {
    char buffer[1024] = {0};
    int buffer_length = 1024;

    if (type == 0) {
        LOGD(TAG, "New Firmware Version: %s", version);

        IOT_Linkkit_Query(g_master_dev_id, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}

static int user_event_notify_event_handler(const int devid, const char *request, const int request_len) {
    LOGD(TAG, "Event Notify Received, Devid: %d, Request: %s ", devid, request);
    return 0;
}

static int user_link_visual_handler(const int devid, const char *service_id,
                                    const int service_id_len, const char *payload,
                                    const int payload_len) {
    /* Linkvisual自定义的消息，直接全交由LinkVisual来处理 */
    if (payload == NULL || payload_len == 0) {
        return 0;
    }

    /* 此处请注意：需要使用devid准确查询出实际的三元组 */
    lv_device_auth_s auth;
    iotx_linkkit_dev_meta_info_t sub_dev = {0};
    linkkit_get_auth(devid, &auth, &sub_dev);

    lv_message_adapter_param_s in = {0};
    in.type = LV_MESSAGE_ADAPTER_TYPE_LINK_VISUAL;
    in.service_name = (char *)service_id;
    in.service_name_len = service_id_len;
    in.request = (char *)payload;
    in.request_len = payload_len;
    int ret = lv_message_adapter(&auth, &in);
    if (ret < 0) {
        LOGE(TAG, "LinkVisual process service request failed, ret = %d", ret);
        return -1;
    }

    return 0;
}

static void *user_dispatch_yield(void *args) {
    while (g_running) {
        IOT_Linkkit_Yield(10);
    }

    g_thread_not_quit = 0;
    return NULL;
}

int linkkit_client_start(const iotx_linkkit_dev_meta_info_t *main,
                         unsigned int sub_num, const iotx_linkkit_dev_meta_info_t *sub) {
    LOGD(TAG, "Before start linkkit");

    if (g_running) {
        LOGD(TAG, "Already running");
        return 0;
    }
    g_running = 1;

    if (!main) {
        LOGE(TAG, "Illegal input");
        return -1;
    }

    //存储子设备信息，异步使用
    if (sub_num && sub) {
        g_sub_num = sub_num;
        g_sub = (iotx_linkkit_dev_meta_info_t *)malloc(sizeof(iotx_linkkit_dev_meta_info_t) *sub_num);
        if (!g_sub) {
            return -1;
        }
        memcpy(g_sub, sub, sizeof(iotx_linkkit_dev_meta_info_t) * sub_num);
        g_sem = HAL_SemaphoreCreate();//信号量用于线程上线子设备，也可以条件变量、定时器实现等
        if (!g_sem) {
            return -1;
        }
        g_sub_thread_not_quit = 1;
        if (HAL_ThreadCreate(&g_sub_thread, sub_online, NULL, NULL, NULL) != 0) {
            LOGE(TAG, "HAL_ThreadCreate Failed");
            g_thread_not_quit = 0;
            return -1;
        }
    }

    /* 设置调试的日志级别 */
    IOT_SetLogLevel(IOT_LOG_WARNING);

#ifdef SMARTLIVING
    aiot_kv_init();
#endif

    /* 注册链接状态的回调 */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);

    /* 注册消息通知 */
    IOT_RegisterCallback(ITE_LINK_VISUAL, user_link_visual_handler);//linkvisual自定义消息
    IOT_RegisterCallback(ITE_SERVICE_REQUST, user_service_request_handler);//物模型服务类消息
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_handler);//物模型属性设置
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_handler);//NTP时间
    IOT_RegisterCallback(ITE_FOTA, user_fota_handler);//固件OTA升级事件
    IOT_RegisterCallback(ITE_EVENT_NOTIFY, user_event_notify_event_handler);

    if (!g_master_dev_info) {
        g_master_dev_info = (iotx_linkkit_dev_meta_info_t *)malloc(sizeof(iotx_linkkit_dev_meta_info_t));
        if (!g_master_dev_info) {
            LOGE(TAG, "Malloc failed ");
            return -1;
        }
        memset(g_master_dev_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    }
    string_safe_copy(g_master_dev_info->product_key, main->product_key, PRODUCT_KEY_MAXLEN - 1);
    string_safe_copy(g_master_dev_info->device_name, main->device_name, DEVICE_NAME_MAXLEN - 1);
    string_safe_copy(g_master_dev_info->device_secret, main->device_secret, DEVICE_SECRET_MAXLEN - 1);
    string_safe_copy(g_master_dev_info->product_secret, main->product_secret, PRODUCT_SECRET_MAXLEN - 1);
    /* 选择服务器地址，当前使用上海服务器 */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* 动态注册 */
#ifdef LINKKIT_DYNAMIC_REGISTER
    int dynamic_register = 1;
#else
    int dynamic_register = 0;
#endif
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

#ifdef LINKKIT_INSTANCE
    int bootstrap_enabled = 1;
    IOT_Ioctl(IOTX_IOCTL_SET_BOOTSTRAP_ENABLED, (void *)&bootstrap_enabled);
#endif

    /* 创建linkkit资源 */
    g_master_dev_id = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, g_master_dev_info);
    if (g_master_dev_id < 0) {
        LOGE(TAG, "IOT_Linkkit_Open Failed");
        return -1;
    }

    /* 连接到服务器 */
    int ret = IOT_Linkkit_Connect(g_master_dev_id);
    if (ret < 0) {
        LOGE(TAG, "IOT_Linkkit_Connect Failed");
        return -1;
    }

    /* 创建线程，线程用于轮训消息 */
    g_thread_not_quit = 1;
    ret = HAL_ThreadCreate(&g_thread, user_dispatch_yield, NULL, NULL, NULL);
    if (ret != 0) {//!= 0 而非 < 0
        LOGE(TAG, "HAL_ThreadCreate Failed, ret = %d", ret);
        g_running = 0;
        g_thread_not_quit = 0;
        IOT_Linkkit_Close(g_master_dev_id);
        return -1;
    }

    /* 等待主设备链接成功（demo做了有限时长的等待，实际产品中，可设置为在网络可用时一直等待） */
    for(int i = 0; i < 100; i++) {
        if(!g_connect) {
            HAL_SleepMs(200);
        } else {
            break;
        }
    }

    if (!g_connect) {
        LOGE(TAG, "linkkit connect Failed");
        linkkit_client_destroy();
        return -1;
    }

    LOGD(TAG, "After start linkkit");

    return 0;
}

void linkkit_client_destroy() {
    LOGD(TAG, "Before destroy linkkit");
    if (!g_running) {
        return;
    }
    g_running = 0;

    /* 等待线程退出，并释放线程资源，也可用分离式线程，但需要保证线程不使用linkkit资源后，再去释放linkkit */
    while (g_thread_not_quit) {
        HAL_SleepMs(20);
    }
    if (g_thread) {
        HAL_ThreadDelete(g_thread);
        g_thread = NULL;
    }

    IOT_Linkkit_Close(g_master_dev_id);
    g_master_dev_id = -1;

    if (g_master_dev_info) {
        free(g_master_dev_info);
        g_master_dev_info = NULL;
    }
    if (g_sub) {
        free(g_sub);
        g_sub = NULL;
    }
    if (g_sem) {
        HAL_SemaphorePost(g_sem);//让线程退出
    }
    while (g_sub_thread_not_quit) {
        HAL_SleepMs(20);
    }
    if (g_sem) {
        HAL_SemaphoreDestroy(g_sem);
        g_sem = NULL;
    }
    if (g_sub_thread) {
        HAL_ThreadDelete(g_sub_thread);
        g_sub_thread = NULL;
    }
    LOGD(TAG, "After destroy linkkit");
}

int linkkit_message_publish_cb(const lv_message_publish_param_s *param) {
    int rc = -1;
    iotx_mqtt_topic_info_t topic_msg;

    /* Initialize topic information */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    topic_msg.qos = param->qos;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = param->message;
    topic_msg.payload_len = strlen(param->message);
#if (defined LINKVISUAL_WITH_LINKKIT) && LINKVISUAL_WITH_LINKKIT
    rc = IOT_MQTT_Publish(NULL, param->topic, &topic_msg);
#else
    if(iot_msg_callback) {
        rc = iot_msg_callback(NULL, param->topic, &topic_msg);
    }
#endif
    if (rc < 0) {
        LOGE(TAG, "Publish msg error:%d", rc);
        return -1;
    }

    return 0;
}


/* 属性设置回调 */
void linkkit_client_set_property_handler(const lv_device_auth_s *auth, const char *value) {
    /**
     * 当收到属性设置时，开发者需要修改设备配置、改写已存储的属性值，并上报最新属性值。demo只上报了最新属性值。
     */
    IOT_Linkkit_Report(auth->dev_id, ITM_MSG_POST_PROPERTY, (unsigned char *)value, strlen(value));

}

void iot_msg_callback_register(iot_msg_cb  cb)
{
    iot_msg_callback = cb;
}