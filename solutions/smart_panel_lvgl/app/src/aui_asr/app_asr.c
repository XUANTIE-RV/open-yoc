/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifdef CONFIG_ALG_ASR_LYEVA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>
#include <cJSON.h>
#include <yoc/mic.h>
#include "player/app_player.h"
#include "aui_action/aui_action.h"
#include "app_asr.h"

#define TAG "appasr"

static aos_timer_t asr_timer = NULL;
int g_continue_talk_mode = 0;

const char *asrTable[]={
    //灯光
    "CTL_ACC_LIGHT_ON_OFF",
    "CTL_ACC_LIGHT_BRIGHTNESS_SET",
    "CTL_ACC_LIGHT_LIVING_ROOM",
    "CTL_ACC_LIGHT_BED_ROOM",
    //窗帘
    "CTL_ACC_SCREEN_ON_OFF",
    //音箱
    "CTL_ACC_PLAYER_VOLUMN_CHANGE",
    "CTL_ACC_PLAYER_MUTE_UNMUTE",
    "CTL_ACC_PLAYER_PLAY_MUSIC",
    "CTL_ACC_PLAYER_PAUSE_RESUME",
    "CTL_ACC_PLAYER_MUSIC_CHANGE",
    "CTL_ACC_PLAYER_VOLUMN_SET",
    //场景
    "CTL_ACC_SCENE",
};

asr_action_t asrAction[]={
    //灯光
    aui_action_iot_light_onoff_ctrl,
    aui_action_iot_light_brightness_ctrl,
    aui_action_iot_living_room_light_ctrl,
    aui_action_iot_bed_room_light_ctrl,
    //窗帘
    aui_action_iot_curtain_ctrl,
    //音箱
    aui_action_player_volume_change,
    aui_action_player_mute_ctrl,
    aui_action_player_local_play,
    aui_action_player_play_ctrl,
    aui_action_player_switch_song,
    aui_action_player_volumn_set,
    //场景
    aui_action_iot_mode_ctrl,
};
static int asrTableLen=sizeof(asrTable)/sizeof(char*);
static asr_action_t tempAsrAction=NULL;
static void *tempAsrPara=NULL;

static cJSON *cJSON_GetObjectItemByPath(cJSON *object, const char *path)
{
    if (object == NULL) {
        return NULL;
    }

    char *dup_path = strdup(path);
    char *saveptr  = NULL;
    char *next     = strtok_r(dup_path, ".", &saveptr);

    cJSON *item = object;
    while (next) {
        char *arr_ptr = strchr(next, '[');
        if (arr_ptr) {
            *arr_ptr++ = '\0';
            item = cJSON_GetObjectItem(item, next);
            if (item) {
                item = cJSON_GetArrayItem(item, atoi(arr_ptr));
            }
        } else {
            item = cJSON_GetObjectItem(item, next);
        }
        if (item == NULL) {
            break;
        }

        next = strtok_r(NULL, ".", &saveptr);
    }

    free(dup_path);

    return item;
}

//返回值：int ,tempAsrAction,tempAsrPara
//fix me：没必要封装到结构体吧，解析和调用action都在这个源文件
int asr(cJSON *js){
    cJSON *intent = cJSON_GetObjectItemByPath(cJSON_GetArrayItem(js, 0), "payload.semantics[0].intent");
    if(cJSON_IsString(intent)){
        //遍历一维数组
        int i=0;
        for(;i<asrTableLen;i++){
            if(strcmp(intent->valuestring, asrTable[i]) == 0){
                break;
            }
        }
        if(i==asrTableLen){
            return -1;
        }
        else{
            tempAsrAction=asrAction[i];
            cJSON *value=cJSON_GetObjectItemByPath(cJSON_GetArrayItem(js, 0), "payload.semantics[0].slots.SLOT_ACTION[0].value");
            if(cJSON_IsString(value)){
                tempAsrPara=(void*)value->valuestring;
            }
            return 0;
        }
    }
    else{
        //fixe me：有哪些异常值
        return -1;
    }
}

static void asr_timeout(void *arg1, void *arg2)
{
    LOGD(TAG, "asr timeout ,disable asr");
    aui_mic_control(MIC_CTRL_ENABLE_ASR, 0);
    local_audio_play("npl_nothing.mp3");
}

void asr_process_init()
{
    aos_timer_new_ext(&asr_timer, asr_timeout, NULL, 10000, 0, 0);
    aui_mic_control(MIC_CTRL_ENABLE_ASR, 1);
}

void asr_process_session_start(mic_kws_t *wk_info)
{
    aui_mic_control(MIC_CTRL_ENABLE_ASR, 1);
    if (g_continue_talk_mode == 0) {
        aos_timer_stop(&asr_timer);
        aos_timer_start(&asr_timer);
        LOGD(TAG, "wakeup mode");
    } else if (g_continue_talk_mode == 1) {
        LOGD(TAG, "continue mode");
    }
}

void app_aui_shortcutCMD_offline(const char *resultCMD)
{
    cJSON *js = cJSON_Parse(resultCMD);
    
    //应用逻辑
    int ret = asr(js);
    LOGD(TAG, "shortcutCMD:ret=%d", ret);
    if(ret==0&&tempAsrAction){
        tempAsrAction(tempAsrPara);
        tempAsrAction=NULL;
    }
    else{
        local_audio_play("npl_confused.mp3");
    }
    //麦克风控制逻辑
    cJSON *asrResult = cJSON_GetObjectItemByPath(cJSON_GetArrayItem(js, 0), "payload.asrresult");
    if(cJSON_IsString(asrResult)){
        LOGD(TAG, "asrresult=%s\n", asrResult->valuestring);
        if (0 == strcmp(asrResult->valuestring, "打开连续对话")) {
            g_continue_talk_mode = 1;
            aos_timer_stop(&asr_timer);
            aui_mic_control(MIC_CTRL_ENABLE_ASR, 1);
        } else if (0 == strcmp(asrResult->valuestring, "关闭连续对话")) {
            g_continue_talk_mode = 0;
            aui_mic_control(MIC_CTRL_ENABLE_ASR, 0);
        }
    }

    cJSON_Delete(js);

    if (g_continue_talk_mode == 0) {
        aui_mic_control(MIC_CTRL_ENABLE_ASR, 0);
        aos_timer_stop(&asr_timer);
        LOGD(TAG, "disable asr");
    }
}

#endif
