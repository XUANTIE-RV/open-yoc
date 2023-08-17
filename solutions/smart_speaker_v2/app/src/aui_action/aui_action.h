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



typedef void(*asr_action_t)(void *);
typedef void(*nlp_action_t)(void *);

//两种恢复风格
//播放音乐 -> “增加音量” ->不说“好的”，直接播放的声音更大 ;没有播放音乐时，才说“好的”
//播放音乐 -> “增加音量” ->“好的”->播放的声音更大
//所有解决方案，都用第二种风格