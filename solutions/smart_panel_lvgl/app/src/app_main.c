/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <board.h>

#include <stdio.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
#include <aos/yloop.h>
#include <sys_clk.h>
#include <gateway.h>

#include "sys/app_sys.h"
#include "aui_cloud/app_aui_cloud.h"
#include "player/app_player.h"
#include "occ_fota/app_fota.h"
#include "voice/app_voice.h"
#include "wifi/app_net.h"
#include "key_msg/app_key_msg.h"
#include "event_mgr/app_event.h"
#include "linkkit_gateway/app_gateway_ut.h"
#include "display/app_disp.h"
#include "bt/app_bt.h"
#include "alarms/app_alarms.h"
#include "button/app_button.h"
#include "factory/app_factory.h"
#include "ui/ui_entry.h"
#include "app_main.h"

#define TAG "UI"

app_init_state_e g_init_state = APP_INIT_RESOURCE;

gw_evt_discovered_info_t subdev[10];

struct updatestatus {
    gw_subdev_t subdev;
    uint8_t     onoff;
} subdevstatus;

bool doonce = true;
bool addif  = true;
int  dev;

extern int         board_ftmode_check(void);
extern const char *bt_hex_real(const void *buf, size_t len);
#if defined(CONFIG_SMARTLIVING_OTA) && CONFIG_SMARTLIVING_OTA
extern gw_status_t app_smartliving_ota_subdev_status_process(gw_subdev_t subdev, gw_subdev_status_type_e gw_status,
                                                             gw_status_param_t status, int status_len);
extern gw_status_t app_smartliving_ota_event_process(gw_event_type_e gw_evt, gw_evt_param_t gw_evt_param);
#endif

static void _app_print_fail_info(gw_evt_subdev_add_fail_t *fail_info)
{
    switch (fail_info->protocol) {
        case GW_NODE_BLE_MESH: {
            LOGE(TAG,
                 "Add mesh dev failded, err %d, dev addr:%s",
                 fail_info->failed_reason,
                 bt_hex_real(fail_info->protocol_info.ble_mesh_info.dev_addr, 6));
        } break;
        default:
            LOGE(TAG, "Unknow protocol for failed prov info");
    }
}

static inline int bt_addr_val_to_str(const uint8_t addr[6], char *str, size_t len)
{
    return snprintf(str, len, "%02X:%02X:%02X:%02X:%02X:%02X", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
}

static inline int bt_uuid_val_to_str(const uint8_t uuid[16], char *str, size_t len)
{
    return snprintf(str,
                    len,
                    "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                    uuid[0],
                    uuid[1],
                    uuid[2],
                    uuid[3],
                    uuid[4],
                    uuid[5],
                    uuid[6],
                    uuid[7],
                    uuid[8],
                    uuid[9],
                    uuid[10],
                    uuid[11],
                    uuid[12],
                    uuid[13],
                    uuid[14],
                    uuid[15]);
}

gw_status_t app_gw_event_process_cb(gw_event_type_e gw_evt, gw_evt_param_t gw_evt_param)
{
    LOGD(TAG, "Gw event:%02x", gw_evt);
    switch (gw_evt) {
        case GW_SUBGRP_EVT_CREATE: {
            gw_evt_subgrp_create_t *group = (gw_evt_subgrp_create_t *)gw_evt_param;
            LOGD(TAG, "group.status:%d, group.subgrp:%d\r\n", group->status, group->subgrp);
        } break;
        case GW_EVT_FOUND_DEV: {
            if (doonce) {
                dev    = 0;
                doonce = false;
            }
            /*publish msg*/
            gw_evt_discovered_info_t *msg = (gw_evt_discovered_info_t *)malloc(sizeof(gw_evt_discovered_info_t));
            memcpy(msg, gw_evt_param, sizeof(gw_evt_discovered_info_t));
            event_publish(EVENT_GUI_USER_MESH_SCAN_INFO_UPDATE, (void *)msg);

            gw_evt_discovered_info_t *scan_msg = (gw_evt_discovered_info_t *)gw_evt_param;
            if (scan_msg->protocol == GW_NODE_BLE_MESH) {
                LOGI(TAG,
                     "Found Mesh Node:%s,%s",
                     bt_hex_real(scan_msg->protocol_info.ble_mesh_info.dev_addr, 6),
                     bt_hex_real(scan_msg->protocol_info.ble_mesh_info.uuid, 16));
            }
            for (int i = 0; i < 10; i++) {
                if (scan_msg == &subdev[i]) {
                    addif = false;
                }
            }
            if (addif) {
                memcpy(&subdev[dev], scan_msg, sizeof(gw_discovered_dev_t));
                dev++;
                if (9 == dev)
                    dev = 0;
            }
        } break;
        case GW_EVT_FOUND_DEV_TIMEOUT: {
            LOGI(TAG, "Dev discover timeout");
        } break;
        case GW_SUBDEV_EVT_ADD: {
            // memcpy((void *)&gw_evt_subdev_add_init, gw_evt_param, sizeof(gw_evt_subdev_add_t));
            gw_evt_subdev_add_t *add = (gw_evt_subdev_add_t *)malloc(sizeof(gw_evt_subdev_add_t));
            memcpy(add, gw_evt_param, sizeof(gw_evt_subdev_add_t));
            event_publish(EVENT_GUI_USER_MESH_ADD_INFO_UPDATE, (void *)add);
        }
        case GW_SUBDEV_EVT_DEL:
#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO
            app_gateway_ut_event_process(gw_evt, gw_evt_param);
#endif
            break;
        case GW_SUBDEV_EVT_ADD_FAIL: {
            _app_print_fail_info((gw_evt_subdev_add_fail_t *)gw_evt_param);
        }
        case GW_SUBDEV_HB_MGMT_SET: {
            gw_subdev_hb_mgmt_set_status_t *set_status = (gw_subdev_hb_mgmt_set_status_t *)gw_evt_param;
            LOGD(TAG, "Gw hb mgmt set status:%02x", set_status->status);
            (void)set_status;
        } break;
        case GW_SUBDEV_EVT_OTA_FW:

#if defined(CONFIG_SMARTLIVING_OTA) && CONFIG_SMARTLIVING_OTA
            app_smartliving_ota_event_process(gw_evt, gw_evt_param);
#endif
        default:
            break;
    }

    return 0;
}

static inline char *_state_str(uint8_t state)
{
    switch (state) {
        case GW_SUBDEV_NOT_ACTIVE:
            return "INACTIVE";
        case GW_SUBDEV_ACTIVE:
            return "ACTIVE";
        default:
            return "UNKNOW";
    }
}

static inline char *_reason_str(uint8_t state)
{
    switch (state) {
        case GW_SUBDEV_STATUS_CHANGE_BY_NODE_ADD:
            return "NODE_ADD";
        case GW_SUBDEV_STATUS_CHANGE_BY_NODE_RST:
            return "NODE_RST";
        case GW_SUBDEV_STATUS_CHANGE_BY_ACTIVE_CHECK_FAIL:
            return "ACTIVE_CHECK_FAIL";
        case GW_SUBDEV_STATUS_CHANGE_BY_ACTIVE_CHECK_SUCCESS:
            return "ACTIVE_CHECK_SUCCESS";
        case GW_SUBDEV_STATUS_CHANGE_BY_HB_NOT_ENABLE:
            return "HB_NOT_ENABLE";
        case GW_SUBDEV_STATUS_CHANGE_BY_HB_ERR_PARAM:
            return "HB_ERR_PARAM";
        case GW_SUBDEV_STATUS_CHANGE_BY_HB_RECV:
            return "HB_RECV";
        case GW_SUBDEV_STATUS_CHANGE_BY_VERSION_RECV:
            return "VERSION_RECV";
        case GW_SUBDEV_STATUS_CHANGE_BY_HB_LOST:
            return "HB_LOST";
        default:
            return "UNKNOW";
    }
}

gw_status_t app_subdev_status_process_cb(gw_subdev_t subdev, gw_subdev_status_type_e gw_status,
                                         gw_status_param_t status, int status_len)
{

    LOGD(TAG, "Subdev[%d] status:%02x ", subdev, gw_status);
    switch (gw_status) {
        case GW_SUBDEV_STATUS_HB_SET: {
            gw_subdev_hb_set_status_t *hb_set_status = (gw_subdev_hb_set_status_t *)status;
            LOGD(TAG,
                 "Subdev %d Hb set status:%02x,period:%04x",
                 subdev,
                 hb_set_status->status,
                 hb_set_status->period_s);
            (void)hb_set_status;
        } break;
        case GW_SUBDEV_STATUS_ONOFF: {
            subdevstatus.subdev = subdev;
            subdevstatus.onoff  = *(uint8_t *)status;
            event_publish(EVENT_GUI_USER_MESH_STATUS_ONOFF_UPDATE, (void *)&subdevstatus);
            if (subdev == SUBDEV_GW_SELF) {
#if defined(CONFIG_BOARD_PWM) && CONFIG_BOARD_PWM > 0
                app_pwm_led_control(*(uint8_t *)status);
#endif
            }
        } break;
        case GW_SUBDEV_STATUS_BRI:
        case GW_SUBDEV_STATUS_TRIPLES_INFO:
        case GW_SUBDEV_STATUS_RAW_DATA:
        case GW_SUBDEV_STATUS_MODEL: {
#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO
            app_gateway_ut_subdev_status_process(subdev, gw_status, status, status_len);
#endif
        } break;
        case GW_SUBDEV_STATUS_CHANGE: {
            gw_status_subdev_t *subdev_status = (gw_status_subdev_t *)status;
            LOGD(TAG,
                 "Subdev %d status change from %s ---> %s, reason:%s",
                 subdev,
                 _state_str(subdev_status->old_status),
                 _state_str(subdev_status->new_status),
                 _reason_str(subdev_status->reason));
            (void)subdev_status;
        } break;
        case GW_SUBDEV_STATUS_OTA:
        case GW_SUBDEV_STATUS_VERSION: {

#if defined(CONFIG_SMARTLIVING_OTA) && CONFIG_SMARTLIVING_OTA
            app_smartliving_ota_subdev_status_process(subdev, gw_status, status, status_len);
#endif
        } break;
        default:
            break;
    }

    return 0;
}

static int _app_set_init_state(app_init_state_e state)
{
    switch (state) {
        case APP_INIT_RESOURCE: {
            g_init_state = state;

            LOGD(TAG, "init state: %d", state);

            aos_loop_init();
#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO
            app_gateway_ut_init();
#endif
        }
        case APP_INIT_GATEWAY: {
            g_init_state = state;

            LOGD(TAG, "init state: %d", state);

            /* Register callback for gateway operate event */
            gateway_event_cb_register(app_gw_event_process_cb);

            /* Register callback for subdev status report */
            gateway_subdev_status_cb_register(app_subdev_status_process_cb);

            /* init gateway service */
            gateway_init();

            gw_config_e gw_config = 0;

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
            /* config if gateway support occ auth/tsl engine and so on*/
            gw_config |= GW_CONFIG_SUPPORT_OCC_AUTH;
#endif

#if defined(CONFIG_TSL_DEVICER_MESH) && CONFIG_TSL_DEVICER_MESH
            /* config if gateway support occ auth/tsl engine and so on*/
            gw_config |= GW_CONFIG_SUPPORT_TSL_ENGINE;
#endif
            gateway_config(gw_config);

#if defined(CONFIG_GW_AT_EN) && CONFIG_GW_AT_EN
            /* Registe AT Command after all service started */
            app_at_cmd_init();
#endif

            break;
        }
        default:
            break;
    }

    return 0;
}

static void _app_init_set_state_cb(uint32_t event_id, const void *param, void *context)
{
    if (event_id != EVENT_APP_INIT_STATE)
        return;

    app_init_state_e state = *(app_init_state_e *)param;
    _app_set_init_state(state);
}

extern void cxx_system_init(void);
int         main(int argc, char *argv[])
{
    cxx_system_init();
    board_yoc_init();

    LOGI("main", "build time: %s, %s clock=%uHz\r\n", __DATE__, __TIME__, soc_get_cur_cpu_freq());

    /* 文件系统 */
    app_sys_fs_init();

    /* 图形 */
    app_ui_init();

    /* 应用事件 */
    app_sys_init();
    app_display_init();
    app_event_init();
    app_ui_event_init();
    app_key_msg_init();
    app_cli_init();

    event_subscribe(EVENT_APP_INIT_STATE, _app_init_set_state_cb, NULL);
    if (board_ftmode_check() == 1) {
        app_ftmode_entry();
    }

    /* 播放器 */
    LOGI("main", "audio init");
#if defined(BOARD_AUDIO_SUPPORT) && BOARD_AUDIO_SUPPORT
    board_audio_init();

    app_speaker_init();
    app_player_init();
#endif

    /* 网络蓝牙 */
#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT
    board_wifi_init();
#endif

#if defined(BOARD_BT_SUPPORT) && BOARD_BT_SUPPORT
    board_bt_init();
#endif

#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT
    app_network_init();
#endif
    _app_set_init_state(g_init_state);
#if defined(CONFIG_BT_BREDR) && (CONFIG_BT_BREDR == 1)
    app_bt_init();
#endif

    /* 语音交互 */
    app_mic_init();
#if defined(CONFIG_AUI_CLOUD) && CONFIG_AUI_CLOUD
    app_aui_cloud_init();
#endif

    /* 其他外设 */
#if defined(CONFIG_BOARD_BUTTON) && BOARD_BUTTON_NUM > 0
    app_button_init();
#endif

    // app_alrams_init();

#if defined(CONFIG_GW_FOTA_EN) && (CONFIG_GW_FOTA_EN == 1)
    app_fota_init();
#endif

    return 0;
}
