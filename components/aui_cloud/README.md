# 概述

aui cloud是专为rtos打造的语音解决方案，提供了语音合成、语义理解、本地语义端文本理解等接口。

# 示例代码
```C
/* ai cmd */
aui_cmd_set_t g_aui_nlp_process;

/* ai engine */
aui_config_t g_aui_cfg;
aui_t        g_aui_handler;

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

    g_aui_cfg.per         = "xiaoyan";
    g_aui_cfg.vol         = 50;
    g_aui_cfg.pit         = 50;
    g_aui_cfg.spd         = 50;
    g_aui_cfg.nlp_cb      = aui_nlp_cb;
    g_aui_handler.config  = &g_aui_cfg;
    g_aui_handler.context = NULL;

    ret = aui_cloud_init(&g_aui_handler);

    if (ret != 0) {
        LOGE(TAG, "ai engine error");
    }

    return ret;
}
```

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

## 向AUI系统输入文本数据，AUI进行处理并回调NLP处理回调函数

```C
int aui_cloud_push_text(aui_t *aui, char *text, const char *engine_name);
```

* 参数
  * aui: `aui_t`指针
  * test: 文本字符串
  * engine_name: 预留多引擎的支持
* 返回值
  * 0: 成功
  * < 0: 失败

## 向AUI系统输入文本数据，要求返回文本的TTS转换后的语音结果

```C
int aui_cloud_req_tts(aui_t *aui, const char *name, const char *text);
```

* 参数
  * aui: `aui_t`指针
  * name: 播放器的播放接口,若为NULL,默认请使用`fifo://tts/1`
  * text: 需要转换的文本
* 返回值
  * 0: 成功
  * < 0: 失败

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

## 预处理命令与AUI NLP的绑定, 把语音通过ASR转化为文本后，再调用该函数，把预处理的控制命令统一通过AUI的NLP处理回调进行处理

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
  * 0: 成功
  * < 0: 失败

## 删除NLP处理回调

```C
void aui_nlp_process_remove(aui_cmd_set_t *set, aui_nlp_proc_cb_t cmd);
```

* 参数
  * set: `aui_cmd_set_t`指针
  * cmd: NLP处理回调函数
* 返回值
  * 0: 成功
  * < 0: 失败

## 设置命令执行器的预处理回调，预处理回调函数不是必须的，一般对NLP返回的格式进行与检查是否合法

```C
void aui_nlp_process_set_pre_check(aui_cmd_set_t *set, aui_nlp_proc_cb_t cmd);
```

* 参数
  * set: `aui_cmd_set_t`指针
  * cmd: 预处理回调函数
* 返回值
  * 0: 成功
  * < 0: 失败

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