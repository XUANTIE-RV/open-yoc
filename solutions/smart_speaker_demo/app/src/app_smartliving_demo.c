/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "stdio.h"
#include <aos/kernel.h>
#include <aos/debug.h>
#include <ulog/ulog.h>
#include <smartliving/exports/iot_export_linkkit.h>
#include "cJSON.h"
#include "app_main.h"

#define TAG "SL"

#define USER_EXAMPLE_YIELD_TIMEOUT_MS (200)

typedef struct {
    int master_devid;
    int cloud_connected;
    int master_initialized;
    int started;
} user_example_ctx_t;

static user_example_ctx_t g_user_example_ctx;

static user_example_ctx_t *user_example_get_ctx(void)
{
    return &g_user_example_ctx;
}

#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO
void user_post_property(void);
void user_post_key_press_event(int key_num);
#endif
static int user_master_dev_available(void);

static int user_connected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    printf("Cloud Connected\n");
    user_example_ctx->cloud_connected = 1;

#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO
    user_post_property();
    user_post_key_press_event(-1);
#endif

    return 0;
}

static int user_disconnected_event_handler(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    printf("Cloud Disconnected\n");

    user_example_ctx->cloud_connected = 0;

    return 0;
}

// static int user_down_raw_data_arrived_event_handler(const int devid, const unsigned char *payload,
//         const int payload_len)
// {
//     printf("Down Raw Message, Devid: %d, Payload Length: %d\n", devid, payload_len);
//     return 0;
// }

// static int user_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
//         const char *request, const int request_len,
//         char **response, int *response_len)
// {
//     int contrastratio = 0, to_cloud = 0;
//     cJSON *root = NULL, *item_transparency = NULL, *item_from_cloud = NULL;
//     printf("Service Request Received, Devid: %d, Service ID: %.*s, Payload: %s\n", devid, serviceid_len,
//                   serviceid,
//                   request);

//     /* Parse Root */
//     root = cJSON_Parse(request);
//     if (root == NULL || !cJSON_IsObject(root)) {
//         printf("JSON Parse Error\n");
//         return -1;
//     }

//     if (strlen("Custom") == serviceid_len && memcmp("Custom", serviceid, serviceid_len) == 0) {
//         /* Parse Item */
//         const char *response_fmt = "{\"Contrastratio\":%d}";
//         item_transparency = cJSON_GetObjectItem(root, "transparency");
//         if (item_transparency == NULL || !cJSON_IsNumber(item_transparency)) {
//             cJSON_Delete(root);
//             return -1;
//         }
//         printf("transparency: %d\n", item_transparency->valueint);
//         contrastratio = item_transparency->valueint + 1;

//         /* Send Service Response To Cloud */
//         *response_len = strlen(response_fmt) + 10 + 1;
//         *response = (char *)HAL_Malloc(*response_len);
//         if (*response == NULL) {
//             printf("Memory Not Enough\n");
//             return -1;
//         }
//         memset(*response, 0, *response_len);
//         HAL_Snprintf(*response, *response_len, response_fmt, contrastratio);
//         *response_len = strlen(*response);
//     } else if (strlen("SyncService") == serviceid_len && memcmp("SyncService", serviceid, serviceid_len) == 0) {
//         /* Parse Item */
//         const char *response_fmt = "{\"ToCloud\":%d}";
//         item_from_cloud = cJSON_GetObjectItem(root, "FromCloud");
//         if (item_from_cloud == NULL || !cJSON_IsNumber(item_from_cloud)) {
//             cJSON_Delete(root);
//             return -1;
//         }
//         printf("FromCloud: %d\n", item_from_cloud->valueint);
//         to_cloud = item_from_cloud->valueint + 1;

//         /* Send Service Response To Cloud */
//         *response_len = strlen(response_fmt) + 10 + 1;
//         *response = (char *)HAL_Malloc(*response_len);
//         if (*response == NULL) {
//             printf("Memory Not Enough\n");
//             return -1;
//         }
//         memset(*response, 0, *response_len);
//         HAL_Snprintf(*response, *response_len, response_fmt, to_cloud);
//         *response_len = strlen(*response);
//     }
//     cJSON_Delete(root);

//     return 0;
// }

#if defined(EN_COMBO_NET) && (EN_COMBO_NET == 1)
int user_combo_get_dev_status_handler(uint8_t *buffer, uint32_t length)
{
    combo_ble_operation_e ble_ops = COMBO_BLE_MAX;
    cJSON *root = NULL;

    /* Parse Root */
    root = cJSON_Parse(buffer);
    if (root == NULL || !cJSON_IsObject(root)) {
        printf("%d, JSON Parse Error\n", __LINE__);
        return -1;
    }

    cJSON *method = cJSON_GetObjectItem(root, "method");
    if (strcmp((const char*)method->valuestring, "thing.service.property.set") == 0) {
        printf("combo: ble set\r\n");
        ble_ops = COMBO_BLE_SET;
    } else if (strcmp((const char*)method->valuestring, "thing.service.property.get") == 0) {
        printf("combo: ble get\r\n");
        ble_ops = COMBO_BLE_GET;
    }

    if (ble_ops == COMBO_BLE_SET) {
        cJSON *params = cJSON_GetObjectItemByPath(root, "params");
        if (!cJSON_IsObject(params)) {
            printf("%d, JSON Parse Error\n", __LINE__);
            return -1;
        }

        // TODO: Process message from IOT Cloud here
        combo_net_post_ext();
    }

    cJSON_Delete(root);

    return 0;
}
#endif

static int user_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    printf("Property Set Received, Devid: %d, Request: %s\n", devid, request);

#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO
    cJSON *root = NULL;

    /* Parse Root */
    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root)) {
        printf("JSON Parse Error\n");
        return -1;
    }
    cJSON *item = cJSON_GetObjectItem(root, "LightSwitch");
    if (item && cJSON_IsNumber(item)) {
        if (item->valueint) {
            //app_pwm_led_control(1);
        } else {
            //app_pwm_led_control(0);
        }
    }

    item = cJSON_GetObjectItem(root, "Brightness");
    if (item && cJSON_IsNumber(item)) {
        //app_pwm_led_set_brightness(item->valueint);
    }

    cJSON_Delete(root);
#endif

    return 0;
}

static int user_property_get_event_handler(const int devid, const char *request, const int request_len, char **response,
        int *response_len)
{
    cJSON *request_root = NULL, *item_propertyid = NULL;
    cJSON *response_root = NULL;
    int index = 0;
    printf("Property Get Received, Devid: %d, Request: %s\n", devid, request);

    /* Parse Request */
    request_root = cJSON_Parse(request);
    if (request_root == NULL || !cJSON_IsArray(request_root)) {
        printf("JSON Parse Error\n");
        return -1;
    }

    /* Prepare Response */
    response_root = cJSON_CreateObject();
    if (response_root == NULL) {
        printf("No Enough Memory\n");
        cJSON_Delete(request_root);
        return -1;
    }

    for (index = 0; index < cJSON_GetArraySize(request_root); index++) {
        item_propertyid = cJSON_GetArrayItem(request_root, index);
        if (item_propertyid == NULL || !cJSON_IsString(item_propertyid)) {
            printf("JSON Parse Error\n");
            cJSON_Delete(request_root);
            cJSON_Delete(response_root);
            return -1;
        }

        printf("Property ID, index: %d, Value: %s\n", index, item_propertyid->valuestring);

        if (strcmp("WIFI_Tx_Rate", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "WIFI_Tx_Rate", 1111);
        } else if (strcmp("WIFI_Rx_Rate", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "WIFI_Rx_Rate", 2222);
        } else if (strcmp("RGBColor", item_propertyid->valuestring) == 0) {
            cJSON *item_rgbcolor = cJSON_CreateObject();
            if (item_rgbcolor == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }
            cJSON_AddNumberToObject(item_rgbcolor, "Red", 100);
            cJSON_AddNumberToObject(item_rgbcolor, "Green", 100);
            cJSON_AddNumberToObject(item_rgbcolor, "Blue", 100);
            cJSON_AddItemToObject(response_root, "RGBColor", item_rgbcolor);
        } else if (strcmp("HSVColor", item_propertyid->valuestring) == 0) {
            cJSON *item_hsvcolor = cJSON_CreateObject();
            if (item_hsvcolor == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }
            cJSON_AddNumberToObject(item_hsvcolor, "Hue", 50);
            cJSON_AddNumberToObject(item_hsvcolor, "Saturation", 50);
            cJSON_AddNumberToObject(item_hsvcolor, "Value", 50);
            cJSON_AddItemToObject(response_root, "HSVColor", item_hsvcolor);
        } else if (strcmp("HSLColor", item_propertyid->valuestring) == 0) {
            cJSON *item_hslcolor = cJSON_CreateObject();
            if (item_hslcolor == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }
            cJSON_AddNumberToObject(item_hslcolor, "Hue", 70);
            cJSON_AddNumberToObject(item_hslcolor, "Saturation", 70);
            cJSON_AddNumberToObject(item_hslcolor, "Lightness", 70);
            cJSON_AddItemToObject(response_root, "HSLColor", item_hslcolor);
        } else if (strcmp("WorkMode", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "WorkMode", 4);
        } else if (strcmp("NightLightSwitch", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "NightLightSwitch", 1);
        } else if (strcmp("Brightness", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "Brightness", 30);
        } else if (strcmp("LightSwitch", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "LightSwitch", 1);
        } else if (strcmp("ColorTemperature", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "ColorTemperature", 2800);
        } else if (strcmp("PropertyCharacter", item_propertyid->valuestring) == 0) {
            cJSON_AddStringToObject(response_root, "PropertyCharacter", "testprop");
        } else if (strcmp("Propertypoint", item_propertyid->valuestring) == 0) {
            cJSON_AddNumberToObject(response_root, "Propertypoint", 50);
        } else if (strcmp("LocalTimer", item_propertyid->valuestring) == 0) {
            cJSON *array_localtimer = cJSON_CreateArray();
            if (array_localtimer == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                return -1;
            }

            cJSON *item_localtimer = cJSON_CreateObject();
            if (item_localtimer == NULL) {
                cJSON_Delete(request_root);
                cJSON_Delete(response_root);
                cJSON_Delete(array_localtimer);
                return -1;
            }
            cJSON_AddStringToObject(item_localtimer, "Timer", "10 11 * * * 1 2 3 4 5");
            cJSON_AddNumberToObject(item_localtimer, "Enable", 1);
            cJSON_AddNumberToObject(item_localtimer, "IsValid", 1);
            cJSON_AddItemToArray(array_localtimer, item_localtimer);
            cJSON_AddItemToObject(response_root, "LocalTimer", array_localtimer);
        }
    }
    cJSON_Delete(request_root);

    *response = cJSON_PrintUnformatted(response_root);
    if (*response == NULL) {
        printf("No Enough Memory\n");
        cJSON_Delete(response_root);
        return -1;
    }
    cJSON_Delete(response_root);
    *response_len = strlen(*response);

    printf("Property Get Response: %s\n", *response);

    return SUCCESS_RETURN;
}

static int user_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
        const int reply_len)
{
    const char *reply_value = (reply == NULL) ? ("NULL") : (reply);
    const int reply_value_len = (reply_len == 0) ? (strlen("NULL")) : (reply_len);

    printf("Message Post Reply Received, Devid: %d, Message ID: %d, Code: %d, Reply: %.*s\n", devid, msgid, code,
                  reply_value_len,
                  reply_value);
    return 0;
}

static int user_trigger_event_reply_event_handler(const int devid, const int msgid, const int code, const char *eventid,
        const int eventid_len, const char *message, const int message_len)
{
    printf("Trigger Event Reply Received, Devid: %d, Message ID: %d, Code: %d, EventID: %.*s, Message: %.*s\n", devid,
                  msgid, code,
                  eventid_len,
                  eventid, message_len, message);

    return 0;
}

static int user_timestamp_reply_event_handler(const char *timestamp)
{
    printf("Current Timestamp: %s\n", timestamp);

    return 0;
}

static int user_initialized(const int devid)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    printf("Device Initialized, Devid: %d\n", devid);

    if (user_example_ctx->master_devid == devid) {
        user_example_ctx->master_initialized = 1;
    }

    return 0;
}

static uint64_t user_update_sec(void)
{
    static uint64_t time_start_ms = 0;

    if (time_start_ms == 0) {
        time_start_ms = HAL_UptimeMs();
    }

    return (HAL_UptimeMs() - time_start_ms) / 1000;
}

#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO
void user_post_property(void)
{
    if (!user_master_dev_available()) {
        return;
    }

    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char property_payload[256];

    int light_switch = 0;//app_pwm_led_get_status();
    int brightness = 0;//app_pwm_led_get_brightness();
    snprintf(property_payload, sizeof(property_payload),
             "{\"LightSwitch\":%d,\"Brightness\":%d}", light_switch, brightness);

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)property_payload, strlen(property_payload));

    printf("Post Property Message ID: %d\n", res);
}

void user_post_key_press_event(int key_num)
{
    if (!user_master_dev_available()) {
        return;
    }

    int res = 0;
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();
    char event_payload[128];
    if (key_num == -1) {
        snprintf(event_payload, sizeof(event_payload), "{\"KeyPress\":\"没有按键\"}");
    } else {
        snprintf(event_payload, sizeof(event_payload), "{\"KeyPress\":\"已按键%d\"}", key_num);
    }

    res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)event_payload, strlen(event_payload));
    printf("Post KeyPress Event Message ID: %d\n", res);
}
#endif

// void user_post_raw_data(void)
// {
//     int res = 0;
//     user_example_ctx_t *user_example_ctx = user_example_get_ctx();
//     unsigned char raw_data[7] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

//     res = IOT_Linkkit_Report(user_example_ctx->master_devid, ITM_MSG_POST_RAW_DATA,
//                              raw_data, 7);
//     printf("Post Raw Data Message ID: %d\n", res);
// }

static int user_master_dev_available(void)
{
    user_example_ctx_t *user_example_ctx = user_example_get_ctx();

    if (user_example_ctx->cloud_connected && user_example_ctx->master_initialized) {
        return 1;
    }

    return 0;
}

static int user_get_iot_info(iotx_linkkit_dev_meta_info_t *iot_info)
{
    iotx_linkkit_dev_meta_info_t    master_meta_info;
    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));

    if (!HAL_GetProductKey(master_meta_info.product_key)
        || !HAL_GetDeviceName(master_meta_info.device_name)
        || !HAL_GetDeviceSecret(master_meta_info.device_secret)
        || !HAL_GetProductSecret(master_meta_info.product_secret)) {
        return -1;
    }

    if (iot_info) {
        *iot_info = master_meta_info;
    }

    return 0;
}

void linkkit_thread(void *paras)
{
    uint64_t                        time_prev_sec = 0, time_now_sec = 0;
    iotx_linkkit_dev_meta_info_t    master_meta_info;
    user_example_ctx_t             *user_example_ctx = user_example_get_ctx();

    user_example_get_ctx()->started = 1;

    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, user_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, user_disconnected_event_handler);
    // IOT_RegisterCallback(ITE_RAWDATA_ARRIVED, user_down_raw_data_arrived_event_handler);
    // IOT_RegisterCallback(ITE_SERVICE_REQUEST, user_service_request_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, user_property_set_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_GET, user_property_get_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, user_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, user_trigger_event_reply_event_handler);
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, user_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, user_initialized);

    // check iot devinfo
    //memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    if (user_get_iot_info(&master_meta_info) < 0) {
        LOGE(TAG, "missing iot devinfo\n");
        aos_task_exit(-1);
        return;
    }

    /* Choose Login Server, domain should be configured before IOT_Linkkit_Open() */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* Choose Whether You Need Post Property/Event Reply */
    int post_event_reply = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_event_reply);

    /* Create Master Device Resources */
    user_example_ctx->master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (user_example_ctx->master_devid < 0) {
        printf("IOT_Linkkit_Open Failed\n");
        aos_task_exit(-1);
        return;
    }

    /* Start Connect Aliyun Server */
    while (IOT_Linkkit_Connect(user_example_ctx->master_devid) < 0) {
        HAL_SleepMs(1000);
    }

    while (user_example_get_ctx()->started) {
        IOT_Linkkit_Yield(USER_EXAMPLE_YIELD_TIMEOUT_MS);
        HAL_SleepMs(USER_EXAMPLE_YIELD_TIMEOUT_MS);

        time_now_sec = user_update_sec();
        if (time_prev_sec == time_now_sec) {
            continue;
        }

#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO
        /* Post Proprety Example */
        if (time_now_sec % 30 == 0 && user_master_dev_available()) {
            user_post_property();
        }
#endif
        time_prev_sec = time_now_sec;
    }

    IOT_Linkkit_Close(user_example_ctx->master_devid);
    memset(user_example_ctx, 0, sizeof(user_example_ctx_t));

    // IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_SetLogLevel(IOT_LOG_ERROR);

    return;
}

int smartliving_client_control(const int start)
{
    if (start) {
        if (user_example_get_ctx()->started) {
            LOGW(TAG, "smartliving client already started\n");
            return 0;
        }

        if (user_get_iot_info(NULL)  < 0) {
            LOGW(TAG, "missing iot devinfo\n");
            return -1;
        }

        aos_task_t tsk;
        int ret = aos_task_new_ext(&tsk, "mqttkit", linkkit_thread, NULL, 1024*8, AOS_DEFAULT_APP_PRI);
        if (ret == 0) {
            LOGD(TAG, "smartliving client started\n");
        } else {
            LOGE(TAG, "start smartliving client failed\n");
        }
        return ret;
    } else {
        if (!user_example_get_ctx()->started) {
            LOGW(TAG, "smartliving client not started\n");
            return -1;
        }

        user_example_get_ctx()->started = 0;

        return 0;
    }
}

int smartliving_client_is_connected(void)
{
    return user_master_dev_available();
}
