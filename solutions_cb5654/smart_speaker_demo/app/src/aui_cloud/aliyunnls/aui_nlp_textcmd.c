/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "aui_nlp.h"

#define TAG "nlptxt"

#define URL_BASE "https://cop-image-prod.oss-cn-hangzhou.aliyuncs.com/resource/undefined"
#define MP3_HZ300 "1577167230775/Sin300Hz.mp3"
#define MP3_HZ1K "1577167215618/Sin1000Hz.mp3"
#define MP3_TEST1 "1577166283488/AudioTest1.mp3"
#define MP3_TEST2 "1577167181404/AudioTest2.mp3"

void app_text_cmd_init(void)
{
    aui_textcmd_matchinit(16, ",");

    /* 播放控制 */
    aui_textcmd_matchadd("media_test", "播放测试音");
    aui_textcmd_matchadd("media_1khz", "播放标准音");
    aui_textcmd_matchadd("media_300hz", "播放低,音");

}

/**
 * 文本命令解析的处理
*/
int aui_nlp_proc_textcmd(const char *asr_text)
{
    LOGD(TAG, "Enter %s", __FUNCTION__);

    if (asr_text == NULL) {
        LOGD(TAG, "textcmd not found");
        return -1;
    }

    const char *cmd = aui_textcmd_find(asr_text);

    LOGD(TAG, "process_textcmd:%s", cmd);
    if (strcmp(cmd, "media_close") == 0) {
        aui_player_stop(MEDIA_ALL);
    } else if (strcmp(cmd, "media_test") == 0) {
        int id = (rand() % 2) + 1;
        switch(id){
            case 1:
                aui_player_play(MEDIA_MUSIC, URL_BASE"/"MP3_TEST1, 0);
                break;
            case 2:
                aui_player_play(MEDIA_MUSIC, URL_BASE"/"MP3_TEST2, 0);
                break;
            default:
                return -1;
        }
    } else if (strcmp(cmd, "media_1khz") == 0) {
        aui_player_play(MEDIA_MUSIC, URL_BASE"/"MP3_HZ1K, 0);
    } else if (strcmp(cmd, "media_300hz") == 0) {
        aui_player_play(MEDIA_MUSIC, URL_BASE"/"MP3_HZ300, 0);
    } else {
        LOGW(TAG, "unknown cmd");
        return -1;
    }

    return 0;
}
