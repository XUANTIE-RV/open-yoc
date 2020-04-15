## 简介

该音频播放器基于轻量级的多媒体AV框架开发，当前支持mp3、wav、mp4(aac)、raw pcm、adts等音频格式的本地(SD卡/内存)、网络(http/https)、语音合成(TTS)的播放。同时其还支持音频重采样和音效均衡等功能。

## 播放器当前支持的url格式

url格式定义规则：

| 流类型       | URL前缀    | URL格式                                                      |
| :----------- | :--------- | :----------------------------------------------------------- |
| http网络流   | http(s):// | http(s)://ip:port/xx.mp3                                     |
| 文件流(SD卡) | file://    | file:///fatfs0/xx.mp3?avformat=%s&avcodec=%u&channel=%u&rate=%u |
| 内存流       | mem://     | mem://addr=%u&size=%u&avformat=%u&avcodec=%u&channel=%u&rate=%u |
| fifo流       | fifo://    | fifo://tts/1?avformat=%s&avcodec=%u&channel=%u&rate=%u       |

###  播放器使用示例

```c
//核心代码片段
static player_t* g_player;

static void _player_event(player_t *player, uint8_t type, const void *data, uint32_t len)
{
    UNUSED(len);
    UNUSED(data);
    UNUSED(handle);
    LOGI(TAG, "=====%s, %d, type = %d", __FUNCTION__, __LINE__, type);

    switch (type) {
    case PLAYER_EVENT_ERROR:
        player_stop(player);
        break;

    case PLAYER_EVENT_START:
        break;

    case PLAYER_EVENT_FINISH:
        player_stop(player);
        break;

    default:
        break;
    }
}

void main()
{
    plyh_t plyh;

    memset(&plyh, 0, sizeof(plyh_t));
    /* 配置eq功能为8段式 */
    plyh.eq_segments   = 8;
    /* 配置音频重采样到48k */
    plyh.resample_rate = 48000;
    plyh.rcv_timeout   = 0;
    /* 配置播放器状态回调函数 */
    plyh.event_cb      = _player_event;
    /* 实例化一个播放器 */
    g_player = player_new(&plyh);

    /* 网络mp3歌曲 */
    char *url = "http://www.baidu.com/xx.mp3";
    /* 文件/sd卡mp3歌曲 */
    //char *url = "file:///fatfs0/test.MP3";
    /* 播放指定url的音频文件 */
    player_play(g_player, url);
}
```

