## 概述

IoT API 定义了数据处理及上云接口，规范不同的云平台，采用一致的编程习惯，降低用户使用不同平台的学习成本。

YoC 中 uDATA 是根据物联网的特点，设计成的面对IoT的感知设备处理框架。对数据上云的数据模型进行抽象，隐藏不同云平台协议的差异，消除不同云平台SDK 的差异。

uDATA 框架主要分 IoT 框架和 uDATA 数据模型两层：

- IoT 框架负责完成与云端的数据对接，通过云平台官方提供的 SDK，完成数据的上行与下发。
- uDATA 数据模型，负责数据的采集与反馈处理。系统定时的将外围的传感器数据收集起来，保存在uDATA中，定时通过 IoT 框架将数据模型上传到云平台。云平台下发的数据，通过 IoT 框架的传换，存入到 uDATA 数据模型中，并触发数据更新事件。

## 配置

无

## 接口列表

## 接口详细说明

### 创建 IoT 上云实例

```c
iot_t *iot_new_alicoap(iot_alicop_config_t *config);
iot_t *iot_new_alimqtt(iot_alimqtt_config_t *config);
```

创建IoT上云实例，不同云平台采用不同的创建函数及参数。

| 云平台  | 说明                   |
| ------- | ---------------------- |
| AliCoAP | 阿里物联网平台CoAP协议 |
| AliMQTT | 阿里物联网平台MQTT协议 |

* 参数
  * config: 通道配置项，详细见示例代码
* 返回值
  * 创建成功返回IoT对象指针，失败返回NULL

* 示例代码

```c
/* AliCoAP 配置 */
iot_alicoap_config_t cfg = {
    /* 服务器访问地址后缀，括号部分为阿里云三要素(元组)的pruduct key. 
    coaps://[product_key].iot-as-coap.cn-shanghai.aliyuncs.com:1883
    由于SDK固化未提供配置接口，该参数暂时无效
    */
    .server_url_suffix = "iot-as-mqtt.cn-shanghai.aliyuncs.com:1883"
};
iot_new_alicoap(&cfg);

/* AliMQTT 配置 */
iot_alimqtt_config_t cfg = {
    /* 服务器访问地址后缀，括号部分为阿里云三要素(元组)的pruduct key. 
    [product_key].iot-as-coap.cn-shanghai.aliyuncs.com:5684
    */
    .server_url_suffix = "iot-as-coap.cn-shanghai.aliyuncs.com:5684"
};
iot_new_alicoap(&cfg);
````

### 销毁 IoT 上云实例

```c
int iot_destroy(iot_t *iot);
```

销毁指定的通道，释放资源

* 参数:
  * iot: IoT对象指针
* 返回值:
  * 0: 销毁成功
  * 小于0：销毁失败

### 订阅 IoT 事件

```c
#include <yoc/uservice.h>
#include <yoc/eventid.h>
void event_subscribe(uint32_t event_id, event_callback_t cb, void *context);
```

订阅事件通过回调函数告知事件，iot_event_id 是回调函数的事件，
支持的事件如下：

| 事件名                    | 说明                                            | OceanConnet | OneNET | AliCoAP                        | AliMQTT |
| ------------------------- | ----------------------------------------------- | ----------- | ------ | ------------------------------ | ------- |
| EVENT_IOT_CONNECT_SUCCESS | 连接成功，用户可以进行数据推送                  | 支持        | 支持   | 支持                           | 支持    |
| EVENT_IOT_CONNECT_FAILD   | 连接失败                                        | 不支持      | 不支持 | 支持                           | 支持    |
| EVENT_IOT_DISCONNECTED    | 异常断开, 主动调用iot_channel_close不产生该事件 | 支持        | 支持   | 支持(通过发送失败次数模拟实现) | 支持    |
| EVENT_IOT_PUSH_SUCCESS    | 推送成功                                        | 不支持      | 不支持 | 支持                           | 支持    |
| EVENT_IOT_PUSH_FAILED     | 推送失败                                        | 不支持      | 不支持 | 支持                           | 支持    |

* 参数:
  * iot: IoT对象指针
  * cb: 事件回调函数
  * arg: 用户参数，回调函数传出
* 返回值:
  * 0: 订阅成功
  * 小于0: 订阅失败

### 打开通道

```c
iot_channel_t *iot_channel_open(iot_t *iot, const char *ch_arg);
```

通道实例初始化，每个通道的参数需要看具体的IoT平台  

| IoT平台     | 说明                                                         |
| ----------- | ------------------------------------------------------------ |
| AliCoAP     | 阿里物联网平台CoAP协议，只支持定义上传的topic， 如果ch_arg配置为"udpate"，则上传主题为/topic/[product_key]/[device_name]/update/ |
| AliMQTT     | 阿里物联网平台MQTT协议，支持定义上传和订阅，如果ch_arg配置为"update,get"，则上传主题为/[product_key]/[device_name]/update)，订阅主题为/[product_key]/[device_name]/get |

* 参数:
  * iot: IoT对象指针
  * ch_arg: 通道初始化参数
* 返回值:
  * 通道对象指针
  * NULL: 打开失败

### 启动通道

```c
int iot_channel_start(iot_channel_t *ch);
```

启动通道，接入云平台

* 参数:
  * ch: 通道对象指针
* 返回值:
  * 0: 成功
  * 小于0: 失败

### 关闭通道

```c
int iot_channel_close(iot_channel_t *ch);
```

关闭通道

* 参数:
  * ch: 通道对象指针
* 返回值:
  * 0: 成功
  * 小于0: 失败

### 配置通道数据处理回调

IoT API以数据节点(key/value)的方式定义了传感器和uData的交互过程，
推送数据时会调用channel_get 回调，用户需要把传感数据同步到uData，API会把更新的数据进行上传；
接收到云端数据，API会自动更新数据，并回调channel_set，用户继续做相应的处理。

```c
typedef void (*channel_set_t)(uData *udata, void *arg);
typedef void (*channel_get_t)(uData *udata, void *arg);
int iot_channel_config(iot_channel_t *ch, channel_set_t set, channel_get_t get, void *arg);
```

* 参数:
  * ch: 通道对象指针
  * set: channel_set回调函数
  * get: channel_get回调函数
* 返回值:
  * 0: 成功
  * 小于0: 失败

### 推送数据

推送数据到云端，推送前会先触发channel_set回调。

```c
int iot_channel_push(iot_channel_t *ch, uint8_t sync);
```

* 参数:
  * ch: 通道对象指针
  * sync: 是否为同步推送
* 返回值:
  * 0: 成功
  * 小于0: 失败

### 原生数据推送

提供通道的原生数据推送接口

| 云平台      | 说明   |
| ----------- | ------ |
| AliCoAP     | 支持   |
| AliMQTT     | 支持   |

```c
int iot_channel_raw_push(iot_channel_t *ch, void *payload, int len);
```

* 参数:
  * ch: 通道对象指针
  * payload: 数据缓冲
  * len: 数据长度
* 返回值:
  * 0: 成功
  * 小于0: 失败

## 云端 SDK 对接

IoT 上云框架中已实现“阿里云coap”、“阿里云mqtt”的官方 SDK的对接。

```c
iot_t *iot_new_alimqtt(void **config*);
iot_t *iot_new_onenet(void **config*);
```

### IoT 模型

框架中定义了数据结构  iot_t ，

```c
typedef struct _iot {
    slist_t       udata_list;
    void         *user_cfg;
    void         *ch_cfg_arg; /* iot_channel_config arg */
    iot_ops_t     ops;
    iot_config_t *config; /* iot_channel_config arg */
    aos_mutex_t   ch_mutex;
    void         *priv;
} iot_t;
```

### IoT 数据操作

```c
typedef struct _iot_ops {
    const char *name;
    int (*open)(iot_channel_t *ch);
    int (*start)(iot_channel_t *ch);
    int (*send)(iot_channel_t *ch);
    int (*raw_send)(iot_channel_t *ch, void *payload, int len);
    int (*recv)(iot_channel_t *ch);
    void (*close)(iot_channel_t *ch);
    int (*destroy)(iot_t *iot);
    slist_t udata_list;
} iot_ops_t;
```

### IoT 配置

```c
typedef struct iot_config {
    void *data;
    void (*on_update)(iot_t *iot, uData *udata); /* recv data from cloud, do user action */
    void (*on_read)(iot_t *iot, uData *udata); /* before push data to cloud, user update uData from hardware */
    void (*on_pushed)(iot_t *iot, uData *udata);
    void (*on_push_fail)(iot_t *iot, uData *udata);
    void (*on_connected)(iot_t *iot);
    void (*on_connnect_fail)(iot_t *iot);
    void (*on_disconnect)(iot_t *iot);
} iot_config_t;
```

## 示例代码

```c
#define DATA_PUSH_INTERVAL (5 * 1000)

#define DATA_PUSH_EVENT_ID (EVENT_USER + 1)

static const char *   TAG         = "app";
static iot_t *        iot_handle  = NULL;
static iot_channel_t *iot_ch_post = NULL;

static int  g_iot_connect_fail = 1;
static void push_action(void)
{
    if (!netmgr_is_gotip(app_netmgr_hdl)) {   
        return;
    }

    if (g_iot_connect_fail) {
        return;
    }

    LOGD(TAG,"iot push");
    if (iot_channel_push(iot_ch_post, 1) != 0) {
        LOGE(TAG, "push err");
    }
}

static void channel_set(uData *udata, void *arg)
{
    uData *node;
    slist_for_each_entry(&udata->head, node, uData, head)
    {
        if (node->value.updated) {
            if (node->value.type == TYPE_STR) {
                LOGI(TAG, "ch set (%s,%s)", node->key.v_str, node->value.v_str);
            } else {
                LOGI(TAG, "ch set (%s,%d)", node->key.v_str, node->value.v_int);
            }
        }
    }
}

static void channel_get(uData *udata, void *arg)
{
    yoc_udata_set(udata, value_s("temp"), value_i((uint32_t)rand() % 50), 1);
    yoc_udata_set(udata, value_s("humi"), value_i((uint32_t)rand() % 80), 1);
    yoc_udata_set(udata, value_s("led"), value_i(1), 1);
    yoc_udata_set(udata, value_s("deviceName"), value_s(aos_get_device_name()), 1);
}

static void iot_event(uint32_t event_id, const void *param, void *context)
{
    switch (event_id) {
        case EVENT_IOT_CONNECT_SUCCESS:
            LOGD(TAG, "IOT_CONNECT_SUCCESS");
            g_iot_connect_fail = 0;
            break;

        case EVENT_IOT_CONNECT_FAILED:
            LOGD(TAG, "IOT_CONNECT_FAILED");
            if (iot_ch_post) {
                //iot_channel_start(iot_ch_post);
            }
            g_iot_connect_fail = 1;
            break;

        case EVENT_IOT_DISCONNECTED:
            LOGD(TAG, "IOT_DISCONNECTED");
            if (iot_ch_post) {
                //iot_channel_start(iot_ch_post);
            }
            g_iot_connect_fail = 1;
            break;

        case EVENT_IOT_PUSH_SUCCESS:
            LOGI(TAG, "IOT_PUSH_SUCCESS");
            break;

        case EVENT_IOT_PUSH_FAILED:
            LOGW(TAG, "IOT_PUSH_FAILED");
            break;

        default:;
    }
}

void init_channel_udata(uData *udata)
{
    yoc_udata_set(udata, value_s("temp"), value_i((uint32_t)rand() % 50), 1);
    yoc_udata_set(udata, value_s("humi"), value_i((uint32_t)rand() % 80), 1);
    yoc_udata_set(udata, value_s("led"), value_i(1), 1);
    yoc_udata_set(udata, value_s("deviceName"), value_s(aos_get_device_name()), 1);
}

static void sub_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (iot_handle == NULL) {
        return;
    }

    if (event_id == EVENT_NETMGR_GOT_IP) {

        /* one channel include two topic(publish&subscribe)
        publish data to topic /[product_key]/[device_name]/update
        subscribe topic /[product_key]/[device_name]/get */
        iot_ch_post = iot_channel_open(iot_handle, "thing/event/property/post,thing/service/property/set");
        if (iot_ch_post) {
            iot_channel_config(iot_ch_post, channel_set, channel_get, NULL);

            /* init data node */
            init_channel_udata(iot_ch_post->uData);

            iot_channel_start(iot_ch_post);
        } else {
            LOGE(TAG, "channel open");
        }

    } else if (event_id == EVENT_NETMGR_NET_DISCON) {
        iot_channel_close(iot_ch_post);
        iot_ch_post = NULL;
    } else if (event_id == DATA_PUSH_EVENT_ID){
        push_action();
        /* restart timer */
        event_publish_delay(DATA_PUSH_EVENT_ID, NULL, DATA_PUSH_INTERVAL);
    }
}

void main()
{
    board_yoc_init();

    /* Subscribe */
    event_subscribe(EVENT_NETMGR_GOT_IP, sub_event_cb, NULL);
    event_subscribe(EVENT_NETMGR_NET_DISCON, sub_event_cb, NULL);
    event_subscribe(DATA_PUSH_EVENT_ID, sub_event_cb, NULL);

    /* Subscribe IOT Event */
    event_subscribe(EVENT_IOT_CONNECT_SUCCESS, iot_event, NULL);
    event_subscribe(EVENT_IOT_CONNECT_FAILED, iot_event, NULL);
    event_subscribe(EVENT_IOT_DISCONNECTED, iot_event, NULL);
    event_subscribe(EVENT_IOT_PUSH_SUCCESS, iot_event, NULL);
    event_subscribe(EVENT_IOT_PUSH_FAILED, iot_event, NULL);

    /* start push Timer */
    event_publish_delay(DATA_PUSH_EVENT_ID, NULL, DATA_PUSH_INTERVAL);

    /* [product_key].iot-as-coap.cn-shanghai.aliyuncs.com:1883 */
    iot_alimqtt_config_t cfg = {.server_url_suffix =
                                    "iot-as-mqtt.cn-shanghai.aliyuncs.com:1883"};

    iot_handle = iot_new_alimqtt(&cfg);
    if (iot_handle == NULL) {
        LOGE(TAG, "iot new");
    }
}
```

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

minilibc: v7.2.0及以上。
aos: v7.2.0及以上。

## 组件参考

无。