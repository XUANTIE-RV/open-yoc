/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <pinmux.h>
#include <devices/led.h>
#include <devices/devicelist.h>
#include <soc.h>
#include <syscfg.h>
#include <yoc/adc_key_srv.h>
#include <yoc/netmgr.h>

#include "silan_voice_adc.h"
#include "app_lpm.h"
#include "app_sys.h"

#include "app_main.h"

#define TAG "LPM"

static int g_lpm_idle_check_time = APP_LPM_IDLE_CHECK_TIME; /* 秒 */
static pwr_idle_check_cb_t user_lpm_check_cb;
SRAM_DATA lpm_pin_src_t wake_srcs_sleep[MAX_LPM_WAKESRC_COUNT];
static lpm_pin_src_t wake_srcs_standby[MAX_LPM_WAKESRC_COUNT];
SRAM_DATA uint16_t g_wake_src_power_pin = 0xFFFF;
SRAM_DATA uint8_t g_pwr_key_pressed = 0;

extern void lpm_clk_shutdown();

static void lpm_enable(void)
{
    pm_config_policy(LPM_POLICY_LOW_POWER);
    event_publish_delay(EVENT_UPDATE_STATUS, NULL, 50);
}

static void user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    static int g_event_update_status = 0;

    switch (event_id) {
        case EVENT_UPDATE_STATUS:
            if (g_event_update_status == 0) {
                g_event_update_status = 1;
            }
            break;
        case EVENT_LPM_CHECK:
            if (g_event_update_status) {
                if (user_lpm_check_cb && user_lpm_check_cb()) {
                    //LOGI(TAG, "EVENT_LPM_CHECK (%d)", g_event_update_status);
                    g_event_update_status++;
                    
                    if (g_event_update_status >= g_lpm_idle_check_time) {
                        g_event_update_status = 0;
                        pm_agree_halt(0);
                    }
                } else {
                    if (g_event_update_status != 1) {
                        LOGI(TAG, "LPM CHK RST %d/%d", g_event_update_status, g_lpm_idle_check_time);
                    }
                    g_event_update_status = 1;
                }
            }
            event_publish_delay(EVENT_LPM_CHECK, NULL, 1000);

            break;
        case EVENT_NET_NTP_SUCCESS:
            // app_lpm_enable();
            break;
        default:;
    }
}

static void lpm_config_sdbwake_gpio(int32_t pin_num, gpio_irq_mode_e int_mode)
{
extern int silan_iomux_adc_close(uint8_t channel);
    silan_iomux_adc_close(pin_num);
    drv_pinmux_config(pin_num, PIN_FUNC_GPIO);

    gpio_pin_handle_t *pin = csi_gpio_pin_initialize(pin_num, NULL);
    csi_gpio_pin_config_mode(pin, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pin, GPIO_DIRECTION_INPUT);
    csi_gpio_pin_set_irq(pin, int_mode, 1);
    csi_gpio_pin_uninitialize(pin);   
}

static void lpm_config_sdbwake_src()
{
    if (g_pwr_key_pressed && g_wake_src_power_pin != 0xffff) {
        lpm_config_sdbwake_gpio(g_wake_src_power_pin, GPIO_IRQ_MODE_FALLING_EDGE);
    } else {
        for (int i = 0; i < MAX_LPM_WAKESRC_COUNT; ++i) {
            if (wake_srcs_standby[i].type == LPM_PIN_SRC_NONE) {
                continue;
            } else {
                lpm_config_sdbwake_gpio(wake_srcs_standby[i].pin, wake_srcs_standby[i].mode);
            }
        }
    }
}

extern netmgr_hdl_t app_netmgr_hdl;
void app_lpm_enter_standby_mode(void)
{
    if (app_netmgr_hdl) {
        netmgr_stop(app_netmgr_hdl);
        netmgr_dev_wifi_deinit(app_netmgr_hdl);
        netmgr_service_deinit();
    }

#ifdef PIN_WIFI_POWER_EN
    app_gpio_write(PIN_WIFI_POWER_EN, 0);
#endif

    board_enter_lpm(LPM_POLICY_DEEP_SLEEP);

    key_srv_stop();
    silan_risc_debug_close();

    lpm_config_sdbwake_src();
    lpm_clk_shutdown();
}

int app_lpm_set_power_pin(uint16_t pwr_pin) 
{
    if (g_wake_src_power_pin != 0xFFFF) {
        LOGW(TAG, "only one power key");
        return -1;
    }

    g_wake_src_power_pin = pwr_pin;
    return 0;
}

int app_lpm_add_wake_src(lpm_wake_src_cfg_t cfg)
{
    static uint8_t sleep_src_num = 0;
    static uint8_t standby_src_num = 0;
    uint8_t *src_num;
    lpm_pin_src_t *lpm_pin_srcs;

    if (cfg.lpm_policy == LPM_POLICY_DEEP_SLEEP) {
        src_num = &standby_src_num;
        lpm_pin_srcs = wake_srcs_standby;
    } else {
        src_num = &sleep_src_num;
        lpm_pin_srcs = wake_srcs_sleep;
    }

    if (*src_num >= MAX_LPM_WAKESRC_COUNT) {
        LOGW(TAG, "too many wakeup src");
        return -1;
    }

    lpm_pin_srcs[(*src_num)++] = cfg.pin_src;

    return 0;
}

void app_lpm_init(pwr_idle_check_cb_t idle_check_cb, lpm_event_cb_t lpm_enter_hook)
{
    CHECK_PARAM(idle_check_cb && lpm_enter_hook, );

    if (g_lpm_idle_check_time < 5) {
        g_lpm_idle_check_time = 5;
    }

    user_lpm_check_cb = idle_check_cb;
    pm_init(lpm_enter_hook);

    pm_config_mask(LPM_DEV_MASK_GENERAL_ADC);
    pm_config_mask(LPM_DEV_MASK_GENERAL_GPIO2);

    wifi_lpm_enable(1);

    /* 事件订阅 */
    event_subscribe(EVENT_UPDATE_STATUS, user_local_event_cb, NULL);
    event_subscribe(EVENT_NET_NTP_SUCCESS, user_local_event_cb, NULL);
    event_subscribe(EVENT_LPM_CHECK, user_local_event_cb, NULL);

    lpm_enable();

    event_publish_delay(EVENT_LPM_CHECK, NULL, 3000);
}

void app_lpm_sleep(pm_policy_t policy, int power_key_pressed)
{
    if (policy == LPM_POLICY_DEEP_SLEEP) {
        if (power_key_pressed) {
            g_pwr_key_pressed = 1;
            app_sys_set_boot_reason(BOOT_REASON_POWER_KEY);
        } else {
            g_pwr_key_pressed = 0;
            app_sys_set_boot_reason(BOOT_REASON_WAKE_STANDBY);
        }
    }

    pm_config_policy(policy);
    pm_agree_halt(0);
}

