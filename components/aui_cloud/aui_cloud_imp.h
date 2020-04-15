/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __CLOUD_H__
#define __CLOUD_H__

#include <string.h>
#include <yoc/aui_cloud.h>
#include <webclient.h>

typedef enum {
    AUISRV_TYPE_INVALID = 0,
    AUISRV_TYPE_ASR     = (1 << 0),
    AUISRV_TYPE_NLP     = (1 << 1),
    AUISRV_TYPE_ASR_NLP = (AUISRV_TYPE_ASR | AUISRV_TYPE_NLP),
    AUISRV_TYPE_TTS     = (1 << 2),
} aiservice_type_t;

typedef enum aui_cloud_err_code {
    AUI_ERR_FAIL = -1,
    AUI_ERR_OK = 0,
    AUI_ERR_HTTP,
    AUI_ERR_MEM,
    AUI_ERR_PARAM,
    AUI_ERR_JSON,
}aui_cloud_err_code_t;

/* baidu proxy */
#define BAIDU_ASR_IP "ai.c-sky.com"

#define BAIDU_ASR_PORT 8001

/* aliyun proxy */
#define ALIYUN_ASR_IP "ai.c-sky.com"
#define ALIYUN_ASR_PORT 8090

/* aliyun */
#define ALIYUN_RASR_HOST    "nls-gateway.cn-shanghai.aliyuncs.com"
#define ALIYUN_RASR_URL     "wss://nls-gateway.cn-shanghai.aliyuncs.com/ws/v1"
#define ALIYUN_RASR_APPKEY      "xMVGxGX3VD9TDrZk"
#define ALIYUN_RASR_ACCESS_ID   "LTAIBnAs6m5MrUYD"
#define ALIYUN_RASR_ACCESS_SEC  "HXXWT08q6EUyr8ribobgOafMDCOSOr"

#define HTTP_BUFFER_SIZE (10 * 1024)
#define HTTP_HEADER_SIZE (1024)

#define XF_APPID "5bce8799"

/** appid 1 */
#define XF_TTS_APPID1 "5bce8799"
#define XF_TTS_API_KEY1 "48ffe85c5b5126f3d9d16524d3fc6301"
/** appid 2 */
#define XF_TTS_APPID2 "5bc00620"
#define XF_TTS_API_KEY2 "d5eb840b2d3b7d2606a9d26fcc6fc86e"

/* */
#define XF_RASR_APPID   "5d904b1b"
#define XF_RASR_API_KEY "e41436cb42253e0b7ea78e9f9b0cf613"

#define XF_ASR_API_KEY "ceb70c7974634ce69b4b4827acd83287"
#define XF_ASR_POST_URI "http://openapi.xfyun.cn/v2/aiui"
#define XF_TTS_POST_URI "http://api.xfyun.cn/v1/service/v1/tts"
#define XF_RASR_BASE_HOST "rtasr.xfyun.cn"

#define XF_TTS_TXT_SIZE_MAX (1024)



#define XF_NLP_PARAM_FORMATX                                                                       \
    "{\"result_level\":\"complete\",\"auth_id\":\"2049a1b2fdedae553bd03ce6f4820ac4\",\"data_"      \
    "type\":\"text\",\"scene\":\"main\",\"lat\":\"%s\",\"lng\":\"%s\"}"
#define XF_ASR_PARAM_FORMATX                                                                       \
    "{\"result_level\":\"plain\",\"aue\":\"raw\",\"auth_id\":"                                     \
    "\"2049a1b2fdedae553bd03ce6f4820ac4\",\"data_type\":\"audio\",\"sample_rate\":\"16000\","      \
    "\"scene\":\"main\",\"lat\":\"%s\",\"lng\":\"%s\"}"
#define XF_TTS_PARAM_FORMATX                                                                        \
    "{\"aue\":\"raw\",\"auf\":\"audio/"                                                            \
    "L16;rate=16000\",\"voice_name\":\"xiaoyan\",\"engine_type\":\"intp65\"}"

char *xfyun_rasr_get_handshake_params(void);
int webclient_build_http_header(aui_t *aui, struct webclient_session *session, 
                                 aiservice_type_t ai_type, int content_length);
int webclient_read_response(struct webclient_session *session, char *buffer, int buffer_size);
int http_url_encode(char *output, const char *input);

void xfyun_tts_config_json_gen(aui_t *aui, char **json_out);

void xfyun_asr_init();
int xfyun_asr_source_pcm_finish(aui_t *aui);
int xfyun_asr_source_pcm(char *data, int size);

void xfyun_rasr_init(aui_t *aui);
int xfyun_rasr_source_pcm(aui_t *aui, char *data, int size);
int xfyun_rasr_source_pcm_finish(aui_t *aui);

void baidu_asr_init(aui_t *aui);
int baidu_asr_source_pcm_finish(aui_t *aui);
int baidu_asr_source_pcm(aui_t *aui, char *data, int size);

void proxy_ws_asr_init(aui_t *aui);
int proxy_ws_asr_source_pcm(aui_t *aui, char *data, int size);
int proxy_ws_asr_source_pcm_finish(aui_t *aui);

/* aliyun rasr */
const char *aliyun_rasr_get_token(void);
void aliyun_gen_uuid(char uuid[33]);
void aliyun_rasr_init(aui_t *aui);
int aliyun_rasr_source_pcm(aui_t *aui, char *data, int size);
int aliyun_rasr_source_pcm_finish(aui_t *aui);

/** process baidu music */
#define MUSIC_PREFIX "music:"
#define MUSIC_URL_NLP  "{\"music::url\" : \"%s\"}"
int baidu_music(aui_t *aui, char *text);
int qq_music(aui_t *aui, char *text);

int xfyun_nlp(aui_t *aui, char *text_in);
int xfyun_tts(aui_t *aui, const char* name, char *txt_data);
char *xfyun_tts_get_appid();
char *xfyun_tts_get_apikey();
void xfyun_tts_init();

#endif
