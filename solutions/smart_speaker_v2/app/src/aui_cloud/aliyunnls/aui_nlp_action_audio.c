/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <sys/time.h>
#include <smart_audio.h>

#include "aui_nlp.h"
#include "ex_cjson.h"

#define TAG "nlpaudio"

/**
 * 解析ASR&NLP信息
 */
int aui_nlp_action_set_volume(cJSON *js)
{
    int ret = -1;

    cJSON *action_params = cJSON_GetObjectItemByPath(js, "payload.action_params");
    if (cJSON_IsArray(action_params)) {
        cJSON *param      = NULL;
        cJSON *param_name = NULL, *param_value = NULL;
        char  *action   = NULL;
        int    vol_step = -1;
        int    vol_set  = -1;

        cJSON_ArrayForEach(param, action_params)
        {
            if (cJSON_HasObjectItem(param, "name")) {
                param_name = cJSON_GetObjectItemCaseSensitive(param, "name");
                if (json_string_eq(param_name, "sound") && cJSON_HasObjectItem(param, "value")) {
                    param_value = cJSON_GetObjectItemCaseSensitive(param, "value");
                    action      = param_value->valuestring;
                    LOGD(TAG, "nlp volume: action [%s]", action);
                }

                if (json_string_eq(param_name, "degree") && cJSON_HasObjectItem(param, "value")) {
                    param_value = cJSON_GetObjectItemCaseSensitive(param, "value");
                    vol_step    = atoi(param_value->valuestring);
                    LOGD(TAG, "nlp volume: degree [%d]", vol_step);
                }

                if (json_string_eq(param_name, "volumn") && cJSON_HasObjectItem(param, "value")) {
                    param_value = cJSON_GetObjectItemCaseSensitive(param, "value");
                    vol_set     = atoi(param_value->valuestring);
                    LOGD(TAG, "nlp volume: volumn [%d]", vol_set);
                }
            }
        }

        if (strcmp(action, "up") == 0) {
            vol_step = vol_step < 0 ? 10 : vol_step;
            LOGD(TAG, "nlp_action:volume up %d", vol_step);
            smtaudio_vol_up((int16_t)vol_step);
            ret = 0;
        } else if (strcmp(action, "down") == 0) {
            vol_step = vol_step < 0 ? 10 : vol_step;
            LOGD(TAG, "nlp_action:volume down %d", vol_step);
            smtaudio_vol_down((int16_t)vol_step);
            ret = 0;
        } else if (strcmp(action, "max") == 0) {
            LOGD(TAG, "nlp_action:volume max");
            smtaudio_vol_set(100);
            ret = 0;
        } else if (strcmp(action, "min") == 0) {
            LOGD(TAG, "nlp_action:volume min");
            smtaudio_vol_set(0);
            ret = 0;
        } else if (strcmp(action, "mute") == 0) {
            smtaudio_enable_ready_list(0);
            ret = 0;
        } else if (strcmp(action, "unmute") == 0) {
            smtaudio_enable_ready_list(1);
            smtaudio_resume();
            ret = 0;
        } else if (strcmp(action, "set") == 0) {
            if (vol_set >= 0) {
                LOGD(TAG, "nlp_action:volume set %d", vol_set);
                smtaudio_vol_set((int16_t)vol_set);
                ret = 0;
            } else {
                // fix cloud bug, set action to "unmute"
                LOGD(TAG, "nlp volume: action -> umute");
                smtaudio_enable_ready_list(1);
                int resume_result = smtaudio_resume();
                if(resume_result != -1) {
                    ret = AUI_NLP_STATE_PLAYING;
                } else {
                    ret = 0;
                }
            }
        }
    } else {
        LOGE(TAG, "set_volume payload error!");
        return -1;
    }

    return ret;
}

char *aui_nlp_action_get_music_url(cJSON *js)
{
    cJSON *action_params_name = cJSON_GetObjectItemByPath(js, "payload.action_params[0].name");
    if (cJSON_IsString(action_params_name) && json_string_eq(action_params_name, "listenFile")) {
        cJSON *action_params_value = cJSON_GetObjectItemByPath(js, "payload.action_params[0].value");
        if (cJSON_IsString(action_params_value) && cJSON_IsString(action_params_value)) {
            return action_params_value->valuestring;
        }
    }

    return NULL;
}
