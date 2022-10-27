/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#if defined(CONFIG_BT_MESH_MODEL_BLOB_CLI) && CONFIG_BT_MESH_MODEL_BLOB_CLI > 0
#include "stdint.h"
#include "errno.h"
#include "mesh_model/mesh_model.h"
#include "mesh_ota/mesh_ota_server.h"
#include "sig_model/blob_cli.h"
#include "inc/net.h"
#include "ota_process.h"
#include "aos/kernel.h"
#include "mesh_node.h"
#include "timer_port.h"
#include "inc/mesh.h"
#include "common_mesh_ota.h"

#define TAG             "MESH_OTA_SERVER"
#define DST_STATE_UNSET 0Xff

typedef struct {
    upgrade_firmware *firmware;
    uint16_t          index;
} _mesh_ota_firmware_info;

typedef struct {
    uint8_t  type;
    uint8_t  try_num;
    uint8_t  group_max_try;
    uint8_t  unicast_max_try;
    uint16_t dst_group_addr;
} version_get_ctx;

enum
{
    MESH_OTA_SERVER_STATE_IDLE           = 0x00,
    MESH_OTA_SERVER_STATE_GET_VERSION    = 0x01,
    MESH_OTA_SERVER_STATE_TRANS_START    = 0x02,
    MESH_OTA_SERVER_STATE_TRANS_COMPLETE = 0x03,
    MESH_OTA_SERVER_STATE_CANCEL         = 0x04,
    MESH_OTA_SERVER_STATE_COMPLETE       = 0x05,
    MESH_OTA_SERVER_STATE_FAIL           = 0x06,
};

static char *ota_state_str(uint8_t mesh_ota_state)
{
    switch (mesh_ota_state) {
        case MESH_OTA_SERVER_STATE_IDLE:
            return "IDLE";
        case MESH_OTA_SERVER_STATE_GET_VERSION:
            return "GET_VERSION";
        case MESH_OTA_SERVER_STATE_TRANS_START:
            return "TRANS_START";
        case MESH_OTA_SERVER_STATE_TRANS_COMPLETE:
            return "TRANS_COMPLETE";
        case MESH_OTA_SERVER_STATE_CANCEL:
            return "CANCEL";
        case MESH_OTA_SERVER_STATE_COMPLETE:
            return "COMPLETE";
        case MESH_OTA_SERVER_STATE_FAIL:
            return "FAIL";
        default:
            return "UNKNOW";
    }
}

enum
{
    NODE_STATUS_RECV_NULL = 0x00,
    NODE_STATUS_RECV_SOME = 0x01,
    NODE_STATUS_RECV_ALL  = 0x02,
};

enum
{
    NODE_VERSION_GET_BY_GROUP_ADDR   = 0x00,
    NODE_VERSION_GET_BY_UNICAST_ADDR = 0x01,
};

typedef struct _mesh_ota_server_ctx {
    uint8_t                 init_flag;
    uint8_t                 status;
    uint8_t                 enter_ota_flag;
    uint8_t                 ota_dev_num;
    blob_trans_info         blob_info;
    _mesh_ota_firmware_info fir_info;
    ota_timer_t             retry_timer;
    slist_t                 cancel_list_head;
    aos_sem_t               mesh_ota_sem;
    aos_mutex_t             enter_ota_mutex;
    version_get_ctx         get_version_ctx;
} mesh_ota_server_ctx;

mesh_ota_server_ctx g_mesh_ota_server_ctx;

static void set_enter_ota_flag()
{
    aos_mutex_lock(&g_mesh_ota_server_ctx.enter_ota_mutex, AOS_WAIT_FOREVER);
    g_mesh_ota_server_ctx.enter_ota_flag = 1;
    aos_mutex_unlock(&g_mesh_ota_server_ctx.enter_ota_mutex);
}

static void clear_enter_ota_flag()
{
    aos_mutex_lock(&g_mesh_ota_server_ctx.enter_ota_mutex, AOS_WAIT_FOREVER);
    g_mesh_ota_server_ctx.enter_ota_flag = 0;
    aos_mutex_unlock(&g_mesh_ota_server_ctx.enter_ota_mutex);
}

static u8_t get_enter_ota_flag()
{
    aos_mutex_lock(&g_mesh_ota_server_ctx.enter_ota_mutex, AOS_WAIT_FOREVER);
    u8_t enter_flag = g_mesh_ota_server_ctx.enter_ota_flag;
    aos_mutex_unlock(&g_mesh_ota_server_ctx.enter_ota_mutex);
    return enter_flag;
}

extern int ble_mesh_node_get_node_app_version(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t addr);

static void retry_timer_start(uint32_t timeout)
{
    int ret = 0;
    LOGD(TAG, "retry timer start procedure now:%02x", g_mesh_ota_server_ctx.status);
    ret = ota_timer_start(&g_mesh_ota_server_ctx.retry_timer, timeout);
    if (ret) {
        LOGE(TAG, "timer start failed %d", ret);
    }
}

static void retry_timer_stop()
{
    int ret = 0;
    LOGD(TAG, "retry timer stop procedure now:%02x", g_mesh_ota_server_ctx.status);
    ret = ota_timer_stop(&g_mesh_ota_server_ctx.retry_timer);
    if (ret) {
        LOGE(TAG, "timer stop failed %d", ret);
    }
}

static void reset_get_version_ctx()
{
    g_mesh_ota_server_ctx.get_version_ctx.try_num = 0;
    if (g_mesh_ota_server_ctx.ota_dev_num < CONFIG_BT_MESH_OTA_GET_VERSION_BY_GROUP_ADDR_THREAD) {
        g_mesh_ota_server_ctx.get_version_ctx.type          = NODE_VERSION_GET_BY_UNICAST_ADDR;
        g_mesh_ota_server_ctx.get_version_ctx.group_max_try = 0;
    } else {
        g_mesh_ota_server_ctx.get_version_ctx.type          = NODE_VERSION_GET_BY_GROUP_ADDR;
        g_mesh_ota_server_ctx.get_version_ctx.group_max_try = CONFIG_BT_MESH_OTA_GET_VERSION_BY_GROUP_ADDR_MAX_RETRY;
    }
}

static int _mesh_ota_server_state_set_fail(upgrade_device *dev, uint8_t reason, uint32_t new_version)
{
    int                     ret = 0;
    ota_process_cmd_message message;
    message.event = OTA_EVENT_FAILED;
#if 0
    ret = ble_mesh_blob_cli_blob_trans_cancel(CONFIG_BT_MESH_OTA_BLOB_CLI_ELEM_ID, g_mesh_ota_server_ctx.blob_info.blob_id, 1, &dev->device.unicast_addr);
    if(ret) {
        LOGE(TAG,"Trans blob cancel fail for %04x",dev->device.unicast_addr);
    }
#endif
    memcpy(&message.data.success_data.dev_info, &dev->device, sizeof(device_info));
    message.data.fail_data.reason         = reason;
    message.data.fail_data.old_ver        = dev->old_version;
    message.data.fail_data.new_ver        = new_version;
    message.data.fail_data.cost_time      = aos_now_ms() - dev->start_time;
    message.data.fail_data.channel        = g_mesh_ota_server_ctx.fir_info.firmware->info.ota_chanel;
    message.data.fail_data.firmware_index = g_mesh_ota_server_ctx.fir_info.index;
    if (g_mesh_ota_server_ctx.ota_dev_num) {
        g_mesh_ota_server_ctx.ota_dev_num--;
    }
    dev->ota_state = OTA_STATE_FAILED;
    ret            = ota_process_cmd_set(&message);
    if (ret) {
        LOGE(TAG, "mesh ota server send fail state fail");
    }
    return 0;
}

static int mesh_ota_server_state_set_success(upgrade_device *dev, uint32_t new_version)
{
    int                     ret = 0;
    ota_process_cmd_message message;
    message.event = OTA_EVENT_SUCCESS;
    memcpy(&message.data.success_data.dev_info, &dev->device, sizeof(device_info));
    message.data.success_data.old_ver = dev->old_version;
    message.data.success_data.new_ver = new_version;
    if (dev->download_finish_time < dev->start_time) {
        message.data.success_data.cost_time = 0;
    } else {
        message.data.success_data.cost_time = dev->download_finish_time - dev->start_time;
    }

    message.data.success_data.channel        = g_mesh_ota_server_ctx.fir_info.firmware->info.ota_chanel;
    message.data.success_data.firmware_index = g_mesh_ota_server_ctx.fir_info.index;
    dev->ota_state                           = OTA_STATE_SUCCESS;
    if (g_mesh_ota_server_ctx.ota_dev_num) {
        g_mesh_ota_server_ctx.ota_dev_num--;
    }
    ota_process_cmd_set(&message);
    if (ret) {
        LOGE(TAG, "mesh ota server send success state fail");
    }

    return 0;
}

static int mesh_ota_server_state_set_ongoing(upgrade_device *dev)
{
    int ret = 0;

    ota_process_cmd_message message;
    message.event = OTA_EVENT_ONGOING;
    memcpy(&message.data.ongoing_data.dev_info, &dev->device, sizeof(device_info));
    message.data.ongoing_data.channel        = g_mesh_ota_server_ctx.fir_info.firmware->info.ota_chanel;
    message.data.ongoing_data.firmware_index = g_mesh_ota_server_ctx.fir_info.index;
    dev->ota_state                           = OTA_STATE_ONGOING;
    dev->start_time                          = aos_now_ms();
    ret                                      = ota_process_cmd_set(&message);
    if (ret) {
        LOGE(TAG, "mesh ota process state set failed\r\n");
        return ret;
    }
    return 0;
}

static uint32_t get_node_version(upgrade_device *device, uint8_t reason)
{
    switch (reason) {
        case OTA_FAIL_GET_VERSION:
            return device->old_version;
            break;
        default:
            return device->old_version;
            break;
    }
}

static void _blob_send_start(u8_t elem_id, u64_t blob_id, int err, void *cb_data)
{
    LOGD(TAG, "Start trans ota image id:%04x using elem:%d", blob_id, elem_id);
}

static void report_each_fail_node(uint8_t reason, uint8_t dst_state)
{
    upgrade_device *device = NULL;
    device                 = dev_list_dev_get(&g_mesh_ota_server_ctx.fir_info.firmware->dev_list_head);
    if (!device) {
        return;
    }
    do {
        if (dst_state == DST_STATE_UNSET || dst_state == device->ota_state) {
            _mesh_ota_server_state_set_fail(device, reason, get_node_version(device, reason));
        }
        device = dev_list_dev_get(&device->list);
    } while (device);
}

static int _exit_mesh_ota_process(uint8_t status_now)
{
    LOGD(TAG, "exist mesh ota process status now %s", ota_state_str(status_now));
    report_each_fail_node(OTA_FAIL_SEND_FIRMWARE, DST_STATE_UNSET);
    return 0;
}

static void _blob_send_end(u8_t elem_id, u64_t blob_id, int err, void *cb_data)
{
    if (blob_id != g_mesh_ota_server_ctx.blob_info.blob_id) { // Consider elem id
        LOGW(TAG, "Ignore the send end,the blob id is %04x,the dst is %04x", blob_id,
             g_mesh_ota_server_ctx.blob_info.blob_id);
        return;
    }
    exist_data *data = (exist_data *)cb_data;

    LOGD(TAG, "blob ID:%04x send end,procedure:%02x,status:%02x", blob_id, data->procedure, data->status);
    switch (data->procedure) {
        case BLOB_CLI_PROCEDURE_TRANSFER_COMPLETE: {
            g_mesh_ota_server_ctx.status = MESH_OTA_SERVER_STATE_GET_VERSION;
            retry_timer_stop();
            retry_timer_start(CONFIG_BT_MESH_OTA_GET_VERSION_DELAY_AFTER_DOWNLOAD);
        } break;
        default:
            _exit_mesh_ota_process(data->procedure);
            break;
    }
}

uint8_t get_ota_fail_reason(uint8_t status)
{
    switch (status) {
        case TRANSFER_STATUS_SUCCESS:
            return 0;
        case TRANSFER_STATUS_INVALID_BLOCK_NUMBER:
        case TRANSFER_STATUS_INVALID_BLOCK_SIZE:
        case TRANSFER_STATUS_BLOB_TOO_LARGE:
        case TRANSFER_STATUS_INVALID_CHUNK_SIZE:
        case TRANSFER_STATUS_INVALID_PARAMETER:
        case TRANSFER_STATUS_WRONG_BLOB_ID:
        case TRANSFER_STATUS_UNSUPPORTED_TRANSFER_MODE:
            return OTA_FAIL_ERR_TRANS_INFO;
        case TRANSFER_STATUS_INTERNAL_ERROR:
        case TRANSFER_STATUS_INFOMATION_UNAVAILABLE:
            return OTA_FAIL_DEV_FAILED;
        case TRANSFER_STATUS_SET_ID_FAILED:
            return OTA_FAIL_SET_TRANS_ID;
    }
    return 0;
}

void _blob_node_status_cb(u8_t elem_id, u64_t blob_id, u16_t unicast_addr, transfer_phase_en phase,
                          transfer_status_en status)
{
    if (blob_id != g_mesh_ota_server_ctx.blob_info.blob_id) { // Consider elem id
        LOGW(TAG, "Ignore the node status cb,the blob id is %04x,the dst is %04x", blob_id,
             g_mesh_ota_server_ctx.blob_info.blob_id);
        return;
    }
    device_info info = {
        .unicast_addr = unicast_addr,
    };
    upgrade_device *device = dev_list_dev_search(&g_mesh_ota_server_ctx.fir_info.firmware->dev_list_head, info,
                                                 DEV_LIST_FOUND_BY_UNICAST_ADDR);
    if (!device) {
        LOGW(TAG, "Node %04x not found in the dev list", unicast_addr);
        return;
    }
    uint8_t ota_fail_reason = get_ota_fail_reason(status);
    LOGD(TAG, "Node %04x phase %s blob status %s", unicast_addr, phase_str(phase), trans_status_str(status));
    switch (phase) {
        case TRANSFER_PHASE_COMPLETE: {
            if (!ota_fail_reason) { // TODO
                device->ota_state            = OTA_STATE_LOAD_COMPLETE;
                device->download_finish_time = aos_now_ms();
                LOGD(TAG, "set node %04x load success", unicast_addr);
            }
        } break;
        default:
            if (ota_fail_reason) {
                _mesh_ota_server_state_set_fail(device, ota_fail_reason, device->old_version); // TODO
            }
            break;
    }
}

static blob_send_cb blob_cb = {
    .send_start_cb  = _blob_send_start,
    .send_end_cb    = _blob_send_end,
    .node_status_cb = _blob_node_status_cb,
};

static int _mesh_node_get_node_app_version(upgrade_firmware *firmware_info, version_get_ctx *ctx)
{
    int             ret    = 0;
    upgrade_device *device = NULL;

    static uint8_t send_fail_last_flag = 0;

    if (!firmware_info || !ctx) {
        LOGE(TAG, "Err get node version arg");
        return -EINVAL;
    }

    LOGD(TAG, "Version get type:%02x, try num:%d,group max:%d, unicast max:%d %d", ctx->type, ctx->try_num,
         ctx->group_max_try, ctx->unicast_max_try, send_fail_last_flag);

    if (ctx->type == NODE_VERSION_GET_BY_GROUP_ADDR && ctx->try_num >= ctx->group_max_try) {
        ctx->try_num = 0;
        ctx->type    = NODE_VERSION_GET_BY_UNICAST_ADDR;
    }

    if (ctx->type == NODE_VERSION_GET_BY_GROUP_ADDR) {
        ret = ble_mesh_node_get_node_app_version(0x0, 0x0, ctx->dst_group_addr);
        if (ret) {
            LOGE(TAG, "Send get node version faid %d", ret);
            return ret;
        }
        ctx->try_num++;
    } else {
        device = dev_list_dev_get(&firmware_info->dev_list_head);
        if (!device) {
            LOGW(TAG, "No dev found in get version list");
            return -EALREADY;
        }
        do {
            if (ret != -ENOBUFS && device
                && (device->ota_state == OTA_STATE_IDLE || device->ota_state == OTA_STATE_LOAD_COMPLETE)) {
                if ((send_fail_last_flag && (device->version_get_status == VERSION_GET_SEND_FAIL))
                    || (!send_fail_last_flag && (device->version_get_status != VERSION_GET_SUCCESS)))
                {
                    ret = ble_mesh_node_get_node_app_version(0x0, 0x0, device->device.unicast_addr);
                    if (ret) {
                        LOGE(TAG, "Send get node version faid %d", ret);
                        if (ret == -ENOBUFS) {
                            device->version_get_status = VERSION_GET_SEND_FAIL;
                        }
                    }
                }
            }
            if (!device) {
                break;
            }
            device = dev_list_dev_get(&device->list);
        } while (device);
        if (!ret) {
            send_fail_last_flag = 0;
            ctx->try_num++;
        } else {
            if (ret == -ENOBUFS) {
                send_fail_last_flag = 1;
            }
        }
    }
    return ret;
}

void _blob_procedure_status_process(model_message message)
{
    uint8_t procedure = net_buf_simple_pull_u8(message.status_data);
    uint8_t status    = net_buf_simple_pull_u8(message.status_data);
    LOGD(TAG, "Mesh ota server blob procedure now:%s status:%04x", procedure_str(procedure), status); // TODO
    switch (procedure) {
        case BLOB_CLI_PROCEDURE_SET_ID: { // Consider all node case
            if (status == PROCEDURE_STATUS_NO_RECEIVE || status == PROCEDURE_STATUS_ALL_RECEIVE_ALL_FAIL
                || status == PROCEDURE_STATUS_SOME_RECEIVE_ALL_FAIL)
            {
                retry_timer_stop(); // only try once when all node status recv
                _exit_mesh_ota_process(g_mesh_ota_server_ctx.status);
            }
        } break;
        case BLOB_CLI_PROCEDURE_TRANSFER_CANCEL: { // Consider
            // aos_sem_signal(&g_mesh_ota_server_ctx.mesh_ota_sem);
        } break;
        case BLOB_CLI_PROCEDURE_RETRIEVE_CAPABLITY: {
            if (status == PROCEDURE_STATUS_NO_RECEIVE || status == PROCEDURE_STATUS_ALL_RECEIVE_ALL_FAIL
                || status == PROCEDURE_STATUS_SOME_RECEIVE_ALL_FAIL)
            {
                retry_timer_stop(); // only try once when all node status recv
                _exit_mesh_ota_process(g_mesh_ota_server_ctx.status);
            }
        } break;
        default: {
            LOGW(TAG, "Unkonw procedure %d", procedure);
        } break;
    }
}

static uint8_t _all_node_version_status_recv_status(slist_t *head)
{
    upgrade_device *device       = NULL;
    device                       = dev_list_dev_get(head);
    uint8_t device_recv_flag     = 0;
    uint8_t device_not_recv_flag = 0;

    device = dev_list_dev_get(head);
    if (!device) {
        return NODE_STATUS_RECV_ALL;
    }

    do {
        if (device->version_get_status == VERSION_GET_SUCCESS) {
            device_recv_flag = 1;
        } else {
            device_not_recv_flag = 1;
        }
        if (device_not_recv_flag && device_recv_flag) {
            return NODE_STATUS_RECV_SOME;
        }
        device = dev_list_dev_get(&device->list);
    } while (device);

    if (device_recv_flag) {
        return device_not_recv_flag ? NODE_STATUS_RECV_SOME : NODE_STATUS_RECV_ALL;
    } else {
        return NODE_STATUS_RECV_NULL;
    }
}

static int _mesh_ota_server_trans_start(upgrade_firmware *firmware_info)
{
    int             ret           = 0;
    uint8_t         dst_addr_size = 0;
    upgrade_device *device        = NULL;
    uint16_t *      dst_addr      = NULL;
    slist_t *       head          = NULL;

    if (!g_mesh_ota_server_ctx.ota_dev_num) {
        LOGE(TAG, "The num of dev need ota is 0");
        return -EINVAL;
    }

    dst_addr = (uint16_t *)aos_zalloc(2 * g_mesh_ota_server_ctx.ota_dev_num);
    if (!dst_addr) {
        LOGE(TAG, "Malloc failed");
        return -ENOMEM;
    }

    dst_addr_size = 0;
    head          = &(firmware_info->dev_list_head);
    do {
        device = dev_list_dev_get(head);
        if (!device) {
            break;
        }
        if (device->ota_state == OTA_STATE_ONGOING) {
            dst_addr[dst_addr_size++] = device->device.unicast_addr;
        }
        device->version_get_status = VERSION_GET_IDLE;
        head                       = &device->list;
    } while (device);

    g_mesh_ota_server_ctx.blob_info.dst_addr_size = dst_addr_size;
    g_mesh_ota_server_ctx.blob_info.dst_addr_list = dst_addr;

    ret = ble_mesh_blob_cli_blob_trans(CONFIG_BT_MESH_OTA_BLOB_CLI_ELEM_ID, &g_mesh_ota_server_ctx.blob_info, blob_cb);
    if (ret) {
        LOGE(TAG, "Blob trans start failed %d", ret);
    }
    aos_free(dst_addr);
    return ret;
}

static int _mesh_ota_server_handle_version(uint16_t unicast_addr, uint32_t node_version)
{
    int         ret  = 0;
    device_info info = {
        .unicast_addr = unicast_addr,
    };

    upgrade_device *device = dev_list_dev_search(&g_mesh_ota_server_ctx.fir_info.firmware->dev_list_head, info,
                                                 DEV_LIST_FOUND_BY_UNICAST_ADDR);
    if (!device) {
        LOGW(TAG, "Device %04x not found in the ota list", unicast_addr);
        return 0;
    }

    if (g_mesh_ota_server_ctx.status != MESH_OTA_SERVER_STATE_GET_VERSION
        && (device->ota_state != OTA_STATE_LOAD_COMPLETE)) {
        LOGW(TAG, "Ignore version message, mesh ota srv state %s, dev ota state %02x",
             ota_state_str(g_mesh_ota_server_ctx.status), device->ota_state);
        return 0;
    }

    device->version_get_status = VERSION_GET_SUCCESS;

    if (device->ota_state == OTA_STATE_LOAD_COMPLETE) {
        if (node_version != g_mesh_ota_server_ctx.fir_info.firmware->info.version) {
            LOGE(TAG, "upgrade fail, the device new version is %04x but the update firmware is %04x", node_version,
                 g_mesh_ota_server_ctx.fir_info.firmware->info.version);
            device->failed_reason = OTA_FAIL_REBOOT_ERR_VERSION;
            _mesh_ota_server_state_set_fail(device, OTA_FAIL_REBOOT_ERR_VERSION, node_version);
        } else {
            mesh_ota_server_state_set_success(device, node_version);
        }
        return 0;
    } else if (device->ota_state == OTA_STATE_IDLE) {
        if (device->old_version != 0 && node_version != device->old_version) {
            LOGE(TAG, "Err version, the device actual version is %04x but dev old version is setted to %04x",
                 node_version, device->old_version);
            device->failed_reason = OTA_FAIL_INVAILD_VERSION;
            device->old_version  = node_version;
            _mesh_ota_server_state_set_fail(device, OTA_FAIL_INVAILD_VERSION, node_version);
            return -1;
        }
        device->old_version = node_version;

#ifndef CONFIG_ALLOW_OTA_FOR_HIGH_VERSION
        if (node_version > g_mesh_ota_server_ctx.fir_info.firmware->info.version) {
            LOGE(TAG, "Err version, the device version is %04x but the update firmware is %04x", node_version,
                 g_mesh_ota_server_ctx.fir_info.firmware->info.version);
            device->failed_reason = OTA_FAIL_INVAILD_VERSION;
            _mesh_ota_server_state_set_fail(device, OTA_FAIL_INVAILD_VERSION, node_version);
            return -1;
        }
#endif

        if (node_version == g_mesh_ota_server_ctx.fir_info.firmware->info.version) {
            LOGD(TAG, "The device version is equal to the update firmware version");
            mesh_ota_server_state_set_success(device, node_version);
            return 0;
        }

        device->ota_state = OTA_STATE_ONGOING;
        mesh_ota_server_state_set_ongoing(device);

        if (_all_node_version_status_recv_status(&g_mesh_ota_server_ctx.fir_info.firmware->dev_list_head)
            == NODE_STATUS_RECV_ALL)
        {
            LOGD(TAG, "recv all node version status,start Blob transfer");
            retry_timer_stop();
            if (!get_enter_ota_flag()) {
                set_enter_ota_flag();
                g_mesh_ota_server_ctx.status = MESH_OTA_SERVER_STATE_TRANS_START;
                ret                          = _mesh_ota_server_trans_start(g_mesh_ota_server_ctx.fir_info.firmware);
                if (ret) {
                    _exit_mesh_ota_process(g_mesh_ota_server_ctx.status);
                    reset_get_version_ctx();
                    clear_enter_ota_flag();
                    return -1;
                }
                reset_get_version_ctx();
            }
        }
        return 0;
    } else {
        LOGW(TAG, "Ignore ota version device %04x state %02x", unicast_addr, device->ota_state);
    }
    return ret;
}

static int _mesh_ota_server_crc_ack(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t dst_addr, uint8_t status)
{
#if defined(CONFIG_BT_MESH_MODEL_VENDOR_CLI) && CONFIG_BT_MESH_MODEL_VENDOR_CLI > 0
    struct bt_mesh_model *vendor_model;
    // int ret = 0;
    vendor_model = ble_mesh_model_find(0, BT_MESH_MODEL_VND_MODEL_CLI, CONFIG_CID_TAOBAO);
    if (!vendor_model) {
        return -ENOTSUP;
    }
    vnd_model_msg vnd_data = { 0 };
    uint8_t       data[4]  = { 0 };

    data[0] = ATTR_TYPE_REPORT_OTA_CRC & 0xff;
    data[1] = (ATTR_TYPE_REPORT_OTA_CRC >> 8) & 0xff;
    data[2] = status;

    vnd_data.netkey_idx = netkey_idx;
    vnd_data.appkey_idx = appkey_idx;
    vnd_data.dst_addr   = dst_addr;
    vnd_data.model      = vendor_model;
    vnd_data.opid       = VENDOR_OP_ATTR_CONFIRM;
    vnd_data.data       = data;
    vnd_data.retry      = 2; // TEMP
    vnd_data.len        = sizeof(data);
    return ble_mesh_vendor_cli_model_msg_send(&vnd_data);
#endif
    return -ENOTSUP;
}

static void _vendor_status_message_process(model_message message)
{
    if (message.ven_data.data_len < 2) {
        return;
    }
    uint8_t *data      = (uint8_t *)message.ven_data.user_data;
    uint16_t status_op = data[0] | data[1] << 8;
    switch (status_op) {
        case ATTR_TYPE_REPORT_VERSION: {
            uint32_t version = (data[2] & 0x00) | data[3] << 16 | data[4] << 8 | data[5];
            LOGD(TAG, "NODE:%04x,Version:%06x", message.source_addr, version);
            _mesh_ota_server_handle_version(message.source_addr, version);
        } break;
        default:
            break;
    }
}

static void _vendor_indicate_message_process(model_message message)
{
    if (message.ven_data.data_len < 2) {
        return;
    }
    uint8_t *data      = (uint8_t *)message.ven_data.user_data;
    uint16_t status_op = data[0] | data[1] << 8;
    int      ret       = 0;
    switch (status_op) {
        case ATTR_TYPE_REPORT_OTA_CRC: {
            device_info info    = { 0x00 };
            info.unicast_addr   = message.source_addr;
            upgrade_device *dev = NULL;
            dev                 = dev_list_dev_search(&g_mesh_ota_server_ctx.fir_info.firmware->dev_list_head, info,
                                      DEV_LIST_FOUND_BY_UNICAST_ADDR);
            if (!dev) {
                LOGD(TAG, "Dev not found in dev list, Ignore the message");
                return;
            }

            uint16_t crc              = data[2] | data[3] << 8;
            uint8_t  crc_check_status = MESH_OTA_CRC_CHECK_SUCCESS;
            if (crc != g_mesh_ota_server_ctx.fir_info.firmware->info.crc16) {
                LOGE(TAG, "NODE:%04x,Err crc:%04x, Actual crc:%04x", message.source_addr, crc,
                     g_mesh_ota_server_ctx.fir_info.firmware->info.crc16);
                crc_check_status = MESH_OTA_CRC_CHECK_FAIL;
                _mesh_ota_server_state_set_fail(dev, OTA_FAIL_CRC_ERR, dev->old_version);
            } else {
                LOGD(TAG, "NODE:%04x,Crc check success:%04x,ota state %02x", message.source_addr, crc, dev->ota_state);
                if (dev->ota_state != OTA_STATE_LOAD_COMPLETE) {
                    ret = ble_mesh_blob_cli_blob_trans_upper_set_success(CONFIG_BT_MESH_OTA_BLOB_CLI_ELEM_ID,
                                                                         g_mesh_ota_server_ctx.blob_info.blob_id,
                                                                         message.source_addr);
                    if (ret) {
                        LOGE(TAG, "Upper set node success failed %d", ret);
                    }
                }
            }
            ret = _mesh_ota_server_crc_ack(0, 0, message.source_addr, crc_check_status);
            if (ret) {
                LOGE(TAG, "Mesh ota srv crc ack failed %d", ret);
            }
        } break;
        default:
            break;
    }
}

static int _mesh_ota_trans_status_process(uint16_t src_addr, struct net_buf_simple *buf)
{
    u8_t hdr         = net_buf_simple_pull_u8(buf);
    u8_t status      = (hdr & 0xF0) >> 4;
    u8_t trans_phase = net_buf_simple_pull_u8(buf);
    LOGD(TAG, "Trans status process for node %04x ,trans phase %04x", src_addr, trans_phase);
    if (status == TRANSFER_STATUS_SUCCESS) {
        switch (trans_phase) {
            case TRANSFER_PHASE_INACTIVE: {
                device_info info    = { 0x00 };
                info.unicast_addr   = src_addr;
                upgrade_device *dev = NULL;
                dev = dev_list_dev_search(&g_mesh_ota_server_ctx.fir_info.firmware->cancel_list_head, info,
                                          DEV_LIST_FOUND_BY_UNICAST_ADDR);
                if (dev) {
                    LOGD(TAG, "Dev found in cacel list");
                    _mesh_ota_server_state_set_fail(dev, OTA_FAIL_CANCEL, dev->old_version);
                    return 0;
                }
                LOGW(TAG, "Dev not found in the cancel list and dev list");
            }
        }
    }
    return 0;
}

/* Mesh Models Event Callback Function */
void mesh_ota_model_event_cb(mesh_model_event_en event, void *p_arg)
{
    upgrade_device *device   = NULL;
    device_info     dev_info = { 0x00 };

    if (!g_mesh_ota_server_ctx.init_flag) {
        LOGI(TAG, "Mesh ota has not been init");
        return;
    }

    switch (event) {
        case BT_MESH_MODEL_BLOB_SET_STATUS: {
            if (p_arg) {
                model_message blob_set_status_message = *(model_message *)p_arg;
                uint8_t       status                  = blob_set_status_message.status_data->data[0];
                dev_info.unicast_addr                 = blob_set_status_message.source_addr;
                device = dev_list_dev_search(&g_mesh_ota_server_ctx.fir_info.firmware->dev_list_head, dev_info,
                                             DEV_LIST_FOUND_BY_UNICAST_ADDR);
                if (device == NULL) {
                    LOGW(TAG, "Addr %04x not found in ota dev list");
                    return;
                }
                LOGI(TAG, "Addr %04x, BLOB Set status %02x\n", blob_set_status_message.source_addr, status);
                if (status) {
                    _mesh_ota_server_state_set_fail(device, OTA_FAIL_SET_TRANS_ID, device->old_version);
                }
            }
        } break;

        case BT_MESH_MODEL_BLOB_TRANS_STATUS: {
            if (p_arg) {
                /* Receive Status from OnOff Server */
                model_message blob_trans_status_message = *(model_message *)p_arg;
                LOGI(TAG, "Blob trans status %04x\n", blob_trans_status_message.source_addr);
                struct net_buf_simple_state state;
                net_buf_simple_save(blob_trans_status_message.status_data, &state);
                _mesh_ota_trans_status_process(blob_trans_status_message.source_addr,
                                               blob_trans_status_message.status_data);
                net_buf_simple_restore(blob_trans_status_message.status_data, &state);
            }
        } break;
        case BT_MESH_MODEL_BLOB_ALL_SRV_PROCEDURE_STATUS: {
            if (p_arg) {
                model_message blob_procedure_status_message = *(model_message *)p_arg;
                _blob_procedure_status_process(blob_procedure_status_message);
            }
        } break;
        case BT_MESH_MODEL_VENDOR_MESH_STATUS: {
            model_message message = *(model_message *)p_arg;
            _vendor_status_message_process(message);
        } break;
        case BT_MESH_MODEL_VENDOR_MESH_INDICATE: {
            model_message message = *(model_message *)p_arg;
            _vendor_indicate_message_process(message);
        } break;
        default:
            break;
    }
}

struct model_cb g_model_cb = {
    .event_cb = mesh_ota_model_event_cb,
};

static uint8_t is_get_version_timeout(version_get_ctx *ctx)
{
    if (ctx->type == NODE_VERSION_GET_BY_UNICAST_ADDR && ctx->try_num >= ctx->unicast_max_try) {
        return 1;
    } else {
        return 0;
    }
}

static void _mesh_ota_server_version_get_retry_handler(void *timer, void *args)
{
    int ret = 0;
    retry_timer_stop(&g_mesh_ota_server_ctx.retry_timer);

    uint8_t all_node_status
        = _all_node_version_status_recv_status(&g_mesh_ota_server_ctx.fir_info.firmware->dev_list_head);
    uint8_t try_timeout_flag = is_get_version_timeout(&g_mesh_ota_server_ctx.get_version_ctx);
    LOGD(TAG, "Mesh ota version get retry timeout,try num %d,status now %s, all node status:%d, try timeout:%d",
         g_mesh_ota_server_ctx.get_version_ctx.try_num, ota_state_str(g_mesh_ota_server_ctx.status), all_node_status,
         try_timeout_flag);
    if (all_node_status == NODE_STATUS_RECV_ALL || (all_node_status == NODE_STATUS_RECV_SOME && try_timeout_flag)) {
        if (!get_enter_ota_flag()) {
            if (all_node_status == NODE_STATUS_RECV_SOME) { // report get version fail node
                report_each_fail_node(OTA_FAIL_GET_VERSION, OTA_STATE_IDLE);
            }
            g_mesh_ota_server_ctx.status = MESH_OTA_SERVER_STATE_TRANS_START;
            set_enter_ota_flag();
            ret = _mesh_ota_server_trans_start(g_mesh_ota_server_ctx.fir_info.firmware);
            if (ret) {
                _exit_mesh_ota_process(g_mesh_ota_server_ctx.status);
                clear_enter_ota_flag();
                return;
            }
            reset_get_version_ctx();
        } else {
            if (all_node_status == NODE_STATUS_RECV_SOME) { // report get version fail node
                report_each_fail_node(OTA_FAIL_GET_VERSION, OTA_STATE_LOAD_COMPLETE);
            } else {                                                  // RECV ALL
                _exit_mesh_ota_process(g_mesh_ota_server_ctx.status); /// report success
            }
            reset_get_version_ctx();
        }
    } else if ((all_node_status == NODE_STATUS_RECV_NULL || all_node_status == NODE_STATUS_RECV_SOME)
               && !try_timeout_flag) {
        ret = _mesh_node_get_node_app_version(g_mesh_ota_server_ctx.fir_info.firmware,
                                              &g_mesh_ota_server_ctx.get_version_ctx);
        if (ret) {
            if (ret == -EALREADY) {
                return;
            } else if (ret == -ENOBUFS) {
                retry_timer_start(CONFIG_BT_MESH_OTA_GET_VERSION_NO_BUFFER_RETRY_DELAY);
            } else {
                retry_timer_start(CONFIG_BT_MESH_OTA_GET_VERSION_TIMEOUT);
            }
        } else {
            retry_timer_start(CONFIG_BT_MESH_OTA_GET_VERSION_TIMEOUT);
        }
    } else if (all_node_status == NODE_STATUS_RECV_NULL) {
        if (get_enter_ota_flag()) {
            report_each_fail_node(OTA_FAIL_GET_VERSION, OTA_STATE_LOAD_COMPLETE);
        } else {
            report_each_fail_node(OTA_FAIL_GET_VERSION, OTA_STATE_IDLE);
        }
        _exit_mesh_ota_process(g_mesh_ota_server_ctx.status);
    } else {
        LOGE(TAG, "unkonw case");
    }
}

// For user
int mesh_ota_server_init(uint16_t index, uint8_t channel, upgrade_firmware *firmware_info)
{
    int             ret    = 0;
    upgrade_device *device = NULL;
    slist_t *       head   = &firmware_info->dev_list_head;
    if (g_mesh_ota_server_ctx.init_flag) {
        return -EALREADY;
    }

    memset(&g_mesh_ota_server_ctx, 0x00, sizeof(mesh_ota_server_ctx));
    ble_mesh_model_cb_register(&g_model_cb);

    do {
        device = dev_list_dev_get(head);
        if (!device) {
            break;
        }
        g_mesh_ota_server_ctx.ota_dev_num++;
        head = &device->list;
    } while (device);

    g_mesh_ota_server_ctx.blob_info.multicast_addr        = firmware_info->info.multicast_addr;
    g_mesh_ota_server_ctx.blob_info.netkey_idx            = 0x00; // CONFIG
    g_mesh_ota_server_ctx.blob_info.appkey_idx            = 0x00;
    g_mesh_ota_server_ctx.blob_info.ttl                   = 0x03;
    g_mesh_ota_server_ctx.blob_info.blob_id               = firmware_info->info.crc16;
    g_mesh_ota_server_ctx.blob_info.trans_mode            = TRANSFER_MODE_PUSH;
    g_mesh_ota_server_ctx.blob_info.trans_options         = BLOB_TRANS_NO_INIT_TRANS_GET;
    g_mesh_ota_server_ctx.blob_info.timeout               = 0;
    g_mesh_ota_server_ctx.blob_info.blob_data             = firmware_info->info.address;
    g_mesh_ota_server_ctx.blob_info.blob_data_len         = firmware_info->info.size;
    g_mesh_ota_server_ctx.fir_info.firmware               = firmware_info;
    g_mesh_ota_server_ctx.fir_info.index                  = index;
    g_mesh_ota_server_ctx.get_version_ctx.try_num         = 0;
    g_mesh_ota_server_ctx.get_version_ctx.dst_group_addr  = CONFIG_BT_MESH_OTA_NODE_COMMON_SUB_ADDR;
    g_mesh_ota_server_ctx.get_version_ctx.unicast_max_try = CONFIG_BT_MESH_OTA_GET_VERSION_BY_UNICAST_ADDR_MAX_RETRY;
    if (g_mesh_ota_server_ctx.ota_dev_num < CONFIG_BT_MESH_OTA_GET_VERSION_BY_GROUP_ADDR_THREAD) {
        g_mesh_ota_server_ctx.get_version_ctx.type          = NODE_VERSION_GET_BY_UNICAST_ADDR;
        g_mesh_ota_server_ctx.get_version_ctx.group_max_try = 0;
    } else {
        g_mesh_ota_server_ctx.get_version_ctx.type          = NODE_VERSION_GET_BY_GROUP_ADDR;
        g_mesh_ota_server_ctx.get_version_ctx.group_max_try = CONFIG_BT_MESH_OTA_GET_VERSION_BY_GROUP_ADDR_MAX_RETRY;
    } // TODO

#if 0
// for legacy mesh ota channel
    if (channel == OTA_CHANNEL_ON_MESH_LEGACY) {
        g_mesh_ota_server_ctx.blob_info.trans_net_if = NET_TRANS_LEGACY;
    } else 
#endif

    if (channel == OTA_CHANNEL_ON_MESH_EXT_1M) {
#if defined(CONFIG_BT_MESH_EXT_ADV) && CONFIG_BT_MESH_EXT_ADV > 0
        g_mesh_ota_server_ctx.blob_info.trans_net_if = NET_TRANS_EXT_ADV_1M;
#else
        LOGE(TAG, "Unsport mesh trans channel %02x", channel);
        return -EINVAL;
#endif
    } else if (channel == OTA_CHANNEL_ON_MESH_EXT_2M) {
#if defined(CONFIG_BT_MESH_EXT_ADV) && CONFIG_BT_MESH_EXT_ADV > 0
        g_mesh_ota_server_ctx.blob_info.trans_net_if = NET_TRANS_EXT_ADV_2M;
#else
        LOGE(TAG, "Unsport mesh trans channel %02x", channel);
        return -EINVAL;
#endif
    } else {
        LOGE(TAG, "Unsport mesh trans channel %02x", channel);
        return -EINVAL;
    }

    if (!ble_mesh_model_find(CONFIG_BT_MESH_OTA_BLOB_CLI_ELEM_ID, BT_MESH_MODEL_ID_BLOB_CLI, 0xFFFF)) {
        LOGE(TAG, "Mesh Blob cli model not found in elem %d", CONFIG_BT_MESH_OTA_BLOB_CLI_ELEM_ID);
        return -ENOTSUP;
    }

    ret = ota_timer_init(&g_mesh_ota_server_ctx.retry_timer, _mesh_ota_server_version_get_retry_handler, NULL);
    if (ret) {
        LOGE(TAG, "Retry timer create failed %d", ret);
        return ret;
    }

    ret = aos_sem_new(&g_mesh_ota_server_ctx.mesh_ota_sem, 0);
    if (ret) {
        LOGE(TAG, "Create mesh ota sem failed %d", ret);
        return ret;
    }

    ret = aos_mutex_new(&g_mesh_ota_server_ctx.enter_ota_mutex);
    if (ret) {
        LOGE(TAG, "Create mesh ota mutex failed %d", ret);
        return ret;
    }

    retry_timer_start(10);
    g_mesh_ota_server_ctx.status    = MESH_OTA_SERVER_STATE_GET_VERSION;
    g_mesh_ota_server_ctx.init_flag = 1;
    return 0;
}

int mesh_ota_server_ota_stop()
{
    int ret = 0;

    uint16_t cancel_num = slist_entry_number(&g_mesh_ota_server_ctx.fir_info.firmware->cancel_list_head);
    if (cancel_num == 0) {
        LOGW(TAG, "Cancel num is null");
        return 0;
    } else {
        LOGD(TAG, "Cancel num is %d", cancel_num);
    }

    uint16_t *cancel_addr = aos_zalloc(cancel_num * 2);
    if (!cancel_addr) {
        LOGE(TAG, "Cancel addr malloc failed");
        return -1;
    }

    upgrade_device *device_temp = NULL;
    slist_t *       head_list   = &g_mesh_ota_server_ctx.fir_info.firmware->cancel_list_head;
    for (int i = 0; i < cancel_num; i++) {
        device_temp = dev_list_dev_get(head_list);
        if (!device_temp) {
            return -1;
        }
        device_temp->failed_reason = OTA_FAIL_CANCEL;
        cancel_addr[i]            = device_temp->device.unicast_addr;
        head_list                 = head_list->next;
    }
    ret = ble_mesh_blob_cli_blob_trans_cancel(CONFIG_BT_MESH_OTA_BLOB_CLI_ELEM_ID,
                                              g_mesh_ota_server_ctx.blob_info.blob_id, cancel_num, cancel_addr);
    if (ret) {
        LOGE(TAG, "Blob trans cancel failed %d", ret);
    }

    aos_free(cancel_addr);
    return ret;
}

int mesh_ota_server_reset()
{
    int ret = 0;
    LOGD(TAG, "Mesh ota server reset");
    if (!g_mesh_ota_server_ctx.init_flag) {
        return 0;
    }
    ret = ble_mesh_blob_cli_blob_reset(CONFIG_BT_MESH_OTA_BLOB_CLI_ELEM_ID);
    if (ret) {
        LOGE(TAG, "Blob cli reset %d failed", ret);
    }
    aos_sem_free(&g_mesh_ota_server_ctx.mesh_ota_sem);
    aos_mutex_free(&g_mesh_ota_server_ctx.enter_ota_mutex);
    ota_timer_free(&g_mesh_ota_server_ctx.retry_timer);
    ble_mesh_model_cb_unregister(&g_model_cb);
    memset(&g_mesh_ota_server_ctx, 0x00, sizeof(mesh_ota_server_ctx));
    g_mesh_ota_server_ctx.init_flag = 0;
    return 0;
}
#endif
