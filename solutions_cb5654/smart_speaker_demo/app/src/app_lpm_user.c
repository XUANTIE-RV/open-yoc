/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "app_main.h"
#include "app_lpm.h"
#include "app_sys.h"

#define TAG "LPM"

static long long g_lpm_vad_tag;
static int lpm_check(void)
{
    if (pm_get_policy() == LPM_POLICY_NO_POWER_SAVE) {
        return 0;
    }

    //播放状态
    if (app_player_is_busy()) {
        return 0;
    }

    //MIC状态
    if (app_mic_is_busy()) {
        return 0;
    }

    //VAD
    if (aos_now_ms() - g_lpm_vad_tag < 2000) {
        return 0;
    }

    //wifi state
    if (wifi_is_pairing() || wifi_connecting()) {
        return 0;
    }

#if defined(APP_FOTA_EN) && APP_FOTA_EN
    // fota state
    if (app_fota_is_downloading()) {
        return 0;
    }
#endif

    if (clock_alarm_get_status() == 2) {
        return 0;
    }

    return 1;
}

static void lpm_wake_rtc_check(void)
{
    time_t time_sys_now = time(NULL);
    time_t time_rtc_now = rtc_to_system();

    event_publish(EVENT_CLOCK_ALARM, NULL);
    if (abs((int)(time_sys_now - time_rtc_now)) > 5) {
        event_publish_delay(EVENT_NTP_RETRY_TIMER, NULL, 6000);
        LOGE(TAG, "ntp time");
    }
}

extern uint32_t g_sram_timer_wakeup;
static void lpm_wake_cb(pm_policy_t policy, lpm_event_e event)
{
    switch (event) {
        case LPM_EVENT_SUSPEND:
            LOGI(TAG, "suspend");
            wifi_set_lpm(1);
            app_audio_pa_ctrl(0);
            app_set_led_state(LED_TURN_OFF);
            if(policy == LPM_POLICY_DEEP_SLEEP) {
                LOGI(TAG, "enter deep sleep mode!");
                app_lpm_enter_standby_mode();
            } else {
                board_enter_lpm(LPM_POLICY_LOW_POWER);
            }
            break;
        case LPM_EVENT_WAKUP:
#if SRAM_TIMER_EN
            LOGI(TAG, "wakeup %u %u", g_sram_timer_wakeup, sram_timer_get_us());
#else
            LOGI(TAG, "wakeup");
#endif

            board_leave_lpm(policy);

            app_audio_pa_ctrl(1);
            wifi_set_lpm(0);
            app_status_update();
#if defined(APP_FOTA_EN) && APP_FOTA_EN
            app_fota_do_check();
#endif
#if defined(CONFIG_RTC_EN) && CONFIG_RTC_EN
            /* 系统使用rtc 时间 */
            lpm_wake_rtc_check();
#endif
            break;
        default:;
    }
}



int app_low_power_init(void)
{
    /* 低功耗 */
    int lpm_mode = 0;
    aos_kv_getint("lpm_en", &lpm_mode);
    LOGD(TAG, lpm_mode ? "lpm enable" : "lpm disable");
    if (lpm_mode) {
        lpm_wake_src_cfg_t wake_src;

        wake_src.lpm_policy = LPM_POLICY_LOW_POWER;
        wake_src.pin_src.pin = APP_LPM_VAD_PIN;
        wake_src.pin_src.type = LPM_PIN_SRC_ADC;
        // wake_src.pin_src.mode = GPIO_IRQ_MODE_LOW_LEVEL;
        app_lpm_add_wake_src(wake_src);

#if defined(APP_LPM_WL_WAKE_HOST_EN) && APP_LPM_WL_WAKE_HOST_EN
        wake_src.lpm_policy = LPM_POLICY_LOW_POWER;
        wake_src.pin_src.pin = PIN_WL_WAKE_HOST;
        wake_src.pin_src.type = LPM_PIN_SRC_GPIO;
        wake_src.pin_src.mode = GPIO_IRQ_MODE_LOW_LEVEL;
        app_lpm_add_wake_src(wake_src);
#endif

#ifdef APP_LPM_DEEP_WAKEUP_PIN
        wake_src.lpm_policy = LPM_POLICY_DEEP_SLEEP;
        wake_src.pin_src.pin = APP_LPM_DEEP_WAKEUP_PIN;
        wake_src.pin_src.type = LPM_PIN_SRC_GPIO;
        wake_src.pin_src.mode = GPIO_IRQ_MODE_RISING_EDGE;
        app_lpm_add_wake_src(wake_src);
#endif

#if defined(SIMULATED_POWER_KEY) && SIMULATED_POWER_KEY
        app_lpm_set_power_pin(POWER_KEY_PIN);
#endif

        app_lpm_init(lpm_check, lpm_wake_cb);
    }

    return 0;
}

void app_lpm_vad_check(void)
{
     g_lpm_vad_tag = aos_now_ms();
}