/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include <board.h>
#include <app_factory.h>
#include <gateway.h>
#include <yoc/netmgr.h>
#include <yoc/atserver.h>

#include "app_main.h"
#include "app_sys.h"
#include "app_init.h"
#include "app_factory.h"
#if defined(CONFIG_GW_FOTA_EN) && CONFIG_GW_FOTA_EN
#include "app_fota.h"
#endif

#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO
#include "linkkit_gateway/app_gateway_ut.h"

#if defined(CONFIG_SMARTLIVING_OTA) && CONFIG_SMARTLIVING_OTA
//#include "linkkit_gateway/app_smartliving_ota.h"
#endif
#endif

#define TAG "app_main"

app_init_state_e g_init_state = APP_INIT_RESOURCE;

gw_evt_discovered_info_t subdev;

extern const char *bt_hex_real(const void *buf, size_t len);
extern int board_ftmode_check(void);
#if defined(CONFIG_SMARTLIVING_OTA) && CONFIG_SMARTLIVING_OTA
extern gw_status_t app_smartliving_ota_subdev_status_process(gw_subdev_t subdev, gw_subdev_status_type_e gw_status,
                                                             gw_status_param_t status, int status_len);
extern gw_status_t app_smartliving_ota_event_process(gw_event_type_e gw_evt, gw_evt_param_t gw_evt_param);
#endif

static void _app_print_fail_info(gw_evt_subdev_add_fail_t *fail_info)
{
    switch (fail_info->protocol) {
        case GW_NODE_BLE_MESH: {
            LOGE(TAG, "Add mesh dev failded, err %d, dev addr:%s", fail_info->failed_reason,
                 bt_hex_real(fail_info->protocol_info.ble_mesh_info.dev_addr, 6));
        } break;
        default:
            LOGE(TAG, "Unknow protocol for failed prov info");
    }
}

gw_status_t app_gw_event_process_cb(gw_event_type_e gw_evt, gw_evt_param_t gw_evt_param)
{
    LOGD(TAG, "Gw event:%02x", gw_evt);
    switch (gw_evt) {
        case GW_EVT_FOUND_DEV: {
            gw_evt_discovered_info_t *scan_msg = (gw_evt_discovered_info_t *)gw_evt_param;
            if (scan_msg->protocol == GW_NODE_BLE_MESH) {
                LOGI(TAG, "Found Mesh Node:%s,%s", bt_hex_real(scan_msg->protocol_info.ble_mesh_info.dev_addr, 6),
                     bt_hex_real(scan_msg->protocol_info.ble_mesh_info.uuid, 16));
            }
            memcpy(&subdev, scan_msg, sizeof(gw_discovered_dev_t));
        } break;
        case GW_EVT_FOUND_DEV_TIMEOUT: {
            LOGI(TAG, "Dev discover timeout");
        } break;
        case GW_SUBDEV_EVT_ADD:
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
#if defined(CONFIG_GW_FOTA_EN) && CONFIG_GW_FOTA_EN
            app_fota_event_process(gw_evt, gw_evt_param);
#endif

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
    LOGD(TAG, "Subdev[%d] status:%02x", subdev, gw_status);
    switch (gw_status) {
        case GW_SUBDEV_STATUS_HB_SET: {
            gw_subdev_hb_set_status_t *hb_set_status = (gw_subdev_hb_set_status_t *)status;
            LOGD(TAG, "Subdev %d Hb set status:%02x,period:%04x", subdev, hb_set_status->status,
                 hb_set_status->period_s);
            (void)hb_set_status;
        } break;
        case GW_SUBDEV_STATUS_ONOFF: {
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
            LOGD(TAG, "Subdev %d status change from %s ---> %s, reason:%s", subdev,
                 _state_str(subdev_status->old_status), _state_str(subdev_status->new_status),
                 _reason_str(subdev_status->reason));
            (void)subdev_status;
        } break;
        case GW_SUBDEV_STATUS_OTA:
        case GW_SUBDEV_STATUS_VERSION: {
#if defined(CONFIG_GW_FOTA_EN) && CONFIG_GW_FOTA_EN
            app_fota_subdev_status_process(subdev, gw_status, status, status_len);
#endif

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

            app_sys_init();

            aos_loop_init();

#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO
            app_gateway_ut_init();
#endif

#if defined(CONFIG_BOARD_BUTTON) && CONFIG_BOARD_BUTTON > 0
            app_button_init();
#endif

#if defined(CONFIG_BOARD_PWM) && CONFIG_BOARD_PWM > 0
            app_pwm_led_init();
#endif

#if defined(CONFIG_GW_NETWORK_SUPPORT) && CONFIG_GW_NETWORK_SUPPORT
            app_network_init();
            break;
#else
            LOGD(TAG, "don't support wifi network, init gateway directly");
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

#if defined(CONFIG_GW_FOTA_EN) && CONFIG_GW_FOTA_EN
            app_fota_init(); /* init fota and load fota devices*/
#endif

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

int main()
{
    board_yoc_init();

    LOGI(TAG, "Ble mesh gateway,Version %s\n", aos_get_app_version());

    event_service_init(NULL);
    event_subscribe(EVENT_APP_INIT_STATE, _app_init_set_state_cb, NULL);

#if defined(CONFIG_GW_AT_EN) && CONFIG_GW_AT_EN && defined(BOARD_UART_NUM) && (BOARD_UART_NUM > 1)
    app_at_server_init(NULL, CONFIG_AT_UART_NAME);
    app_rpt_fwver(CONFIG_APP_VERSION);
#endif

    if (board_ftmode_check() == 1) {
        app_ftmode_entry();
    }

    _app_set_init_state(g_init_state);

    aos_loop_run();

    return 0;
}
