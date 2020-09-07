/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <aos/log.h>
#include <aos/kernel.h>
#include <aos/debug.h>
#include <yoc/nvram.h>
#include <yoc/eventid.h>
#include <yoc/uservice.h>

#include "drv/tee.h"
#include "iot_import.h"
#include "iot_export.h"
#include "iot_alimqtt_uservice.h"

#define YEILD_WAIT_TIME 20 /*ms*/
#define YEILD_INTERVAL_TIME 100 /*ms*/

#define YIO_TAG "IoT"

#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_YIO)
#define YIO_LOGD(format, ...) LOGD(YIO_TAG, format, ##__VA_ARGS__)
#else
#define YIO_LOGD(format, ...)
#endif

#define YIO_LOGI(format, ...) LOGI(YIO_TAG, format, ##__VA_ARGS__)
#define YIO_LOGW(format, ...) LOGW(YIO_TAG, format, ##__VA_ARGS__)
#define YIO_LOGE(format, ...) LOGE(YIO_TAG, format, ##__VA_ARGS__)

#define TOPIC_LEN 98
#define MSG_LEN_MAX 512

static void *g_pclient       = NULL;
static void *private_sub_arg = NULL;

static int g_mqtt_disconnect_event = 0;

static subscribe_cb_t private_sub_cb = NULL;

struct ali_uservice {
    uservice_t *srv;
    utask_t *   task;
    aos_timer_t yield_timer;
    int         yield_count;
};
static struct ali_uservice g_alimqtt_svc;

static void event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg);

static void _message_arrive(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    /* Print topic name and topic message */
    YIO_LOGD("MQTT Rec Msg");
    YIO_LOGD("Topic:'%.*s' (len:%d)", ptopic_info->topic_len, ptopic_info->ptopic,
             ptopic_info->topic_len);
    YIO_LOGD("Payload:'%.*s' (len:%d)", ptopic_info->payload_len, ptopic_info->payload,
             ptopic_info->payload_len);

    if (private_sub_cb) {
        private_sub_cb(ptopic_info->ptopic, (void *)(ptopic_info->payload),
                       ptopic_info->payload_len, private_sub_arg);
    }
}

static int alimqtt_inter_send(const char *topic, void *payload, int len)
{
    iotx_mqtt_topic_info_t topic_msg;
    int                    rc;
    char                   uri[TOPIC_LEN];

    char product_key[PRODUCT_KEY_LEN + 1];
    char device_name[DEVICE_NAME_LEN + 1];

    if (g_pclient == NULL) {
        YIO_LOGE("no init");
        return -EINVAL;
    }

    topic_msg.qos         = IOTX_MQTT_QOS1;
    topic_msg.retain      = 0;
    topic_msg.dup         = 0;
    topic_msg.payload     = payload;
    topic_msg.payload_len = len;

    /* Set topic */
    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);
    snprintf(uri, sizeof(uri) - 1, "/%s/%s/%s", product_key, device_name, topic);

    YIO_LOGD("payload: %s\n\tlen :%d\n\ttopic: %s", (char *)payload, len, uri);

    rc = IOT_MQTT_Publish(g_pclient, uri, &topic_msg);

    if (rc >= 0) {
        return 0;
    }

    YIO_LOGE("Pub %d", rc);

    return rc;
}

static int alimqtt_inter_connect(void)
{
    int rc = 0;
    //Tee_mode          tee_mode;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t mqtt_params;

    char product_key[PRODUCT_KEY_LEN + 1];
    char device_name[DEVICE_NAME_LEN + 1];
    char device_secret[PRODUCT_SECRET_LEN + 1];

    uint32_t product_key_len   = sizeof(product_key);
    uint32_t device_name_len   = sizeof(device_name);
    uint32_t device_secret_len = sizeof(device_secret);

    rc = nvram_get_iot_info(product_key, &product_key_len, device_name, &device_name_len,
                            device_secret, &device_secret_len);

    if (product_key[0] == '\0' || device_name[0] == '\0' || device_secret[0] == '\0' || rc < 0) {
        YIO_LOGE("mqtt param");
        return -EINVAL;
    }

    if (g_pclient != NULL) {
        YIO_LOGE("pclient");
        return -EINVAL;
    }

    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(product_key, device_name, device_secret, (void **)&pconn_info)) {
        YIO_LOGE("set conn info");
        return -EINVAL;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.port      = pconn_info->port;
    mqtt_params.host      = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username  = pconn_info->username;
    mqtt_params.password  = pconn_info->password;
    mqtt_params.pub_key   = pconn_info->pub_key;

    mqtt_params.request_timeout_ms    = 2000;
    mqtt_params.clean_session         = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    //mqtt_params.pread_buf             = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    //mqtt_params.pwrite_buf            = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;

    mqtt_params.handle_event.h_fp     = event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    //IOT_OpenLog("mqtt");
    //IOT_SetLogLevel(IOT_LOG_DEBUG);

    /* Construct a MQTT client with specify parameter */
    g_pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == g_pclient) {
        YIO_LOGE("construct");
        rc = -1;
        goto do_exit;
    }

    return 0;

do_exit:
    //do exit
    return rc;
}

static int alimqtt_inter_disconnect(void)
{
    if (g_pclient == NULL) {
        return -EINVAL;
    }

    IOT_MQTT_Destroy(&g_pclient);
    g_pclient       = NULL;
    private_sub_cb  = NULL;
    private_sub_arg = NULL;

    YIO_LOGI("ALIMQTT DEINIT");

    return 0;
}

static int alimqtt_inter_yield(void)
{
    int ret;

    if (g_pclient == NULL) {
        return -EINVAL;
    }

    ret = IOT_MQTT_Yield(g_pclient, YEILD_WAIT_TIME);

    if (g_mqtt_disconnect_event) {
        alimqtt_inter_disconnect();
        g_mqtt_disconnect_event = 0;

        aos_timer_stop(&g_alimqtt_svc.yield_timer);
        event_publish(EVENT_IOT_DISCONNECTED, NULL);
    }

    return ret;
}

static int alimqtt_inter_subscribe(const char *topic, subscribe_cb_t cb, void *private_data)
{
    int rc;

    static char sub_topic[TOPIC_LEN];

    char product_key[PRODUCT_KEY_LEN + 1];
    char device_name[DEVICE_NAME_LEN + 1];

    if (g_pclient == NULL) {
        YIO_LOGE("no init");
        return -1;
    }

    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);
    /* Set topic */
    snprintf(sub_topic, sizeof(sub_topic) - 1, "/%s/%s/%s", product_key, device_name, topic);

    YIO_LOGD("Topic: %s", sub_topic);

    if (cb == NULL) {
        private_sub_cb  = NULL;
        private_sub_arg = NULL;

        rc = IOT_MQTT_Unsubscribe(g_pclient, sub_topic);

        if (rc < 0) {
            YIO_LOGE("Unsub %d", rc);
            return rc;
        }
    } else {

        /* Subscribe the specific topic */
        rc = IOT_MQTT_Subscribe(g_pclient, sub_topic, IOTX_MQTT_QOS1, _message_arrive, NULL);

        if (rc < 0) {
            YIO_LOGE("Subscribe %d", rc);
            return rc;
        }

        private_sub_cb  = cb;
        private_sub_arg = private_data;
    }

    return 0;
}

/******************
 *uService
 ******************/
#ifdef CONFIG_TEE_CA
#define ALIYUN_STACK_SIZE 2564
#else
#define ALIYUN_STACK_SIZE (1024 * 6)
#endif

#define API_ALI_USVC_CONNECT 1
#define API_ALI_USVC_SEND 2
#define API_ALI_USVC_SUBSCRIBE 3
#define API_ALI_USVC_DISCONNECT 4
#define API_ALI_USVC_YILED 5

struct _param_send {
    const char *topic;
    void *      payload;
    int         len;
};

struct _param_subscribe {
    const char *   topic;
    subscribe_cb_t cb;
    void *         arg;
};

/* Service */
static int alimqtt_service(void *context, rpc_t *rpc)
{
    //struct ali_uservice *ali_usvc = (struct ali_uservice *)context;
    //printf("rpc %d\n", rpc->cmd_id);
    int ret;
    switch (rpc->cmd_id) {
        case API_ALI_USVC_CONNECT:
            ret = alimqtt_inter_connect();
            break;
        case API_ALI_USVC_SEND: {
            struct _param_send *param = (struct _param_send *)rpc_get_point(rpc);
            ret = alimqtt_inter_send(param->topic, param->payload, param->len);
            break;
        }

        case API_ALI_USVC_SUBSCRIBE: {
            struct _param_subscribe *param = (struct _param_subscribe *)rpc_get_point(rpc);
            ret = alimqtt_inter_subscribe(param->topic, param->cb, param->arg);
            break;
        }

        case API_ALI_USVC_DISCONNECT:
            ret = alimqtt_inter_disconnect();
            break;

        case API_ALI_USVC_YILED: {
            alimqtt_inter_yield();

            if (g_pclient) {
                aos_timer_start(&g_alimqtt_svc.yield_timer);
            }
            g_alimqtt_svc.yield_count--;
        }
        default:
            rpc_reply(rpc);

            return 0;
    }

    rpc_put_reset(rpc);

    /*
     * When an asynchronous API call, 
     * rpc->data is empty and cannot have a return value.
     */
    if (rpc->data) {
        rpc_put_int(rpc, ret);
    }

    rpc_reply(rpc);

    return 0;
}

static void yield_timer_entry(void *timer, void *arg)
{
    //YIO_LOGD("yield");
    alimqtt_usrv_yield();
}

/* API */
int alimqtt_usrv_init(void)
{
    utask_t *task;

    /* uService */
    task = utask_new("aliyun", ALIYUN_STACK_SIZE, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
    g_alimqtt_svc.srv = uservice_new("aliyun_svc", alimqtt_service, &g_alimqtt_svc);
    utask_add(task, g_alimqtt_svc.srv);
    g_alimqtt_svc.task = task;

    /* Timer */
    aos_timer_new_ext(&g_alimqtt_svc.yield_timer, yield_timer_entry, NULL, YEILD_INTERVAL_TIME, 0, 0);

    return 0;
}

void alimqtt_usrv_deinit(void)
{
    aos_timer_stop(&g_alimqtt_svc.yield_timer);
    aos_timer_free(&g_alimqtt_svc.yield_timer);
    utask_destroy(g_alimqtt_svc.task);
    uservice_destroy(g_alimqtt_svc.srv);
}

int alimqtt_usrv_connect(void)
{
    int ret;

    uservice_call_sync(g_alimqtt_svc.srv, API_ALI_USVC_CONNECT, NULL, &ret, sizeof(int));
    //uservice_call_async(g_alimqtt_svc.srv, API_ALI_USVC_CONNECT, NULL, 0);

    if (ret == 0) {
        //aos_timer_change(&g_alimqtt_svc.yield_timer, )
        aos_timer_start(&g_alimqtt_svc.yield_timer);
        event_publish(EVENT_IOT_CONNECT_SUCCESS, NULL);
    } else {
        event_publish(EVENT_IOT_CONNECT_FAILED, NULL);
    }

    return ret;
}

int alimqtt_usrv_yield(void)
{

    if (g_alimqtt_svc.yield_count <= 0) {
        uservice_call_async(g_alimqtt_svc.srv, API_ALI_USVC_YILED, NULL, 0);
        g_alimqtt_svc.yield_count++;
        g_alimqtt_svc.yield_count %= 20;
    }

    return 0;
}

int alimqtt_usrv_send(const char *topic, void *payload, int len)
{
    int ret;

    struct _param_send param;
    param.topic   = topic;
    param.payload = payload;
    param.len     = len;

    uservice_call_sync(g_alimqtt_svc.srv, API_ALI_USVC_SEND, &param, &ret, sizeof(int));

    return ret;
}

int alimqtt_usrv_subscribe(const char *topic, subscribe_cb_t cb, void *arg)
{
    int ret;

    struct _param_subscribe param;

    param.topic = topic;
    param.cb    = cb;
    param.arg   = arg;

    uservice_call_sync(g_alimqtt_svc.srv, API_ALI_USVC_SUBSCRIBE, &param, &ret, sizeof(int));

    return ret;
}

int alimqtt_usrv_disconnect(void)
{
    int ret;

    uservice_call_sync(g_alimqtt_svc.srv, API_ALI_USVC_DISCONNECT, NULL, &ret, sizeof(int));

    return ret;
}

/******************
 *Event Convert
 ******************/
static void event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    //uintptr_t packet_id = (uintptr_t)msg->msg;
    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_RECONNECT:
        case IOTX_MQTT_EVENT_UNDEF:
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            /* notify yio for post event */
            g_mqtt_disconnect_event = 1;
            //YIO_LOGI("EVENT disconn");
            break;
        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            /* notify yio for post event */
            YIO_LOGD("EVENT sub succ");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            /* notify yio for post event */
            YIO_LOGE("EVENT sub fail");
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            /* notify yio for post event */
            YIO_LOGD("EVENT unsub succ");
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            /* notify yio for post event */
            YIO_LOGE("EVENT unsub fail");
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            /* notify yio for post event */
            event_publish(EVENT_IOT_PUSH_SUCCESS, NULL);
            //YIO_LOGD("EVENT pub succ");
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            /* notify yio for post event */
            event_publish(EVENT_IOT_PUSH_FAILED, NULL);
            YIO_LOGD("EVENT pub fail %d", msg->event_type);
            break;

        default:
            break;
    }

    YIO_LOGD("EVENT type %d", msg->event_type);
}