/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include <aos/debug.h>
#include "jsapi_publish.h"

#define TAG "miniapp_debug"
#define ERR_CHAR 0XFF

extern void start_jquick_debugger();
extern void jquick_set_screen_on(int ON);

static int battery = 20;
static void cmd_miniapp_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
        return;
    }

    if (strcmp(argv[1], "debug") == 0) {
        start_jquick_debugger();
    } else if (strcmp(argv[1], "touch") == 0) {
        if (argc == 3) {
            jquick_set_screen_on(atoi(argv[2]));
        }
    } else if (strcmp(argv[1], "power") == 0) {
        if (argc == 3) {
            if (strcmp(argv[2], "change") == 0) {
                jsapi_power_publish_batteryChange(battery);
            }
            if (strcmp(argv[2], "charge") == 0) {
                jsapi_power_publish_charge(0);
            }
            if (strcmp(argv[2], "low") == 0) {
                battery = 20;
                jsapi_power_publish_batteryLow(battery);
            }
            if (strcmp(argv[2], "emergency") == 0) {
                battery = 5;
                jsapi_power_publish_batteryEmergency(battery);
            }
        }
    }
    battery++;
}

void cli_reg_cmd_miniapp(void)
{
    static const struct cli_command cmd_info = { "miniapp", "app extend command.", cmd_miniapp_func };

    aos_cli_register_command(&cmd_info);
}

#if defined(CONFIG_BT_MESH) && CONFIG_BT_MESH
#include <gateway.h>
uint8_t gw_subdev_traverse(gw_subdev_t subdev, void *data)
{
    gw_status_t   ret         = 0;
    subdev_info_t subdev_info = { 0 };

    ret = gateway_subdev_get_info(subdev, &subdev_info);
    if (ret == 0) {
        LOGD("", "subdev[%d] mac: %02x%02x%02x%02x%02x%02x name: %s", subdev, subdev_info.dev_addr[5], subdev_info.dev_addr[4],
             subdev_info.dev_addr[3], subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0], subdev_info.name);
    }

    return GW_SUBDEV_ITER_CONTINUE;
}

uint8_t gw_subgrp_subdev_foreach(gw_subgrp_t subgrp, gw_subdev_t subdev, void *data)
{
    gw_status_t   ret         = 0;
    subdev_info_t subdev_info = { 0 };

    ret = gateway_subdev_get_info(subdev, &subdev_info);
    if (ret == 0) {
        LOGD("", "subgrp[%d] ==== subdev[%d] ==== mac: %02x%02x%02x%02x%02x%02x name: %s",subgrp, subdev, subdev_info.dev_addr[5], subdev_info.dev_addr[4],
             subdev_info.dev_addr[3], subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0], subdev_info.name);
    }

    return GW_SUBDEV_ITER_CONTINUE;
}
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

static int str2bt_addr(const char *str, uint8_t addr[6])
{
    uint8_t i, j;
    uint8_t tmp;

    if (strlen(str) != 17 || !addr) {
        return -EINVAL;
    }

    for (i = 5, j = 1; *str != '\0'; str++, j++) {
        if (!(j % 3) && (*str != ':')) {
            return -EINVAL;
        } else if (*str == ':') {
            i--;
            continue;
        }

        addr[i] = addr[i] << 4;

        tmp = char2u8(*str);

        if (tmp == ERR_CHAR) {
            return -EINVAL;
        }

        addr[i] |= tmp;
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

static int _gateway_add_mesh_subdev(int argc, char **argv)
{
    if (argc < 5) {
        LOGE(TAG, "Err argc %d", argc);
        return -1;
    }

    int err = 0;

    uint8_t             addr_type = atoi(argv[1]);
    uint8_t             oob_info  = atoi(argv[3]);
    uint8_t             bearer    = atoi(argv[4]);
    gw_discovered_dev_t subdev    = { 0x00 };

    if (addr_type != 0x1 && addr_type != 0x0) {
        LOGE(TAG, "Err addr type %02x", addr_type);
        return -1;
    }

    if (bearer != 0x01 && bearer != 0x02) {
        LOGE(TAG, "Err bearer type %02x", bearer);
        return -1;
    }

    err = str2bt_addr((char *)argv[0], subdev.protocol_info.ble_mesh_info.dev_addr);
    if (err) {
        LOGE(TAG, "Err addr data");
        return -1;
    }

    if (str2hex(subdev.protocol_info.ble_mesh_info.uuid, (char *)argv[2], strlen(argv[2])) < 1) {
        LOGE(TAG, "Err uuid data");
        return -1;
    }
    subdev.protocol_info.ble_mesh_info.addr_type = addr_type;
    subdev.protocol_info.ble_mesh_info.oob_info  = oob_info;
    subdev.protocol_info.ble_mesh_info.bearer    = bearer;

    return gateway_subdev_add(subdev);
}

void cmd_gateway_test_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int err = 0;

    if (argc >= 2) {
        if (strcmp(argv[1], "sub_traverse") == 0) {
            // gw sub_traverse
            LOGD(TAG, "show dev");
            gateway_subdev_foreach(gw_subdev_traverse, NULL);
        } else if (strcmp(argv[1], "scan_dev") == 0) {
            if (argc < 3) {
                LOGE(TAG, "Err argc");
                return;
            }
            uint32_t timeout = GW_DEV_DISCOVER_NO_TIMEOUT;
            uint8_t  enable  = atoi(argv[2]);
            if (argc >= 4) {
                timeout = strtoul(argv[3], NULL, 16);
            }
            if (enable) {
                gateway_discover_start(timeout);
            } else {
                gateway_discover_stop();
            }
        } else if (strcmp(argv[1], "add_dev") == 0) {
            if (argc < 3) {
                LOGE(TAG, "Err argc");
                return;
            }
            uint8_t protocol_type = atoi(argv[2]);
            argc--;
            argv += 3;
            switch (protocol_type) {
                case GW_NODE_BLE_MESH: {
                    err = _gateway_add_mesh_subdev(argc, argv);
                    if (err) {
                        LOGE(TAG, "Add sub dev failed %d", err);
                        return;
                    }
                } break;
                default:
                    LOGE(TAG, "Unsupport dev protocol %02x", protocol_type);
                    return;
            }

        } else if (strcmp(argv[1], "del_dev") == 0) {
            if (argc < 3) {
                LOGE(TAG, "Err argc");
                return;
            }
            gw_subdev_t dev_dev = (gw_subdev_t)atoi(argv[2]);
            gateway_subdev_del(dev_dev);
        } else if (strcmp(argv[1], "subdev_onoff") == 0) {
            if (argc < 4) {
                LOGE(TAG, "Err argc");
                return;
            }
            uint16_t    index  = atoi(argv[2]);
            uint8_t     onoff  = atoi(argv[3]);
            gw_subdev_t subdev = (gw_subdev_t)index;
            err                = gateway_subdev_set_onoff(subdev, onoff);
            if (err) {
                LOGE(TAG, "Gw onoff set failed %d", err);
            }
        }else if (strcmp(argv[1], "set_name") == 0) {
            // gw set name 1 new_name
            if (argc < 4) {
                LOGE(TAG, "Err argc");
                return;
            }
            uint16_t    index  = atoi(argv[2]);
            gw_subdev_t subdev = (gw_subdev_t)index;
            err                = gateway_subdev_set_name(subdev, argv[3]);
            if (err) {
                LOGE(TAG, "set name failed %d", err);
            }
        }else if (strcmp(argv[1], "create_subgrp") == 0) {
            // gw create_subgrp grp_name
            if (argc < 3) {
                LOGE(TAG, "Err argc");
                return;
            }
            err                = gateway_subgrp_create(argv[2]);
            if (err) {
                LOGE(TAG, "gateway subgrp create failed %d", err);
            }
        }else if (strcmp(argv[1], "del_subgrp") == 0) {
            if (argc < 3) {
                LOGE(TAG, "Err argc");
                return;
            }
            uint16_t    index  = atoi(argv[2]);
            err                = gateway_subgrp_del(index);
            if (err) {
                LOGE(TAG, "gateway subgrp create failed %d", err);
            }
        }else if (strcmp(argv[1], "get_onoff") == 0) {
            if (argc < 3) {
                LOGE(TAG, "Err argc");
                return;
            }
            uint16_t    index  = atoi(argv[2]);
            err                = gateway_subdev_get_onoff(index);
            LOGD(TAG, "get_onoff++++++++++++%d",err);
        }else if (strcmp(argv[1], "subgrp_set_onoff") == 0) {
            if (argc < 3) {
                LOGE(TAG, "Err argc");
                return;
            }
            uint16_t    index  = atoi(argv[2]);
            uint8_t     onoff  = atoi(argv[3]);
            gw_subgrp_t subdev = (gw_subgrp_t)index;
            err                = gateway_subgrp_set_onoff(subdev, onoff);
        }else if (strcmp(argv[1], "subgrp_add_subdev") == 0) {
            if (argc < 3) {
                LOGE(TAG, "Err argc");
                return;
            }
            uint16_t    index1  = atoi(argv[2]);
            uint8_t     index2  = atoi(argv[3]);

            gw_subgrp_t grpIndex = (gw_subgrp_t)index1;
            gw_subdev_t subIndex = (gw_subdev_t)index2;
            err                = gateway_subgrp_add_subdev(grpIndex, subIndex);
        }else if (strcmp(argv[1], "subdev_set_brightness") == 0) {
            if (argc < 3) {
                LOGE(TAG, "Err argc");
                return;
            }
            uint16_t    index1  = atoi(argv[2]);
            int         value  = atoi(argv[3]);

            gw_subdev_t grpIndex = (gw_subdev_t)index1;
            err                = gateway_subdev_set_brightness(grpIndex, value);
        }else if (strcmp(argv[1], "subgrp_set_brightness") == 0) {
            if (argc < 3) {
                LOGE(TAG, "Err argc");
                return;
            }
            uint16_t    index1  = atoi(argv[2]);
            int         value  = atoi(argv[3]);

            gw_subgrp_t grpIndex = (gw_subgrp_t)index1;
            err                = gateway_subgrp_set_brightness(grpIndex, value);
        }
    }
}

void cli_reg_cmd_gateway(void)
{
    static const struct cli_command cmd_info = {
        "gw",
        "gw cmd for testing",
        cmd_gateway_test_func,
    };

    aos_cli_register_command(&cmd_info);
}
#endif
