/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/cli.h"
#include "app_main.h"
#include "app_sys.h"
#include "app_init.h"
#include "w800_devops.h"
#include "hci_hal_h4.h"
#include <yoc/netmgr.h>
#include <uservice/eventid.h>
#include <uservice/uservice.h>
#include <yoc/netmgr_service.h>
#include "aos/ble.h"
#include <aos/yloop.h>
#include "combo_net.h"

#define TAG "app"
extern int wifi_prov_method;

extern int tls_set_bt_mac_addr(uint8_t *mac);
extern int tls_get_bt_mac_addr(uint8_t *mac);

static void cmd_prov_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (strcmp(argv[1], "0") == 0) {
        LOGD(TAG, "smart config");
        wifi_prov_method = WIFI_PROVISION_SL_SMARTCONFIG;
        aos_kv_setint("wprov_method", wifi_prov_method);
        app_sys_set_boot_reason(BOOT_REASON_WIFI_CONFIG);
        aos_reboot();
    } else if (strcmp(argv[1], "1") == 0) {
        LOGD(TAG, "dev ap");
        wifi_prov_method = WIFI_PROVISION_SL_DEV_AP;
        aos_kv_setint("wprov_method", wifi_prov_method);
        app_sys_set_boot_reason(BOOT_REASON_WIFI_CONFIG);
        aos_reboot();
    } else if (strcmp(argv[1], "2") == 0) {
        LOGD(TAG, "ble");
        wifi_prov_method = WIFI_PROVISION_SL_BLE;
        aos_kv_setint("wprov_method", wifi_prov_method);

        aos_kv_del("AUTH_AC_AS");
        aos_kv_del("AUTH_KEY_PAIRS");
        app_sys_set_boot_reason(BOOT_REASON_WIFI_CONFIG);
        aos_reboot();
    } else if (strcmp(argv[1], "3") == 0) {
        LOGD(TAG, "softap");
        wifi_prov_method = WIFI_PROVISION_SOFTAP;
        aos_kv_setint("wprov_method", wifi_prov_method);
        app_sys_set_boot_reason(BOOT_REASON_WIFI_CONFIG);
        aos_reboot();
    }
}

void cli_reg_cmd_wifi_prov(void)
{
    static const struct cli_command cmd_info = {
        "prov",
        "wifi prov commands",
        cmd_prov_func,
    };

    aos_cli_register_command(&cmd_info);
}

static void cmd_bt_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (strcmp(argv[1], "mac") == 0) {
        if (argc >= 3) {
            if (strlen(argv[2]) == 12) {
                uint8_t mac[6];
                char *p = argv[2];
                for (int i = 0; i < 6; ++i) {
                    uint8_t t = 0;
                    for (int j = 0; j < 2; ++j) {
                        t <<= 4;
                        if (*p >= '0' && *p <= '9') {
                            t += *p - '0';
                        } else if (*p >= 'a' && *p <= 'f') {
                            t += *p - 'a' + 10;
                        } else if (*p >= 'A' && *p <= 'F') {
                            t += *p - 'A' + 10;
                        } else {
                            printf("Usage: bt mac C01122334455\n");
                            return;
                        }
                        ++p;
                    }
                    mac[i] = t;
                }
                if (!tls_set_bt_mac_addr(mac)) {
                    printf("set bt mac successfully\n");
                } else {
                    printf("set bt mac failed\n");
                }
            } else {
                printf("Usage: bt mac C01122334455\n");
            }
        } else {
            uint8_t mac[6];
            if (!tls_get_bt_mac_addr(mac)) {
                printf("bt mac = %02X%02X%02X%02X%02X%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            } else {
                printf("bt mac not set\n");
            }
        }
    }
}

void cli_reg_cmd_bt(void)
{
    static const struct cli_command cmd_info = {
        "bt",
        "bt commands",
        cmd_bt_func,
    };

    aos_cli_register_command(&cmd_info);
}

static void cli_reg_cmds(void)
{
    cli_reg_cmd_wifi_prov();
    cli_reg_cmd_bt();
    //cli_reg_cmd_iperf();
    cli_reg_cmd_w800();
    //cli_reg_cmd_ifconfig();
    //cli_reg_cmd_sysinfo();
}

int main()
{
    int tick_counter = 0;
    board_yoc_init();
    LOGD(TAG, "%s\n", aos_get_app_version());

    aos_kv_setint(KV_WIFI_EN, 1);
    
    event_service_init(NULL);

    app_sys_init();
    
    board_base_init();

    aos_loop_init();
//    bt_w800_register();

    cli_reg_cmds();
    
    wifi_mode_e mode = app_network_init();

#if defined(APP_FOTA_EN) && APP_FOTA_EN
    app_fota_init();
#endif

    app_button_init();

    /* AT cmd related */
    app_pinmap_usart_init(1);//uart1
    app_at_server_init(NULL, "uart1");
    app_at_cmd_init();

    app_pwm_led_init();

    aos_loop_run();

    return 0;
}
