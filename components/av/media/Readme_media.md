# 概述

- 支持多种类型播放、暂停、恢复、停止等操作

- 支持音乐播放后自动恢复

- 支持音量调节及渐入渐出效果

- 支持最低音量播放

# 示例代码

 ```c
const static char *type_buf[] = {
    "music",
    "system",
};

const static char *evt_buf[] = {
    "error",
    "start",
    "finish",
};
void media_evt(int type, aui_player_evtid_t evt_id)
{
    LOGD(TAG, "audio %s play %s", type_buf[type], evt_buf[evt_id]);
}

void  app_media_init(void)
{
    aui_player_init(NULL, media_evt);
    aui_player_play(MEDIA_MUSIC, http://www.srcbin.net/ai/result.mp3, 0);
}
 ```
