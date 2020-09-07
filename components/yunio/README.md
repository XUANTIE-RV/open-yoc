# 概述

IoT API 定义了数据处理及上云接口，规范不同的云平台，尽可能的统一使用方法，降低用户使用不同平台的学习成本。

# 示例代码

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

