/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "app_main.h"
#include "app_lpm.h"

#define TAG "BAT"

#ifdef CONFIG_BATTERY_ADC
static uint32_t battery_volt = 0;
static int g_pin_adc, g_pin_charge, g_check_interval_s;

static void battery_check_cb(int bat_volt)
{
    //LOGD(TAG, "battery check ****");

    if (bat_volt < BATTERY_VOLT_MIN) {
        LOGI(TAG, "***VOLTAGE TOO LOW!! SHUTDOWN! %lfV***\n", bat_volt / 1000.0);
        app_lpm_sleep(LPM_POLICY_DEEP_SLEEP, 0);
    } else if (bat_volt < BATTERY_VOLT_LOW) {
        LOGI(TAG,"***VOLTAGE LOW!! %lfV***\n", bat_volt / 1000.0);
    }
}

static uint32_t battery_voltage_read(void)
{
#if PIN_BATTERY_ADC >= 0
    int voltage;
    if (app_adc_value_read(g_pin_adc, &voltage)) {
        return -1;
    }

    /* mutil 1.39 depend on har hardware */
    return voltage * 1.39 * 3300 / 4096;
#else
    return 3800;
#endif
}

void battery_update_avg(void)
{
    static uint8_t read_cnt = 0;
    static uint32_t battery_vsum = 0;
    static uint32_t volts[BATTERY_AVG_ROUND];

    volts[read_cnt] = battery_voltage_read();
    battery_vsum += volts[read_cnt];

    if (++read_cnt == BATTERY_AVG_ROUND) {        
        uint32_t avg;
        avg = battery_vsum / read_cnt;

        for (int i = 0; i < BATTERY_AVG_ROUND; ++i) {
            if (abs(volts[i] - avg) > BATTERY_VOLT_DIFF) {
                //printf("volt diff big %u %u\n", volts[i], avg);

                battery_vsum -= volts[i];
                read_cnt--;

                if (read_cnt > 0) {
                    avg = battery_vsum / read_cnt;
                }
            }
        }

        if (read_cnt > 0) {
            battery_volt = avg;
            // printf("battery volt %lf\n", battery_volt / 1000.0);
        }

        read_cnt = 0;
        battery_vsum = 0;
    }
}

static void app_battery_check(uint32_t event_id, const void *param, void *context)
{
    switch (event_id) {
        case EVENT_BATTERY_CHECK:
            battery_check_cb(battery_volt);

            event_publish_delay(EVENT_BATTERY_CHECK, NULL, g_check_interval_s * 1000);
            break;
        default:;
    }
}

int app_battery_init(int pin_adc, int pin_pwr, int check_interval_s)
{
    if (check_interval_s < 10) {
        LOGE(TAG, "battery params error");
        return -1;
    }

    g_pin_adc = pin_adc;
    g_pin_charge = pin_pwr;
    g_check_interval_s = check_interval_s;

    /* check the battery level when power up, if too low stop booting*/
    for (int i = 0; i < BATTERY_AVG_ROUND; i++) {
        battery_update_avg();
        aos_msleep(2);
    }

    event_subscribe(EVENT_BATTERY_CHECK, app_battery_check, NULL);
    event_publish_delay(EVENT_BATTERY_CHECK, NULL, check_interval_s * 1000);

    return 0;
}

uint32_t app_battery_read(void)
{
    return battery_volt;
}

void app_charger_check(void)
{
    static uint8_t charge_cnt = 0;
    int charged;
    int ret;
    
    if (g_pin_charge < 0) {
        return;
    }

    ret = app_gpio_read(g_pin_charge, &charged);
    if (ret == 0 && !charged) {
        if (charge_cnt) {
            LOGD(TAG, "usb plugged in!!");
            app_lpm_sleep(LPM_POLICY_DEEP_SLEEP, 0);
            aos_msleep(3000);
        }
        charge_cnt = charge_cnt % 100 + 1;
    } else {
        charged = 0;
    }
}
#endif
