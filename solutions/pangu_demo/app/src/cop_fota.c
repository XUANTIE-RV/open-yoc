#include <aos/aos.h>
#include <yoc/fota.h>
#include <yoc/eventid.h>
#include <yoc/uservice.h>

#include "app_main.h"

#define KV_FOTA_CYCLE_MS        "fota_cycle"
#define KV_FOTA_START_MS        "fota_start"
#define KV_FOTA_ENABLED         "fota_en"
#define FOTA_CYCLE_DELAY_TIME   (60000)
#define FOTA_START_DELAY_TIME   (0)

#define TAG "app_fota"

int g_fota_delay_timer = 2500;
static fota_t *g_fota_handle = NULL;

void fota_user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (event_id == EVENT_FOTA_START) {
        fota_start(g_fota_handle);
    }
    else {
        ;
    }
}

static int fota_event_cb(void *arg, fota_event_e event) //return 0: still do the default handle      not zero: only do the user handle
{
    fota_t *fota = (fota_t *)arg;
    switch (event) {
        case FOTA_EVENT_VERSION:
            LOGD(TAG, "FOTA VERSION :%x", fota->status);
            break;

        case FOTA_EVENT_START:
            LOGD(TAG, "FOTA START :%x", fota->status);
            break;

        case FOTA_EVENT_FAIL:
            LOGD(TAG, "FOTA FAIL :%x", fota->status);
            break;

        case FOTA_EVENT_FINISH:
            LOGD(TAG, "FOTA FINISH :%x", fota->status);
            aos_kv_del("fota_offset");
            extern void speaker_uninit();
            speaker_uninit();            
            aos_reboot();
            break;

        default:
            break;
    }
    return 0;
}

void app_fota_init(void)
{
    int ret;
    LOGI(TAG, "======> %s\r\n", aos_get_app_version());
    // LOGI(TAG, "hahahhaha!!!!");
    int fota_en = 1;
    ret = aos_kv_getint(KV_FOTA_ENABLED, &fota_en);
    if (ret == 0 && fota_en == 0) {
        return;
    }

    fota_register_cop();
    netio_register_http();
    netio_register_flash();
    g_fota_handle = fota_open("cop", "flash://misc", fota_event_cb);

    // if (pm_get_policy() == LPM_POLICY_NO_POWER_SAVE) {
        g_fota_handle->auto_check_en = 1;
    // } else {
    //     g_fota_handle->auto_check_en = 0;
    // }
    ret = aos_kv_getint(KV_FOTA_CYCLE_MS, &(g_fota_handle->sleep_time));
    if (ret != 0 || g_fota_handle->sleep_time < 1000) {
        g_fota_handle->sleep_time = FOTA_CYCLE_DELAY_TIME;
    }
    g_fota_handle->timeoutms = 10000;
    g_fota_handle->retry_count = 0;

    ret = aos_kv_getint(KV_FOTA_START_MS, &g_fota_delay_timer);
    if (ret != 0) {
        g_fota_delay_timer = FOTA_START_DELAY_TIME;
    }

/*
    const char *url = "http://cid.c-sky.com/api/image/ota/pull";
    aos_kv_setstring("otaurl", url);
    aos_kv_setstring("device_id", "1f19d54804400000d7aca515bbebbe4d");
    aos_kv_setstring("model", "sc5654a");
*/
    event_subscribe(EVENT_FOTA_START, fota_user_local_event_cb, NULL);
}

void app_fota_start(void)
{
    int ret;
    int fota_en = 1;
    ret = aos_kv_getint(KV_FOTA_ENABLED, &fota_en);
    if (ret == 0 && fota_en == 0) {
        return;
    }

    event_publish_delay(EVENT_FOTA_START, NULL, g_fota_delay_timer);
}

int app_fota_is_running(void)
{
    if (g_fota_handle->status == FOTA_DOWNLOAD) {
        LOGD(TAG, "fota is running..........\n");
        return 1;
    }
    return 0;
}

void app_fota_do_check(void)
{
    LOGD(TAG, "app_fota_do_check");
    if (!g_fota_handle->auto_check_en) {
        aos_msleep(200);
        fota_do_check(g_fota_handle);        
    }
}

void app_fota_set_auto_check(int enable)
{
    g_fota_handle->auto_check_en = enable;
}

int app_fota_is_downloading(void)
{
    if (g_fota_handle == NULL) {
        return 0;
    }

    if (fota_get_status(g_fota_handle) == FOTA_DOWNLOAD) {
        return 1;
    }
    return 0;
}