/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <sys/time.h>
#include <aos/kernel.h>
#include <aos/kv.h>
#include <common/log.h>
#include <http_client.h>
#include <cJSON.h>
#include <tsl_engine/file_utils.h>
#include <ulog/ulog.h>
#include <crypto_md.h>
#include <sec_crypto_auth.h>
#include <key_mgr.h>
#include "gateway_mgmt.h"
#if !defined(CONFIG_SAVE_JS_TO_RAM) || !CONFIG_SAVE_JS_TO_RAM
#include <vfs.h>
#endif

#define TAG                               "GW_OCC_AUTH"
#define DEFAULT_URL                       "https://occ.t-head.cn"
#define BUFFER_SIZE                       2048
#define MAX_OCC_AUTH_FAIL_TIME            5
#define MAX_OCC_AUTH_NODE_NUM             20
#define MAX_OCC_AUTH_OUT_SIMPLE_SAVE_NODE 5
#define NO_INDEX_VALID                    0xff

enum
{
    NODE_TYPE_UNUSED = 0x0,
    NODE_TYPE_USED,
};

enum
{
    NODE_AUTH_STATUS_ERASABLE = 0x0,
    NODE_AUTH_STATUS_UNERASABLE,
};

enum
{
    OCC_AUTH_CMD_START = 0x0,
    OCC_AUTH_CMD_RESULT_GET,
};

typedef struct {
    uint8_t                node_state        : 1;
    uint8_t                auth_status       : 2;
    uint8_t                dev_protocal      : 4;
    uint8_t                auth_out_erasable : 1;
    uint8_t                auth_out_index;
    occ_protocol_info_in_t in_data;
} gw_auth_node_t;

typedef struct {
    uint32_t short_oob;
    char     CID[33];
} mesh_auth_out_info_t;

typedef union
{
    mesh_auth_out_info_t mesh_out_data;
} occ_auth_out_info_t;

typedef struct {
    uint8_t             node_state : 1;
    occ_auth_out_info_t info;
} gw_auth_out_info_t;

typedef struct {
    uint8_t         cmd_event;
    uint8_t         failed_num;
    gw_auth_node_t *node;
    aos_sem_t *     cmd_sem;
    gw_auth_out_t * temp_auth_out_info;
} occ_auth_cmd_message_t;

typedef struct {
    uint8_t                init_flag;
    uint8_t                enable_flag;
    aos_task_t             occ_auth_task;
    aos_queue_t            auth_queue;
    occ_auth_cmd_message_t auth_queue_message[CONFIG_OCC_AUTH_NODE_MAX_MSG_SIZE];
    gw_auth_node_t         in_nodes[MAX_OCC_AUTH_NODE_NUM];
    gw_auth_out_info_t     out_nodes[MAX_OCC_AUTH_OUT_SIMPLE_SAVE_NODE];
    gw_occ_auth_cb_t *     cb_head;
    aos_mutex_t            node_mutex;
} occ_auth_ctx_t;

aos_sem_t      js_sem;
occ_auth_ctx_t g_auth_ctx;

static uint8_t char2u8(char c)
{
    if (c >= '0' && c <= '9') {
        return (c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 10);
    } else {
        return 0;
    }
}

static void str2hex(uint8_t hex[], char *s, uint8_t cnt)
{
    uint8_t i;

    if (!s) {
        return;
    }

    for (i = 0; (*s != '\0') && (i < cnt); i++, s += 2) {
        hex[i] = ((char2u8(*s) & 0x0f) << 4) | ((char2u8(*(s + 1))) & 0x0f);
    }
}

static int post_verify_msg_url(char *ota_url, int len)
{
    int  ret         = 0;
    char occ_url[40] = { 0 };
    int  occ_url_len = sizeof(occ_url);

    ret = aos_kv_get("occurl", occ_url, &occ_url_len);
    if (ret != 0) {
        strcpy(occ_url, "https://occ.t-head.cn");
    }

    snprintf(ota_url, len, "%s%s", occ_url, "/api/device/gateway/deviceAuth");

    return 0;
}

static bool process_again(int status_code)
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

    // process_again() returns true only in case of redirection.
    if (data_size > 0 && process_again(status_code)) {
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

        if (process_again(status_code)) {
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
    } while (process_again(status_code));

    return err;
}

static void _http_cleanup(http_client_handle_t client)
{
    if (client) {
        http_client_cleanup(client);
    }
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

static char *hextostr(const uint8_t *source, char *dest, int len)
{
    int  i;
    char tmp[3];

    for (i = 0; i < len; i++) {
        sprintf(tmp, "%02x", (unsigned char)source[i]);
        memcpy(&dest[i * 2], tmp, 2);
    }

    dest[len * 2] = 0;
    return dest;
}

static int mesh_check_device_auth_identification(mesh_occ_auth_in_t in_data, char *pcid, uint32_t *short_oob)
{
    int                  ret     = 0;
    char *               payload = NULL;
    char                 getvalue[255];
    cJSON *              js     = NULL;
    char *               buffer = NULL;
    http_errors_t        err;
    http_client_config_t config = { 0 };
    http_client_handle_t client = NULL;
    key_handle           key_addr;
    char                 cidr[32 + 1];
    char                 authmsg[20];
    char                 temp[33 * 2];
    char                 timestr[10];
    uint32_t             cid_len;
    uint32_t             t;
    struct timeval       t1;
    gettimeofday(&t1, NULL);

    uint8_t  oob_hex[16] = { 0x00 };
    uint32_t authcode;
    uint8_t  mac[6] = { 0x0 };
    uint8_t  random = 0;
    uint32_t vid    = 0;

    memcpy(&authcode, &in_data.uuid[0], 4);
    memcpy(mac, &in_data.uuid[4], 6);
    random = in_data.uuid[10];
    memcpy(&vid, &in_data.uuid[11], 2);

    buffer = aos_zalloc(BUFFER_SIZE + 1);
    if (buffer == NULL) {
        ret = -ENOMEM;
        goto out;
    }

    if ((payload = aos_malloc(256)) == NULL) {
        ret = -ENOMEM;
        goto out;
    }

    cidr[32] = 0;
    memset(cidr, 0, sizeof(cidr));
    ret = km_get_key(KEY_ID_CHIPID, &key_addr, &cid_len);

    if (ret != KM_OK) {
        goto out;
    }

    memcpy(cidr, (void *)key_addr, 32);
    stringtohex(cidr, (uint8_t *)temp, 16);
    cid_len = 16;
    t       = t1.tv_sec;
    sprintf(timestr, "%d", t);

    ret = sc_srv_get_authmsg((uint8_t *)temp, cid_len, (unsigned char *)timestr, strlen(timestr), (uint8_t *)authmsg);

    snprintf(payload, 256,
             "{\"mac\":\"%02x%02x%02x%02x%02x%02x\",\"vid\":\"%d\",\"message\":\"%d\",\"authcode\":\"%u\", "
             "\"authMsg\":{\"cid\":\"%s\", \"timestamp\":\"%s\", \"authcode\":\"%s\"}}",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], vid, random, authcode, cidr, timestr,
             hextostr((const uint8_t *)authmsg, temp, sizeof(authmsg)));

    LOGD(TAG, "check: %s", payload);

    memset(getvalue, 0, sizeof(getvalue));
    post_verify_msg_url(getvalue, sizeof(getvalue));

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

    cJSON *identify = cJSON_GetObjectItem(result, "identify");

    if (!(identify && cJSON_IsString(identify))) {
        LOGW(TAG, "get identify failed");
        ret = -1;
        goto out;
    }

    LOGD(TAG, "identify: %s", identify->valuestring);
    /* cid */
    cJSON *cid = cJSON_GetObjectItem(result, "cid");

    if (!(cid && cJSON_IsString(cid))) {
        LOGW(TAG, "get cid failed");
        ret = -1;
        goto out;
    }

    LOGD(TAG, "cid: %s", cid->valuestring);
    strcpy(pcid, cid->valuestring);
    /* oob */
    cJSON *oob = cJSON_GetObjectItem(result, "oob");

    if (!(oob && cJSON_IsString(oob))) {
        LOGW(TAG, "get oob failed");
        ret = -1;
        goto out;
    }

    LOGD(TAG, "oob: %s", oob->valuestring);
    // strcpy(poob, oob->valuestring);
    str2hex(oob_hex, oob->valuestring, 16);
    *short_oob = oob_hex[0] << 24 | oob_hex[1] << 16 | oob_hex[2] << 8 | oob_hex[3];
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

static int gateway_auth_out_node_index_get(uint8_t *valid_index)
{
    *valid_index = NO_INDEX_VALID;
    aos_mutex_lock(&g_auth_ctx.node_mutex, AOS_WAIT_FOREVER);

    for (int i = 0; i < sizeof(g_auth_ctx.out_nodes) / sizeof(g_auth_ctx.out_nodes[0]); i++) {
        if (g_auth_ctx.out_nodes[i].node_state == NODE_TYPE_UNUSED) {
            *valid_index = i;
            aos_mutex_unlock(&g_auth_ctx.node_mutex);
            return 0;
        }
    }

    aos_mutex_unlock(&g_auth_ctx.node_mutex);

    return -ENOENT;
}

static int occ_auth_start(gw_auth_node_t *node, gw_auth_out_t *out)
{
    int ret = 0;

    switch (node->dev_protocal) {
        case GW_OCC_AUTH_TYPE_MESH: {
            char                 pcid[33];
            uint32_t             short_oob;
            uint8_t              save_out_index = 0;
            mesh_occ_auth_out_t *mesh_out_data  = &out->out_data.mesh_occ_out_data;
            ret = mesh_check_device_auth_identification(node->in_data.mesh_occ_in_data, pcid, &short_oob);
            if (ret) {
                LOGE(TAG, "Mesh dev occ auth failed for dev:%s %d", bt_hex_real(node->in_data.mesh_occ_in_data.addr, 6),
                     ret);
                return ret;
            } else {
                LOGD(TAG, "Mesh dev occ auth success for dev:%s", bt_hex_real(node->in_data.mesh_occ_in_data.addr, 6));
                memcpy(mesh_out_data->addr, node->in_data.mesh_occ_in_data.addr, 6);
                memcpy(mesh_out_data->uuid, node->in_data.mesh_occ_in_data.uuid, 16);
                memcpy(mesh_out_data->CID, pcid, strlen(pcid));
                mesh_out_data->addr_type = node->in_data.mesh_occ_in_data.addr_type;
                mesh_out_data->short_oob = short_oob;
                mesh_out_data->bearer    = node->in_data.mesh_occ_in_data.bearer;
                mesh_out_data->oob_info  = node->in_data.mesh_occ_in_data.oob_info;
                if (!gateway_auth_out_node_index_get(&save_out_index)) {
                    node->auth_out_index                            = save_out_index;
                    g_auth_ctx.out_nodes[save_out_index].node_state = NODE_TYPE_USED;
                    memcpy(g_auth_ctx.out_nodes[save_out_index].info.mesh_out_data.CID, mesh_out_data->CID,
                           strlen(mesh_out_data->CID));
                    g_auth_ctx.out_nodes[save_out_index].info.mesh_out_data.short_oob = mesh_out_data->short_oob;
                } else {
                    LOGD(TAG, "No auth output index for save");
                }
            }
        } break;
        default:
            LOGE(TAG, "Unsupport auth protocal %02x", node->dev_protocal);
            return -1;
    }

    return ret;
}

static int occ_auth_cmd_get(occ_auth_cmd_message_t *cmd, unsigned int timeout)
{
    int          ret       = 0;
    size_t read_size = 0;

    if (!cmd || !aos_queue_is_valid(&g_auth_ctx.auth_queue)) {
        return -EINVAL;
    }

    ret = aos_queue_recv(&g_auth_ctx.auth_queue, timeout, (void *)cmd, &read_size);

    if (ret == 0 && read_size > 0) {
        return 0;
    } else {
        LOGE(TAG, "get queue message fail:%d", ret);
        return -EAGAIN;
    }

    return 0;
}

static int occ_auth_dev_cmd_send(occ_auth_cmd_message_t *cmd, uint32_t timeout)
{
    int ret = 0;

    ret = aos_queue_send(&g_auth_ctx.auth_queue, cmd, sizeof(occ_auth_cmd_message_t));
    if (ret) {
        LOGE(TAG, "Add auth node failed %d", ret);
        return ret;
    }

    if (timeout) {
        ret = aos_sem_wait(cmd->cmd_sem, timeout);
    }
    return ret;
}

static void occ_auth_event_process(gw_auth_out_t *auth_out_status)
{
    gw_occ_auth_cb_t *cb;

    for (cb = g_auth_ctx.cb_head; cb; cb = cb->_next) {
        if (cb->auth_cb) {
            cb->auth_cb(auth_out_status);
        }
    }
}

static gw_auth_node_t *gateway_auth_node_found_and_grep_valid_index(gw_auth_in_t auth_in_info, uint8_t *valid_index)
{
    uint8_t node_size = sizeof(g_auth_ctx.in_nodes) / sizeof(g_auth_ctx.in_nodes[0]);

    if (valid_index) {
        *valid_index = NO_INDEX_VALID;
    }

    uint8_t eraseable_index = NO_INDEX_VALID;

    aos_mutex_lock(&g_auth_ctx.node_mutex, AOS_WAIT_FOREVER);
    for (int i = 0; i < node_size; i++) {
        if (g_auth_ctx.in_nodes[i].node_state == NODE_TYPE_USED) {
            if (eraseable_index == NO_INDEX_VALID
                && g_auth_ctx.in_nodes[i].auth_out_erasable == NODE_AUTH_STATUS_ERASABLE) {
                eraseable_index = i;
            }

            if (auth_in_info.dev_protocal == g_auth_ctx.in_nodes[i].dev_protocal) {
                switch (auth_in_info.dev_protocal) {
                    case GW_OCC_AUTH_TYPE_MESH: {
                        if (auth_in_info.in_data.mesh_occ_in_data.addr_type
                                == g_auth_ctx.in_nodes[i].in_data.mesh_occ_in_data.addr_type
                            && !memcmp(g_auth_ctx.in_nodes[i].in_data.mesh_occ_in_data.addr,
                                       auth_in_info.in_data.mesh_occ_in_data.addr, 6))
                        {
                            LOGD(TAG, "Occ auth node already exist");
                            aos_mutex_unlock(&g_auth_ctx.node_mutex);
                            return &g_auth_ctx.in_nodes[i];
                        }
                    } break;
                    default:
                        LOGE(TAG, "Unsupport occ auth protocal %02x", auth_in_info.dev_protocal);
                        aos_mutex_unlock(&g_auth_ctx.node_mutex);
                        return NULL;
                }
            }
        } else if (valid_index && *valid_index == NO_INDEX_VALID) {
            *valid_index = i;
        }
    }

    if (valid_index && *valid_index == NO_INDEX_VALID) {
        if (eraseable_index == NO_INDEX_VALID) {
            LOGE(TAG, "Occ auth node queue full, you should remove or clear the node queue first");
        } else {
            LOGD(TAG, "Occ auth node queue full, But found index %d occ auth info erasable");
            uint8_t out_data_index = g_auth_ctx.in_nodes[eraseable_index].auth_out_index;
            if (out_data_index != NO_INDEX_VALID) {
                memset(&g_auth_ctx.out_nodes[out_data_index], 0x0, sizeof(g_auth_ctx.out_nodes[out_data_index]));
            }
            memset(&g_auth_ctx.in_nodes[eraseable_index], 0x0, sizeof(g_auth_ctx.in_nodes[eraseable_index]));
        }
        *valid_index = eraseable_index;
    }

    aos_mutex_unlock(&g_auth_ctx.node_mutex);
    return NULL;
}

static gw_auth_node_t *gateway_auth_node_get(gw_auth_in_t auth_in_info)
{
    return gateway_auth_node_found_and_grep_valid_index(auth_in_info, NULL);
}

static void occ_auth_task_handler(void *arg)
{
    int                    ret = 0;
    occ_auth_cmd_message_t cmd;

    while (1) {
        ret = occ_auth_cmd_get(&cmd, AOS_WAIT_FOREVER);
        if (ret < 0) {
            LOGE(TAG, "Get occ cmd get failed %d", ret);
        }

        LOGD(TAG, "Occ event:%4x", cmd.cmd_event);

        switch (cmd.cmd_event) {
            case OCC_AUTH_CMD_START: {
                gw_auth_out_t temp_auth_out = { 0x0 };
                temp_auth_out.auth_status   = OCC_AUTH_IN;
                cmd.node->auth_status       = OCC_AUTH_IN;
                ret                         = occ_auth_start(cmd.node, &temp_auth_out);
                if (ret) {
                    cmd.failed_num++;
                    LOGE(TAG, "Occ auth failed time %d", ret, cmd.failed_num);
                    if (cmd.failed_num >= MAX_OCC_AUTH_FAIL_TIME) {
                        temp_auth_out.auth_status = OCC_AUTH_FAILED;
                        cmd.node->auth_status     = OCC_AUTH_FAILED;
                    } else {
                        occ_auth_dev_cmd_send(&cmd, 0);
                        break;
                    }
                } else {
                    temp_auth_out.auth_status = OCC_AUTH_SUCCESS;
                    cmd.node->auth_status     = OCC_AUTH_SUCCESS;
                }
                occ_auth_event_process(&temp_auth_out);
            } break;
            case OCC_AUTH_CMD_RESULT_GET: {
                cmd.temp_auth_out_info->auth_status = OCC_AUTH_IN;
                cmd.node->auth_status               = OCC_AUTH_IN;
                ret                                 = occ_auth_start(cmd.node, cmd.temp_auth_out_info);
                if (ret) {
                    cmd.failed_num++;
                    LOGE(TAG, "Occ auth failed time %d", ret, cmd.failed_num);
                    if (cmd.failed_num >= MAX_OCC_AUTH_FAIL_TIME) {
                        cmd.temp_auth_out_info->auth_status = OCC_AUTH_FAILED;
                        cmd.node->auth_status               = OCC_AUTH_FAILED;
                        if (cmd.cmd_sem) {
                            aos_sem_signal(cmd.cmd_sem);
                        }
                    } else {
                        occ_auth_dev_cmd_send(&cmd, 0);
                        break;
                    }
                } else {
                    cmd.temp_auth_out_info->auth_status = OCC_AUTH_SUCCESS;
                    cmd.node->auth_status               = OCC_AUTH_SUCCESS;
                    if (cmd.cmd_sem) {
                        aos_sem_signal(cmd.cmd_sem);
                    }
                }
            } break;

            default:
                break;
        }
    }
}

/* user api */
int gateway_occ_auth_init()
{
    int ret = 0;

    if (g_auth_ctx.init_flag) {
        LOGD(TAG, "gateway occ auth init already");
        return -EALREADY;
    }

    memset(&g_auth_ctx, 0x00, sizeof(occ_auth_ctx_t));

    ret = aos_mutex_new(&g_auth_ctx.node_mutex);
    if (ret) {
        LOGE(TAG, "Occ auth mutex init failed %d", ret);
        return -ENOMEM;
    }

    ret = aos_queue_new(&g_auth_ctx.auth_queue, (void *)g_auth_ctx.auth_queue_message,
                        sizeof(g_auth_ctx.auth_queue_message), sizeof(occ_auth_cmd_message_t));
    if (ret) {
        LOGE(TAG, "Occ auth queue create failed");
        return -ENOMEM;
    }

    ret = aos_task_new_ext(&g_auth_ctx.occ_auth_task, "occ_auth_task", occ_auth_task_handler, NULL,
                           CONFIG_OCC_AUTH_STACK_SIZE, CONFIG_OCC_AUTH_TASK_PRIO);

    if (ret < 0) {
        LOGE(TAG, "Occ auth task create failed %d", ret);
        return -ENOMEM;
    }

    for (int i = 0; i < sizeof(g_auth_ctx.in_nodes) / sizeof(g_auth_ctx.in_nodes[0]); i++) {
        g_auth_ctx.in_nodes[i].auth_out_index = NO_INDEX_VALID;
    }

    g_auth_ctx.init_flag = 1;

    return 0;
}

int gateway_occ_auth_enable(uint8_t enable)
{
    if (!g_auth_ctx.init_flag) {
        LOGE(TAG, "occ auth not init");
        return -EPERM;
    }
    g_auth_ctx.enable_flag = enable;
    return 0;
}

int gateway_occ_auth_is_enabled(void)
{
    return g_auth_ctx.enable_flag;
}

int gateway_occ_auth_cb_register(gw_occ_auth_cb_t *cb)
{
    if (!g_auth_ctx.init_flag) {
        LOGE(TAG, "occ auth not init");
        return -EPERM;
    }

    if (g_auth_ctx.cb_head) {
        if (g_auth_ctx.cb_head == cb) {
            return 0;
        }

        for (gw_occ_auth_cb_t *temp = g_auth_ctx.cb_head; temp->_next != NULL; temp = temp->_next) {
            if (temp->_next == cb) {
                return 0;
            }
        }
    }
    cb->_next          = g_auth_ctx.cb_head;
    g_auth_ctx.cb_head = cb;

    return 0;
}

int gateway_occ_auth_cb_unregister(gw_occ_auth_cb_t *cb)
{
    if (!g_auth_ctx.init_flag) {
        LOGE(TAG, "occ auth not init");
        return -EPERM;
    }

    gw_occ_auth_cb_t *temp = g_auth_ctx.cb_head;

    if (!g_auth_ctx.cb_head) {
        return -EINVAL;
    }

    if (cb == g_auth_ctx.cb_head) {
        g_auth_ctx.cb_head = cb->_next;
        return 0;
    }

    for (; temp->_next != NULL; temp = temp->_next) {
        if (temp->_next == cb) {
            temp->_next = cb->_next;
            cb->_next   = NULL;
        }
    }

    return 0;
}

#if defined(CONFIG_DEBUG) && CONFIG_DEBUG > 0
static char *_auth_status_str(uint8_t state)
{
    switch (state) {
        case OCC_AUTH_IDLE:
            return "OCC_AUTH_IDLE";
        case OCC_AUTH_IN:
            return "OCC_AUTH_IN";
        case OCC_AUTH_FAILED:
            return "OCC_AUTH_FAILED";
        case OCC_AUTH_SUCCESS:
            return "OCC_AUTH_SUCCESS";
        default:
            return "UNKNOW";
    }
}
#endif

int gateway_occ_node_auth(gw_auth_in_t auth_in_info, uint8_t *auth_status)
{
    if (!g_auth_ctx.init_flag) {
        LOGE(TAG, "occ auth not init");
        return -EPERM;
    }

    if (!g_auth_ctx.enable_flag) {
        LOGE(TAG, "occ auth not enabled");
        return -EPERM;
    }

    int             ret         = 0;
    uint8_t         valid_index = 0;
    gw_auth_node_t *node        = gateway_auth_node_found_and_grep_valid_index(auth_in_info, &valid_index);
    if (node) {
        *auth_status = node->auth_status;
        LOGD(TAG, "Node already exist, status %s", _auth_status_str(*auth_status));
        return 0;
    }

    if (valid_index == NO_INDEX_VALID) {
        LOGE(TAG, "Occ node queue full");
        return -EINVAL;
    } else {
        aos_mutex_lock(&g_auth_ctx.node_mutex, AOS_WAIT_FOREVER);
        g_auth_ctx.in_nodes[valid_index].node_state        = NODE_TYPE_USED;
        g_auth_ctx.in_nodes[valid_index].auth_status       = OCC_AUTH_IDLE;
        g_auth_ctx.in_nodes[valid_index].dev_protocal      = auth_in_info.dev_protocal;
        g_auth_ctx.in_nodes[valid_index].auth_out_erasable = NODE_AUTH_STATUS_ERASABLE;
        g_auth_ctx.in_nodes[valid_index].auth_out_index    = NO_INDEX_VALID;
        memcpy(&g_auth_ctx.in_nodes[valid_index].in_data, &auth_in_info.in_data, sizeof(occ_protocol_info_in_t));
        aos_mutex_unlock(&g_auth_ctx.node_mutex);
        occ_auth_cmd_message_t cmd = { 0x00 };
        cmd.cmd_event              = OCC_AUTH_CMD_START;
        cmd.failed_num             = 0;
        cmd.cmd_sem                = NULL;
        cmd.temp_auth_out_info     = NULL;
        cmd.node                   = &g_auth_ctx.in_nodes[valid_index];
        ret                        = occ_auth_dev_cmd_send(&cmd, 0);
        if (ret) {
            LOGE(TAG, "Occ auth start failed %d", ret);
            *auth_status = OCC_AUTH_FAILED;
        } else {
            *auth_status = OCC_AUTH_IDLE;
        }
        LOGD(TAG, "Node auth status %s", _auth_status_str(*auth_status));
    }
    return ret;
}

int gateway_occ_node_auth_result_get(gw_auth_in_t auth_in_info, gw_auth_out_t *auth_out, uint32_t timeout)
{
    if (!g_auth_ctx.init_flag) {
        LOGE(TAG, "occ auth not init");
        return -EPERM;
    }

    if (!g_auth_ctx.enable_flag) {
        LOGE(TAG, "occ auth not enabled");
        return -EPERM;
    }

    if (!auth_out) {
        LOGE(TAG, "occ auth out NULL");
        return -EINVAL;
    }

    int                    ret         = 0;
    uint8_t                valid_index = 0;
    gw_auth_node_t *       to_auth     = NULL;
    occ_auth_cmd_message_t cmd         = { 0x00 };
    gw_auth_node_t *       node        = gateway_auth_node_found_and_grep_valid_index(auth_in_info, &valid_index);

    if ((!node && !timeout) || (node && node->auth_out_index == NO_INDEX_VALID && !timeout)) {
        LOGE(TAG, "No auth result cached, Can not get auth result immediately");
        return -ENOENT;
    }

    if (!node && valid_index == NO_INDEX_VALID) {
        LOGE(TAG, "No auth buffer");
        return -ENOMEM;
    }

    memset(auth_out, 0x0, sizeof(gw_auth_out_t));
    memcpy(auth_out->out_data.mesh_occ_out_data.addr, auth_in_info.in_data.mesh_occ_in_data.addr, 6);
    memcpy(auth_out->out_data.mesh_occ_out_data.uuid, auth_in_info.in_data.mesh_occ_in_data.uuid, 16);
    auth_out->dev_protocal                         = auth_in_info.dev_protocal;
    auth_out->out_data.mesh_occ_out_data.addr_type = auth_in_info.in_data.mesh_occ_in_data.addr_type;
    auth_out->out_data.mesh_occ_out_data.bearer    = auth_in_info.in_data.mesh_occ_in_data.bearer;
    auth_out->out_data.mesh_occ_out_data.oob_info  = auth_in_info.in_data.mesh_occ_in_data.oob_info;

    if (node) {
        LOGD(TAG, "Status cached");
        if (node->auth_status == OCC_AUTH_FAILED) {
            LOGD(TAG, "Node auth failed");
            auth_out->auth_status = OCC_AUTH_FAILED;
            return 0;
        } else if (node->auth_status == OCC_AUTH_IDLE || node->auth_status == OCC_AUTH_IN) {
            LOGD(TAG, "Node auth status Idle or In, auth again");
            to_auth = node;
            goto auth_again;
        } else if (node->auth_status == OCC_AUTH_SUCCESS) {
            if (node->auth_out_index != NO_INDEX_VALID) {
                LOGD(TAG, "Result cached");
                occ_auth_out_info_t *auth_out_info;
                auth_out_info = &g_auth_ctx.out_nodes[node->auth_out_index].info;
                memcpy(auth_out->out_data.mesh_occ_out_data.CID, auth_out_info->mesh_out_data.CID,
                       strlen(auth_out_info->mesh_out_data.CID));
                auth_out->out_data.mesh_occ_out_data.short_oob = auth_out_info->mesh_out_data.short_oob;
                auth_out->auth_status                          = OCC_AUTH_SUCCESS;
                return 0;
            } else {
                LOGD(TAG, "Result not cached");
                to_auth = node;
                goto auth_again;
            }
        } else {
            LOGE(TAG, "Nnknow auth status %02x", node->auth_status);
            return -1;
        }
    } else {
        LOGD(TAG, "Status and Result not cached");
        aos_mutex_lock(&g_auth_ctx.node_mutex, AOS_WAIT_FOREVER);
        memcpy(g_auth_ctx.in_nodes[valid_index].in_data.mesh_occ_in_data.addr,
               auth_in_info.in_data.mesh_occ_in_data.addr, 6);
        memcpy(g_auth_ctx.in_nodes[valid_index].in_data.mesh_occ_in_data.uuid,
               auth_in_info.in_data.mesh_occ_in_data.uuid, 16);
        g_auth_ctx.in_nodes[valid_index].node_state = NODE_TYPE_USED;
        g_auth_ctx.in_nodes[valid_index].in_data.mesh_occ_in_data.addr_type
            = auth_in_info.in_data.mesh_occ_in_data.addr_type;
        g_auth_ctx.in_nodes[valid_index].in_data.mesh_occ_in_data.bearer = auth_in_info.in_data.mesh_occ_in_data.bearer;
        g_auth_ctx.in_nodes[valid_index].in_data.mesh_occ_in_data.oob_info
            = auth_in_info.in_data.mesh_occ_in_data.oob_info;
        g_auth_ctx.in_nodes[valid_index].dev_protocal = auth_in_info.dev_protocal;
        aos_mutex_unlock(&g_auth_ctx.node_mutex);
        to_auth = &g_auth_ctx.in_nodes[valid_index];
        goto auth_again;
    }

    return -1;

auth_again:
    cmd.cmd_event          = OCC_AUTH_CMD_RESULT_GET;
    cmd.failed_num         = 0;
    cmd.node               = to_auth;
    cmd.cmd_sem            = (aos_sem_t *)malloc(sizeof(aos_sem_t));
    cmd.temp_auth_out_info = auth_out;
    if (!cmd.cmd_sem) {
        LOGE(TAG, "cmd sem malloc failed");
        return -1;
    }

    ret = aos_sem_new(cmd.cmd_sem, 0);
    if (ret) {
        LOGE(TAG, "cmd sem create failed");
        return -1;
    }

    ret = occ_auth_dev_cmd_send(&cmd, timeout);
    if (ret) {
        LOGE(TAG, "Occ cmd get occ auth result failed %d", ret);
    }

    aos_sem_free(cmd.cmd_sem);
    free(cmd.cmd_sem);

    return ret;
}

int gateway_occ_node_auth_cache_remove(gw_auth_in_t auth_in_info)
{
    if (!g_auth_ctx.init_flag) {
        LOGE(TAG, "occ auth not init");
        return -1;
    }

    if (!g_auth_ctx.enable_flag) {
        LOGE(TAG, "occ auth not enabled");
        return -1;
    }

    gw_auth_node_t *node = gateway_auth_node_get(auth_in_info);
    if (!node) {
        LOGD(TAG, "Node Removed already");
        return 0;
    } else {
        aos_mutex_lock(&g_auth_ctx.node_mutex, AOS_WAIT_FOREVER);
        node->node_state = NODE_TYPE_UNUSED;
        if (node->auth_out_index != NO_INDEX_VALID) {
            g_auth_ctx.out_nodes[node->auth_out_index].node_state = NODE_TYPE_UNUSED;
        }
        node->auth_out_index = NO_INDEX_VALID;
        aos_mutex_unlock(&g_auth_ctx.node_mutex);
    }
    return 0;
}

int gateway_occ_node_auth_cache_clear()
{
    if (!g_auth_ctx.init_flag) {
        LOGE(TAG, "occ auth not init");
        return -1;
    }

    if (!g_auth_ctx.enable_flag) {
        LOGE(TAG, "occ auth not enabled");
        return -1;
    }

    aos_mutex_lock(&g_auth_ctx.node_mutex, AOS_WAIT_FOREVER);
    memset(g_auth_ctx.in_nodes, 0x0, sizeof(g_auth_ctx.in_nodes));
    memset(g_auth_ctx.out_nodes, 0x0, sizeof(g_auth_ctx.out_nodes));
    for (int i = 0; i < sizeof(g_auth_ctx.in_nodes) / sizeof(g_auth_ctx.in_nodes[0]); i++) {
        g_auth_ctx.in_nodes[i].auth_out_index = NO_INDEX_VALID;
    }
    aos_mutex_unlock(&g_auth_ctx.node_mutex);
    return 0;
}

static int post_getjs_msg_url(char *js_url, int len)
{
    int  ret         = 0;
    char occ_url[40] = { 0 };
    int  occ_url_len = sizeof(occ_url);

    ret = aos_kv_get("occurl", occ_url, &occ_url_len);
    if (ret != 0) {
        strcpy(occ_url, DEFAULT_URL);
    }

    snprintf(js_url, len, "%s%s", occ_url, "/api/device/gateway/getScriptFile");

    return 0;
}

static int post_getcert_url(char *ota_url, int len)
{
    int  ret         = 0;
    char occ_url[40] = { 0 };
    int  occ_url_len = sizeof(occ_url);

    ret = aos_kv_get("occurl", occ_url, &occ_url_len);
    if (ret != 0) {
        strcpy(occ_url, DEFAULT_URL);
    }

    snprintf(ota_url, len, "%s%s", occ_url, "/api/device/gateway/getCert");

    return 0;
}

int gateway_occ_get_dev_triples(uint8_t *pcid, gw_subdev_triples_t *gw_occ_subdev_triples)
{
    int                  ret     = 0;
    char *               payload = NULL;
    char                 getvalue[255];
    cJSON *              js     = NULL;
    char *               buffer = NULL;
    http_errors_t        err;
    http_client_config_t config = { 0 };
    http_client_handle_t client = NULL;
    key_handle           key_addr;
    char                 cidr[32 + 1];
    char                 authmsg[20];
    char                 temp[33 * 2];
    char                 timestr[10];
    uint32_t             t;
    uint32_t             cid_len;
    struct timeval       t1;

    if ((pcid == NULL) || (gw_occ_subdev_triples == NULL)) {
        LOGE(TAG, "invalid param");
        return -1;
    }

    gettimeofday(&t1, NULL);

    buffer = aos_zalloc(BUFFER_SIZE + 1);
    if (buffer == NULL) {
        ret = -ENOMEM;
        goto out;
    }
    if ((payload = aos_malloc(256)) == NULL) {
        ret = -ENOMEM;
        goto out;
    }

    memset(cidr, 0, sizeof(cidr));
    ret = km_get_key(KEY_ID_CHIPID, &key_addr, &cid_len);
    if (ret != KM_OK) {
        printf("===%s, %d\n", __FUNCTION__, __LINE__);
        goto out;
    }
    memcpy(cidr, (void *)key_addr, 32);
    stringtohex(cidr, (uint8_t *)temp, 16);
    cid_len = 16;

    t = t1.tv_sec;
    sprintf(timestr, "%d", t);

    ret = sc_srv_get_authmsg((uint8_t *)temp, cid_len, (unsigned char *)timestr, strlen(timestr), (uint8_t *)authmsg);
    snprintf(payload, 256, "{\"cid\":\"%s\", \"authMsg\":{\"cid\":\"%s\", \"timestamp\":\"%s\", \"authcode\":\"%s\"}}",
             pcid, cidr, timestr, hextostr((uint8_t *)authmsg, temp, 20));

    LOGD(TAG, "check: %s", payload);

    memset(getvalue, 0, sizeof(getvalue));
    post_getcert_url(getvalue, sizeof(getvalue));

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
    /* deviceName */
    cJSON *deviceName = cJSON_GetObjectItem(result, "deviceName");
    if (!(deviceName && cJSON_IsString(deviceName))) {
        LOGW(TAG, "get deviceName failed");
        ret = -1;
        goto out;
    }
    strcpy((char *)gw_occ_subdev_triples->device_name, deviceName->valuestring);
    LOGD(TAG, "deviceName: %s", gw_occ_subdev_triples->device_name);
    /* productKey */
    cJSON *productKey = cJSON_GetObjectItem(result, "productKey");
    if (!(productKey && cJSON_IsString(productKey))) {
        LOGW(TAG, "get productKey failed");
        ret = -1;
        goto out;
    }
    strcpy((char *)gw_occ_subdev_triples->product_key, productKey->valuestring);
    LOGD(TAG, "productKey: %s", gw_occ_subdev_triples->product_key);
    /* productSecret */
    cJSON *productSecret = cJSON_GetObjectItem(result, "productSecret");
    if (!(productSecret && cJSON_IsString(productSecret))) {
        LOGW(TAG, "get productSecret failed");
        ret = -1;
        goto out;
    }
    strcpy((char *)gw_occ_subdev_triples->product_secret, productSecret->valuestring);
    LOGD(TAG, "productSecret: %s", gw_occ_subdev_triples->product_secret);
    /* productId */
    cJSON *productId = cJSON_GetObjectItem(result, "productId");
    if (!(productId && cJSON_IsString(productId))) {
        LOGW(TAG, "get productId failed");
        ret = -1;
        goto out;
    }
    strcpy((char *)gw_occ_subdev_triples->occ_pid, productId->valuestring);
    LOGD(TAG, "productId: %s", gw_occ_subdev_triples->occ_pid);

out:
    if (buffer)
        aos_free(buffer);
    if (payload)
        aos_free(payload);
    if (js)
        cJSON_Delete(js);
    _http_cleanup(client);
    return ret;
}

#if defined(CONFIG_SAVE_JS_TO_RAM) && (CONFIG_SAVE_JS_TO_RAM == 1)

#else
static int _save_to_file(const char *path, const char *data, size_t size)
{
    int rc;
    int fd = aos_open(path, O_CREAT | O_RDWR | O_TRUNC);

    rc = aos_write(fd, data, size);
    if (rc != size) {
        LOGE(TAG, "write fail: rc = %d, size = %u", rc, size);
        rc = -1;
        goto err;
    }
    aos_sync(fd);
    rc = 0;

err:
    if (fd >= 0)
        aos_close(fd);
    return rc;
}
#endif

void occ_getjs(void *priv)
{
#define JS_BUF_SIZE  20 * 1024
#define PAYLOAD_SIZE 256
    char *               pid     = priv;
    int                  ret     = 0;
    char *               payload = NULL;
    cJSON *              js      = NULL;
    char *               buffer  = NULL;
    http_errors_t        err;
    http_client_config_t config = { 0 };
    http_client_handle_t client = NULL;
    key_handle           key_addr;
    char                 cidr[32 + 1];
    char                 authmsg[20];
    char                 temp[33 * 2];
    char                 timestr[10];
    char                 getvalue[255];
    uint32_t             cid_len;
    uint32_t             t;
    struct timeval       t1;
    gettimeofday(&t1, NULL);

    buffer = aos_zalloc(JS_BUF_SIZE + 1);
    if (buffer == NULL) {
        ret = -ENOMEM;
        goto out;
    }

    payload = aos_malloc(PAYLOAD_SIZE);
    if (payload == NULL) {
        ret = -ENOMEM;
        goto out;
    }

    cidr[32] = 0;
    memset(cidr, 0, sizeof(cidr));
    ret = km_get_key(KEY_ID_CHIPID, &key_addr, &cid_len);
    if (ret != KM_OK) {
        goto out;
    }
    memcpy(cidr, (void *)key_addr, 32);
    stringtohex(cidr, (uint8_t *)temp, 16);
    cid_len = 16;
    t       = t1.tv_sec;
    sprintf(timestr, "%d", t);

    ret = sc_srv_get_authmsg((uint8_t *)temp, cid_len, (unsigned char *)timestr, strlen(timestr), (uint8_t *)authmsg);

    snprintf(payload, PAYLOAD_SIZE,
             "{\"productId\":\"%s\", \"authMsg\":{\"cid\":\"%s\", \"timestamp\":\"%s\", \"authcode\":\"%s\"}}", pid,
             cidr, timestr, hextostr((const uint8_t *)authmsg, temp, sizeof(authmsg)));

    memset(getvalue, 0, sizeof(getvalue));
    post_getjs_msg_url(getvalue, sizeof(getvalue));

    LOGD(TAG, "js url:%s", getvalue);
    config.method        = HTTP_METHOD_POST;
    config.url           = getvalue;
    config.timeout_ms    = 10000;
    config.buffer_size   = JS_BUF_SIZE;
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
    err = _http_connect(client, payload, buffer, JS_BUF_SIZE);
    if (err != HTTP_CLI_OK) {
        LOGE(TAG, "Client connect e");
        ret = -1;
        goto out;
    }
    int read_len = http_client_read(client, buffer, JS_BUF_SIZE);
    if (read_len <= 0) {
        ret = -1;
        goto out;
    }
    buffer[read_len] = 0;
    // LOGD(TAG, "resp: %s", buffer);

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
    if (code->valueint != 0) {
        ret = -1;
        goto out;
    }

    cJSON *result = cJSON_GetObjectItem(js, "result");
    if (!(result && cJSON_IsObject(result))) {
        LOGW(TAG, "get result failed");
        ret = -1;
        goto out;
    }

    cJSON *file = cJSON_GetObjectItem(result, "file");
    if (!(file && cJSON_IsString(file))) {
        LOGW(TAG, "get file failed");
        ret = -1;
        goto out;
    }
    LOGD(TAG, "js file: %s", file->valuestring);
#if defined(CONFIG_SAVE_JS_TO_RAM) && (CONFIG_SAVE_JS_TO_RAM == 1)
    ret = save_js_to_ram(pid, file->valuestring, strlen(file->valuestring));
#else
    ret = _save_to_file("/lfs/download.js", file->valuestring, strlen(file->valuestring));
    LOGD(TAG, "save file ret: %d", ret);
#endif

out:
    aos_sem_signal(&js_sem);
    if (buffer)
        aos_free(buffer);
    if (js)
        cJSON_Delete(js);
    _http_cleanup(client);
    return;
}

int gateway_occ_getjs(char *pid)
{
    return aos_task_new("occ", occ_getjs, pid, 8 * 1024);
}

#if defined(CONFIG_TSL_DEVICER_MESH) && CONFIG_TSL_DEVICER_MESH

static void _gateway_init_jse(void *priv)
{
    jse_init();
    jse_start();
    gateway_model_conv_init();
}

int gateway_jse_init(occ_js_get_cb cb)
{
    int ret = 0;
    ret     = aos_task_new("init jse related resource", _gateway_init_jse, NULL, 1024 * 8);
    if (ret) {
        LOGE(TAG, "jse task create failed %d", ret);
        return ret;
    }
    return aos_post_delayed_action(GW_GET_OCC_JS_DELAYED_TIME, cb, NULL);
}
#endif
