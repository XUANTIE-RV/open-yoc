/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <yoc_config.h>

#include <stdlib.h>
#include <string.h>
#include <aos/log.h>

#include "app_main.h"
#include "audio/audio_res.h"

#define TAG "app"

void app_text_cmd_init(void)
{
    aui_textcmd_matchinit(12, ",");

    /*增加音量*/
    aui_textcmd_matchadd("volume_plus", "音量大");
    aui_textcmd_matchadd("volume_plus", "音量高");
    aui_textcmd_matchadd("volume_plus", "声音大");
    aui_textcmd_matchadd("volume_plus", "声音响");

    aui_textcmd_matchadd("volume_plus", "增加音量");
    aui_textcmd_matchadd("volume_plus", "增大音量");
    aui_textcmd_matchadd("volume_plus", "增大声音");

    /*减小音量*/
    aui_textcmd_matchadd("volume_minus", "音量小");
    aui_textcmd_matchadd("volume_minus", "音量低");
    aui_textcmd_matchadd("volume_minus", "声音小");
    aui_textcmd_matchadd("volume_minus", "声音轻");

    aui_textcmd_matchadd("volume_minus", "降低音量");
    aui_textcmd_matchadd("volume_minus", "减小音量");
    aui_textcmd_matchadd("volume_minus", "降低声音");

    /*静音*/
    aui_textcmd_matchadd("volume_mute", "静音");
    aui_textcmd_matchadd("volume_mute", "关,音");

    /*播放*/
    //检索循序添加的倒序,所以较长的检索放后面
    aui_textcmd_matchadd("media_repaly", "继续播放");
    aui_textcmd_matchadd("media_close", "关闭播放");
    aui_textcmd_matchadd("media_close", "停止播放");
    aui_textcmd_matchadd("media_close", "退出播放");

    /*配网*/
    aui_textcmd_matchadd("wifi_pair_start", "进入配网");
    aui_textcmd_matchadd("wifi_pair_start", "打开配网");

}

/**
 * 文本命令解析的处理
*/
int aui_nlp_proc_cb_textcmd(cJSON *js, const char *json_text)
{
    LOGD(TAG, "Enter %s", __FUNCTION__);

    cJSON *cmd = cJSON_GetObjectItemByPath(js, "textcmd");
    if (!cJSON_IsString(cmd)) {
        LOGD(TAG, "textcmd not found");
        return -1;
    }

    LOGD(TAG, "process_textcmd:%s", cmd->valuestring);
    if (strcmp(cmd->valuestring, "volume_plus") == 0) {
        app_volume_inc(1);
    } else if (strcmp(cmd->valuestring, "volume_minus") == 0) {
        app_volume_dec(1);
    } else if (strcmp(cmd->valuestring, "volume_mute") == 0) {
        app_volume_mute();
    } else if (strcmp(cmd->valuestring, "media_repaly") == 0) {
        app_player_resume();
    } else if (strcmp(cmd->valuestring, "media_close") == 0) {
        app_player_pause();
        local_audio_play(LOCAL_AUDIO_OK);
    } else if (strcmp(cmd->valuestring, "media_pause") == 0) {
        app_player_pause();
        local_audio_play(LOCAL_AUDIO_OK);
    } else if (strcmp(cmd->valuestring, "wifi_pair_start") == 0) {
        extern void wifi_pair_start();
        wifi_pair_start();
    } else {
        LOGW(TAG, "unknown cmd");
        return -1;
    }

    return 0;
}
