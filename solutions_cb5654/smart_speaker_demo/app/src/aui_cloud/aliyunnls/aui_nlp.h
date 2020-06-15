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

int json_string_eq(cJSON *js, const char *str);

/* ASR文本传入进行命令处理 */
int aui_nlp_proc_textcmd(const char *json_text);

/* nlp action */
int aui_nlp_action_set_volume(cJSON *js);
int aui_nlp_action_play_pause(void);
int aui_nlp_action_play_resume(void);

int aui_nlp_proc_mit(cJSON *js, const char *json_text);

#endif
