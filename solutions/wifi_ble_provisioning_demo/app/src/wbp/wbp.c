/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/ble.h>
#include <ulog/ulog.h>
#include <wifi_provisioning.h>

#include "wbp.h"

#define TAG "WBP"

static struct _wifi_info_t {
    uint8_t            ssid_set : 1;
    uint8_t            pwd_set  : 1;
    int                method_id;
    wifi_prov_cb       prov_cb;
    wifi_prov_result_t result;
} wifi_info = { 0 };

static aos_sem_t sync_sem;

static char ssid_char_des[] = "Wifi BLE Provisioning SSID";
static char pwd_char_des[]  = "Wifi BLE Provisioning Password";

typedef int (*wbps_ssid_cb_t)(const char *ssid);
typedef int (*wbps_password_cb_t)(const char *pwd);

typedef struct _wbps_t {
    int (*wbps_ssid_cb)(const uint8_t *ssid, size_t len);
    int (*wbps_password_cb)(const uint8_t *pwd, size_t len);
} wbps_t;

enum
{
    YOC_WIFI_BLE_IDX_SVC,

    YOC_WIFI_BLE_IDX_SSID_CHAR,
    YOC_WIFI_BLE_IDX_SSID_VAL,
    YOC_WIFI_BLE_IDX_SSID_DES,

    YOC_WIFI_BLE_IDX_PWD_CHAR,
    YOC_WIFI_BLE_IDX_PWD_VAL,
    YOC_WIFI_BLE_IDX_PWD_DES,

    YOC_WIFI_BLE_IDX_MAX,
};

#define WBPS_UUID                                                                                                      \
    UUID128_DECLARE(0x7e, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0xA0)
#define YOC_WIFI_BLE_SSID_UUID                                                                                         \
    UUID128_DECLARE(0x7e, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0xA1)
#define YOC_WIFI_BLE_PWD_UUID                                                                                          \
    UUID128_DECLARE(0x7e, 0x31, 0x35, 0xd4, 0x12, 0xf3, 0x11, 0xe9, 0xab, 0x14, 0xd6, 0x63, 0xbd, 0x87, 0x3d, 0xA2)

struct _wbps_handle_t {
    int16_t conn_handle;
    int16_t svc_handle;
    wbps_t *wbps;
} wbps_handle = { 0 };

static gatt_service wifi_ble_profile;

static gatt_attr_t wifi_ble_attrs[YOC_WIFI_BLE_IDX_MAX] = {
    [YOC_WIFI_BLE_IDX_SVC] = GATT_PRIMARY_SERVICE_DEFINE(WBPS_UUID),

    [YOC_WIFI_BLE_IDX_SSID_CHAR] = GATT_CHAR_DEFINE(YOC_WIFI_BLE_SSID_UUID, GATT_CHRC_PROP_WRITE),
    [YOC_WIFI_BLE_IDX_SSID_VAL]  = GATT_CHAR_VAL_DEFINE(YOC_WIFI_BLE_SSID_UUID, GATT_PERM_READ | GATT_PERM_WRITE),
    [YOC_WIFI_BLE_IDX_SSID_DES]  = GATT_CHAR_CUD_DEFINE(ssid_char_des, GATT_PERM_READ),

    [YOC_WIFI_BLE_IDX_PWD_CHAR] = GATT_CHAR_DEFINE(YOC_WIFI_BLE_PWD_UUID, GATT_CHRC_PROP_WRITE),
    [YOC_WIFI_BLE_IDX_PWD_VAL]  = GATT_CHAR_VAL_DEFINE(YOC_WIFI_BLE_PWD_UUID, GATT_PERM_WRITE),
    [YOC_WIFI_BLE_IDX_PWD_DES]  = GATT_CHAR_CUD_DEFINE(pwd_char_des, GATT_PERM_READ),
};

static void _conn_change(ble_event_en event, void *event_data)
{
    evt_data_gap_conn_change_t *e = (evt_data_gap_conn_change_t *)event_data;

    if (e->connected == CONNECTED) {
        wbps_handle.conn_handle = e->conn_handle;
    } else {
        wbps_handle.conn_handle = -1;
        wbps_handle.svc_handle  = 0;
    }
}

static int _event_char_write(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_write_t *e             = (evt_data_gatt_char_write_t *)event_data;
    int16_t                     handle_offset = 0;

    BLE_CHAR_RANGE_CHECK(wbps_handle.svc_handle, YOC_WIFI_BLE_IDX_MAX, e->char_handle, handle_offset);

    switch (handle_offset) {
        case YOC_WIFI_BLE_IDX_SSID_VAL:
            if (wbps_handle.wbps && wbps_handle.wbps->wbps_ssid_cb) {
                wbps_handle.wbps->wbps_ssid_cb(e->data, e->len);
            }
            break;
        case YOC_WIFI_BLE_IDX_PWD_VAL:
            if (wbps_handle.wbps && wbps_handle.wbps->wbps_password_cb) {
                wbps_handle.wbps->wbps_password_cb(e->data, e->len);
            }
            break;
        default:
            e->len = 0;
            break;
    }

    return 0;
}

static int _event_char_read(ble_event_en event, void *event_data)
{
    evt_data_gatt_char_read_t *e             = (evt_data_gatt_char_read_t *)event_data;
    int16_t                    handle_offset = 0;

    BLE_CHAR_RANGE_CHECK(wbps_handle.svc_handle, YOC_WIFI_BLE_IDX_MAX, e->char_handle, handle_offset);

    switch (handle_offset) {
        case YOC_WIFI_BLE_IDX_SSID_DES:
            e->data = (uint8_t *)ssid_char_des;
            e->len  = strlen(ssid_char_des);
            break;
        case YOC_WIFI_BLE_IDX_PWD_DES:
            e->data = (uint8_t *)pwd_char_des;
            e->len  = strlen(pwd_char_des);
            break;

        default:
            e->data = NULL;
            e->len  = 0;
            break;
    }

    return 0;
}

static int _wbps_event_callback(ble_event_en event, void *event_data)
{
    switch (event) {
        case EVENT_GAP_CONN_CHANGE:
            _conn_change(event, event_data);
            break;

        case EVENT_GATT_CHAR_WRITE:
            _event_char_write(event, event_data);
            break;
        case EVENT_GATT_CHAR_READ:
            _event_char_read(event, event_data);
            break;
        default:
            break;
    }

    return 0;
}

static ble_event_cb_t ble_cb = {
    .callback = _wbps_event_callback,
};

int ble_prf_wbps_init(wbps_t *wbps)
{
    int ret;

    if (NULL == wbps) {
        return -BT_STACK_STATUS_EINVAL;
    }

    ret = ble_stack_event_register(&ble_cb);

    if (ret) {
        return ret;
    }

    ret = ble_stack_gatt_registe_service(&wifi_ble_profile, wifi_ble_attrs, BLE_ARRAY_NUM(wifi_ble_attrs));
    if (ret < 0) {
        return ret;
    }

    wbps_handle.svc_handle = ret;

    wbps_handle.conn_handle = -1;

    wbps_handle.wbps = wbps;

    return 0;
}

static int _wbps_ssid_cb(const uint8_t *ssid, size_t len)
{
    if (len > sizeof(wifi_info.result.ssid) - 1) {
        LOGE(TAG, "invaild ssid len %d", len);
        return -1;
    }

    memcpy(wifi_info.result.ssid, ssid, len);
    wifi_info.result.ssid[len] = '\0';
    wifi_info.ssid_set         = 1;

    aos_sem_signal(&sync_sem);
    return 0;
}

static int _wbps_password_cb(const uint8_t *pwd, size_t len)
{
    if (len > sizeof(wifi_info.result.password) - 1) {
        LOGE(TAG, "invaild ssid len %d", len);
        return -1;
    }

    memcpy(wifi_info.result.password, pwd, len);
    wifi_info.result.password[len] = '\0';
    wifi_info.pwd_set              = 1;

    aos_sem_signal(&sync_sem);
    return 0;
}

static wbps_t wbps = { .wbps_ssid_cb = _wbps_ssid_cb, .wbps_password_cb = _wbps_password_cb };

static int sl_wbp_start(wifi_prov_cb cb)
{
    int ret;

    LOGD(TAG, "sl wbp start");

    ad_data_t ad[2] = { 0 };

    uint8_t flag = AD_FLAG_GENERAL;

    ad[0].type = AD_DATA_TYPE_FLAGS;
    ad[0].data = (uint8_t *)&flag;
    ad[0].len  = 1;

    ad[1].type = AD_DATA_TYPE_UUID128_ALL;
    ad[1].data = (uint8_t *)UUID128(WBPS_UUID);
    ad[1].len  = sizeof(UUID128(WBPS_UUID));

    adv_param_t param = {
        ADV_IND, ad, NULL, BLE_ARRAY_NUM(ad), 0, ADV_FAST_INT_MIN_2, ADV_FAST_INT_MAX_2,
    };

    ret = ble_stack_adv_start(&param);

    wifi_info.prov_cb = cb;

    return ret;
}

static void sl_wbp_stop()
{
    LOGD(TAG, "sl wbp stop");

    ble_stack_adv_stop();

    return;
}

static wifi_prov_t wifi_ble_priv = {
    .name  = "wifi_ble_prov", /* wifi-provsioning method name */
    .start = sl_wbp_start,    /* wifi-provsioning start function porting */
    .stop  = sl_wbp_stop,     /* wifi-provsioning stop function porting */
};

static void _wbp_task(void *arg)
{
    while (1) {
        aos_sem_wait(&sync_sem, AOS_WAIT_FOREVER);
        if (wifi_info.ssid_set && wifi_info.pwd_set) {
            if (wifi_info.prov_cb) {
                wifi_info.prov_cb(0, WIFI_RPOV_EVENT_GOT_RESULT, &wifi_info.result);
            }
            aos_task_exit(0);
        }
    }
}

int wbp_init()
{
    int        ret;
    aos_task_t wbp_task;

    aos_sem_new(&sync_sem, 0);

    /* wifi ble provisioning sevice initialization */
    ble_prf_wbps_init(&wbps);

    /* registe the wifi-ble provisioning method to wifi-provisioning component */
    ret = wifi_prov_method_register(&wifi_ble_priv);
    if (ret < 0) {
        LOGE(TAG, "wifi prov register err, %d", ret);
        return ret;
    }

    wifi_info.method_id = ret;

    aos_task_new_ext(&wbp_task, "wbp task", _wbp_task, NULL, 6 * 1024, AOS_DEFAULT_APP_PRI);

    return 0;
}
