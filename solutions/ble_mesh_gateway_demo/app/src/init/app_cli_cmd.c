/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <aos/kv.h>
#include <dut/hal/common.h>
#include <gateway.h>

#include "board.h"

#include "app_main.h"
#include "app_sys.h"
#include "app_fota.h"
#include "app_voice.h"
#include "../linkkit_gateway/app_gateway_ut.h"

#define TAG      "cli_cmd"
#define ERR_CHAR 0XFF

extern int                      wifi_prov_method;
extern gw_subdev_cloud_info_t * g_gateway_subdev_cloud_info;
extern gw_evt_discovered_info_t subdev;

extern void cli_reg_cmd_ping(void);
extern void cli_reg_cmd_ifconfig(void);
extern void cli_reg_cmd_kvtool(void);
extern void cli_reg_cmd_ps(void);
extern void cli_reg_cmd_sysinfo(void);
extern void cli_reg_cmd_free(void);
extern void cli_reg_cmd_ping(void);
extern void cli_reg_cmd_ifconfig(void);
extern void cli_reg_cmd_w800(void);
#ifdef CONFIG_BTSOOP
extern void cli_reg_cmd_btsnoop(void);
#endif
#ifdef CONFIG_CLI_CMD_DEBUG
extern void cli_reg_cmd_ble(void);
extern void cli_reg_cmd_blemesh(void);
extern void cli_reg_cmd_iperf(void);
#endif

//#define CONFIG_CLI_CMD_DEBUG

uint8_t gw_subdev_traverse(gw_subdev_t subdev, void *data)
{
    gw_status_t   ret         = 0;
    subdev_info_t subdev_info = { 0 };

    ret = gateway_subdev_get_info(subdev, &subdev_info);
    if (ret == 0) {
        LOGD("", "subdev[%d] mac %02x%02x%02x%02x%02x%02x", subdev, subdev_info.dev_addr[5], subdev_info.dev_addr[4],
             subdev_info.dev_addr[3], subdev_info.dev_addr[2], subdev_info.dev_addr[1], subdev_info.dev_addr[0]);
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
            LOGD(TAG, "show dev");
            gateway_subdev_foreach(gw_subdev_traverse, NULL);
        } else if (strcmp(argv[1], "sub_triple") == 0) {
            if (argc < 6) {
                LOGI("", "gw sub_triple [index] [dn] [pk] [ps]");
                return;
            }

            int i = atoi(argv[2]);
#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO
            strcpy(g_gateway_subdev_cloud_info->info[i].meta.device_name, argv[3]);
            strcpy(g_gateway_subdev_cloud_info->info[i].meta.product_key, argv[4]);
            strcpy(g_gateway_subdev_cloud_info->info[i].meta.product_secret, argv[5]);

            g_gateway_subdev_cloud_info->info[i].cloud_devid = GATEWAY_NODE_UNREG;
#endif
            LOGI(TAG, "[index] %d, [dn] %s, [pk] %s  [ps] %s\r\n", i, argv[3], argv[4], argv[5]);
        }
#if defined(CONFIG_GW_FOTA_EN) && (CONFIG_GW_FOTA_EN)
        else if (strcmp(argv[1], "fota_show") == 0)
        {
            app_fota_device_show();
        }
#endif
        else if (strcmp(argv[1], "wifi_mac") == 0)
        {
            uint8_t mac_addr[6] = { 0 };

            dut_hal_mac_get(mac_addr);
            LOGI(TAG, "mac addr %02x%02x%02x%02x%02x%02x\r\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
                 mac_addr[4], mac_addr[5]);
        } else if (strcmp(argv[1], "playvoice") == 0) {
            if (argc < 3) {
                LOGE(TAG, "ERR arg");
                return;
            }

            uint16_t index = strtoul(argv[2], NULL, 16);
            uint16_t voice = strtoul(argv[3], NULL, 16);
            app_voice_play(index, voice);
        } else if (strcmp(argv[1], "prov") == 0) {
#if defined(CONFIG_GW_NETWORK_SUPPORT) && CONFIG_GW_NETWORK_SUPPORT
            if (strcmp(argv[2], "0") == 0) {
                LOGD(TAG, "smart config");
                wifi_prov_method = WIFI_PROVISION_SL_SMARTCONFIG;
                aos_kv_setint("wprov_method", wifi_prov_method);
                app_sys_set_boot_reason(BOOT_REASON_WIFI_CONFIG);
                aos_reboot();
            } else if (strcmp(argv[2], "1") == 0) {
                LOGD(TAG, "dev ap");
                wifi_prov_method = WIFI_PROVISION_SL_DEV_AP;
                aos_kv_setint("wprov_method", wifi_prov_method);
                app_sys_set_boot_reason(BOOT_REASON_WIFI_CONFIG);
                aos_reboot();
            } else if (strcmp(argv[2], "2") == 0) {
                LOGD(TAG, "ble");
                wifi_prov_method = WIFI_PROVISION_SL_BLE;
                aos_kv_setint("wprov_method", wifi_prov_method);

                aos_kv_del("AUTH_AC_AS");
                aos_kv_del("AUTH_KEY_PAIRS");
                app_sys_set_boot_reason(BOOT_REASON_WIFI_CONFIG);
                aos_reboot();
            } else if (strcmp(argv[2], "3") == 0) {
                LOGD(TAG, "softap");
                wifi_prov_method = WIFI_PROVISION_SOFTAP;
                aos_kv_setint("wprov_method", wifi_prov_method);
                app_sys_set_boot_reason(BOOT_REASON_WIFI_CONFIG);
                aos_reboot();
            }
#else
            LOGE(TAG, "Unsupport wifi option");
            return;
#endif
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

void board_cli_init()
{
    int err = 0;

    err = aos_cli_init();
    if (err) {
        LOGE(TAG, "aos cli init failed %d", err);
        return;
    }

    cli_reg_cmd_ps();

    cli_reg_cmd_sysinfo();

    cli_reg_cmd_free();

    cli_reg_cmd_kvtool();

#if defined(CONFIG_BTSOOP) && CONFIG_BTSOOP
    cli_reg_cmd_btsnoop();
#endif

    cli_reg_cmd_ping();

    cli_reg_cmd_ifconfig();

    cli_reg_cmd_gateway();

#if defined(CONFIG_BOARD_RF_CMD) && CONFIG_BOARD_RF_CMD
    extern void cli_reg_cmd_board_rf(void);
    cli_reg_cmd_board_rf();
#endif

#ifdef CONFIG_CLI_CMD_DEBUG
    cli_reg_cmd_ble();

    cli_reg_cmd_blemesh();

    cli_reg_cmd_iperf();

    int ble_debug = 0;
    int ret;
    ret = aos_kv_getint("ble_debug", &ble_debug);
    if (ret < 0) {
        ble_debug = 0;
    }

    if (ble_debug) {
        LOGI("Init", "BLE Debug Mode, type `ble` or `blemesh` for support command");
        while (1) {
            aos_msleep(100000);
        }
    }
#endif
}
