/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */
#include <smart_audio.h>
#include <ulog/ulog.h>
#include <cJSON.h>
#include "aui_action/aui_action.h"
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
#include "aui_cloud/app_aui_cloud.h"
#endif

#define TAG "playeraction"
#define VOLUME_STEP 10

/*
二百三十六，忽略权值“百”和“十”，依次提取2,3,6
权值“十”的前后可能没有数字，需要特殊处理
十二，先补充成一十二，提取1,2
三十，先补充成三十零，提取3,0
其他权值的前面一定有数字：没有百二十的说法
其他权值的后面可能没有数字：一百，一千
通过测试：[0,111]
*/
int getNum(const char*str){
    //GB2312 2个字节，utf-8 3个字节
    static int codeLen=strlen("一");
    int ret=0;
    int len=strlen(str);
    //提取1
    if(strncmp(str,"十",codeLen)==0){
        ret=1;
    }
    for(int i=0;i<len;i+=codeLen){
        if(strncmp(str+i,"零",codeLen)==0){
            ret=ret*10+0;
        }
        else if(strncmp(str+i,"一",codeLen)==0){
            ret=ret*10+1;
        }
        else if(strncmp(str+i,"二",codeLen)==0){
            ret=ret*10+2;
        }
        else if(strncmp(str+i,"三",codeLen)==0){
            ret=ret*10+3;
        }
        else if(strncmp(str+i,"四",codeLen)==0){
            ret=ret*10+4;
        }
        else if(strncmp(str+i,"五",codeLen)==0){
            ret=ret*10+5;
        }
        else if(strncmp(str+i,"六",codeLen)==0){
            ret=ret*10+6;
        }
        else if(strncmp(str+i,"七",codeLen)==0){
            ret=ret*10+7;
        }
        else if(strncmp(str+i,"八",codeLen)==0){
            ret=ret*10+8;
        }
        else if(strncmp(str+i,"九",codeLen)==0){
            ret=ret*10+9;
        }
    }
    //提取0
    if(strncmp(str+len-codeLen,"十",codeLen)==0){
        ret=ret*10;
    }
    else if(strncmp(str+len-codeLen,"百",codeLen)==0){
        ret=ret*100;
    }
    else if(strncmp(str+len-codeLen,"千",codeLen)==0){
        ret=ret*1000;
    }
  return ret;
}


//任何时候：我要听歌，播放file:///mnt/alibaba1.mp3 cur_num=0
//没有放歌时，下一首，上一首，播放file:///mnt/alibaba1.mp3  cur_num=0
//已经在放歌时，下一首，上一首，基于在放的歌曲进行切换        cur_num++/-- 
//切歌时，应该调用stop，不然有线程在等resume
static char *urlList[]={
    "file:///mnt/alibaba1.mp3",
    "file:///mnt/alibaba2.mp3",
    "file:///mnt/alibaba3.mp3",
};
static int cur_num=0;

/* 静音控制：静音/取消静音 */
void aui_action_player_mute_ctrl(void *para){
    char *arg=(char*)para;
    if(strcmp(arg,"mute")==0){
        aui_action_player_mute(NULL);
    }
    else{
        aui_action_player_unmute(NULL);
    }
}

/* 静音 */
void aui_action_player_mute(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    //重复说静音 
    if (smtaudio_get_state() != SMTAUDIO_STATE_MUTE){
        //fixe me :应该说好的，再进入静音状态；但是好的得等云端返回tts
        smtaudio_mute();
    }
}

/* 取消静音：恢复播放/好的,“你好芯宝”没有提示音 */
//之前放歌但是又暂停了，静音再取消静音说好的，恢复节点的状态保持在0，再说继续播放可以恢复
/*
测试用例：
1 “静音”->“取消静音”->提示“好的”
2 “我要听歌”->“静音”->"取消静音"->恢复播放
3 "我要听歌"->“暂停播放”->提示“好的”->"静音"    ->“取消静音”->提示“好的”->“继续播放”->恢复播放
*/
void aui_action_player_unmute(void *para){
    //int flag=0;
    LOGD(TAG, "%s", __FUNCTION__);
    if(smtaudio_get_state() == SMTAUDIO_STATE_MUTE){
        //不能用ctrl_online_music.status来判断：我要听歌->暂停->静音->取消静音
        /*
        if(!smtaudio_check_resume_list_by_id(MEDIA_MUSIC)){
            flag=1;
        }
        smtaudio_unmute();
        if(flag){    
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
            app_aui_cloud_tts_run("好的", 0);
#else
            smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
        }
        */
        smtaudio_unmute();
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
        app_aui_cloud_tts_run("好的", 0);
#else
        smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
    }
}

/* 改变音量：high/low */
void aui_action_player_volume_change(void *para){
    char *arg=(char*)para;
    if(strcmp(arg,"high")==0){
        aui_action_player_volume_up(NULL);
    }
    else{
        aui_action_player_volume_down(NULL);
    }
}


/* 增加音量 */
void aui_action_player_volume_up(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    smtaudio_vol_up((int16_t)VOLUME_STEP);
    //之前没放歌（包括播放后暂停），说好的；之前在放歌，直接恢复播放
    /*
    if(smtaudio_get_state_by_id(MEDIA_MUSIC)==SMTAUDIO_STATE_PAUSE && smtaudio_check_resume_list_by_id(MEDIA_MUSIC)){
        smtaudio_resume();
    }
    else{
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
            app_aui_cloud_tts_run("好的", 0);
#else
            smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
    }
    */
    //好的（恢复播放）
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}

/* 减少音量：减少到0之后，“你好芯宝”没有提示音，“增加音量”，语音提示“好的” */
void aui_action_player_volume_down(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    smtaudio_vol_down((int16_t)VOLUME_STEP);
    //之前没放歌（包括播放后暂停），说好的；之前在放歌，直接恢复播放
    /*
    if(smtaudio_get_state_by_id(MEDIA_MUSIC)==SMTAUDIO_STATE_PAUSE && smtaudio_check_resume_list_by_id(MEDIA_MUSIC)){
        smtaudio_resume();
    }
    else{
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
            app_aui_cloud_tts_run("好的", 0);
#else
            smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
    }
    */
    //好的（恢复播放）
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}

/* 设置音量 :"音量调到五十六"->"五十六"->"56" */
void aui_action_player_volumn_set(void *para){
    int volumn=getNum((char*)para);
    LOGD(TAG, "%s:%d", __FUNCTION__,volumn);
    if(volumn<0){
        volumn=0;
    }
    else if(volumn>100){
        volumn=100;
    }
    smtaudio_vol_set((int16_t)volumn);
    //之前没放歌（包括播放后暂停），说好的；之前在放歌，直接恢复播放
/*
    if(smtaudio_get_state_by_id(MEDIA_MUSIC)==SMTAUDIO_STATE_PAUSE && smtaudio_check_resume_list_by_id(MEDIA_MUSIC)){
        smtaudio_resume();
    }
    else{
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
        app_aui_cloud_tts_run("好的", 0);
#else
        smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
    }
*/
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif    
}

/* 切歌：next/prev */
void aui_action_player_switch_song(void *para){
    char *arg=(char*)para;
    if(strcmp(arg,"next")==0){
        aui_action_player_next(NULL);
    }
    else{
        aui_action_player_prev(NULL);
    }
}

/* 下一首 */
void aui_action_player_next(void *para){
    int next_num=0;
    if(smtaudio_get_state_by_id(MEDIA_MUSIC)==SMTAUDIO_STATE_PAUSE){
        char *url=smtaudio_get_url_by_id(MEDIA_MUSIC);
        if(url){
            if (strcmp(url, urlList[0]) == 0){
                next_num=1;
            }
            else if (strcmp(url, urlList[1]) == 0){
                next_num=2;
            }
            else if (strcmp(url, urlList[2]) == 0){
                next_num=0;
            }
        }
        else{
            next_num=0;
        }
    }
    else{
        next_num=0;
    }
    cur_num=next_num;
    smtaudio_stop(MEDIA_ALL);
    LOGD(TAG, "%s: play_url= \"%s\"", __FUNCTION__, urlList[cur_num]);
    smtaudio_start(MEDIA_MUSIC, urlList[cur_num], 0, 0);
}

/* 上一首 */
void aui_action_player_prev(void *para){
    int prev_num=0;
    if(smtaudio_get_state_by_id(MEDIA_MUSIC)==SMTAUDIO_STATE_PAUSE){
        char *url=smtaudio_get_url_by_id(MEDIA_MUSIC);
        if(url){
            if (strcmp(url, urlList[0]) == 0){
                prev_num=2;
            }
            else if (strcmp(url, urlList[1]) == 0){
                prev_num=0;
            }
            else if (strcmp(url, urlList[2]) == 0){
                prev_num=1;
            }
        }
        else{
            prev_num=0;
        }
    }
    else{
        prev_num=0;
    }
    cur_num=prev_num;
    smtaudio_stop(MEDIA_ALL);
    LOGD(TAG, "%s: play_url= \"%s\"", __FUNCTION__, urlList[cur_num]);
    smtaudio_start(MEDIA_MUSIC, urlList[cur_num], 0, 0);
}

/* 播放器控制：pause/resume */
void aui_action_player_play_ctrl(void *para){
    char *arg=(char*)para;
    if(strcmp(arg,"pause")==0){
        aui_action_player_pause(NULL);
    }
    else{
        aui_action_player_resume(NULL);
    }
}

/* 暂停播放 */
void aui_action_player_pause(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    smtaudio_enable_ready_list(0);
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}

/* 继续播放 */
void aui_action_player_resume(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    if(smtaudio_get_state_by_id(MEDIA_MUSIC)==SMTAUDIO_STATE_PAUSE){
        smtaudio_enable_ready_list(1);
        int ret=smtaudio_resume();
        if(ret==-1){
            smtaudio_start(MEDIA_MUSIC, urlList[cur_num], 0, 0);
        }
    }
    else{
        smtaudio_start(MEDIA_MUSIC, urlList[cur_num], 0, 0);
    }
}

/* 停止播放 */
void aui_action_player_stop(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    smtaudio_stop(MEDIA_ALL);
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}

/* 播放本地音乐 */
void aui_action_player_local_play(void *para){
    smtaudio_stop(MEDIA_ALL);
    LOGD(TAG, "%s: play_url= \"%s\"", __FUNCTION__, "file:///mnt/alibaba1.mp3");
    smtaudio_start(MEDIA_MUSIC, "file:///mnt/alibaba1.mp3", 0, 0);
}

/* 播放云端音乐 */
void aui_action_player_online_play(void *para){
    char *url=(char*)para;
    LOGD(TAG, "%s: play_url= \"%s\"", __FUNCTION__, url);
    smtaudio_start(MEDIA_MUSIC, url, 0, 0);
}

/* tts */
void aui_action_player_tts(void *para){
    char *spoken_text=(char*)para;
    LOGD(TAG, "%s: tts_text= \"%s\"", __FUNCTION__, spoken_text);
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run(spoken_text, 0);
#endif
}