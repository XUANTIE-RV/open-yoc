/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <http_client.h>
#include <aos/kernel.h>
#include <aos/log.h>
#include <aos/debug.h>
#include <cJSON.h>
#include <aos/network.h>
#include <yoc/partition.h>
#include <yoc/manifest_info.h>
#include <yoc/nvram.h>
#include <yoc/sysinfo.h>
#include "mbedtls/md5.h"
#include "ntp.h"

#define TAG "cop_verify"

#define VERIFY_DEBUG 1

/** global value to judge if
    the device is valid */
int device_vaild = 0;

struct device_info {
    char *id;   /** CID, aka tht device name */
    char *deviceCode; /** md5 calculated from .text */
    char *productKey;
    char *deviceName;
    char *deviceSercet;
    char *version; /** application version */
};

static int gen_timestamp()
{
    if (ntp_sync_time(NULL) == 0) {
        struct timeval t1;
        gettimeofday(&t1, NULL);
        return t1.tv_sec;
    } else {
        return -1;
    }
}

static int gen_random()
{
    srand(aos_now_ms());
    int random = rand();

    return (int)random;
}

//三要素
static void gen_device_tuple(char **productKey, char **deviceName, char **deviceSercet)
{
    static char productKey_val[64];
    static char deviceName_val[64];
    static char deviceSercet_val[64];
    nvram_get_val("PRODUCTKEY", productKey_val, sizeof(productKey_val));
    nvram_get_val("DEVICENAME", deviceName_val, sizeof(deviceName_val));
    nvram_get_val("DEVICESECRET", deviceSercet_val, sizeof(deviceSercet_val));

    *productKey = productKey_val;
    *deviceName = deviceName_val;
    *deviceSercet = deviceSercet_val;
}



#if 0
static char *gen_device_md5()
{
#define READ_EVERY 512
#define MD5_MAX_LEN (33)
    partition_t part = partition_open("prim");

    if (part == -EINVAL) {
        LOGE(TAG, "open prim partition failed");
        return NULL;
    }

    unsigned char *read_buffer = aos_zalloc(READ_EVERY);
    char *md = aos_zalloc(MD5_MAX_LEN);
    partition_info_t *info = hal_flash_get_info(part);

    LOGD(TAG, "Flash:%s base=%x start=%x len=%x sector_size=%d idx=%d",
         info->description,
         info->base_addr,
         info->start_addr,
         info->length,
         info->sector_size,
         info->idx);

    int partition_length = info->length;
    int i = 0;
    unsigned char digest[MD5_MAX_LEN];

    mbedtls_md5_context md5_ctx;
    mbedtls_md5_init(&md5_ctx);
    mbedtls_md5_starts(&md5_ctx);

    int p_offset = 0;
    int p_length = 0;

    //printf("Flash start\n");

    while (p_offset < partition_length) {
        if (partition_length - p_offset > READ_EVERY) {
            p_length = READ_EVERY;
        } else {
            p_length = partition_length - p_offset;
        }

        partition_read(part, p_offset, read_buffer, p_length);
        //printf("update addr=%d len=%d\n", p_offset, p_length);
        //printf(".");
        mbedtls_md5_update(&md5_ctx, read_buffer, p_length);
        p_offset += p_length;
    }

    //printf("Flash end\n");

    mbedtls_md5_finish(&md5_ctx, digest);
    mbedtls_md5_free(&md5_ctx);
    partition_close(part);
    aos_free(read_buffer);

    for (i = 0; i < 16; ++i) {
        sprintf(&md[i * 2], "%02x", (unsigned int)digest[i]);
    }

    return md;
}
#endif

static char *gen_device_hash()
{
    char *ret_buffer = NULL;
    int i = 0;
    partition_t partition = partition_open("prim");

    if (partition == -EINVAL) {
        LOGE(TAG, "open prim partition failed");
        return NULL;
    }

    int32_t data_len = 128;
    unsigned char *data = aos_zalloc(data_len);
    digest_sch_e digest_type;

    if (get_partition_digest(partition, data, &data_len, &digest_type) == 0) {
        char *digest_buffer = aos_zalloc(32 + 1);

        for (i = 0; i < 16; ++i) {
            sprintf(&digest_buffer[i * 2], "%02x", (unsigned int)data[i]);
        }

        ret_buffer = digest_buffer;
    } else {
        ;
    }

    partition_close(partition);
    aos_free(data);
    return ret_buffer;
}


static int md5_calc(char *md, const char *buf)
{
#define MD5_MAX_LEN (33)
    unsigned char digest[MD5_MAX_LEN];
    int len = strlen(buf);
    int i = 0;

    mbedtls_md5_context md5_ctx;

    mbedtls_md5_init(&md5_ctx);
    mbedtls_md5_starts(&md5_ctx);
    mbedtls_md5_update(&md5_ctx, (unsigned char *)buf, len);
    mbedtls_md5_finish(&md5_ctx, digest);
    mbedtls_md5_free(&md5_ctx);

    for (i = 0; i < 16; ++i) {
        sprintf(&md[i * 2], "%02x", (unsigned int)digest[i]);
    }

    return MD5_MAX_LEN;
}



/**
    Upload json format to server:

    {
        "id":"TEST2019051711108",
        "nonce":"nonce",
        "timestamp":"timestamp",
        "deviceCode":"deviceCode",
        "version":"version",
        "extInfo":{
                "productKey":"productKey",
                "deviceName":"deviceName",
                "deviceSercet":"deviceSercet"
        },
        "sign":"sign"
    }

*/
static char *cop_upload_json_build(struct device_info *info)
{
    char *json_str = NULL;

    char *nonce_buffer = aos_zalloc(100);
    char *timestamp_buffer = aos_zalloc(100);
    char *sign_tmp1 = aos_zalloc(512);
    char *sign_tmp2 = aos_zalloc(512);
    char *sign = aos_zalloc(512);

    sprintf(nonce_buffer, "%d", gen_random());
    sprintf(timestamp_buffer, "%d", gen_timestamp());

    cJSON *json_req = cJSON_CreateObject();
    cJSON_AddStringToObject(json_req, "id", info->id);
    cJSON_AddStringToObject(json_req, "nonce", nonce_buffer);
    cJSON_AddStringToObject(json_req, "timestamp", timestamp_buffer);
    cJSON_AddStringToObject(json_req, "deviceCode", info->deviceCode);
    cJSON_AddStringToObject(json_req, "version", info->version);

    cJSON *extInfo = cJSON_CreateObject();
    cJSON_AddStringToObject(extInfo, "productKey", info->productKey);
    cJSON_AddStringToObject(extInfo, "deviceName", info->deviceName);
    cJSON_AddStringToObject(extInfo, "deviceSercet", info->deviceSercet);
    cJSON_AddItemToObject(json_req, "extInfo", extInfo);

    sprintf(sign_tmp1, "%s%s%s%s%s", info->id, nonce_buffer, timestamp_buffer, info->deviceCode, info->version);
    md5_calc(sign_tmp2, sign_tmp1);
    md5_calc(sign, sign_tmp2);

    cJSON_AddStringToObject(json_req, "sign", sign);

    json_str = cJSON_Print(json_req);
    cJSON_Delete(json_req);
    aos_free(nonce_buffer);
    aos_free(timestamp_buffer);
    aos_free(sign);
    aos_free(sign_tmp1);
    aos_free(sign_tmp2);

    return json_str;
}



static int cop_verify_entry(char *server_url, struct device_info *info)
{
    int ret = 0;
    int http_response_size = 100;
    char *buffer = aos_zalloc(http_response_size);

    if (buffer == NULL) {
        LOGE(TAG, "cannot aos_malloc http buffer");
        return ret;
    }

    http_client_config_t config = {
        .url = server_url,
        //.event_handler = _http_event_handler,
        //.is_async = true,
        .timeout_ms = 5000,
    };
    http_client_handle_t client = http_client_init(&config);
    http_errors_t err;
    char *post_data = cop_upload_json_build(info);

    if (post_data == NULL) {
        LOGE(TAG, "Error to build JSON data");
        goto exit;
    }

#if VERIFY_DEBUG
    printf("postdata=%s\n", post_data);
#endif

    const char *result_fail = "\"code\":-1";

    http_client_set_method(client, HTTP_METHOD_POST);
    http_client_set_header(client, "Content-Type", "application/json");
    http_client_set_post_field(client, post_data, strlen(post_data));

    while (1) {
        err = http_client_perform(client);

        if (err != EAGAIN) {
            break;
        }
    }

    if (err != HTTP_CLI_OK) {
        LOGE(TAG, "Error perform http request 0x%x", (err));
        goto exit;
    }

    if (http_client_get_status_code(client) != 200) {
        LOGE(TAG, "Error perform http request Status=%d", http_client_get_status_code(client));
        goto exit;
    }

    http_client_fetch_headers(client);

    if (http_client_is_chunked_response(client)) {
        http_client_read(client, buffer, http_response_size);

        if (strstr(buffer, result_fail) != NULL) {
#if VERIFY_DEBUG
            LOGD(TAG, "Server report failed other reason %s", buffer);
#endif
            ret = -1;

        } else {
#if VERIFY_DEBUG
            LOGD(TAG, "Server report ok %s", buffer);
#endif
        }
    } else {
        LOGE(TAG, "Server not c-sky server, critical error!");
    }

exit:
    http_client_cleanup(client);
    aos_free(buffer);

    if (post_data) {
        aos_free(post_data);
    }

    return ret;
}


static void cop_verify_thread(void *arg)
{
    unsigned int out_len = 0;
    char *server_url = "https://cid.c-sky.com/api/device/authorize";
    struct device_info info;
    //kv set PRODUCTKEY a1J8co0Sgy4
    //kv set DEVICENAME 6f7e174304400800c816910dd531a060
    //kv set DEVICESECRET s8nkqV5apnk3cRBtuGlI4KBSAmiTDPZo
    gen_device_tuple(&info.productKey, &info.deviceName, &info.deviceSercet);
    /*
        valid id list:
        TEST2019051711108
        DEMO2019051711108
        CID2019051711120
    */
    //kv set device_id TEST2019051711108
    info.id = aos_get_device_id();

    if (info.id == NULL) {
        info.id = "";
    }

    info.deviceCode = gen_device_hash();
    info.version = aos_zalloc(512);
    get_app_version((unsigned char *)info.version, &out_len);

    if (strlen(info.version) == 0) {
        strcpy(info.version, "");
    }

    int server_ret = cop_verify_entry(server_url, &info);

    if (server_ret == 0) {
#if VERIFY_DEBUG
        LOGD(TAG, "COP Check OK or Network error");
#endif
    } else {
#if VERIFY_DEBUG
        LOGD(TAG, "COP Check failed");
#endif
        device_vaild = -1;
        //aos_kernel_sched_suspend(); disable system running
    }

    if (info.deviceCode) {
        aos_free(info.deviceCode);
    }
}


void cop_verify(void)
{
    aos_task_t handle;
    aos_task_new_ext(&handle, "cop_verify_thread", cop_verify_thread, NULL, 4 * 1024, AOS_DEFAULT_APP_PRI);
}
