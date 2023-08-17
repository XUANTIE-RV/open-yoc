#include <ulog/ulog.h>
#include <aos/kv.h>
#include <aos/kernel.h>
#include <uservice/eventid.h>
#include <uservice/uservice.h>
#include "JSFotaImpl.h"
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
#include <yoc/ota_ab.h>
#endif

#define KV_FOTA_CYCLE_MS        "fota_cycle"
#define KV_FOTA_START_MS        "fota_start"
#define KV_FOTA_ENABLED         "fota_en"
#define FOTA_CYCLE_DELAY_TIME   (20000)
#define FOTA_START_DELAY_TIME   (0)

#define TAG "JSFota"

extern void publish_fota_jsapi(int event, void *P);

namespace aiot
{
int js_fota_delay_timer = 2500;
static fota_t *js_fota_handle = NULL;
static int fota_cb_status;

void fota_user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (event_id == EVENT_FOTA_START) {
        fota_start(js_fota_handle);
    }
    else {
        ;
    }
}

struct timespec diff_timespec(struct timespec start, struct timespec end)
{
     struct timespec result;
 
     if (end.tv_nsec < start.tv_nsec)
     { 
        result.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;        
        result.tv_sec = end.tv_sec - 1 - start.tv_sec;
     }
     else
     {
        result.tv_nsec = end.tv_nsec - start.tv_nsec;        
        result.tv_sec = end.tv_sec - start.tv_sec;
     }
 
    return result;
}

static int fota_event_cb(void *arg, fota_event_e event)
{
    static int data_offset;
    static struct timespec tv;
    fota_t *fota = (fota_t *)arg;
    
    switch (event) {
        case FOTA_EVENT_START:
            LOGD(TAG, "FOTA START :%x", fota->status);
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
            otaab_start();
#endif
        fota_cb_status = 0;
            break;
        case FOTA_EVENT_VERSION:
        {
            LOGD(TAG, "FOTA VERSION CHECK :%x", fota->status);
//            FotaVersion *version  = (FotaVersion *) malloc(sizeof(FotaVersion));
            FotaVersion *version = new FotaVersion();
            data_offset = 0;
            memset(&tv, 0, sizeof(struct timespec));
            cJSON *root = cJSON_CreateObject();
            if (fota->error_code != FOTA_ERROR_NULL) {
                cJSON_AddNumberToObject(root, "code", 1);
                cJSON_AddStringToObject(root, "msg", "version check failed!");
                (*version).code = 1;
                version->msg = "version check failed!";
            } else {
                cJSON_AddNumberToObject(root, "code", 0);
                cJSON_AddStringToObject(root, "curversion", fota->info.cur_version);
                cJSON_AddStringToObject(root, "newversion", fota->info.new_version);
                cJSON_AddNumberToObject(root, "timestamp", fota->info.timestamp);
                cJSON_AddStringToObject(root, "changelog", fota->info.changelog);
                cJSON_AddStringToObject(root, "local_changelog", fota->info.local_changelog);
                (*version).code = 0;
                (*version).curversion = aos_get_app_version();
                (*version).newversion = (*fota).info.new_version;
                (*version).deviceid = aos_get_device_id();
                (*version).model = aos_get_product_model();
                if(fota->info.local_changelog == NULL){
                    (*version).local_changelog = "null";
                }else{
                    (*version).local_changelog = fota->info.local_changelog; //不能为空
                }
                (*version).changelog = fota->info.changelog;
                (*version).msg = "Check ok!";
            }
            char *out = cJSON_PrintUnformatted(root);
            cJSON_Delete(root);
            if (out != NULL) {
                LOGD(TAG, out);
                cJSON_free(out);
            }

            publish_fota_jsapi(0, version);
            
            break;
        }
        case FOTA_EVENT_PROGRESS:
        {
            LOGD(TAG, "FOTA PROGRESS :%x, %d, %d", fota->status, fota->offset, fota->total_size);
//            FotaDownload *downloadInfo = (FotaDownload *) malloc(5 * sizeof(FotaDownload));
            FotaDownload *downloadInfo = new FotaDownload();
            int64_t cur_size = fota->offset;
            int64_t total_size = fota->total_size;
            int speed = 0; //kbps
            int percent = 0;
            if (total_size > 0) {
                percent = (int)(cur_size * 100 / total_size);
            }
            cJSON *root = cJSON_CreateObject();

            if (js_fota_handle->error_code != FOTA_ERROR_NULL) {
                cJSON_AddNumberToObject(root, "code", 1);
                (* downloadInfo).code = 1;
                if (js_fota_handle->error_code == FOTA_ERROR_VERIFY) {
                    cJSON_AddStringToObject(root, "msg", "fota image verify failed!");
                    (* downloadInfo).msg = "fota image verify failed!";
                } else {
                    cJSON_AddStringToObject(root, "msg", "download failed!");
                    (* downloadInfo).msg = "download failed!";
                }
                cJSON_AddNumberToObject(root, "total_size", total_size);
                cJSON_AddNumberToObject(root, "cur_size", cur_size);
                cJSON_AddNumberToObject(root, "percent", percent);
                (* downloadInfo).cur_size = cur_size;
                (* downloadInfo).total_size = total_size;
                (* downloadInfo).percent = percent;
            } else {
                // current_size, total_size, percent, speed
                struct timespec now;
                clock_gettime(CLOCK_MONOTONIC, &now);
                if (tv.tv_sec > 0 && tv.tv_nsec > 0 && data_offset > 0) {
                    struct timespec diff = diff_timespec(tv, now);
                    int millisecond = diff.tv_sec * 1000 + diff.tv_nsec / 1000000;
                    LOGD(TAG, "interval time: %d ms", millisecond);
                    speed = ((js_fota_handle->offset - data_offset) / 1024) * 1000 / millisecond;
                }
                cJSON_AddNumberToObject(root, "code", 0);
                cJSON_AddNumberToObject(root, "total_size", total_size);
                cJSON_AddNumberToObject(root, "cur_size", cur_size);
                cJSON_AddNumberToObject(root, "percent", percent);
                cJSON_AddNumberToObject(root, "speed", speed);
                (* downloadInfo).code = 0;
                (* downloadInfo).total_size = total_size;
                (* downloadInfo).cur_size = cur_size;
                (* downloadInfo).percent = percent;
                (* downloadInfo).spend = speed;
                tv.tv_sec = now.tv_sec;
                tv.tv_nsec = now.tv_nsec;
                data_offset = js_fota_handle->offset;
            }
            char *out = cJSON_PrintUnformatted(root);
            cJSON_Delete(root);
            if (out != NULL) {
                LOGD(TAG, out);
                cJSON_free(out);
            }

            publish_fota_jsapi(1, downloadInfo);

            if (percent == 100)
            {
//                FotaEnd *E = (FotaEnd *) malloc(sizeof(FotaEnd));
                FotaEnd *E = new FotaEnd();
                (*E).code = 0;
                (*E).msg = "download finish!";
                publish_fota_jsapi(2, E);

            }
            break;          
        }
            
        case FOTA_EVENT_FAIL:
            LOGD(TAG, "FOTA FAIL :%x, %d", fota->status, fota->error_code);
            fota_cb_status = 3;
            break;
        case FOTA_EVENT_VERIFY:
            LOGD(TAG, "FOTA VERIFY :%x", fota->status);
            break;
        case FOTA_EVENT_FINISH:
            LOGD(TAG, "FOTA FINISH :%x", fota->status);
            fota_cb_status = 5;
            break;
        case FOTA_EVENT_RESTART:
            LOGD(TAG, "FOTA RESTART :%x", fota->status);
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
            char buffer[64];
            memset(buffer, 0, sizeof(buffer));
            if (aos_kv_getstring(KV_COP_VERSION, buffer, sizeof(buffer)) > 0) {
                otaab_upgrade_end((const char *)buffer);
            }
#endif
            // reboot to upgrade by bootloader;
            LOGD(TAG, "I am going to reboot...");
            aos_reboot();
            break;
        case FOTA_EVENT_QUIT:
            LOGD(TAG, "FOTA QUIT :%x", fota->status);
            break;
        default:
            break;
    }
    return 0;
}

static void js_fota_init()
{
    int ret;
    int fota_en = 1;
    int read_timeoutms;         /*!< read timeout, millisecond */
    int write_timeoutms;        /*!< write timeout, millisecond */
    int retry_count;            /*!< when download abort, it will retry to download again in retry_count times */
    int sleep_time;             /*!< the sleep time for auto-check task */
    int auto_check_en;          /*!< whether check version automatic */

#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
    otaab_finish(1);
#endif
    char * curVersion=aos_get_app_version();
    LOGI(TAG, "fota_start");
    LOGI(TAG, "ver=============> %s", curVersion);
    LOGI(TAG, "deviceid========> %s", aos_get_device_id());
    LOGI(TAG, "model===========> %s", aos_get_product_model());
    ret = aos_kv_getint(KV_FOTA_ENABLED, &fota_en);
    if (ret != 0 && fota_en == 0) {
        aos_kv_setint(KV_FOTA_ENABLED, 0);
    }
    aos_kv_setstring(KV_COP_VERSION, curVersion); 
    aos_kv_setstring("cur_version", curVersion); 
    fota_register_cop();
    netio_register_httpc(NULL);
#if defined(CONFIG_OTA_AB) && (CONFIG_OTA_AB > 0)
    netio_register_flashab();
#else
    netio_register_flash();
#endif
    js_fota_handle = fota_open("cop", "flash://misc", fota_event_cb);

    if (aos_kv_getint(KV_FOTA_READ_TIMEOUTMS, &read_timeoutms) < 0) {
        read_timeoutms = 20000;
    }
    if (aos_kv_getint(KV_FOTA_WRITE_TIMEOUTMS, &write_timeoutms) < 0) {
        write_timeoutms = 20000;
    }
    if (aos_kv_getint(KV_FOTA_RETRY_COUNT, &retry_count) < 0) {
        retry_count = 3;
    }
    if (aos_kv_getint(KV_FOTA_AUTO_CHECK, &auto_check_en) < 0) {
        auto_check_en = 0;
    }
    if (aos_kv_getint(KV_FOTA_SLEEP_TIMEMS, &sleep_time) < 0) {
        sleep_time = FOTA_CYCLE_DELAY_TIME;
    }

    js_fota_handle->config.read_timeoutms = read_timeoutms;
    js_fota_handle->config.write_timeoutms = write_timeoutms;
    js_fota_handle->config.retry_count = retry_count;
    js_fota_handle->config.auto_check_en = auto_check_en;
    js_fota_handle->config.sleep_time = sleep_time;

    ret = aos_kv_getint(KV_FOTA_START_MS, &js_fota_delay_timer);
    if (ret != 0) {
        js_fota_delay_timer = FOTA_START_DELAY_TIME;
    }
    event_subscribe(EVENT_FOTA_START, fota_user_local_event_cb, NULL);
}

void JS_Fota_sart(void)
{
    int ret;
    int fota_en = 1;
    ret = aos_kv_getint(KV_FOTA_ENABLED, &fota_en);
    if (ret == 0 && fota_en == 0) {
        aos_kv_setint(KV_FOTA_ENABLED, 1);
    }

    event_publish_delay(EVENT_FOTA_START, NULL, js_fota_delay_timer);  
}

void JS_Fota_stop(void)
{
    int ret;
    int fota_en = 1;
    ret = aos_kv_getint(KV_FOTA_ENABLED, &fota_en);
    if(0 == ret && 0 == fota_en)
        return;
    aos_kv_setint(KV_FOTA_ENABLED, 0);
    fota_stop(js_fota_handle);
}

int JS_Fota_get_status(void)
{
    return(fota_get_status(js_fota_handle));
}

int JS_Fota_versionCheck(void)
{
    fota_do_check(js_fota_handle);
    return 0;
}

int JS_Fota_download(void)
{
    if(!fota_download(js_fota_handle))
    {return 0;}
    else
    {return -1;}
}

void JSFotaImpl::init(JQuick::sp<JQPublishObject> pub)
{
    _pub = pub;
    js_fota_init();
    FotaStatus fotasta;
    fotasta.status = "ready";
    _pub->publish(JS_FOTA_EVENT_READY, Bson());
}


int JSFotaImpl::start(void)
{
    // TODO
    JS_Fota_sart();
    return 0;
}

int JSFotaImpl::stop(void)
{
    // TODO
    JS_Fota_stop();
    return 0;
}

std::string JSFotaImpl::getState(void)
{
    // TODO: "idle", "download", "abort","finish"
    std::string state;
    switch (JS_Fota_get_status())
    {
    case 1:
        state = "idle";
        break;
    case 2:
        state = "download";
        break;
    case 3:
        state = "abort";
        break;
    case 4:
        state = "finish";
        break;
    default:
        break;
    }
    return state;
}

int JSFotaImpl::versionCheck(void)
{
    // TODO
    if(!JS_Fota_versionCheck())
    {   
        return 0;}
    else
    {   return -1;}
    
}

int JSFotaImpl::download(void)
{
    // TODO
    if(0 == JS_Fota_download())
    {   
        return 0;
    }else{
        return -1;
    }
}

int JSFotaImpl::restart(int delayms)
{
    // TODO
    LOGD(TAG, "I am going to reboot...");
    // FRestart *R = ((FRestart *) malloc(sizeof(FRestart)));
    // (*R).code = 0;
    // (*R).msg = "I am going to reboot...";
    // if(publish_fota_jsapi(3, R))

    aos_msleep(delayms);
    aos_reboot();
    return 0;
}

} // namespace aiot
