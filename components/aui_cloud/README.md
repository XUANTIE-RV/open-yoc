# 概述

aui cloud是专为rtos打造的语音解决方案，提供了语音合成、语义理解、本地语义端文本理解等接口。

# 接口定义

## AUI系统初始化

```C
int aui_cloud_init(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 0: 成功
  * < 0: 失败

## 本次唤醒是否开启云端二次确认(wake word verification)功能
在aui_cloud_start_pcm前调用

```C
int aui_cloud_enable_wwv(aui_t *aui, int enable);
```

* 参数
  * aui: `aui_t`指针
  * enable: 本次唤醒是否做云端二次确认, 1:使能
* 返回值
  * 0: 成功
  * < 0: 失败

## 启动语音数据交互

```C
int aui_cloud_start_pcm(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 0: 成功
  * < 0: 失败

## 设置ASR云端会话ID

 ```C
int aui_cloud_set_asr_session_id(aui_t *aui, const char *session_id);
```

* 参数
  * aui: `aui_t`指针
  * session_id: 会话ID字符串，相同ID认为是同一轮会话
* 返回值
  * 0: 成功
  * < 0: 失败

## 向AUI系统输入语音数据

```C
int aui_cloud_push_pcm(aui_t *aui, void *data, size_t size);
```

* 参数
  * aui: `aui_t`指针
  * data: 数据指针
  * size: 数据大小
* 返回值
  * 0: 成功
  * < 0: 失败

## 结束语音数据输入，AUI系统开始处理

```C
int aui_cloud_stop_pcm(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 0: 成功
  * < 0: 失败

## 初始化云端唤醒二次确认(wake word verification)

```C
int aui_cloud_init_wwv(aui_t *aui, aui_wwv_cb_t cb);
```

* 参数
  * aui: `aui_t`指针
  * cb: 二次唤醒结果回调
* 返回值
  * 0: 成功
  * < 0: 失败

### 回调函数定义:
```C
typedef void (* aui_wwv_cb_t)(aui_wwv_resut_e wwv_result);
```

### 处理结果定义:
```C
typedef enum aui_wwv_resut {
    AUI_WWV_REJECT = 1,
    AUI_WWV_CONFIRM
} aui_wwv_resut_e;
```

## 推送唤醒二次确认音频数据到云端

```C
int aui_cloud_push_wwv_data(aui_t *aui, void *data, size_t size);
```

* 参数
  * aui: `aui_t`指针
  * data: 数据指针
  * size: 数据大小
* 返回值
  * 0: 成功
  * < 0: 失败

## 启动nlp系统，连接云端

```C
int aui_cloud_start_nlp(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 0: 成功
  * < 0: 失败

## 向AUI系统输入文本数据，AUI进行处理并回调NLP处理回调函数

```C
int aui_cloud_push_text(aui_t *aui, char *text);
```

* 参数
  * aui: `aui_t`指针
  * text: 文本字符串
* 返回值
  * 0: 成功
  * < 0: 失败

## 停止nlp系统，断开云端连接

```C
int aui_cloud_stop_nlp(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 0: 成功
  * < 0: 失败

## 设置AUI系统tts状态监听回调函数

需要在aui_cloud_req_tts之前调用

 ```C
void aui_cloud_set_tts_status_listener(aui_t *aui, aui_tts_cb_t stat_cb);
```

* 参数
  * aui: `aui_t`指针
  * stat_cb: tts状态监听回调函数
* 返回值
  * 无

### 状态回调函数定义:
```C
typedef void (* aui_tts_cb_t)(aui_tts_state_e tts_state);
```

### 状态定义:
```C
typedef enum aui_tts_state {
    AUI_TTS_INIT    = 1,
    AUI_TTS_PLAYING = 2,
    AUI_TTS_FINISH  = 3,
    AUI_TTS_ERROR   = 4,
} aui_tts_state_e;
```

## 启动tts系统，连接云端

```C
int aui_cloud_start_tts(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 0: 成功
  * < 0: 失败

## 向AUI系统输入文本数据，要求返回文本的TTS转换后的语音结果

```C
int aui_cloud_req_tts(aui_t *aui, const char *text, const char *player_fifo_name);
```

* 参数
  * aui: `aui_t`指针
  * text: 需要转换的文本
  * player_fifo_name: 例如"fifo://tts1",播放接口可使用该字符串来播放音频
* 返回值
  * 0: 成功
  * < 0: 失败

## 停止tts系统，断开云端连接

```C
int aui_cloud_stop_tts(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 0: 成功
  * < 0: 失败

## 停止上云流程

```C
int aui_cloud_stop(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 0: 成功
  * < 0: 失败

## ASR使用讯飞云

```C
void aui_asr_register_xunfei(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 无

## asr使用阿里云

```C
void aui_asr_register_aliyun(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 无

## asr使用MIT云

 ```C
void aui_asr_register_mit(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 无

## nlp使用讯飞云

```C
void aui_nlp_register_xunfei(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 无

## tts使用讯飞云

```C
void aui_tts_register_xunfei(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 无

## tts使用MIT云

```C
void aui_tts_register_mit(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 无

## 初始化本地短文本匹配

```C
void aui_textcmd_matchinit(int max_text_len, const char* delim);
```

* 参数
  * max_text_len: 匹配字符串的长度
  * delim: 匹配文本的分隔符
* 返回值
  * 无

## 增加需要匹配的短文本

```C
void aui_textcmd_matchadd(const char *cmd, const char *text);
```

* 参数
  * cmd: 控制命令字符串
  * text: 需要匹配的文本
* 返回值
  * 无

## 预处理命令与AUI NLP的绑定
把语音通过ASR转化为文本后，再调用该函数，把预处理的控制命令统一通过AUI的NLP处理回调进行处理

```C
int aui_textcmd_matchnlp(aui_t *aui, const char *text);
```

* 参数
  * aui: `aui_t`指针
  * text: 命令文本
* 返回值
  * 0: 匹配到关键字
  * < 0: 没有匹配到

## NLP处理回调

```C
typedef int (*aui_nlp_proc_cb_t)(cJSON *js, const char *json_text);
```

* 参数
  * js: `cJSON`对象
  * json_text: json文本
* 返回值
  * 0: 解析到请求并做了处理,处理循环退出,不继续检查其他处理
  * -1: 没有解析到请求,处理循环继续匹配其他回调
  * -2: 解析到请求但请求的格式或内容有误,处理循环退出,不继续检查其他处理

## 添加NLP处理回调

```C
void aui_nlp_process_add(aui_cmd_set_t *set, aui_nlp_proc_cb_t cmd);
```

* 参数
  * set: `aui_cmd_set_t`指针
  * cmd: NLP处理回调函数
* 返回值
  * 无

## 删除NLP处理回调

```C
void aui_nlp_process_remove(aui_cmd_set_t *set, aui_nlp_proc_cb_t cmd);
```

* 参数
  * set: `aui_cmd_set_t`指针
  * cmd: NLP处理回调函数
* 返回值
  * 无

## 设置命令执行器的预处理回调
预处理回调函数不是必须的，一般对NLP返回的格式进行与检查是否合法

```C
void aui_nlp_process_set_pre_check(aui_cmd_set_t *set, aui_nlp_proc_cb_t cmd);
```

* 参数
  * set: `aui_cmd_set_t`指针
  * cmd: 预处理回调函数
* 返回值
  * 无

## 按照添加顺序逆序执行aui_cmd_set_t 中每个注册的命令处理回调函数

```C
int aui_nlp_process_run(aui_cmd_set_t *set, const char *json_text);
```

* 参数
  * set: `aui_cmd_set_t`指针
  * json_text: 需要进行分析处理的NLP处理文本
* 返回值
  * 0: 成功
  * `命令执行器错误码`: 失败

### 命令执行器错误码

* AUI_CMD_PROC_NOMATCH -1        无法匹配到可处理的技能 
* AUI_CMD_PROC_MATCH_NOACTION -2 匹配到技能执行器，但意图不支持或解析有错误 
* AUI_CMD_PROC_NET_ABNORMAL -3   网络异常 
* AUI_CMD_PROC_ERROR -10         其他错误 


# 示例代码
```C
/* ai cmd */
aui_cmd_set_t g_aui_nlp_process;

/* ai engine */
aui_t g_aui_handler;

static int aui_nlp_process1(cJSON *js, const char *json_text)
{
    /* 用户增加堆json的具体解析 */
    return 0;
}

static int aui_nlp_process2(cJSON *js, const char *json_text)
{
    /* 用户增加堆json的具体解析 */
    return 0;
}

static void aui_nlp_cb(const char *json_text)
{
    int ret;

    LOGI("NLP", "json= %s", json_text);

    /* 处理的主入口, 具体处理见初始化注册的处理函数 */
    ret = aui_nlp_process_run(&g_aui_nlp_process, json_text);

    LOGI("NLP", "nlp_process ret=%d", ret);

}

/* ai cmd init */
static int app_aui_cmd_init(void)
{
    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_process1);
    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_process2);

    return 0;
}

/* ai engine init */
static int app_aui_nlp_init(void)
{
    int ret;

    aui_config_t cfg;

    cfg.per         = "xiaoyan";
    cfg.vol         = 50;
    cfg.pit         = 50;
    cfg.spd         = 50;
    cfg.js_account  = "{\"asr\":{\"app_key\":\"1243\",\"app_id\":\"456\"},\"tts\":{\"app_key\":\"3434\",\"app_id\":\"9988\"}}";
    cfg.nlp_cb      = aui_nlp_cb;
    g_aui_handler.config  = cfg;
    g_aui_handler.context = NULL;

    aui_asr_register_aliyun(&g_aui_handler);
    aui_nlp_register_xunfei(&g_aui_handler);
    aui_tts_register_xunfei(&g_aui_handler);

    ret = aui_cloud_init(&g_aui_handler);

    if (ret != 0) {
        LOGE(TAG, "ai engine error");
    }

    return ret;
}
```