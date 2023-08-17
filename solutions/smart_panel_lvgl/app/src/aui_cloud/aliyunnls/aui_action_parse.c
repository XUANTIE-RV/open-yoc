/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <yoc/mic.h>
#include <smart_audio.h>
#include <ulog/ulog.h>
#include "aui_cloud/ex_cjson.h"
#include "aui_action/aui_action.h"
#include "player/app_player.h"

#define TAG "actionparse"

#define ASR_GENE_NUM 8

static const char *asrTable[][ASR_GENE_NUM]={
    /*-----------------------------------------------子设备控制-----------------------------------------------*/
    //控制单个子设备:单个控制 
    {"打开客厅灯","开客厅灯","打开客厅登","开客厅登"},
    {"关闭客厅灯","关客厅灯","关闭客厅登","关客厅登"},
    {"打开卧室灯","开卧室灯","打开卧室登","打开卧式登","打开卧式灯","开卧室登","开卧式登","开卧式灯"},
    {"关闭卧室灯","关卧室灯","关闭卧室登","关闭卧式登","关闭卧式灯","关卧室登","关卧式登","关卧式灯"},
    {"开窗帘","打开窗帘","请打开窗帘"},
    {"关窗帘","关闭窗帘","请关闭窗帘"},

    //控制多个子设备:分组控制   
    {"开灯","打开灯","打开照明灯","请打开照明灯","请打开灯","请开灯"},
    {"关灯","关闭灯","关闭照明灯","请关闭照明灯","请关闭灯","请关灯"},

    //控制多个子设备：模式控制 
    {"离家模式","打开离家模式","设置离家模式"},
    {"睡眠模式","打开睡眠模式","设置睡眠模式"},
    {"电影模式","影音模式","打开影音模式","设置影音模式"},
    {"回家模式","打开回家模式","设置回家模式"},

    /*------------------------------------------------网关控制------------------------------------------------*/
    //音乐播放器
    {"下一首","下一首音乐"},
    {"上一首","上一首音乐"},
    {"暂停","暂停音乐","暂停播放"},
    {"继续播放"},
    {"停止","停止音乐","停止播放"},
    {"我要听歌"},
};
asr_action_t asrAction[]={
    /*-----------------------------------------------子设备控制-----------------------------------------------*/
    //控制单个子设备:单个控制 
    aui_action_iot_open_living_room_light,
    aui_action_iot_close_living_room_light,
    aui_action_iot_open_bed_room_light,
    aui_action_iot_close_bed_room_light,
    aui_action_iot_open_curtain,
    aui_action_iot_close_curtain,

    //控制多个子设备:分组控制 
    aui_action_iot_open_light,
    aui_action_iot_close_light,

    //控制多个子设备：模式控制 
    aui_action_iot_away_mode,
    aui_action_iot_sleep_mode,
    aui_action_iot_video_mode,
    aui_action_iot_home_mode,

    /*------------------------------------------------网关控制------------------------------------------------*/
    //音乐播放器
    aui_action_player_next,
    aui_action_player_prev,
    aui_action_player_pause,
    aui_action_player_resume,
    aui_action_player_stop,
    aui_action_player_local_play,
};
static int asrTableLen=sizeof(asrTable)/sizeof(char*[ASR_GENE_NUM]);
static asr_action_t tempAsrAction=NULL;
static void *tempAsrPara=NULL;

/*
static const char *nlpTable[][ASR_GENE_NUM]={
    {"我要听歌"},
    {"杭州天气"},
};
static int nlpTableLen=sizeof(nlpTable)/sizeof(char*[ASR_GENE_NUM]);
*/
asr_action_t nlpAction[]={
    aui_action_player_online_play,
    aui_action_player_tts,
};
static nlp_action_t tempNlpAction=NULL;
static void *tempNlpPara=NULL;

/*
一次交互会有asr和nlp两次事件，但两次事件的task id相同，
如果asr处理是对该值赋值，说明要忽略后续的nlp处理。
*/
static char g_nlp_task_id[40] = {0};


//-1 : 无法匹配到可处理的技能
//-2 : 解析有错误
//-3": 网络异常
//-4 : 意图不支持  //asr有识别结果，但在asrTable中没有匹配的命令词
//-10: 其他错误    
int asr(cJSON *js){
    bool knownCmd=false;
    cJSON *aui_result = cJSON_GetObjectItemByPath(js, "aui_result");
    if (cJSON_IsNumber(aui_result)) {
        LOGD(TAG, "asr:aui_result=%d", aui_result->valueint);
        if (aui_result->valueint == -101) {
            return -3;
        } else {
            return -1;
        }
    }  
    cJSON *name = cJSON_GetObjectItemByPath(js, "header.name");
    if (cJSON_IsString(name)) {
        LOGD(TAG, "asr:name=%s", name->valuestring);
    }
    else{
        return -10;
    }
    //asr
    if (strcmp(name->valuestring, "RecognitionCompleted") == 0) {
        cJSON *cmd = cJSON_GetObjectItemByPath(js, "payload.result");
        if (cJSON_IsString(cmd)) {
            LOGD(TAG, "asr:asr_text=%s", cmd->valuestring);
            //遍历二维数组
            int i=0,j=0;
            for(i=0;i<asrTableLen;i++){
                for(j=0;j<ASR_GENE_NUM;j++){
                    if(asrTable[i][j]&&strcmp(cmd->valuestring, asrTable[i][j]) == 0){
                        knownCmd=true;
                        break;
                    }
                }
                if(knownCmd){
                    break;
                }
            }
            //忽略后面的NLP处理
            if(knownCmd){
                cJSON *task_id = cJSON_GetObjectItemByPath(js, "header.task_id");
                if (cJSON_IsString(task_id)) {
                    strncpy(g_nlp_task_id, task_id->valuestring, sizeof(g_nlp_task_id) - 1);
                    g_nlp_task_id[sizeof(g_nlp_task_id) - 1] = 0;
                }
                tempAsrAction=asrAction[i];
                return 0;//唯一成功的路径
            }
            else{
                return -4;
                //fix me：当前对-4的异常处理用第一种方法
                /*
                asr有识别结果，但在asrTable中没有匹配的命令词，可以有三种异常处理
                1 播一段云端的tts，不忽略第二次的nlp返回
                2 页面播放自定义的提示音：
                    g_nlp_task_id=task_id;//忽略第二次的nlp返回，不播放tts
                    页面shortcutCMD的回调函数中增加一种异常处理
                    if(ret.value==-4){
                    smta.play("file:///mnt/XXX.mp3",1,1);
                    }
                3 没有任何反应，没有tts也没有提示音
                    g_nlp_task_id=task_id;//没有tts
                    页面没有==-4的判断;//没有提示音
                */ 
            }
        }
        else{
            LOGD(TAG, "asr:RecognitionCompleted but no cmd!");
            return -2;
        }
    }
    else{
        return -1;
    }
}

//nlp不做异常处理，仅有-2和0两种返回
int nlp(cJSON *js){
    cJSON *task_id = cJSON_GetObjectItemByPath(js, "header.task_id");
    if (json_string_eq(task_id, g_nlp_task_id)) {
        g_nlp_task_id[0] = '\0';
        LOGD(TAG, "nlp:ignore");
        return -2;
    }
    cJSON *action = cJSON_GetObjectItemByPath(js, "payload.action");
    cJSON *spoken_text = cJSON_GetObjectItemByPath(js, "payload.spoken_text");
    if(cJSON_IsString(action)){
        LOGD(TAG, "nlp:nlp_action=%s",action->valuestring);
    }
    //保留在线播放的功能
    //虽然有spoken_text="好的，请听我唱小鸭子"，但是既然已经在放歌了，就不必放tts了
    if(cJSON_IsString(action)&&strcmp(action->valuestring, "play_music") == 0){
        cJSON *temp = cJSON_GetObjectItemByPath(js, "payload.action_params[0].name");
        cJSON *music_url = cJSON_GetObjectItemByPath(js, "payload.action_params[0].value");
        if(cJSON_IsString(temp)&&cJSON_IsString(music_url)&&strcmp(temp->valuestring, "listenFile") == 0){
            LOGD(TAG, "nlp:play_music url=%s", music_url->valuestring);
            tempNlpAction=nlpAction[0];
            tempNlpPara=(void*)music_url->valuestring;
            return 0;
        }
        else{
            return -2;
        }
    }
    else if(cJSON_IsString(spoken_text)){
        LOGD(TAG, "nlp:spoken_text=%s", spoken_text->valuestring);
        tempNlpAction=nlpAction[1];
        tempNlpPara=(void*)spoken_text->valuestring;
        return 0;
    }
    else{
        LOGD(TAG, "nlp:好的");
        tempNlpAction=nlpAction[1];
        tempNlpPara=(void*)"好的";
        return 0;
    }
}

//asr + 异常处理
void app_aui_shortcutCMD(cJSON *object, const char * resultCMD){
    LOGD(TAG, "shortcutCMD:json=%s", resultCMD);

    cJSON *js = cJSON_Parse(resultCMD);
    int ret = asr(js);
    LOGD(TAG, "shortcutCMD:ret=%d", ret);

    /* 识别返回，云端断句 */
    if (ret == -1 || ret == -3) {
        //云端识别错误，断句
        aui_mic_control(MIC_CTRL_START_SESSION, 0);
    } else {
        //识别完成，断句
        cJSON *name = cJSON_GetObjectItemByPath(js, "header.name");
        if (name) {
            if (strcmp(name->valuestring, "RecognitionCompleted") == 0) {
                aui_mic_control(MIC_CTRL_START_SESSION, 0);
            } else {
                LOGE(TAG, "unknown nlp error");
            }
        }
    }
    //（当前对-4的异常处理，播放一段云端的tts）
    if (ret == -1) {
        local_audio_play("npl_nothing.mp3"); 
    } else if (ret == -2) {
        local_audio_play("npl_nothing.mp3");
    } else if (ret == -3) {
        local_audio_play("npl_nothing.mp3");  
    } else if (ret == -10) {
        local_audio_play("npl_nothing.mp3");   
    } else {
        ;
    }

    if(ret==0&&tempAsrAction){
        tempAsrAction(tempAsrPara);
        tempAsrAction=NULL;
    }
 
    cJSON_Delete(js);
}

//nlp only
void app_aui_nlpEnd(const char * data){
    LOGD(TAG, "nlpEnd:json=%s", data);

    cJSON *js = cJSON_Parse(data);
    int ret = nlp(js);
    LOGD(TAG, "nlpEnd:ret=%d", ret);

    if(ret==0&&tempNlpAction){
        tempNlpAction(tempNlpPara);
        tempNlpAction=NULL;
    }
 
    cJSON_Delete(js);
}

void app_aui_shortcutCMD_offline(const char *resultCMD){
    LOGD(TAG, "shortcutCMD_offline:json=%s", resultCMD);
}