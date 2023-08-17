/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */
#include <cJSON.h>

//返回：asr action 和 nlp action 暂时没有返回值
//入参：asr action 和 nlp action 需要的参数可能是int ,char*,double
// 一 都定义成cJSON  判断cJSON_IsString/cJSON_IsNumber后使用
// 二 都定义成void*  不同接口内部使用不同的强制转换


/*
 * 播放器相关执行动作
 */
/* 静音 */
void aui_action_player_mute(void *para);
/* 取消静音 */
void aui_action_player_unmute(void *para);
/* 静音控制：静音/取消静音 */
void aui_action_player_mute_ctrl(void *para);

/* 改变音量：high/low */
void aui_action_player_volume_change(void *para);
/* 增加音量 */
void aui_action_player_volume_up(void *para);
/* 减少音量 */
void aui_action_player_volume_down(void *para);
/* 设置音量 :"音量调到五十六"->"五十六"->"56" */
void aui_action_player_volumn_set(void *para);

/* 切歌：next/prev */
void aui_action_player_switch_song(void *para);
/* 下一首 */
void aui_action_player_next(void *para);
/* 上一首 */
void aui_action_player_prev(void *para);

/* 播放器控制：pause/resume */
void aui_action_player_play_ctrl(void *para);
/* 暂停播放 */
void aui_action_player_pause(void *para);
/* 继续播放 */
void aui_action_player_resume(void *para);

/* 停止播放 */
void aui_action_player_stop(void *para);

/* 播放本地音乐 */
void aui_action_player_local_play(void *para);
/* 播放云端音乐 */
void aui_action_player_online_play(void *para);

/* tts */
void aui_action_player_tts(void *para);


void app_aui_shortcutCMD(cJSON *object, const char * resultCMD);
void app_aui_nlpEnd(const char * data);

/*
 * IoT设备控制
 */
/* 开灯 */
void aui_action_iot_open_light(void *para);
/* 关灯 */
void aui_action_iot_close_light(void *para);
/* 灯光开关控制：开灯/关灯 */
void aui_action_iot_light_onoff_ctrl(void *para);
/* 灯光亮度控制：亮度 */
void aui_action_iot_light_brightness_ctrl(void *para);


/* 打开客厅灯 */
void aui_action_iot_open_living_room_light(void *para);
/* 关闭客厅灯 */
void aui_action_iot_close_living_room_light(void *para);
/* 客厅灯控制：打开客厅灯/关闭客厅灯 */
void aui_action_iot_living_room_light_ctrl(void *para);

/* 打开卧室灯 */
void aui_action_iot_open_bed_room_light(void *para);
/* 关闭卧室灯 */
void aui_action_iot_close_bed_room_light(void *para);
/* 卧室灯控制：打开卧室灯/关闭卧室灯 */
void aui_action_iot_bed_room_light_ctrl(void *para);

/* 打开窗帘 */
void aui_action_iot_open_curtain(void *para);
/* 关闭窗帘 */
void aui_action_iot_close_curtain(void *para);
/* 窗帘控制：打开窗帘/关闭窗帘 */
void aui_action_iot_curtain_ctrl(void *para);


/* 离家模式 */
void aui_action_iot_away_mode(void *para);
/* 睡眠模式 */
void aui_action_iot_sleep_mode(void *para);
/* 影音模式 */
void aui_action_iot_video_mode(void *para);
/* 回家模式 */
void aui_action_iot_home_mode(void *para);
/* 模式控制：离家/睡眠/影音/回家 */
void aui_action_iot_mode_ctrl(void *para);

//一百二十三->123
int getNum(const char*str);

typedef void(*asr_action_t)(void *);
typedef void(*nlp_action_t)(void *);

/*
带屏的需要考虑和界面同步的问题，因此有两种实现action的方案

1 直接调用smtaudio_start，
  然后发送一个事件去更新页面的显示，类比smart_panel_miniapp中的$falcon.trigger或者jsapi的音乐事件
  可以新增事件，也可以用EVENT_MEDIA_START

2 间接调用smtaudio_start，等价于按了一下界面的播放按钮 
  smart_panel_lvgl/app/src/aui_cloud/aliyunnls/aui_nlp_proc_music.c：
  app_event_update(EVENT_GUI_USER_MUSIC_PLAY);
  lv_event_send(play_obj, LV_EVENT_CLICKED, NULL);       
  play_obj_event_click_cb
  smtaudio_start
  现在可以通过法二完成功能的，只有上一首，下一首；（语音的暂停，播放和按键的暂停播放逻辑不一样）
  
  综上：都用法一

模仿smart_panel_miniapp的风格：
1 按键或者语音，只是调用smtaudio_start等接口
2 在EVENT_MEDIA_START等事件的回调函数中统一更新页面的显示

---------------------------------------------------------

网关操作，也可以改成这种风格：
在mesh_scr_devctl_listbtn_event_cb中只调用gateway_subdev_set_onoff
在gw_main_event_handler  EVENT_GUI_USER_MESH_STATUS_ONOFF_UPDATE事件中更新界面显示
*/