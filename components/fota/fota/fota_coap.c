/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc/fota.h>
#include <yoc/netio.h>
#include <aos/network.h>
#include <aos/kv.h>
#include <aos/version.h>
#include <yoc/sysinfo.h>
#include "../http/http.h"
#include <yoc/eventid.h>
#include <yoc/uservice.h>
#include <yoc/fota.h>
#include <aos/log.h>

#define COP_IMG_URL "cop_img_url"
#define COP_VERSION "cop_version"
#define TO_URL      "flash://misc"

#define TAG "fota"

static int g_update = 0;

static fota_data_t *data;

// extern void update_data(fota_data_t *data);

static void coap_event_cb(uint32_t event_id, const void *param, void *context)
{
    data = (fota_data_t *)param;
    // LOGD(TAG, "coap event %x (%s) (%p) (%p) (%p) (%p)", event_id, data->url, data->data, data, context, param);
    switch(event_id) {
        case EVENT_FOTA_START:
            {
                int iscontinue = -1;
                char url_old[80];

                if (aos_kv_getstring(FW_URL_KEY, url_old, 80) > 0) {
                    // LOGD(TAG, "get old url %s", url_old);
                    iscontinue = (strcmp(data->url, url_old) == 0 ? 1 : 0);
                } else if (data->url != NULL) {
                    // LOGD(TAG, "get data fail");
                    iscontinue = 0;
                }

                if (iscontinue == 0) {
                    if (data != NULL && data->url != NULL && (aos_kv_setstring(FW_URL_KEY, data->url) < 0)) {
                        LOGE(TAG, "set kv url fail");
                    }

                    aos_kv_setint("fota_offset", 0);
                }

                if (iscontinue >= 0) {
                    g_update = 1;
                }
            }
            break;
        case EVENT_FOTA_UPDATE:
            {
                extern void oceancon_fota_reboot(struct fota_data *f_data);
                oceancon_fota_reboot(data);
            }
            break;
        default:;
    }
}

static int coap_version_check(fota_info_t *info)
{
    int rc = g_update;
    if (rc == 1) {
        char url[80];
        if (aos_kv_getstring(FW_URL_KEY, url, 80) > 0) {
            if (info->fota_url) {
                free(info->fota_url);
                info->fota_url = NULL;
            }
            info->fota_url = strdup(url);
            // LOGD(TAG, "get path pass");
        } else if (data->url != NULL){
            if (info->fota_url) {
                free(info->fota_url);
                info->fota_url = NULL;
            }
            info->fota_url = strdup(data->url);
            // LOGD(TAG, "get kv fail");
        } else {
            rc = 0;
            LOGE(TAG, "get url fail");
        }
        if (data->url) {
            free(data->url);
            data->url = NULL;
        }
        g_update = 0;
    }
    // LOGD(TAG, "rc: %x", rc);
    return rc > 0 ? 0 : -1;
}

int coap_init(void)
{
    event_subscribe(EVENT_FOTA_START, coap_event_cb, NULL);
    event_subscribe(EVENT_FOTA_UPDATE, coap_event_cb, NULL);
    return 0;
}

static int coap_finish(void)
{
    extern void oceancon_fota_finish(struct fota_data *f_data);
    oceancon_fota_finish(data);
    return 0;
}

static int coap_fail(void)
{
    extern void oceancon_fota_fail(struct fota_data *f_data);
    oceancon_fota_fail(data);
    return 0;
}

const fota_cls_t fota_coap_cls = {
    "coap",
    coap_init,
    coap_version_check,
    coap_finish,
    coap_fail,
};

int fota_register_coap(void)
{
    return fota_register(&fota_coap_cls);
}
