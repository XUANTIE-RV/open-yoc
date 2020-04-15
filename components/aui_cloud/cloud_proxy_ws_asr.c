/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc/aui_cloud.h>
#include <yoc_config.h>
#include <aos/log.h>
#include <aos/kv.h>
#include <yoc/eventid.h>
#include <yoc/uservice.h>
#include <librws.h>
#include "aui_cloud_imp.h"

#define TAG "PROXY_WS_ASR"

#define PER_WSIZE ((16000 / 1000) * (16 / 8) * 80)
#define EVENT_RECONN 1
#define EVENT_ASR_END 1

typedef enum send_pcm_state {
    PCM_START = 100,
    PCM_CONTINUE,
    PCM_FINISH
} send_pcm_state_e;

struct proxy_asr_priv {
    aui_t                 *aui;
    rws_socket            sock;
    send_pcm_state_e      send_pcm_state;
    aos_event_t           event;
    aos_event_t           end_event;
    char                  *result;
};

static bool g_net_connected = false;
static bool g_is_reconn = false;

static void _on_received_text(rws_socket socket, const char *text, const unsigned int length, bool is_finish)
{
    struct proxy_asr_priv *priv = rws_socket_get_user_object(socket);

    if (!(text && length)) {
        LOGE(TAG, "%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }
    // LOGD(TAG, "%s, len:%d", __func__, length);
    // LOGD(TAG, "text: %s\n", text);
    if (priv->result) {
        aos_free(priv->result);
        priv->result = aos_zalloc_check(length + 1);
    } else {
        priv->result = aos_zalloc_check(length + 1);
    }
    memcpy(priv->result, text, length);
    priv->result[length] = 0;
    LOGD(TAG, "priv->result: %s", priv->result);
    if (strstr(priv->result, "\"is_end\": \"1\"")) {
        //aos_event_set(&priv->end_event, EVENT_ASR_END, AOS_EVENT_OR);
        // FIXME: wait for 3000 milliseconds when there is no` EVENT_ASR_END` event.
        //unsigned int flags;
        //aos_event_get(&priv->end_event, EVENT_ASR_END, AOS_EVENT_OR_CLEAR, &flags, 3000);
        LOGI(TAG, "wait end asr result finish ...");

        aui_t *aui = priv->aui;        
        if (aui && aui->config.nlp_cb) {
            if (priv->result)
                aui->config.nlp_cb(priv->result);
            else
                aui->config.nlp_cb("{\"error\":\"aliyun asr failed\"}");
        }
    }
}

static void _on_received_bin(rws_socket socket, const void * data, const unsigned int length, bool is_finish)
{

}

static void _on_received_pong(rws_socket socket)
{
    if (!socket) {
        LOGE(TAG, "%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }
    LOGD(TAG, "received pong!!!!!!!!!!!");
}

static void _on_connected(rws_socket socket)
{
    LOGD(TAG, "%s: Socket connected", __FUNCTION__);
}

static void _on_disconnected(rws_socket socket)
{
    struct proxy_asr_priv *priv = rws_socket_get_user_object(socket);
    rws_error error = rws_socket_get_error(socket);

    if (error) {
        LOGI(TAG, "%s: Socket disconnect with code, %i, %s",
             __FUNCTION__,
             rws_error_get_code(error),
             rws_error_get_description(error));
    }
    aos_msleep(2000);
    if (g_net_connected && !g_is_reconn) {
        aos_event_set(&priv->event, EVENT_RECONN, AOS_EVENT_OR);
    }

    LOGD(TAG, "!!!!!!!!!!%s, %d", __func__, __LINE__);
}

static rws_socket _proxy_ws_new(struct proxy_asr_priv *priv)
{
    rws_socket sock = NULL;
    char ars_ip_port[64];
    int asr_port = 0;

    int ret = aos_kv_getstring("asr_server", ars_ip_port, sizeof(ars_ip_port));
    if (ret > 0) {
        char *str_port = strchr(ars_ip_port, ':');
        if (str_port) {
            *str_port = '\0';
            str_port ++;
        }
        asr_port = atoi(str_port);
    } else {
        asr_port = ALIYUN_ASR_PORT;
        strcpy(ars_ip_port, ALIYUN_ASR_IP);
    }

    CHECK_RET_WITH_RET(priv, NULL);

    sock = rws_socket_create(); // create and store socket handle
    CHECK_RET_WITH_GOTO(sock, err);

    LOGD(TAG, "ws init, path==%s, port:%d", ars_ip_port, asr_port);

    rws_socket_set_url(sock, "ws", ars_ip_port, asr_port, "/");
    rws_socket_set_on_disconnected(sock, &_on_disconnected);
    rws_socket_set_on_connected(sock, &_on_connected);
    rws_socket_set_on_received_text(sock, &_on_received_text);
    rws_socket_set_on_received_bin(sock, &_on_received_bin);
    rws_socket_set_on_received_pong(sock, &_on_received_pong);

    rws_socket_set_user_object(sock, priv);
    rws_socket_connect(sock);

    return sock;
err:
    rws_socket_delete(sock);
    return NULL;
}

static void _proxy_ws_delete(struct proxy_asr_priv *priv)
{
    rws_socket sock = priv->sock;

    rws_socket_disconnect_and_release(sock);
}

static void _ws_reconn_task(void *arg)
{
    unsigned int flags;
    struct proxy_asr_priv *priv = (struct proxy_asr_priv *)arg;

    while (1)
    {
        aos_event_get(&priv->event, EVENT_RECONN, AOS_EVENT_OR_CLEAR, &flags, AOS_WAIT_FOREVER);
        g_is_reconn = true;
        _proxy_ws_delete(priv);
        rws_socket sock = _proxy_ws_new(priv);
        priv->sock = sock;
        g_is_reconn = false;
    }
    
}

static void user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    struct proxy_asr_priv *priv = (struct proxy_asr_priv *)context;

    switch(event_id) {

        case EVENT_NETMGR_GOT_IP:
            LOGD(TAG, "GOT IP");
            g_net_connected = true;
            aos_event_set(&priv->event, EVENT_RECONN, AOS_EVENT_OR);
            break;
        case EVENT_NETMGR_NET_DISCON:
            LOGW(TAG, "NET DISCONNECT!");
            g_net_connected = false;
            break;
        default:
            break;
    }
}

void proxy_ws_asr_init(aui_t *aui)
{
    struct proxy_asr_priv *priv;

    priv = aos_zalloc_check(sizeof(struct proxy_asr_priv));
    
    aui->context = priv;
    rws_socket sock = _proxy_ws_new(priv);
    priv->aui         = aui;
    priv->sock        = sock;
    priv->send_pcm_state = PCM_FINISH;
    priv->result = NULL;
    aos_event_new(&priv->event, 0);
    aos_event_new(&priv->end_event, 0);

    aos_task_new("_proxy_ws_reconn", _ws_reconn_task, priv, 4096);

    event_subscribe(EVENT_NETMGR_GOT_IP, user_local_event_cb, priv);
    event_subscribe(EVENT_NETMGR_NET_DISCON, user_local_event_cb, priv);
}

static int ws_send_bin(rws_socket _socket, const char *bin, size_t size)
{
	if (bin && size > 3) {
		int piece_size = size / 3;
		int last_piece_size = piece_size + size % 3;

		rws_bool ret1 = rws_socket_send_bin_start(_socket, &bin[0], piece_size);
		rws_bool ret2 = rws_socket_send_bin_continue(_socket, &bin[piece_size], piece_size);
		rws_bool ret3 = rws_socket_send_bin_finish(_socket, &bin[piece_size * 2], last_piece_size);
        return ret1 && ret2 && ret3;
	}
    return -1;
}

// #define PCM_REC_DEBUG
#ifdef PCM_REC_DEBUG
#define PCM_REC_BUF_SIZE (1024 * 400) 
char g_pcm_rec_buf[PCM_REC_BUF_SIZE];
int g_pcm_rec_len = 0;
#endif
int proxy_ws_asr_source_pcm(aui_t *aui, char *data, int size)
{
    rws_bool ret = rws_false;
    struct proxy_asr_priv *priv;

    if (!(aui && data && size)) {
        return -1;
    }
    // LOGD(TAG, "proxy_ws_asr_source_pcm, size:%d", size);
    priv = aui->context;

    if (!(priv && priv->sock)) {
        LOGE(TAG, "push e, invalid argument...");
        return -EINVAL;
    }
    if (rws_socket_is_connected(priv->sock) == rws_false) {
        if (g_net_connected && !g_is_reconn) {
            aos_event_set(&priv->event, EVENT_RECONN, AOS_EVENT_OR);
            LOGE(TAG, "push e, reconn ws server.");
            return -1;
        }
        LOGE(TAG, "push e..");
        return -1;
    }
    if (1) {
        if (priv->send_pcm_state == PCM_FINISH) {
            priv->send_pcm_state = PCM_START;
            if (priv->result) {
                aos_free(priv->result);
                priv->result = NULL;
            }
#ifdef PCM_REC_DEBUG
            g_pcm_rec_len = 0;
            if (g_pcm_rec_len + size < PCM_REC_BUF_SIZE) {
                memcpy(g_pcm_rec_buf + g_pcm_rec_len, data, size);
                g_pcm_rec_len += size;
            }
#endif
            if (rws_socket_send_text(priv->sock, "start") == rws_true) {
                ret = ws_send_bin(priv->sock, data, size);
            }
        }  else {
            priv->send_pcm_state = PCM_CONTINUE;
            ret = ws_send_bin(priv->sock, data, size);
#ifdef PCM_REC_DEBUG
            if (g_pcm_rec_len + size < PCM_REC_BUF_SIZE) {
                memcpy(g_pcm_rec_buf + g_pcm_rec_len, data, size);
                g_pcm_rec_len += size;
            }
#endif
        }
        // LOGD(TAG, "send bin ret:%d", ret);
    }

    return ret == rws_true ? 0 : -1;
}

// const char *end = "{\"end\": true}";
int proxy_ws_asr_source_pcm_finish(aui_t *aui)
{
    rws_bool ret = rws_false;
    struct proxy_asr_priv *priv;

    if (!(aui && aui->context)) {
        return -1;
    }

    priv = aui->context;
    priv->send_pcm_state = PCM_FINISH;
    if (priv && priv->sock && rws_socket_is_connected(priv->sock)) {
        ret = rws_socket_send_text(priv->sock, "end");
    } else {
        LOGE(TAG, "push finish e....");
    }
    if (ret == rws_false) {
        return -1;
    }
    LOGI(TAG, "pcm finish ...");

    return -1;
}


