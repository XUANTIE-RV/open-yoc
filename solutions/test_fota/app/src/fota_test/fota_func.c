/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <app_config.h>

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
    int fota_en = 1;
    ret = aos_kv_getint(KV_FOTA_ENABLED, &fota_en);
    if (ret == 0 && fota_en == 0) {
        return;
    }

    fota_register_cop();
    netio_register_http();
    netio_register_flash();
    g_fota_handle = fota_open("cop", "flash://misc", fota_event_cb);


    g_fota_handle->auto_check_en = 1;
  
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
void app_fota_stop(void)
{
    if (g_fota_handle != NULL) {
        fota_stop(g_fota_handle);
    }
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

static void function_fota_test(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int check_id = 0;
    if(strcmp(argv[1], "init") == 0) {
        app_fota_init();
        return;
    }
    if(strcmp(argv[1], "start") == 0) {
        app_fota_start();
        return;
    }
    if (strcmp(argv[1], "stop") == 0) {
        app_fota_stop();
        return;
    }
    if (strcmp(argv[1], "check") == 0) {
        app_fota_do_check();
        return;
    }
    if (strcmp(argv[1], "set_auto_check") == 0) {
        check_id = atoi(argv[2]);
        app_fota_set_auto_check(check_id);
        return;
    }
    printf("Usage:\tfota init\n\tfota start\n\tfota stop\n\tfota set_auto_check 1|0\n\tfota check\n");
    return;
}

void cli_reg_cmd_fotatest(void)
{
    static struct cli_command cmd_info = {
        "fota",
        "fota test commands",
        function_fota_test
    };

    aos_cli_register_command(&cmd_info);
}
