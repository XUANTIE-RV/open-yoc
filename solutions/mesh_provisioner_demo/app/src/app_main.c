/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/cli.h"
#include "common/log.h"
#include "app_mesh_provisioner.h"
#include "app_init.h"
#include <gateway.h>

#define TAG "app"
#define ERR_CHAR 0XFF
#define GW_DEV_DISCOVER_NO_TIMEOUT 0xFFFFFFFF
#define HELP_PROVISIONER_INFO "Usage: provisioner scan\n \
      provisioner add\n \
      provisioner del\n \
      provisioner onoff"

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

static int str2bt_mac(const char *str, uint8_t mac[6])
{
    uint8_t i, j;
    uint8_t tmp;

    if (strlen(str) != 17 || !mac) {
        LOGE(TAG, "Invalid argument");
        return -22;
    }

    for (i = 5, j = 1; *str != '\0'; str++, j++) {
        if (!(j % 3) && (*str != ':')) {
            LOGE(TAG, "Invalid argument");
            return -22;
        } else if (*str == ':') {
            i--;
            continue;
        }

        mac[i] = mac[i] << 4;

        tmp = char2u8(*str);

        if (tmp == ERR_CHAR) {
            LOGE(TAG, "Invalid argument");
            return -22;
        }

        mac[i] |= tmp;
    }

    return 0;
}
static int str2hex(uint8_t hex[], char *s, uint8_t cnt)
{
    uint8_t i;

    if (!s) {
        return 0;
    }

    for (i = 0; (*s != '\0') && (i < cnt); i++, s += 2) {
        hex[i] = ((char2u8(*s) & 0x0f) << 4) | ((char2u8(*(s + 1))) & 0x0f);
    }

    return i;
}

static void cmd_provisioner_test_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ret = 0;

    if (argc < 3) {
        printf("%s\n", HELP_PROVISIONER_INFO);
        return;
    }

    if (strcmp(argv[1], "scan") == 0){
        if (argc < 3){
            LOGE("","provisioner scan [1/0] [timeout]");
            return;
        }
        uint32_t timeout = GW_DEV_DISCOVER_NO_TIMEOUT;
        uint8_t enable = atoi(argv[2]);
        if (argc >= 4){
            timeout = strtoul(argv[3], NULL, 16);
        }
        if (enable) {
            gateway_discover_start(timeout);
        } else {
                gateway_discover_stop();
            }
    } else if (strcmp(argv[1], "add") == 0){
        gw_discovered_dev_t subdev = { 0x00 };
        if (argc < 7){
            LOGE("","provisioner add [dev_addr] [addr_type] [uuid] [oob_info] [bearer]");
            LOGE("","dev_addr = xx:xx:xx:xx:xx:xx");
            return;
        }
        subdev.protocol_info.ble_mesh_info.bearer    = atoi(argv[6]);
        subdev.protocol_info.ble_mesh_info.oob_info  = atoi(argv[5]);
        subdev.protocol_info.ble_mesh_info.addr_type = atoi(argv[3]);
        if (subdev.protocol_info.ble_mesh_info.addr_type != 0x1 && subdev.protocol_info.ble_mesh_info.addr_type != 0x0) {
            LOGE(TAG, "Err addr type %02x", subdev.protocol_info.ble_mesh_info.addr_type);
        return;
        }
        if (subdev.protocol_info.ble_mesh_info.bearer != 0x01 && subdev.protocol_info.ble_mesh_info.bearer != 0x02) {
            LOGE(TAG, "Err bearer type %02x", subdev.protocol_info.ble_mesh_info.bearer);
        return;
        }
        ret = str2bt_mac((char *)argv[2], subdev.protocol_info.ble_mesh_info.dev_addr);
        if (ret) {
            LOGE(TAG, "Err addr data");
            return;
        }
        if (str2hex(subdev.protocol_info.ble_mesh_info.uuid, (char *)argv[4], sizeof(subdev.protocol_info.ble_mesh_info.uuid)) < 1) {
            LOGE(TAG, "Err uuid data");
            return;
        }
        printf("%02x:%02x:%02x:%02x:%02x:%02x %02x %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x %02x %02x\n",                          \
                subdev.protocol_info.ble_mesh_info.dev_addr[5], subdev.protocol_info.ble_mesh_info.dev_addr[4], subdev.protocol_info.ble_mesh_info.dev_addr[3], \
                subdev.protocol_info.ble_mesh_info.dev_addr[2], subdev.protocol_info.ble_mesh_info.dev_addr[1], subdev.protocol_info.ble_mesh_info.dev_addr[0], \
                subdev.protocol_info.ble_mesh_info.addr_type, subdev.protocol_info.ble_mesh_info.uuid[0], subdev.protocol_info.ble_mesh_info.uuid[1],           \
                subdev.protocol_info.ble_mesh_info.uuid[2], subdev.protocol_info.ble_mesh_info.uuid[3], subdev.protocol_info.ble_mesh_info.uuid[4],             \
                subdev.protocol_info.ble_mesh_info.uuid[5], subdev.protocol_info.ble_mesh_info.uuid[6], subdev.protocol_info.ble_mesh_info.uuid[7],             \
                subdev.protocol_info.ble_mesh_info.uuid[8], subdev.protocol_info.ble_mesh_info.uuid[9], subdev.protocol_info.ble_mesh_info.uuid[10],            \
                subdev.protocol_info.ble_mesh_info.uuid[11], subdev.protocol_info.ble_mesh_info.uuid[12], subdev.protocol_info.ble_mesh_info.uuid[13],          \
                subdev.protocol_info.ble_mesh_info.uuid[14], subdev.protocol_info.ble_mesh_info.uuid[15],subdev.protocol_info.ble_mesh_info.oob_info,           \
                subdev.protocol_info.ble_mesh_info.bearer);
        ret = gateway_subdev_add(subdev);
        if (ret) {
            LOGE(TAG, "Add sub dev failed %d", ret);
            return;
        }
    } else if (strcmp(argv[1], "del") == 0){
        if (argc < 3){
            LOGE("","provisioner del [dev_id]");
        }
        gw_subdev_t dev_dev = (gw_subdev_t)atoi(argv[2]);
        gateway_subdev_del(dev_dev);
    } else if (strcmp(argv[1],"onoff") == 0){
        if (argc < 4){
            LOGE("","provisioner onoff [dev_id] [0/1]");
        }
        uint16_t index = (uint16_t)atoi(argv[2]);
        uint8_t  onoff = atoi(argv[3]);
        gw_subdev_t subdev = (gw_subdev_t)index;
        ret = gateway_subdev_set_onoff(subdev, onoff);
        if (ret)  {
            LOGD(TAG, "node onoff Failed %d", ret);
            return;
        }
    }
}

void cli_reg_cmd_provisioner(void)
{
    static const struct cli_command cmd_info = {
        "provisioner",
        "provisioner cmd for testing",
        cmd_provisioner_test_func,
    };

    aos_cli_register_command(&cmd_info);
}

int main()
{

    board_yoc_init();

    LOGD(TAG, "%s\n", aos_get_app_version());

    appmesh_provisioner_init();
 
    cli_reg_cmd_provisioner();

    return 0;
}
