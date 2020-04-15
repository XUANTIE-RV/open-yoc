/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc_config.h>
#include <yoc/aui_cloud.h>
#include "aui_cloud_imp.h"
#include <librws.h>
#include "avutil/straight_fifo.h"

#define TAG "CLOUD_XFYUN_RASR"

#define PER_WSIZE ((16000 / 1000) * (16 / 8) * 40)

typedef enum {
    ASR_STOPED,
    ASR_INIT,
    ASR_START, // connnet success
    ASR_CONTINUE,
} asr_status;

/* FIXME: rtasr instance exit only one */
static int g_rtasr_using;

#define ASR_DISCONN_EVT       (0x01)
#define ASR_TASKQUIT_EVT      (0x02)
struct xf_rtasr_priv {
    aui_t                 *aui;
    rws_socket            sock;
    sfifo_t               *fifo;
    aos_sem_t             sem_conn;
    aos_event_t           evt_quit;
    asr_status            state;
    char                  *result;
};

static void _rtasr_task(void *arg);

static void _on_received_text(rws_socket socket, const char *text, const unsigned int length, bool is_finish)
{
    char *buff = NULL;
    struct xf_rtasr_priv *priv = rws_socket_get_user_object(socket);

    if (!(text && length)) {
        LOGE(TAG, "%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }

    //LOGD(TAG, "%s: Socket text[%10d]: %s", __FUNCTION__, length, text);
    if (priv->result) {
        if (length > strlen(priv->result)) {
            aos_free(priv->result);
            priv->result = NULL;
        } else {
            return;
        }
    }

    buff = (char *)aos_malloc(length + 1);
    if (!buff) {
        LOGE(TAG, "%s: Not enough memory. len:%d", __FUNCTION__, length + 1);
        return;
    }

    memcpy(buff, text, length);
    buff[length] = 0;
    priv->result = buff;
}

static void _on_received_bin(rws_socket socket, const void * data, const unsigned int length, bool is_finish)
{
#if 0
    char *buff = NULL;
    LOGD(TAG, "_on_received_bin.....");
    if (!socket || !data || !length) {
        LOGE(TAG, "%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }

    buff = (char *)aos_malloc(length + 1);
    if (!buff) {
        LOGE(TAG, "%s: Not enough memory. len:%d", __FUNCTION__, length + 1);
        return;
    }

    memcpy(buff, data, length);
    buff[length] = 0;

    // LOGD(TAG, "%s: Socket bin: \n%s", __FUNCTION__, buff);
    aos_free(buff);
    buff = NULL;
#else
    return;
#endif
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
    struct xf_rtasr_priv *priv = rws_socket_get_user_object(socket);

    priv->state = ASR_START;
    aos_sem_signal(&priv->sem_conn);
    LOGD(TAG, "%s: Socket connected", __FUNCTION__);
}

static void _on_disconnected(rws_socket socket)
{
    struct xf_rtasr_priv *priv = rws_socket_get_user_object(socket);
    rws_error error = rws_socket_get_error(socket);

    if (error) {
        LOGI(TAG, "%s: Socket disconnect with code, %i, %s",
             __FUNCTION__,
             rws_error_get_code(error),
             rws_error_get_description(error));
    }

    LOGD(TAG, "!!!!!!!!!!%s, %d", __func__, __LINE__);
    priv->state = ASR_STOPED;
    aos_sem_signal(&priv->sem_conn);
    aos_event_set(&priv->evt_quit, ASR_DISCONN_EVT, AOS_EVENT_OR);
}

void xfyun_rasr_init(aui_t *aui)
{

}

static void _xf_rtasr_delete(struct xf_rtasr_priv *priv)
{
    rws_socket sock = priv->sock;

    rws_socket_disconnect_and_release(sock);
    aos_sem_free(&priv->sem_conn);
    aos_event_free(&priv->evt_quit);
    priv->state = ASR_STOPED;

    sfifo_set_eof(priv->fifo, 1, 1);
    sfifo_destroy(priv->fifo);
    aos_free(priv->result);
    aos_free(priv);
    g_rtasr_using = 0;
}

static struct xf_rtasr_priv * _xf_rtasr_new(aui_t *aui)
{
    int rc;
    char path[128];
    sfifo_t *fifo   = NULL;
    rws_socket sock = NULL;
    struct xf_rtasr_priv *priv;

    if (g_rtasr_using) {
        LOGI(TAG, "xf rtasr is using. call later", path);
        return NULL;
    }
    priv = aos_zalloc(sizeof(struct xf_rtasr_priv));
    CHECK_RET_WITH_RET(priv, NULL);

    fifo = sfifo_create(PER_WSIZE * 80);
    CHECK_RET_WITH_GOTO(fifo, err);

    sock = rws_socket_create(); // create and store socket handle
    CHECK_RET_WITH_GOTO(sock, err);

    snprintf(path, 128, "/v1/ws?%s", xfyun_rasr_get_handshake_params());
    LOGD(TAG, "path==%s", path);

    rws_socket_set_url(sock, "ws", XF_RASR_BASE_HOST, 80, path);
    rws_socket_set_on_disconnected(sock, &_on_disconnected);
    rws_socket_set_on_connected(sock, &_on_connected);
    rws_socket_set_on_received_text(sock, &_on_received_text);
    rws_socket_set_on_received_bin(sock, &_on_received_bin);
    rws_socket_set_on_received_pong(sock, &_on_received_pong);

    rws_socket_set_user_object(sock, priv);
    priv->aui         = aui;
    priv->fifo        = fifo;
    priv->sock        = sock;
    priv->state       = ASR_INIT;
    aos_sem_new(&priv->sem_conn, 0);
    aos_event_new(&priv->evt_quit, 0);
    rws_socket_connect(sock);

    aui->context  = priv;
    g_rtasr_using = 1;
    //LOGI(TAG, "%s success, priv = %p, sock = %p", __FUNCTION__, priv, sock);
    rc = aos_task_new("xf_rtasr", _rtasr_task, (void *)priv, 8*1024);
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);

    return priv;
err:
    rws_socket_delete(sock);
    sfifo_destroy(fifo);
    aos_free(priv);
    return NULL;
}

static void _rtasr_task(void *arg)
{
    int rlen, rc;
    char *pos;
    rws_bool ret;
    uint8_t weof = 0;
    unsigned int flag;
    const char *end = "{\"end\": true}";
    struct xf_rtasr_priv *priv = arg;
    aui_t *aui                 = priv->aui;
    sfifo_t *fifo              = priv->fifo;
    rws_socket sock            = priv->sock;

    rc = aos_sem_wait(&priv->sem_conn, 5000);
    if (priv->state != ASR_START) {
        LOGE(TAG, "connect err, state = %d, rc = %d.", priv->state, rc);
        goto quit;
    }

    while (priv->state != ASR_STOPED) {
        rlen = sfifo_get_rpos(fifo, &pos, 1*1000);
        sfifo_get_eof(fifo, NULL, &weof);
        if (rlen <= 0 || weof) {
            LOGE(TAG, "quit. rlen = %d, weof = %d", rlen, weof);
            break;
        }

        if (rlen >= PER_WSIZE) {
            ret = rws_socket_send_text2(sock, pos, PER_WSIZE);
            if (ret == rws_false) {
                LOGE(TAG, "send text2 failed. errno = %d, ret = %d", errno, ret);
                sfifo_set_eof(fifo, 1, 0);
                break;
            }
            //LOGD(TAG, "=================>>>>send: rlen = %d", rlen);
            sfifo_set_rpos(fifo, PER_WSIZE);
        } else {
            aos_msleep(20);
        }
    }

    if (weof && rws_socket_is_connected(sock)) {
        ret = rws_socket_send_text2(sock, end, strlen(end));
        LOGD(TAG, "pcm finish, send end ret = %d", ret);
    }

    /* wait disconnet and get the last text resp */
    rc = aos_event_get(&priv->evt_quit, ASR_DISCONN_EVT, AOS_EVENT_OR_CLEAR, &flag, 10000);

quit:
    LOGI(TAG, "task pcm finish1 .... state = %d, rc = %d", priv->state, rc);
    sfifo_set_eof(priv->fifo, 1, 0);
    // TODO: nlp process
    if (aui && aui->config.nlp_cb) {
        if (priv->result)
            aui->config.nlp_cb(priv->result);
        else
            aui->config.nlp_cb("{\"error\":\"xf rtasr failed\"}");
    }

    /* wait the quit evt, we can free priv res */
    aos_event_get(&priv->evt_quit, ASR_TASKQUIT_EVT, AOS_EVENT_OR_CLEAR, &flag, AOS_WAIT_FOREVER);
    _xf_rtasr_delete(priv);
    //LOGI(TAG, "task pcm finish3 ..., priv = %p", priv);

    return;
}

int xfyun_rasr_source_pcm(aui_t *aui, char *data, int size)
{
    int wlen;
    char *pos;
    sfifo_t *fifo;
    uint8_t reof = 0;
    struct xf_rtasr_priv *priv;

    if (!(aui && data && size)) {
        return -1;
    }

    priv = aui->context;
    if (!priv) {
        priv = _xf_rtasr_new(aui);
        CHECK_RET_WITH_RET(priv, -1);
    }

    fifo = priv->fifo;
    //LOGD(TAG, "pcm push size:%d, %d", size, priv->state);
    if (priv->state == ASR_START) {
        priv->state = ASR_CONTINUE;
    }

    while (size > 0) {
        wlen = sfifo_get_wpos(fifo, &pos, 2*1000);
        sfifo_get_eof(fifo, &reof, NULL);
        if ((wlen < 0) || (wlen == 0 && reof)) {
            LOGE(TAG, "quit. wlen = %d, reof = %d", wlen, reof);
            break;
        }

        wlen  = wlen >= size ? size : wlen;
        size -= wlen;
        memcpy(pos, data, wlen);
        sfifo_set_wpos(fifo, wlen);
    }

    return size == 0 ? 0 : -1;
}

int xfyun_rasr_source_pcm_finish(aui_t *aui)
{
    struct xf_rtasr_priv *priv;

    if (!(aui && aui->context)) {
        return -1;
    }

    priv = aui->context;
    sfifo_set_eof(priv->fifo, 0, 1);

    /* priv res free when after the evt set */
    aos_event_set(&priv->evt_quit, ASR_TASKQUIT_EVT, AOS_EVENT_OR);
    aui->context = NULL;
    LOGI(TAG, "pcm finish ...");

    return 0;
}


