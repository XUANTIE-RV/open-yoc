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
#include <iot_export.h>
#include "iot_alicoap_uservice.h"

#define MAX_UNACK_MSG 5
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

static const char *g_server_url_fmt = NULL;
static iotx_device_info_t    g_deviceinfo;
static char g_alicoap_url[IOTX_URI_MAX_LEN];
static iotx_coap_context_t *g_p_ctx = NULL;

static int cnt_unacked_msg = 0;

static void iotx_response_handler(void *arg, void *p_response)
{
    int                   len       = 0;
    unsigned char *       p_payload = NULL;
    iotx_coap_resp_code_t resp_code;

    if (p_response) {
        IOT_CoAP_GetMessageCode(p_response, &resp_code);
        IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
        //YIO_LOGD( "[APPL]: Message response code: %d", resp_code);
        //YIO_LOGD( "[APPL]: Len: %d, Payload: %s", len, p_payload);

        /* Decrease the unacked message num, if recv ark */
        cnt_unacked_msg = 0;

        /* notify yio for post event */
        event_publish(EVENT_IOT_PUSH_SUCCESS, NULL);
    } else {
        /* notify yio for post event */
        event_publish(EVENT_IOT_PUSH_FAILED, NULL);
        YIO_LOGE("no ack");
    }
}

static int alicoap_inter_send(const char *topic, void *payload, int len)
{
    int            rc;
    unsigned char  uri[IOTX_URI_MAX_LEN + 1] = {0};
    iotx_message_t message;

    if (g_p_ctx == NULL || topic == NULL || payload == NULL) {
        return -EINVAL;
    }

    message.p_payload     = payload;
    message.payload_len   = len;
    message.resp_callback = iotx_response_handler;
    message.msg_type      = IOTX_MESSAGE_CON;
    message.content_type  = IOTX_CONTENT_TYPE_JSON;

    snprintf((char *)uri, IOTX_URI_MAX_LEN, "/topic/%s/%s/%s/", g_deviceinfo.product_key,
             g_deviceinfo.device_name, topic);

    YIO_LOGD("payload: %s\r\nlen :%d\r\ntopic: %s\r\n type: %d", (char *)payload, len, uri,
             IOTX_MESSAGE_CON);
    rc = IOT_CoAP_SendMessage(g_p_ctx, (char *)uri, &message);

    if (rc) {
        YIO_LOGE("Pub %d", rc);
        //IOTX_ERR_RECV_MSG_TIMEOUT = -9,   /*Receive message timeout */
        //IOTX_ERR_SEND_MSG_FAILED =  -8,   /* Send message failed*/
        //IOTX_ERR_MSG_TOO_LOOG    =  -7,   /* The payload too loog */
        //IOTX_ERR_URI_TOO_LOOG    =  -6,   /* URI length too long */
        //IOTX_ERR_NOT_AUTHED      =  -5,   /* Client isn't authed */
        //IOTX_ERR_AUTH_FAILED     =  -4,   /* Client authed failed */
        //IOTX_ERR_BUFF_TOO_SHORT  =  -3,   /* Buffer too short */
        //IOTX_ERR_NO_MEM          =  -2,   /* Malloc failed */
        //IOTX_ERR_INVALID_PARAM   =  -1,   /* Invalid parameter */
        //IOTX_SUCCESS             =   0,   /* Success */
    } else {
        /* Increase the unacked message num, if send success */
        cnt_unacked_msg++;
    }

    return rc;
}

static int alicoap_inter_connect(void)
{
    int rc;

    char product_key[PRODUCT_KEY_LEN + 1];
    char device_name[DEVICE_NAME_LEN + 1];
    char device_secret[PRODUCT_SECRET_LEN + 1];

    uint32_t product_key_len   = sizeof(product_key);
    uint32_t device_name_len   = sizeof(device_name);
    uint32_t device_secret_len = sizeof(device_secret);

    rc = nvram_get_iot_info(product_key, &product_key_len, device_name, &device_name_len,
                            device_secret, &device_secret_len);

    if (product_key[0] == '\0' || device_name[0] == '\0' || device_secret[0] == '\0' || rc < 0) {
        YIO_LOGE("alicoap param");
        return -EINVAL;
    }

    if (g_p_ctx != NULL || rc < 0) {
        YIO_LOGE("coap param");
        return -EINVAL;
    }

    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(product_key, device_name, device_secret, NULL)) {
        YIO_LOGE("set conn info");
        return -EINVAL;
    }

    /* Implement in linkkit lib */
    extern int iotx_device_info_get(iotx_device_info_t *device_info);
    iotx_device_info_get(&g_deviceinfo);
    snprintf(g_alicoap_url, sizeof(g_alicoap_url), g_server_url_fmt, g_deviceinfo.product_key);

    iotx_coap_config_t config = {
        .p_devinfo = &g_deviceinfo,
        .wait_time_ms = YEILD_WAIT_TIME,
        .p_url = g_alicoap_url,
        .event_handle = NULL,
    };
    

    //IOT_OpenLog("coap");
    //IOT_SetLogLevel(IOT_LOG_DEBUG);

    g_p_ctx = IOT_CoAP_Init(&config);

    if (g_p_ctx == NULL) {
        YIO_LOGE("init");
        return -1;
    }

    rc = IOT_CoAP_DeviceNameAuth(g_p_ctx);
    if (rc) {
        YIO_LOGE("Auth %d", rc);
        IOT_CoAP_Deinit(&g_p_ctx);
        return -1;
    }
    //YIO_LOGI("conn ok");

    /* reset unacked msg when connected */
    cnt_unacked_msg = 0;
    return 0;
}

static int alicoap_inter_disconnect(void)
{
    if (g_p_ctx == NULL) {
        return -EINVAL;
    }

    IOT_CoAP_Deinit(&g_p_ctx);
    YIO_LOGI("ALICOAP DEINIT");
    return 0;
}

static int alicoap_inter_yield(void)
{
    if (g_p_ctx == NULL) {
        return -EINVAL;
    }

    IOT_CoAP_Yield(g_p_ctx);

    /*
     * If the unacked messages are more than MAX_UNACK_MSG,
     * it means we may loss the connect with server,
     * we need to reconnect
     */
    if (cnt_unacked_msg > MAX_UNACK_MSG) {
        alicoap_inter_disconnect();
        event_publish(EVENT_IOT_DISCONNECTED, NULL);
    }

    //YIO_LOGD( "cnt_unacked_msg: %d", cnt_unacked_msg);

    return 0;
}

/******************
 *uService
 ******************/
#include <yoc/uservice.h>

#define ALIYUN_STACK_SIZE 3072

#define API_ALI_USVC_CONNECT 1
#define API_ALI_USVC_SEND 2
#define API_ALI_USVC_SUBSCRIBE 3
#define API_ALI_USVC_DISCONNECT 4
#define API_ALI_USVC_YILED 5

struct ali_uservice {
    uservice_t *srv;
    utask_t    *task;
    aos_timer_t yield_timer;
    int yield_count;
};

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

static struct ali_uservice g_alicoap_svc;

/* Service */
static int alicoap_service(void *context, rpc_t *rpc)
{
    //struct ali_uservice *ali_usvc = (struct ali_uservice *)context;
    //YIO_LOGD("rpc %d", rpc->cmd_id);
    int ret;
    switch (rpc->cmd_id) {
        case API_ALI_USVC_CONNECT: {
            ret = alicoap_inter_connect();
            break;
        }

        case API_ALI_USVC_SEND: {
            struct _param_send *param = (struct _param_send *)rpc_get_point(rpc);
            ret = alicoap_inter_send(param->topic, param->payload, param->len);
            break;
        }
/*
        case API_ALI_USVC_SUBSCRIBE: {
            struct _param_subscribe *param = (struct _param_subscribe *)rpc_get_point(rpc);
            ret = alicoap_inter_subscribe(param->topic, param->cb, param->arg);
            break;
        }
*/
        case API_ALI_USVC_DISCONNECT: {
            ret = alicoap_inter_disconnect();
            break;
        }

        case API_ALI_USVC_YILED: {
            alicoap_inter_yield();

            if (g_p_ctx) {
                aos_timer_start(&g_alicoap_svc.yield_timer);
            }
            g_alicoap_svc.yield_count --;
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
    if (g_p_ctx == NULL) {
        return;
    }
    alicoap_usrv_yield();
}

/* API */
int alicoap_usrv_init(const char *server_url_suffix)
{
    utask_t *task;

    g_server_url_fmt = server_url_suffix;

    /* uService */
    task              = utask_new("aliyun", ALIYUN_STACK_SIZE, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
    g_alicoap_svc.srv = uservice_new("aliyun_svc", alicoap_service, &g_alicoap_svc);
    utask_add(task, g_alicoap_svc.srv);
    g_alicoap_svc.task = task;

    /* Timer */
    aos_timer_new_ext(&g_alicoap_svc.yield_timer, yield_timer_entry, NULL, YEILD_INTERVAL_TIME, 0, 0);

    return 0;
}

void alicoap_usrv_deinit(void)
{
    aos_timer_stop(&g_alicoap_svc.yield_timer);
    aos_timer_free(&g_alicoap_svc.yield_timer);
    utask_destroy(g_alicoap_svc.task);
    uservice_destroy(g_alicoap_svc.srv);
}

int alicoap_usrv_connect(void)
{
    int ret;

    uservice_call_sync(g_alicoap_svc.srv, API_ALI_USVC_CONNECT, NULL, &ret, sizeof(int));

    if (ret == 0) {
        //aos_timer_change(&g_alicoap_svc.yield_timer, )
        aos_timer_start(&g_alicoap_svc.yield_timer);
        event_publish(EVENT_IOT_CONNECT_SUCCESS, NULL);
    } else {
        event_publish(EVENT_IOT_CONNECT_FAILED, NULL);
    }

    return ret;
}

int alicoap_usrv_yield(void)
{
    if (g_alicoap_svc.yield_count <= 0) {
        uservice_call_async(g_alicoap_svc.srv, API_ALI_USVC_YILED, NULL, 0);
        g_alicoap_svc.yield_count++;
        g_alicoap_svc.yield_count %= 20;
    }

    return 0;
}

int alicoap_usrv_send(const char *topic, void *payload, int len)
{
    int ret;

    struct _param_send param;
    param.topic   = topic;
    param.payload = payload;
    param.len     = len;

    uservice_call_sync(g_alicoap_svc.srv, API_ALI_USVC_SEND, &param, &ret, sizeof(int));

    return ret;
}

/*
int alicoap_usrv_subscribe(const char *topic, subscribe_cb_t cb, void *arg)
{
    int ret;

    struct _param_subscribe param;

    param.topic = topic;
    param.cb    = cb;
    param.arg   = arg;

    uservice_call_sync(g_alicoap_svc.srv, API_ALI_USVC_SUBSCRIBE, &param, &ret, sizeof(int));

    return ret;
}
*/

int alicoap_usrv_disconnect(void)
{
    int ret;

    uservice_call_sync(g_alicoap_svc.srv, API_ALI_USVC_DISCONNECT, NULL, &ret, sizeof(int));

    return ret;
}
