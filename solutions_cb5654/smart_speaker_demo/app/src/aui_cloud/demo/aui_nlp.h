#ifndef _AUI_NLP_H_
#define _AUI_NLP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/log.h>
#include <yoc/aui_cloud.h>
#include <cJSON.h>
#include <media.h>
#include "audio_res.h"
#include "app_player.h"
#include "app_aui_cloud.h"

cJSON *xunfei_nlp_find(cJSON *array, const char *key, const char *value);
cJSON *xunfei_nlp_find_service(cJSON *service_array, const char *service_name);
cJSON *xunfei_nlp_get_first_intent(cJSON *js);

int aui_nlp_process_pre_check(cJSON *js, const char *json_text);
int aui_nlp_process_music_nlp(cJSON *js, const char *json_text);
int aui_nlp_process_music_url(cJSON *js, const char *json_text);
int aui_nlp_proc_cb_tts_nlp(cJSON *js, const char *json_text);
int aui_nlp_process_cmd(cJSON *js, const char *json_text);
int aui_nlp_process_xf_rtasr(cJSON *js, const char *json_text);
int aui_nlp_proc_cb_textcmd(cJSON *js, const char *json_text);
int aui_nlp_proc_cb_story_nlp(cJSON *js, const char *json_text);
int aui_nlp_process_baidu_asr(cJSON *js, const char *json_text);
int aui_nlp_process_proxy_ws_asr(cJSON *js, const char *json_text);

#endif
