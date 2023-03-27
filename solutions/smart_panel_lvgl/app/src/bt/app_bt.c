#if defined(CONFIG_BT_BREDR) && (CONFIG_BT_BREDR == 1)
#include <smart_audio.h>
#include <yoc_app_bt.h>
#include <aos/bt.h>
#include <ulog/ulog.h>

#include "app_wifi.h"
#include "event_mgr/app_event.h"

#define TAG "appbt"

#if defined(CONFIG_BT_A2DP) && (CONFIG_BT_A2DP == 1)
static void bt_callback(yoc_app_bt_event_t event, yoc_app_bt_param_t *param)
{
    switch (event) {
        case YOC_APP_BT_A2DP_CONNECTED:
            app_event_update(EVENT_STATUS_BT_CONNECTED);
            break;
        case YOC_APP_BT_A2DP_DISCONNECTED:
            app_event_update(EVENT_STATUS_BT_DISCONNECTED);
            break;
        case YOC_APP_BT_A2DP_LINK_LOSS:
            break;
        case YOC_APP_BT_A2DP_PLAY_STATUS_STOPPED:
            break;
        case YOC_APP_BT_A2DP_PLAY_STATUS_PLAYING:
            break;
        case YOC_APP_BT_A2DP_PLAY_STATUS_PAUSEED:
            break;
        case YOC_APP_BT_AVRCP_STATUS_PAUSEED:
            break;
        case YOC_APP_BT_AVRCP_STATUS_STOPPED:
            break;
        case YOC_APP_BT_AVRCP_STATUS_PLAYING:
            break;
        default:
            break;
    }
}
#endif

void app_bt_adv_enable(int enable)
{
    if (enable) {
        bt_dev_addr_t addr;
        char          dev_name[64] = "YoC_BT";
        ble_stack_get_local_addr(&addr);
        sprintf(dev_name,
                "YoC[%02x:%02x:%02x:%02x:%02x:%02x]",
                addr.val[0],
                addr.val[1],
                addr.val[2],
                addr.val[3],
                addr.val[4],
                addr.val[5]);
        LOGD(TAG, "bt addr: %s", dev_name);
        yoc_app_bt_set_device_name(dev_name);

        yoc_app_bt_gap_set_scan_mode(1);
    } else {
        yoc_app_bt_gap_set_scan_mode(0);
        yoc_app_bt_a2dp_disconnect();
    }
}

void app_bt_init(void)
{
    yoc_app_bt_init();

    yoc_app_bt_gap_set_scan_mode(0);

#if defined(CONFIG_BT_A2DP) && (CONFIG_BT_A2DP == 1)
    yoc_app_bt_a2dp_register_cb(bt_callback);
    smtaudio_register_bt_a2dp(0, NULL, 0, 1.0f, 0);
#endif

#if defined(CONFIG_BT_HFP) && (CONFIG_BT_HFP == 1)
    smtaudio_register_bt_hfp(0, NULL, 0, 1.0f, 0);
#endif
}
#endif
