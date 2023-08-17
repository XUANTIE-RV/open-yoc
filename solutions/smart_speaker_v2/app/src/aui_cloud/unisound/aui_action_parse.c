/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <smart_audio.h>
#include <ulog/ulog.h>
#include "aui_cloud/ex_cjson.h"
#include "aui_action/aui_action.h"

#define TAG "actionparse"

//离线命令词：若干中文命令词，对应同一个英文意图。
//云之声方案，离线的识别不了，才会送到云端，所以两张表没有重合的地方。

#define MUTE_ENABLE 0

const char *asrTable_offline[]={
#if MUTE_ENABLE
    "music_mute",//静音
    "music_unmute",//取消静音
#endif
    "music_vol_up",//增加音量|音量调高               
    "music_vol_dw",//降低音量|音量调低
    "music_play",//我要听歌|播放歌曲|播放一首歌曲
    "music_pause",//暂停播放
    "music_resume",//继续播放
    "music_pre",//上一首|播放上一首
    "music_next",//下一首|播放下一首
};
asr_action_t asrAction_offline[]={
#if MUTE_ENABLE
    aui_action_player_mute,
    aui_action_player_unmute,
#endif    
    aui_action_player_volume_up,
    aui_action_player_volume_down,
    aui_action_player_local_play,
    aui_action_player_pause,
    aui_action_player_resume,
    aui_action_player_next,
    aui_action_player_prev,
};
static int asrTableLen_offline=sizeof(asrTable_offline)/sizeof(char*);


static asr_action_t tempAsrAction=NULL;
static void *tempAsrPara=NULL;

//返回值：int ,tempAsrAction,tempAsrPara
//fix me：没必要封装到结构体吧，解析和调用action都在这个源文件
int asr_offline(cJSON *js){
    cJSON *intent = cJSON_GetObjectItemByPath(js, "payload.semantics[0].intent");
    if(cJSON_IsString(intent)){
        //遍历一维数组
        int i=0;
        for(;i<asrTableLen_offline;i++){
            if(strcmp(intent->valuestring, asrTable_offline[i]) == 0){
                break;
            }
        }
        if(i==asrTableLen_offline){
            return -1;
        }
        else{
            tempAsrAction=asrAction_offline[i];
            return 0;
        }
    }
    else{
        //fixe me：有哪些异常值
        return -1;
    }
}

int asr(cJSON *js){
    cJSON *intent = cJSON_GetObjectItemByPath(js, "asr_recongize");
    if(cJSON_IsString(intent)){
        LOGD(TAG, "asr_text=%s", intent->valuestring);
    }
    return 0; 
}

//离线与在线结合
//1 离线可以识别时，调用action
//2 离线无法识别时，根据云端识别结果调用补充的action，但是目前仅仅播放“没明白，换个说法试试吧？”
void app_aui_shortcutCMD_offline(const char *resultCMD){
    LOGD(TAG, "shortcutCMD_offline:json=%s", resultCMD);

    cJSON *js = cJSON_Parse(resultCMD);
    int ret = asr_offline(js);
    LOGD(TAG, "shortcutCMD_offline:ret=%d", ret);
    if(ret==0&&tempAsrAction){
        tempAsrAction(tempAsrPara);
        tempAsrAction=NULL;
    }
    cJSON_Delete(js);
}

void app_aui_shortcutCMD(cJSON *object, const char * resultCMD){
    LOGD(TAG, "shortcutCMD:json=%s", resultCMD);

    cJSON *js = cJSON_Parse(resultCMD);
    int ret = asr(js);
    LOGD(TAG, "shortcutCMD:ret=%d", ret);
    
    //云端现在没有补充的action：本地识别失败时，“没明白，换个说法试试吧？”，之后可能会恢复播放
    cJSON *text = cJSON_GetObjectItemByPath(js, "general.text");
    if(cJSON_IsString(text)){
        aui_action_player_tts((void*)text->valuestring);
    }
    //唤醒之后，什么都不说：{\"asr_recongize\":\"\",\"returnCode\":0}
    else{
        //aui_action_player_tts("对不起，没听清楚，请再说一次");
    }

    cJSON_Delete(js);
}

void app_aui_nlpEnd(const char * data){
    LOGD(TAG, "nlpEnd:json=%s", data);
}