/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __YOC_MEDIA_H__
#define __YOC_MEDIA_H__

#include <aos/list.h>
#include <yoc/uservice.h>
#include <avutil/eq_typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************
 * 音频服务
 * type:
 *  0: 音乐播放
 *  1: 提示音播放
 *      每一个 type 只有一个播放实例，新的播放会替换旧的播放实例，
 *      提示音的播放会先暂停音乐播放，播放完成之后继续音乐播放
 *************************************************/

typedef enum {
    MEDIA_MUSIC  = 0, /*音乐播放*/
    MEDIA_SYSTEM = 1, /*通知播放*/
    MEDIA_ALL    = 255,
} aui_player_type_t;

typedef enum {
    AUI_PLAYER_UNKNOWN,
    AUI_PLAYER_STOP,
    AUI_PLAYER_PLAYING,
    AUI_PLAYER_PAUSED
} aui_player_state_t;

typedef enum {
    AUI_PLAYER_EVENT_ERROR,
    AUI_PLAYER_EVENT_START,
    AUI_PLAYER_EVENT_FINISH
} aui_player_evtid_t;

typedef int (*media_key_cb_t)(const void *in, size_t ilen, void *out, size_t *olen);

typedef struct {
    uint32_t  resample_rate;
    uint32_t  web_cache_size;      ///< size of the web cache. 0 use default
    uint32_t  web_start_threshold; ///< (0~100)start read for player when up to cache_start_threshold. 0 use default
    uint32_t  snd_period_ms;       ///< period cache size(ms) for audio out. 0 means use default
    uint32_t  snd_period_num;      ///< number of period_ms. total cache size for ao is (period_num * period_ms * (rate / 1000) * 2 * (16/8)). 0 means use default

} aui_player_config_t;

typedef struct {
    uint64_t  duration;      ///< ms, maybe a dynamic time
    uint64_t  curtime;       ///< ms, current time
} aui_play_time_t;

/**
 * 播放器事件用户处理函数
 *
 * @param evt_id
 */
typedef void (*media_evt_t)(int type, aui_player_evtid_t evt_id);

/**
 * 获取指定播放器的状态
 *
 * @param task 指定播放器服务的utask
 *        如果==NULL，播放器自己创建utask
 * @return 0:成功
 */
int aui_player_init(utask_t *task, media_evt_t evt_cb);

/**
 * 播放音乐，强制停止已经在播的音乐
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM
 * @param url 媒体资源
 *        file:///fatfs0/Music/1.mp3  SD卡中的音频文件
 *        http://.../1.mp3            http音频
 *        fifo://tts/1           播放云端反馈的tts流
 *        mem://addr=%u&size=%u       播放存放在ram中资源
 * @param seek_time   播放偏移时间
 * @param resume   自动重启音乐(0：手动 1：自动)
 * @return 0:成功
 */
int aui_player_play(int type, const char *url, int resume);
int aui_player_seek_play(int type, const char *url, uint64_t seek_time, int resume);

/**
 * 暂停播放
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL
 * @return 0:成功
 */
int aui_player_pause(int type);

/**
 * 暂停状态继续播放和静音状态恢复播放
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,
 * @return 0:成功
 */
int aui_player_resume(int type);

/**
 * 停止播放器
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL
 * @return 0:成功
 */
int aui_player_stop(int type);

/**
 * 播放器seek
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL
 * @param seek_time seek时间，单位ms
 * @return 0:成功
 */
int aui_player_seek(int type, uint64_t seek_time);

/**
 * 播放器静音,调用aui_player_resume接口恢复音频输出
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL
 * @return 0:成功
 */
int aui_player_mute(int type);

/**
 * 调整音量
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,,MEDIA_ALL
 * @param inc_volume 正数加大，负数减小
 * @return 0:成功
 */
int aui_player_vol_adjust(int type, int inc_volume);

/**
 * 调整音量到指定值
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL
 * @param volume 指定音量的百分比 0~100
 * @return 0:成功
 */
int aui_player_vol_set(int type, int volume);

/**
 * 渐变调整音量到指定值
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,
 * @param new_volume 目标音量百分比0~100
 * @param ms 渐变时间
 * @return 0:成功
 */
int aui_player_vol_gradual(int type, int new_volume, int ms);

/**
 * 获取当前音量值
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM
 * @return 音量值
 */
int aui_player_vol_get(int type);

/**
 * 设置播放时最小音量
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL
 * @param volume 指定音量的百分比 0~100
 * @return 0:成功
 */
int aui_player_set_minvol(int type, int volume);

/**
 * 获取指定播放器的状态
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM
 * @return aui_player_state_t
 */
aui_player_state_t aui_player_get_state(int type);

/**
 * 通知播放完成后恢复音乐播放(有音量渐变效果)
 *
 * @return 0:成功
 */
int aui_player_resume_music(void);

/**
 * 配置参数
 *
 * @return 0:成功
 */
int aui_player_config(aui_player_config_t *config);

/**
 * 配置参数
 *
 * @param eq_segments EQ段配置数组
 * @param count 段个数
 * @return 0:成功
 */
int aui_player_eq_config(eqfp_t *eq_segments, int count);

/**
 * 配置获取密钥回调
 *
 * @param cb 密钥回调
 * @return 0:成功
 */
int aui_player_key_config(media_key_cb_t cb);

/**
 * 获取播放时间
 *
 * @param type 支持MEDIA_MUSIC,MEDIA_SYSTEM
 * @param t    播放时长
 * @return 0:成功
 */
int aui_player_get_time(int type, aui_play_time_t *t);

#ifdef __cplusplus
}
#endif

#endif
