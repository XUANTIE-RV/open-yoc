/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/cli.h>
#include <aos/kv.h>
#include <aos/yloop.h>
#include <cJSON.h>
#include <http_client.h>
#include <key_mgr.h>
#include <yoc/fota.h>

//#include <yoc/lpm.h>
//#include "yoc/partition.h"

//#include <uservice/eventid.h>
//#include <uservice/uservice.h>

#include "app_config.h"
#include "app_fota.h"

#define FOTA_DEVICE_CHECK_INTERVAL (10000)
#define FOTA_DEVICE_RECHECK        (1000)

#define KV_FOTA_CYCLE_MS "fota_cycle"
#define KV_FOTA_ENABLED  "fota_en"

#define DEFAULT_URL "https://occ.t-head.cn"

#define DEIVICE_STR_CID_LEN (32)

#define FOTA_OCC_FIRMWARE_ID_INVAIL (-1)

#define TAG "gw_fota"

extern int     get_version_str(uint32_t version, char *ver_str);
static int     gw_fota_loop_check_timeout = 0;
static fota_t *g_fota_handle              = NULL;

static char local_device_cid[DEIVICE_STR_CID_LEN + 1] = { 0 };
extern int  board_ota_gateway_upgrade(const void *image, uint32_t size);

enum fota_st_en
{
    FOTA_ST_IDLE = 0,
    FOTA_ST_NEW_VERSION,
    FOTA_ST_DOWNLOADING,
    FOTA_ST_UPDATING,
    FOTA_ST_OVER,

    FOTA_ST_INVAILD,
};

typedef struct fota_occ_firmware_t {
    slist_t  next;
    char *   fota_url;
    void *   image;
    uint32_t image_size;
    int      firmware_id;
} fota_occ_firmware_t;

typedef struct fota_occ_device_t {
    slist_t              next;
    char *               cid;
    char *               version;
    fota_occ_firmware_t *firmware;
    gw_subdev_t          subdev;
    enum fota_st_en      state;
    long long            last_check_time;
} fota_occ_device_t;

struct fota_occ_ctx_t {
    uint8_t              enable_flag;  // 1:enable;0:disable
    uint8_t              ota_flag;     // 1:in ota;0:not in
    slist_t              device_list;  // devices that need fota
    slist_t              upgrade_list; // devices that need upgrade
    slist_t              firmware_list;
    fota_occ_firmware_t *firmware;
    fota_occ_device_t *  device; // device in upgrade
    aos_mutex_t          mutex;
} fota_occ_ctx;

void cli_reg_cmd_gateway_fota(void);

static void _app_fota_recheck(int ms);
static void _fota_set_auto_check(int enable);
void        app_fota_do_check(void);
static int  _fota_occ_device_update_finish(const char *cid, const char *version);

static void *_fota_ota_image_alloc(void *image, uint32_t size)
{
    void *ret_image = aos_realloc(image, size);

    LOGI(TAG, "alloc image %p, %d\n", ret_image, size);

    return ret_image;
}

static void _fota_ota_image_free(void *image)
{
    LOGI(TAG, "free image 0x%p\n", image);

    if (!image) {
        return;
    }
    return aos_free(image);
}

unsigned long fota_data_address_get(void)
{
    if (fota_occ_ctx.device) {
        return (unsigned long)fota_occ_ctx.device->firmware->image;
    }

    return 0;
}

static int _fota_occ_is_local_devcie(const char *cid)
{
    if (!strncmp(cid, local_device_cid, DEIVICE_STR_CID_LEN)) {
        return 1;
    }

    return 0;
}

static fota_occ_firmware_t *_fota_occ_firmware_new(int firmware_id, char *fota_url, void *image, uint32_t size)
{
    aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);
    fota_occ_firmware_t *firmware = aos_zalloc(sizeof(fota_occ_firmware_t));

    if (firmware) {
        firmware->firmware_id = firmware_id;
        firmware->image       = image;
        firmware->image_size  = size;
        firmware->fota_url    = fota_url;
        slist_add_tail(&firmware->next, &fota_occ_ctx.firmware_list);
        LOGI(TAG, "add firmware %d\n", firmware_id);
    }

    aos_mutex_unlock(&fota_occ_ctx.mutex);

    return firmware;
}

static int _fota_occ_firmware_del(fota_occ_firmware_t *firmware)
{
    fota_occ_firmware_t *node;
    slist_t *            tmp;

    aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);

    /* check if there is new version for all nodes */
    slist_for_each_entry_safe(&fota_occ_ctx.firmware_list, tmp, node, fota_occ_firmware_t, next)
    {
        if (node == firmware) {
            slist_del(&node->next, &fota_occ_ctx.firmware_list);

            if (node->image) {
                _fota_ota_image_free(node->image);
            }

            if (node->fota_url) {
                aos_free(node->fota_url);
            }

            aos_free(node);
            break;
        }
    }
    aos_mutex_unlock(&fota_occ_ctx.mutex);

    return 0;
}

static fota_occ_firmware_t *_fota_occ_firmware_get_by_fota_url(char *fota_url)
{
    fota_occ_firmware_t *firmware = NULL;
    slist_t *            tmp;

    if (!fota_url) {
        return NULL;
    }

    aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);

    /* check if there is new version for all nodes */
    slist_for_each_entry_safe(&fota_occ_ctx.firmware_list, tmp, firmware, fota_occ_firmware_t, next)
    {
        if (!strncmp(fota_url, firmware->fota_url, strlen(firmware->fota_url))) {
            break;
        }
    }
    aos_mutex_unlock(&fota_occ_ctx.mutex);

    return firmware;
}

static fota_occ_firmware_t *_fota_occ_firmware_get_by_id(int firmware_id)
{
    fota_occ_firmware_t *firmware = NULL;
    slist_t *            tmp;

    aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);

    /* check if there is new version for all nodes */
    slist_for_each_entry_safe(&fota_occ_ctx.firmware_list, tmp, firmware, fota_occ_firmware_t, next)
    {
        if (firmware_id == firmware->firmware_id) {
            break;
        }
    }
    aos_mutex_unlock(&fota_occ_ctx.mutex);

    return firmware;
}

static int _fota_occ_updata_start()
{
    int ret = 0;
    aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);
    fota_occ_ctx.device->state = FOTA_ST_UPDATING;

    if (fota_occ_ctx.device && fota_occ_ctx.device->firmware->image && fota_occ_ctx.device->firmware->image_size) {
        if (_fota_occ_is_local_devcie(fota_occ_ctx.device->cid)) {
            ret                = board_ota_gateway_upgrade(fota_occ_ctx.device->firmware->image,
                                            fota_occ_ctx.device->firmware->image_size);
        } else {
            LOGI(TAG, "Node Upgrading");

            ret = gateway_subdev_ota(&fota_occ_ctx.device->firmware->firmware_id, &fota_occ_ctx.device->subdev, 1,
                                     fota_occ_ctx.device->firmware->image, fota_occ_ctx.device->firmware->image_size);
            if (ret) {
                LOGE(TAG, "Node Ota failed");
            }
        }
    } else {
        LOGE(TAG, "FOTA Device loss");
    }

    aos_mutex_unlock(&fota_occ_ctx.mutex);

    return ret;
}

/**
 *  return 0: still do the default handle
 *  not zero: only do the user handle
 */
static int _fota_event_cb(void *arg, fota_event_e event)
{
    int     ret  = 0;
    fota_t *fota = (fota_t *)arg;
    LOGD(TAG, "fota event %d\n", event);

    switch (event) {
        case FOTA_EVENT_VERSION:
            if (fota_occ_ctx.device->state != FOTA_ST_NEW_VERSION) {
                return -1;
            }

            /* get new version, return -1 if we do not want upgrade */
            LOGI(TAG, "FOTA VERSION :%x", fota->status);
            aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);
            fota_occ_ctx.device->state = FOTA_ST_DOWNLOADING;
            aos_mutex_unlock(&fota_occ_ctx.mutex);
            break;

        case FOTA_EVENT_FAIL:

            /* fota download or flash error
             * it will try next version check
             */
            aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);

            if (fota_occ_ctx.device) {
                if (fota_occ_ctx.device->state == FOTA_ST_DOWNLOADING) {
                    LOGI(TAG, "FOTA Download fail :%x", fota->status);

                    if (fota_occ_ctx.device->firmware->image) {
                        _fota_ota_image_free(fota_occ_ctx.device->firmware->image);
                        fota_occ_ctx.device->firmware->image = NULL;
                    }

                    fota_occ_ctx.device->firmware->image_size = 0;
                    fota_occ_ctx.device->state                = FOTA_ST_IDLE;

                    slist_del(&fota_occ_ctx.device->next, &fota_occ_ctx.upgrade_list);
                    slist_add_tail(&fota_occ_ctx.device->next, &fota_occ_ctx.device_list);

                    _fota_occ_firmware_del(fota_occ_ctx.device->firmware);
                    fota_occ_ctx.device->firmware = NULL;
                    fota_occ_ctx.device           = NULL;
                }
            }

            aos_mutex_unlock(&fota_occ_ctx.mutex);
            LOGE(TAG, "FOTA FAIL :%x, err code %d", fota->status, fota->error_code);
            break;

        case FOTA_EVENT_FINISH:
            LOGI(TAG, "FOTA FINISH :%x", fota->status);
            _fota_occ_updata_start();
            // aos_reboot();
            break;

        default:
            break;
    }

    return ret;
}

static int _flash_open(netio_t *io, const char *path)
{
    struct fota_occ_ctx_t *ctx = &fota_occ_ctx;

    if (ctx && ctx->device) {
        ctx->device->firmware->image = _fota_ota_image_alloc(NULL, CONFIG_OTA_GATEWAY_IMG_SIZE);

        if (!ctx->device->firmware->image) {
            LOGE(TAG, "devcie %s image alloc fail", ctx->device->cid);
            return -1;
        }

        ctx->device->firmware->image_size = 0;
        io->size                          = CONFIG_OTA_GATEWAY_IMG_SIZE;
        io->block_size                    = 128;

        io->private = ctx;

        return 0;
    }

    return -1;
}

static int _flash_close(netio_t *io)
{
    return 0;
}

static int _flash_read(netio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    struct fota_occ_ctx_t *ctx = (struct fota_occ_ctx_t *)io->private;

    if (io->size - io->offset < length) {
        length = io->size - io->offset;
    }

    memcpy(buffer, ctx->device->firmware->image + io->offset, length);
    io->offset += length;

    return length;
}

static int _flash_write(netio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    struct fota_occ_ctx_t *ctx = (struct fota_occ_ctx_t *)io->private;

    // LOGD(TAG, "%d %d %d\n", io->size, io->offset, length);
    if (io->size - io->offset < length) {
        uint8_t *image = _fota_ota_image_alloc(ctx->device->firmware->image, io->size * 2);

        if (!image) {
            LOGE(TAG, "image realloc fail");
            return -1;
        }

        ctx->device->firmware->image = image;
        io->size                     = io->size * 2;

        LOGI(TAG, "image realloc size %d", io->size * 2);
    }

    memcpy(ctx->device->firmware->image + io->offset, buffer, length);
    io->offset += length;
    ctx->device->firmware->image_size += length;

    return length;
}

static int _flash_seek(netio_t *io, size_t offset, int whence)
{
    // partition_t handle = (partition_t)io->private;

    switch (whence) {
        case SEEK_SET:
            io->offset = offset;
            return 0;

        case SEEK_CUR:
            io->offset += offset;
            return 0;

        case SEEK_END:
            io->offset = io->size - offset;
            return 0;
    }

    return -1;
}

const netio_cls_t fota_occ_flash = {
    .name  = "flash",
    .open  = _flash_open,
    .close = _flash_close,
    .write = _flash_write,
    .read  = _flash_read,
    .seek  = _flash_seek,
};

static int _cop_get_ota_url(char *ota_url, int len)
{
    int  ret         = 0;
    char occ_url[40] = { 0 };
    int  occ_url_len = sizeof(occ_url);

    ret = aos_kv_get("occurl", occ_url, &occ_url_len);
    if (ret != 0) {
        strcpy(occ_url, DEFAULT_URL);
    }

    snprintf(ota_url, len, "%s%s", occ_url, "/api/image/ota/pull");

    return ret;
}

static int _http_event_handler(http_client_event_t *evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            LOGD(TAG, "HTTP_EVENT_ERROR");
            break;

        case HTTP_EVENT_ON_CONNECTED:
            LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;

        case HTTP_EVENT_HEADER_SENT:
            LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;

        case HTTP_EVENT_ON_HEADER:
            // LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;

        case HTTP_EVENT_ON_DATA:
            // LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            break;

        case HTTP_EVENT_ON_FINISH:
            LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;

        case HTTP_EVENT_DISCONNECTED:
            LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }

    return 0;
}

static bool _process_again(int status_code)
{
    switch (status_code) {
        case HttpStatus_MovedPermanently:
        case HttpStatus_Found:
        case HttpStatus_TemporaryRedirect:
        case HttpStatus_Unauthorized:
            return true;

        default:
            return false;
    }

    return false;
}

static http_errors_t _http_handle_response_code(http_client_handle_t http_client, int status_code, char *buffer,
                                                int buf_size, int data_size)
{
    http_errors_t err;

    if (status_code == HttpStatus_MovedPermanently || status_code == HttpStatus_Found
        || status_code == HttpStatus_TemporaryRedirect)
    {
        err = http_client_set_redirection(http_client);

        if (err != HTTP_CLI_OK) {
            LOGE(TAG, "URL redirection Failed");
            return err;
        }
    } else if (status_code == HttpStatus_Unauthorized) {
        return HTTP_CLI_FAIL;
    } else if (status_code == HttpStatus_NotFound || status_code == HttpStatus_Forbidden) {
        LOGE(TAG, "File not found(%d)", status_code);
        return HTTP_CLI_FAIL;
    } else if (status_code == HttpStatus_InternalError) {
        LOGE(TAG, "Server error occurred(%d)", status_code);
        return HTTP_CLI_FAIL;
    }

    // _process_again() returns true only in case of redirection.
    if (data_size > 0 && _process_again(status_code)) {
        /*
         *  In case of redirection, http_client_read() is called
         *  to clear the response buffer of http_client.
         */
        int data_read;

        while (data_size > buf_size) {
            data_read = http_client_read(http_client, buffer, buf_size);

            if (data_read <= 0) {
                return HTTP_CLI_OK;
            }

            data_size -= buf_size;
        }

        data_read = http_client_read(http_client, buffer, data_size);

        if (data_read <= 0) {
            return HTTP_CLI_OK;
        }
    }

    return HTTP_CLI_OK;
}

static http_errors_t _http_connect(http_client_handle_t http_client, const char *payload, char *buffer, int buf_size)
{
#define MAX_REDIRECTION_COUNT 10
    http_errors_t err              = HTTP_CLI_FAIL;
    int           status_code      = 0, header_ret;
    int           redirect_counter = 0;

    do {
        if (redirect_counter++ > MAX_REDIRECTION_COUNT) {
            LOGE(TAG, "redirect_counter is max");
            return HTTP_CLI_FAIL;
        }

        if (_process_again(status_code)) {
            LOGD(TAG, "process again,status code:%d", status_code);
        }

        err = http_client_open(http_client, strlen(payload));

        if (err != HTTP_CLI_OK) {
            LOGE(TAG, "Failed to open HTTP connection");
            return err;
        }

        int wlen = http_client_write(http_client, payload, strlen(payload));

        if (wlen < 0) {
            LOGE(TAG, "Write payload failed");
            return HTTP_CLI_FAIL;
        }

        LOGD(TAG, "write payload ok...");
        header_ret = http_client_fetch_headers(http_client);

        if (header_ret < 0) {
            LOGE(TAG, "header_ret:%d", header_ret);
            return header_ret;
        }

        LOGD(TAG, "header_ret:%d", header_ret);
        status_code = http_client_get_status_code(http_client);
        LOGD(TAG, "status code:%d", status_code);
        err = _http_handle_response_code(http_client, status_code, buffer, buf_size, header_ret);

        if (err != HTTP_CLI_OK) {
            LOGE(TAG, "e handle resp code:%d", err);
            return err;
        }
    } while (_process_again(status_code));

    return err;
}

static void _http_cleanup(http_client_handle_t client)
{
    if (client) {
        http_client_cleanup(client);
    }
}

static int _fota_occ_rpt_version(const char *cid, const char *version)
{
    int                  ret     = 0;
    char *               payload = NULL;
    char                 getvalue[64];
    char *               buffer = NULL;
    http_errors_t        err;
    http_client_config_t config = { 0 };
    http_client_handle_t client = NULL;

    if (cid == NULL || version == NULL) {
        return -1;
    }

    buffer = aos_zalloc(BUFFER_SIZE + 1);

    if (buffer == NULL) {
        ret = -ENOMEM;
        goto out;
    }

    if ((payload = aos_malloc(156)) == NULL) {
        ret = -ENOMEM;
        goto out;
    }

    snprintf(payload, 156, "{\"cid\":\"%s\",\"type\":\"report\",\"version\":\"%s\"}", cid, version);

    LOGD(TAG, "rpt version to occ: %s", payload);

    memset(getvalue, 0, sizeof(getvalue));
    _cop_get_ota_url(getvalue, sizeof(getvalue));
    LOGD(TAG, "ota url:%s", getvalue);

    config.method        = HTTP_METHOD_POST;
    config.url           = getvalue;
    config.timeout_ms    = 10000;
    config.buffer_size   = BUFFER_SIZE;
    config.event_handler = _http_event_handler;
    LOGD(TAG, "http client init start.");
    client = http_client_init(&config);

    if (!client) {
        LOGE(TAG, "Client init e");
        ret = -1;
        goto out;
    }

    LOGD(TAG, "http client init ok.");
    http_client_set_header(client, "Content-Type", "application/json");
    http_client_set_header(client, "Connection", "keep-alive");
    http_client_set_header(client, "Cache-Control", "no-cache");
    err = _http_connect(client, payload, buffer, BUFFER_SIZE);

    if (err != HTTP_CLI_OK) {
        LOGE(TAG, "Client connect e");
        ret = -1;
        goto out;
    }

out:

    if (buffer) {
        aos_free(buffer);
    }

    if (payload) {
        aos_free(payload);
    }

    _http_cleanup(client);
    return ret;
}

static int _fota_occ_url_get(const char *cid, const char *version, char **fota_url)
{
    int                  ret     = 0;
    char *               payload = NULL;
    char                 getvalue[64];
    cJSON *              js     = NULL;
    char *               buffer = NULL;
    http_errors_t        err;
    http_client_config_t config = { 0 };
    http_client_handle_t client = NULL;

    if (cid == NULL || version == NULL || fota_url == NULL) {
        return -1;
    }

    buffer = aos_zalloc(BUFFER_SIZE + 1);

    if (buffer == NULL) {
        ret = -ENOMEM;
        goto out;
    }

    if ((payload = aos_malloc(156)) == NULL) {
        ret = -ENOMEM;
        goto out;
    }

    snprintf(payload, 156, "{\"cid\":\"%s\",\"version\":\"%s\"}", cid, version);

    LOGD(TAG, "check: %s", payload);

    memset(getvalue, 0, sizeof(getvalue));
    _cop_get_ota_url(getvalue, sizeof(getvalue));
    LOGD(TAG, "ota url:%s", getvalue);

    config.method        = HTTP_METHOD_POST;
    config.url           = getvalue;
    config.timeout_ms    = 10000;
    config.buffer_size   = BUFFER_SIZE;
    config.event_handler = _http_event_handler;
    LOGD(TAG, "http client init start.");
    client = http_client_init(&config);

    if (!client) {
        LOGE(TAG, "Client init e");
        ret = -1;
        goto out;
    }

    LOGD(TAG, "http client init ok.");
    http_client_set_header(client, "Content-Type", "application/json");
    http_client_set_header(client, "Connection", "keep-alive");
    http_client_set_header(client, "Cache-Control", "no-cache");
    err = _http_connect(client, payload, buffer, BUFFER_SIZE);

    if (err != HTTP_CLI_OK) {
        LOGE(TAG, "Client connect e");
        ret = -1;
        goto out;
    }

    int read_len = http_client_read(client, buffer, BUFFER_SIZE);

    if (read_len <= 0) {
        ret = -1;
        goto out;
    }

    buffer[read_len] = 0;
    LOGD(TAG, "resp: %s", buffer);

    js = cJSON_Parse(buffer);

    if (js == NULL) {
        ret = -1;
        LOGW(TAG, "cJSON_Parse failed");
        goto out;
    }

    cJSON *code = cJSON_GetObjectItem(js, "code");

    if (!(code && cJSON_IsNumber(code))) {
        ret = -1;
        LOGW(TAG, "get code failed");
        goto out;
    }

    LOGD(TAG, "code: %d", code->valueint);

    if (code->valueint < 0) {
        ret = -1;
        goto out;
    }

    cJSON *result = cJSON_GetObjectItem(js, "result");

    if (!(result && cJSON_IsObject(result))) {
        LOGW(TAG, "get result failed");
        ret = -1;
        goto out;
    }

    cJSON *js_version = cJSON_GetObjectItem(result, "version");

    if (!(version && cJSON_IsString(js_version))) {
        LOGW(TAG, "get version failed");
        ret = -1;
        goto out;
    }

    LOGD(TAG, "version: %s", js_version->valuestring);

    cJSON *url = cJSON_GetObjectItem(result, "url");

    if (!(url && cJSON_IsString(url))) {
        ret = -1;
        LOGW(TAG, "get url failed");
        goto out;
    }

    LOGD(TAG, "url: %s", url->valuestring);

    if (*fota_url) {
        free(*fota_url);
        *fota_url = NULL;
    }

    *fota_url = aos_zalloc(strlen(url->valuestring) + 1);

    if (strstr(url->valuestring, "https://")) {
        snprintf(*fota_url, strlen(url->valuestring), "http://%s", url->valuestring + strlen("https://"));
    } else {
        memcpy(*fota_url, url->valuestring, strlen(url->valuestring));
    }

    aos_kv_setint("fota_offset", 0);

    LOGD(TAG, "get url: %s", *fota_url);

    ret = 0;
out:

    if (buffer) {
        aos_free(buffer);
    }

    if (payload) {
        aos_free(payload);
    }

    if (js) {
        cJSON_Delete(js);
    }

    _http_cleanup(client);
    return ret;
}

int _fota_new_version_check(char *cid)
{
    fota_occ_device_t *device;
    slist_t *          tmp;
    int                ret;
    int                new_version = 0;

    aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);

    /* check if there is new version for all nodes */
    slist_for_each_entry_safe(&fota_occ_ctx.device_list, tmp, device, fota_occ_device_t, next)
    {
        if (cid == NULL || (cid && !strncmp(device->cid, cid, DEIVICE_STR_CID_LEN)))
            if (device->state == FOTA_ST_IDLE) {
                char *fota_url = NULL;
                ret            = _fota_occ_url_get(device->cid, device->version, &fota_url);

                if (ret == 0) {

                    fota_occ_firmware_t *firmware;

                    firmware = _fota_occ_firmware_get_by_fota_url(fota_url);

                    if (!firmware) {
                        firmware = _fota_occ_firmware_new(FOTA_OCC_FIRMWARE_ID_INVAIL, fota_url, NULL, 0);
                    }

                    if (!firmware) {
                        LOGE(TAG, "new firmware add fail");
                        aos_mutex_unlock(&fota_occ_ctx.mutex);
                        goto ret_version;
                    }

                    device->firmware = firmware;
                    device->state    = FOTA_ST_NEW_VERSION;
                    slist_del(&device->next, &fota_occ_ctx.device_list);
                    slist_add_tail(&device->next, &fota_occ_ctx.upgrade_list);

                    new_version = 1;
                }

                device->last_check_time = aos_now_ms();
            }
    }
    aos_mutex_unlock(&fota_occ_ctx.mutex);

ret_version:

    return new_version;
}

static int _fota_occ_version_check(fota_info_t *info)
{
    aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);
    fota_occ_device_t *device;
    slist_t *          tmp;

    /* the device is in fota process or no upgrade devices*/
    if (fota_occ_ctx.device || slist_empty(&fota_occ_ctx.upgrade_list)) {
        aos_mutex_unlock(&fota_occ_ctx.mutex);
        return -1;
    }

    /* return devcie that need upgrade */
    slist_for_each_entry_safe(&fota_occ_ctx.upgrade_list, tmp, device, fota_occ_device_t, next)
    {
        if (device->state == FOTA_ST_NEW_VERSION) {
            fota_occ_ctx.device = device;
            info->fota_url      = device->firmware->fota_url;
            aos_mutex_unlock(&fota_occ_ctx.mutex);
            return 0;
        }
    }

    aos_mutex_unlock(&fota_occ_ctx.mutex);
    return -1;
}

const fota_cls_t fota_occ_cls = {
    "gateway", NULL, _fota_occ_version_check, NULL, NULL,
};

static int _fota_occ_device_update_finish(const char *cid, const char *version)
{
    int ret;

    int device_update = 0;

    if (!cid || !version) {
        return -1;
    }

    aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);
    fota_occ_device_t *node = NULL;
    slist_t *          tmp;

    slist_for_each_entry_safe(&fota_occ_ctx.upgrade_list, tmp, node, fota_occ_device_t, next)
    {
        if (!node->version) {
            continue;
        }
        if (strlen(node->cid) == strlen(cid) && !strncmp(node->cid, cid, strlen(cid))) {
            if (strlen(node->version) == strlen(version) && !strncmp(node->version, version, strlen(version))) {
                LOGI(TAG, "device %s, already exist", node->cid);
            } else {
                char *new_version = aos_zalloc(strlen(version) + 1);

                if (!new_version) {
                    LOGE(TAG, "device $s, version update fail", node->cid);
                    aos_mutex_unlock(&fota_occ_ctx.mutex);
                    return -1;
                }

                strncpy(new_version, version, strlen(version));

                if (node->version) {
                    aos_free(node->version);
                }

                node->version = new_version;
                LOGI(TAG, "device %s version update to %s", node->cid, node->version);
            }

            slist_del(&node->next, &fota_occ_ctx.upgrade_list);
            slist_add_tail(&node->next, &fota_occ_ctx.device_list);
            node->state   = FOTA_ST_IDLE;
            device_update = 1;
            break;
        }
    }

    fota_occ_device_t *device = NULL;

    if (device_update) {
        slist_for_each_entry_safe(&fota_occ_ctx.upgrade_list, tmp, device, fota_occ_device_t, next)
        {
            if (device->state == FOTA_ST_NEW_VERSION
                && !strncmp(node->firmware->fota_url, device->firmware->fota_url, strlen(node->firmware->fota_url)))
            {

                device->firmware = node->firmware;
                device->state    = FOTA_ST_UPDATING;

                fota_occ_ctx.device = device;

                if (_fota_occ_is_local_devcie(device->cid)) {
                    ret = board_ota_gateway_upgrade(device->firmware->image, device->firmware->image_size);
                    if (ret) {
                        LOGE(TAG, "Board ota failed %d", ret);
                    }
                } else {
                    LOGI(TAG, "Node Upgrading");
                    int firmaware_index = 0;
                    ret = gateway_subdev_ota(&firmaware_index, &device->subdev, 1, device->firmware->image,
                                             device->firmware->image_size);
                    if (ret) {
                        LOGE(TAG, "add ota dev failed");
                        aos_mutex_unlock(&fota_occ_ctx.mutex);
                        return -1;
                    }
                }

                LOGI(TAG, "Reuse Firmware %d for %s", device->firmware->firmware_id, device->cid);
                aos_mutex_unlock(&fota_occ_ctx.mutex);
                return 0;
            }
        }
    }

    fota_occ_ctx.device = NULL;
    aos_mutex_unlock(&fota_occ_ctx.mutex);

    return 0;
}

int app_fota_device_add(const char *cid, const char *version, gw_subdev_t subdev)
{
    if (!fota_occ_ctx.enable_flag) {
        LOGE(TAG, "fota not enable");
        return -1;
    }

    if (!cid || !version) {
        return -1;
    }

    aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);
    fota_occ_device_t *node;
    slist_t *          tmp;

    /* check if there is new version for all nodes */
    slist_for_each_entry_safe(&fota_occ_ctx.device_list, tmp, node, fota_occ_device_t, next)
    {
        if (strlen(node->cid) == strlen(cid) && !strncmp(node->cid, cid, strlen(cid))) {
            if (strlen(node->version) == strlen(version) && !strncmp(node->version, version, strlen(version))) {
                LOGI(TAG, "device %s, already exist", node->cid);
                aos_mutex_unlock(&fota_occ_ctx.mutex);
                return 0;
            } else {
                char *new_version = aos_zalloc(strlen(version) + 1);

                if (!new_version) {
                    LOGE(TAG, "device $s, version update fail", node->cid);
                    aos_mutex_unlock(&fota_occ_ctx.mutex);
                    return -1;
                }

                strncpy(new_version, version, strlen(version));

                if (node->version) {
                    aos_free(node->version);
                }

                node->version = new_version;

                LOGI(TAG, "device %s version update to %s", node->cid, node->version);
                aos_mutex_unlock(&fota_occ_ctx.mutex);
                return 0;
            }
        }
    }

    slist_for_each_entry_safe(&fota_occ_ctx.upgrade_list, tmp, node, fota_occ_device_t, next)
    {
        if (strlen(node->cid) == strlen(cid) && !strncmp(node->cid, cid, strlen(cid))) {
            LOGE(TAG, "Devcie %s is in OTA process, can't update", cid);
            aos_mutex_unlock(&fota_occ_ctx.mutex);
            return -1;
        }
    }

    aos_mutex_unlock(&fota_occ_ctx.mutex);

    fota_occ_device_t *device;

    device = (fota_occ_device_t *)aos_zalloc(sizeof(fota_occ_device_t));

    if (!device) {
        LOGE(TAG, "Device alloc fail");
        return -1;
    }

    device->cid = aos_zalloc(strlen(cid) + 1);

    if (!device->cid) {
        aos_free(device);
        LOGE(TAG, "Device cid alloc fail");
        return -1;
    }

    strncpy(device->cid, cid, strlen(cid));

    device->version = aos_zalloc(strlen(version) + 1);

    if (!device->version) {
        aos_free(device->cid);
        aos_free(device);
        LOGE(TAG, "Device version alloc fail");
        return -1;
    }

    strncpy(device->version, version, strlen(version));

    device->subdev = subdev;
    device->state  = FOTA_ST_IDLE;

    aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);
    slist_add_tail(&device->next, &fota_occ_ctx.device_list);
    aos_mutex_unlock(&fota_occ_ctx.mutex);
    LOGI(TAG, "add new device %s", device->cid);
    return 0;
}

int app_fota_device_delete(const char *cid)
{
    if (!fota_occ_ctx.enable_flag) {
        LOGE(TAG, "fota not enable");
        return -1;
    }

    int                ret = -1;
    fota_occ_device_t *node;
    slist_t *          tmp;

    aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);
    slist_for_each_entry_safe(&fota_occ_ctx.device_list, tmp, node, fota_occ_device_t, next)
    {
        if (strlen(node->cid) == strlen(cid) && !strncmp(node->cid, cid, strlen(cid))) {
            if (node->state == FOTA_ST_IDLE) {
                slist_del(&node->next, &fota_occ_ctx.device_list);

                if (node->cid) {
                    aos_free(node->cid);
                }

                if (node->version) {
                    aos_free(node->version);
                }

                aos_free(node);
                LOGI(TAG, "Devcie %s is deleted", cid);
                ret = 0;
            } else {
                LOGE(TAG, "Devcie %s is in OTA process, can't delete", cid);
                ret = -1;
            }

            break;
        }
    }

    slist_for_each_entry_safe(&fota_occ_ctx.upgrade_list, tmp, node, fota_occ_device_t, next)
    {
        if (strlen(node->cid) == strlen(cid) && !strncmp(node->cid, cid, strlen(cid))) {
            LOGE(TAG, "Devcie %s is in OTA process, status :%02x\r\n", cid, node->state);
            if (node->state == FOTA_ST_UPDATING || node->state == FOTA_ST_DOWNLOADING) {
                ret = gateway_subdev_ota_stop(node->firmware->firmware_id, &node->subdev, 1);
                if (ret) {
                    LOGE(TAG, "Rm ota device failed %d", ret);
                }
                fota_occ_ctx.device = NULL;
            }
            slist_del(&node->next, &fota_occ_ctx.upgrade_list);
            if (node->cid) {
                aos_free(node->cid);
            }

            if (node->version) {
                aos_free(node->version);
            }

            aos_free(node);
            LOGI(TAG, "Devcie %s is deleted", cid);
            ret = 0;
        }
    }

    aos_mutex_unlock(&fota_occ_ctx.mutex);

    if (ret) {
        LOGE(TAG, "Devcie %s is not find, can't delete", cid);
    }

    return ret;
}

int app_fota_device_show()
{
    fota_occ_device_t *node;
    slist_t *          tmp;
    int                i = 0;
    aos_mutex_lock(&fota_occ_ctx.mutex, AOS_WAIT_FOREVER);
    printf("fota device list:\r\n");
    slist_for_each_entry_safe(&fota_occ_ctx.device_list, tmp, node, fota_occ_device_t, next)
    {
        printf("[%03d]cid:%s, %d, version %s, state %d\r\n", i, node->cid, node->subdev, node->version, node->state);
        i++;
    }

    printf("fota upgrade list:\r\n");
    i = 0;
    slist_for_each_entry_safe(&fota_occ_ctx.upgrade_list, tmp, node, fota_occ_device_t, next)
    {
        printf("[%03d]cid:%s, %d, version %s, state %d\r\n", i, node->cid, node->subdev, node->version, node->state);
        i++;
    }
    aos_mutex_unlock(&fota_occ_ctx.mutex);
    return 0;
}

static void _fota_occ_event_cb(gw_subdev_ota_state_e event_id, void *param)
{
    if (!fota_occ_ctx.device || slist_empty(&fota_occ_ctx.upgrade_list)) {
        return;
    }

    uint8_t              firmware_id = *(uint8_t *)param;
    fota_occ_firmware_t *firmware    = _fota_occ_firmware_get_by_id(firmware_id);

    if (!firmware) {
        return;
    }

    switch (event_id) {
        case GW_OTA_STATE_ONGOING: {
            fota_occ_ctx.ota_flag = 1;

        } break;

        case GW_OTA_STATE_SUCCESS: {
            if (!fota_occ_ctx.device) {
                LOGD(TAG, "fota_occ_ctx.device has been deleted");
                return;
            }

            char             version_str[32] = { 0 };
            char             device_cid[33]  = { 0 };
            gw_status_ota_t *status          = (gw_status_ota_t *)param;

            get_version_str(status->new_version, version_str);
            strcpy(device_cid, fota_occ_ctx.device->cid);
            _fota_occ_device_update_finish(fota_occ_ctx.device->cid, version_str);
            _fota_occ_rpt_version(device_cid, version_str);

            fota_occ_ctx.ota_flag = 0;
            _app_fota_recheck(FOTA_DEVICE_RECHECK);
        } break;

        case GW_OTA_STATE_FAILED: {
            gw_status_ota_t *status          = (gw_status_ota_t *)param;
            gw_subdev_t      subdev          = status->subdev;
            subdev_info_t    subdev_info     = { 0 };
            char             version_str[32] = { 0 };

            gateway_subdev_get_info(subdev, &subdev_info);

            fota_occ_ctx.ota_flag = 0;
            _app_fota_recheck(FOTA_DEVICE_RECHECK);

            get_version_str(subdev_info.version, version_str);
            _fota_occ_device_update_finish(fota_occ_ctx.device->cid, version_str);

        } break;

        default:
            break;
    }
}

static uint8_t _subdev_node_load(gw_subdev_t subdev, void *data)
{
    int           ret         = 0;
    subdev_info_t subdev_info = { 0 };

    gateway_subdev_get_info(subdev, &subdev_info);

    if (subdev_info.version) {
        char version_str[14] = { 0 };
        snprintf(version_str, sizeof(version_str) - 1, "%d.%d.%d", (subdev_info.version >> 16) & 0xff,
                 (subdev_info.version >> 8) & 0xff, (subdev_info.version >> 0) & 0xff);
        ret = app_fota_device_add((const char *)subdev_info.occ_cid, version_str, subdev);
    }

    if (ret) {
        return GW_SUBDEV_ITER_STOP;
    } else {
        return GW_SUBDEV_ITER_CONTINUE;
    }
}

static char *_app_get_version(void)
{
    char *version = aos_get_app_version();

    if (*version == 'v') {
        return version + 1; /* skip 'v' */
    }

    return version;
}

static int app_fota_load_local_device()
{
    app_fota_device_add(local_device_cid, _app_get_version(), 0);

    gateway_subdev_foreach(_subdev_node_load, NULL);

    return 0;
}

void app_fota_rpt_gw_ver(void)
{
    _fota_occ_rpt_version(local_device_cid, _app_get_version());
}

static void _app_fota_delay_check(void *p)
{
    LOGD(TAG, "Fota check %d %d", fota_occ_ctx.ota_flag, g_fota_handle->quit);
    if (!fota_occ_ctx.ota_flag) {
        int ret;
        ret = _fota_new_version_check(p);
        if (ret) {
            LOGI(TAG, "new version find");
            app_fota_do_check();
        }
    } else {
        LOGD(TAG, "In ota process cancel check");
    }

    if (!g_fota_handle->quit) {
        aos_post_delayed_action(gw_fota_loop_check_timeout, _app_fota_delay_check, NULL);
    }
}

int app_fota_event_process(gw_event_type_e gw_evt, gw_evt_param_t gw_evt_param)
{
    switch (gw_evt) {
        case GW_SUBDEV_EVT_ADD: {
            gw_evt_subdev_add_t *gw_evt_subdev_add = (gw_evt_subdev_add_t *)gw_evt_param;

            if (gw_evt_subdev_add->status != 0) {
                LOGE(TAG, "add subdev fail, status %d", gw_evt_subdev_add->status);
                return -1;
            }

            LOGD(TAG, "add subdev[%d]", gw_evt_subdev_add->subdev);

            break;
        }
        case GW_SUBDEV_EVT_DEL: {
            gw_evt_subdev_del_t *gw_evt_subdev_del = (gw_evt_subdev_del_t *)gw_evt_param;
            subdev_info_t        subdev_info       = { 0 };
            LOGD(TAG, "delete subdev[%d]", gw_evt_subdev_del->subdev);

            gateway_subdev_get_info(gw_evt_subdev_del->subdev, &subdev_info);

            app_fota_device_delete((const char *)subdev_info.occ_cid);
            break;
        }
        case GW_SUBDEV_EVT_OTA_FW: {
            gw_evt_ota_firmware_t *gw_evt_ota = (gw_evt_ota_firmware_t *)gw_evt_param;
            fota_occ_firmware_t *  firmware   = _fota_occ_firmware_get_by_id(gw_evt_ota->index);

            if (!firmware)
                return -1;

            if (gw_evt_ota->status == GW_FIRMWARE_STATE_END_USE) {
                LOGD(TAG, "OTA firmware index :%02x end use", gw_evt_ota->index);
                if (!slist_empty(&fota_occ_ctx.upgrade_list)) {
                    app_fota_do_check();
                }
            } else if (gw_evt_ota->status == GW_FIRMWARE_STATE_REMOVED) {
                _fota_occ_firmware_del(firmware);
                LOGD(TAG, "OTA firmware index :%02x removed", gw_evt_ota->index);
            } else if (gw_evt_ota->status == GW_FIRMWARE_STATE_IN_USE) {
                LOGD(TAG, "OTA firmware index :%02x in use", gw_evt_ota->index);
            }

            break;
        }
        default:
            break;
    }

    return 0;
}

gw_status_t app_fota_subdev_status_process(gw_subdev_t subdev, gw_subdev_status_type_e gw_status,
                                           gw_status_param_t status, int status_len)
{
    switch (gw_status) {
        case GW_SUBDEV_STATUS_VERSION: {
            subdev_info_t subdev_info = { 0 };

            LOGD(TAG, "fota status: subdev[%d]", subdev);

            gateway_subdev_get_info(subdev, &subdev_info);

            if (subdev_info.version) {
                char version_str[14] = { 0 };
                snprintf(version_str, sizeof(version_str) - 1, "%d.%d.%d", (subdev_info.version >> 16) & 0xff,
                         (subdev_info.version >> 8) & 0xff, (subdev_info.version >> 0) & 0xff);

                app_fota_device_add((const char *)subdev_info.occ_cid, version_str, subdev);
            }

            break;
        }
        case GW_SUBDEV_STATUS_OTA: {
            gw_status_ota_t *ota_status = (gw_status_ota_t *)status;

            _fota_occ_event_cb(ota_status->status, (void *)ota_status);

            break;
        }
        default:
            break;
    }

    return 0;
}

int app_fota_init(void)
{
    int      ret;
    uint32_t len;

    LOGD(TAG, "%s", aos_get_app_version());

    int fota_en = 1;
    ret         = aos_kv_getint(KV_FOTA_ENABLED, &fota_en);

    memset(&fota_occ_ctx, 0, sizeof(fota_occ_ctx));

    if (ret == 0 && fota_en == 0) {
        LOGI(TAG, "fota disabled, use \"kv setint fota_en 1\" to enable");
        return -1;
    }

    slist_init(&fota_occ_ctx.device_list);
    slist_init(&fota_occ_ctx.upgrade_list);

    ret = aos_mutex_new(&fota_occ_ctx.mutex);

    if (ret) {
        LOGE(TAG, "fota mutex new fail");
        return -1;
    }

    fota_register(&fota_occ_cls);
    netio_register(&fota_occ_flash);
    netio_register_httpc(NULL);

    g_fota_handle = fota_open("gateway", "flash://ram", _fota_event_cb);

    fota_config_t config;

    config.sleep_time      = APP_FOTA_SLEEP_TIME;
    config.read_timeoutms  = 10000;
    config.write_timeoutms = 10000;
    config.retry_count     = 0;
    config.auto_check_en   = 0;

    aos_kv_getint(KV_FOTA_CYCLE_MS, &(config.sleep_time));

    gw_fota_loop_check_timeout = config.sleep_time;

    fota_config(g_fota_handle, &config);

    fota_occ_ctx.enable_flag = 1;

    key_handle key_addr;
    ret = km_get_key(KEY_ID_CHIPID, &key_addr, &len);

    if (ret != KM_OK || len != DEIVICE_STR_CID_LEN) {
        return -1;
    }

    memcpy(local_device_cid, (void *)key_addr, DEIVICE_STR_CID_LEN);

    ret = app_fota_device_load();
    if (ret) {
        LOGE(TAG, "App fota device load failed %d", ret);
        return ret;
    }

    _fota_set_auto_check(1);

    return 0;
}

static void _app_fota_recheck(int ms)
{
    aos_cancel_delayed_action(gw_fota_loop_check_timeout, _app_fota_delay_check, NULL);
    aos_post_delayed_action(ms, _app_fota_delay_check, NULL);
}

int app_fota_device_load()
{
    if (!fota_occ_ctx.enable_flag) {
        LOGE(TAG, "Fota not enable\r\n");
        return -1;
    }
    return app_fota_load_local_device();
}

void app_fota_start(void)
{
    int ret = 0;
    if (g_fota_handle != NULL) {
        ret = fota_start(g_fota_handle);
        if (ret) {
            LOGE(TAG, "Fota start failed %d", ret);
        }
        aos_post_delayed_action(gw_fota_loop_check_timeout, _app_fota_delay_check, NULL);
        fota_occ_ctx.enable_flag = 1;
    }
}

void app_fota_stop(void)
{
    int ret = 0;
    if (g_fota_handle != NULL) {
        fota_stop(g_fota_handle);
        if (ret) {
            LOGE(TAG, "Fota stop failed %d", ret);
        }
        aos_cancel_delayed_action(gw_fota_loop_check_timeout, _app_fota_delay_check, NULL);
        fota_occ_ctx.enable_flag = 0;
    }
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

void app_fota_do_check(void)
{
    if (g_fota_handle == NULL) {
        return;
    }

    LOGD(TAG, "app_fota_do_check");

    /* do sleep here, for avoid lpm conflict */
    aos_msleep(200);
    fota_do_check(g_fota_handle);
}

static void _fota_set_auto_check(int enable)
{
    if (g_fota_handle == NULL) {
        return;
    }

    fota_set_auto_check(g_fota_handle, enable);
}

int app_fota_occ_url_reset()
{
    aos_kv_del("occ_url");
    return 0;
}
