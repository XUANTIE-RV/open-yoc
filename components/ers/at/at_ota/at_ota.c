/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <ctype.h>
#include <stdio.h>
#include <errno.h>

#include <aos/kernel.h>
#include <dfu_port.h>
#include "ulog/ulog.h"

#ifdef CONFIG_BT_MESH_PROVISIONER
#include "provisioner_main.h"
#endif

#include <yoc/atserver.h>
#include "../at_internal.h"

#include "at_ota.h"
#include "at_ota_internal.h"

#include "ota_module.h"
#include "ota_server.h"

#define TAG                 "AT_OTA"
#define ERR_CHAR            0XFF
#define ADDR_MAC_SZIE       30
#define MAX_MAC_FILTER_SIZE 6

static char dev_addr_str[ADDR_MAC_SZIE * MAX_MAC_FILTER_SIZE] = { 0 };

static at_ota_ctx_t at_ota = { 0 };

static void _at_ota_dev_event_cb(ota_device_state_en event_id, void *param);

static struct ota_server_cb at_ota_cb = {
    .device_event_cb = _at_ota_dev_event_cb,
};

static uint8_t char2u8(char c)
{
    if (c >= '0' && c <= '9') {
        return (c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 10);
    } else {
        return ERR_CHAR;
    }
}

static inline int addr_val_to_str(mac_t addr, char *str, size_t len)
{
    return snprintf(str, len, "%02X:%02X:%02X:%02X:%02X:%02X,%X", addr.val[5], addr.val[4], addr.val[3], addr.val[2],
                    addr.val[1], addr.val[0], addr.type);
}

static int str2bt_dev_addr(const char *str, mac_t *dev)
{
    uint8_t i, j;
    uint8_t tmp;

    if (strlen(str) != 19 || !dev) {
        return -EINVAL;
    }

    for (i = 5, j = 1; j <= 17; str++, j++) {
        if (!(j % 3) && (*str != ':')) {
            return -EINVAL;
        } else if (*str == ':') {
            i--;
            continue;
        }

        dev->val[i] = dev->val[i] << 4;

        tmp = char2u8(*str);

        if (tmp == ERR_CHAR) {
            return -EINVAL;
        }

        dev->val[i] |= tmp;
    }

    str++;

    dev->type = char2u8(*str);

    if (dev->type != 0 && dev->type != 1) {
        return -EINVAL;
    }

    return 0;
}

static int grep_mac_addr_from_str(char *mac_str, device_info *devices, uint8_t size)
{
    int     ret              = 0;
    uint8_t dev_index        = 0;
    char    mac_str_temp[20] = { 0x0 };

    if (!mac_str) {
        return -1;
    }

    for (dev_index = 0; dev_index < size && *mac_str != '\0'; dev_index++) {
        memcpy(mac_str_temp, mac_str, 19);
        ret = str2bt_dev_addr(mac_str_temp, &devices[dev_index].addr);

        if (ret) {
            return -1;
        }

        mac_str += 20;
    }

    if (dev_index != size) {
        return -1;
    } else {
        return 0;
    }
}

static int grep_unicast_addr_from_str(char *unicast_str, device_info *devices, uint8_t size)
{
    uint8_t dev_index           = 0;
    char    unicast_str_temp[7] = { 0x0 };
    uint8_t temp_length         = 0;
    char *  temp_head           = NULL;

    for (dev_index = 0; dev_index < size; dev_index++) {
        temp_head   = unicast_str;
        temp_length = 0;

        while (*unicast_str != '\0' && *unicast_str != ',') {
            unicast_str++;
            temp_length++;
        }

        if (*unicast_str == ',') {
            unicast_str++;
        }

        if (temp_length == 0) {
            return dev_index;
        } else if (temp_length > 6) {
            return -EINVAL;
        }

        memcpy(unicast_str_temp, temp_head, temp_length);
        devices[dev_index].unicast_addr = strtoul(temp_head, NULL, 16);
#ifdef CONFIG_BT_MESH_PROVISIONER
        struct bt_mesh_node_t *node = bt_mesh_provisioner_get_node_info(devices[dev_index].unicast_addr);

        if (!node) {
            LOGE(TAG, "Not found the node %04x in the provisioner", devices[dev_index].unicast_addr);
            return -EINVAL;
        } else {
            memcpy(devices[dev_index].addr.val, node->addr_val, 6);
            devices[dev_index].addr.type   = node->addr_type;
            devices[dev_index].old_version = node->version;
        }

#endif
    }

    return dev_index;
}

void at_cmd_add_ota_fw(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int     ret          = 0;
        int32_t ota_target   = 0;
        int32_t ota_protocol = 0;
        int32_t ota_channel  = 0;
        int16_t input_num    = 0;
        ret                  = ota_server_init();

        if (ret && ret != -EALREADY) {
            LOGE(TAG, "Ota server init failed %d", ret);
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        input_num = atserver_scanf("%x,%x,%x", &ota_target, &ota_protocol, &ota_channel);

        if (ota_target < AT_OTA_TARGET_LOCAL || ota_target >= AT_OTA_TARGET_MAX) {
            LOGE(TAG, "Unsupport target %02x", ota_target);
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        if (ota_protocol < OTA_PROTOCOL_AIS || ota_protocol >= OTA_PROTOCOL_MAX) {
            LOGE(TAG, "Unsupport protocol %02x", ota_target);
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        if (ota_channel < OTA_CHANNEL_ON_GATT || ota_channel >= OTA_CHANNEL_MAX) {
            LOGE(TAG, "Unsupport channel %02x", ota_target);
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        if (ota_target == AT_OTA_TARGET_REMOTE) {
            if (input_num < 2) {
                ota_protocol = OTA_PROTOCOL_AIS;
                ota_channel  = OTA_CHANNEL_ON_GATT;
            } else if (input_num < 3) {
                if (ota_protocol == OTA_PROTOCOL_AIS) {
                    ota_channel = OTA_CHANNEL_ON_GATT;
                } else if (ota_protocol == OTA_PROTOCOL_MESH) {
                    ota_channel = OTA_CHANNEL_ON_MESH_EXT_2M;
                } else {
                    LOGE(TAG, "Unsupport protocol %02x", ota_protocol);
                    AT_BACK_ERRNO(AT_ERR_INVAL);
                    return;
                }
            } else {
                if ((ota_protocol == OTA_PROTOCOL_AIS
                     && (ota_channel == OTA_CHANNEL_ON_MESH_EXT_1M || ota_channel == OTA_CHANNEL_ON_MESH_EXT_2M))
                    || (ota_protocol == OTA_PROTOCOL_MESH
                        && (ota_channel == OTA_CHANNEL_ON_GATT || ota_channel == OTA_CHANNEL_ON_UART
                            || ota_channel == OTA_CHANNEL_ON_HCI_UART)))
                {
                    LOGE(TAG, "Unsupport ota channel %02x for protocol %02x", ota_channel, ota_protocol);
                    AT_BACK_ERRNO(AT_ERR_INVAL);
                    return;
                }
            }
        }

        ret = at_ota_start(ota_target, ota_protocol, ota_channel);

        if (ret < 0) {
            AT_BACK_ERR();
            return;
        }

        AT_BACK_OK();
    } else {
        AT_BACK_RET_OK(cmd, "<ota_target>,[protocol],[channel]");
    }
}

void at_cmd_get_ota_fw_index(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        at_ota_file_t *ota_file;

        if (!data) {
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        ota_file = at_ota_file_find_by_file_name(data);

        if (NULL == ota_file) {
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        } else {
            AT_BACK_OK();
            atserver_send("\r\n+ADDOTAFW:%d\r\n", ota_file->ota_idx);
        }
    } else {
        AT_BACK_RET_OK(cmd, "<firmware_name>");
    }
}

void at_cmd_rm_ota_fw(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int32_t index;
        int16_t input_num;
        int16_t ret;

        input_num = atserver_scanf("%x", &index);

        if (input_num < 1) {
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        ret = ota_server_upgrade_firmware_rm(index);

        if (ret < 0) {
            AT_BACK_ERRNO(ret);
            return;
        } else {
            AT_BACK_OK();
        }
    } else {
        AT_BACK_RET_OK(cmd, "<index>");
    }
}

void at_cmd_add_ota_node(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int32_t      size;
        int32_t      index;
        int16_t      input_num;
        device_info *device = NULL;
        int16_t      ret;
        input_num = atserver_scanf("%x,%x,%[^\n]", &index, &size, dev_addr_str);

        if (input_num < 3) {
            LOGE(TAG, "Err input num %d", input_num);
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        if (size > MAX_MAC_FILTER_SIZE || !size || size < 0) {
            LOGE(TAG, "Err input arg");
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        if (strlen(dev_addr_str) != (19 * size + size - 1)) {
            LOGE(TAG, "Err input addr str");
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        device = (device_info *)aos_zalloc(size * sizeof(device_info));

        if (!device) {
            LOGE(TAG, "Malloc devices failed");
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        ret = grep_mac_addr_from_str(dev_addr_str, device, size);

        if (ret < 0) {
            LOGE(TAG, "Grep addr failed");
            aos_free(device);
            AT_BACK_ERRNO(ret);
            return;
        }

        ret = ota_server_upgrade_device_add(index, size, device);

        if (ret < 0) {
            LOGE(TAG, "Add ota device failed %d", ret);
            aos_free(device);
            AT_BACK_ERRNO(ret);
            return;
        } else {
            AT_BACK_OK();
        }
    } else {
        AT_BACK_RET_OK(cmd, "<index>,<size>,<mac>,<type>...");
    }
}

void at_cmd_rm_ota_node(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int32_t      size;
        int32_t      index;
        int16_t      input_num;
        device_info *device = NULL;
        int16_t      ret;

        input_num = atserver_scanf("%x,%x,%[^\n]", &index, &size, dev_addr_str);

        if (input_num < 3) {
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        if (size > MAX_MAC_FILTER_SIZE || !size || size < 0) {
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        if (strlen(dev_addr_str) != (19 * size + size - 1)) {
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        device = (device_info *)aos_zalloc(size * sizeof(device_info));

        if (!device) {
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        ret = grep_mac_addr_from_str(dev_addr_str, device, size);

        if (ret < 0) {
            aos_free(device);
            AT_BACK_ERRNO(ret);
            return;
        }

        ret = ota_server_upgrade_device_rm(index, size, device);

        if (ret < 0) {
            aos_free(device);
            AT_BACK_ERRNO(ret);
            return;
        } else {
            AT_BACK_OK();
        }
    } else {
        AT_BACK_RET_OK(cmd, "<index>,<size>,<mac>,<type>...");
    }
}

void at_cmd_start_ota(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int           err = 0;
        int32_t       index;
        int16_t       input_num;
        ota_firmware *firmware = NULL;

        input_num = atserver_scanf("%x", &index);

        if (input_num < 1) {
            AT_BACK_ERRNO(AT_ERR_INVAL);
        }

        firmware = ota_server_upgrade_firmware_get(index);

        if (!firmware) {
            LOGE(TAG, "No firmware found for index %d", index);
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        err = ota_server_upgrade_start(index);

        if (err) {
            LOGE(TAG, "Ota dev start failed %d", err);
            AT_BACK_ERRNO(err);
        } else {
            LOGD(TAG, "Ota dev start success for image %d ", index);
            AT_BACK_OK();
        }

        return;
    } else {
        AT_BACK_RET_OK(cmd, "<index>");
    }
}

void at_cmd_add_ota_node_by_unicast_addr(char *cmd, int type, char *data)
{

    if (type == WRITE_CMD) {
        int32_t       size;
        int32_t       index;
        int16_t       input_num;
        device_info * device   = NULL;
        ota_firmware *firmware = NULL;
        int16_t       ret;
        int           set_size = 0;
        input_num              = atserver_scanf("%x,%x,%[^\n]", &index, &size, dev_addr_str);

        if (input_num < 3 || input_num != (size + 2)) {
            LOGE(TAG, "Err input %d", input_num);
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        device = (device_info *)aos_zalloc(size * sizeof(device_info));

        if (!device) {
            LOGE(TAG, "Malloc devices failed");
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        set_size = grep_unicast_addr_from_str(dev_addr_str, device, size);

        if (set_size < 0) {
            LOGE(TAG, "Grep device unicast addr failed");
            aos_free(device);
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        firmware = ota_server_upgrade_firmware_get(index);

        if (!firmware) {
            LOGE(TAG, "No firmware found for index %d", index);
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        ret = ota_server_upgrade_device_add(index, (set_size < size ? set_size : size), device);

        if (ret < 0) {
            LOGE(TAG, "Add ota device failed %d", ret);
            aos_free(device);
            AT_BACK_ERRNO(ret);
            return;
        } else {
            AT_BACK_OK();
        }
    } else {
        AT_BACK_RET_OK(cmd, "<index>,<size>,<unicast_addr>...");
    }
}

void at_cmd_rm_ota_node_by_unicast_addr(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int32_t       size;
        int32_t       index;
        int16_t       input_num;
        device_info * device   = NULL;
        ota_firmware *firmware = NULL;
        int16_t       ret;
        int           set_size = 0;
        input_num              = atserver_scanf("%x,%x,%[^\n]", &index, &size, dev_addr_str);

        if (input_num < 3 || input_num != (size + 2)) {
            LOGE(TAG, "Err input %d", input_num);
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        device = (device_info *)aos_zalloc(size * sizeof(device_info));

        if (!device) {
            LOGE(TAG, "Malloc devices failed");
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        set_size = grep_unicast_addr_from_str(dev_addr_str, device, size);

        if (set_size < 0) {
            LOGE(TAG, "Grep device unicast addr failed");
            aos_free(device);
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        firmware = ota_server_upgrade_firmware_get(index);

        if (!firmware) {
            LOGE(TAG, "No firmware found for index %d", index);
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        ret = ota_server_upgrade_device_rm(index, (set_size < size ? set_size : size), device);

        if (ret < 0) {
            LOGE(TAG, "rm ota device failed %d", ret);
            aos_free(device);
            AT_BACK_ERRNO(ret);
            return;
        } else {
            AT_BACK_OK();
        }
    } else {
        AT_BACK_RET_OK(cmd, "<index>,<size>,<unicast_addr>...");
    }
}

void at_ota_report_firmware_index(int index)
{
    aos_msleep(200);
    atserver_send("\r\n+ADDOTAFW:%d\r\n", index);
}

void at_ota_report_ota_status(uint16_t index, uint8_t status, mac_t addr, char *version)
{
    char addr_str[40] = { 0 };
    addr_val_to_str(addr, addr_str, sizeof(addr_str));
    atserver_send("\r\n+DEVOTA:%02x,%02x,%s,%s\r\n", index, status, addr_str, version);
}

void at_ota_report_fw_status(uint8_t index, uint8_t status)
{
    atserver_send("\r\n+FW:%02x,%02x\r\n", index, status);
}

static void _at_ota_dev_event_cb(ota_device_state_en event_id, void *param)
{
    char           version_str[20] = { 0 };
    at_ota_file_t *ota_file        = NULL;
    uint8_t        firmware_id     = *(uint8_t *)param;

    ota_file = at_ota_file_find_by_idx(firmware_id);

    if (NULL == ota_file) {
        return;
    }

    switch (event_id) {
        case OTA_STATE_ONGOING: {
            ota_state_ongoing *status = (ota_state_ongoing *)param;
            LOGI(TAG, "ota ready for firmware:%d dev: %02x:%02x:%02x:%02x:%02x:%02x type: %d by channel :%02x\r\n",
                 status->firmware_index, status->dev_info.addr.val[5], status->dev_info.addr.val[4],
                 status->dev_info.addr.val[3], status->dev_info.addr.val[2], status->dev_info.addr.val[1],
                 status->dev_info.addr.val[0], status->dev_info.addr.type, status->channel);
        } break;

        case OTA_STATE_SUCCESS: {
            ota_state_success *status = (ota_state_success *)param;
            LOGI(TAG,
                 "ota success for firmware:%d dev: %02x:%02x:%02x:%02x:%02x:%02x type: %d old_version:0x%04x "
                 "new_verison:0x%04x cost %d ms by channel :%02x\r\n\r\n",
                 status->firmware_index, status->dev_info.addr.val[5], status->dev_info.addr.val[4],
                 status->dev_info.addr.val[3], status->dev_info.addr.val[2], status->dev_info.addr.val[1],
                 status->dev_info.addr.val[0], status->dev_info.addr.type, status->old_ver, status->new_ver,
                 status->cost_time, status->channel);
            get_version_str(status->new_ver, version_str);
            at_ota_report_ota_status(status->firmware_index, OTA_STATE_SUCCESS, status->dev_info.addr, version_str);
#if (defined(CONFIG_BT_MESH) && CONFIG_BT_MESH > 0)                                                                    \
    && (defined(CONFIG_BT_MESH_PROVISIONER) && CONFIG_BT_MESH_PROVISIONER > 0)
            struct bt_mesh_node_t *node = NULL;
            dev_addr_t             addr = { 0 };
            memcpy(addr.val, status->dev_info.addr.val, 6);
            node = bt_mesh_provisioner_get_node_info_by_mac(addr);

            if (!node) {
                LOGE(TAG, "Get node by mac failed");
            } else {
                if (node->version != status->new_ver) {
                    provisioner_node_version_set(bt_mesh_provisioner_get_node_id(node), status->new_ver);
                }
            }

#endif
        } break;

        case OTA_STATE_FAILED: {
            ota_state_fail *status = (ota_state_fail *)param;
            LOGE(
                TAG,
                "ota fail for firmware:%d dev: %02x:%02x:%02x:%02x:%02x:%02x type: %d reason:%02x by channel :%02x\r\n",
                status->firmware_index, status->dev_info.addr.val[5], status->dev_info.addr.val[4],
                status->dev_info.addr.val[3], status->dev_info.addr.val[2], status->dev_info.addr.val[1],
                status->dev_info.addr.val[0], status->dev_info.addr.type, status->reason, status->channel);
            get_version_str(status->new_ver, version_str);
            at_ota_report_ota_status(status->firmware_index, OTA_STATE_FAILED, status->dev_info.addr, version_str);
#if defined(CONFIG_BT_MESH) && CONFIG_BT_MESH > 0
            struct bt_mesh_node_t *node = NULL;
            dev_addr_t             addr = { 0 };
            memcpy(addr.val, status->dev_info.addr.val, 6);
            node = bt_mesh_provisioner_get_node_info_by_mac(addr);

            if (!node) {
                LOGE(TAG, "no node find");
                return;
            }

            if (status->reason == OTA_FAIL_INVAILD_VERSION && node->version != status->old_ver) {
                provisioner_node_version_set(bt_mesh_provisioner_get_node_id(node), status->old_ver);
            }

#endif
        } break;

        default:
            break;
    }
}

int at_ota_prepare(at_ota_ctx_t *ctx)
{
    if (!ctx) {
        AT_BACK_ERRNO(AT_ERR_INVAL);
        return -1;
    }

    atserver_stop();

    if (ctx->ops->prepare) {
        return ctx->ops->prepare();
    }

    return 0;
}

static int at_ota_upgrade(at_ota_ctx_t *ctx, at_ota_target_en target, void *file, uint32_t file_size)
{
    if (ctx && ctx->ops->upgrade) {
        return ctx->ops->upgrade(target, file, file_size);
    }

    return AT_OTA_UPGRAD_SKIP;
}

int at_ota_finish(at_ota_ctx_t *ctx, int result)
{
    int ret;

    atserver_resume();

    if (result) {
        AT_BACK_ERRNO(AT_ERR);
        return -1;
    }

    if (ctx->cur_file && ctx->cur_file->complete) {
        ret = at_ota_upgrade(ctx, ctx->cur_file->ota_target, ctx->cur_file->file, ctx->cur_file->file_size);

        if (ret == AT_OTA_UPGRAD_SKIP) {
            /* at_ota not support upgrade local device */
            if (ctx->cur_file->ota_target == AT_OTA_TARGET_LOCAL) {
                return -1;
            }

            return at_ota_start_upgrade(ctx->cur_file);
        } else if (ret == AT_OTA_UPGRADED) {
            at_ota_del_file(ctx->cur_file);
            ctx->cur_file = NULL;
            return 0;
        }

        return ret;
    }

    if (at_ota.ops->finish) {
        return at_ota.ops->finish(result);
    }

    return 0;
}

at_ota_file_t *at_ota_new_file(int ota_target, int ota_protocol, int ota_channel)
{
    int i;

    for (i = 0; i < CONFIG_AT_OTA_MAX_FILE_NUM; i++) {
        if (0 == at_ota.file[i].used) {
            at_ota.file[i].used         = 1;
            at_ota.file[i].complete     = 0;
            at_ota.file[i].ota_target   = ota_target;
            at_ota.file[i].ota_protocol = ota_protocol;
            at_ota.file[i].ota_channel  = ota_channel;
            return &at_ota.file[i];
        }
    }

    return NULL;
}

void at_ota_del_file(at_ota_file_t *file)
{
    if (file && file->file) {
        free(file->file);
        memset(file, 0, sizeof(at_ota_file_t));
    }

    return;
}

at_ota_file_t *at_ota_file_find_by_idx(int ota_idx)
{
    int i;

    for (i = 0; i < CONFIG_AT_OTA_MAX_FILE_NUM; i++) {
        if (at_ota.file[i].used && at_ota.file[i].ota_idx == ota_idx) {
            return &at_ota.file[i];
        }
    }

    return NULL;
}

at_ota_file_t *at_ota_file_find_by_file_name(const char *file_name)
{
    int i;

    for (i = 0; i < CONFIG_AT_OTA_MAX_FILE_NUM; i++) {
        if (at_ota.file[i].used && !strcmp(at_ota.file[i].file_name, file_name)) {
            return &at_ota.file[i];
        }
    }

    return NULL;
}

static void _at_ota_firmware_event_cb(ota_firmware_state_en event, void *data)
{
    at_ota_file_t *          ota_file;
    ota_firmware_state_data *event_data = (ota_firmware_state_data *)data;

    ota_file = at_ota_file_find_by_idx(event_data->firmware_index);

    if (NULL == ota_file) {
        return;
    }

    switch (event) {
        case FIRMWARE_STATE_IDLE: {
            /* do nothing */
        } break;
        case FIRMWARE_STATE_IN_USE: {
            LOGD(TAG, "OTA firmware index :%02x in use", event_data->firmware_index);
            at_ota_report_fw_status(event_data->firmware_index, FIRMWARE_STATE_IN_USE);
        } break;

        case FIRMWARE_STATE_END_USE: {
            LOGD(TAG, "OTA firmware index :%02x end use", event_data->firmware_index);
            at_ota_report_fw_status(event_data->firmware_index, FIRMWARE_STATE_END_USE);
        } break;

        case FIRMWARE_STATE_REMOVED: {
            LOGD(TAG, "OTA firmware index :%02x removed", event_data->firmware_index);
            at_ota_report_fw_status(event_data->firmware_index, FIRMWARE_STATE_REMOVED);
            at_ota_del_file(ota_file);
        } break;
    }
}

int at_ota_start_upgrade(at_ota_file_t *file)
{
    int           ota_idx;
    firmware_info info;

    info.address    = (uint8_t *)file->file;
    info.image_type = 0x00;
    info.image_pos  = IMAGE_POS_RAM;
    info.size       = dfu_get_fota_file_size(info.address, info.image_pos);
    info.version    = dfu_get_fota_file_app_version(info.address, info.image_pos);
    info.ota_chanel = file->ota_channel;
    info.protocol   = file->ota_protocol;
    info.cb         = _at_ota_firmware_event_cb;

    if (info.size < 0) {
        LOGE(TAG, "read ota file fail\r\n");
        return -1;
    }

    LOGD(TAG, "ota file size %d\r\n", info.size);

    ota_idx = ota_server_upgrade_firmware_add(info);

    if (ota_idx < 0) {
        LOGE(TAG, "Add OTA file failed");
        return -1;
    }

    file->ota_idx = ota_idx;

    atserver_resume();

    at_ota_report_firmware_index(ota_idx);

    return 0;
}

int at_ota_start(int ota_target, int ota_protocol, int ota_channel)
{
    if (0 == at_ota.init) {
        return AT_OTA_ERR;
    }

    if (at_ota.cur_file) {
        return AT_OTA_ERR_RUNING;
    }

    at_ota.cur_file = at_ota_new_file(ota_target, ota_protocol, ota_channel);

    if (NULL == at_ota.cur_file) {
        return AT_OTA_ERR_NULL;
    }

    return at_ota_ymodel_init(&at_ota);
}

int at_ota_init(at_ota_config_t config)
{
    at_ota.ymodem_port = config.at_ota_port;
    at_ota.ops         = config.ops;

    at_ota.init = 1;

    ota_server_cb_register(&at_ota_cb);

    return 0;
}