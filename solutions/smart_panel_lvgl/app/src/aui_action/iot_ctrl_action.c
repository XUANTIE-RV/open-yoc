/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */
#include <aos/kv.h>
#include <smart_audio.h>
#include <gateway.h>
#include <ulog/ulog.h>
#include <cJSON.h>
#include "src/misc/lv_printf.h"
#include "aui_action/aui_action.h"
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
#include "aui_cloud/app_aui_cloud.h"
#endif

#define TAG "iotaction"

//影音模式tx_num：5
#define MODE_TX_NUM  5
#define ACT_OPEN     0
#define ACT_CLOSE    1
#define ACT_BRIVALUE 2

typedef struct mode_cfg {
    const int  mode;//components/alg_asr_lyeva/nlu/asr_nlu.c：asrPara，模式枚举，1,2,3
    struct {
        const char *operands;
        const int   operator;
        const int   value;
    } subdevCtrl[MODE_TX_NUM];
} mode_cfg_t;

static const mode_cfg_t subdevMode[]={
    //离家：关灯关窗帘 
    { 
        .mode=1,
        .subdevCtrl={
            {
                .operands="客厅灯",
                .operator=ACT_CLOSE,
                .value=1,
            },
            {
                .operands="卧室灯",
                .operator=ACT_CLOSE,
                .value=1,
            },
            {
                .operands="窗帘",
                .operator=ACT_CLOSE,
                .value=1,
            }
        }
    },
    //睡眠：关灯关窗帘  
    {
        .mode=2,
        .subdevCtrl={
            {
                .operands="客厅灯",
                .operator=ACT_CLOSE,
                .value=1,
            },
            {
                .operands="卧室灯",
                .operator=ACT_CLOSE,
                .value=1,
            },
            {
                .operands="窗帘",
                .operator=ACT_CLOSE,
                .value=1,
            }
        },
    },
    //影音：开灯调亮度关窗帘  
    {
        .mode=3,
        .subdevCtrl={
            {
                .operands="客厅灯",
                .operator=ACT_OPEN,
                .value=1,
            },
            {
                .operands="客厅灯",
                .operator=ACT_BRIVALUE,
                .value=20,
            },
            {
                .operands="卧室灯",
                .operator=ACT_OPEN,
                .value=1,
            },
            {
                .operands="卧室灯",
                .operator=ACT_BRIVALUE,
                .value=20,
            },
            {
                .operands="窗帘",
                .operator=ACT_CLOSE,
                .value=1,
            }
        },
    },
    //回家：开灯开窗帘 
    {
        .mode=4,
        .subdevCtrl={
            {
                .operands="客厅灯",
                .operator=ACT_OPEN,
                .value=1,
            },
            {
                .operands="卧室灯",
                .operator=ACT_OPEN,
                .value=1,
            },
            {
                .operands="窗帘",
                .operator=ACT_OPEN,
                .value=1,
            }
        },
    }
};
static int modeLen=sizeof(subdevMode)/sizeof(mode_cfg_t);
    
//kv get index1：第一个入网设备的名字
static char kv_pos[32];
static char kv_index[10];

/* 灯光开关控制：开灯/关灯 */
void aui_action_iot_light_onoff_ctrl(void *para){
    char *arg=(char*)para;
    if(strcmp(arg,"on")==0){
        aui_action_iot_open_light(NULL);
    }
    else{
        aui_action_iot_close_light(NULL);
    }
}
/* 开灯 */
void aui_action_iot_open_light(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    for(int i=1;i<11;i++){
        memset(kv_index, 0, sizeof(kv_index));
        memset(kv_pos, 0, sizeof(kv_pos));
        lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if (ret >= 0) {
            if (strcmp(kv_pos, "客厅灯") == 0||strcmp(kv_pos, "卧室灯") == 0){
                gateway_subdev_set_onoff(i, atoi("1"));
            }
        }
    }
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}
/* 关灯 */
void aui_action_iot_close_light(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    for(int i=1;i<11;i++){
        memset(kv_index, 0, sizeof(kv_index));
        memset(kv_pos, 0, sizeof(kv_pos));
        lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if (ret >= 0) {
            if (strcmp(kv_pos, "客厅灯") == 0||strcmp(kv_pos, "卧室灯") == 0){
                gateway_subdev_set_onoff(i, atoi("0"));
            }
        }
    }
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}

/* 灯光亮度控制：亮度 */
void aui_action_iot_light_brightness_ctrl(void *para){
    int brightness=getNum((char*)para);
    LOGD(TAG, "%s:%d", __FUNCTION__,brightness);
    if(brightness>=0&&brightness<=100){
        for(int i=1;i<11;i++){
            memset(kv_index, 0, sizeof(kv_index));
            memset(kv_pos, 0, sizeof(kv_pos));
            lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
            int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
            if (ret >= 0) {
                if (strcmp(kv_pos, "客厅灯") == 0||strcmp(kv_pos, "卧室灯") == 0){
                    gateway_subdev_set_brightness(i, brightness);
                }
            }
        }
    }
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}



/* 客厅灯控制：打开客厅灯/关闭客厅灯 */
void aui_action_iot_living_room_light_ctrl(void *para){
    char *arg=(char*)para;
    if(strcmp(arg,"on")==0){
        aui_action_iot_open_living_room_light(NULL);
    }
    else{
        aui_action_iot_close_living_room_light(NULL);
    }
}
/* 打开客厅灯 */
void aui_action_iot_open_living_room_light(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    for(int i=1;i<11;i++){
        memset(kv_index, 0, sizeof(kv_index));
        memset(kv_pos, 0, sizeof(kv_pos));
        lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if (ret >= 0) {
            if (strcmp(kv_pos, "客厅灯") == 0){
                gateway_subdev_set_onoff(i, atoi("1"));
                break;
            }
        }
    }
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}
/* 关闭客厅灯 */
void aui_action_iot_close_living_room_light(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    for(int i=1;i<11;i++){
        memset(kv_index, 0, sizeof(kv_index));
        memset(kv_pos, 0, sizeof(kv_pos));
        lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if (ret >= 0) {
            if (strcmp(kv_pos, "客厅灯") == 0){
                gateway_subdev_set_onoff(i, atoi("0"));
                break;
            }
        }
    }
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}



/* 卧室灯控制：打开卧室灯/关闭卧室灯 */
void aui_action_iot_bed_room_light_ctrl(void *para){
    char *arg=(char*)para;
    if(strcmp(arg,"on")==0){
        aui_action_iot_open_bed_room_light(NULL);
    }
    else{
        aui_action_iot_close_bed_room_light(NULL);
    }
}
/* 打开卧室灯 */
void aui_action_iot_open_bed_room_light(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    for(int i=1;i<11;i++){
        memset(kv_index, 0, sizeof(kv_index));
        memset(kv_pos, 0, sizeof(kv_pos));
        lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if (ret >= 0) {
            if (strcmp(kv_pos, "卧室灯") == 0){
                gateway_subdev_set_onoff(i, atoi("1"));
                break;
            }
        }
    }
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}
/* 关闭卧室灯 */
void aui_action_iot_close_bed_room_light(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    for(int i=1;i<11;i++){
        memset(kv_index, 0, sizeof(kv_index));
        memset(kv_pos, 0, sizeof(kv_pos));
        lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if (ret >= 0) {
            if (strcmp(kv_pos, "卧室灯") == 0){
                gateway_subdev_set_onoff(i, atoi("0"));
                break;
            }
        }
    }
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}



/* 窗帘控制：打开窗帘/关闭窗帘 */
void aui_action_iot_curtain_ctrl(void *para){
    char *arg=(char*)para;
    if(strcmp(arg,"on")==0){
        aui_action_iot_open_curtain(NULL);
    }
    else{
        aui_action_iot_close_curtain(NULL);
    }
}
/* 打开窗帘 */
void aui_action_iot_open_curtain(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    for(int i=1;i<11;i++){
        memset(kv_index, 0, sizeof(kv_index));
        memset(kv_pos, 0, sizeof(kv_pos));
        lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if (ret >= 0) {
            if (strcmp(kv_pos, "窗帘") == 0){
                gateway_subdev_set_onoff(i, atoi("1"));
                break;
            }
        }
    }
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}
/* 关闭窗帘 */
void aui_action_iot_close_curtain(void *para){
    LOGD(TAG, "%s", __FUNCTION__);
    for(int i=1;i<11;i++){
        memset(kv_index, 0, sizeof(kv_index));
        memset(kv_pos, 0, sizeof(kv_pos));
        lv_snprintf(kv_index, sizeof(kv_index), "index%d", i);
        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
        if (ret >= 0) {
            if (strcmp(kv_pos, "窗帘") == 0){
                gateway_subdev_set_onoff(i, atoi("0"));
                break;
            }
        }
    }
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}


/* 模式控制 */
void aui_action_iot_mode_ctrl(void *para){
    int mode=atoi((char*)para);
    LOGD(TAG, "%s:%d", __FUNCTION__,mode);

    for(int i=0;i<modeLen;i++){
        if(subdevMode[i].mode==mode){
            for(int j=0;j<MODE_TX_NUM;j++){
                if(subdevMode[i].subdevCtrl[j].operands){
                    for(int k=1;k<11;k++){
                        memset(kv_index, 0, sizeof(kv_index));
                        memset(kv_pos, 0, sizeof(kv_pos));
                        lv_snprintf(kv_index, sizeof(kv_index), "index%d", k);
                        int ret = aos_kv_getstring(kv_index, kv_pos, sizeof(kv_pos));
                        if (ret >= 0) {
                            if (strcmp(kv_pos,subdevMode[i].subdevCtrl[j].operands) == 0){
                                if(subdevMode[i].subdevCtrl[j].operator==ACT_OPEN){
                                    gateway_subdev_set_onoff(k, atoi("1"));
                                }
                                else if(subdevMode[i].subdevCtrl[j].operator==ACT_CLOSE){
                                    gateway_subdev_set_onoff(k, atoi("0"));
                                }
                                else if(subdevMode[i].subdevCtrl[j].operator==ACT_BRIVALUE){
                                    gateway_subdev_set_brightness(k, subdevMode[i].subdevCtrl[j].value);
                                }
                                break;
                            }
                        }
                    }
                }
                else{
                    break;
                }
            }
            break;
        }
    }

#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_tts_run("好的", 0);
#else
    smtaudio_start(MEDIA_SYSTEM, "file:///mnt/well.mp3", 0, 1);
#endif
}
/* 离家模式：关灯关窗帘 */
void aui_action_iot_away_mode(void *para){
    aui_action_iot_mode_ctrl((void*)"1");
}
/* 睡眠模式：关灯关窗帘 */
void aui_action_iot_sleep_mode(void *para){
    aui_action_iot_mode_ctrl((void*)"2");
}
/* 影音模式：开灯调亮度关窗帘 */
void aui_action_iot_video_mode(void *para){
    aui_action_iot_mode_ctrl((void*)"3");
}
/* 回家模式：开灯开窗帘 */
void aui_action_iot_home_mode(void *para){
    aui_action_iot_mode_ctrl((void*)"4");
}