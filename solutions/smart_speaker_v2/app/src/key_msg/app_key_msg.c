/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdint.h>

#include <aos/kernel.h>
#include <aos/kv.h>

#include <smart_audio.h>
#include <bt/yoc_app_bt.h>

#include "wifi/app_wifi.h"
#include "sys/app_sys.h"
#include "event_mgr/app_event.h"
#include "bt/app_bt.h"

#include "app_key_msg.h"

#define TAG "keymsg"

#define MESSAGE_NUM 10

static uint8_t     s_q_buffer[sizeof(int) * MESSAGE_NUM];
static aos_queue_t s_queue;

void app_key_msg_send(int keymsg_id, void *priv)
{
    LOGD(TAG, "keymsg_id=%d\n", keymsg_id);
    int ret = aos_queue_send(&s_queue, &keymsg_id, sizeof(int));
    if (ret < 0) {
        LOGE(TAG, "queue send failed");
    }
}

static void key_msg_proc_task(void *arg)
{
    int    keymsg_id;
    size_t len;

    while (1) {
        aos_queue_recv(&s_queue, AOS_WAIT_FOREVER, &keymsg_id, &len);
        app_event_update(EVENT_KEY_PRESSED);

        switch (keymsg_id) {
            case KEY_MSG_VOL_UP:
                smtaudio_vol_up(10);
                break;

            case KEY_MSG_VOL_DOWN:
                smtaudio_vol_down(10);
                break;

            case KEY_MSG_MUTE:
                if (smtaudio_get_state() == SMTAUDIO_STATE_MUTE) {
                    smtaudio_unmute();
                } else {
                    smtaudio_mute();
                }
                app_event_update(EVENT_PLAYER_CHANGE);
                break;

            case KEY_MSG_PLAY: {
                smtaudio_state_t smta_state = smtaudio_get_state();
                LOGD(TAG, "smta state %d", smta_state);
                if (smta_state == SMTAUDIO_STATE_PLAYING) {
                    smtaudio_pause();
                } else if (smta_state == SMTAUDIO_STATE_PAUSE || smta_state == SMTAUDIO_STATE_STOP) {
                    smtaudio_enable_ready_list(1);
                    smtaudio_resume();
                }
                app_event_update(EVENT_PLAYER_CHANGE);
            } break;

            case KEY_MSG_BT:
                LOGD(TAG, "open bluetooth...");
#if defined(CONFIG_BT_BREDR) && (CONFIG_BT_BREDR == 1)
                app_bt_adv_enable(1);
#endif
                break;

            case KEY_MSG_WIFI_PROV:
                LOGD(TAG, "wifi prov");
                aos_kv_setint("wprov_method", WIFI_PROVISION_SOFTAP);
                app_sys_reboot(BOOT_REASON_WIFI_CONFIG);
                break;

            case KEY_MSG_POWER:
                LOGD(TAG, "power key");
                aos_msleep(10000);
                break;

            case KEY_MSG_UPLOG:
                LOGD(TAG, "upload log");
                aos_msleep(10000);
                break;

            case KEY_MSG_FACTORY:
                LOGD(TAG, "factory");
                aos_msleep(10000);
                break;

            default:
                break;
        }
    }
}

void app_key_msg_init(void)
{
    aos_task_t task;

    int ret = aos_queue_new(&s_queue, s_q_buffer, MESSAGE_NUM * sizeof(int), sizeof(int));
    aos_check(!ret, EIO);
    aos_task_new_ext(&task, "keymsg", key_msg_proc_task, NULL, 4096, AOS_DEFAULT_APP_PRI + 4);
}
