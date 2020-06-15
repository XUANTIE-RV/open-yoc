/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc/aui_cloud.h>
#include <aos/log.h>
#include <aos/kv.h>
#include <aos/ringbuffer.h>
#include <yoc/eventid.h>
#include <yoc/uservice.h>
#include <librws.h>
#include <rws_socket.h>
#include <cJSON.h>
#include "cloud_aliyun.h"

#define TAG "aliyun_rasr"

#define EVENT_CONNECTED     1
#define EVENT_DISCONNECT    (1<<1)
#define EVENT_ASR_END       (1<<2)
#define EVENT_ASR_SER_OK    (1<<3)
#define EVENT_ASR_SER_ERR   (1<<4)
#define EVENT_PCM_DATA      (1<<5)
#define EVENT_PCM_LAST_DATA (1<<6)
#define EVENT_PCM_EMPTY     (1<<7)
#define EVENT_TASK_QUIT     (1<<8)

const char *CONTEXT = "context";
const char *CONTEXT_SDK_KEY = "sdk";
const char *CONTEXT_SDK_KEY_NAME = "name";
const char *CONTEXT_SDK_VALUE_NAME = "nls-sdk-python";
const char *CONTEXT_SDK_KEY_VERSION = "version";
const char *CONTEXT_SDK_VALUE_VERSION = "2.0.1";
const char *HEADER_TOKEN = "X-NLS-Token";
const char *HEADER = "header";
const char *HEADER_KEY_NAMESPACE = "namespace";
const char *HEADER_KEY_NAME = "name";
const char *HEADER_KEY_MESSAGE_ID = "message_id";
const char *HEADER_KEY_APPKEY = "appkey";
const char *HEADER_KEY_TASK_ID = "task_id";
const char *HEADER_KEY_STATUS = "status";
const char *HEADER_KEY_STATUS_TEXT = "status_text";
const char *PAYLOAD = "payload";
const char *PAYLOAD_KEY_SAMPLE_RATE = "sample_rate";
const char *PAYLOAD_KEY_FORMAT = "format";
const char *PAYLOAD_KEY_ENABLE_ITN = "enable_inverse_text_normalization";
const char *PAYLOAD_KEY_ENABLE_INTERMEDIATE_RESULT = "enable_intermediate_result";
const char *PAYLOAD_KEY_ENABLE_PUNCTUATION_PREDICTION = "enable_punctuation_prediction";
const char *PAYLOAD_KEY_VOICE = "voice";
const char *PAYLOAD_KEY_TEXT = "text";
const char *PAYLOAD_KEY_VOLUME = "volume";
const char *PAYLOAD_KEY_SPEECH_RATE = "speech_rate";
const char *PAYLOAD_KEY_PITCH_RATE = "pitch_rate";
const char *HEADER_VALUE_ASR_NAMESPACE = "SpeechRecognizer";
const char *HEADER_VALUE_ASR_NAME_START = "StartRecognition";
const char *HEADER_VALUE_ASR_NAME_STOP = "StopRecognition";
const char *HEADER_VALUE_ASR_NAME_STARTED = "RecognitionStarted";
const char *HEADER_VALUE_ASR_NAME_RESULT_CHANGED = "RecognitionResultChanged";
const char *HEADER_VALUE_ASR_NAME_COMPLETED = "RecognitionCompleted";
const char *HEADER_VALUE_NAME_TASK_FAILED = "TaskFailed";
const char *HEADER_VALUE_TRANS_NAMESPACE = "SpeechTranscriber";
const char *HEADER_VALUE_TRANS_NAME_START = "StartTranscription";
const char *HEADER_VALUE_TRANS_NAME_STOP = "StopTranscription";
const char *HEADER_VALUE_TRANS_NAME_STARTED = "TranscriptionStarted";
const char *HEADER_VALUE_TRANS_NAME_SENTENCE_BEGIN = "SentenceBegin";
const char *HEADER_VALUE_TRANS_NAME_SENTENCE_END = "SentenceEnd";
const char *HEADER_VALUE_TRANS_NAME_RESULT_CHANGE = "TranscriptionResultChanged";
const char *HEADER_VALUE_TRANS_NAME_COMPLETED = "TranscriptionCompleted";
const char *HEADER_VALUE_TTS_NAMESPACE = "SpeechSynthesizer";
const char *HEADER_VALUE_TTS_NAME_START = "StartSynthesis";
const char *HEADER_VALUE_TTS_NAME_COMPLETED = "SynthesisCompleted";

typedef enum speech_state {
    SPEECH_NO_STAT = 0,
    TRANS_STARTED,
    TRANS_RET_CHANGED,
    TRANS_COMPLETED,   
    SENTENCE_BEGIN,
    SENTENCE_END
} speech_state_e;

#define RINGBUFFER_SIZE (128 * 1024 + 1)
struct aliyun_rasr_priv {
    aui_t *     aui;
    rws_socket  sock;
    aos_event_t event;
    int         task_quit;
    int         status;
    char *      message_id;
    char *      task_id;
    char *      result;
    char *      rbuffer;
    dev_ringbuf_t ringbuffer;
};

static const char *aliyun_rasr_cert = {
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIEaTCCA1GgAwIBAgILBAAAAAABRE7wQkcwDQYJKoZIhvcNAQELBQAwVzELMAkG\r\n"
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n"
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw0xNDAyMjAxMDAw\r\n"
    "MDBaFw0yNDAyMjAxMDAwMDBaMGYxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n"
    "YWxTaWduIG52LXNhMTwwOgYDVQQDEzNHbG9iYWxTaWduIE9yZ2FuaXphdGlvbiBW\r\n"
    "YWxpZGF0aW9uIENBIC0gU0hBMjU2IC0gRzIwggEiMA0GCSqGSIb3DQEBAQUAA4IB\r\n"
    "DwAwggEKAoIBAQDHDmw/I5N/zHClnSDDDlM/fsBOwphJykfVI+8DNIV0yKMCLkZc\r\n"
    "C33JiJ1Pi/D4nGyMVTXbv/Kz6vvjVudKRtkTIso21ZvBqOOWQ5PyDLzm+ebomchj\r\n"
    "SHh/VzZpGhkdWtHUfcKc1H/hgBKueuqI6lfYygoKOhJJomIZeg0k9zfrtHOSewUj\r\n"
    "mxK1zusp36QUArkBpdSmnENkiN74fv7j9R7l/tyjqORmMdlMJekYuYlZCa7pnRxt\r\n"
    "Nw9KHjUgKOKv1CGLAcRFrW4rY6uSa2EKTSDtc7p8zv4WtdufgPDWi2zZCHlKT3hl\r\n"
    "2pK8vjX5s8T5J4BO/5ZS5gIg4Qdz6V0rvbLxAgMBAAGjggElMIIBITAOBgNVHQ8B\r\n"
    "Af8EBAMCAQYwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQUlt5h8b0cFilT\r\n"
    "HMDMfTuDAEDmGnwwRwYDVR0gBEAwPjA8BgRVHSAAMDQwMgYIKwYBBQUHAgEWJmh0\r\n"
    "dHBzOi8vd3d3Lmdsb2JhbHNpZ24uY29tL3JlcG9zaXRvcnkvMDMGA1UdHwQsMCow\r\n"
    "KKAmoCSGImh0dHA6Ly9jcmwuZ2xvYmFsc2lnbi5uZXQvcm9vdC5jcmwwPQYIKwYB\r\n"
    "BQUHAQEEMTAvMC0GCCsGAQUFBzABhiFodHRwOi8vb2NzcC5nbG9iYWxzaWduLmNv\r\n"
    "bS9yb290cjEwHwYDVR0jBBgwFoAUYHtmGkUNl8qJUC99BM00qP/8/UswDQYJKoZI\r\n"
    "hvcNAQELBQADggEBAEYq7l69rgFgNzERhnF0tkZJyBAW/i9iIxerH4f4gu3K3w4s\r\n"
    "32R1juUYcqeMOovJrKV3UPfvnqTgoI8UV6MqX+x+bRDmuo2wCId2Dkyy2VG7EQLy\r\n"
    "XN0cvfNVlg/UBsD84iOKJHDTu/B5GqdhcIOKrwbFINihY9Bsrk8y1658GEV1BSl3\r\n"
    "30JAZGSGvip2CTFvHST0mdCF/vIhCPnG9vHQWe3WVjwIKANnuvD58ZAWR65n5ryA\r\n"
    "SOlCdjSXVWkkDoPWoC209fN5ikkodBpBocLTJIg1MGCUF7ThBCIxPTsvFwayuJ2G\r\n"
    "K1pp74P1S8SqtCr4fKGxhZSM9AyHDPSsQPhZSZg=\r\n"
    "-----END CERTIFICATE-----\r\n"
};

static speech_state_e _check_state(struct aliyun_rasr_priv *priv, const char *json_text)
{
    speech_state_e ret = SPEECH_NO_STAT;
    cJSON *js = NULL;

    if (json_text) {
        js = cJSON_Parse(json_text);
        if (js == NULL) {
            return SPEECH_NO_STAT;
        }
        cJSON *header = cJSON_GetObjectItem(js, HEADER);
        if (!(header && cJSON_IsObject(header))) {
            goto fail;
        }
        cJSON *status = cJSON_GetObjectItem(header, HEADER_KEY_STATUS);
        if (!(status && cJSON_IsNumber(status))) {
            goto fail;
        }
        int stat = status->valueint;
        if (stat != 20000000) {
            LOGE(TAG, "stat: %d", stat);
            goto fail;
        }
        cJSON *name = cJSON_GetObjectItem(header, HEADER_KEY_NAME);
        if (!(name && cJSON_IsString(name))) {
            goto fail;
        }
        cJSON *task_id = cJSON_GetObjectItem(header, HEADER_KEY_TASK_ID);
        if (!(task_id && cJSON_IsString(task_id))) {
            goto fail;
        }
        cJSON *message_id = cJSON_GetObjectItem(header, HEADER_KEY_MESSAGE_ID);
        if (!(message_id && cJSON_IsString(message_id))) {
            
        }
        char *n = name->valuestring;
        if (strcmp(HEADER_VALUE_TRANS_NAME_STARTED, n) == 0) {
            ret = TRANS_STARTED;
            strncpy(priv->task_id, task_id->valuestring, 39);
        } else if (strcmp(HEADER_VALUE_TRANS_NAME_RESULT_CHANGE, n) == 0) {
            ret = TRANS_RET_CHANGED;
        } else if (strcmp(HEADER_VALUE_TRANS_NAME_COMPLETED, n) == 0) {
            ret = TRANS_COMPLETED;
        } else if (strcmp(HEADER_VALUE_TRANS_NAME_SENTENCE_BEGIN, n) == 0) {
            ret = SENTENCE_BEGIN;
        } else if (strcmp(HEADER_VALUE_TRANS_NAME_SENTENCE_END, n) == 0) {
            ret = SENTENCE_END;
        }

        cJSON *payload = cJSON_GetObjectItem(js, PAYLOAD);
        if (payload && cJSON_IsObject(payload)) {
            cJSON *result = cJSON_GetObjectItem(payload, "result");
            if (result && cJSON_IsString(result)) {
                if (priv->result) aos_free(priv->result);
                int len = strlen(result->valuestring) + 64;
                priv->result = aos_zalloc(len);
                snprintf(priv->result, len, "{\"identify\":\"ALIRASR\",\"code\":\"0\",\"data\":\"%s\"}", result->valuestring);
                LOGD(TAG, "==========>result: \n%s", priv->result);
            }
        }

        cJSON_Delete(js);
        return ret;
    }
fail:
    if (js) cJSON_Delete(js);
    return SPEECH_NO_STAT;
}

static int _send_start_cmd(struct aliyun_rasr_priv *priv)
{
#define BUF_SIZE 1024
    int rc;
    char uuid[40];
    char appkey_buf[48];
    char *appkey;
    char *buffer;
    cJSON *root;

    buffer = aos_zalloc_check(BUF_SIZE);

    aliyun_gen_uuid(uuid);
    rc = aos_kv_getstring("aliyun_appkey", appkey_buf, sizeof(appkey_buf));
    appkey = rc > 0 ? appkey_buf : ALIYUN_RASR_APPKEY;

    root = cJSON_CreateObject();
    // header
    cJSON *header = cJSON_CreateObject();
    cJSON_AddStringToObject(header, "namespace", "SpeechTranscriber");
    cJSON_AddStringToObject(header, "appkey", appkey);
    cJSON_AddStringToObject(header, "name", "StartTranscription");
    cJSON_AddStringToObject(header, "message_id", uuid);
    cJSON_AddStringToObject(header, "task_id", uuid);
    cJSON_AddItemToObject(root, "header", header);
    // context
    cJSON *context = cJSON_CreateObject();
    cJSON *sdk = cJSON_CreateObject();
    cJSON_AddStringToObject(sdk, "name", "nls-sdk-python");
    cJSON_AddStringToObject(sdk, "version", "2.0.1");
    cJSON_AddItemToObject(context, "sdk", sdk);
    cJSON_AddItemToObject(root, "context", context);
    // payload
    cJSON *payload = cJSON_CreateObject();
    cJSON_AddStringToObject(payload, "format", "pcm");
    cJSON_AddNumberToObject(payload, "sample_rate", 16000);
    cJSON_AddBoolToObject(payload, "enable_intermediate_result", false);
    cJSON_AddBoolToObject(payload, "enable_punctuation_prediction", false);
    cJSON_AddBoolToObject(payload, "enable_inverse_text_normalization", true);
    cJSON_AddNumberToObject(payload, "max_sentence_silence", 2000);
    cJSON_AddItemToObject(root, "payload", payload);
    // 打印JSON数据包
    char *out = cJSON_Print(root);
    // LOGD(TAG, "send start cmd:\n%s", out);
    rws_bool ret = rws_socket_send_text(priv->sock, out);
    aos_free(buffer);
    cJSON_Delete(root);
    aos_free(out);
    return ret == rws_true ? 0 : -1;
}

static int _send_stop_cmd(struct aliyun_rasr_priv *priv)
{
    int rc;
    char appkey_buf[48];
    char uuid[40];
    char *appkey;
    cJSON *root;

    LOGD(TAG, "%s, %d", __func__, __LINE__);
    rc = aos_kv_getstring("aliyun_appkey", appkey_buf, sizeof(appkey_buf));
    appkey = rc > 0 ? appkey_buf : ALIYUN_RASR_APPKEY;

    aliyun_gen_uuid(uuid);
    root = cJSON_CreateObject();
    // header
    cJSON *header = cJSON_CreateObject();
    cJSON_AddStringToObject(header, "namespace", "SpeechTranscriber");
    cJSON_AddStringToObject(header, "appkey", appkey);
    cJSON_AddStringToObject(header, "name", "StopTranscription");
    cJSON_AddStringToObject(header, "message_id", uuid);
    cJSON_AddStringToObject(header, "task_id", priv->task_id);
    cJSON_AddItemToObject(root, "header", header);

    char *out = cJSON_Print(root);
    // LOGD(TAG, "send stop cmd:\n%s", out);
    rws_bool ret = rws_socket_send_text(priv->sock, out);
    cJSON_Delete(root);
    aos_free(out);
    return ret == rws_true ? 0 : -1;
}

static void _on_received_text(rws_socket socket, const char *text, const unsigned int length, bool is_finish)
{
    speech_state_e ret;
    struct aliyun_rasr_priv *priv = rws_socket_get_user_object(socket);

    if (!(text && length)) {
        LOGE(TAG, "%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }
    LOGD(TAG, "%s, len:%d", __func__, length);
    LOGD(TAG, "text: %s\n", text);
    if (!priv->event.hdl)
        return;

    ret = _check_state(priv, text);
    if (ret == TRANS_STARTED) {
        aos_event_set(&priv->event, EVENT_ASR_SER_OK, AOS_EVENT_OR);
    } else if (ret == SENTENCE_BEGIN) {

    } else if (ret == SPEECH_NO_STAT) {
        aos_event_set(&priv->event, EVENT_ASR_SER_ERR, AOS_EVENT_OR);
        priv->status = 2;
    } else if (ret == TRANS_COMPLETED) {
        aos_event_set(&priv->event, EVENT_ASR_END, AOS_EVENT_OR);
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
    // LOGD(TAG, "received pong!!!!!!!!!!!");
}

static void _on_connected(rws_socket socket)
{
    struct aliyun_rasr_priv *priv = rws_socket_get_user_object(socket);
    LOGD(TAG, "%s: Socket connected", __FUNCTION__);
    if (priv->event.hdl)
        aos_event_set(&priv->event, EVENT_CONNECTED, AOS_EVENT_OR);
}

static void _on_disconnected(rws_socket socket)
{
    struct aliyun_rasr_priv *priv = rws_socket_get_user_object(socket);
    rws_error error = rws_socket_get_error(socket);

    if (error) {
        LOGI(TAG, "%s: Socket disconnect with code, %i, %s",
             __FUNCTION__,
             rws_error_get_code(error),
             rws_error_get_description(error));
    }
    LOGD(TAG, "!!!!!!!!!!%s, %d", __func__, __LINE__);
    if (priv->event.hdl)
        aos_event_set(&priv->event, EVENT_DISCONNECT, AOS_EVENT_OR);
}

static rws_socket _aliyun_ws_new(struct aliyun_rasr_priv *priv)
{
    rws_socket sock = NULL;
    char ars_ip_port[64];
    int asr_port = 443;
    char *token = (char *)aliyun_rasr_get_token();

    if (token == NULL) {
        LOGE(TAG, "token is null");
        return NULL;
    }

    int ret = aos_kv_getstring("asr_server", ars_ip_port, sizeof(ars_ip_port));
    if (ret > 0) {
        char *str_port = strchr(ars_ip_port, ':');
        if (str_port) {
            *str_port = '\0';
            str_port ++;
        }
        asr_port = atoi(str_port);
    } else {
        asr_port = 443;
        strcpy(ars_ip_port, ALIYUN_RASR_HOST);
    }

    CHECK_RET_WITH_RET(priv, NULL);

    sock = rws_socket_create(); // create and store socket handle
    CHECK_RET_WITH_GOTO(sock, err);

    LOGD(TAG, "ws init, path==%s, port:%d", ars_ip_port, asr_port);

    rws_socket_set_url(sock, "wss", ars_ip_port, asr_port, "/ws/v1");
    rws_socket_set_on_disconnected(sock, &_on_disconnected);
    rws_socket_set_on_connected(sock, &_on_connected);
    rws_socket_set_on_received_text(sock, &_on_received_text);
    rws_socket_set_on_received_bin(sock, &_on_received_bin);
    rws_socket_set_on_received_pong(sock, &_on_received_pong);

    rws_socket_set_user_object(sock, priv);
    // token
    sock->token_field = "X-NLS-Token";
    sock->token = token;
    // LOGD(TAG, "token: %s", sock->token);
#ifdef WEBSOCKET_SSL_ENABLE
    rws_socket_set_server_cert(sock, aliyun_rasr_cert, strlen(aliyun_rasr_cert) + 1);
#endif

    rws_socket_connect(sock);

    return sock;
err:
    rws_socket_delete(sock);
    return NULL;
}

static void _aliyun_ws_delete(struct aliyun_rasr_priv *priv)
{
    rws_socket sock = priv->sock;

    rws_socket_disconnect_and_release(sock);
    priv->sock = NULL;
}

static rws_bool ws_send_bin(rws_socket _socket, const char *bin, size_t size)
{
	if (bin && size > 3) {
		int piece_size = size / 3;
		int last_piece_size = piece_size + size % 3;

		rws_bool ret1 = rws_socket_send_bin_start(_socket, &bin[0], piece_size);
		rws_bool ret2 = rws_socket_send_bin_continue(_socket, &bin[piece_size], piece_size);
		rws_bool ret3 = rws_socket_send_bin_finish(_socket, &bin[piece_size * 2], last_piece_size);
        return ret1 && ret2 && ret3;
	}
    return rws_false;
}

// #define PCM_REC_DEBUG
#ifdef PCM_REC_DEBUG
#define PCM_REC_BUF_SIZE (1024 * 400) 
char g_pcm_rec_buf[PCM_REC_BUF_SIZE];
int g_pcm_rec_len = 0;
#endif
static void _send_task(void *arg)
{
#define SLEEP_MS 80
    unsigned int flags = 0;
    struct aliyun_rasr_priv *priv  = (struct aliyun_rasr_priv *)arg;

    priv->status = 0;
    if (!priv->sock) {
#ifdef PCM_REC_DEBUG
        g_pcm_rec_len = 0;
#endif
        priv->sock   = _aliyun_ws_new(priv);
        if (priv->sock == NULL) {
            LOGE(TAG, "websocket create failed...");
            goto out;
        }
        flags = 0;
        aos_event_get(&priv->event, EVENT_CONNECTED, AOS_EVENT_OR_CLEAR, &flags, 3000);
        if (!(flags & EVENT_CONNECTED)) {
            LOGE(TAG, "websocket connect timeout...[0x%x]", flags);
            goto out;
        }
        LOGD(TAG, "websocket connect ok...[0x%x]", flags);
        if (_send_start_cmd(priv)) {
            LOGE(TAG, "send start cmd failed...");
            goto out;
        }
        flags = 0;
        aos_event_get(&priv->event, EVENT_ASR_SER_OK | EVENT_DISCONNECT | EVENT_ASR_SER_ERR, AOS_EVENT_OR_CLEAR,
                        &flags, 10000);
        if (flags & EVENT_DISCONNECT) {
            LOGE(TAG, "websocket disconnect ...[0x%x]", flags);
            goto out;
        }
        else if (flags & EVENT_ASR_SER_ERR) {
            LOGE(TAG, "asr ser error ...[0x%x]", flags);
            goto out;
        }
        else if (flags & EVENT_ASR_SER_OK) {
            LOGD(TAG, "asr ser ok...");
            priv->status = 1;            
        }
        else {
            LOGW(TAG, "wait asr ok timeout");
        }
    }
    if (priv->status == 0) {
        LOGE(TAG, "ws not conn or some errors, %d", priv->status);
        goto out;
    }
  
    while (!priv->task_quit && priv->status == 1) {
        rws_bool ret;
        char *data = NULL;
        int size = 0;

        if (rws_socket_is_connected(priv->sock) == rws_false) {
            LOGE(TAG, "socket connect failed, push e..");
            priv->status = 2;
            continue;
        }
        size = ringbuffer_available_read_space(&priv->ringbuffer);
        // LOGD(TAG, "available read size: %d", size);
        if (size >= SLEEP_MS * 32) {
            size = SLEEP_MS * 32;
        }
        if (size == 0) {
            aos_event_set(&priv->event, EVENT_PCM_EMPTY, AOS_EVENT_OR);
            priv->task_quit = 1;
            aos_msleep(100);
        }
        else if (size > 0) {
            data = aos_malloc(size);
            ringbuffer_read(&priv->ringbuffer, (uint8_t *)data, size);
            ret = ws_send_bin(priv->sock, data, size);
            if (ret == rws_false) {
                priv->status = 2;
                LOGE(TAG, "snd bin failed!!");
            }
            // LOGD(TAG, "snd bin, size:%d, ret:%d", size, ret);
#ifdef PCM_REC_DEBUG
            if (g_pcm_rec_len + size < PCM_REC_BUF_SIZE) {
                memcpy(g_pcm_rec_buf + g_pcm_rec_len, data, size);
                g_pcm_rec_len += size;
            }
#endif
            aos_free(data);               
        }
        aos_msleep(SLEEP_MS);
    }
out:
    LOGD(TAG, "snd-task quit...");
    aos_event_set(&priv->event, EVENT_TASK_QUIT | EVENT_PCM_EMPTY, AOS_EVENT_OR);
    aos_task_exit(0);
}

static int aliyun_rasr_init(aui_t *aui)
{
    struct aliyun_rasr_priv *priv;

    priv = aos_zalloc_check(sizeof(struct aliyun_rasr_priv));
    
    aui->context = priv;
    priv->aui = aui;
    priv->status = 0;
    priv->message_id = aos_zalloc_check(40);
    priv->task_id = aos_zalloc_check(40);
    priv->result = NULL;
    aos_event_new(&priv->event, 0);
    priv->rbuffer = aos_zalloc_check(RINGBUFFER_SIZE);
    priv->ringbuffer.buffer = NULL;

    return 0;
}

static int aliyun_rasr_deinit(aui_t *aui)
{
    (void)aui;
    return 0;
}

static int aliyun_rasr_start(aui_t *aui)
{
    (void)aui;
    return 0;
}

static int aliyun_rasr_source_pcm(aui_t *aui, void *data, size_t size)
{
    struct aliyun_rasr_priv *priv;

    if (!(aui && aui->context && data && size > 0)) {
        return -1;
    }

    priv = aui->context;
    if (priv->ringbuffer.buffer == NULL) {
        memset(priv->rbuffer, 0, RINGBUFFER_SIZE);
        ringbuffer_create(&priv->ringbuffer, priv->rbuffer, RINGBUFFER_SIZE);
        aos_event_new(&priv->event, 0);
        priv->task_quit = 0;
        aos_task_t task;
        aos_task_new_ext(&task, "aliyun-snd", _send_task, (void *)priv, 4096, AOS_DEFAULT_APP_PRI - 1);
    }
    if (priv->status == 2) {
        LOGE(TAG, "snd bin already failed...");
        return -1;
    }
    
    ringbuffer_write(&priv->ringbuffer, (uint8_t *)data, size);
    aos_event_set(&priv->event, EVENT_PCM_DATA, AOS_EVENT_OR);

    return 0;
}

static int aliyun_rasr_source_pcm_finish(aui_t *aui)
{
    rws_bool ret = rws_false;
    struct aliyun_rasr_priv *priv;
    unsigned int flags = 0;

    if (!(aui && aui->context)) {
        return -1;
    }
    LOGD(TAG, "%s, %d", __func__, __LINE__);

    priv = aui->context;
    
    if (priv->ringbuffer.buffer == NULL) {
        return -1;
    }
    if (priv->sock && rws_socket_is_connected(priv->sock)) {
        aos_event_get(&priv->event, EVENT_PCM_EMPTY, AOS_EVENT_OR_CLEAR, &flags, -1);
        if (priv->status != 0) {
            _send_stop_cmd(priv);
            aos_event_get(&priv->event, EVENT_ASR_END, AOS_EVENT_OR_CLEAR, &flags, 5000);
            if (!(flags & EVENT_ASR_END)) {
                LOGE(TAG, "EVENT_ASR_END timeout...");
                ret = rws_false;
            } else {
                if (aui && aui->config.nlp_cb) {
                    if (priv->result)
                        aui->config.nlp_cb(priv->result);
                    else
                        aui->config.nlp_cb("{\"error\":\"aliyun rasr failed\"}");
                }
                ret = rws_true;
            }
        }              
    } else {
        LOGE(TAG, "[socket e]push finish e....");
        ret = rws_false;            
    }
    priv->status = 0;
    priv->task_quit = 1;
    aos_event_set(&priv->event, ~0 & ~EVENT_TASK_QUIT, AOS_EVENT_OR);
    aos_event_get(&priv->event, EVENT_TASK_QUIT, AOS_EVENT_OR_CLEAR, &flags, -1);
    aos_event_free(&priv->event);
    ringbuffer_destroy(&priv->ringbuffer);
    if (priv->result) {
        aos_free(priv->result);
        priv->result = NULL;
    }
    _aliyun_ws_delete(priv);
             
    LOGI(TAG, "### pcm finish ...");

    return ret == rws_false ? -1 : 0;
}

static aui_asr_cls_t aliyun_asr_cls = {
    .init = aliyun_rasr_init,
    .deinit = aliyun_rasr_deinit,
    .start = aliyun_rasr_start,
    .push_data = aliyun_rasr_source_pcm,
    .stop_push_data = aliyun_rasr_source_pcm_finish,
};

void aui_asr_register_aliyun(aui_t *aui)
{
    if (aui) {
        aui->ops.asr = &aliyun_asr_cls;
    }
}